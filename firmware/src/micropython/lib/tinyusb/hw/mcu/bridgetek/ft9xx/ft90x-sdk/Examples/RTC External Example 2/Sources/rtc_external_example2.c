/**
  @file
  @brief
  External RTC Example 2

  Displays updated time for every 2 seconds via RTC interrupt

 */
/*
 * ============================================================================
 * History
 * =======
 * 2017-09-02 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek ") subject to the licence terms set out
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
#include "../Includes/ft900_rtc.h"
#include<ft900_sys.h>

/* MACROS ***********************************************************************/
#define GPIO_UART0_TX 	48
#define GPIO_UART0_RX 	49
#define PRINT_TIME(t)	print_time(t)
#define SLEEP_MODE		0

/* CONSTANTS/ GLOBAL VARIABLES *****************************************************/
const char *days[]		= {"NONE","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
const char *t_conv[] 	= {"AM","PM"};
uint8_t INT_1 = 0;
uint8_t INT_2 = 0;

static inline void print_time(ext_rtc_time_t time)
{
	tfp_printf("%02d/%02d/%02d%s  %02d:%02d:%02d",time.date,time.month,time.year,days[time.day],time.hour,time.min,time.sec);
	if(time.fmt_12_24)
		tfp_printf("%s",t_conv[time.AM_PM]);
	tfp_printf(" \r\n");
}

/* LOCAL FUNCTIONS / INLINES *******************************************************/
void setup(void);
void loop(void);
void tfp_putc(void* p, char c);
#if SLEEP_MODE
void enter_sleep(void);
void exit_sleep(void);
void power_management_ISR(void);
#endif	// SLEEP_MODE

/* FUNCTIONS ***********************************************************************/

void rtc_callback()
{
	ext_rtc_alarm_stat stat;
	stat = ext_rtc_alarm_status();
	if (gpio_is_interrupted(RTC_INT_PIN))
	{
		if(stat & AL_0)
		{
			ext_rtc_clear_alarm_interrupt(ALARM_0);
		INT_1 = 1;
		}
		if(stat & AL_1)
		{
			ext_rtc_clear_alarm_interrupt(ALARM_1);
			INT_2 = 1;
		}
	}
}

int main(void)
{
	setup();
	for(;;) loop();
	return 0;
}

void setup()
{
	rtc_result iRet;
	ext_rtc_time_t time;
	ext_rtc_alarm_time_t al_time = {0};

	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
	/* Set UART0 GPIO functions to UART0_TXD and UART0_RXD... */
	gpio_function(GPIO_UART0_TX, pad_uart0_txd); /* UART0 TXD */
	gpio_function(GPIO_UART0_RX, pad_uart0_rxd); /* UART0 RXD */
	uart_open(UART0,                    /* Device */
			  1,                        /* Prescaler = 1 */
			  UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
			  uart_data_bits_8,         /* No. Data Bits */
			  uart_parity_none,         /* Parity */
			  uart_stop_bits_1);        /* No. Stop Bits */

	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
			"Copyright (C) Bridgetek Pte Ltd \r\n"
			"--------------------------------------------------------------------- \r\n"
			"Welcome to RTC External Example 2... \r\n"
			"\r\n"
			"Display  Time every two seconds via External RTC interrupt.\r\n"
			"--------------------------------------------------------------------- \r\n"
			);


	/* Enable tfp_printf() functionality... */
	init_printf(UART0, tfp_putc);

	if(!sys_check_ft900_revB())
	{
		tfp_printf("External RTC is supported only in FT900 Revision B \r\n");
		exit(1);
	}
   /* Enable I2C*/
	sys_enable(sys_device_i2c_master);
	/* Set GPIO44 to I2CM_SCL and GPIO45 to I2CM_SDA... */
	gpio_function(44, pad_i2c0_scl);
	gpio_pull(44, pad_pull_none);
	gpio_function(45, pad_i2c0_sda);
	gpio_pull(45, pad_pull_none);
	i2cm_init(I2CM_NORMAL_SPEED, 10000);

	/*disable all interrupts external rtc interrupts*/
	iRet = ext_rtc_disable_interrupt(ALL_INT);checkStat(iRet,"disabling interrupt");

	/*Initialize  rtc with interrupt*/
	iRet = ext_rtc_init(rtc_callback);checkStat(iRet,"rtc init");


	/* time for RTC*/
	time.sec		= 1;
	time.hour 		= 9;
	time.min		= 00;
	time.date		= 9;
	time.month		= August;
	time.year		= 17;
	time.day		= Wednesday;
	time.fmt_12_24 	= HOUR_FORMAT_12;
	time.AM_PM		= AM_HOUR_FORMAT;
	iRet = ext_rtc_write(time);checkStat(iRet,"updating time");


	tfp_printf("RTC Time: ");
	ext_rtc_read(&time);checkStat(iRet,"getting time");
	PRINT_TIME(time);

#if SLEEP_MODE
	al_time.sec = 55;
#else
	al_time.sec = 5;
#endif
	ext_rtc_set_alarm(ALARM_0, al_time, seconds);checkStat(iRet,"setting alarm");

#if SLEEP_MODE
		enter_sleep();
#endif

}

void loop()
{
	rtc_result iRet;
	ext_rtc_time_t time;
	ext_rtc_alarm_time_t al_time;

	if(INT_1)
	{
		tfp_printf("Alarm 1:  ");
		iRet = ext_rtc_read(&time);checkStat(iRet,"getting time");
		PRINT_TIME(time);
		al_time.sec = time.sec + 2;
		al_time.sec %= 60;
		iRet = ext_rtc_set_alarm(ALARM_0, al_time, seconds);checkStat(iRet,"setting alarm");
		INT_1 = 0;
	}
#if SLEEP_MODE
	else
	{
		tfp_printf("========Awake=========\r\n");
		delayms(1000); // 1 seconds once
	}
#endif
}

/** @name tfp_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
void tfp_putc(void* p, char c)
{
	uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}

#if SLEEP_MODE

void enter_sleep(void)
{
		/* Allow RTC alarm to wakeup the system */
//		SYS->PMCFG_L |= MASK_SYS_PMCFG_RTC_ALARM_IRQ_EN;

	interrupt_attach(interrupt_0, (int8_t)interrupt_0, power_management_ISR);

	/* Turn off both PLL and Oscillator */
	SYS->PMCFG_L |= (MASK_SYS_PMCFG_PM_PWRDN | MASK_SYS_PMCFG_PM_PWRDN_MODE);

	delayms(10);
}

void exit_sleep(void)
{
	/* Clear power down mode flags */
	SYS->PMCFG_L &= ~(MASK_SYS_PMCFG_PM_PWRDN_MODE | MASK_SYS_PMCFG_PM_PWRDN);
	interrupt_detach(interrupt_0);
}


/* Power management ISR */
void power_management_ISR(void)
{
	if (SYS->PMCFG_H & MASK_SYS_PMCFG_PM_GPIO_IRQ_PEND)
	{
		tfp_printf("=========RTC WAKE-UP===========\r\n");
		SYS->PMCFG_H |= MASK_SYS_PMCFG_PM_GPIO_IRQ_PEND;
		delayms(1); /* RTC needs a delay before access when you wakeup from sleep */

		 exit_sleep();
	}
}

#endif	// SLEEP_MODE

