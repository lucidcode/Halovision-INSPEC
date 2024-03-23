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
    @brief   Time between writes of data to the Android App.
    @details Number of counts to wait until an update to the pushbutton state
    		  is sent to the app.
 */
#define EVENT_COUNTER 340

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
    @brief   Buffer to receive data from the Android App and pass from the callback
    		 function to the main data processing function.
 */
uint8_t readBuf[64];

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

// Callback which occurs when data is read from the accessory.
// It may be with a timeout result or with data.
int8_t aoa_read_cb(uint32_t id, int8_t status, size_t len, uint8_t *buffer)
{
	int i;

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

		// Decode commands from the Android app.
		if (readBuf[0] == 0x14)
		{
			tfp_printf("Reset command\r\n");
		}
		else if (readBuf[0] == 0x11)
		{
			tfp_printf("Configure command: Outmap ");
			for (i = 0; i < 7; i++)
			{
				tfp_printf("%c", ((readBuf[2] >> i) & 1)?'1':'0');
			}
			tfp_printf(" Inmap ");
			for (i = 0; i < 7; i++)
			{
				tfp_printf("%c", ((readBuf[3] >> i) & 1)?'1':'0');
			}
			tfp_printf("\r\n");
		}
		else if (readBuf[0] == 0x13)
		{
			tfp_printf("Write Port: Bitmap ");
			for (i = 0; i < 7; i++)
			{
				tfp_printf("%c", ((readBuf[1] >> i) & 1)?'1':'0');
			}
			tfp_printf("\r\n");
		}
		else
		{
			tfp_printf("Unknown command %02x.\r\n", readBuf[0]);
		}
	}
	tfp_printf(".");

	// Resubmit asyncronous read.
	return USBH_AOA_accessory_read_async((USBH_AOA_context *)id, readBuf, 64, id, aoa_read_cb);
}

void aoa_testing(USBH_AOA_context *ctx, USBH_device_handle hAOAdev)
{
    int32_t result;
    uint8_t writeBuf[64];
    int i;
    uint16_t usbVid, usbPid;
    USBH_device_info devInfo;
    char *speed[3] = {"low", "full", "high"};
    uint8_t map;

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

    // Start by writing 4 zeros to the Android app. This clears all the
    // LEDs.
    writeBuf[0] = 0;
	result = USBH_AOA_accessory_write(ctx, writeBuf, 4);
	tfp_printf("Write: %d\r\n", (int)result);

	// Asynchronously read data from the Android app.
	USBH_AOA_accessory_read_async(ctx, readBuf, 64, (uint32_t)ctx, aoa_read_cb);

	map = 1;
    while (1)
    {
	    USBH_process();
	    if (eventCounter == 0)
	    {
	    	eventCounter = EVENT_COUNTER;

	        writeBuf[1] = map;
	    	result = USBH_AOA_accessory_write(ctx, writeBuf, 4);

#ifdef USBH_AOA_DEBUG_DATA
			tfp_printf("Write: %d ", result);
			if (result >= 0)
			{
				for (i = 0; i < result; i++)
				{
					tfp_printf("%02x", writeBuf[i]);
				}
			}
			tfp_printf("\r\n");
#endif // USBH_AOA_DEBUG_DATA

			tfp_printf("Read Port: Bitmap ");
			for (i = 0; i < 7; i++)
			{
				tfp_printf("%c", ((writeBuf[1] >> i) & 1)?'1':'0');
			}
			tfp_printf("\r\n");

			// Shift 'on' LED. Wrap at bit 8 (it's not used).
			map = map << 1;
	    	if (map == 0x80) map = 1;
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
    		"FTDIGPIODemo",
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

    /* Timer A = 1ms */
	timer_prescaler(timer_select_a, 1000);
    timer_init(timer_select_a, 1000, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
    timer_enable_interrupt(timer_select_a);
    timer_start(timer_select_a);

    tfp_printf("Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to USBH GPIO AOA Tester Example 1... \r\n"
        "\r\n"
        "Tests the FTDIGPIODemo on an Android device.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    uart_disable_interrupt(UART0, uart_interrupt_tx);
    uart_disable_interrupt(UART0, uart_interrupt_rx);

    usbh_testing();

    interrupt_detach(interrupt_timers);
    interrupt_disable_globally();
    sys_disable(sys_device_timer_wdt);

    // Wait forever...
    for (;;);

}
