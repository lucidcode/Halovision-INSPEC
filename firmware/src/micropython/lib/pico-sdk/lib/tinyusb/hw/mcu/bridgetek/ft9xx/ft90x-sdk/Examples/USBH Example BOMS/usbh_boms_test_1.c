/**
  @file usbh_boms_test_1.c
  @brief
  USB BOMS Device Test

  This example program will print sectors from a BOMS disk detected on the USB.
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
#include <ft900_usbh_boms.h>
#include <ft900_startup_dfu.h>

/* UART support for printf output. */
#include "tinyprintf.h"

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

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

void dump_data(uint8_t *data)
{
    int i;

    for (i = 0; i < USBH_BOMS_BLOCK_SIZE; i++)
    {
    	tfp_printf("%02x ", *data++);
        if (i % 16 == 15)
        	tfp_printf("\r\n");
    }
}

void boms_testing(USBH_interface_handle hBOMS)
{
    int8_t status;
    USBH_BOMS_context bomsCtx;
    uint8_t data[USBH_BOMS_BLOCK_SIZE];
    int i, k;

    // Open first LUN on device
    status = USBH_BOMS_init(hBOMS, 0, &bomsCtx);

    if (status == USBH_BOMS_OK)
    {
		tfp_printf("\r\nSector 0\r\n");
		status = USBH_BOMS_read(&bomsCtx, 0, USBH_BOMS_BLOCK_SIZE, &data[0]);
    }
    if (status == USBH_BOMS_OK)
    {
    	dump_data(&data[0]);
    	tfp_printf("\r\nSector 1025\r\n");
    	status = USBH_BOMS_read(&bomsCtx, 1025, USBH_BOMS_BLOCK_SIZE, &data[0]);
    }
    if (status == USBH_BOMS_OK)
    {
    	dump_data(&data[0]);
    }


    if (status == USBH_BOMS_OK)
    {
    	for (k = 0; k < 1024; k++)
    	{
			status = USBH_BOMS_mult_read_start(&bomsCtx, 0, 32768);
			if (status == USBH_BOMS_OK)
			{
				for (i = 0; i < 32768; i += USBH_BOMS_BLOCK_SIZE)
				{
					//tfp_printf("\r\nSector %d of %d\r\n", i / USBH_BOMS_BLOCK_SIZE, 32768 / USBH_BOMS_BLOCK_SIZE);
					if (USBH_BOMS_mult_read_data(&bomsCtx, USBH_BOMS_BLOCK_SIZE, &data[0]) != USBH_BOMS_OK)
						break;
					//dump_data(&data[0]);
				}
			}

			status = USBH_BOMS_mult_end(&bomsCtx);
		}
    	tfp_printf("\r\nDone %d\r\n", status);
    }
    if (status != USBH_BOMS_OK)
    {
		switch (status)
		{
		case USBH_BOMS_ERR_PARAMETER:
			tfp_printf("Parameter\r\n"); break;
		case USBH_BOMS_ERR_CLASS_NOT_SUPPORTED:
			tfp_printf("Class not supported\r\n"); break;
		case USBH_BOMS_ERR_SCSI:
			tfp_printf("SCSI Error\r\n"); break;
		case USBH_BOMS_ERR_STATUS:
			tfp_printf("SCSI Status Phase\r\n"); break;
		case USBH_BOMS_ERR_CLASS:
			tfp_printf("Class\r\n"); break;
		case USBH_BOMS_ERR_LUN:
			tfp_printf("MAX Lun exceeded\r\n"); break;
		case USBH_BOMS_ERR_CAPACITY_TIMEOUT:
			tfp_printf("Get Capacity timeout\r\n"); break;
		default:
			tfp_printf("Unknown error\r\n"); break;
		}
    }
}

void hub_scan_for_boms(USBH_device_handle hDev, int level)
{
    uint8_t status;
    USBH_device_handle hNext;
    USBH_interface_handle hInterface;
	uint8_t usbClass;
    uint8_t usbSubclass;
    uint8_t usbProtocol;

	do {
		// Search all the interfaces on this device for a HID interface
		status = USBH_get_interface_list(hDev, &hInterface);
		while (status == USBH_OK)
		{
       		if (USBH_interface_get_class_info(hInterface, &usbClass, &usbSubclass, &usbProtocol) == USBH_OK)
        	{
       			// If it is a HID interface then use that...
                if ((usbClass == USB_CLASS_MASS_STORAGE) &&
                    (usbSubclass == USB_SUBCLASS_MASS_STORAGE_SCSI) &&
                    (usbProtocol == USB_PROTOCOL_MASS_STORAGE_BOMS))
       			{
       			    tfp_printf("BOMS device found at level %d\r\n", level);
       				boms_testing(hInterface);
       			}
	        }
       		// Next interface on this device
			status = USBH_get_next_interface(hInterface, &hInterface);
		}

		// Get any child devices of this device (i.e. this is a hub).
		status = USBH_get_device_list(hDev, &hNext);
		if (status == USBH_OK)
		{
			// Recursively scan.
			hub_scan_for_boms(hNext, level + 1);
		}

		// Get any other devices on the same device.
		status = USBH_get_next_device(hDev, &hNext);
		if (status == USBH_OK)
		{
			hDev = hNext;
        }

    } while (status == USBH_OK);
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

		do{
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
			hub_scan_for_boms(hRootDev, 1);
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
        "Welcome to USBH BOMS Tester Example 1... \r\n"
        "\r\n"
        "Find and exercises BOMS (Flash Disk) devices connected to the USB \r\n"
    	"Host Port\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    uart_disable_interrupt(UART0, uart_interrupt_tx);
    uart_disable_interrupt(UART0, uart_interrupt_rx);

    usbh_testing();

    interrupt_detach(interrupt_timers);
    interrupt_disable_globally();

    sys_disable(sys_device_timer_wdt);

    // Wait forever...
    for (;;) {};

	return 0;
}
