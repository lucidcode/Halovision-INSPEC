#include "broadcom/pins.h"

bool pin_find_alt(const mcu_pin_obj_t* pin,
                  pin_function_type_t type,
                  uint8_t index,
                  uint8_t function,
                  BP_Function_Enum* alt) {
    for (size_t i = 0; i < NUM_ALT_FUNC; i++) {
        const pin_function_t* f = &pin->functions[i];
        if (f->type == type &&
            f->index == index &&
            f->function == function) {
            *alt = FSEL_VALUES[i];
            return true;
        }
    }
    return false;
}

#if BCM_VERSION != 2711

const mcu_pin_obj_t pin_GPIO0 = {
    PIN_PREFIX_VALUES
    .number = 0,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO1 = {
    PIN_PREFIX_VALUES
    .number = 1,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO2 = {
    PIN_PREFIX_VALUES
    .number = 2,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 1, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO3 = {
    PIN_PREFIX_VALUES
    .number = 3,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 1, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO4 = {
    PIN_PREFIX_VALUES
    .number = 4,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO5 = {
    PIN_PREFIX_VALUES
    .number = 5,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO6 = {
    PIN_PREFIX_VALUES
    .number = 6,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO7 = {
    PIN_PREFIX_VALUES
    .number = 7,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_CE1 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO8 = {
    PIN_PREFIX_VALUES
    .number = 8,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO9 = {
    PIN_PREFIX_VALUES
    .number = 9,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO10 = {
    PIN_PREFIX_VALUES
    .number = 10,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO11 = {
    PIN_PREFIX_VALUES
    .number = 11,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO12 = {
    PIN_PREFIX_VALUES
    .number = 12,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL1 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO13 = {
    PIN_PREFIX_VALUES
    .number = 13,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL2 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO14 = {
    PIN_PREFIX_VALUES
    .number = 14,
    .functions = {
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_TXD },
    }
};

const mcu_pin_obj_t pin_GPIO15 = {
    PIN_PREFIX_VALUES
    .number = 15,
    .functions = {
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RXD },
    }
};

const mcu_pin_obj_t pin_GPIO16 = {
    PIN_PREFIX_VALUES
    .number = 16,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_CE2 },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_CTS },
    }
};

const mcu_pin_obj_t pin_GPIO17 = {
    PIN_PREFIX_VALUES
    .number = 17,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_CE1 },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RTS },
    }
};

const mcu_pin_obj_t pin_GPIO18 = {
    PIN_PREFIX_VALUES
    .number = 18,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL1 },
    }
};

const mcu_pin_obj_t pin_GPIO19 = {
    PIN_PREFIX_VALUES
    .number = 19,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL2 },
    }
};

const mcu_pin_obj_t pin_GPIO20 = {
    PIN_PREFIX_VALUES
    .number = 20,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO21 = {
    PIN_PREFIX_VALUES
    .number = 21,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO22 = {
    PIN_PREFIX_VALUES
    .number = 22,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO23 = {
    PIN_PREFIX_VALUES
    .number = 23,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO24 = {
    PIN_PREFIX_VALUES
    .number = 24,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO25 = {
    PIN_PREFIX_VALUES
    .number = 25,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO26 = {
    PIN_PREFIX_VALUES
    .number = 26,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO27 = {
    PIN_PREFIX_VALUES
    .number = 27,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO28 = {
    PIN_PREFIX_VALUES
    .number = 28,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO29 = {
    PIN_PREFIX_VALUES
    .number = 29,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO30 = {
    PIN_PREFIX_VALUES
    .number = 30,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_CTS },
    }
};

const mcu_pin_obj_t pin_GPIO31 = {
    PIN_PREFIX_VALUES
    .number = 31,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RTS },
    }
};

const mcu_pin_obj_t pin_GPIO32 = {
    PIN_PREFIX_VALUES
    .number = 32,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_TXD },
    }
};

const mcu_pin_obj_t pin_GPIO33 = {
    PIN_PREFIX_VALUES
    .number = 33,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RXD },
    }
};

const mcu_pin_obj_t pin_GPIO34 = {
    PIN_PREFIX_VALUES
    .number = 34,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO35 = {
    PIN_PREFIX_VALUES
    .number = 35,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_CE1 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO36 = {
    PIN_PREFIX_VALUES
    .number = 36,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO37 = {
    PIN_PREFIX_VALUES
    .number = 37,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO38 = {
    PIN_PREFIX_VALUES
    .number = 38,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO39 = {
    PIN_PREFIX_VALUES
    .number = 39,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO40 = {
    PIN_PREFIX_VALUES
    .number = 40,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL1 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_TXD },
    }
};

const mcu_pin_obj_t pin_GPIO41 = {
    PIN_PREFIX_VALUES
    .number = 41,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL2 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RXD },
    }
};

const mcu_pin_obj_t pin_GPIO42 = {
    PIN_PREFIX_VALUES
    .number = 42,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_CTS },
    }
};

const mcu_pin_obj_t pin_GPIO43 = {
    PIN_PREFIX_VALUES
    .number = 43,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RTS },
    }
};

const mcu_pin_obj_t pin_GPIO44 = {
    PIN_PREFIX_VALUES
    .number = 44,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_I2C, .index = 1, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO45 = {
    PIN_PREFIX_VALUES
    .number = 45,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL2 },
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_I2C, .index = 1, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO46 = {
    PIN_PREFIX_VALUES
    .number = 46,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO47 = {
    PIN_PREFIX_VALUES
    .number = 47,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO48 = {
    PIN_PREFIX_VALUES
    .number = 48,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO49 = {
    PIN_PREFIX_VALUES
    .number = 49,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO50 = {
    PIN_PREFIX_VALUES
    .number = 50,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO51 = {
    PIN_PREFIX_VALUES
    .number = 51,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO52 = {
    PIN_PREFIX_VALUES
    .number = 52,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO53 = {
    PIN_PREFIX_VALUES
    .number = 53,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

#else

const mcu_pin_obj_t pin_GPIO0 = {
    PIN_PREFIX_VALUES
    .number = 0,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 3, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_UART, .index = 2, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_I2C, .index = 6, .function = I2C_FUNCTION_SDA },
    }
};

const mcu_pin_obj_t pin_GPIO1 = {
    PIN_PREFIX_VALUES
    .number = 1,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 3, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_UART, .index = 2, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_I2C, .index = 6, .function = I2C_FUNCTION_SCL },
    }
};

const mcu_pin_obj_t pin_GPIO2 = {
    PIN_PREFIX_VALUES
    .number = 2,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 1, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 3, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_UART, .index = 2, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_I2C, .index = 3, .function = I2C_FUNCTION_SDA },
    }
};

const mcu_pin_obj_t pin_GPIO3 = {
    PIN_PREFIX_VALUES
    .number = 3,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 1, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 3, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_UART, .index = 2, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_I2C, .index = 3, .function = I2C_FUNCTION_SCL },
    }
};

const mcu_pin_obj_t pin_GPIO4 = {
    PIN_PREFIX_VALUES
    .number = 4,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 4, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_UART, .index = 3, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_I2C, .index = 3, .function = I2C_FUNCTION_SDA },
    }
};

const mcu_pin_obj_t pin_GPIO5 = {
    PIN_PREFIX_VALUES
    .number = 5,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 4, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_UART, .index = 3, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_I2C, .index = 3, .function = I2C_FUNCTION_SCL },
    }
};

const mcu_pin_obj_t pin_GPIO6 = {
    PIN_PREFIX_VALUES
    .number = 6,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 4, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_UART, .index = 3, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_I2C, .index = 4, .function = I2C_FUNCTION_SDA },
    }
};

const mcu_pin_obj_t pin_GPIO7 = {
    PIN_PREFIX_VALUES
    .number = 7,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_CE1 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 4, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_UART, .index = 3, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_I2C, .index = 4, .function = I2C_FUNCTION_SCL },
    }
};

const mcu_pin_obj_t pin_GPIO8 = {
    PIN_PREFIX_VALUES
    .number = 8,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 4, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_I2C, .index = 4, .function = I2C_FUNCTION_SDA },
    }
};

const mcu_pin_obj_t pin_GPIO9 = {
    PIN_PREFIX_VALUES
    .number = 9,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 4, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_I2C, .index = 4, .function = I2C_FUNCTION_SCL },
    }
};

const mcu_pin_obj_t pin_GPIO10 = {
    PIN_PREFIX_VALUES
    .number = 10,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 4, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_I2C, .index = 5, .function = I2C_FUNCTION_SDA },
    }
};

const mcu_pin_obj_t pin_GPIO11 = {
    PIN_PREFIX_VALUES
    .number = 11,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 4, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_I2C, .index = 5, .function = I2C_FUNCTION_SCL },
    }
};

const mcu_pin_obj_t pin_GPIO12 = {
    PIN_PREFIX_VALUES
    .number = 12,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL1 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 5, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_UART, .index = 5, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_I2C, .index = 5, .function = I2C_FUNCTION_SDA },
    }
};

const mcu_pin_obj_t pin_GPIO13 = {
    PIN_PREFIX_VALUES
    .number = 13,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL2 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 5, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_UART, .index = 5, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_I2C, .index = 5, .function = I2C_FUNCTION_SCL },
    }
};

const mcu_pin_obj_t pin_GPIO14 = {
    PIN_PREFIX_VALUES
    .number = 14,
    .functions = {
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 5, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_UART, .index = 5, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_TXD },
    }
};

const mcu_pin_obj_t pin_GPIO15 = {
    PIN_PREFIX_VALUES
    .number = 15,
    .functions = {
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 5, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_UART, .index = 5, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RXD },
    }
};

const mcu_pin_obj_t pin_GPIO16 = {
    PIN_PREFIX_VALUES
    .number = 16,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_CE2 },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_CTS },
    }
};

const mcu_pin_obj_t pin_GPIO17 = {
    PIN_PREFIX_VALUES
    .number = 17,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_CE1 },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RTS },
    }
};

const mcu_pin_obj_t pin_GPIO18 = {
    PIN_PREFIX_VALUES
    .number = 18,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 6, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL1 },
    }
};

const mcu_pin_obj_t pin_GPIO19 = {
    PIN_PREFIX_VALUES
    .number = 19,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 6, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL2 },
    }
};

const mcu_pin_obj_t pin_GPIO20 = {
    PIN_PREFIX_VALUES
    .number = 20,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 6, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO21 = {
    PIN_PREFIX_VALUES
    .number = 21,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 6, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_SPI, .index = 1, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO22 = {
    PIN_PREFIX_VALUES
    .number = 22,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_I2C, .index = 6, .function = I2C_FUNCTION_SDA },
    }
};

const mcu_pin_obj_t pin_GPIO23 = {
    PIN_PREFIX_VALUES
    .number = 23,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_I2C, .index = 6, .function = I2C_FUNCTION_SCL },
    }
};

const mcu_pin_obj_t pin_GPIO24 = {
    PIN_PREFIX_VALUES
    .number = 24,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 3, .function = SPI_FUNCTION_CE1 },
    }
};

const mcu_pin_obj_t pin_GPIO25 = {
    PIN_PREFIX_VALUES
    .number = 25,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 4, .function = SPI_FUNCTION_CE1 },
    }
};

const mcu_pin_obj_t pin_GPIO26 = {
    PIN_PREFIX_VALUES
    .number = 26,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 5, .function = SPI_FUNCTION_CE1 },
    }
};

const mcu_pin_obj_t pin_GPIO27 = {
    PIN_PREFIX_VALUES
    .number = 27,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_SPI, .index = 6, .function = SPI_FUNCTION_CE1 },
    }
};

const mcu_pin_obj_t pin_GPIO28 = {
    PIN_PREFIX_VALUES
    .number = 28,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO29 = {
    PIN_PREFIX_VALUES
    .number = 29,
    .functions = {
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO30 = {
    PIN_PREFIX_VALUES
    .number = 30,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_CTS },
    }
};

const mcu_pin_obj_t pin_GPIO31 = {
    PIN_PREFIX_VALUES
    .number = 31,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RTS },
    }
};

const mcu_pin_obj_t pin_GPIO32 = {
    PIN_PREFIX_VALUES
    .number = 32,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_TXD },
    }
};

const mcu_pin_obj_t pin_GPIO33 = {
    PIN_PREFIX_VALUES
    .number = 33,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RXD },
    }
};

const mcu_pin_obj_t pin_GPIO34 = {
    PIN_PREFIX_VALUES
    .number = 34,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO35 = {
    PIN_PREFIX_VALUES
    .number = 35,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_CE1 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO36 = {
    PIN_PREFIX_VALUES
    .number = 36,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_CE0 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_TXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO37 = {
    PIN_PREFIX_VALUES
    .number = 37,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_MISO },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RXD },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO38 = {
    PIN_PREFIX_VALUES
    .number = 38,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_MOSI },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_CTS },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO39 = {
    PIN_PREFIX_VALUES
    .number = 39,
    .functions = {
        { .type = PIN_FUNCTION_SPI, .index = 0, .function = SPI_FUNCTION_SCLK },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 0, .function = UART_FUNCTION_RTS },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO40 = {
    PIN_PREFIX_VALUES
    .number = 40,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 1, .function = PWM_FUNCTION_CHANNEL1 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_TXD },
    }
};

const mcu_pin_obj_t pin_GPIO41 = {
    PIN_PREFIX_VALUES
    .number = 41,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 1, .function = PWM_FUNCTION_CHANNEL2 },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RXD },
    }
};

const mcu_pin_obj_t pin_GPIO42 = {
    PIN_PREFIX_VALUES
    .number = 42,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_CTS },
    }
};

const mcu_pin_obj_t pin_GPIO43 = {
    PIN_PREFIX_VALUES
    .number = 43,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_UART, .index = 1, .function = UART_FUNCTION_RTS },
    }
};

const mcu_pin_obj_t pin_GPIO44 = {
    PIN_PREFIX_VALUES
    .number = 44,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_I2C, .index = 1, .function = I2C_FUNCTION_SDA },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO45 = {
    PIN_PREFIX_VALUES
    .number = 45,
    .functions = {
        { .type = PIN_FUNCTION_PWM, .index = 0, .function = PWM_FUNCTION_CHANNEL2 },
        { .type = PIN_FUNCTION_I2C, .index = 0, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_I2C, .index = 1, .function = I2C_FUNCTION_SCL },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO46 = {
    PIN_PREFIX_VALUES
    .number = 46,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO47 = {
    PIN_PREFIX_VALUES
    .number = 47,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO48 = {
    PIN_PREFIX_VALUES
    .number = 48,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO49 = {
    PIN_PREFIX_VALUES
    .number = 49,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO50 = {
    PIN_PREFIX_VALUES
    .number = 50,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO51 = {
    PIN_PREFIX_VALUES
    .number = 51,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO52 = {
    PIN_PREFIX_VALUES
    .number = 52,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO53 = {
    PIN_PREFIX_VALUES
    .number = 53,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO54 = {
    PIN_PREFIX_VALUES
    .number = 54,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO55 = {
    PIN_PREFIX_VALUES
    .number = 55,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO56 = {
    PIN_PREFIX_VALUES
    .number = 56,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

const mcu_pin_obj_t pin_GPIO57 = {
    PIN_PREFIX_VALUES
    .number = 57,
    .functions = {
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
        { .type = PIN_FUNCTION_NONE },
    }
};

#endif