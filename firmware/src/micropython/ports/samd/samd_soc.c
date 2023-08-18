/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * This file initialises the USB (tinyUSB) and USART (SERCOM). Board USART settings
 * are set in 'boards/<board>/mpconfigboard.h.
 *
 * IMPORTANT: Please refer to "I/O Multiplexing and Considerations" chapters
 *            in device datasheets for I/O Pin functions and assignments.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2022 Robert Hammelrath
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

#include "py/runtime.h"
#include "modmachine.h"
#include "samd_soc.h"
#include "sam.h"
#include "tusb.h"
#include "mphalport.h"

#if MICROPY_PY_MACHINE_RTC
extern void machine_rtc_start(bool force);
#endif

static void usb_init(void) {
    // Init USB clock
    #if defined(MCU_SAMD21)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_ID_USB;
    PM->AHBMASK.bit.USB_ = 1;
    PM->APBBMASK.bit.USB_ = 1;
    uint8_t alt = 6; // alt G, USB
    #elif defined(MCU_SAMD51)
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK5;
    while (GCLK->PCHCTRL[USB_GCLK_ID].bit.CHEN == 0) {
    }
    MCLK->AHBMASK.bit.USB_ = 1;
    MCLK->APBBMASK.bit.USB_ = 1;
    uint8_t alt = 7; // alt H, USB
    #endif

    // Init USB pins
    PORT->Group[0].DIRSET.reg = 1 << 25 | 1 << 24;
    PORT->Group[0].OUTCLR.reg = 1 << 25 | 1 << 24;
    PORT->Group[0].PMUX[12].reg = alt << 4 | alt;
    PORT->Group[0].PINCFG[24].reg = PORT_PINCFG_PMUXEN;
    PORT->Group[0].PINCFG[25].reg = PORT_PINCFG_PMUXEN;

    tusb_init();
}

// Initialize the µs counter on TC 0/1 or TC4/5
void init_us_counter(void) {
    #if defined(MCU_SAMD21)

    PM->APBCMASK.bit.TC3_ = 1; // Enable TC3 clock
    PM->APBCMASK.bit.TC4_ = 1; // Enable TC4 clock
    // Select multiplexer generic clock source and enable.
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK3 | GCLK_CLKCTRL_ID_TC4_TC5;
    // Wait while it updates synchronously.
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }

    // configure the timer
    TC4->COUNT32.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT32_Val;
    TC4->COUNT32.CTRLA.bit.RUNSTDBY = 1;
    TC4->COUNT32.CTRLA.bit.ENABLE = 1;
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY) {
    }
    TC4->COUNT32.READREQ.reg = TC_READREQ_RREQ | TC_READREQ_RCONT | 0x10;
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY) {
    }
    // Enable the IRQ
    TC4->COUNT32.INTENSET.reg = TC_INTENSET_OVF;
    NVIC_EnableIRQ(TC4_IRQn);

    #elif defined(MCU_SAMD51)

    MCLK->APBAMASK.bit.TC0_ = 1; // Enable TC0 clock
    MCLK->APBAMASK.bit.TC1_ = 1; // Enable TC1 clock
    // Peripheral channel 9 is driven by GCLK3, 8 MHz.
    GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK3 | GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[TC0_GCLK_ID].bit.CHEN == 0) {
    }

    // configure the timer
    TC0->COUNT32.CTRLA.bit.PRESCALER = 0;
    TC0->COUNT32.CTRLA.bit.MODE = TC_CTRLA_MODE_COUNT32_Val;
    TC0->COUNT32.CTRLA.bit.RUNSTDBY = 1;
    TC0->COUNT32.CTRLA.bit.ENABLE = 1;
    while (TC0->COUNT32.SYNCBUSY.bit.ENABLE) {
    }

    // Enable the IRQ
    TC0->COUNT32.INTENSET.reg = TC_INTENSET_OVF;
    NVIC_EnableIRQ(TC0_IRQn);
    #endif
}

void samd_init(void) {
    init_clocks(get_cpu_freq());
    init_us_counter();
    usb_init();
    check_usb_recovery_mode();
    #if defined(MCU_SAMD51)
    mp_hal_ticks_cpu_enable();
    #endif
    #if MICROPY_PY_MACHINE_RTC
    machine_rtc_start(false);
    #endif
}
