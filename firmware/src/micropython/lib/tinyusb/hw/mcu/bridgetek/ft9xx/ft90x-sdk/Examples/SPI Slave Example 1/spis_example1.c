/**
  @file
  @brief
  SPI Slave Example 1

  Loopback between two MM900EVxA modules, FT900 (SPI Master) and FT900 (SPI Slave).

*/
/*
 * ============================================================================
 * History
 * =======
 * 3 Oct 2014 : Created
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

#define GPIO_SPIS_CLK	36
#define GPIO_SPIS_MISO	39
#define GPIO_SPIS_MOSI	38
#define GPIO_SPIS_SS	37

#elif defined(__FT930__)

#define GPIO_UART0_TX 	23
#define GPIO_UART0_RX 	22

#define GPIO_SPIS_CLK	34
#define GPIO_SPIS_MISO	35
#define GPIO_SPIS_MOSI	36
#define GPIO_SPIS_SS	37

#endif

void setup(void);
void loop(void);
void spis_dev_ISR(void);
void myputc(void* p, char c);

#define APP_BUFFER_SIZE (8)
#define APP_BUFFER_LOOP_CHUNK	(4)	//Check for min of this size and loopback

uint8_t buffer[APP_BUFFER_SIZE] = {0xF7,0xD1,0x04,0x03,0xde,0xad,0xbe,0xef};
#define SPIS_FIFOSIZE (64)

uint8_t* spis_dev_buffer;
uint8_t spis_dev_buffer_size;
uint8_t spis_dev_buffer_ptr;

/** Main entry point that contains initialization and loopback functionality */
int main(void)
{
    setup();
    for(;;)
    	loop();
    return 0;
}

/** Setup api to demonstrate initialization of SPI slave */
void setup(void)
{
	int32_t i;

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
        "Welcome to SPI Slave Example 1... \r\n"
        "\r\n"
        "Loopback use case between two FT900/FT930 (SPI Master) and FT900/FT930 (SPI Slave) \r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(NULL,myputc);

    /* Enable the SPI Slave device... */
    sys_enable(sys_device_spi_slave0);

    /* Set GPIOs to SPIS function */
    gpio_function(GPIO_SPIS_CLK, pad_spis0_clk);
    gpio_function(GPIO_SPIS_SS, pad_spis0_ss);
    gpio_function(GPIO_SPIS_MOSI, pad_spis0_mosi);
    gpio_function(GPIO_SPIS_MISO, pad_spis0_miso);

    /* Enable the SPI Slave Device... */
    spi_init(
        SPIS0,          /* Device */
        spi_dir_slave,  /* Direction */
        spi_mode_0,     /* SPI mode */
        4);             /* Ignored */

    /* Enable the FIFOs on the SPI Slave... */
    spi_option(SPIS0, spi_option_fifo, 1);
	spi_option(SPIS0,spi_option_fifo_size,SPIS_FIFOSIZE);
	spi_option(SPIS0,spi_option_fifo_receive_trigger,1);

	/* Global variables initialization */
    spis_dev_buffer = buffer;
    spis_dev_buffer_size = APP_BUFFER_SIZE;
    spis_dev_buffer_ptr = 0;

    /* Load the first 8 bytes into the SPIS0 buffer... */
	for(i=0;i<APP_BUFFER_SIZE;i++)
	{
		SPIS0->SPI_DATA = spis_dev_buffer[i];
	}

	/* Example use for interrupts - either interrupt mechanism shall be used or polling mechanism */
    //spi_enable_interrupts_globally(SPIS0);
    //interrupt_attach(interrupt_spis0, (uint8_t) interrupt_spis0, spis_dev_ISR);
    //interrupt_enable_globally();
}

/** loop - Api to demonstrate SPI slave 0 to receive and loopback the data to master */
/** First few bytes are predefined pattern followed by loopback of master data. Check
 * for the number of bytes available in receive fifo, read and write back the bytes received */

void loop(void)
{
	int32_t i;
	volatile int32_t rcvfifolen;
	volatile uint8_t rdwrbyte = 0;

	/* Check for the data present in the receive fifo */
	rcvfifolen = SPIS0->SPI_RCV_FIFO_COUNT;
	rcvfifolen = SPIS0->SPI_RCV_FIFO_COUNT;

	/* Check for the number of bytes received */
	if(rcvfifolen >= APP_BUFFER_SIZE )
	{
		int32_t bufflen = APP_BUFFER_SIZE;

		/* Read followed by write */
		for(i=0;i<bufflen;i++)
		{
			rdwrbyte  = SPIS0->SPI_DATA;
			SPIS0->SPI_DATA = rdwrbyte;
		}
	}
}

/* Example use case of interrupts */
void spis_dev_ISR(void)
{

    if (spi_is_interrupted(SPIS0, spi_interrupt_transmit_1bit_complete))
    {
    }
    if (spi_is_interrupted(SPIS0, spi_interrupt_data_ready))
    {
    }
    if (spi_is_interrupted(SPIS0, spi_interrupt_fault))
    {
    }
    if (spi_is_interrupted(SPIS0, spi_interrupt_transmit_empty))
    {
        /* Read in the arrived data */
        spis_dev_buffer[spis_dev_buffer_ptr] = SPIS0->SPI_DATA;

        /* Increment the buffer pointer */
        spis_dev_buffer_ptr++;
        if (spis_dev_buffer_ptr == spis_dev_buffer_size)
        {
            spis_dev_buffer_ptr = 0;
        }

        /* Set up the new data to send */
        SPIS0->SPI_DATA = spis_dev_buffer[spis_dev_buffer_ptr];
    }
}

void myputc(void* p, char c)
{
    uart_write(UART0, (uint8_t)c);
}
