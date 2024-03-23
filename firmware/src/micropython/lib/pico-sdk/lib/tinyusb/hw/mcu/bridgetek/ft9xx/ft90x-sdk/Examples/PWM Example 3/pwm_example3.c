/**
  @file
  @brief
  PWM Example 3

  Output a sine wave on the PWM audio channel (PWM0 and PWM1)
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

#define COUNT_UP_STEP (100)
#define COUNT_DOWN_STEP (100)

void setup(void);
void loop(void);
void pwmpcmISR(void);

static const int16_t sin_wave[64*2] = {
#include "sin_wave_stereo_signed.i"
};

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
        "Welcome to PWM Example 3... \r\n"
        "\r\n"
        "Output a sine wave on the PWM audio channel (PWM0 and PWM1)\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable the PWM subsystem */
    sys_enable(sys_device_pwm);

    /* Set up pins for PWM */
#if defined(__FT900__)
    gpio_function(56, pad_pwm0); /* PWM0 */
    gpio_function(57, pad_pwm1); /* PWM1 */
#elif defined(__FT930__)
    gpio_function(11, pad11_pwm0); /* PWM0 is also available at GPIOs 35 and 25 */
    gpio_function(10, pad10_pwm1); /* PWM1 is also available at GPIOs 36 and 24 */
#endif

    /* Set the PWM channel to operate at the correct sample rate... */
    pwm_init(
        0,                          /* Prescaler 100 MHz / 22050 Hz = 4535 */
        (100000000UL/22050UL),      /* Max count */
        0);                         /* Shots (Infinite) */

    /* Setup the PWM PCM output... */
    pwm_pcm_open(
        pwm_pcm_channels_stereo,    /* Channels */
        22050,                      /* Sample Rate */
        pwm_pcm_data_size_16,       /* Data width */
        pwm_pcm_endianness_big,     /* Endianness */
        pwm_pcm_filter_on);         /* PCM Filter */

    /* Set the volume to max... */
    pwm_pcm_volume(pwm_pcm_volume_100);

    /* Register the PWM PCM interrupt... */
    interrupt_attach(interrupt_pwm, (uint8_t) interrupt_pwm, pwmpcmISR);
    /* Set up all the PWM PCM interrupts... */
    pwm_pcm_enable_interrupt(pwm_pcm_interrupt_empty);
    pwm_pcm_enable_interrupt(pwm_pcm_interrupt_half_full);
    pwm_pcm_enable_interrupt(pwm_pcm_interrupt_underflow);

    interrupt_enable_globally();

    /* Start PWM Output... */
    pwm_enable();
}

void loop(void)
{
    /* Do Nothing */
}

void pwmpcmISR(void)
{
    static uint8_t offset = 0;

    if (pwm_pcm_is_interrupted(pwm_pcm_interrupt_empty) ||
        pwm_pcm_is_interrupted(pwm_pcm_interrupt_underflow))
    {
        pwm_pcm_writen((uint16_t *) sin_wave, 64);
    }

    if (pwm_pcm_is_interrupted(pwm_pcm_interrupt_half_full))
    {
        pwm_pcm_writen((uint16_t *) sin_wave + offset, 32);

        if (offset == 0) offset = 32;
        else if (offset == 32) offset = 64;
        else if (offset == 64) offset = 96;
        else offset = 0;
    }

}
