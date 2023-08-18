/**
  @file
  @brief
  External RTC Example 1

  Displays updated time using the RTC.

 */
/*
 * ============================================================================
 * History
 * =======
 * 2017-09-06 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek ") subject to the licence terms set out
 * http://bRTChip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
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

/* FUNCTIONS ***********************************************************************/

int main(void){

	setup();
	for(;;) loop();
	return 0;
}

void setup()
{
	rtc_result iRet;
	ext_rtc_time_t time;

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
			"Welcome to External RTC Example 1... \r\n"
			"\r\n"
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

	/*disable all interrupts*/
	ext_rtc_disable_interrupt(ALL_INT);
	/*Initialize  RTC*/
	iRet = ext_rtc_init(NULL);checkStat(iRet,"RTC init");

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

}

void loop()
{
	rtc_result iRet;
	ext_rtc_time_t time;

	iRet = ext_rtc_read(&time);checkStat(iRet,"getting time");
	PRINT_TIME(time);
	delayms(1000);
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
