/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#pragma once

#include "broadcom_peripherals_config.h"

#include "broadcom/gpio.h"

typedef enum {
    PIN_FUNCTION_NONE = 0,
    PIN_FUNCTION_I2C = 1,
    PIN_FUNCTION_SPI = 2,
    PIN_FUNCTION_UART = 3,
    PIN_FUNCTION_PWM = 4
} pin_function_type_t;

typedef enum {
    I2C_FUNCTION_SDA = 0,
    I2C_FUNCTION_SCL = 1
} pin_i2c_function_t;

typedef enum {
    SPI_FUNCTION_SCLK = 0,
    SPI_FUNCTION_MOSI = 1,
    SPI_FUNCTION_MISO = 2,
    SPI_FUNCTION_CE0 = 3,
    SPI_FUNCTION_CE1 = 4,
    SPI_FUNCTION_CE2 = 5
} pin_spi_function_t;

typedef enum {
    UART_FUNCTION_TXD = 0,
    UART_FUNCTION_RXD = 1,
    UART_FUNCTION_CTS = 2,
    UART_FUNCTION_RTS = 3
} pin_uart_function_t;


typedef enum {
    PWM_FUNCTION_CHANNEL1 = 0,
    PWM_FUNCTION_CHANNEL2 = 1
} pin_pwm_function_t;

#define NONE_FUNCTION_0

typedef struct {
    uint8_t type;
    uint8_t index;
    uint8_t function;
} pin_function_t;

typedef struct {
    PIN_PREFIX_FIELDS
    uint8_t number;
    pin_function_t functions[6];
} mcu_pin_obj_t;

#define NUM_ALT_FUNC (6)

bool pin_find_alt(const mcu_pin_obj_t* pin,
                  pin_function_type_t type,
                  uint8_t index,
                  uint8_t function,
                  BP_Function_Enum* alt);

#if BCM_VERSION == 2711
#define BCM_PIN_COUNT (58)
#else
#define BCM_PIN_COUNT (54)
#endif

extern const mcu_pin_obj_t pin_GPIO0;
extern const mcu_pin_obj_t pin_GPIO1;
extern const mcu_pin_obj_t pin_GPIO2;
extern const mcu_pin_obj_t pin_GPIO3;
extern const mcu_pin_obj_t pin_GPIO4;
extern const mcu_pin_obj_t pin_GPIO5;
extern const mcu_pin_obj_t pin_GPIO6;
extern const mcu_pin_obj_t pin_GPIO7;
extern const mcu_pin_obj_t pin_GPIO8;
extern const mcu_pin_obj_t pin_GPIO9;

extern const mcu_pin_obj_t pin_GPIO10;
extern const mcu_pin_obj_t pin_GPIO11;
extern const mcu_pin_obj_t pin_GPIO12;
extern const mcu_pin_obj_t pin_GPIO13;
extern const mcu_pin_obj_t pin_GPIO14;
extern const mcu_pin_obj_t pin_GPIO15;
extern const mcu_pin_obj_t pin_GPIO16;
extern const mcu_pin_obj_t pin_GPIO17;
extern const mcu_pin_obj_t pin_GPIO18;
extern const mcu_pin_obj_t pin_GPIO19;

extern const mcu_pin_obj_t pin_GPIO20;
extern const mcu_pin_obj_t pin_GPIO21;
extern const mcu_pin_obj_t pin_GPIO22;
extern const mcu_pin_obj_t pin_GPIO23;
extern const mcu_pin_obj_t pin_GPIO24;
extern const mcu_pin_obj_t pin_GPIO25;
extern const mcu_pin_obj_t pin_GPIO26;
extern const mcu_pin_obj_t pin_GPIO27;
extern const mcu_pin_obj_t pin_GPIO28;
extern const mcu_pin_obj_t pin_GPIO29;

extern const mcu_pin_obj_t pin_GPIO30;
extern const mcu_pin_obj_t pin_GPIO31;
extern const mcu_pin_obj_t pin_GPIO32;
extern const mcu_pin_obj_t pin_GPIO33;
extern const mcu_pin_obj_t pin_GPIO34;
extern const mcu_pin_obj_t pin_GPIO35;
extern const mcu_pin_obj_t pin_GPIO36;
extern const mcu_pin_obj_t pin_GPIO37;
extern const mcu_pin_obj_t pin_GPIO38;
extern const mcu_pin_obj_t pin_GPIO39;

extern const mcu_pin_obj_t pin_GPIO40;
extern const mcu_pin_obj_t pin_GPIO41;
extern const mcu_pin_obj_t pin_GPIO42;
extern const mcu_pin_obj_t pin_GPIO43;
extern const mcu_pin_obj_t pin_GPIO44;
extern const mcu_pin_obj_t pin_GPIO45;
extern const mcu_pin_obj_t pin_GPIO46;
extern const mcu_pin_obj_t pin_GPIO47;
extern const mcu_pin_obj_t pin_GPIO48;
extern const mcu_pin_obj_t pin_GPIO49;

extern const mcu_pin_obj_t pin_GPIO50;
extern const mcu_pin_obj_t pin_GPIO51;
extern const mcu_pin_obj_t pin_GPIO52;
extern const mcu_pin_obj_t pin_GPIO53;
#if BCM_VERSION == 2711
extern const mcu_pin_obj_t pin_GPIO54;
extern const mcu_pin_obj_t pin_GPIO55;
extern const mcu_pin_obj_t pin_GPIO56;
extern const mcu_pin_obj_t pin_GPIO57;
#endif
