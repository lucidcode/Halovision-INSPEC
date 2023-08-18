/**
  @file usbh_hub_test.c
  @brief
  USB Hub Test

  This example program will print device and configuration descriptors on
  the UART. It will traverse the tree of USB devices and print this information
  for each device found. If hubs or HID devices are found then additional
  descriptors will be displayed.
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
#include <ft900_usb_hid.h>
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

void getDeviceDescp(USBH_device_handle hDev, int level);
void getConfigDescp(USBH_device_handle hDev);
void getHubDescp(USBH_device_handle hDev, uint8_t iface);
void getReportDescp(USBH_device_handle hDev, unsigned char descIndex, unsigned short descLength);
void getHIDDescp(USBH_device_handle hDev, uint8_t iface);

void ISR_timer(void)
{
    if (timer_is_interrupted(timer_select_a))
    {
    	// Call USB timer handler to action transaction and hub timers.
   		USBH_timer();
    }
}

void getDeviceDescp(USBH_device_handle hDev, int level)
{
    int32_t result;
	uint8_t buf[256];
	uint8_t bufString[32];
	int i;
	char *pstr;
	USB_device_request desc_dev;
	USB_device_descriptor hc_iocb_descriptor ALIGN32;
	USB_string_descriptor *string;

	char *start1Msg = "\r\nDevice Descriptors:\r\n";
	char *bcdUSBMsg = " bcdUSB:                   %04x\r\n";
	char *devClassMsg = " bDeviceClass:             %02x\r\n";
	char *subClassMsg = " bDeviceSubClass:          %02x\r\n";
	char *devProtMsg = " bDeviceProtocol:          %02x\r\n";
	char *packSizeMsg = " bMaxPacketSize0:          %02x\r\n";
	char *vidMsg = " VID:                      %04x\r\n";
	char *pidMsg = " PID:                      %04x\r\n";
	char *bcdMsg = " bcdDevice:                %04x\n\r\n";
	char *manfMsg = " iManufacturer:            %s\r\n";
	char *productMsg = " iProduct:                 %s\r\n";
	char *serialMsg = " iSerialNumber:            %s\r\n";

	desc_dev.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_STANDARD |
			USB_BMREQUESTTYPE_RECIPIENT_DEVICE;

	desc_dev.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;

	// Device descriptors
	desc_dev.wValue = (USB_DESCRIPTOR_TYPE_DEVICE << 8) | 0x00;
	desc_dev.wIndex = 0x0000;
	desc_dev.wLength = 0x0012;

	// hc_iocb_descriptor
	result = USBH_device_setup_transfer(hDev, &desc_dev, (uint8_t *)&hc_iocb_descriptor, 500);

	if (result >= 0)
	{
		// device descriptors
		tfp_printf("\r\nDevice found at level %d\r\n", level);
		tfp_printf(start1Msg);
		tfp_printf(bcdUSBMsg, hc_iocb_descriptor.bcdUSB);
		tfp_printf(devClassMsg, hc_iocb_descriptor.bDeviceClass);
		tfp_printf(subClassMsg, hc_iocb_descriptor.bDeviceSubClass);
		tfp_printf(devProtMsg, hc_iocb_descriptor.bDeviceProtocol);
		tfp_printf(packSizeMsg, hc_iocb_descriptor.bMaxPacketSize0);
		tfp_printf(vidMsg, hc_iocb_descriptor.idVendor);
		tfp_printf(pidMsg, hc_iocb_descriptor.idProduct);
		tfp_printf(bcdMsg, hc_iocb_descriptor.bcdDevice);

		// points to the contents (bString) of the structure usb_deviceStringDescriptor_t
		string = (USB_string_descriptor *)buf;

		// Device String Descriptors
		// manufacturer
		if (hc_iocb_descriptor.iManufacturer != 0)
		{
	    	desc_dev.wValue = (USB_DESCRIPTOR_TYPE_STRING << 8) | hc_iocb_descriptor.iManufacturer;
			desc_dev.wIndex = USB_LANGID_ENGLISH_UNITED_STATES;
			desc_dev.wLength = sizeof(buf); // max number of bytes

			USBH_device_setup_transfer(hDev, &desc_dev, buf, 500);

			// remove the header and strip high byte from unicode
			pstr = (char *)&string->bString;
			for (i = 0; (i < (string->bLength - 2) / 2) && (i < 32); i++)
			{
				bufString[i] = *pstr;
				pstr += 2;
			}

			bufString[i++] = '\0';
			tfp_printf(manfMsg, bufString);
		}
		else
		{
			tfp_printf(manfMsg, "No manufacturer string");
		}

		// product
		if (hc_iocb_descriptor.iProduct != 0)
		{
			desc_dev.wValue = (USB_DESCRIPTOR_TYPE_STRING << 8) | hc_iocb_descriptor.iProduct;
			desc_dev.wIndex = USB_LANGID_ENGLISH_UNITED_STATES;
			desc_dev.wLength = sizeof(buf); // max 64

			USBH_device_setup_transfer(hDev, &desc_dev, buf, 500);

			// remove the header and strip high byte from unicode
			pstr = (char *)&string->bString;
			for (i = 0; (i < (string->bLength - 2) / 2) && (i < 32); i++)
			{
				bufString[i] = *pstr;
				pstr += 2;
			}

			bufString[i++] = '\0';
			tfp_printf(productMsg, bufString);
		}
		else
		{
			tfp_printf(productMsg, "No product string");
		}

		// serial
		if (hc_iocb_descriptor.iSerialNumber != 0)
		{
			desc_dev.wValue = (USB_DESCRIPTOR_TYPE_STRING << 8) | hc_iocb_descriptor.iSerialNumber;
			desc_dev.wIndex = USB_LANGID_ENGLISH_UNITED_STATES;
			desc_dev.wLength = sizeof(buf); // max 64

			USBH_device_setup_transfer(hDev, &desc_dev, buf, 500);

			// remove the header and strip high byte from unicode
			pstr = (char *)&string->bString;
			for (i = 0; (i < (string->bLength - 2) / 2) && (i < 32); i++)
			{
				bufString[i] = *pstr;
				pstr += 2;
			}

			bufString[i++] = '\0';
			tfp_printf(serialMsg, bufString);
		}
		else
		{
			tfp_printf(serialMsg, "No serial number string");
		}
	}
}

void getConfigDescp(USBH_device_handle hDev)
{
	int32_t result;
	uint8_t buf[256] ALIGN32;
	uint16_t offset = 0;
	uint16_t totallen = 1;
	uint16_t desclen;
	char desctype;
	unsigned char *pbuf;
	uint8_t ifClass = 0;
	USB_device_request desc_dev;

	desc_dev.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_STANDARD |
			USB_BMREQUESTTYPE_RECIPIENT_DEVICE;

	desc_dev.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;

	// Config descriptors
	desc_dev.wValue = (USB_DESCRIPTOR_TYPE_CONFIGURATION << 8) | 0x00;
	desc_dev.wIndex = 0x0000;
	desc_dev.wLength = 0x00ff;

	result = USBH_device_setup_transfer(hDev, &desc_dev, buf, 500);

	if (result >= 0)
	{
		pbuf = &buf[0];

		while (offset < totallen)
		{
			desclen = pbuf[0];
			desctype = pbuf[1];

			if (desctype == USB_DESCRIPTOR_TYPE_CONFIGURATION)
			{
				totallen = (pbuf[3] << 8) + pbuf[2];
				tfp_printf("\r\nConfiguration Descriptors:\r\n");
				tfp_printf(" wTotalLength:             %04x\r\n", totallen);
				tfp_printf(" bNumInterfaces:           %02x\r\n", pbuf[4]);
				tfp_printf(" bConfigurationValue:      %02x\r\n", pbuf[5]);
				tfp_printf(" iConfiguration:           %02x\r\n", pbuf[6]);
				tfp_printf(" bmAttributes:             %02x\r\n", pbuf[7]);
				tfp_printf(" MaxPower:                 %02x\r\n", pbuf[8]);
			}

			if (desctype == USB_DESCRIPTOR_TYPE_INTERFACE)
			{
				tfp_printf("\r\nInterface Descriptors:\r\n");
				tfp_printf(" bInterfaceNumber:         %02x\r\n", pbuf[2]);
				tfp_printf(" bAlternateSetting:        %02x\r\n", pbuf[3]);
				tfp_printf(" bNumEndpoints:            %02x\r\n", pbuf[4]);
				tfp_printf(" bInterfaceClass:          %02x\r\n", pbuf[5]);
				tfp_printf(" bInterfaceSubClass:       %02x\r\n", pbuf[6]);
				tfp_printf(" bInterfaceProtocol:       %02x\r\n", pbuf[7]);
				tfp_printf(" iInterface:               %02x\r\n", pbuf[8]);
			}

			if (desctype == USB_DESCRIPTOR_TYPE_ENDPOINT)
			{
				tfp_printf("\r\nEndpoint Descriptors:\r\n");
				tfp_printf(" bEndpointAddress:         %02x\r\n", pbuf[2]);
				tfp_printf(" Transfer Type:            ");

				if ((pbuf[3] & 0x03) == USB_ENDPOINT_DESCRIPTOR_ATTR_CONTROL)
				{
					tfp_printf("Control\r\n");
				}

				if ((pbuf[3] & 0x03) == USB_ENDPOINT_DESCRIPTOR_ATTR_ISOCHRONOUS)
				{
					tfp_printf("Isochronous\r\n");
				}

				if ((pbuf[3] & 0x03) == USB_ENDPOINT_DESCRIPTOR_ATTR_BULK)
				{
					tfp_printf("Bulk\r\n");
				}

				if ((pbuf[3] & 0x03) == USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT)
				{
					tfp_printf("Interrupt\r\n");
				}

				tfp_printf(" wMaxPacketSize:           %04x\r\n", ((pbuf[5] << 8) | pbuf[4]));
				tfp_printf(" bInterval:                %02x\r\n", pbuf[6]);
			}

			offset += desclen;
			pbuf += desclen;
		}


		offset = 0;
		totallen = 1;

		pbuf = &buf[0];

		while (offset < totallen)
		{
			desclen = pbuf[0];
			desctype = pbuf[1];

			if (desctype == USB_DESCRIPTOR_TYPE_CONFIGURATION)
			{
				totallen = (pbuf[3] << 8) + pbuf[2];
			}

			if (desctype == USB_DESCRIPTOR_TYPE_INTERFACE)
			{
				ifClass = pbuf[5];

				if (ifClass == USB_CLASS_HUB)
				{
					// read the hub descriptor
					getHubDescp(hDev, pbuf[2]);
				}
				else if (ifClass == USB_CLASS_HID)
				{
					// read the HID descriptors
					getHIDDescp(hDev, pbuf[2]);
				}
			}

			offset += desclen;
			pbuf += desclen;
		}
	}
}

void getHubDescp(USBH_device_handle hDev, uint8_t iface)
{
	int32_t result;
	uint8_t buf[256] ALIGN32;
	uint16_t offset = 0;
	uint16_t totallen = 1;
	uint16_t desclen;
	uint8_t desctype;
	unsigned char *pbuf;

	int i = 0;
	uint8_t var = 0;
	uint8_t numports = 0;

	USB_device_request desc_dev;

	desc_dev.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_CLASS |
			USB_BMREQUESTTYPE_RECIPIENT_DEVICE;

	desc_dev.bRequest = USB_HUB_REQUEST_CODE_GET_DESCRIPTOR;

	// HUB descriptors
	desc_dev.wValue = (USB_DESCRIPTOR_TYPE_HUB << 8) | 0x00;
	desc_dev.wIndex = 0x0000;
	desc_dev.wLength = 0x00ff;

	result = USBH_device_setup_transfer(hDev, &desc_dev, buf, 500);

	if (result >= 0)
	{
		pbuf = &buf[0];

		while (offset < totallen)
		{
			desclen = pbuf[0];
			desctype = pbuf[1];
			numports = pbuf[2];

			tfp_printf("\r\nHub Descriptor (interface %d):\r\n", iface);
			tfp_printf(" bNbrPorts:                %02x\r\n", pbuf[2]);
			tfp_printf(" wHubCharacteristics:      %04x\r\n", ((pbuf[4] << 8) | pbuf[3]));
			tfp_printf(" bPwrOn2PwrGood:           %02x\r\n", pbuf[5]);
			tfp_printf(" bHubContrCurrent:         %02x\r\n", pbuf[6]);

			// the remaining fields have 1 bit per port, so there are variable
			// numbers of them depending on the bNbrPorts field
			var = (numports / 8);

			if (numports % 8 != 0)
				var++;

			for (i = 0; i < var; i++)
			{
				tfp_printf(" DeviceRemovable_%d:        %x\r\n", i, pbuf[i + 7]);
			}

			for (i = 0; i < var; i++)
			{
				tfp_printf(" PortPwrCtrlMask_%d:        %x\r\n", i, pbuf[i + 7 + var]);
			}

			offset += desclen;
			pbuf += desclen;
		}
	}
	(void)desctype;
}

void getReportDescp(USBH_device_handle hDev, unsigned char descIndex, unsigned short descLength)
{
	int32_t result;
	uint8_t buf[256] ALIGN32;
	uint16_t offset = 0;
	uint16_t totallen = 1;
	unsigned char *pbuf;

	int i = 0;
	uint8_t var = 0;

	USB_device_request desc_dev;

	desc_dev.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_STANDARD |
			USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;

	desc_dev.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;

	// HID Report descriptors
	desc_dev.wValue = (USB_DESCRIPTOR_TYPE_REPORT << 8) | 0x00;
	desc_dev.wIndex = descIndex;
	desc_dev.wLength = 0x00ff;

	result = USBH_device_setup_transfer(hDev, &desc_dev, buf, 500);

	if (result > 0)
	{
		pbuf = &buf[0];

		while (offset < totallen)
		{
			tfp_printf("\r\n Report Descriptor %d:\r\n", descIndex);

			// Report decriptors have a variable length
			var = descLength;

			for (i = 0; i < var; i += 2)
			{
				tfp_printf("  %02x %02x\r\n", pbuf[i], pbuf[i + 1]);
			}

			offset += descLength;
			pbuf += descLength;
		}
	}
}

void getHIDDescp(USBH_device_handle hDev, uint8_t iface)
{
	int32_t result;
	uint8_t buf[256] ALIGN32;
	uint16_t offset = 0;
	uint16_t totallen = 1;
	uint16_t desclen;
	uint8_t desctype;
	unsigned char *pbuf;

	int i = 0;
	uint8_t var = 0;
	uint8_t numdesc = 0;

	USB_device_request desc_dev;

	desc_dev.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_STANDARD |
			USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;

	desc_dev.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;

	// HID descriptors
	desc_dev.wValue = (USB_DESCRIPTOR_TYPE_HID << 8) | 0x00;
	desc_dev.wIndex = 0x0000;
	desc_dev.wLength = 0x00ff;

	result = USBH_device_setup_transfer(hDev, &desc_dev, buf, 500);

	if (result >= 0)
	{
		pbuf = &buf[0];

		while (offset < totallen)
		{
			desclen = pbuf[0];
			desctype = pbuf[1];
			// remember the number of descriptors
			numdesc = pbuf[5];

			tfp_printf("\r\nHID Descriptor (interface %d):\r\n", iface);
			tfp_printf(" bcdHID:                   %04x\r\n", ((pbuf[3] << 8) | pbuf[2]));
			tfp_printf(" bCountryCode:             %02x\r\n", pbuf[4]);
			tfp_printf(" bNumDescriptors:          %02x\r\n", pbuf[5]);

			// HID devices have at least 1 report descriptor
			// there are variable fields from here depending on the bNumDescriptors field
			var = numdesc;

			for (i = 0; i < var; i++)
			{
				tfp_printf(" bDescriptorType_%d:        %02x\r\n", i, pbuf[i + 6]);
				tfp_printf(" wDescriptorLength_%d:      %02x%02x\r\n", i, pbuf[i + 7 + var], pbuf[i + 6 + var]);

				if (pbuf[i + 6] == USB_DESCRIPTOR_TYPE_REPORT)
				{
					getReportDescp(hDev, i, ((pbuf[i + 7 + var] << 8) | pbuf[i + 6 + var]));
				}
			}

			offset += desclen;
			pbuf += desclen;
		}
	}
	(void)desctype;
}

void hub_testing(USBH_device_handle hHub, int level)
{
    uint8_t status;
    USBH_device_handle hDev;

	tfp_printf("\r\nUSB Device Enumerated\r\n");
	do {
		tfp_printf("hub %08x level %d\r\n", (unsigned int)hHub, level);
		getDeviceDescp(hHub, level);
		//getConfigDescp(hHub);

		status = USBH_get_device_list(hHub, &hDev);
		if (status == USBH_OK)
		{
			tfp_printf("child of hub %08x\r\n", (unsigned int)hDev);
			hub_testing(hDev, level + 1);
		}

		status = USBH_get_next_device(hHub, &hDev);
		if (status == USBH_OK)
		{
			tfp_printf("next hub %08x\r\n", (unsigned int)hDev);
            hHub = hDev;
        }

    } while (status == USBH_OK);
}

volatile int change = 0;

int8_t enum_change(uint32_t id, int8_t status, size_t len, uint8_t *buffer)
{
	//tfp_printf("enumeration change (%s) unique %d\r\n", status == 0?"disconnect":"connect", (int)id);
	change = 1;
	return USBH_OK;
}

uint8_t usbh_testing(void)
{
    uint8_t status;
    USBH_device_handle hRootDev;
    USBH_STATE connect;
    USBH_ctx ctx = {0};

    ctx.enumeration_change = enum_change;

    USBH_initialise(&ctx);

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

		while (change == 0)
		{
			status = USBH_process();
			USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
			if (connect == USBH_STATE_NOTCONNECTED)
			{
				break;
			}
		}

		if (change)
		{
			tfp_printf("\r\nUSB Device Detected\r\n");
			change = 0;
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
				hub_testing(hRootDev, 1);
				tfp_printf("\r\nPlease remove the USB Device\r\n");
			}
		}
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
    timer_init(timer_select_a, 2000, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
    timer_enable_interrupt(timer_select_a);
    timer_start(timer_select_a);

    tfp_printf("Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to USBH Hub Tester Example 1... \r\n"
        "\r\n"
        "List devices connected to the USB Host Port\r\n"
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


