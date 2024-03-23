/**
  @file
  @brief
  SPI Master Example 1

  Loopback between two MM900EVxA modules, FT900 (SPI Master) and FT900 (SPI Slave)

*/
/*
 * ============================================================================
 * History
 * =======
 * 12 Nov 2014 : Created
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

#define GPIO_SPIM_CLK	27
#define GPIO_SPIM_MISO	30
#define GPIO_SPIM_MOSI	29
#define GPIO_SPIM_SS0	28

#elif defined(__FT930__)

#define GPIO_UART0_TX 	23
#define GPIO_UART0_RX 	22

#define GPIO_SPIM_CLK	34
#define GPIO_SPIM_MISO	35
#define GPIO_SPIM_MOSI	36
#define GPIO_SPIM_SS0	30

// Use GPIO 30 as SS0
#define pad_spim_ss0 	pad30_spim_ss0

#endif

#define DATASIZETESTING	(8)

void setup(void);
void loop(void);

void myputc(void* p, char c);


/** Main Program */
int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

/** Setup */
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
        "Welcome to SPI Master Example 1... \r\n"
        "\r\n"
        "Loopback use case between two MM900EVxA/FT930 Mini module, FT900/FT930 (SPI Master) and FT900/FT930 (SPI Slave) \r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(UART0,myputc);

    /* Enable the SPI Master device... */
    sys_enable(sys_device_spi_master);

    /* Initialize the pin functions and directions */
    gpio_function(GPIO_SPIM_CLK, pad_spim_sck);
    gpio_function(GPIO_SPIM_SS0, pad_spim_ss0);
    gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi);
    gpio_function(GPIO_SPIM_MISO, pad_spim_miso);

    /* CS high */
    gpio_write(GPIO_SPIM_SS0, 1);

    /* Init/configure SPI master with fifo enabled and depth of 64 bytes */
    spi_init(SPIM, spi_dir_master, spi_mode_0, 64);
	spi_option(SPIM,spi_option_fifo_size,64);
	spi_option(SPIM,spi_option_fifo,1);
	spi_option(SPIM,spi_option_fifo_receive_trigger,1);

	usleep(30000);
}

/** Loop - Api to demonstrate exchange of SPI data between SPI master and slave */
/** Two MM900EVxA modules are connected via SPI master and SPI slave */
void loop(void)
{
	uint8_t test_data_transfer[DATASIZETESTING] = {1,2,3,4,5,6,7,8},test_data_receive[DATASIZETESTING];
	int16_t i;
	unsigned char tempvar = 0;

	/* Note - First few bytes from slave may be pre defined data */
    while(1)
	{
    	/* Initialize the transfer buffer with predefined data */
		for(i=0;i<DATASIZETESTING;i++)
		{
			test_data_transfer[i] = tempvar++;
		}

		/* Exchange data between master and slave */
		spi_open(SPIM, 0);
		spi_xchangen(SPIM,test_data_receive,test_data_transfer,DATASIZETESTING);
		spi_close(SPIM, 0);

		/* Print the data sent from master and received from Slave */
		tfp_printf(" \r\n Data sent and received");
		for(i=0;i<DATASIZETESTING;i++)
		{
			tfp_printf(" %x %x ",test_data_transfer[i],test_data_receive[i]);
		}
		/* Delay of 30ms for print statements */
		usleep(30000);
	}
}

void myputc(void* p, char c)
{
    uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}
