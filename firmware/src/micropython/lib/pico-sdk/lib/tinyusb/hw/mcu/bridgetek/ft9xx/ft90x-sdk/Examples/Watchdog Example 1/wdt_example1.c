/**
  @file
  @brief
  WDT Example 1

  Kick the watchdog for 10 seconds then let it expire.
*/
/*
 * ============================================================================
 * History
 * =======
 * 2 Oct 2014 : Created
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
#include <ft900_dlog.h>
#include "tinyprintf.h"

void setup(void);
void loop(void);
void myputc(void* p, char c);
void dump (uint8_t *d, int len);
void watchdog_ISR(void);

/* Soft ISR Vector table as defined in crt0.S */
extern isrptr_t vector_table[N_INTERRUPTS];

/**< global variable indicating start of dlog partition in flash */
extern __flash__ uint32_t __dlog_partition[];
int	dlog_hdl = -1;
int	pgsz = 0;
int	pages = 0;
/* Buffers to save the current context of application (dummy) at the
 * time of watchdog level1 timeout
 */
#define DLOG_PAGE_FOR_CPU_CONTEXT	1
uint8_t	wrbuf[256] __attribute__((aligned(4)))= {0};
uint8_t	rdbuf[256] __attribute__((aligned(4)))= {0};

int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void setup(void)
{
	volatile uint8_t reset_status;
	uint8_t read_last_context = 0;

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
        "---------------------------------------------------------------------------- \r\n"
        "Welcome to WDT Example 1... \r\n"
        );

#if defined(__FT930__)
	uart_puts(UART0,
			"Saves current context(dummy) to a DLOG page on first level watchdog timeout."
			"\n"
			"---------------------------------------------------------------------------- \n"
			);
    /* Check for watchdog second level reset */
    reset_status = SYS_RST->RESET_STATUS;
    if (reset_status & MASK_SYS_GLOBAL_RST_WDG_RESET)
    {
    	SYS_RST->RESET_STATUS = MASK_SYS_GLOBAL_RST_WDG_RESET;
    	uart_puts(UART0,"!!!WATCHDOG Power-On-Reset!!!\n");
    	read_last_context = 1;
    }
    if (reset_status & MASK_SYS_GLOBAL_RST_SOFT_RESET)
    {
    	SYS_RST->RESET_STATUS = ~(MASK_SYS_GLOBAL_RST_SOFT_RESET);
    	uart_puts(UART0,"!!!SOFT Power-On-Reset!!!\n");
    }
    if (reset_status & MASK_SYS_GLOBAL_RST_HW_RESET)
    {
    	SYS_RST->RESET_STATUS = MASK_SYS_GLOBAL_RST_HW_RESET;
    	uart_puts(UART0,"!!!EXTERNAL HW Power-On-Reset!!!\n");
    }
#else
	if (!sys_check_ft900_revB()) //90x series rev C
	{
		uart_puts(UART0,
		    	"Saves current context(dummy) to a DLOG page on first level watchdog timeout."
		        "\n"
		        "---------------------------------------------------------------------------- \n"
		        );
	    /* clearing watchdog status after POR */
	    reset_status = TIMER->TIMER_WDG_STATUS;
		if (reset_status &  MASK_TIMER_WDG_STATUS)
		{
			uart_puts(UART0,"!!!WATCHDOG Power-On-Reset!!!\n");
			read_last_context = 1;
		}
		reset_status = SYS_RST->RESET_STATUS;
		if (reset_status & MASK_SYS_GLOBAL_RST_SOFT_RESET)
		{
			SYS_RST->RESET_STATUS = ~(MASK_SYS_GLOBAL_RST_SOFT_RESET);
			uart_puts(UART0,"!!!SOFT Power-On-Reset!!!\n");
		}
		if (reset_status & MASK_SYS_GLOBAL_RST_HW_RESET)
		{
			SYS_RST->RESET_STATUS = MASK_SYS_GLOBAL_RST_HW_RESET;
			uart_puts(UART0,"!!!EXTERNAL HW Power-On-Reset!!!\n");
		}
	}
	else
	{
		uart_puts(UART0,
		        "---------------------------------------------------------------------------- \n"
		        );
		read_last_context = 1;
	}
#endif
    interrupt_disable_globally();
    /* Enable tfp_printf() functionality... */
    init_printf(NULL,myputc);
    
	dlog_hdl = dlog_init (__dlog_partition, &pgsz, &pages);
    if (read_last_context)
    {
		if (dlog_hdl == 0)
		{
			dlog_read (DLOG_PAGE_FOR_CPU_CONTEXT, (uint32_t *)rdbuf);
			dump (rdbuf, pgsz);
		}
		else
		{
			tfp_printf ("DLOG init failed!\n");
		}
    }

    tfp_printf("Wait for 10 Seconds before starting the test\r\n");
    delayms(10000);

    /* attaching watchdog interrupt */
    //vector_table[33] = watchdog_ISR;
    interrupt_attach(interrupt_wdg, interrupt_wdg, watchdog_ISR);
    /* Enable the WDT... */
    sys_enable(sys_device_timer_wdt);

    /* Set the watchdog to elapse after ~1.28 seconds... */
    wdt_init(wdt_counter_128M_clocks);
   
}

void loop(void)
{
    static unsigned int n_kicks = 0;

    if (n_kicks < 10)
    {
        wdt_kick();
        n_kicks++;

        tfp_printf("WDT Kick %d\r\n", n_kicks);
    }
    delayms(1000);
}

void myputc(void* p, char c)
{
    uart_write(UART0, (uint8_t)c);
}


void dump (uint8_t *d, int len)
{
	int	i;

	tfp_printf("last saved context on a watchdog reset: \n");
	for (i = 0; i < len; i++)
	{
		if ((i & 0xF) == 0)
			tfp_printf("\n0x%02x: ", i);
		tfp_printf("%02x ", *d++);
	}
	tfp_printf("\n");
}

void watchdog_ISR(void)
{
	TIMER->TIMER_CONTROL_2 |= MASK_TIMER_CONTROL_2_WDG_INT;
	uart_puts(UART0,"Level 1 watchdog timeout...\n");
    //saving dummy context in a dlog page
	{
		if (dlog_hdl == 0)
		{
			// erase the partition
			dlog_erase ();
			for (int j = 0; j < pgsz; j++)
						wrbuf[j] = rdbuf[j]+1;
			// write the dummy context
			dlog_prog (DLOG_PAGE_FOR_CPU_CONTEXT, (uint32_t *)wrbuf);
		}
	}
#if defined(__FT900__)
	if (sys_check_ft900_revB()) //90x series rev B
	{
		//reboot the chip as no second level timeout
		chip_reboot();
	}
	else
	{
		//wait until second timeout
		while (1);
	}
#else
	//wait until second timeout
	while (1);
#endif
}
