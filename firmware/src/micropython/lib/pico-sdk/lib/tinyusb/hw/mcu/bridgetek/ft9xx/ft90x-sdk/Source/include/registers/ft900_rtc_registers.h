/**
    @file ft900_rtc_registers.h

    @brief
    Real Time Clock registers
    
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

#ifndef FT900_RTC_REGISTERS_H_
#define FT900_RTC_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
/* On board RTC used in FT900 Rev B */
#define ONBOARD_RTC_TYPE_1
/* On board RTC used in FT900 Rev C and FT930 */
#define ONBOARD_RTC_TYPE_2

#if defined(ONBOARD_RTC_TYPE_2)
/* RTC Time Register */
#define BIT_RTC_HOURS    		(0)
#define MASK_RTC_HOURS			(0x3F << BIT_RTC_HOURS)

#define BIT_RTC_12HR_FORMAT    	(6)
#define MASK_12HR_FORMAT		(1 << BIT_RTC_12HR_FORMAT)

#define BIT_RTC_MINS    		(0)
#define MASK_RTC_MINS			(0x7F << BIT_RTC_HOURS)

#define BIT_RTC_SECONDS    		(0)
#define MASK_RTC_SECONDS		(0x7F << BIT_RTC_SECONDS)

/* RTC Date Register */
#define BIT_RTC_DATE    		(0)
#define MASK_RTC_DATE			(0x7F << BIT_RTC_DATE)

#define BIT_RTC_WDAY    		(0)
#define MASK_RTC_WDAY			(0x03 << BIT_RTC_WDAY)

#define BIT_RTC_MONTH    		(0)
#define MASK_RTC_MONTH			(0x1F << BIT_RTC_MONTH)

#define BIT_RTC_CENTURY    		(7)
#define MASK_RTC_CENTURY		(0x1 << BIT_RTC_CENTURY)

#define BIT_RTC_YEAR    		(0)
#define MASK_RTC_YEAR			(0xFF << BIT_RTC_YEAR)

/* RTC Control Register */
#define BIT_RTC_CTRL_RTCEN    	(0)
#define MASK_RTC_CTRL_RTCEN		(0x1 << BIT_RTC_CTRL_RTCEN)

#define BIT_RTC_CTRL_INTEN    	(1)
#define MASK_RTC_CTRL_INTEN		(0x1 << BIT_RTC_CTRL_INTEN)

#define BIT_RTC_CTRL_A1IE    	(2)
#define MASK_RTC_CTRL_A1IE		(0x1 << BIT_RTC_CTRL_A1IE)

#define BIT_RTC_CTRL_A2IE    	(3)
#define MASK_RTC_CTRL_A2IE		(0x1 << BIT_RTC_CTRL_A2IE)

#define BIT_RTC_CTRL_EOSIE    	(4)
#define MASK_RTC_CTRL_EOSIE		(0x1 << BIT_RTC_CTRL_EOSIE)

#define BIT_RTC_CTRL_IOSIE    	(5)
#define MASK_RTC_CTRL_IOSIE		(0x1 << BIT_RTC_CTRL_IOSIE)

#define BIT_RTC_CTRL_AUCAL    	(6)
#define MASK_RTC_CTRL_AUCAL		(0x1 << BIT_RTC_CTRL_AUCAL)

#define BIT_RTC_CTRL_AURFE    	(7)
#define MASK_RTC_CTRL_AURFE		(0x1 << BIT_RTC_CTRL_AURFE)

#define BIT_RTC_CTRL_MARFE    	(8)
#define MASK_RTC_CTRL_MARFE		(0x1 << BIT_RTC_CTRL_MARFE)

#define BIT_RTC_CTRL_WRST    	(9)
#define MASK_RTC_CTRL_WRST		(0x1 << BIT_RTC_CTRL_WRST)

/* RTC Status Register */
#define BIT_RTC_STAT_A1IE    	(2)
#define MASK_RTC_STAT_A1IE		(0x1u << BIT_RTC_STAT_A1IE)

#define BIT_RTC_STAT_A2IE    	(3)
#define MASK_RTC_STAT_A2IE		(0x1u << BIT_RTC_STAT_A2IE)

#define BIT_RTC_STAT_EOSIE    	(4)
#define MASK_RTC_STAT_EOSIE		(0x1u << BIT_RTC_STAT_EOSIE)

#define BIT_RTC_STAT_IOSIE    	(5)
#define MASK_RTC_STAT_IOSIE		(0x1u << BIT_RTC_STAT_IOSIE)
#endif

#if defined(ONBOARD_RTC_TYPE_1)

#define BIT_RTC_CCR_INTEN       (0)
#define MASK_RTC_CCR_INTEN      (1 << BIT_RTC_CCR_INTEN)
#define BIT_RTC_CCR_INTMASK     (1)
#define MASK_RTC_CCR_INTMASK    (1 << BIT_RTC_CCR_INTMASK)
#define BIT_RTC_CCR_EN          (2)
#define MASK_RTC_CCR_EN         (1 << BIT_RTC_CCR_EN)
#define BIT_RTC_CCR_WRAPEN      (3)
#define MASK_RTC_CCR_WRAPEN     (1 << BIT_RTC_CCR_WRAPEN)

#define BIT_RTC_STAT_STAT       (0)
#define MASK_RTC_STAT_STAT      (1 << BIT_RTC_STAT_STAT)

#define BIT_RTC_RSTAT_RSTAT     (0)
#define MASK_RTC_RSTAT_RSTAT    (1 << BIT_RTC_RSTAT_RSTAT)

#define BIT_RTC_EOI_EOI         (0)
#define MASK_RTC_EOI_EOI        (1 << BIT_RTC_EOI_EOI)

#endif

/* TYPES ***************************************************************************/

#if defined(ONBOARD_RTC_TYPE_2)
/** @brief Register mappings for RTC registers in case of RTC hw used in
 * FT900 Rev C and FT930 */

typedef struct
{
    volatile uint32_t RTC_TIME;
    volatile uint32_t RTC_DATE;
    volatile uint32_t RTC_AALARM1;
    volatile uint32_t RTC_AALARM2;
    volatile uint32_t RTC_CTRL;
    volatile uint32_t RTC_STA;
    volatile uint32_t RTC_TRIMMING;
    volatile uint32_t RTC_SLE_TIME;
    volatile uint32_t RTC_SLE_DATE;
    volatile uint32_t RTC_COMP_PER;
    volatile uint32_t RTC_VREF_TRIM;
} ft900_rtc_regs_t;
#endif

#if defined(ONBOARD_RTC_TYPE_1)
/** @brief Register mappings for RTC registers in case of RTC hw used in
 * FT900 rev B*/
typedef struct
{
    volatile uint32_t RTC_CCVR;
    volatile uint32_t RTC_CMR;
    volatile uint32_t RTC_CLR;
    volatile uint32_t RTC_CCR;
    volatile uint32_t RTC_STAT;
    volatile uint32_t RTC_RSTAT;
    volatile uint32_t RTC_EOI;
    volatile uint32_t RTC_COMP_VERSION;
} ft900_rtc_legacy_regs_t;
#endif


/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/


#endif /* FT900_RTC_REGISTERS_H_ */
