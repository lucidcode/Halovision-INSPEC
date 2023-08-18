/**
    @file

    @brief
    Pulse Width Modulation

    
**/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek") subject to the licence terms set out
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
#include <ft900_pwm.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/
#define PWM_N_CHANNELS      (8U)
/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

/** @brief Initialise the PWM subsystem
 *  @param prescaler The prescaler for the PWM subsystem (0 = /1)
 *  @param maxcount The maximum count of the 16 bit master counter
 *  @param shots The number of loops the PWM subsystem will make, 0 is infinity
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_init(uint8_t prescaler, uint16_t maxcount, uint8_t shots)
{
    PWM->PWM_PRESCALER = prescaler;
    PWM->PWM_CNTH = (maxcount >> 8) & 0x00FF;
    PWM->PWM_CNTL = (maxcount >> 0) & 0x00FF;
    PWM->PWM_CTRL_BL_CMP8 = shots;

    return 0;
}


/** @brief Enable the PWM subsystem
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_enable(void)
{
    PWM->PWM_CTRL1 |= MASK_PWM_CTRL1_PWMEN;
    return 0;
}


/** @brief Disable the PWM subsystem
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_disable(void)
{
    PWM->PWM_CTRL1 &= ~MASK_PWM_CTRL1_PWMEN;
    return 0;
}


/** @brief Set up the logic levels for a PWM counter
 *  @param channel The channel to use
 *  @param initstate The initial state of the counter (high or low)
 *  @param restorestate The rollover restore setting
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_levels(uint8_t channel, pwm_state_t initstate, pwm_restore_t restorestate)
{
    int8_t iRet = 0;

    if (channel >= PWM_N_CHANNELS)
    {
        iRet = -1; /* Invalid channel */
    }

    if (iRet == 0)
    {
        /* Set up the initial counter state */
        switch(initstate)
        {
            case pwm_state_high: PWM->PWM_INIT |= (1 << channel); break;
            case pwm_state_low: PWM->PWM_INIT &= ~(1 << channel); break;
            default: iRet = -1; break;
        }

        /* Set up the restore state */
        switch(restorestate)
        {
            case pwm_restore_enable: PWM->PWM_OUT_CLR_EN |= (1 << channel); break;
            case pwm_restore_disable: PWM->PWM_OUT_CLR_EN &= ~(1 << channel); break;
            default: iRet = -1; break;
        }

    }

    return iRet;
}


/** @brief Set a compare value for a PWM counter
 *  @param channel The channel to use
 *  @param value The value to toggle on
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_compare(uint8_t channel, uint16_t value)
{
    int8_t iRet = 0;

    /* Set up the compare value */
    switch(channel)
    {
        case 0: PWM->PWM_CMP0H = (value >> 8) & 0x00FF; PWM->PWM_CMP0L = (value >> 0) & 0x00FF; break;
        case 1: PWM->PWM_CMP1H = (value >> 8) & 0x00FF; PWM->PWM_CMP1L = (value >> 0) & 0x00FF; break;
        case 2: PWM->PWM_CMP2H = (value >> 8) & 0x00FF; PWM->PWM_CMP2L = (value >> 0) & 0x00FF; break;
        case 3: PWM->PWM_CMP3H = (value >> 8) & 0x00FF; PWM->PWM_CMP3L = (value >> 0) & 0x00FF; break;
        case 4: PWM->PWM_CMP4H = (value >> 8) & 0x00FF; PWM->PWM_CMP4L = (value >> 0) & 0x00FF; break;
        case 5: PWM->PWM_CMP5H = (value >> 8) & 0x00FF; PWM->PWM_CMP5L = (value >> 0) & 0x00FF; break;
        case 6: PWM->PWM_CMP6H = (value >> 8) & 0x00FF; PWM->PWM_CMP6L = (value >> 0) & 0x00FF; break;
        case 7: PWM->PWM_CMP7H = (value >> 8) & 0x00FF; PWM->PWM_CMP7L = (value >> 0) & 0x00FF; break;
        default: iRet = -1; break;
    }

    return iRet;
}


/** @brief Add a toggle to a specific PWM channel
 *  @param channel The channel to add the toggle to
 *  @param toggle The channel to toggle on
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_add_toggle(uint8_t channel, uint8_t toggle)
{
    int8_t iRet = 0;

    if (channel >= PWM_N_CHANNELS || toggle >= PWM_N_CHANNELS)
    {
        iRet = -1; /* Invalid channel */
    }

    if (iRet == 0)
    {
        switch(channel)
        {
            case 0: PWM->PWM_TOGGLE0 |= (1 << toggle); break;
            case 1: PWM->PWM_TOGGLE1 |= (1 << toggle); break;
            case 2: PWM->PWM_TOGGLE2 |= (1 << toggle); break;
            case 3: PWM->PWM_TOGGLE3 |= (1 << toggle); break;
            case 4: PWM->PWM_TOGGLE4 |= (1 << toggle); break;
            case 5: PWM->PWM_TOGGLE5 |= (1 << toggle); break;
            case 6: PWM->PWM_TOGGLE6 |= (1 << toggle); break;
            case 7: PWM->PWM_TOGGLE7 |= (1 << toggle); break;
            default: break;
        }
    }

    return iRet;
}


/** @brief Remove a toggle to a specific PWM channel
 *  @param channel The channel to remove the toggle from
 *  @param toggle The channel to remove the toggle of
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_remove_toggle(uint8_t channel, uint8_t toggle)
{
    int8_t iRet = 0;

    if (channel >= PWM_N_CHANNELS || toggle >= PWM_N_CHANNELS)
    {
        iRet = -1; /* Invalid channel */
    }

    if (iRet == 0)
    {
        switch(channel)
        {
            case 0: PWM->PWM_TOGGLE0 &= ~(1 << toggle); break;
            case 1: PWM->PWM_TOGGLE1 &= ~(1 << toggle); break;
            case 2: PWM->PWM_TOGGLE2 &= ~(1 << toggle); break;
            case 3: PWM->PWM_TOGGLE3 &= ~(1 << toggle); break;
            case 4: PWM->PWM_TOGGLE4 &= ~(1 << toggle); break;
            case 5: PWM->PWM_TOGGLE5 &= ~(1 << toggle); break;
            case 6: PWM->PWM_TOGGLE6 &= ~(1 << toggle); break;
            case 7: PWM->PWM_TOGGLE7 &= ~(1 << toggle); break;
            default: break;
        }
    }

    return iRet;
}


/** @brief Set the external trigger settings
 *  @param trigger The trigger setting
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_trigger(pwm_trigger_t trigger)
{
    int8_t iRet = 0;

    switch(trigger)
    {
        case pwm_trigger_disabled:
            PWM->PWM_CTRL1 = (PWM->PWM_CTRL1 & MASK_PWM_CTRL1_PWMTRIG) | (0 << BIT_PWM_CTRL1_PWMTRIG); break;
        case pwm_trigger_negative_edge:
            PWM->PWM_CTRL1 = (PWM->PWM_CTRL1 & MASK_PWM_CTRL1_PWMTRIG) | (1 << BIT_PWM_CTRL1_PWMTRIG); break;
        case pwm_trigger_positive_edge:
            PWM->PWM_CTRL1 = (PWM->PWM_CTRL1 & MASK_PWM_CTRL1_PWMTRIG) | (2 << BIT_PWM_CTRL1_PWMTRIG); break;
        case pwm_trigger_any_edge:
            PWM->PWM_CTRL1 = (PWM->PWM_CTRL1 & MASK_PWM_CTRL1_PWMTRIG) | (3 << BIT_PWM_CTRL1_PWMTRIG); break;
        default:
            iRet = -1; break;
    }

    return iRet;
}

