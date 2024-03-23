#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "broadcom/gpio.h"

#include "broadcom/defines.h"

// Map the alt number to the actual value.
BP_Function_Enum FSEL_VALUES[6] = {
    GPIO_FUNCTION_ALT0,
    GPIO_FUNCTION_ALT1,
    GPIO_FUNCTION_ALT2,
    GPIO_FUNCTION_ALT3,
    GPIO_FUNCTION_ALT4,
    GPIO_FUNCTION_ALT5
};

BP_PULL_Enum gpio_get_pull(uint8_t pin_number) {
    volatile uint32_t* pupd = &GPIO->GPIO_PUP_PDN_CNTRL_REG0;
    return (pupd[pin_number / 16] >> ((pin_number % 16) * 2)) & 0x3;
}

void gpio_set_pull(uint8_t pin_number, BP_PULL_Enum pull) {
    volatile uint32_t* pupd = &GPIO->GPIO_PUP_PDN_CNTRL_REG0;
    uint32_t shift = (pin_number % 16) * 2;
    uint32_t mask = 0x3 << shift;
    uint32_t current = pupd[pin_number / 16];
    pupd[pin_number / 16] = (current & ~mask) | (pull & 0x3) << shift;
}

BP_Function_Enum gpio_get_function(uint8_t pin_number) {
    volatile uint32_t* fsel = &GPIO->GPFSEL0;
    size_t shift = (pin_number % 10) * 3;
    return (fsel[pin_number / 10] >> shift) & 0x7;
}

void gpio_set_function(uint8_t pin_number, BP_Function_Enum function) {
    volatile uint32_t* fsel = &GPIO->GPFSEL0;
    uint32_t start = fsel[pin_number / 10];
    size_t shift = (pin_number % 10) * 3;
    uint32_t mask = 0x7 << shift;
    uint32_t new = (start & ~mask) | ((function & 0x7) << shift);
    fsel[pin_number / 10] = new;
}

bool gpio_get_value(uint8_t pin_number) {
    volatile const uint32_t* lev = &GPIO->GPLEV0;
    return (lev[pin_number / 32] & (1 << (pin_number % 32))) != 0;
}

void gpio_set_value(uint8_t pin_number, bool value) {
    volatile uint32_t* lev;
    if (value) {
        lev = &GPIO->GPSET0;
    } else {
        lev = &GPIO->GPCLR0;
    }
    lev[pin_number / 32] = 1 << (pin_number % 32);  
}
