/**
    @file ft900.h

    @brief
    FT900 API

    This file includes the entire FT900 API.
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
 * ("Bridgetek") subject to the licence terms set out
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

#ifndef FT900_H_
#define FT900_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief FT9xx library version x.y.z format, where x - Major number, y - Minor number, z - patch build.
 *         !!! To be updated in every release !!!
 */
#define FT9XX_LIBRARY_MAJOR   2
#define FT9XX_LIBRARY_MINOR   6
#define FT9XX_LIBRARY_PATCH   0

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifdef __CDT_PARSER__
#define __flash__ // to avoid eclipse syntax error
#endif

/** @brief FT9xx library version in string format
 */
#define FT9XX_LIBRARY_VERSION_STR "" STR(FT9XX_LIBRARY_MAJOR) "." STR(FT9XX_LIBRARY_MINOR) "." STR(FT9XX_LIBRARY_PATCH)

/** @brief Single number format for library version
 *  Test for toolchain version > 2.4.0, then usage is
 *  \#if FT9XX_LIBRARY_VERSION > 20400
 *    \<call a new API\>
 *  \#endif
 */
#define FT9XX_LIBRARY_VERSION (FT9XX_LIBRARY_MAJOR * 10000 \
        							+ FT9XX_LIBRARY_MINOR * 100 \
										+ FT9XX_LIBRARY_PATCH)

/* INCLUDES ************************************************************************/
#if defined(__FT930__)

#include <ft900_adc.h>
#include <ft900_asm.h>
#include <ft900_dac.h>
#include <ft900_delay.h>
#include <ft900_gpio.h>
#include <ft900_i2cm.h>
#include <ft900_interrupt.h>
#include <ft900_memctl.h>
#include <ft900_pwm_pcm.h>
#include <ft900_pwm.h>
#include <ft900_rtc.h>
#include <ft900_sdhost.h>
#include <ft900_spi.h>
#include <ft900_sys.h>
#include <ft900_timers.h>
#include <ft900_uart_simple.h>
#include <ft900_usbd.h>
#include <ft900_usbd_dfu.h>
#include <ft900_wdt.h>
#include <registers/ft900_registers.h>

#else // Assume __FT900 as default (for backward compatibility)

#include <ft900_adc.h>
#include <ft900_asm.h>
#include <ft900_cam.h>
#include <ft900_can.h>
#include <ft900_dac.h>
#include <ft900_delay.h>
#include <ft900_eth.h>
#include <ft900_gpio.h>
#include <ft900_i2cm.h>
#include <ft900_i2cs.h>
#include <ft900_i2s.h>
#include <ft900_interrupt.h>
#include <ft900_memctl.h>
#include <ft900_pwm_pcm.h>
#include <ft900_pwm.h>
#include <ft900_rtc.h>
#include <ft900_sdhost.h>
#include <ft900_spi.h>
#include <ft900_sys.h>
#include <ft900_timers.h>
#include <ft900_uart_simple.h>
#include <ft900_usbd.h>
#include <ft900_usbd_dfu.h>
#include <ft900_usbh.h>
#include <ft900_wdt.h>
#include <registers/ft900_registers.h>

#endif

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

#if defined(__FT930__)
#define CRITICAL_SECTION_BEGIN	{ \
			unsigned char	__mask_all__ = 0x80; 		\
			__asm__ volatile ("exa.b %[m], 0x10123" 	"\n\t"\
							  "nop"						"\n\t"\
							  "nop"						"\n\t"\
					: [m] "+r" (__mask_all__)	\
					: : "memory" );	{

#define CRITICAL_SECTION_END	} \
			__asm__ volatile ("exa.b %[m], 0x10123" 	\
					: [m] "+r" (__mask_all__)	\
					: : "memory" );	}

#else // Assume __FT900 as default (for backward compatibility)
#define CRITICAL_SECTION_BEGIN	{ \
			unsigned char	__mask_all__ = 0x80; 		\
			__asm__ volatile ("exa.b %[m], 0x100E3" 	"\n\t"\
							  "nop"						"\n\t"\
							  "nop"						"\n\t"\
					: [m] "+r" (__mask_all__)	\
					: : "memory" );	{

#define CRITICAL_SECTION_END	} \
			__asm__ volatile ("exa.b %[m], 0x100E3" 	\
					: [m] "+r" (__mask_all__)	\
					: : "memory" );	}
#endif

/* FUNCTION PROTOTYPES *************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_H_ */
