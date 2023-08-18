#pragma once

#include "broadcom/defines.h"

typedef enum {
    GPIO_FUNCTION_INPUT  = 0,
    GPIO_FUNCTION_OUTPUT = 1,
    GPIO_FUNCTION_ALT5   = 2,
    GPIO_FUNCTION_ALT4   = 3,
    GPIO_FUNCTION_ALT0   = 4,
    GPIO_FUNCTION_ALT1   = 5,
    GPIO_FUNCTION_ALT2   = 6,
    GPIO_FUNCTION_ALT3   = 7,
} BP_Function_Enum;

// Map the alt number to the actual value.
extern BP_Function_Enum FSEL_VALUES[6];

BP_Function_Enum gpio_get_function(uint8_t pin_number);
void gpio_set_function(uint8_t pin_number, BP_Function_Enum function);
BP_PULL_Enum gpio_get_pull(uint8_t pin_number);
void gpio_set_pull(uint8_t pin_number, BP_PULL_Enum pull);
BP_PULL_Enum gpio_get_pull(uint8_t pin_number);

bool gpio_get_value(uint8_t pin_number);
void gpio_set_value(uint8_t pin_number, bool value);
