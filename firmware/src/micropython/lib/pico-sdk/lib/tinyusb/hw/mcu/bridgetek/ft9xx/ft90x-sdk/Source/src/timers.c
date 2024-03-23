/**
    @file

    @brief
    Timers

    
**/
/*
 * ============================================================================
 * History
 * =======
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
#include <ft900_timers.h>
#include <registers/ft900_registers.h>
#include <ft900_sys.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/
/** @brief Initialise a timer
 *  @param timer The timer to set up
 *  @param initial The initial value for the timer
 *  @param dir The direction that the timer should count in
 *  @param prescaler Whether or not this timer should use the prescaler
 *  @param mode If the timer should be continuously counting or a one shot
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_init(timer_select_t timer, uint16_t initial, timer_direction_t dir,
        timer_prescaler_select_t prescaler, timer_mode_t mode)
{
    int8_t iRet = 0;

    if ((timer < timer_select_a || timer > timer_select_d) ||
        (dir < timer_direction_up || dir > timer_direction_down) ||
        (prescaler < timer_prescaler_select_off || prescaler > timer_prescaler_select_on) ||
        (mode < timer_mode_continuous || mode > timer_mode_oneshot))
    {
        iRet = -1; /* Out of range */
    }

    if (iRet == 0)
    {
        switch(timer)
        {
            case timer_select_a:
            case timer_select_b:
            case timer_select_c:
            case timer_select_d:
            {
                TIMER->TIMER_CONTROL_1 |= MASK_TIMER_CONTROL_1_STOP(timer);

                TIMER->TIMER_SELECT = (timer << BIT_TIMER_SELECT_TIMER_WRITE_SEL);
                TIMER->TIMER_WRITE_LS = initial & 0xFF;
                TIMER->TIMER_WRITE_MS = initial >> 8;

                uint8_t control3 = TIMER->TIMER_CONTROL_3;
                uint8_t control2 = TIMER->TIMER_CONTROL_2;

                control3 &= ~(MASK_TIMER_CONTROL_3_DIRECTION(timer) | MASK_TIMER_CONTROL_3_MODE(timer));
                control2 &= ~(MASK_TIMER_CONTROL_2_PRESCALER_EN(timer));

                if (dir == timer_direction_up)
                    control3 |= (MASK_TIMER_CONTROL_3_DIRECTION(timer));

                if (mode == timer_mode_oneshot)
                    control3 |= (MASK_TIMER_CONTROL_3_MODE(timer));

                if (prescaler == timer_prescaler_select_on)
                    control2 |= (MASK_TIMER_CONTROL_2_PRESCALER_EN(timer));

                TIMER->TIMER_CONTROL_3 = control3;
                TIMER->TIMER_CONTROL_2 = control2;
            }
            break;

            default: break;

        }

    }

    return iRet;
}


/** @brief Set up the prescaler
 *  @param prescaler The clock prescaler to apply to the timer
 *  @returns On success a 0, otherwise -1
 *  @warning This can only be used before starting timers
 */
int8_t timer_prescaler_ex(timer_select_t timer, uint16_t prescaler)
{
	if (sys_check_ft900_revB()) //90x series rev B
	{
		TIMER->TIMER_CONTROL_4 |= MASK_TIMER_CONTROL_4_PRESC_CLEAR;
	}
	else
	{
	   if (timer > timer_select_d) return -1;
		TIMER->TIMER_SELECT = (timer << BIT_TIMER_SELECT_TIMER_WRITE_SEL);
		TIMER->TIMER_CONTROL_4 |= (MASK_TIMER_CONTROL_4_PRESC_CLEAR_A << timer);
	}
    TIMER->TIMER_PRESC_LS = (prescaler & 0x00FF) >> 0;
    TIMER->TIMER_PRESC_MS = (prescaler & 0xFF00) >> 8;
    return 0;
}


/** @brief Start a timer
 *  @param timer The timer to start
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_start(timer_select_t timer)
{
    int8_t iRet = 0;

    switch(timer)
    {
        case timer_select_a:
        case timer_select_b:
        case timer_select_c:
        case timer_select_d:
        {
            TIMER->TIMER_CONTROL_1 |= MASK_TIMER_CONTROL_1_START(timer);
        }
        break;
        default: iRet = -1; break;
    }

    return iRet;
}

/** @brief Stop a timer
 *  @param timer The timer to stop
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_stop(timer_select_t timer)
{
    int8_t iRet = 0;

    switch(timer)
    {
        case timer_select_a: 
        case timer_select_b:
        case timer_select_c:
        case timer_select_d:
        {
            TIMER->TIMER_CONTROL_1 |= MASK_TIMER_CONTROL_1_STOP(timer); break;
        }
        default: iRet = -1; break;
    }

    return iRet;
}

/** @brief Read the value of a timer
 *  @param timer The timer to read from
 *  @param value A pointer to store the value
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_read(timer_select_t timer, uint16_t *value)
{
    int8_t iRet = 0;

    switch(timer)
    {
        case timer_select_a:
        case timer_select_b:
        case timer_select_c:
        case timer_select_d:
            TIMER->TIMER_SELECT = (timer << BIT_TIMER_SELECT_TIMER_READ_SEL); break;
        default: iRet = -1; break;
    }

    if (iRet != -1)
    {
        *value = (TIMER->TIMER_READ_MS << 8) | (TIMER->TIMER_READ_LS);
    }

    return iRet;
}


/** @brief Enable the interrupt for a timer
 *  @param timer The timer to enable the interrupt for
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_enable_interrupt(timer_select_t timer)
{
    int8_t iRet = 0;

    switch(timer)
    {
        case timer_select_a:
        case timer_select_b:
        case timer_select_c:
        case timer_select_d:
            TIMER->TIMER_INT |= (MASK_TIMER_INT_TIMER_INT_EN(timer) | MASK_TIMER_INT_TIMER_INT(timer)); break;
        default: iRet = -1; break;
    }

    return iRet;
}


/** @brief Disable the interrupt for a timer
 *  @param timer The timer to disable the interrupt for
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_disable_interrupt(timer_select_t timer)
{
    int8_t iRet = 0;

    switch(timer)
    {
        case timer_select_a:
        case timer_select_b:
        case timer_select_c:
        case timer_select_d:
            TIMER->TIMER_INT &= ~MASK_TIMER_INT_TIMER_INT_EN(timer); break;
        default: iRet = -1; break;
    }

    return iRet;
}


/** @brief Check if a timer has been interrupted
 *  @param timer The timer to check
 *  @warning This function clears the current interrupt status bit
 *  @returns 1 for if a timer is interrupted, 0 if the timer is not interrupted, -1 otherwise
 */
int8_t timer_is_interrupted(timer_select_t timer)
{
    int8_t iRet = 0;
    uint8_t timer_int_ens = TIMER->TIMER_INT &
            (MASK_TIMER_INT_TIMER_INT_EN(timer_select_a) | MASK_TIMER_INT_TIMER_INT_EN(timer_select_b) |
             MASK_TIMER_INT_TIMER_INT_EN(timer_select_c) | MASK_TIMER_INT_TIMER_INT_EN(timer_select_d));

    switch(timer)
    {
        case timer_select_a:
        case timer_select_b:
        case timer_select_c:
        case timer_select_d:
        {
            if (TIMER->TIMER_INT & MASK_TIMER_INT_TIMER_INT(timer))
            {
                TIMER->TIMER_INT = timer_int_ens | MASK_TIMER_INT_TIMER_INT(timer);
                iRet = 1;
            }
            else
            {
                iRet = 0;
            }
        }
        break;

        default: iRet = -1; break;
    }

    return iRet;
}
