/*
 * This file is part of the OpenMV project.
 * The MIT License (MIT)
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 */
#include <string.h>
#include "py/mphal.h"
#include "storage.h"
#include "sdram.h"
#if MICROPY_HW_USB_HS_ULPI3320
#include "ulpi.h"
#endif

void OPENMV4_H7_PRO_board_usb_enable(int enable);

void OPENMV4_H7_PRO_board_startup(void) {
}

void OPENMV4_H7_PRO_board_early_init(void) {
    HAL_InitTick(0);

    // Enable oscillator pin
    OPENMV4_H7_PRO_board_osc_enable(true);

    // Enable ULPI.
    OPENMV4_H7_PRO_board_usb_enable(true);

    #if MICROPY_HW_USB_HS_ULPI3320
    // Make sure UPLI is Not in low-power mode.
    ulpi_leave_low_power();
    #endif
}

void OPENMV4_H7_PRO_board_enter_bootloader(void) {
    NVIC_SystemReset();
}

void OPENMV4_H7_PRO_board_osc_enable(int enable) {
    mp_hal_pin_config(pyb_pin_OSCEN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_pin_config_speed(pyb_pin_OSCEN, MP_HAL_PIN_SPEED_LOW);
    mp_hal_pin_write(pyb_pin_OSCEN, enable);
}

void OPENMV4_H7_PRO_board_usb_enable(int enable) {
    mp_hal_pin_config(pyb_pin_USBEN, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_UP, 0);
    mp_hal_pin_config_speed(pyb_pin_USBEN, MP_HAL_PIN_SPEED_LOW);
    mp_hal_pin_write(pyb_pin_USBEN, enable);
    HAL_Delay(10);
}

void OPENMV4_H7_PRO_board_low_power(int mode) {
    switch (mode) {
        case 0:     // Leave stop mode.
            #if MICROPY_HW_USB_HS_ULPI3320
            ulpi_leave_low_power();
            #endif
            sdram_leave_low_power();
            break;
        case 1:     // Enter stop mode.
            #if MICROPY_HW_USB_HS_ULPI3320
            ulpi_enter_low_power();
            #endif
            sdram_enter_low_power();
            break;
        case 2:     // Enter standby mode.
            #if MICROPY_HW_USB_HS_ULPI3320
            ulpi_enter_low_power();
            #endif
            sdram_enter_power_down();
            break;
    }

    #if (MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE == 0)
    // Enable QSPI deepsleep for modes 1 and 2
    mp_spiflash_deepsleep(&spi_bdev.spiflash, (mode != 0));
    #endif
}
