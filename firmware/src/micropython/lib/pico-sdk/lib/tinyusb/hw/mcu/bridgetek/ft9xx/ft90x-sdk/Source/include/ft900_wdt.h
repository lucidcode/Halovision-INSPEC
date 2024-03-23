/**
    @file ft900_wdt.h

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

#ifndef FT900_WTD_H_
#define FT900_WTD_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/
/** @brief Watchdog Timeout value
 *
    @details Corresponds to watchdog duration or timeout value in number of system clock cycles.
    The number of clock cycles is equal to 2^(enum wdt_counter_t).
    Refer to the comments against the enum values for the corresponding timeout time in seconds.
 */
typedef enum
{
    wdt_counter_1_clocks,       /*!< 10 nsec @ 100 MHz */
    wdt_counter_2_clocks,       /*!< 20 nsec @ 100 MHz */
    wdt_counter_4_clocks,       /*!< 40 nsec @ 100 MHz */
    wdt_counter_8_clocks,       /*!< 80 nsec @ 100 MHz */
    wdt_counter_16_clocks,      /*!< 160 nsec @ 100 MHz */
    wdt_counter_32_clocks,      /*!< 320 nsec @ 100 MHz */
    wdt_counter_64_clocks,      /*!< 640 nsec @ 100 MHz */
    wdt_counter_128_clocks,     /*!< 1.28 usec @ 100 MHz */
    wdt_counter_256_clocks,     /*!< 2.56 usec @ 100 MHz */
    wdt_counter_512_clocks,     /*!< 5.12 usec @ 100 MHz */
    wdt_counter_1K_clocks,      /*!< 10.24 usec @ 100 MHz */
    wdt_counter_2K_clocks,      /*!< 20.48 usec @ 100 MHz */
    wdt_counter_4K_clocks,      /*!< 40.96 usec @ 100 MHz */
    wdt_counter_8K_clocks,      /*!< 81.92 usec @ 100 MHz */
    wdt_counter_16K_clocks,     /*!< 163.84 usec @ 100 MHz */
    wdt_counter_32K_clocks,     /*!< 327.68 usec @ 100 MHz */
    wdt_counter_64K_clocks,     /*!< 655.35 usec @ 100 MHz */
    wdt_counter_128K_clocks,    /*!< ~1.31 msec @ 100 MHz */
    wdt_counter_256K_clocks,    /*!< ~2.62 msec @ 100 MHz */
    wdt_counter_512K_clocks,    /*!< ~5.24 msec @ 100 MHz */
    wdt_counter_1M_clocks,      /*!< ~10.49 msec @ 100 MHz */
    wdt_counter_2M_clocks,      /*!< ~20.97 msec @ 100 MHz */
    wdt_counter_4M_clocks,      /*!< ~41.94 msec @ 100 MHz */
    wdt_counter_8M_clocks,      /*!< ~83.89 msec @ 100 MHz */
    wdt_counter_16M_clocks,     /*!< ~167.77 msec @ 100 MHz */
    wdt_counter_32M_clocks,     /*!< ~335.54 msec @ 100 MHz */
    wdt_counter_64M_clocks,     /*!< ~671.09 msec @ 100 MHz */
    wdt_counter_128M_clocks,    /*!< ~1.34 sec @ 100 MHz */
    wdt_counter_256M_clocks,    /*!< ~2.68 sec @ 100 MHz */
    wdt_counter_512M_clocks,    /*!< ~5.37 sec @ 100 MHz */
    wdt_counter_1G_clocks,      /*!< ~10.74 sec @ 100 MHz */
    wdt_counter_2G_clocks       /*!< ~21.47 sec @ 100 MHz */
} wdt_counter_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Initialise and start the Watchdog timer
* @details If ‘X’ is the timeout time, in case of
*
* FT90x Revision B: the user application shall kick the watchdog within ‘X’ time. When the watchdog times out,
* the default handler resets the system
*
* FT90x Revision C and FT93x: the user application shall kick the watchdog within ‘X’ time. When the watchdog
* times out, irrespective of any handler, a second level timer runs for an additional ‘X’ time and the system is
* reset after that.
*
*  @param timeout Enum value corresponding to the timeout value of the Watchdog
*  @returns 0 on success, -1 otherwise
*/

int8_t wdt_init(wdt_counter_t timeout);

/** @brief Reset a running Watchdog Timer
 *  @returns 0 on success, -1 otherwise
 */
int8_t wdt_kick(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_WTD_H_ */
