/**
    @file

    @brief
    General Purpose IO and Pad control

    
**/
/*
 * ============================================================================
 * History
 * =======
 * 09 FEB 15: Bug #181 Optimise constants to get put into .rodata.
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Future Technology Devices
 * International Limited ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES ************************************************************************/
#include <stddef.h>
#include <ft900_gpio.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/
#if defined(__FT900__)
#define MAX_GPIO (66)
#elif defined(__FT930__)
#define MAX_GPIO (39)
#endif

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/
#if defined(__FT900__)
static uint32_t volatile * const valRegs[] =
{
        &(GPIO->GPIO00_31_VAL),
        &(GPIO->GPIO32_63_VAL),
        &(GPIO->GPIO64_66_VAL),
        NULL
};

static uint32_t volatile * const intenRegs[] =
{
        &(GPIO->GPIO00_31_INT_EN),
        &(GPIO->GPIO32_63_INT_EN),
        &(GPIO->GPIO64_66_INT_EN),
        NULL
};

static uint32_t volatile * const intpendRegs[] =
{
        &(GPIO->GPIO00_31_INT_PEND),
        &(GPIO->GPIO32_63_INT_PEND),
        &(GPIO->GPIO64_66_INT_PEND),
        NULL
};

static uint32_t volatile * const configRegs[] =
{
        &(GPIO->GPIO00_07_CFG),
        &(GPIO->GPIO08_15_CFG),
        &(GPIO->GPIO16_23_CFG),
        &(GPIO->GPIO24_31_CFG),
        &(GPIO->GPIO32_39_CFG),
        &(GPIO->GPIO40_47_CFG),
        &(GPIO->GPIO48_55_CFG),
        &(GPIO->GPIO56_63_CFG),
        &(GPIO->GPIO64_71_CFG),
        NULL
};
#elif defined(__FT930__)

static uint32_t volatile * const valRegs[] =
{
        &(GPIO->GPIO00_31_VAL),
        &(GPIO->GPIO32_39_VAL),
        NULL
};

static uint32_t volatile * const intenRegs[] =
{
        &(GPIO->GPIO00_31_INT_EN),
        &(GPIO->GPIO32_39_INT_EN),
        NULL
};

static uint32_t volatile * const intpendRegs[] =
{
        &(GPIO->GPIO00_31_INT_PEND),
        &(GPIO->GPIO32_39_INT_PEND),
        NULL
};

static uint32_t volatile * const configRegs[] =
{
        &(GPIO->GPIO00_07_CFG),
        &(GPIO->GPIO08_15_CFG),
        &(GPIO->GPIO16_23_CFG),
        &(GPIO->GPIO24_31_CFG),
        &(GPIO->GPIO32_39_CFG),
        NULL
};

#else

#error -- processor type not defined

#endif

/* MACROS **************************************************************************/
#define GPIO_PAD(x) *(&(GPIO->PAD00_CFG)+x)

#define determine_val_reg(num, reg) determine_reg(valRegs, num, reg)
#define determine_inten_reg(num, reg) determine_reg(intenRegs, num, reg)
#define determine_intpend_reg(num, reg) determine_reg(intpendRegs, num, reg)

/* LOCAL FUNCTIONS / INLINES *******************************************************/
static inline int8_t not_valid_gpio(uint8_t num)
{
    return (num > MAX_GPIO);
}

static inline void determine_reg(uint32_t volatile * const * array, uint8_t* num, uint32_t volatile ** reg)
{
    while((*num > 31) && (*array))
    {
        *num -= 32;
        array++;
    }
    *reg = (uint32_t volatile *)(*array);
}

static inline void determine_config_reg(uint8_t* num, uint32_t volatile ** reg)
{
    uint32_t volatile * const * array = configRegs;

    while((*num > 7) && (*array))
    {
        *num -= 8;
        array++;
    }
    *reg = (uint32_t volatile *)(*array);
}


/* FUNCTIONS ***********************************************************************/

/** @brief Configure the direction of a pin
 *  @param num The GPIO number
 *  @param dir The direction
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_dir(uint8_t num, pad_dir_t dir)
{
    int8_t iRet = 0;
    volatile uint32_t *pReg = 0;
    uint8_t iShift = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        pReg = (&(GPIO->GPIO00_07_CFG) + (num / 8));
        iShift = (num % 8) * 4;

        if (dir == pad_dir_input)
        {
            *pReg &= ~((MASK_GPIOXX_XX_CFG_DIR1 | MASK_GPIOXX_XX_CFG_DIR0) << iShift);
        }
        else if (dir == pad_dir_output)
        {
            *pReg &= ~((MASK_GPIOXX_XX_CFG_DIR1) << iShift);
            *pReg |= ((MASK_GPIOXX_XX_CFG_DIR0) << iShift);
        }
        else if (dir == pad_dir_open_drain)
        {
            *pReg |= ((MASK_GPIOXX_XX_CFG_DIR1) << iShift);
        }
        else
        {
            iRet = -1;
        }

    }

    return iRet;
}


/** @brief Configure the slew rate for a pin
 *  @param num The GPIO number
 *  @param slew The slew rate of the pin
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_slew(uint8_t num, pad_slew_t slew)
{
    int8_t iRet = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (slew == pad_slew_fast)
        {
            GPIO_PAD(num) &= ~MASK_PADXX_CFG_SLEW;
        }
        else if (slew == pad_slew_slow)
        {
            GPIO_PAD(num) |= MASK_PADXX_CFG_SLEW;
        }
        else
        {
            iRet = -1;
        }
    }

    return iRet;
}


/** @brief Configure the schmitt trigger for a pin
 *  @param num The GPIO number
 *  @param schmitt The Schmitt trigger configuration
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_schmitt(uint8_t num, pad_schmitt_t schmitt)
{
    int8_t iRet = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (schmitt == pad_schmitt_off)
        {
            GPIO_PAD(num) &= ~MASK_PADXX_CFG_SCHMITT;
        }
        else if (schmitt == pad_schmitt_on)
        {
            GPIO_PAD(num) |= MASK_PADXX_CFG_SCHMITT;
        }
        else
        {
            iRet = -1;
        }
    }

    return iRet;
}


/** @brief Configure the pull up/down for a pin
 *  @param num The GPIO number
 *  @param pull The pullup/down configuration
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_pull(uint8_t num, pad_pull_t pull)
{
    int8_t iRet = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (pull == pad_pull_none)
        {
            GPIO_PAD(num) &= ~(MASK_PADXX_CFG_PULL1 | MASK_PADXX_CFG_PULL0);
        }
        else if (pull == pad_pull_pullup)
        {
            GPIO_PAD(num) |= MASK_PADXX_CFG_PULL1;
            GPIO_PAD(num) &= ~MASK_PADXX_CFG_PULL0;
        }
        else if (pull == pad_pull_pulldown)
        {
            GPIO_PAD(num) &= ~MASK_PADXX_CFG_PULL1;
            GPIO_PAD(num) |= MASK_PADXX_CFG_PULL0;
        }
        else if (pull == pad_pull_keeper)
        {
            GPIO_PAD(num) |= (MASK_PADXX_CFG_PULL1 | MASK_PADXX_CFG_PULL0);
        }
        else
        {
            iRet = -1;
        }
    }

    return iRet;
}


/** @brief Configure the maximum current drive for a pin
 *  @param num The GPIO number
 *  @param drive The maximum current
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_idrive(uint8_t num, pad_drive_t drive)
{
    int8_t iRet = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (drive == pad_drive_4mA)
        {
            GPIO_PAD(num) &= ~(MASK_PADXX_CFG_DRV1 | MASK_PADXX_CFG_DRV0);
        }
        else if (drive == pad_drive_8mA)
        {
            GPIO_PAD(num) &= ~MASK_PADXX_CFG_DRV1;
            GPIO_PAD(num) |= MASK_PADXX_CFG_DRV0;
        }
        else if (drive == pad_drive_12mA)
        {
            GPIO_PAD(num) |= MASK_PADXX_CFG_DRV1;
            GPIO_PAD(num) &= ~MASK_PADXX_CFG_DRV0;
        }
        else if (drive == pad_drive_16mA)
        {
            GPIO_PAD(num) |= (MASK_PADXX_CFG_DRV1 | MASK_PADXX_CFG_DRV0);
        }
        else
        {
            iRet = -1;
        }
    }

    return iRet;
}


/** @brief Configure the alternative function for a pin
 *  @param num The GPIO number
 *  @param func The function that the pin should use
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_function(uint8_t num, pad_func_t func)
{
    int8_t iRet = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (func == pad_func_0)
        {
            GPIO_PAD(num) &= ~(MASK_PADXX_CFG_FUNC1 | MASK_PADXX_CFG_FUNC0);
        }
        else if (func == pad_func_1)
        {
            GPIO_PAD(num) &= ~MASK_PADXX_CFG_FUNC1;
            GPIO_PAD(num) |= MASK_PADXX_CFG_FUNC0;
        }
        else if (func == pad_func_2)
        {
            GPIO_PAD(num) |= MASK_PADXX_CFG_FUNC1;
            GPIO_PAD(num) &= ~MASK_PADXX_CFG_FUNC0;
        }
        else if (func == pad_func_3)
        {
            GPIO_PAD(num) |= (MASK_PADXX_CFG_FUNC1 | MASK_PADXX_CFG_FUNC0);
        }
        else
        {
            iRet = -1;
        }
    }

    return iRet;
}


/** @brief Write a value to a GPIO pin
 *  @param num The GPIO number
 *  @param val The value to write
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_write(uint8_t num, uint8_t val)
{
    int8_t iRet = 0;
    volatile uint32_t *pReg = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        determine_val_reg(&num, &pReg);

        if (pReg)
        {
            if (val == 0)
            {
                /* Clear */
                *pReg &= ~(1 << num);
            }
            else if (val == 1)
            {
                /* Set */
                *pReg |= (1 << num);
            }
            else
            {
                iRet = -1;
            }
        }
        else
        {
            iRet = -1;
        }
    }

    return iRet;
}


/** @brief Read a value from a GPIO pin
 *  @param num The GPIO number
 *  @param val The value to write
 *  @returns The value of the pin (1 = high, 0 = low), otherwise -1
 */
int8_t gpio_read(uint8_t num)
{
    int8_t iRet = 0;
    volatile uint32_t *pReg = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        determine_val_reg(&num, &pReg);

        if (*pReg & (1 << num))
        {
            iRet = 1;
        }
        else
        {
            iRet = 0;
        }
    }

    return iRet;
}


/** @brief Toggle the value of a GPIO pin
 *  @param num The GPIO number
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_toggle(uint8_t num)
{
    int8_t iRet = 0;
    volatile uint32_t *pReg = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        determine_val_reg(&num, &pReg);

        if (pReg) *pReg = *pReg ^ (1 << num);
        else iRet = -1;
    }

    return iRet;
}


/** @brief Enable an interrupt on a GPIO pin
 *  @param num The GPIO number
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_interrupt_enable(uint8_t num, gpio_int_edge_t edge)
{
    int8_t iRet = 0;
    volatile uint32_t *enreg = 0, *cfgreg = 0;
    uint8_t i;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {

        i = num;
        determine_config_reg(&i, &cfgreg);

        if (edge == gpio_int_edge_raising)
        {
            *cfgreg |= MASK_GPIOXX_XX_CFG_INTEDGE << (4 * i);
        }
        else
        {
            *cfgreg &= ~(MASK_GPIOXX_XX_CFG_INTEDGE << (4 * i));
        }

        *cfgreg |= MASK_GPIOXX_XX_CFG_INTEN << (4 * i);

        /* Enable interrupts on the pad */
        i = num;
        determine_inten_reg(&i, &enreg);

        *enreg = *enreg | (1 << i);

    }

    return iRet;
}


/** @brief Disable an interrupt on a GPIO pin
 *  @param num The GPIO number
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_interrupt_disable(uint8_t num)
{
    int8_t iRet = 0;
    volatile uint32_t *enreg = 0, *cfgreg = 0;
    uint8_t i;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        i = num;
        determine_inten_reg(&i, &enreg);

        *enreg = *enreg & ~(1 << i);

        i = num;
        determine_config_reg(&i, &cfgreg);

        *cfgreg &= ~(MASK_GPIOXX_XX_CFG_INTEN << (4 * i));
    }

    return iRet;
}


/** @brief Check if an interrupt has happened on a GPIO pin
 *  @param num The GPIO number
 *  @returns On no interrupt 0, on an interrupt 1, otherwise -1
 */
int8_t gpio_is_interrupted(uint8_t num)
{
    int8_t iRet = 0;
    volatile uint32_t *pReg = 0;

    if (not_valid_gpio(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        determine_intpend_reg(&num, &pReg);

        if (*pReg & (1 << num))
        {
            /* Interrupted, clear the interrupt */
            iRet = 1;
            *pReg = *pReg | (1 << num);
        }

    }

    return iRet;
}
