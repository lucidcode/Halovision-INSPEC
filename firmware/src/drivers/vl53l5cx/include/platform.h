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
 * VL53L5CX platform implementation.
 */
#ifndef __VL53L5CX_PLATFORM_H__
#define __VL53L5CX_PLATFORM_H__
#include <stdint.h>
#include <string.h>
#include <omv_i2c.h>
#include "py/mphal.h"

// Driver configuration
#define VL53L5CX_NB_TARGET_PER_ZONE (1U)
#define VL53L5CX_DISABLE_AMBIENT_PER_SPAD
#define VL53L5CX_DISABLE_NB_SPADS_ENABLED
#define VL53L5CX_DISABLE_NB_TARGET_DETECTED
#define VL53L5CX_DISABLE_SIGNAL_PER_SPAD
#define VL53L5CX_DISABLE_RANGE_SIGMA_MM
#define VL53L5CX_DISABLE_REFLECTANCE_PERCENT
#define VL53L5CX_DISABLE_TARGET_STATUS
#define VL53L5CX_DISABLE_MOTION_INDICATOR

// Platform struct.
typedef struct {
    omv_i2c_t *bus;
    uint16_t address;
} VL53L5CX_Platform;

void vl53l5cx_reset(VL53L5CX_Platform *platform);
void vl53l5cx_shutdown(VL53L5CX_Platform *platform);
void vl53l5cx_swap(uint8_t *buf, uint16_t size);
uint8_t vl53l5cx_read(VL53L5CX_Platform *platform, uint16_t addr, uint8_t *buf, uint32_t size);
uint8_t vl53l5cx_write(VL53L5CX_Platform *platform, uint16_t addr, uint8_t *buf, uint32_t size);

// Platform API.
#define VL53L5CX_CHECK_STATUS(status) ({ \
        if (status != VL53L5CX_STATUS_OK) { \
            return -1; \
        } \
        status; \
    })
#define VL53L5CX_RdByte(platform, addr, buf) ({ \
        uint8_t status = vl53l5cx_read(platform, addr, buf, 1); \
        VL53L5CX_CHECK_STATUS(status); \
    })

#define VL53L5CX_WrByte(platform, addr, buf) ({ \
        uint8_t status = vl53l5cx_write(platform, addr, (uint8_t [1]){ buf }, 1); \
        VL53L5CX_CHECK_STATUS(status); \
    })

#define VL53L5CX_RdMulti(platform, addr, buf, size) ({ \
        uint8_t status = vl53l5cx_read(platform, addr, buf, size); \
        VL53L5CX_CHECK_STATUS(status); \
    })

#define VL53L5CX_WrMulti(platform, addr, buf, size) ({ \
        uint8_t status = vl53l5cx_write(platform, addr, buf, size); \
        VL53L5CX_CHECK_STATUS(status); \
    })
#define VL53L5CX_WaitMs(platform, ms) ({ mp_hal_delay_ms(ms); 0; })
#define VL53L5CX_SwapBuffer(buf, size) vl53l5cx_swap(buf, size)
#endif	// __VL53L5CX_PLATFORM_H__
