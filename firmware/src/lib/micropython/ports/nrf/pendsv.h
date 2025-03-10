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
#ifndef MICROPY_INCLUDED_NRF_PENDSV_H
#define MICROPY_INCLUDED_NRF_PENDSV_H

enum {
    MICROPY_BOARD_PENDSV_ENTRIES
    PENDSV_DISPATCH_MAX
};

#define PENDSV_DISPATCH_NUM_SLOTS PENDSV_DISPATCH_MAX

#define NVIC_PRIORITYGROUP_4    ((uint32_t)0x00000003)
#define IRQ_PRI_PENDSV          NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 15, 0)

typedef void (*pendsv_dispatch_t)(void);

void pendsv_init(void);
void pendsv_schedule_dispatch(size_t slot, pendsv_dispatch_t f);

#endif // MICROPY_INCLUDED_NRF_PENDSV_H
