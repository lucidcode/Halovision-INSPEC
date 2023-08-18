/**
  @file gpio_dfu.c
  @brief
  GPIO DFU Example

  Use interrupts to trigger a DFU update. Also allow a UART input to trigger
  a DFU update.
*/
/*
 * ============================================================================
 * History
 * =======
 * 19 Nov 2014 : Created
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
#include <ft900_startup_dfu.h>

#define STR(a) STR_(a)
#define STR_(a) #a

#if defined(__FT930__)
#define GPIO_PIN_1 19
#define GPIO_PIN_2 20
#define GPIO_PIN_3 21
#else
#define GPIO_PIN_1 18
#define GPIO_PIN_2 17
#define GPIO_PIN_3 16
#endif

void setup(void);
void loop(void);
void ISR_gpio(void);
void ISR_uart(void);
void enable_uart(void);
void enable_gpio(void);
void enable_interrupt(void);

volatile int trigger = 0;

int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void setup()
{
	enable_uart();
	enable_gpio();

    /* Print out a welcome message... */
	uart_puts(UART0, "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
            "(C) Copyright, Bridgetek Pte Ltd \r\n"
			"--------------------------------------------------------------------- \r\n"
			"Welcome to GPIO DFU Example... \r\n"
			"\r\n"
			"Start a DFU interface on the USB Device Port when a GPIO is pulled low\r\n"
			"---------------------------------------------------------------------- \r\n"
	);

    uart_puts(UART0, "On GPIO trigger GPIO"STR(GPIO_PIN_1)" for infinite DFU timeout\r\n"
    				 "                GPIO"STR(GPIO_PIN_2)" for 5 seconds DFU timeout\r\n"
    				 "                GPIO"STR(GPIO_PIN_3)" for default timeout\r\n");
    uart_puts(UART0, "On UART press <CR> for infinite DFU timeout\r\n"
    				 "              <space> for 5 seconds DFU timeout\r\n"
    				 "              Any other key for default timeout\r\n");
    enable_interrupt();
}

void loop()
{
	if (trigger)
	{
		if (trigger == 1)
		{
			uart_puts(UART0,"Starting DFU - never to return\r\n");
			STARTUP_DFU(0);
		}
		else if (trigger == 2)
		{
			uart_puts(UART0,"Starting DFU - for 5 seconds\r\n");
			STARTUP_DFU(5000);
		}
		else if (trigger == 3)
		{
			uart_puts(UART0,"Starting DFU - with the default timeout\r\n");
			STARTUP_DFU();
		}

		// Re-enable UART and Interrupt. These are disabled by the STARTUP_DFU
		// function.
		enable_uart();
		enable_gpio();
		enable_interrupt();

		trigger = 0;

		uart_puts(UART0,"Finished DFU\r\n");
	}
}

void ISR_gpio()
{
    if (gpio_is_interrupted(GPIO_PIN_1))
    {
        uart_puts(UART0,"GPIO"STR(GPIO_PIN_1)" interrupted\r\n");
        trigger = 1;
    }
    if (gpio_is_interrupted(GPIO_PIN_2))
    {
        uart_puts(UART0,"GPIO"STR(GPIO_PIN_2)" interrupted\r\n");
        trigger = 2;
    }
    if (gpio_is_interrupted(GPIO_PIN_3))
    {
        uart_puts(UART0,"GPIO"STR(GPIO_PIN_3)" interrupted\r\n");
        trigger = 3;
    }
}

void ISR_uart()
{
	unsigned char c;
	if (uart_is_interrupted(UART0, uart_interrupt_rx))
	{
        uart_read(UART0, &c);
        if (c == '\r')
        {
            uart_puts(UART0,"UART <CR> interrupted\r\n");
        	trigger = 1;
        }
        else if (c == ' ')
        {
            uart_puts(UART0,"UART <space> interrupted\r\n");
        	trigger = 2;
        }
        else
        {
            uart_puts(UART0,"UART interrupted\r\n");
        	trigger = 3;
        }
	}
}

void enable_interrupt()
{
    /* Attach the interrupt so it can be called... */
    interrupt_attach(interrupt_uart0, (uint8_t) interrupt_uart0, ISR_uart);
    /* Enable the UART to fire interrupts when receiving data... */
    uart_enable_interrupt(UART0, uart_interrupt_rx);
    /* Enable interrupts to be fired... */
    uart_enable_interrupts_globally(UART0);
    /* Attach the interrupt so it can be called... */
	interrupt_attach(interrupt_gpio, (uint8_t)interrupt_gpio, ISR_gpio);
    /* Enable the GPIO to fire an interrupt for the GPIO pin. */
	gpio_interrupt_enable(GPIO_PIN_1, gpio_int_edge_falling);
	gpio_interrupt_enable(GPIO_PIN_2, gpio_int_edge_falling);
	gpio_interrupt_enable(GPIO_PIN_3, gpio_int_edge_falling);
    /* Enable interrupt handling. */
	interrupt_enable_globally();
}

void enable_uart(void)
{
	/* Enable the UART Device... */
    sys_enable(sys_device_uart0);
#if defined(__FT930__)
    /* Make GPIO23 function as UART0_TXD and GPIO22 function as UART0_RXD... */
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#else
	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#endif
    uart_open(UART0,                    /* Device */
              1,                        /* Prescaler = 1 */
              UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */

}

void enable_gpio(void)
{
	/* Set up the pin */
	gpio_dir(GPIO_PIN_1, pad_dir_input);
	gpio_dir(GPIO_PIN_2, pad_dir_input);
	gpio_dir(GPIO_PIN_3, pad_dir_input);
	gpio_pull(GPIO_PIN_1, pad_pull_pullup);
	gpio_pull(GPIO_PIN_2, pad_pull_pullup);
	gpio_pull(GPIO_PIN_3, pad_pull_pullup);
}
