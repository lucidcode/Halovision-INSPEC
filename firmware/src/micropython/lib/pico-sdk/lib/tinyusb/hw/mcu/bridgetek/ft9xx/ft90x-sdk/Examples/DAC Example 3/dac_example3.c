/**
  @file
  @brief
  DAC Example 3

  Output a series of values in continuous mode by interrupt.
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

static const uint16_t sin_wave[64] = {
#include "sin_wave.i"
};

void setup(void);
void loop(void);
void dacISR(void);

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
        "Welcome to DAC Example 3... \r\n"
        "\r\n"
        "Output a series of values in continuous mode by interrupt. \r\n"
        "The values will be output on DAC0\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable the DAC device... */
    sys_enable(sys_device_dac0);
    sys_enable(sys_device_dac1);

#if defined(__FT900__)
    /* Make GPIO13 function as DAC0... */
    gpio_function(13, pad_dac0); /* DAC0 */
    gpio_function(14, pad_dac1); /* DAC1 */
#endif

    /* Set the DAC to single shot mode */
    dac_mode(0, dac_mode_continuous);

    /* Set up the divider for the DAC clock... */
    dac_divider(64);

    /* Register the interrupt for the DAC... */
    interrupt_attach(interrupt_dac, 21, dacISR);

    /* Allow the DAC to set interrupt flags... */
    dac_enable_interrupt(0);

    /* Allow interrupts to fire... */
    interrupt_enable_globally();

    /* Preload the DAC with 128 Samples... (max FIFO size) */
    dac_writen(0, (uint16_t *) sin_wave, 64);
    dac_writen(0, (uint16_t *) sin_wave, 64);

    /* Start the DAC... */
    dac_start(0);
}

void loop(void)
{
    /* Do nothing */
}

/**
  Interrupt which loads more data into the DAC

  @warning This needs to happen as fast as possible. If you do not add more data
           to the DAC in time it will stop.
*/
void dacISR(void)
{
	/* The DAC is interrupted after 64 samples have been converted or when the FIFO is empty */
    if (dac_is_interrupted(0))
    {
        /* Write 64 samples to the DAC... */
        dac_writen(0, (uint16_t *) sin_wave, 64);
    }
}
