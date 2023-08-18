/**
  @file
  @brief
  ADC Example 5

  Read ADC 1 in single-shot Interrupt mode.
*/
/*
 * ============================================================================
 * History
 * =======
 * Created on: 19 Mar 2019
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
#include <stdbool.h>
#include <ft900.h>
#include "tinyprintf.h"

void setup(void);
void loop(void);
void adcISR(void);
void myputc(void* p, char c);

static uint16_t adcsample = 0;
static volatile int intr = 0;

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
        "Welcome to ADC Example 5... \r\n"
        "\r\n"
        "Read ADC 1 in single-shot interrupt mode.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(NULL,myputc);

    /* Enable the ADCs... */
    sys_enable(sys_device_adc);

    /* FT93x has dedicated ADC pins */
#if defined(__FT900__)
    /* Set GPIO6 to ADC1... */
    gpio_function(6, pad_adc1);
#endif

    /* single-shot read data... */
    adc_mode(adc_mode_single);

	/* Register the ADC interrupt... */
	interrupt_attach(interrupt_adc, (uint8_t) interrupt_adc, adcISR);
	interrupt_enable_globally();

	/* Enable the ADC to fire an interrupt... */
	adc_enable_interrupt();

}

void myputc(void* p, char c)
{
    uart_write(UART0, (uint8_t)c);
}

void adcISR()
{
    if (adc_is_interrupted())
    {
		intr=true;
		adc_read(&adcsample);
		tfp_printf("ADC 1 = %04d\r", adcsample);
    }
}

void loop(void)
{
	/* Start the ADC and use ADC1... */
	adc_start(1);
	delayus(1);
	/*ADC_START will be automatically cleared in the one-shot mode after the conversion is completed.*/
	while (!intr);
	intr = 0;
}
