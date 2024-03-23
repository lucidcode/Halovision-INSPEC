/**
  @file
  @brief
  PWM Example 1

  Output a number of PWM levels on various pins:
   * PWM0 will output 25% duty cycle
   * PWM1 will output 50% duty cycle
   * PWM2 will output 75% duty cycle
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


void setup(void);
void loop(void);

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
        "Welcome to PWM Example 1... \r\n"
        "\r\n"
        "Output a number of PWM levels on various pins: \r\n"
        " * PWM0 will output 25% duty cycle\r\n"
        " * PWM1 will output 50% duty cycle\r\n"
        " * PWM2 will output 75% duty cycle\r\n"
        "--------------------------------------------------------------------- \r\n"
        );


    /* Enable the PWM subsystem */
    sys_enable(sys_device_pwm);

    /* Set up pins for PWM */
#if defined(__FT900__)
    gpio_function(56, pad_pwm0); /* PWM0 */
    gpio_function(57, pad_pwm1); /* PWM1 */
    gpio_function(58, pad_pwm2); /* PWM2 */
#elif defined(__FT930__)
    gpio_function(11, pad11_pwm0); /* PWM0 is also available at GPIOs 35 and 25 */
    gpio_function(10, pad10_pwm1); /* PWM1 is also available at GPIOs 36 and 24 */
    gpio_function(9, pad9_pwm2); /* PWM2 is also available at GPIO 23 */
#endif

    /* Initialise the PWM Subsystem... */
    pwm_init(
        1,                      /* Prescaler */
        0xFFFF,                 /* Max count */
        0);                     /* Shots (Infinite) */

    /* Set up PWM 0... */
    /* Set output levels */
    pwm_levels(
        0,                      /* Counter */
        pwm_state_high,         /* Initial State */
        pwm_restore_enable);    /* Rollover State */
    /* Set Compare value */
    pwm_compare(
        0,                      /* Counter */
        0x3FFF);                /* Compare Value */
    /* Make Counter 0 Toggle PWM0 */
    pwm_add_toggle(
        0,                      /* PWM Channel */
        0);                     /* Counter */

    /* Set up PWM 1... */
    /* Set output levels */
    pwm_levels(
        1,                      /* Counter */
        pwm_state_high,         /* Initial State */
        pwm_restore_enable);    /* Rollover State */
    /* Set Compare value */
    pwm_compare(
        1,                      /* Counter */
        0x7FFF);                /* Compare Value */
    /* Make Counter 1 Toggle PWM1 */
    pwm_add_toggle(
        1,                      /* PWM Channel */
        1);                     /* Counter */

    /* Set up PWM 2... */
    /* Set output levels */
    pwm_levels(
        2,                      /* Counter */
        pwm_state_high,         /* Initial State */
        pwm_restore_enable);    /* Rollover State */
    /* Set Compare value */
    pwm_compare(
        2,                      /* Counter */
        0xBFFF);                /* Compare Value */
    /* Make Counter 2 Toggle PWM2 */
    pwm_add_toggle(
        2,                      /* PWM Channel */
        2);                     /* Counter */

    /* Enable PWM... */
    pwm_enable();

}

void loop(void)
{
    /* Do Noting */
}
