/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2013-2024 OpenMV, LLC.
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
 *
 * Board configuration and pin definitions.
 */
/* *INDENT-OFF* */
#ifndef __OMV_BOOTCONFIG_H__
#define __OMV_BOOTCONFIG_H__
#include STM32_HAL_H
#include "port.h"

// Misc config.
#define OMV_BOOT_VID                (0x37C5)
#define OMV_BOOT_PID                (0x9202)
#define OMV_BOOT_DFU_TIMEOUT        (1500)
#define OMV_BOOT_LED_PIN            (0) // index in omv_boot_pins
#define OMV_BOOT_MAGIC_ADDR         (0x1000FFFCU)

// Flash config
#define OMV_BOOT_AXI_FLASH_ENABLE   (1)

// Boot I/O pins.
static const pin_t omv_boot_pins[] = {
  { .gpio = GPIOC, .pin = GPIO_PIN_2, .speed = GPIO_SPEED_LOW, .mode = GPIO_MODE_OUTPUT_PP, .pull = GPIO_PULLUP  }, // LED
  { .gpio = GPIOA, .pin = GPIO_PIN_9,  .speed = GPIO_SPEED_HIGH, .mode = GPIO_MODE_INPUT,  .pull = GPIO_NOPULL     }, // VBUS
  { .gpio = GPIOA, .pin = GPIO_PIN_11, .speed = GPIO_SPEED_HIGH, .mode = GPIO_MODE_AF_PP, .alt = GPIO_AF10_OTG_FS  }, // DM
  { .gpio = GPIOA, .pin = GPIO_PIN_12, .speed = GPIO_SPEED_HIGH, .mode = GPIO_MODE_AF_PP, .alt = GPIO_AF10_OTG_FS  }, // DP
};
#define OMV_BOOT_PINS_COUNT         (sizeof(omv_boot_pins) / sizeof(omv_boot_pins[0]))

// Boot partitions.
static const partition_t OMV_BOOT_DFU_PARTITIONS[] = {
  { .type = PTYPE_AXI_FLASH, .region =  -1, .rdonly = 1, .start = 0x08000000, .limit = 0x08008000, .attr = 0 }, // Boot
  { .type = PTYPE_AXI_FLASH, .region =  -1, .rdonly = 0, .start = 0x08008000, .limit = 0x08010000, .attr = 0 }, // FFS
  { .type = PTYPE_AXI_FLASH, .region =  -1, .rdonly = 0, .start = 0x08010000, .limit = 0x080E0000, .attr = 0 }, // Firmware
  { .type = PTYPE_AXI_FLASH, .region =  -1, .rdonly = 0, .start = 0x080E0000, .limit = 0x08100000, .attr = 0 }, // ROMFS
};
#define OMV_BOOT_DFU_PARTITIONS_COUNT   4 // Must be a literal
#define OMV_BOOT_DFU_PARTITIONS_STR    "BOOTLOADER", "FILESYSTEM", "FIRMWARE", "ROMFS0"
#endif //__OMV_BOOTCONFIG_H__
