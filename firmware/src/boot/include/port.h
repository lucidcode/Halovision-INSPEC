/*
 * Copyright (C) 2023-2024 OpenMV, LLC.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Any redistribution, use, or modification in source or binary form
 *    is done solely for personal benefit and not for any commercial
 *    purpose or for monetary gain. For commercial licensing options,
 *    please contact openmv@openmv.io
 *
 * THIS SOFTWARE IS PROVIDED BY THE LICENSOR AND COPYRIGHT OWNER "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE LICENSOR OR COPYRIGHT
 * OWNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Bootloader common types and functions for ports to implement.
 */
#ifndef __OMV_BOOT_PORT_H__
#define __OMV_BOOT_PORT_H__
#include <stdint.h>
#include <stdbool.h>

#include "tusb.h"
#include "tusb_config.h"
#include "class/dfu/dfu.h"
#include "class/dfu/dfu_device.h"

#ifndef OMV_BOOT_MAGIC_VALUE
#define OMV_BOOT_MAGIC_VALUE  (0xB00710AD)
#endif

typedef struct pin {
    void *gpio;
    uint16_t port;
    uint16_t pin;
    uint16_t alt;
    uint16_t pad;
    uint16_t mode;
    uint16_t pull;
    uint16_t speed;
} pin_t;

typedef enum {
    PIN_MODE_INPUT,
    PIN_MODE_OUTPUT
} pin_mode_t;

// DFU partition entry.
typedef struct {
    uint32_t type;
    int32_t region;
    uint32_t rdonly;
    uintptr_t start;
    uintptr_t limit;
    uint32_t attr;
} partition_t;

typedef enum {
    PTYPE_AXI_FLASH,
    PTYPE_SPI_FLASH,
    PTYPE_XIP_FLASH,
    PTYPE_REC_FLASH,
} partition_type_t;

// Common MPU memory attributes.
typedef enum {
    MEMATTR_DEVICE_nGnRE,
    MEMATTR_NORMAL_RA,
    MEMATTR_NORMAL_WB_RA_WA,
    MEMATTR_NORMAL_NT_RA,
    MEMATTR_NORMAL_NCACHE,
    MEMATTR_COUNT
} mpu_mem_attr_t;

int port_init(void);
int port_deinit(void);
int port_get_uid(uint8_t *buf);
uint32_t port_ticks_ms();
void port_delay_ms(uint32_t ms);
void port_pin_mode(uint32_t pin, uint32_t mode);
uint32_t port_pin_read(uint32_t pin);
void port_pin_write(uint32_t pin, uint32_t state);
void port_led_blink(uint32_t interval_ms);
void port_mpu_init(void);
void port_mpu_deinit(void);
void port_mpu_load_defaults();
void port_mpu_config(const partition_t *p, bool ro, bool np, bool xn);
#endif  // __OMV_BOOT_PORT_H__
