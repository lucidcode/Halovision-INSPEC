/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdio.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/objint.h"
#include "extmod/modmachine.h"
#include "rfcore.h"
#include "portmodules.h"
#include "subghz.h"

#if MICROPY_PY_STM

#include "genhdr/modstm_mpz.h"

#if MICROPY_HW_XSPI_OTA
#include <string.h>
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "storage.h"
#include "xspi.h"

// Addresses are relative to the start of the XSPI flash. The bootloader below
// MICROPY_HW_XSPI_OTA_PROTECTED_END and the FAT filesystem must never be written.
#define OTA_XIP_BASE        (0x70000000)
#define OTA_FLASH_SIZE      (1 << (MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2 - 3))
#define OTA_SECTOR_SIZE     (MP_SPIFLASH_ERASE_BLOCK_SIZE)
#define OTA_FS_START        (MICROPY_HW_BDEV_SPIFLASH_OFFSET_BYTES)
#define OTA_FS_END          (MICROPY_HW_BDEV_SPIFLASH_OFFSET_BYTES + MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES)

// Raises ValueError unless [addr, addr + length) lies in writable flash.
static void stm_check_flash_range(uint32_t addr, uint32_t length) {
    if (addr < MICROPY_HW_XSPI_OTA_PROTECTED_END || length > OTA_FLASH_SIZE || addr > OTA_FLASH_SIZE - length
        || (addr < OTA_FS_END && addr + length > OTA_FS_START)) {
        mp_raise_ValueError(MP_ERROR_TEXT("flash range"));
    }
}

// stm.flash_erase(addr[, length])
// Erases length bytes (one sector by default) at the given flash-relative address. Both
// must be sector-aligned.
static mp_obj_t stm_flash_erase(size_t n_args, const mp_obj_t *args) {
    uint32_t addr = mp_obj_get_int(args[0]);
    uint32_t length = n_args > 1 ? mp_obj_get_int(args[1]) : OTA_SECTOR_SIZE;
    stm_check_flash_range(addr, length);
    if (addr % OTA_SECTOR_SIZE != 0 || length % OTA_SECTOR_SIZE != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("unaligned"));
    }
    if (spi_bdev_eraseblocks_raw(&spi_bdev, addr / OTA_SECTOR_SIZE, length) != 0) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stm_flash_erase_obj, 1, 2, stm_flash_erase);

// stm.flash_write(addr, data)
static mp_obj_t stm_flash_write(mp_obj_t addr_obj, mp_obj_t data_obj) {
    uint32_t addr = mp_obj_get_int(addr_obj);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);
    stm_check_flash_range(addr, bufinfo.len);
    if (spi_bdev_writeblocks_raw(&spi_bdev, bufinfo.buf, 0, addr, bufinfo.len) != 0) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(stm_flash_write_obj, stm_flash_write);

// stm.flash_read(addr, length)
// Reads through the memory map. Erases and writes invalidate the caches for the range
// they touch, so this always sees the current contents.
static mp_obj_t stm_flash_read(mp_obj_t addr_obj, mp_obj_t len_obj) {
    uint32_t addr = mp_obj_get_int(addr_obj);
    uint32_t length = mp_obj_get_int(len_obj);
    if (length > OTA_FLASH_SIZE || addr > OTA_FLASH_SIZE - length) {
        mp_raise_ValueError(MP_ERROR_TEXT("flash range"));
    }
    vstr_t vstr;
    vstr_init_len(&vstr, length);
    memcpy(vstr.buf, (const void *)(OTA_XIP_BASE + addr), length);
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_2(stm_flash_read_obj, stm_flash_read);

static uint32_t stm_ota_sector_buf[OTA_SECTOR_SIZE / 4];

// Copies length bytes from src_addr to dst_addr, then resets. Does not return.
// The firmware runs in place from the flash being overwritten, so this runs from ITCM
// with IRQs off and calls only the XSPI flash driver, which is also linked into ITCM
// (OMV_RAMFUNC_OBJS). Nothing in the firmware region may be touched after the first
// erase: the word copy is volatile so the compiler cannot turn it into a memcpy call,
// and the reset writes AIRCR directly instead of calling NVIC_SystemReset.
// Each erase or write leaves the XSPI memory-mapped again, so the staged source sector
// is read through the memory map into RAM before its destination is erased.
// A power cut part-way leaves no bootable firmware, but the bootloader is never
// touched, so DFU can still recover the board.
__attribute__((section(".ram_function"), noinline, noreturn))
static void stm_flash_copy_and_reset_impl(uint32_t src_addr, uint32_t dst_addr, uint32_t length) {
    __disable_irq();

    for (uint32_t offset = 0; offset < length; offset += OTA_SECTOR_SIZE) {
        uint32_t chunk = length - offset;
        if (chunk > OTA_SECTOR_SIZE) {
            chunk = OTA_SECTOR_SIZE;
        }
        const volatile uint32_t *src = (const volatile uint32_t *)(OTA_XIP_BASE + src_addr + offset);
        for (uint32_t i = 0; i < (chunk + 3) / 4; i++) {
            stm_ota_sector_buf[i] = src[i];
        }
        spi_bdev_eraseblocks_raw(&spi_bdev, (dst_addr + offset) / OTA_SECTOR_SIZE, OTA_SECTOR_SIZE);
        spi_bdev_writeblocks_raw(&spi_bdev, (const uint8_t *)stm_ota_sector_buf, 0, dst_addr + offset, chunk);
    }

    __DSB();
    SCB->AIRCR = (0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)
        | SCB_AIRCR_SYSRESETREQ_Msk;
    __DSB();
    while (1) {
    }
}

// stm.flash_copy_and_reset(src_addr, dst_addr, length)
// Copies staged firmware into the app region, then resets. Does not return.
static mp_obj_t stm_flash_copy_and_reset(mp_obj_t src_obj, mp_obj_t dst_obj, mp_obj_t len_obj) {
    uint32_t src_addr = mp_obj_get_int(src_obj);
    uint32_t dst_addr = mp_obj_get_int(dst_obj);
    uint32_t length = mp_obj_get_int(len_obj);

    stm_check_flash_range(src_addr, length);
    stm_check_flash_range(dst_addr, length);
    if (length == 0 || dst_addr % OTA_SECTOR_SIZE != 0 || src_addr % 4 != 0
        || (src_addr < dst_addr + length && dst_addr < src_addr + length)) {
        mp_raise_ValueError(MP_ERROR_TEXT("copy range"));
    }

    stm_flash_copy_and_reset_impl(src_addr, dst_addr, length);
}
static MP_DEFINE_CONST_FUN_OBJ_3(stm_flash_copy_and_reset_obj, stm_flash_copy_and_reset);
#endif // MICROPY_HW_XSPI_OTA

static const mp_rom_map_elem_t stm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_stm) },

    { MP_ROM_QSTR(MP_QSTR_mem8), MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16), MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32), MP_ROM_PTR(&machine_mem32_obj) },

    #if MICROPY_PY_STM_CONST
    #include "genhdr/modstm_const.h"
    #endif

    #if MICROPY_HW_XSPI_OTA
    { MP_ROM_QSTR(MP_QSTR_flash_erase), MP_ROM_PTR(&stm_flash_erase_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_write), MP_ROM_PTR(&stm_flash_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_read), MP_ROM_PTR(&stm_flash_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash_copy_and_reset), MP_ROM_PTR(&stm_flash_copy_and_reset_obj) },
    #endif

    #if defined(STM32WB)
    { MP_ROM_QSTR(MP_QSTR_rfcore_status), MP_ROM_PTR(&rfcore_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_rfcore_fw_version), MP_ROM_PTR(&rfcore_fw_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_rfcore_sys_hci), MP_ROM_PTR(&rfcore_sys_hci_obj) },
    #endif

    #if defined(STM32WL)
    { MP_ROM_QSTR(MP_QSTR_subghz_cs), MP_ROM_PTR(&subghz_cs_obj) },
    { MP_ROM_QSTR(MP_QSTR_subghz_irq), MP_ROM_PTR(&subghz_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_subghz_is_busy), MP_ROM_PTR(&subghz_is_busy_obj) },
    #endif
};

static MP_DEFINE_CONST_DICT(stm_module_globals, stm_module_globals_table);

const mp_obj_module_t stm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&stm_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_stm, stm_module);

#endif // MICROPY_PY_STM
