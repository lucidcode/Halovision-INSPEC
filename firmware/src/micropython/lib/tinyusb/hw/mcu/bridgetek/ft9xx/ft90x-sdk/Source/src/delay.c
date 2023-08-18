/**
    @file

    @brief
    Delay Routines

    
**/
/*
 * ============================================================================
 * History
 * =======
 * 09 FEB 15: Bug #182 Fix delayus optimisation/inlining
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
#include <stdint.h>
#include <ft900_delay.h>
#include <ft900_sys.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/
#define CYCLES_IN_US (100)

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

/** @brief Delay by microseconds
 *  @param t The number of microseconds to delay by
 */
void delayus(uint32_t t)
{
    static uint8_t const shamt[8] = {0,1,2,3,6,7,9,9};
    register uint32_t r = t;

    /* Cycles per microsecond */
    r = r * CYCLES_IN_US; 

#if defined(__FT900__)
		if (!sys_check_ft900_revB())//if 90x series is rev C
		{
			/* Divider */
			r >>= shamt[(SYS->CLKCFG & MASK_SYS_CLKCFG_CPU_CLK_DIV) >> BIT_SYS_CLKCFG_CPU_CLK_DIV];
		}
#else
		/*FT930*/
		/* Divider */
		r >>= shamt[(SYS->CLKCFG & MASK_SYS_CLKCFG_CPU_CLK_DIV) >> BIT_SYS_CLKCFG_CPU_CLK_DIV];
#endif

    __asm__ volatile
    (
        "   move.l  $r0,%0"     "\n\t"
        "1:"                    "\n\t"
        "   sub.l   $r0,$r0,3"  "\n\t" /* Subtract the loop time = 4 cycles */
        "   cmp.l   $r0,0"      "\n\t" /* Check that the counter is equal to 0 */
        "   jmpc    gt,1b"      "\n\t"
        /* Outputs */ :
        /* Inputs */  : "r"(r)
        /* Using */   : "$r0"
    );
}

/** @brief Delay by milliseconds
 *  @param t The number of microseconds to delay by
 */
void delayms(uint32_t t)
{
    register uint32_t r = t;
    while (r--)
    {
        delayus(1000);
    }
}
