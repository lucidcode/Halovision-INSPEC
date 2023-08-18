/**
    @file

    @brief
    Real Time Clock

    
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

#ifndef FT900_RTC_H_
#define FT900_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <time.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/** @brief RTC Interrupts */
typedef enum
{
	/* Below ENUMS applicable to FT930 and FT900 Rev C onwards! */
    rtc_interrupt_alarm1,              	/**< Alarm 1 Interrupt */
    rtc_interrupt_alarm2,              	/**< Alarm 2 Interrupt */
    rtc_interrupt_ext_osc_stopped,   	/**< External Oscillator Stopped Interrupt */
    rtc_interrupt_int_osc_stopped,   	/**< Internal Oscillator Stopped Interrupt */
    rtc_interrupt_int_auto_calibration,   	/**< RTC auto-calibration Interrupt */
    rtc_interrupt_max,                   /**< Max enum */
	rtc_legacy_interrupt_alarm			/**< This ENUM is ONLY applicable to FT900 Rev B */
} rtc_interrupt_t;

/** @brief RTC Alarm Type */
typedef enum
{
	/* Below ENUMS applicable to FT930 and FT900 Rev C onwards! */
    rtc_alarm_1Hz,              			/**< Alarm every second */
    rtc_alarm_match_sec,              		/**< Alarm when seconds match*/
    rtc_alarm_match_min_sec,   				/**< Alarm when minutes and seconds match */
	rtc_alarm_match_hr_min_sec,   			/**< Alarm when hours, minutes and seconds match */
	rtc_alarm_match_date_hr_min_sec,		/**< Alarm when date, hours, minutes and second match */
	rtc_alarm_match_day_hr_min_sec,			/**< Alarm when day, hours, minutes and second match */
	rtc_legacy_alarm						/**< This ENUM is ONLY applicable to FT900 Rev B */
} rtc_alarm_type_t;

/** @brief RTC Options */
typedef enum
{
	rtc_option_auto_refresh,          /**< Enable or disable the Auto Refresh of RTC */
	/* Below ENUMS are ONLY applicable to FT900 Rev B */
	rtc_option_wrap,					/**< Enable or disable RTC Wrap around  */
	rtc_option_mask_interrupt         /**< Option to set whether the Real Time Clock module should mask the match interrupt line */
} rtc_option_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Initialise the Real Time Clock
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_init(void);

/** @brief Allow the Real Time Clock to start counting
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_start(void);

/** @brief Stops the Real Time Clock from counting
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_stop(void);

/** @brief Set the Real Time Clock to a value
 *  @param time A pointer of type struct tm (defined in <time.h>) contains
 *  			time to be written into the RTC for FT93x and FT90x rev C.
 *  			Only the field tm.sec should contain the RTC time to be written in
 *  			case of FT90x rev B.
 *  @returns 0 on success, -1 otherwise
 */

int8_t rtc_write(const struct tm* time);

/** @brief Read the current value of the Real Time Clock
 *  @param time A pointer of type struct tm (defined in <time.h>) to which
 *  			the RTC time is read in case of FT93x and FT90x rev C.
 *  			Only the field tm.sec is written to with RTC time in
 *  			case of FT90x rev B.
 *  @returns 0 on success, -1 otherwise
 */

int8_t rtc_read(struct tm* const time);

/** @brief Set up various RTC options
 *  @param opt The type of option selected
 *  @param val The value of the option selected
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_option(rtc_option_t opt, uint8_t val);

/** @brief Set up the an alarm for the RTC
 *  @param number The Alarm number (1 or 2)
 *  @param time The time that the alarm should fire at
 *  @param alarm_type The type of the alarm that determines the match conditions
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_set_alarm(uint8_t number,  struct tm* time, rtc_alarm_type_t alarm_type);

/** @brief Enable an interrupt on the RTC
 *  @param interrupt The interrupt type to enable
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_enable_interrupt(rtc_interrupt_t interrupt);

/** @brief Enable an interrupt on the RTC
 *  @param interrupt The interrupt type to disable
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_disable_interrupt(rtc_interrupt_t interrupt);

/** @brief Disable a RTC from interrupting
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_disable_interrupts_globally(void);

/** @brief Enable RTC interrupts
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_enable_interrupts_globally(void);

/** @brief Check if an interrupt has been triggered
 *  @param interrupt The interrupt to check
 *  @warning This function clears the current interrupt status bit
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 */
int8_t rtc_is_interrupted(rtc_interrupt_t interrupt);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_RTC_H_ */
