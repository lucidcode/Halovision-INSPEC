/**
  @file
  @brief
  Timer Example 3

  This example program will demonstrate continuous timers being handled by an
  interrupt
  Timer A will fire every 2 seconds.
  Timer B will fire every 3 seconds.
  Timer C will fire every 4 seconds.
  Timer D will fire every 5 seconds.
*/
/*
 * ============================================================================
 * History
 * =======
 * 1 Oct 2014 : Created
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

#include <ft900.h>

/* The Timer Prescaler will divide the 100MHz Master clock down to 2kHz */
#define TIMER_PRESCALER (50000)
#define TIMER_ONE_MILLISECOND (100000/TIMER_PRESCALER)
#define TIMER_ONE_SECOND (1000*TIMER_ONE_MILLISECOND)

static uint8_t timer_interrupt_count[4] = {0};

void setup(void);
void loop(void);
void timerISR(void);

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
#if defined(__FT900__)
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#elif defined(__FT930__)
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#endif
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
        "Welcome to Timer Example 3... \r\n"
        "\r\n"
        "All timers are in continuous mode with interrupts enabled.\r\n"
        " * Timer A will fire every 2 seconds.\r\n"
        " * Timer B will fire every 3 seconds.\r\n"
        " * Timer C will fire every 4 seconds.\r\n"
        " * Timer D will fire every 5 seconds.\r\n"
        "The current state of the timer will be shown every second\r\n"
        "--------------------------------------------------------------------- \r\n"
        );


    /* Enable Timers... */
    sys_enable(sys_device_timer_wdt);
    /* Set up the Timer tick to be 0.5 ms... */
    /* Set up the Timer tick to be 0.5 ms... */
    /* FT900 Rev A and B have timers that share the prescaler */
    /* FT900 Rev C and FT93x timers have dedicated prescalers */
#if defined(__FT900__)
	if (sys_check_ft900_revB()) //90x series rev B
	{
		timer_prescaler(TIMER_PRESCALER);
	}
	else
#endif
	{
		timer_prescaler(timer_select_a, TIMER_PRESCALER);
		timer_prescaler(timer_select_b, TIMER_PRESCALER);
		timer_prescaler(timer_select_c, TIMER_PRESCALER);
		timer_prescaler(timer_select_d, TIMER_PRESCALER);
	}
    /* Set up Timer A to be triggered after 5 seconds... */
    timer_init(timer_select_a,              /* Device */
               2 * TIMER_ONE_SECOND,        /* Initial Value */
               timer_direction_down,        /* Count Direction */
               timer_prescaler_select_on,   /* Prescaler Select */
               timer_mode_continuous);      /* Timer Mode */
    /* Set up Timer B to be triggered after 6 seconds... */
    timer_init(timer_select_b,              /* Device */
               3 * TIMER_ONE_SECOND,        /* Initial Value */
               timer_direction_down,        /* Count Direction */
               timer_prescaler_select_on,   /* Prescaler Select */
               timer_mode_continuous);      /* Timer Mode */
    /* Set up Timer C to be triggered after 7 seconds... */
    timer_init(timer_select_c,              /* Device */
               4 * TIMER_ONE_SECOND,        /* Initial Value */
               timer_direction_down,        /* Count Direction */
               timer_prescaler_select_on,   /* Prescaler Select */
               timer_mode_continuous);      /* Timer Mode */
    /* Set up Timer D to be triggered after 8 seconds... */
    timer_init(timer_select_d,              /* Device */
               5 * TIMER_ONE_SECOND,        /* Initial Value */
               timer_direction_down,        /* Count Direction */
               timer_prescaler_select_on,   /* Prescaler Select */
               timer_mode_continuous);      /* Timer Mode */

    /* Register the interrupt... */
    interrupt_attach(interrupt_timers, 17, timerISR);

    /* Enable all the timers... */
    timer_enable_interrupt(timer_select_a);
    timer_enable_interrupt(timer_select_b);
    timer_enable_interrupt(timer_select_c);
    timer_enable_interrupt(timer_select_d);

    /* Enable interrupts to fire... */
    interrupt_enable_globally();

    /* Start all the timers at the same time... */
    timer_start(timer_select_a);
    timer_start(timer_select_b);
    timer_start(timer_select_c);
    timer_start(timer_select_d);
}

void loop(void)
{
    static char timer_prompt[] = "Timer A B C D\r\n";

    /* Check to see if Timer A has elapsed... */
    if (timer_interrupt_count[0])
    {
        timer_prompt[6] = 'A';

        /* Clear the count... */
        timer_interrupt_count[0] = 0;
    }
    else
    {
        timer_prompt[6] = '_';
    }
    /* Check to see if Timer B has elapsed... */
    if (timer_interrupt_count[1])
    {
        timer_prompt[8] = 'B';

        /* Clear the count... */
        timer_interrupt_count[1] = 0;
    }
    else
    {
        timer_prompt[8] = '_';
    }
    /* Check to see if Timer C has elapsed... */
    if (timer_interrupt_count[2])
    {
        timer_prompt[10] = 'C';

        /* Clear the count... */
        timer_interrupt_count[2] = 0;
    }
    else
    {
        timer_prompt[10] = '_';
    }
    /* Check to see if Timer D has elapsed... */
    if (timer_interrupt_count[3])
    {
        timer_prompt[12] = 'D';

        /* Clear the count... */
        timer_interrupt_count[3] = 0;
    }
    else
    {
        timer_prompt[12] = '_';
    }

    uart_puts(UART0, timer_prompt);

    delayms(1000);
}

/**
 The interrupt handler for the timers.

 This will keep a count of how many times each interrupt has fired in a global
 variable
*/
void timerISR(void)
{
    if (timer_is_interrupted(timer_select_a) == 1)
    {
        if (timer_interrupt_count[0] < 255)
        {
            timer_interrupt_count[0]++;
        }
    }
    if (timer_is_interrupted(timer_select_b) == 1)
    {
        if (timer_interrupt_count[1] < 255)
        {
            timer_interrupt_count[1]++;
        }
    }
    if (timer_is_interrupted(timer_select_c) == 1)
    {
        if (timer_interrupt_count[2] < 255)
        {
            timer_interrupt_count[2]++;
        }
    }
    if (timer_is_interrupted(timer_select_d) == 1)
    {
        if (timer_interrupt_count[3] < 255)
        {
            timer_interrupt_count[3]++;
        }
    }
}
