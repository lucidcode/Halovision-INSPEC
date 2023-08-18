/**
    @file

    @brief
    Watchdog Timer

    
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
#include <ft900_wdt.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

/** @brief Initialise and start the Watchdog timer
 *  @param timeout The timeout value of the Watchdog
 *  @returns 0 on success, -1 otherwise
 */
int8_t wdt_init(wdt_counter_t timeout)
{
    int8_t iRet = 0;

    if (timeout < wdt_counter_1_clocks || timeout > wdt_counter_2G_clocks)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        TIMER->TIMER_WDG = (timeout << BIT_TIMER_WDG_WRITE) & MASK_TIMER_WDG_WRITE;

        TIMER->TIMER_CONTROL_2 |= MASK_TIMER_CONTROL_2_CLEAR_WDT; /* Clear the timer just to be sure */
        TIMER->TIMER_CONTROL_2 |= MASK_TIMER_CONTROL_2_WDG_INT_EN | MASK_TIMER_CONTROL_2_START_WDT; /* Turn on the WDG and its Interrupt */

    }

    return iRet;
}


/** @brief Reset a running Watchdog Timer
 *  @returns 0 on success, -1 otherwise
 */
int8_t wdt_kick(void)
{
    TIMER->TIMER_CONTROL_2 |= MASK_TIMER_CONTROL_2_CLEAR_WDT;
    return 0;
}
