/**
  @file
  @brief
  RTC Example 2

  Display a message every two seconds via an RTC interrupt.
*/
/*
 * ============================================================================
 * History
 * =======
 * 2 Oct 2014 : Created
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

#include <stdint.h>
#include <ft900.h>
#include "tinyprintf.h"

#if defined(__FT900__)
#define GPIO_UART0_TX 	48
#define GPIO_UART0_RX 	49
#elif defined(__FT930__)
#define GPIO_UART0_TX 	23
#define GPIO_UART0_RX 	22
#endif // __FT930__

/* Set to 1 for a demonstration of the RTC usage in Sleep Mode.
 * RTC wakeup applicable only from FT900 Rev C onwards and in FT930
 * */
#define SLEEP_MODE 0

time_t start_time;
//static double seconds_elapsed = 0;

#if SLEEP_MODE
/* Set the time in seconds as to when the program should be taken to sleep. The wakeup is by the RTC alarm */
#define SECONDS_TO_SLEEP		10
volatile int seconds_to_sleep = 0;
void enter_sleep(void);
void exit_sleep(void);
void power_management_ISR(void);
#endif	// SLEEP_MODE

void setup(void);
void loop(void);
void rtcISR(void);
void myputc(void* p, char c);

int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void setup(void)
{
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
        "Welcome to RTC Example 2... \r\n"
        "\r\n"
        "Display a message every two seconds via an RTC interrupt.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(NULL,myputc);

    /* Enable the RTC... */
    sys_enable(sys_device_timer_wdt);

    /* Initialise the RTC to run indefinitely... */
    rtc_init();

    /* Clear the flag... */
    //rtc_is_matched();

    struct tm tim = {0};
    rtc_write(&tim);
    start_time = mktime(&tim);

    /* Set an alarm for 2 Seconds */
    tim.tm_sec = 2;

    if (sys_check_ft900_revB()) //90x series rev B
    {
		rtc_option(rtc_option_wrap, 0); /* rtc wrap disabled */
		rtc_option(rtc_option_mask_interrupt, 0); /* do not mask rtc interrupt */
        rtc_enable_interrupts_globally();
    	rtc_is_interrupted(rtc_legacy_interrupt_alarm); /* clear any interrupts */
    	tim.tm_sec <<= 5;
    	rtc_set_alarm(1,  &tim, rtc_legacy_alarm);
    }
    else
    {
		/* Don't use auto-refresh in sleep mode, as the auto-refresh only occurs
		 * every 1 Second.
		 */
#if !SLEEP_MODE
		/* Use auto refresh mode */
		rtc_option(rtc_option_auto_refresh, 1);
#endif
    	rtc_set_alarm(1,  &tim, rtc_alarm_match_sec);

    }

    /* Start the RTC...*/
    rtc_start();
#if defined(__FT900__)

    /* Register the RTC interrupt... */
    interrupt_attach(interrupt_rtc, (uint8_t)interrupt_rtc, rtcISR);
    if (sys_check_ft900_revB()) //90x series rev B
    {
    	rtc_enable_interrupt(rtc_legacy_interrupt_alarm);
    }
    else
    {
    	rtc_enable_interrupt(rtc_interrupt_alarm1);
    }
    rtc_enable_interrupts_globally();
#else

    /* Register the RTC interrupt... */
    /* NOTE: There is an IP Bug that causes the alarm interrupt to misbehave
     * so we don't use the alarm interrupt in this example.
     *
    interrupt_attach(interrupt_rtc, (uint8_t)interrupt_rtc, rtcISR);
    rtc_enable_interrupt(rtc_interrupt_alarm1);
    rtc_enable_interrupts_globally();
     */
#endif
    interrupt_enable_globally();

}

void loop(void)
{

#if SLEEP_MODE
	if (seconds_to_sleep == SECONDS_TO_SLEEP)
	{
		uart_puts(UART0, "\nenter sleep..\n");
		delayms(1); // Flush the UART buffer
		enter_sleep();
		exit_sleep();
		uart_puts(UART0, "exit sleep..\n");
		seconds_to_sleep = 0;
	}
#endif	// SLEEP_MODE
#if defined(__FT930__)
    /*
     * WORKAROUND for RTC Alarm Interrupt Bug in FT930:
     * The RTC Alarm Interrupt is a "Pulse Mode" interrupt in FT930.
     * This means that the RTC interrupt could be missed if the system has temporarily disabled interrupts
     * in say a Critical Section. To avoid the interrupt being missed, this example polls the RTC interrupt flag
     * every 500mS and executes the ISR if the flag is set. Similarly upon wake-up from sleep, the flag must be
     * checked and ISR polled as the alarm interrupt will have been missed.
     */
    //tfp_printf("%ld, %ld\r", RTCL->RTC_CMR,RTCL->RTC_CCVR);
	rtcISR();
    delayms(500);
#endif

}

void rtcISR(void)
{
    struct tm tim = {0};
    rtc_alarm_type_t type;
    uint32_t seconds = 0;

    if (sys_check_ft900_revB()) //90x series rev B
    {
    	type = rtc_legacy_interrupt_alarm;
    }
    else
    {
    	type = rtc_interrupt_alarm1;
    }

    if (rtc_is_interrupted(type))
    {
        /* Read the time... */
        rtc_read(&tim);
        if (sys_check_ft900_revB()) //90x series rev B
        {
        	seconds = (tim.tm_sec >> 5);
            /* Set the new alarm */
            tim.tm_sec = (seconds + 2)<<5;
            rtc_set_alarm(1,  &tim, rtc_legacy_alarm);
        }
        else
        {
        	//seconds_elapsed = difftime(mktime(&tim), start_time);
        	//in case of gnu system, time can directly subtracted
        	seconds = mktime(&tim) - start_time;
            /* Set the new alarm */
            tim.tm_sec = (tim.tm_sec + 2) % 60;
            rtc_set_alarm(1,  &tim, rtc_alarm_match_sec);
        }
        tfp_printf("%ld seconds elapsed\r", seconds);
#if SLEEP_MODE
        seconds_to_sleep = seconds;
#endif
    }
}

#if SLEEP_MODE

void enter_sleep(void)
{
	if (!sys_check_ft900_revB()) //FT900 rev C or FT930
	{
		/* Allow RTC alarm to wakeup the system */
		SYS->PMCFG_L |= MASK_SYS_PMCFG_RTC_ALARM_IRQ_EN;
	}

	interrupt_attach(interrupt_0, (int8_t)interrupt_0, power_management_ISR);

	/* Turn off both PLL and Oscillator */
	SYS->PMCFG_L |= MASK_SYS_PMCFG_PM_PWRDN | MASK_SYS_PMCFG_PM_PWRDN_MODE;

	delayms(10);
}

void exit_sleep(void)
{
	if (!sys_check_ft900_revB()) //FT900 rev C or FT930
	{
		/* Remove the RTC alarm as a wakeup source */
		SYS->PMCFG_L &= ~MASK_SYS_PMCFG_RTC_ALARM_IRQ_EN;
	}

	/* Clear power down mode flags */
	SYS->PMCFG_L &= ~(MASK_SYS_PMCFG_PM_PWRDN_MODE | MASK_SYS_PMCFG_PM_PWRDN);

}


/* Power management ISR */
void power_management_ISR(void)
{
	if (!sys_check_ft900_revB()) //FT900 rev C or FT930
	{
		if (SYS->PMCFG_H & MASK_SYS_PMCFG_RTC_ALARM_IRQ_PEND)
		{
			SYS->PMCFG_H = MASK_SYS_PMCFG_RTC_ALARM_IRQ_PEND;
			delayms(1); /* RTC needs a delay before access when you wakeup from sleep */
		}
	}
}

#endif	// SLEEP_MODE

void myputc(void* p, char c)
{
    uart_write(UART0, (uint8_t)c);
}
