/**
  @file usbh_cdc_test_1.c
  @brief
  USB CDC Device Test

  This example program will bridge the UART to an CDC detected on the USB.
*/
/*
 * ============================================================================
 * History
 * =======
 * GDM 21 Jan 2015 : Created
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

/* INCLUDES ************************************************************************/

#include <stdint.h>

#include <ft900.h>
#include <ft900_usb.h>
#include "ft900_usbhx.h"
#include <ft900_usbh_cdcacm.h>
#include <ft900_startup_dfu.h>

#include "tinyprintf.h"

/* CONSTANTS ***********************************************************************/

/**
 @brief Change this value in order to modify the size of the Tx and Rx ring buffers
  used to implement UART buffering.
 */
#define RINGBUFFER_SIZE (128)

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
 @name UART ring buffer variables.
 @brief Variables required to implement an Rx and Tx ring buffer for the UART.
 */
//@{
static uint8_t uart0BufferIn_data[RINGBUFFER_SIZE];
static volatile uint16_t uart0BufferIn_wr_idx = 0;
static volatile uint16_t uart0BufferIn_rd_idx = 0;
static volatile uint16_t uart0BufferIn_avail = RINGBUFFER_SIZE;
static uint8_t uart0BufferOut_data[RINGBUFFER_SIZE];
static volatile uint16_t uart0BufferOut_wr_idx = 0;
static volatile uint16_t uart0BufferOut_rd_idx = 0;
static volatile uint16_t uart0BufferOut_avail = RINGBUFFER_SIZE;
//@}

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

size_t uart0Tx(uint8_t *data, size_t len);
size_t uart0Rx(uint8_t *data, size_t len);
void uart0ISR(void);

/** @name tfp_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
void tfp_putc(void* p, char c)
{
    uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}

void ISR_timer(void)
{
    if (timer_is_interrupted(timer_select_a))
    {
    	// Call USB timer handler to action transaction and hub timers.
   		USBH_timer();
    }
}

/**
 Transmit a buffer of data over UART0 asynchronously.

 @return The number of bytes written to the buffer
 */
size_t uart0Tx(uint8_t *data, size_t len)
{
	signed int start;
    size_t copied = 0;

	// Determine if we need to start a transmission or there is one
    // already in action.
    CRITICAL_SECTION_BEGIN
    start = uart0BufferOut_wr_idx - uart0BufferOut_rd_idx;
    CRITICAL_SECTION_END

	CRITICAL_SECTION_BEGIN
	// Copy in as much data from the producer as there is space for
	// in the out buffer.
    while (len--)
    {
    	// Check there is space there to receive the data from the producer.
		if (uart0BufferOut_avail > 0)
		{
			// Reduce the number of available bytes.
			uart0BufferOut_avail--;

			// Add byte of data to the buffer.
			uart0BufferOut_data[uart0BufferOut_wr_idx] = *data++;

			// Increment the pointer and wrap around.
			uart0BufferOut_wr_idx++;
			if (uart0BufferOut_wr_idx == RINGBUFFER_SIZE) uart0BufferOut_wr_idx = 0;

			// Tally up number of bytes actually transmitted.
			copied++;
		}
    }
    CRITICAL_SECTION_END

    // Start new a transmission if nothing is being transmitted.
    if (start == 0)
    {
    	uint8_t c;

    	CRITICAL_SECTION_BEGIN

		// Get data from the out buffer.
        c = uart0BufferOut_data[uart0BufferOut_rd_idx];

        // Increment the consumer pointer and wrap around.
        uart0BufferOut_rd_idx++;
        if (uart0BufferOut_rd_idx == RINGBUFFER_SIZE) uart0BufferOut_rd_idx = 0;

        // Byte has been sent, update available bytes value.
		uart0BufferOut_avail++;

		CRITICAL_SECTION_END

		// Send data to the UART. This will result in a TX interrupt when
		// it completes.
        uart_write(UART0, c);
    }

    return copied;
}

/**
 Receive a number of bytes from UART0

 @return The number of bytes read from UART0
 */
size_t uart0Rx(uint8_t *data, size_t len)
{
	size_t copied = 0;

    /* Copy in as much data as we can ...
       This can be either the maximum size of the buffer being given
       or the maximum number of bytes available in the Serial Port
       buffer */
	while(len--)
	{
		CRITICAL_SECTION_BEGIN
		if (uart0BufferIn_avail < RINGBUFFER_SIZE)
		{
			uart0BufferIn_avail++;

			*data = uart0BufferIn_data[uart0BufferIn_rd_idx];
			data++;

			/* Increment the pointer and wrap around */
			uart0BufferIn_rd_idx++;
			if (uart0BufferIn_rd_idx == RINGBUFFER_SIZE) uart0BufferIn_rd_idx = 0;

			copied++;
		}
		CRITICAL_SECTION_END
	}

    /* Report back how many bytes have been copied into the buffer...*/
    return copied;
}

/**
 The Interrupt which handles asynchronous transmission and reception
 of data into the ring buffer
 */
void uart0ISR()
{
    static uint8_t c;

    /* Transmit interrupt... */
    if (uart_is_interrupted(UART0, uart_interrupt_tx))
    {
        /* Check to see how much data we have to transmit... */
        //avail = uart0BufferOut_rd_idx - uart0BufferOut_wr_idx;

    	if (uart0BufferOut_avail < RINGBUFFER_SIZE)
    	{
    		uart0BufferOut_avail++;

    		/* Copy out the byte to be transmitted so that the uart_write is
               the last thing we do... */
            c = uart0BufferOut_data[uart0BufferOut_rd_idx];

            /* Increment the pointer and wrap around */
            uart0BufferOut_rd_idx++;
            if (uart0BufferOut_rd_idx == RINGBUFFER_SIZE) uart0BufferOut_rd_idx = 0;

            /* Write out a new byte, the following Transmit interrupt should handle
               the remaning bytes... */
            uart_write(UART0, c);
        }
    }

    /* Receive interrupt... */
    if (uart_is_interrupted(UART0, uart_interrupt_rx))
    {
        /* Read a byte into the Ring Buffer... */
        uart_read(UART0, &c);

        // Do not overwrite data already in the buffer.
        if (uart0BufferIn_avail > 0)
        {
        	uart0BufferIn_avail--;

        	uart0BufferIn_data[uart0BufferIn_wr_idx] = c;

			/* Increment the pointer and wrap around */
			uart0BufferIn_wr_idx++;
			if (uart0BufferIn_wr_idx == RINGBUFFER_SIZE) uart0BufferIn_wr_idx = 0;
        }
    }
}

void cdc_uart_bridge(USBH_CDCACM_context *ctx)
{
    int8_t status;
    static uint8_t bufUart2Usb[RINGBUFFER_SIZE] = {0};
    static uint8_t bufUsb2Uart[RINGBUFFER_SIZE] = {0};
    size_t read_bytes;
    int8_t ns, ds;
    int i;

	tfp_printf("Sending encapsulated reset...");
    status = USBH_CDCACM_send_encapsulated_command(ctx, "ATZ", 3);
    if (status == USBH_CDCACM_OK)
    {
		do
		{
			USBH_process();
		}while (!USBH_CDCACM_get_response_available(ctx));
		tfp_printf("\r\n");
		// Big enough buffer for "\r\nOK\r\n" or "\r\nERROR\r\n"
		USBH_CDCACM_get_encapsulated_response(ctx, (char *) bufUsb2Uart, 10);
		tfp_printf("Received %s\r\n", bufUsb2Uart);
    }
    else
    {
    	tfp_printf(" not supported %d\r\nSending reset to DATA interface...", status);
    	USBH_CDCACM_write(ctx, (uint8_t *) "ATZ\r\n", 5);
		do
		{
			USBH_process();
			// Big enough buffer for "\r\nOK\r\n" or "\r\nERROR\r\n"
			read_bytes = USBH_CDCACM_read(ctx, bufUsb2Uart, 10);
			for (i = 0; read_bytes > 0; i++, read_bytes--)
			{
				if ((bufUsb2Uart[i] == 'O') && (bufUsb2Uart[i+1] == 'K'))
					break;
			}
		}while (read_bytes == 0);
		tfp_printf("Received OK\r\n");

		status = USBH_CDCACM_OK;
    }

	do
	{
		// Update USBH status and perform callbacks to cdc_recv if required.
		USBH_process();

		// Read in a packet of data from the UART.
		// (up to the size of the ring buffer)
		read_bytes = uart0Rx(bufUart2Usb, 64);
		if (read_bytes)
		{
			// Write it to the CDC DATA interface.
			USBH_CDCACM_write(ctx, bufUart2Usb, read_bytes);
		}

		// Read in a packet of data from the CDC DATA interface.
		read_bytes = USBH_CDCACM_read(ctx, bufUsb2Uart, 64);
		if (read_bytes)
		{
			// Write it out to the UART.
			uart0Tx(bufUsb2Uart, read_bytes);
		}

		// Check the notification status and data status for the CDC
		// device is OK or quit the bridging loop.
		USBH_CDCACM_get_poll_status(ctx, &ns, &ds);
		if (ns || ds)
		{
			tfp_printf("Status: %d %d\r\n", ns, ds);
			if (ns != USBH_OK)
				status = USBH_CDCACM_ERR_NOTIFICATION_ENDPOINT;
			if (ds != USBH_OK)
				status = USBH_CDCACM_ERR_DATA_ENDPOINT;

			break;
		}

	}
	while (status == USBH_CDCACM_OK);
}

int8_t scan_for_cdc_acm(USBH_device_handle hDev)
{
    uint8_t usbStatus, cdcStatus;
    USBH_interface_handle hInterfaceControl;
    USBH_CDCACM_context ctx;
    uint8_t flags = 0; // USBH_CDCACM_FLAG_NO_NOTIFICATION

        // Start at first interface on device.
    hInterfaceControl = 0;

    // USBH_CDCACM_OK is equal to USBH_OK
    cdcStatus = USBH_CDCACM_ERR_PARAMETER;
	do {
		usbStatus = USBHX_find_by_class(&hDev, &hInterfaceControl,
				USB_CLASS_CDC_CONTROL, USB_SUBCLASS_CDC_CONTROL_ABSTRACT, 0xFF);

		if (usbStatus == USBH_OK)
		{
			cdcStatus = USBH_CDCACM_init(hInterfaceControl, flags, &ctx);
			tfp_printf("Beginning CDC ACM testing %d %d\r\n", cdcStatus, usbStatus);
			if (cdcStatus == USBH_CDCACM_OK)
			{
				break;
			}
		}
	} while (usbStatus == USBH_OK);

	if (cdcStatus == USBH_OK)
	{
		tfp_printf("Beginning CDC ACM testing\r\n");
		cdc_uart_bridge(&ctx);
	}
	else
	{
		tfp_printf("No CDC ACM devices found\r\n");
	}

	return usbStatus;
}

uint8_t usbh_testing(void)
{
    uint8_t status;
    USBH_device_handle hRootDev;
    USBH_STATE connect;

    USBH_initialise(NULL);

    while (1)
    {
    	USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		if (connect == USBH_STATE_NOTCONNECTED)
		{
			tfp_printf("\r\nPlease plug in a USB Device\r\n");

			// Detect usb device connect
			do
			{
				status = USBH_process();
				USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
			} while (connect == USBH_STATE_NOTCONNECTED);
		}
		tfp_printf("\r\nUSB Device Detected\r\n");

		do {
			status = USBH_process();
			USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		} while (connect < USBH_STATE_ENUMERATED);

		if (connect == USBH_STATE_ENUMERATED)
		{
			tfp_printf("USB Devices Enumerated\r\n");
		}
		else
		{
			tfp_printf("USB Devices Partially Enumerated\r\n");
		}

		// Get the first device (device on root hub)
		status = USBH_get_device_list(USBH_ROOT_HUB_HANDLE, &hRootDev);
		if (status != USBH_OK)
		{
			// Report the error code.
			tfp_printf("%d\r\n", status);
		}
		else
		{
			// Perform the actual testing.
			scan_for_cdc_acm(hRootDev);
		}

		tfp_printf("\r\nPlease remove the USB Device\r\n");
		// Detect usb device disconnect
		do
		{
			status = USBH_process();
			USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		} while (connect != USBH_STATE_NOTCONNECTED);
	}

    return 0;
}

/* FUNCTIONS ***********************************************************************/

int main(int argc, char *argv[])
{
	/* Check for a USB device connection and initiate a DFU firmware download or
	   upload operation. This will timeout and return control here if no host PC
	   program contacts the device's DFU interace. USB device mode is disabled
	   before returning.
	*/
	STARTUP_DFU();

	/* Enable the UART Device... */
    sys_enable(sys_device_uart0);
    /* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
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
    	);

    /* Enable tfp_printf() functionality... */
    init_printf(UART0, tfp_putc);

    /* Attach the interrupt so it can be called... */
    interrupt_attach(interrupt_uart0, (uint8_t) interrupt_uart0, uart0ISR);
    /* Enable the UART to fire interrupts when receiving data... */
    uart_enable_interrupt(UART0, uart_interrupt_rx);
    /* Enable the UART to fire interrupts when transmitting data... */
    uart_enable_interrupt(UART0, uart_interrupt_tx);
    /* Enable interrupts to be fired... */
    uart_enable_interrupts_globally(UART0);

    /* Timer A = 1ms */
	timer_prescaler(timer_select_a, 1000);
    timer_init(timer_select_a, 1000, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
    timer_enable_interrupt(timer_select_a);
    timer_start(timer_select_a);

    interrupt_enable_globally();

    tfp_printf("Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to USBH CDC Tester Example 1... \r\n"
        "\r\n"
        "Bridge data from the UART to a CDC ACM device on the USB host port.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    usbh_testing();

    interrupt_detach(interrupt_timers);
    interrupt_disable_globally();

    sys_disable(sys_device_timer_wdt);

    // Wait forever...
    for (;;) {};

	return 0;
}


