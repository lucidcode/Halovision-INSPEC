/**
  @file aoa_gpio_1.c
  @brief
  USB Host AOA GPIO Test

  This example program will connect to the FTDIGPIO Android Demo app.
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
#include "ft900_usb_aoa.h"
#include <ft900_usbh.h>
#include "ft900_usbh_aoa.h"
// For startup DFU mode optional feature.
#include <ft900_startup_dfu.h>

#include <registers/ft900_registers.h>

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
    @name    USB Macros to enable/disable debugging output of the driver
    @details These macros will enable debug output to a linked printf function.
 */
//@{
#undef USBH_AOA_DEBUG_DATA
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
    @brief   Counter to time between writes of data to the Android App.
 */
uint16_t eventCounter;

/**
    @brief   Buffers to send and receive data from the Android App.
 */
uint8_t readBuf[64];
uint8_t writeBuf[64];

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
	//tfp_printf("ISR_timer");;
    if (timer_is_interrupted(timer_select_a))
    {
    	// Call USB timer handler to action transaction and hub timers.
    	USBH_timer();

    	if (eventCounter)
    	{
    		eventCounter--;
    	}
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


// Callback which occurs when data is read from the accessory.
// It may be with a timeout result or with data.
int8_t aoa_read_cb(uint32_t id, int8_t status, size_t len, uint8_t *buffer)
{
#ifdef USBH_AOA_DEBUG_DATA
	int i;
#endif // USBH_AOA_DEBUG_DATA

	// Check for a timeout. If it is then there is no data available.
	if (status != USBH_ERR_TIMEOUT)
	{
#ifdef USBH_AOA_DEBUG_DATA
		tfp_printf("Read: %d ", status);
		if (status == USBH_OK)
		{
			for (i = 0; i < len; i++)
			{
				printf("%02x", readBuf[i]);
			}
		}
		printf("\r\n");
#endif // USBH_AOA_DEBUG_DATA

		// Echo data received out onto the UART.
		uart0Tx(buffer, len);
	}

	// Resubmit asyncronous read.
	return USBH_AOA_accessory_read_async((USBH_AOA_context *)id, readBuf, 64, id, aoa_read_cb);
}

void aoa_testing(USBH_AOA_context *ctx, USBH_device_handle hAOAdev)
{
    int32_t result;
    size_t len;
    int i;
    uint16_t usbVid, usbPid;
    USBH_device_info devInfo;
    char *speed[3] = {"low", "full", "high"};
    int32_t baud = 19200;
    uint8_t stop, databits, parity, flow;

    // Display some information on the Android accessory device.
	if (USBH_device_get_vid_pid(hAOAdev, &usbVid, &usbPid) == USBH_OK)
	{
		tfp_printf("VID: %04x PID: %04x\r\n", usbVid, usbPid);
	}
	if (USBH_device_get_info(hAOAdev, &devInfo) == USBH_OK)
	{
		tfp_printf("Speed: %d %s \r\n", devInfo.speed, speed[devInfo.speed]);
	    tfp_printf("Address: %d \r\n", devInfo.addr);
	}

	// Display accessory interfaces supported.
	i = USBH_AOA_has_accessory(ctx);
    tfp_printf("Accessory: %s \r\n", i==USBH_AOA_DETECTED?"yes":"no");
	i = USBH_AOA_has_audio(ctx);
    tfp_printf("Audio: %s \r\n", i==USBH_AOA_DETECTED?"yes":"no");
	i = USBH_AOA_has_adb(ctx);
    tfp_printf("Adb: %s \r\n", i==USBH_AOA_DETECTED?"yes":"no");

    // Start by reading 8 bytes from the Android app. This is the UART
    // configuration to use.
    while (1)
    {
    	result = USBH_AOA_accessory_read(ctx, readBuf, 64);

		if (result != USBH_ERR_TIMEOUT)
		{
#ifdef USBH_AOA_DEBUG_DATA
			tfp_printf("Config Read: %d ", result);
			if (result > 0)
			{
				for (i = 0; i < result; i++)
				{
					printf("%02x", readBuf[i]);
				}
			}
			printf("\r\n");
#endif // USBH_AOA_DEBUG_DATA

			if (result == 8)
			{
				char *flowOptions[]= {"None", "CTS/RTS", };
				char *parityOptions[]= {"None", "Odd", "Even", "Mark", "Space", };

				baud = readBuf[0] +
						(readBuf[1] << 8) +
						(readBuf[2] << 16) +
						(readBuf[3] << 24);
				stop = readBuf[4];
				databits = readBuf[5];
				parity = readBuf[6];
				flow = readBuf[7];

				tfp_printf("Config: baud %ld stop %d data %d parity %s flow %s\r\n",
						baud, stop, databits, parityOptions[parity], flowOptions[flow]);
			}
			else
			{
				tfp_printf("Default Config\r\n");
			}
			break;
		}
    }
	/*
	 * Can use uart_calculate_baud() and other functions to configure
	 * the UART interface as per the data sent from the Android App.
	 * We will just ignore this and continue as normal with the current
	 * UART setup.
	 */

	// Asynchronously read data from the Android app.
	USBH_AOA_accessory_read_async(ctx, readBuf, 64, (uint32_t)ctx, aoa_read_cb);

    while (1)
    {
	    USBH_process();
	    len = uart0Rx(writeBuf, sizeof(writeBuf));
	    if (len)
	    {
#ifdef USBH_AOA_DEBUG_DATA
			tfp_printf("Write: %d ", result);
			for (i = 0; i < len; i++)
			{
				printf("%02x", writeBuf[i]);
			}
			printf("\r\n");
#endif // USBH_AOA_DEBUG_DATA

	    	result = USBH_AOA_accessory_write(ctx, writeBuf, len);
	    }
    }
}

// Process for connecting to AOA devices is:
// 1) Check AOA protocol is valid. This means that the special vendor SETUP
//    command works and the return value is non-zero and matches a protocol
//    version supported by the driver.
// 2) Send string descriptors to the AOA device using the vendor SETUP
//    commands. Then send a start accessory device vendor SETUP command.
// 3) The device re-enumerates by doing a device reset.
// 4) The host re-enumerates the device and then decodes support for the
//    various accessory modes from the USB PID.
// 5) It is now ready for use.

int8_t hub_scan_for_aoa(USBH_device_handle hDev, int level)
{
    uint8_t status;
    USBH_AOA_context ctx = {0};
    USBH_AOA_descriptors dsc =
    {
    		"FTDI",
    		"FTDIUARTDemo",
    		"FT900 Accessory Test",
    		"1.0",
    		"http://www.ftdichip.com",
    		"FT900Accessory1",
    };
    // Disable audio mode if protocol supports it.
    uint16_t audio = USB_AOA_SET_AUDIO_MODE_NONE;

    tfp_printf("Init AOA\r\n");

    // Initialise the device. Try to change it into an accessory after
    // checking the protocol, setting the descriptors and starting the
    // accessory.
   	status = USBH_AOA_init(&ctx, hDev, &dsc, audio);

    if (status == USBH_AOA_STARTED)
    {
    	// The device is an Android accessory device which is in it's normal
    	// mode of operation. The USBH_AOA_init call has started the
    	// accessory and it will do a device reset and re-enumerate. We go
    	// back to the usbh_testing function to wait for enumeration to
    	// complete.

        // A device in Android accessory mode is connected. We can now
    	// attach to the device before calling the testing function.
        tfp_printf("Attaching AOA\r\n");
        if (USBH_AOA_attach(&ctx) == USBH_AOA_OK)
        {
        	// AOA testing function(s).
            tfp_printf("Testing AOA\r\n");
        	aoa_testing(&ctx, hDev);
        }
    }

    return status;
}

uint8_t usbh_testing(void)
{
    uint8_t status;
    USBH_device_handle hRootDev;
    USBH_STATE connect;
	/* 20 us delay for compatibility issues during USBH_AOA_init in some Android devices*/
    USBH_ctx ctx = { NULL, 20};

    USBH_initialise(&ctx);

    while (1)
    {
    	// Check if a device is already connected to the USB Host.
		USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		if (connect == USBH_STATE_NOTCONNECTED)
		{
			tfp_printf("\r\nPlease plug in a USB Device\r\n");

	    	// Wait for a device (any device) to connect to the USB Host.
			do
			{
				// We must process USB events while waiting to connect.
				status = USBH_process();
				USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
			} while (connect == USBH_STATE_NOTCONNECTED);
		}
		// We now have a device connected - it may not be enumerated though.
		tfp_printf("\r\nUSB Device Detected\r\n");

		do
		{
			// Wait for enumeration to complete.
			// This may be when the device is enumerating as an Android accessory
			// or when it is in it's normal mode of operation.
			do{
				status = USBH_process();
				USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
				tfp_printf(".");
			} while (connect < USBH_STATE_ENUMERATED);

			// Enumeration may not complete depending on available resources.
			// Report this but the accessory device may still work.
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
				// Scan the device for an accessory then begin actual testing.
				status = hub_scan_for_aoa(hRootDev, 1);
			}
		} while (status > USBH_OK);

		// The testing has finished. The device may have been removed or the test
		// exited normally. Ask the user to remove the device and wait for it to
		// be removed.
		tfp_printf("\r\nPlease remove the USB Device\r\n");
		// Detect usb device disconnect
		do
		{
			status = USBH_process();
			USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		} while (connect != USBH_STATE_NOTCONNECTED);

		// Go back and wait for a device to be connected.
    }

    return 0;
}

/* FUNCTIONS ***********************************************************************/

int main(void)
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

	sys_enable(sys_device_timer_wdt);

    interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, ISR_timer);
    interrupt_enable_globally();

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

    tfp_printf("Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to USBH UART AOA Tester Example 1... \r\n"
        "\r\n"
        "Tests the FTDIUARTLoopback on an Android device.\r\n"
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
