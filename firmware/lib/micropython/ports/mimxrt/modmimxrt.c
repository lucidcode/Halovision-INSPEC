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
#include BOARD_FLASH_OPS_HEADER_H

// mimxrt.flash_erase(addr)
// Erase a 4KB sector at the given flash-relative address.
static mp_obj_t mimxrt_flash_erase(mp_obj_t addr_obj) {
    uint32_t addr = mp_obj_get_int(addr_obj);
    status_t status = flash_erase_sector(addr);
    if (status != 0) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mimxrt_flash_erase_obj, mimxrt_flash_erase);

// mimxrt.flash_write(addr, data)
// Write bytes to flash at the given flash-relative address. Sector must be erased first.
static mp_obj_t mimxrt_flash_write(mp_obj_t addr_obj, mp_obj_t data_obj) {
    uint32_t addr = mp_obj_get_int(addr_obj);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);
    status_t status = flash_write_block(addr, bufinfo.buf, bufinfo.len);
    if (status != 0) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mimxrt_flash_write_obj, mimxrt_flash_write);

// mimxrt.flash_read(addr, length)
// Read bytes from flash at the given flash-relative address.
static mp_obj_t mimxrt_flash_read(mp_obj_t addr_obj, mp_obj_t len_obj) {
    uint32_t addr = mp_obj_get_int(addr_obj);
    uint32_t length = mp_obj_get_int(len_obj);
    vstr_t vstr;
    vstr_init_len(&vstr, length);
    flash_read_block(addr, (uint8_t *)vstr.buf, length);
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_2(mimxrt_flash_read_obj, mimxrt_flash_read);

// mimxrt.flash_copy_and_reset(src_addr, dst_addr, length)
// Copy data between flash regions entirely from RAM, then reset.
// Used to copy staged firmware from ROMFS to the app region.
// IRQs are disabled and DCache is off for the entire operation so that
// no code is fetched from the flash region being overwritten.
// This function does not return.
__attribute__((section(".ram_functions")))
static void flash_copy_and_reset_impl(uint32_t src_addr, uint32_t dst_addr, uint32_t length) {
    uint32_t sector_size = SECTOR_SIZE_BYTES;
    uint32_t page_size = PAGE_SIZE_BYTES;
    uint32_t offset = 0;

    // Keep IRQs disabled and DCache off for the entire copy.
    __disable_irq();
    SCB_DisableDCache();

    while (offset < length) {
        // Erase destination sector.
        flexspi_nor_flash_erase_sector(BOARD_FLEX_SPI, dst_addr + offset);

        // Write one sector in page-sized chunks.
        uint32_t remaining = length - offset;
        if (remaining > sector_size) {
            remaining = sector_size;
        }
        uint32_t written = 0;
        while (written < remaining) {
            uint32_t chunk = remaining - written;
            if (chunk > page_size) {
                chunk = page_size;
            }
            // Read from source via memory-mapped flash.
            const uint8_t *src = (const uint8_t *)(BOARD_FLEX_SPI_ADDR_BASE + src_addr + offset + written);
            flexspi_nor_flash_page_program(BOARD_FLEX_SPI, dst_addr + offset + written, (uint32_t *)src, chunk);
            written += chunk;
        }

        offset += sector_size;
    }

    NVIC_SystemReset();
    while (1) {}
}

static mp_obj_t mimxrt_flash_copy_and_reset(mp_obj_t src_obj, mp_obj_t dst_obj, mp_obj_t len_obj) {
    uint32_t src_addr = mp_obj_get_int(src_obj);
    uint32_t dst_addr = mp_obj_get_int(dst_obj);
    uint32_t length = mp_obj_get_int(len_obj);
    flash_copy_and_reset_impl(src_addr, dst_addr, length);
    // Never reached.
    return mp_const_none;
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
