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
 * Common macros.
 */
#ifndef __OMV_COMMON_H__
#define __OMV_COMMON_H__

#define OMV_ATTR_ALIGNED(x, a)    x __attribute__((aligned(a)))
#define OMV_ATTR_SECTION(x, s)    x __attribute__((section(s)))
#define OMV_ATTR_ALWAYS_INLINE    inline __attribute__((always_inline))
#define OMV_ATTR_OPTIMIZE(o)      __attribute__((optimize(o)))
#define OMV_BREAK()               __asm__ volatile ("BKPT")

// Use 32-byte alignment on MCUs with no cache for DMA buffer alignment.
#ifndef __DCACHE_PRESENT
#define OMV_ALLOC_ALIGNMENT       (32)
#else
#define OMV_ALLOC_ALIGNMENT       (__SCB_DCACHE_LINE_SIZE)
#endif

#define OMV_ATTR_ALIGNED_DMA(x)   OMV_ATTR_ALIGNED(x, OMV_ALLOC_ALIGNMENT)

#ifdef OMV_DEBUG_PRINTF
#define debug_printf(fmt, ...) \
    do { printf("%s(): " fmt, __func__, ##__VA_ARGS__);} while (0)
#else
#define debug_printf(...)
#endif

#define OMV_MAX(a, b)             ({ \
        __typeof__ (a) _a = (a);     \
        __typeof__ (b) _b = (b);     \
        _a > _b ? _a : _b;           \
    })

#define OMV_MIN(a, b)             ({ \
        __typeof__ (a) _a = (a);     \
        __typeof__ (b) _b = (b);     \
        _a < _b ? _a : _b;           \
    })

#define OMV_ARRAY_SIZE(a)         (sizeof(a) / sizeof(a[0]))

#if OMV_PROFILE_ENABLE
#include <stdio.h>
#include "py/mphal.h"
#define OMV_CONCATENATE_DETAIL(x, y) x##y
#define OMV_CONCATENATE(x, y)   OMV_CONCATENATE_DETAIL(x, y)
#define OMV_PROFILE_START(F)    mp_uint_t OMV_CONCATENATE(_ticks_start_, F) = mp_hal_ticks_us()
#define OMV_PROFILE_PRINT(F)    printf("%s:%s %u us\n", __FUNCTION__, #F, mp_hal_ticks_us() - OMV_CONCATENATE(_ticks_start_, F))
#else
#define OMV_PROFILE_START(F)
#define OMV_PROFILE_PRINT(F)
#endif

#endif //__OMV_COMMON_H__
