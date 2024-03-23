/**
  @file main.c
  @brief
  DLOG project template.

*/
/*
 * ============================================================================
 * History
 * =======
 * 2017-01-12 : Created
 *
 * (C) Copyright 2014-2017, Bridgetek Pte Ltd
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
#include <ft900_dlog.h>

/**< To enable debug, change to #define. tinyprintf library should be added to project */
#define DEBUG

#ifdef DEBUG
#include "tinyprintf.h"
#define dbg(s,...)	tfp_printf ((s), ##__VA_ARGS__)
#else
#define dbg(s,...)
#endif


/**< global variable indicating start of dlog partition in flash */
extern __flash__ uint32_t __dlog_partition[];	

void myputc(void* p, char c);			/* required for tfp_printf library */
void setup(void);				/* setup UART communication */
int cmp (uint8_t *a, uint8_t *b, int len);	/* helper routine */
void dump (uint8_t *d, int len);		/* helper routine */
	
uint8_t	wrbuf[256] __attribute__((aligned(4)));	
uint8_t	rdbuf[256] __attribute__((aligned(4)));

int main(void)
{
	int	ret;
	int	pgsz;
	int	pages;
	int i, j;


	setup();

	ret = dlog_init (__dlog_partition, &pgsz, &pages);

	if (ret < 0)
	{
		dbg ("dlog_init: failed\n");
		return 0;
	}

	dbg ("dlog_init: passed, pgsz=0x%x, pages=%d\n", pgsz, pages);

	// erase the partition
	dlog_erase ();

	for (i = 0; i < pages; i++)
	{
		for (j = 0; j < pgsz; j++)
			wrbuf[j] = i;

		dlog_prog (i, (uint32_t *)wrbuf);
		
		for (j = 0; j < pgsz; j++)
			rdbuf[j] = 0;
		dlog_read (i, (uint32_t *)rdbuf);
		cmp (wrbuf, rdbuf, pgsz);
		dump (rdbuf, pgsz);
	}

	//dlog_erase ();
	dbg ("program ended\n");
	while (1);
	return 0;
}

int cmp (uint8_t *a, uint8_t *b, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		if (*a != *b)
		{
			dbg ("cmp: mismatch a[%d]=0x%02x, b[%d]=0x%02x\n", i, *a, i, *b);
			return -1;
		}
		a++, b++;
	}
	return 0;
}

void dump (uint8_t *d, int len)
{
	int	i;

	for (i = 0; i < len; i++)
	{
		if ((i & 0xF) == 0)
			dbg ("\n0x%02x: ", i);
		dbg ("%02x ", *d++);
	}
	dbg ("\n");
}


/** Function used to facilitate printf */
void myputc(void* p, char c)
{
    uart_write(p, (uint8_t)c);
}

void setup(void)
{
    /* Enable the UART Device... */
    sys_enable(sys_device_uart0);
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
        "(C) Copyright, Bridgetek Pte. Ltd. \r\n"
            "--------------------------------------------------------------------- \r\n"
            "Welcome to the Datalogger Example ...                                 \r\n"
        	"This example will erase the datalogger partition and fill all pages   \r\n"
        	"from 1 to 14 (14 pages) with repeated values of 0x00 to 0x0D and end. \r\n"
            "--------------------------------------------------------------------- \r\n"
        );

#ifdef DEBUG
    /* Enable dbg() functionality... */
    init_printf(UART0,myputc);
#endif

}


/* end */
