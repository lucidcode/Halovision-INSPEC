/**
  @file usbh_ft232_test_1.c
  @brief
  USB FT232 Device Test

  This example program will bridge the UART to an FT232 detected on the USB.
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
#include <ft900_timers.h>
#include <ft900_sys.h>
#include <ft900_interrupt.h>
#include <ft900_gpio.h>
#include <ft900_usb.h>
#include <ft900_usbh.h>
#include <ft900_usbhx.h>
#include <ft900_usbh_ft.h>
#include <ft900_startup_dfu.h>

/* UART support for printf output. */
#include <ft900_uart_simple.h>
#include "../tinyprintf/tinyprintf.h"

/* CONSTANTS ***********************************************************************/

/**
 @brief Change this value in order to modify the size of the Tx and Rx ring buffers
  used to implement UART buffering.
 */
#define RINGBUFFER_SIZE (128)

/**
 @brief FT232 configuration settings.
 */
#define FT232_FLOW_CONTROL /*USB_FT232_SETFLOWCTRL_DTR_DSR*/ USB_FT232_SETFLOWCTRL_RTS_CTS
#define FT232_BAUD_RATE 19200
#define FT232_DATA USB_FT232_SETDATA_8_BIT
#define FT232_PARITY USB_FT232_SETDATA_NOPAR
#define FT232_STOP USB_FT232_SETDATA_1_STOP

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

void ft232_uart_bridge(USBH_FT232_context *ctx)
{
    int8_t status = USBH_FT232_OK;
    static uint8_t bufUart2Usb[RINGBUFFER_SIZE] = {0};
    static uint8_t bufUsb2Uart[RINGBUFFER_SIZE] = {0};
    uint8_t read_bytes;
    int8_t ds;
    uint16_t lastModemStatus = 0;

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
			USBH_FT232_write(ctx, bufUart2Usb, read_bytes);
		}

		// Read in a packet of data from the CDC DATA interface.
		read_bytes = USBH_FT232_read(ctx, bufUsb2Uart, 64);
		if (read_bytes)
		{
			// Write it out to the UART.
			uart0Tx(bufUsb2Uart, read_bytes);
		}

		// Check the notification status and data status for the CDC
		// device is OK or quit the bridging loop.
		USBH_FT232_get_poll_status(ctx, &ds);
		if (ds)
		{
			tfp_printf("Status: %d\r\n", ds);
			if (ds != USBH_OK)
				status = USBH_FT232_ERR_DATA_ENDPOINT;

			break;
		}
		else
		{
			if (lastModemStatus != ctx->lastModemStatus)
			{
				lastModemStatus = ctx->lastModemStatus;
				tfp_printf("Modem Status: %04x\r\n", lastModemStatus); //Event notification
			}
		}

	}
	while (status == USBH_FT232_OK);
}

uint8_t hub_scan_for_ft(USBH_device_handle hDev)
{
    uint8_t ftStatus, usbStatus;
    USBH_interface_handle hInterface;
	uint16_t usbVid;
    uint16_t usbPid;
    USBH_FT232_context ctx;
    uint8_t flags = 0;
    char *type;

    // CDCACM_OK is equal to USBH_OK
    ftStatus = USBH_FT232_ERR_PARAMETER;
	do {
		usbStatus = USBHX_find_by_vid_pid(&hDev, USB_VID_FTDI, 0xFFFF);
		if (usbStatus == USBH_OK)
		{
			if (USBH_device_get_vid_pid(hDev, &usbVid, &usbPid) == USBH_OK)
			{
				tfp_printf("%04x\\%04x device found\r\n", usbVid, usbPid);
				type = NULL;
				// If it is an Bridgetek device then use that...
				if (usbPid == USB_PID_FTDI_FT232)
				{
					type = "FT232";
				}
				else if (usbPid == USB_PID_FTDI_FT2232)
				{
					type = "FT2232";
				}
				else if (usbPid == USB_PID_FTDI_FT4232)
				{
					type = "FT4232";
				}
				else if (usbPid == USB_PID_FTDI_FT232H)
				{
				type = "FT232H";
				}
				else if (usbPid == USB_PID_FTDI_FT_X_SERIES)
				{
					type = "FTx";
				}
				if (type)
				{
					tfp_printf("%s device found\r\n", type);
					// Get the first interface
					usbStatus = USBH_get_interface_list(hDev, &hInterface);
					if (usbStatus == USBH_OK)
					{
						ftStatus = USBH_FT232_init(hInterface, flags, &ctx);
						if (ftStatus == USBH_FT232_OK)
						{
							uint16_t e2data = 0;

							USBH_FT232_eeprom_read(&ctx, 0x12, &e2data);
							tfp_printf("E2Addr:0x12; E2Data:%04X\r\n", e2data);
							USBH_FT232_eeprom_read(&ctx, 0x13, &e2data);
							tfp_printf("E2Addr:0x13; E2Data:%04X\r\n", e2data);
							USBH_FT232_eeprom_read(&ctx, 0x14, &e2data);
							tfp_printf("E2Addr:0x14; E2Data:%04X\r\n", e2data);

							USBH_FT232_set_baud_rate(&ctx, FT232_BAUD_RATE);
							USBH_FT232_set_flow_control(&ctx, FT232_FLOW_CONTROL);
							USBH_FT232_set_modem_control(&ctx, FT232_FLOW_CONTROL, 1);
							USBH_FT232_set_data(&ctx, FT232_DATA, FT232_PARITY, FT232_STOP);
							break;
						}
					}
				}
			}
		}
	} while (usbStatus == USBH_OK);

	if (ftStatus == USBH_FT232_OK)
	{
		uint16_t latency = 0;
		uint16_t modemstat = 0;

		USBH_FT232_get_latency(&ctx, &latency);
		USBH_FT232_get_modemstat(&ctx, &modemstat);
		tfp_printf("Beginning FT testing... latency: %d, modemstat: %04x\r\n", latency, modemstat);
		ft232_uart_bridge(&ctx);
	}
	else
	{
		tfp_printf("No Bridgetek devices found\r\n");
	}

	return usbStatus;
}

uint8_t usbh_testing(void)
{
    uint8_t status;
    USBH_device_handle hRootDev;
    USBH_STATE connect;

    USBH_initialise(NULL);

    if (!USBHX_root_connected())
    {
        tfp_printf("\r\nPlease plug in a USB Device\r\n");

        // Detect usb device connect
        while (!USBHX_root_connected());
    }
    tfp_printf("\r\nUSB Device Detected\r\n");

    // Start at root hub (dummy for now)
    status = USBH_enumerate(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT);
    if (status != USBH_OK)
    {
    	tfp_printf("status: %d\r\n", status);
    }

    while (!USBHX_root_enumerated());

    USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
    if (connect == USBH_STATE_ENUMERATED_PARTIAL)
    {
    	tfp_printf("USB Device Partially Enumerated (no resources)\r\n");
    }
    else
    {
    	tfp_printf("USB Device Enumerated\r\n");
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
		hub_scan_for_ft(hRootDev);
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

    //sys_enable(sys_device_timer_wdt);

    //interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, ISR_timer);
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
        "Welcome to USBH FT232 Tester Example 1... \r\n"
        "\r\n"
        "Send and receive data from an FT232 connected to the USB host port.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    usbh_testing();

    interrupt_detach(interrupt_timers);
    interrupt_disable_globally();

    sys_disable(sys_device_timer_wdt);

    // Wait forever...
    for (;;);

	return 0;
}


