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

/* INCLUDES ************************************************************************/
#include <ft900_rtc.h>
#include <registers/ft900_registers.h>
#include <private/ft900_internals.h>
#include <ft900_delay.h>
#include <ft900_asm.h>
#include <ft900_sys.h>

/* These map the format of the Date and Time registers on the FT930 RTC */
typedef union {
	struct {
		uint8_t seconds;
		uint8_t minutes;
		uint8_t hours;
		uint8_t reserv;
	};
	uint32_t Time;
} rtc_time;

typedef union {
	struct {
		uint8_t day;
		uint8_t date;
		uint8_t month;
		uint8_t year;
	};
	uint32_t Date;
} rtc_date;

/* CONSTANTS ***********************************************************************/
#define RTC_POWER_ON_DELAY_MS	18	// 512 * 32kHz cycles = 16mS
#define RTC_WRITE_DELAY_MS			10

/* uint8_t to BCD and back */
#define C2B(x)	 	((((uint8_t)(x) / 10) << 4) | ((uint8_t)(x) % 10))
#define B2C(x)	 	(uint8_t)((((x) >> 4) * 10) + ((x) & 0x0F))

// Special alarm flags
#define BIT_RTC_ALARM_DAY_DATE		(6)
#define MASK_RTC_ALARM_DAY_DATE		(0x1 << BIT_RTC_ALARM_DAY_DATE)

#define BIT_RTC_ALARM_MATCH_SECONDS			(7)
#define MASK_RTC_ALARM_MATCH_SECONDS		(0x1 << BIT_RTC_ALARM_MATCH_SECONDS)

#define BIT_RTC_ALARM_MATCH_MINS			(15)
#define MASK_RTC_ALARM_MATCH_MINS			(0x1 << BIT_RTC_ALARM_MATCH_MINS)

#define BIT_RTC_ALARM_MATCH_HOURS			(23)
#define MASK_RTC_ALARM_MATCH_HOURS			(0x1 << BIT_RTC_ALARM_MATCH_HOURS)

#define BIT_RTC_ALARM_MATCH_DAY_DATE		(31)
#define MASK_RTC_ALARM_MATCH_DAY_DATE		(0x1 << BIT_RTC_ALARM_MATCH_DAY_DATE)


/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/
#if defined(__FT930__)
/* Dummy declaration for legacy FT900 Rev B */
#define RTCL        ((ft900_rtc_legacy_regs_t *)    0x10900)
#endif

/* LOCAL FUNCTIONS / INLINES *******************************************************/
static void sync(void);
static uint32_t get_date(const struct tm* time);
static uint32_t get_time(const struct tm* time);
static void write_reg(volatile uint32_t* reg, uint32_t val);
/* FUNCTIONS ***********************************************************************/

/** @brief Initialise the Real Time Clock
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_init(void)
{
    int8_t iRet = 0;
	if (!sys_check_ft900_revB()) //if ft930 or ft900 rev C onwards
    {
		delayms(RTC_POWER_ON_DELAY_MS);     // Wait for power on delay
		//write_reg(&RTC->RTC_STA, 0xFC); // Status register is RW1C
	}

    return iRet;
}

/** @brief Allow the Real Time Clock to start counting
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_start(void)
{
	if (sys_check_ft900_revB()) //90x series rev B
    {
		RTCL->RTC_CCR |= MASK_RTC_CCR_EN;
    }
	else
	{
		/* Enable RTC and Auto-refresh mode */
		write_reg(&RTC->RTC_CTRL, RTC->RTC_CTRL | MASK_RTC_CTRL_RTCEN);
		write_reg(&RTC->RTC_STA, 0xFC); // Status register is RW1C, clear everything
	}
    return 0;
}


/** @brief Stops the Real Time Clock from counting
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_stop(void)
{
	if (sys_check_ft900_revB()) //90x series rev B
    {
		RTCL->RTC_CCR &= ~MASK_RTC_CCR_EN;
    }
	else
	{
		/* Disable RTC and Auto-refresh mode */
		uint32_t ctl = RTC->RTC_CTRL & ~(MASK_RTC_CTRL_RTCEN | MASK_RTC_CTRL_AURFE);
		write_reg(&RTC->RTC_CTRL, ctl);
	}
    return 0;
}


/** @brief Set the Real Time Clock to a value
 *  @param val The value to set the Real Time Clock to
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_write(const struct tm* time)
{
	if(time == NULL) return -1;
	if (sys_check_ft900_revB()) //90x series rev B
    {
		RTCL->RTC_CLR = time->tm_sec;
    }
	else
	{
		// Set the date/time
		RTC->RTC_DATE = get_date(time);
		RTC->RTC_TIME = get_time(time);
		sync();
	}
    return 0;
}


/** @brief Read the current value of the Real Time Clock
 *  @param val A pointer to  store the current Real Time Clock Value into
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_read(struct tm* const time)
{
    if(time == NULL) return -1;
	if (sys_check_ft900_revB()) //90x series rev B
    {
		time->tm_sec = RTCL->RTC_CCVR;
    }
	else
	{
		/* If the ARFE bit is not set, we need to do a manual refresh before reading */
		if((RTC->RTC_CTRL & MASK_RTC_CTRL_AURFE) == 0x00 ){
			RTC->RTC_CTRL |= MASK_RTC_CTRL_MARFE;
			while((RTC->RTC_CTRL & MASK_RTC_CTRL_MARFE) != 0) // Wait until complete
				;
		}
		rtc_time t;
		rtc_date d;
		// Get the date/time
		t.Time = RTC->RTC_TIME;
		d.Date = RTC->RTC_DATE;

		time->tm_sec = B2C(t.seconds & MASK_RTC_SECONDS);
		time->tm_min = B2C(t.minutes & MASK_RTC_MINS);
		time->tm_hour = B2C(t.hours & MASK_RTC_HOURS);
		time->tm_wday = B2C(d.day & MASK_RTC_WDAY) - 1;
		time->tm_mday = B2C(d.date & MASK_RTC_DATE);
		time->tm_mon = B2C(d.month & MASK_RTC_MONTH);
		time->tm_year = B2C(d.year & MASK_RTC_YEAR) + (d.month & MASK_RTC_CENTURY) * 100;
	}
    return 0;
}

/** @brief Enable an interrupt on the RTC
 *  @param interrupt The interrupt type to enable
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_enable_interrupt(rtc_interrupt_t interrupt)
{
	if (!sys_check_ft900_revB()) //FT930 or FT900 Rev C onwards
    {
		if(interrupt >= rtc_interrupt_max) return -1;

		uint32_t ctl = RTC->RTC_CTRL | (MASK_RTC_CTRL_A1IE << interrupt) ;
		write_reg(&RTC->RTC_CTRL, ctl);
    }
	return 0;
}

/** @brief Enable an interrupt on the RTC
 *  @param interrupt The interrupt to enable
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_disable_interrupt(rtc_interrupt_t interrupt)
{
	if (!sys_check_ft900_revB()) //FT930 or FT900 Rev C onwards
    {
		if(interrupt  >= rtc_interrupt_max) return -1;

		uint32_t ctl = RTC->RTC_CTRL & ~(MASK_RTC_CTRL_A1IE << interrupt);
		write_reg(&RTC->RTC_CTRL, ctl);
    }
	return 0;
}

/** @brief Disable a RTC from interrupting
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_disable_interrupts_globally(void)
{
	if (sys_check_ft900_revB()) //90x series rev B
    {
		RTCL->RTC_CCR &= ~MASK_RTC_CCR_INTEN;
    }
	else
	{
		uint32_t ctl = RTC->RTC_CTRL & ~(MASK_RTC_CTRL_INTEN);
		write_reg(&RTC->RTC_CTRL, ctl);
	}
	return 0;
}

/** @brief Enable RTC interrupts
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_enable_interrupts_globally(void)
{
	if (sys_check_ft900_revB()) //90x series rev B
    {
		RTCL->RTC_CCR |= MASK_RTC_CCR_INTEN;
    }
	else
	{
		write_reg(&RTC->RTC_CTRL, RTC->RTC_CTRL | MASK_RTC_CTRL_INTEN);
	}
	return 0;
}

/** @brief Check if an interrupt has been triggered
 *  @param interrupt The interrupt to check
 *  @warning This function clears the current interrupt status bit
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 */
int8_t rtc_is_interrupted(rtc_interrupt_t interrupt)
{
	if (sys_check_ft900_revB()) //90x series rev B
    {
	    uint32_t stat = RTCL->RTC_STAT; /* Back up the stat register */
	    FT900_ATTR_UNUSED uint32_t eoi = RTCL->RTC_EOI; /* Clear the interrupt */
	    return (stat & MASK_RTC_STAT_STAT) > 0;
    }
	else
	{
		if(interrupt >= rtc_interrupt_max) return -1;
		/* Note that the bits are W12C */
		if((RTC->RTC_STA & (MASK_RTC_STAT_A1IE << interrupt)) == (MASK_RTC_STAT_A1IE << interrupt))
		{
			write_reg(&RTC->RTC_STA, RTC->RTC_STA | (MASK_RTC_STAT_A1IE << interrupt));
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

/** @brief Set up the an alarm for the RTC
 *  @param number The Alarm number (1 or 2)
 *  @param time The time that the alarm should fire at
 *  @param alarm_type The type of the alarm that determines the match conditions
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_set_alarm(uint8_t number,  struct tm* time, rtc_alarm_type_t alarm_type)
{

	if (sys_check_ft900_revB()) //90x series rev B
    {
		if((!number) || (number > 1) || (time == NULL) || (alarm_type != rtc_legacy_alarm)) return -1;

        RTCL->RTC_CMR = time->tm_sec;
    }
	else
	{
		if(number < 1 || number > 2) return -1;

		rtc_time t = {0};
		if(time != NULL) // If rtc_alarm_1Hz, time could be NULL. We will just use 0's for this case
		{
			t.Time = get_time(time);
		}

		switch(alarm_type)
		{
		case rtc_alarm_1Hz:
			t.Time |= (MASK_RTC_ALARM_MATCH_SECONDS | MASK_RTC_ALARM_MATCH_MINS | MASK_RTC_ALARM_MATCH_HOURS | MASK_RTC_ALARM_MATCH_DAY_DATE);
			break;
		case rtc_alarm_match_sec:
			t.Time |= (MASK_RTC_ALARM_MATCH_MINS | MASK_RTC_ALARM_MATCH_HOURS | MASK_RTC_ALARM_MATCH_DAY_DATE);
			break;
		case rtc_alarm_match_min_sec:
			t.Time |= (MASK_RTC_ALARM_MATCH_HOURS | MASK_RTC_ALARM_MATCH_DAY_DATE);
			break;
		case rtc_alarm_match_hr_min_sec:
			t.Time |= (MASK_RTC_ALARM_MATCH_DAY_DATE);
			break;
		case rtc_alarm_match_date_hr_min_sec:
			t.reserv = C2B(time->tm_mday) & MASK_RTC_DATE;
			break;
		case rtc_alarm_match_day_hr_min_sec:
			/* Remember to set the Day/Date bit */
			t.reserv = ((C2B(time->tm_wday) & MASK_RTC_WDAY) + 1) | MASK_RTC_ALARM_DAY_DATE;
			break;

		default:
			return -1;
			break;
		}

		if(number == 2)
			RTC->RTC_AALARM2 = t.Time;
		else
			RTC->RTC_AALARM1 = t.Time;

		sync();
	}
	return 0;
}

/** @brief Set up various RTC options
 *  @param number The type of option selected
 *  @param time The value of the option selected
 *  @returns 0 on success, -1 otherwise
 */
int8_t rtc_option(rtc_option_t opt, uint8_t val)
{
	int8_t iRet = 0;

	if (sys_check_ft900_revB()) //90x series rev B
	{
		if(opt == rtc_option_wrap)
		{
			if(val)
			{
				/*case rtc_wrap_enabled:*/
				RTCL->RTC_CCR |= MASK_RTC_CCR_WRAPEN;
			}
			else
			{
				/*case rtc_wrap_disabled:*/
				RTCL->RTC_CCR &= ~MASK_RTC_CCR_WRAPEN;
			}
		}
		else if(opt == rtc_option_mask_interrupt)
		{
			if(val)
			{
				RTCL->RTC_CCR |= MASK_RTC_CCR_INTMASK;  	/* Mask interrupt */
			}
			else
			{
				RTCL->RTC_CCR &= ~MASK_RTC_CCR_INTMASK; 	/* Do not mask interrupt */
			}
		}
		else
		{
			iRet = -1;
		}

	}
	else
	{
		if(opt == rtc_option_auto_refresh)
		{
			if(val)
				write_reg(&RTC->RTC_CTRL, RTC->RTC_CTRL | MASK_RTC_CTRL_AURFE);
			else
				write_reg(&RTC->RTC_CTRL, RTC->RTC_CTRL & (~MASK_RTC_CTRL_AURFE));
		}
		else{
			iRet = -1;
		}
	}


	return iRet;
}

static uint32_t get_time(const struct tm* time)
{
    rtc_time t;
	t.hours = (C2B(time->tm_hour) & MASK_RTC_HOURS);
	t.minutes = C2B(time->tm_min) & MASK_RTC_MINS;
	t.seconds = C2B(time->tm_sec) & MASK_RTC_SECONDS;

	return t.Time;
}

/** @brief Converts a struct tm* to uint32_t to be wrritten into the RTC
 * The WRST bit should be set to update any of the registers that are buffered in the Alive register
 */
static uint32_t get_date(const struct tm* time)
{
    rtc_date d;
	d.date = C2B(time->tm_mday) & MASK_RTC_DATE;
	// FT930 RTC defines WDAY to range from 1 - 7, however time.h defines it to be from 0 - 6
	d.day = (C2B(time->tm_wday) & MASK_RTC_WDAY) + 1;
	d.month = C2B(time->tm_mon) & MASK_RTC_MONTH;
	d.year = C2B(time->tm_year) & MASK_RTC_YEAR;

	return d.Date;
}

/** @brief Waits till write is complete
 * The WRST bit should be set to update any of the registers that are buffered in the Alive register
 */
static void sync(void)
{
	RTC->RTC_CTRL |= MASK_RTC_CTRL_WRST;
	delayms(RTC_WRITE_DELAY_MS);
	while((RTC->RTC_CTRL & MASK_RTC_CTRL_WRST) != 0) // Wait until complete
		;
}

/** @brief Writes an RTC register with a value
 *  @param reg Pointer to the register
 *  @param val Value to be written (uint32_t)
 */
static void write_reg(volatile uint32_t* reg, uint32_t val){

	/* Wait for Busy Bit to clear */
	while((RTC->RTC_CTRL & MASK_RTC_CTRL_WRST) != 0)
		;

	/* When writing the control register, update all the bits and WRST bit together */
	if(reg != &RTC->RTC_CTRL){
		*reg = val;
		RTC->RTC_CTRL |= MASK_RTC_CTRL_WRST;
	}
	else{
		val |= MASK_RTC_CTRL_WRST;
		*reg = val;
	}

	delayms(RTC_WRITE_DELAY_MS);
	/* Wait for Busy Bit to clear */
	while((RTC->RTC_CTRL & MASK_RTC_CTRL_WRST) != 0)
		;
}

