/**
    @file ft900_delay.h

    @brief
    Delay functions

    
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

#ifndef FT900_DELAY_H_
#define FT900_DELAY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

#if defined(__MIKROC_PRO_FOR_FT90x__)
#define delayms(A) VDelay_ms(A)
#define delayus(A) Delay_us(A)
#endif

/* POSIX Standard sleep calls */
/** @def usleep
 *  @brief POSIX standard microsecond sleep call 
 *  @param x The number of microseconds to sleep */
#ifndef usleep
#   define usleep(x) delayus(x)
#endif /* ifndef usleep */

/** @def sleep
 *  @brief POSIX standard second sleep call 
 *  @param x The number of seconds to sleep */
#ifndef sleep
#   define sleep(x) delayms(x*1000)
#endif /* ifndef sleep */

/* FUNCTION PROTOTYPES *************************************************************/

#if defined(__GNUC__)
/** @brief Delay by microseconds
 *  @param t The number of microseconds to delay by
 */
void delayus(uint32_t t);

/** @brief Delay by milliseconds
 *  @param t The number of milliseconds to delay by
 */
void delayms(uint32_t t);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_DELAY_H_ */
