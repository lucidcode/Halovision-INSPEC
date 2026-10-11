/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mperrno.h"
#include "py/runtime.h"
#include "modmimxrt.h"
#include "flash.h"

// The bootloader occupies the first 256KB of flash and must never be written.
#define OTA_PROTECTED_END   (0x40000)
#define OTA_MAX_PAGE_SIZE   (256)

// Raises ValueError unless [addr, addr + length) lies in writable flash.
static void mimxrt_check_flash_range(uint32_t addr, uint32_t length) {
    uint32_t flash_size = qspiflash_config.memConfig.sflashA1Size;
    if (addr < OTA_PROTECTED_END || length > flash_size || addr > flash_size - length) {
        mp_raise_ValueError(MP_ERROR_TEXT("flash range"));
    }
}

// mimxrt.flash_erase(addr[, length])
// Erases length bytes (one sector by default) at the given flash-relative address. Both
// must be sector-aligned. Whole blocks inside the range use the faster block erase.
static mp_obj_t mimxrt_flash_erase(size_t n_args, const mp_obj_t *args) {
    uint32_t addr = mp_obj_get_int(args[0]);
    uint32_t length = n_args > 1 ? mp_obj_get_int(args[1]) : SECTOR_SIZE_BYTES;
    uint32_t sector_size = SECTOR_SIZE_BYTES;
    uint32_t block_size = BLOCK_SIZE_BYTES;
    mimxrt_check_flash_range(addr, length);
    if (addr % sector_size != 0 || length % sector_size != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("unaligned"));
    }
    for (uint32_t end = addr + length; addr < end;) {
        status_t status;
        if (block_size > sector_size && block_size % sector_size == 0
            && addr % block_size == 0 && end - addr >= block_size) {
            status = flash_erase_block(addr);
            addr += block_size;
        } else {
            status = flash_erase_sector(addr);
            addr += sector_size;
        }
        if (status != kStatus_Success) {
            mp_raise_OSError(MP_EIO);
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mimxrt_flash_erase_obj, 1, 2, mimxrt_flash_erase);

// mimxrt.flash_write(addr, data)
// Writes bytes at the given flash-relative address. The sector must be erased first.
static mp_obj_t mimxrt_flash_write(mp_obj_t addr_obj, mp_obj_t data_obj) {
    uint32_t addr = mp_obj_get_int(addr_obj);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);
    mimxrt_check_flash_range(addr, bufinfo.len);
    if (flash_write_block(addr, bufinfo.buf, bufinfo.len) != kStatus_Success) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mimxrt_flash_write_obj, mimxrt_flash_write);

// mimxrt.flash_read(addr, length)
// Reads bytes from the given flash-relative address.
static mp_obj_t mimxrt_flash_read(mp_obj_t addr_obj, mp_obj_t len_obj) {
    uint32_t addr = mp_obj_get_int(addr_obj);
    uint32_t length = mp_obj_get_int(len_obj);
    uint32_t flash_size = qspiflash_config.memConfig.sflashA1Size;
    if (length > flash_size || addr > flash_size - length) {
        mp_raise_ValueError(MP_ERROR_TEXT("flash range"));
    }
    vstr_t vstr;
    vstr_init_len(&vstr, length);
    flash_read_block(addr, (uint8_t *)vstr.buf, length);
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mimxrt_flash_read_obj, mimxrt_flash_read);

// Copies length bytes from src_addr to dst_addr, then resets. Does not return.
// Runs from RAM with IRQs and DCache off, because the app region being overwritten
// holds the code, constants and vector table of the running firmware. Nothing in
// flash may be touched after the first erase: the sizes arrive as arguments, the
// word copy is volatile so the compiler cannot turn it into a memcpy call, and the
// reset writes AIRCR directly instead of calling NVIC_SystemReset.
// Each page is copied into a RAM buffer before programming, because reading the
// memory-mapped source while an IP command is active on the same FlexSPI can stall.
// Whole blocks inside the copy are erased with the block erase, which takes about a
// third of the time of erasing their sectors one by one. This keeps the window in
// which a power cut leaves no bootable firmware short.
__attribute__((section(".ram_functions"), noinline, noreturn))
static void flash_copy_and_reset_impl(uint32_t src_addr, uint32_t dst_addr, uint32_t length,
    uint32_t sector_size, uint32_t block_size, uint32_t page_size) {
    uint32_t page_buf[OTA_MAX_PAGE_SIZE / 4];

    __disable_irq();
    SCB_DisableDCache();

    for (uint32_t offset = 0; offset < length;) {
        uint32_t erase_size = sector_size;
        if (block_size > sector_size && (dst_addr + offset) % block_size == 0
            && length - offset >= block_size) {
            erase_size = block_size;
            flexspi_nor_flash_erase_block(BOARD_FLEX_SPI, dst_addr + offset);
        } else {
            flexspi_nor_flash_erase_sector(BOARD_FLEX_SPI, dst_addr + offset);
        }

        uint32_t erase_end = offset + erase_size;
        if (erase_end > length) {
            erase_end = length;
        }
        for (uint32_t page = offset; page < erase_end; page += page_size) {
            uint32_t chunk = erase_end - page;
            if (chunk > page_size) {
                chunk = page_size;
            }
            // Round up to whole words. The staging sectors were erased, so the tail is 0xFF.
            uint32_t words = (chunk + 3) / 4;
            const volatile uint32_t *src =
                (const volatile uint32_t *)(BOARD_FLEX_SPI_ADDR_BASE + src_addr + page);
            for (uint32_t i = 0; i < words; i++) {
                page_buf[i] = src[i];
            }
            flexspi_nor_flash_page_program(BOARD_FLEX_SPI, dst_addr + page, page_buf, words * 4);
        }
        offset += erase_size;
    }

    __DSB();
    SCB->AIRCR = (0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)
        | SCB_AIRCR_SYSRESETREQ_Msk;
    __DSB();
    while (1) {
    }
}

// mimxrt.flash_copy_and_reset(src_addr, dst_addr, length)
// Copies staged firmware into the app region, then resets. Does not return.
static mp_obj_t mimxrt_flash_copy_and_reset(mp_obj_t src_obj, mp_obj_t dst_obj, mp_obj_t len_obj) {
    uint32_t src_addr = mp_obj_get_int(src_obj);
    uint32_t dst_addr = mp_obj_get_int(dst_obj);
    uint32_t length = mp_obj_get_int(len_obj);
    uint32_t sector_size = SECTOR_SIZE_BYTES;
    uint32_t block_size = BLOCK_SIZE_BYTES;
    uint32_t page_size = PAGE_SIZE_BYTES;
    if (block_size % sector_size != 0) {
        // Fall back to sector erases.
        block_size = sector_size;
    }

    mimxrt_check_flash_range(src_addr, length);
    mimxrt_check_flash_range(dst_addr, length);
    if (length == 0 || page_size > OTA_MAX_PAGE_SIZE || dst_addr % sector_size != 0
        || src_addr % 4 != 0 || (src_addr < dst_addr + length && dst_addr < src_addr + length)) {
        mp_raise_ValueError(MP_ERROR_TEXT("copy range"));
    }

    flash_copy_and_reset_impl(src_addr, dst_addr, length, sector_size, block_size, page_size);
}
static MP_DEFINE_CONST_FUN_OBJ_3(mimxrt_flash_copy_and_reset_obj, mimxrt_flash_copy_and_reset);

static const mp_rom_map_elem_t mimxrt_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_mimxrt) },
    { MP_ROM_QSTR(MP_QSTR_Flash),               MP_ROM_PTR(&mimxrt_flash_type) },
    { MP_ROM_QSTR(MP_QSTR_flash_erase),         MP_ROM_PTR(&mimxrt_flash_erase_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_write),         MP_ROM_PTR(&mimxrt_flash_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_read),          MP_ROM_PTR(&mimxrt_flash_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_copy_and_reset), MP_ROM_PTR(&mimxrt_flash_copy_and_reset_obj) },
    #if MICROPY_HW_USB_MSC
    { MP_ROM_QSTR(MP_QSTR_MSC),                 MP_ROM_TRUE },
    #endif
};
static MP_DEFINE_CONST_DICT(mimxrt_module_globals, mimxrt_module_globals_table);

const mp_obj_module_t mp_module_mimxrt = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mimxrt_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mimxrt, mp_module_mimxrt);
