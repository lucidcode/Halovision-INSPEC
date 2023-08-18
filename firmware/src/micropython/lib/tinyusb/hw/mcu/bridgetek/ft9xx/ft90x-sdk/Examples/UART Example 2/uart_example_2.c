/**
  @file
  @brief
  UART Example 2

  This example program will echo back the input from UART0 onto UART0 in an
  interrupt function.
*/
/*
 * ============================================================================
 * History
 * =======
 * 30 Oct 2014 : Created
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
#include "ft900.h"

void setup(void);
void loop(void);
void uart0ISR(void);

int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void setup()
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
        "Welcome to UART Example 2... \r\n"
        "\r\n"
        "Any character typed here will be echoed back on the same serial port  \r\n"
        "via an interrupt \r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Attach the interrupt so it can be called... */
    interrupt_attach(interrupt_uart0, (uint8_t) interrupt_uart0, uart0ISR);
    /* Enable the UART to fire interrupts when receiving data... */
    uart_enable_interrupt(UART0, uart_interrupt_rx);
    /* Enable interrupts to be fired... */
    uart_enable_interrupts_globally(UART0);
    interrupt_enable_globally();
}

void loop()
{
    /* Do Nothing */
}

/**
 The Interrupt that allows echoing back over UART0
 */
void uart0ISR()
{
    static uint8_t c;

    if (uart_is_interrupted(UART0, uart_interrupt_rx))
    {
        /* Read a byte from the UART... */
        /* This call will block the CPU until a character is received */
        uart_read(UART0, &c);
        /* Write the byte out... */
        uart_write(UART0, c);
    }
}
