/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2023 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2023 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * GPIO port for stm32.
 */
#include STM32_HAL_H
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "py/mphal.h"
#include "irq.h"

#include "omv_boardconfig.h"
#include "omv_gpio.h"

static omv_gpio_irq_descr_t gpio_irq_descr_table[16] = {{0}};

// Returns the port number configured in EXTI_CR for this line.
static uint32_t omv_gpio_irq_get_gpio(uint32_t line) {
    return (SYSCFG->EXTICR[line >> 2] >> (4 * (line & 3))) & 0x0F;
}

// Returns pin number.
static uint32_t omv_gpio_irq_get_line(omv_gpio_t pin) {
    return 31 - __CLZ(pin->pin);
}

// Returns the IRQ number of a an EXTI line.
static uint32_t omv_gpio_irq_get_irqn(omv_gpio_t pin) {
    uint32_t exti_irqn = 0;
    uint32_t exti_line = omv_gpio_irq_get_line(pin);

    switch (exti_line) {
        case 0:
            exti_irqn = EXTI0_IRQn;
            break;
        case 1:
            exti_irqn = EXTI1_IRQn;
            break;
        case 2:
            exti_irqn = EXTI2_IRQn;
            break;
        case 3:
            exti_irqn = EXTI3_IRQn;
            break;
        case 4:
            exti_irqn = EXTI4_IRQn;
            break;
        case 5 ... 9:
            exti_irqn = EXTI9_5_IRQn;
            break;
        case 10 ... 15:
            exti_irqn = EXTI15_10_IRQn;
            break;
        default:
            break;
    }
    return exti_irqn;
}

static inline bool omv_gpio_irq_enabled(omv_gpio_irq_descr_t *gpio_irq_descr, uint32_t line) {
    return (gpio_irq_descr->enabled &&
            gpio_irq_descr->callback &&
            gpio_irq_descr->gpio == omv_gpio_irq_get_gpio(line));

}

void omv_gpio_irq_handler(uint32_t line) {
    if (line < 5) {
        // EXTI lines 0 to 4 have single IRQs.
        omv_gpio_irq_descr_t *gpio_irq_descr = &gpio_irq_descr_table[line];
        if (omv_gpio_irq_enabled(gpio_irq_descr, line)) {
            __HAL_GPIO_EXTI_CLEAR_FLAG(1 << line);
            gpio_irq_descr->callback(gpio_irq_descr->data);
        }
    } else {
        size_t line_max = (line == 5) ? 10 : 16;
        // EXTI lines 5 to 9 and 10 to 15 are multiplexed.
        for (size_t i = line; i < line_max; i++) {
            omv_gpio_irq_descr_t *gpio_irq_descr = &gpio_irq_descr_table[i];
            if (__HAL_GPIO_EXTI_GET_FLAG(1 << i)
                && omv_gpio_irq_enabled(gpio_irq_descr, i)) {
                __HAL_GPIO_EXTI_CLEAR_FLAG(1 << i);
                gpio_irq_descr->callback(gpio_irq_descr->data);
            }
        }
    }
}

void omv_gpio_init0(void) {
    memset(gpio_irq_descr_table, 0, sizeof(gpio_irq_descr_table));
}

void omv_gpio_config(omv_gpio_t pin, uint32_t mode, uint32_t pull, uint32_t speed, uint32_t af) {
    GPIO_InitTypeDef gpio_config = {
        .Pin = pin->pin,
        .Mode = mode,
        .Pull = pull,
        .Speed = speed,
        .Alternate = ((af == -1) ? pin->af : af),
    };
    HAL_GPIO_Init(pin->port, &gpio_config);
}

void omv_gpio_deinit(omv_gpio_t pin) {
    HAL_GPIO_DeInit(pin->port, pin->pin);
}

bool omv_gpio_read(omv_gpio_t pin) {
    return HAL_GPIO_ReadPin(pin->port, pin->pin);
}

void omv_gpio_write(omv_gpio_t pin, bool value) {
    HAL_GPIO_WritePin(pin->port, pin->pin, value);
}

void omv_gpio_irq_register(omv_gpio_t pin, omv_gpio_callback_t callback, void *data) {
    omv_gpio_irq_descr_t *gpio_irq_descr = NULL;
    uint32_t exti_line = omv_gpio_irq_get_line(pin);
    if (exti_line != 0) {
        gpio_irq_descr = &gpio_irq_descr_table[exti_line];
        gpio_irq_descr->enabled = true;
        gpio_irq_descr->data = data;
        gpio_irq_descr->gpio = omv_gpio_irq_get_gpio(exti_line);
        gpio_irq_descr->callback = callback;
    }
}

void omv_gpio_irq_enable(omv_gpio_t pin, bool enable) {
    uint32_t exti_line = omv_gpio_irq_get_line(pin);
    uint32_t exti_irqn = omv_gpio_irq_get_irqn(pin);
    gpio_irq_descr_table[exti_line].enabled = enable;
    if (!enable) {
        HAL_NVIC_DisableIRQ(exti_irqn);
    } else {
        NVIC_SetPriority(exti_irqn, IRQ_PRI_EXTINT);
        HAL_NVIC_EnableIRQ(exti_irqn);
    }
}

void omv_gpio_clock_enable(omv_gpio_t pin, bool enable) {
    if (pin->port == GPIOA) {
        if (enable) {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOA_CLK_DISABLE();
        }
    } else if (pin->port == GPIOB) {
        if (enable) {
            __HAL_RCC_GPIOB_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOB_CLK_DISABLE();
        }
    } else if (pin->port == GPIOC) {
        if (enable) {
            __HAL_RCC_GPIOC_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOC_CLK_DISABLE();
        }
    } else if (pin->port == GPIOD) {
        if (enable) {
            __HAL_RCC_GPIOD_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOD_CLK_DISABLE();
        }
    } else if (pin->port == GPIOE) {
        if (enable) {
            __HAL_RCC_GPIOE_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOE_CLK_DISABLE();
        }
    } else if (pin->port == GPIOF) {
        if (enable) {
            __HAL_RCC_GPIOF_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOF_CLK_DISABLE();
        }
    } else if (pin->port == GPIOG) {
        if (enable) {
            __HAL_RCC_GPIOG_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOG_CLK_DISABLE();
        }
    #if defined(__HAL_RCC_GPIOH_CLK_ENABLE)
    } else if (pin->port == GPIOH) {
        if (enable) {
            __HAL_RCC_GPIOH_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOH_CLK_DISABLE();
        }
    #elif defined(__HAL_RCC_GPIOI_CLK_ENABLE)
    } else if (pin->port == GPIOI) {
        if (enable) {
            __HAL_RCC_GPIOI_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOI_CLK_DISABLE();
        }
    #elif defined(__HAL_RCC_GPIOJ_CLK_ENABLE)
    } else if (pin->port == GPIOJ) {
        if (enable) {
            __HAL_RCC_GPIOJ_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOJ_CLK_DISABLE();
        }
    #elif defined(__HAL_RCC_GPIOK_CLK_ENABLE)
    } else if (pin->port == GPIOK) {
        if (enable) {
            __HAL_RCC_GPIOK_CLK_ENABLE();
        } else {
            __HAL_RCC_GPIOK_CLK_DISABLE();
        }
    #endif
    }
}
