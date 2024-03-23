/**
    @file

    @brief 
    API for HID devices on USB Host stack on FT900.
  
    API functions for USB Host HID devices. These functions provide functionality
    required to communicate with a HID device through the USB Host interface.
**/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek") subject to the licence terms set out
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

#include <ft900_usb.h>
#include <ft900_usb_hid.h>
#include <ft900_delay.h>
#include <ft900_usbh.h>
#include <ft900_usbhx.h>
#include <ft900_usbh_hid.h>

// Debugging only
#undef HID_DEBUG

#if defined(HID_DEBUG)
#include <tfp_printf.h>
#endif

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

int8_t USBH_HID_init(USBH_device_handle hHIDDevice, USBH_interface_handle hHIDInterface, USBH_HID_context *ctx)
{
    int8_t status;
    USBH_endpoint_handle hEp;
    USBH_endpoint_info epInfo;
    USBH_interface_info ifInfo;

    ctx->hHIDDevice = hHIDDevice;
    ctx->hHIDInterface = hHIDInterface;
    ctx->hHIDEpIn = 0;
    ctx->hHIDEpOut = 0;
    ctx->hidInterfaceNumber = -1;
    ctx->reportInSize = 0;
    ctx->reportOutSize = 0;

    status = USBH_interface_get_info(hHIDInterface, &ifInfo);

    if (status == USBH_OK)
    {
        ctx->hidInterfaceNumber = ifInfo.interface_number;
#ifdef HID_DEBUG
        tfp_printf("HID - Using interface number %d\n", ctx->hidInterfaceNumber);
#endif // HID_DEBUG

        status = USBH_get_endpoint_list(hHIDInterface, &hEp);

        if (status == USBH_OK)
        {
            while (hEp)
            {
                if (USBH_endpoint_get_info(hEp, &epInfo) == USBH_OK)
                {
                    if (epInfo.direction == USBH_DIR_OUT)
                    {
                        if (ctx->hHIDEpOut == 0)
                        {
                            ctx->hHIDEpOut = hEp;
                            ctx->reportOutSize = epInfo.max_packet_size;
#ifdef HID_DEBUG
                            tfp_printf("HID - Found OUT endpoint %d bytes\n", ctx->reportOutSize);
#endif // HID_DEBUG
                        }
                    }
                    if (epInfo.direction == USBH_DIR_IN)
                    {
                        if (ctx->hHIDEpIn == 0)
                        {
                            ctx->hHIDEpIn = hEp;
                            ctx->reportInSize = epInfo.max_packet_size;
#ifdef HID_DEBUG
                            tfp_printf("HID - Found IN endpoint %d bytes\n", ctx->reportInSize);
#endif // HID_DEBUG
                        }
                    }
                }

                USBH_get_next_endpoint(hEp, &hEp);
            }
        }
    }

    return status;
}

int8_t USBH_HID_set_idle(USBH_HID_context *ctx, uint16_t idle)
{
	int32_t status;
    USB_device_request devReq;

    devReq.bRequest = USB_HID_REQUEST_CODE_SET_IDLE;
    devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
            USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
    devReq.wIndex = ctx->hidInterfaceNumber;
    devReq.wValue = idle;
    devReq.wLength = 0;

    status = USBH_device_setup_transfer(ctx->hHIDDevice, &devReq, 0, 0);

	if (status < 0)
		return USBH_ERR_USBERR;
	return USBH_OK;
}

int8_t USBH_HID_get_report_size_in(USBH_HID_context *ctx)
{
    return ctx->reportInSize;
}

int8_t USBH_HID_get_report_size_out(USBH_HID_context *ctx)
{
    return ctx->reportOutSize;
}

int8_t USBH_HID_get_report(USBH_HID_context *ctx, uint8_t *buffer)
{
    int8_t status = USBH_OK;
    int32_t count;

    count = USBH_transfer(ctx->hHIDEpIn, buffer, ctx->reportInSize, 1000);

    if (count < 0)
    {
    	status = (int8_t)count;
    }

    return status;
}

int8_t USBH_HID_set_report(USBH_HID_context *ctx, uint8_t *buffer)
{
    int8_t status = USBH_OK;
    int32_t count;

    count = USBH_transfer(ctx->hHIDEpOut, buffer, ctx->reportOutSize, 0);

    if (count < 0)
    {
    	status = (int8_t)count;
    }

    return status;
}

int8_t USBH_HID_get_hid_descriptor(USBH_HID_context *ctx, size_t size, uint8_t *buffer)
{
    int8_t status = USBH_OK;
    uint16_t totalLength;
    USB_interface_descriptor hidInterface;
    USB_configuration_descriptor hidConfig;
    uint16_t offset = 0;

    // Find the HID descriptor for this HID interface.
    // Parse the config descriptor until we find it. This is done by
    // iterating through the descriptor until we find an interface
    // descriptor followed by a HID descriptor.
    status = USBHX_get_config_descriptor(ctx->hHIDDevice, USB_DESCRIPTOR_TYPE_CONFIGURATION, 0,
    		0, sizeof(USB_configuration_descriptor), (uint8_t *)&hidConfig);

    // Total length of the configuration descriptor to iterate.
    totalLength = hidConfig.wTotalLength;

    while (offset < totalLength)
    {
		status = USBHX_get_config_descriptor(ctx->hHIDDevice, USB_DESCRIPTOR_TYPE_CONFIGURATION, 0,
				offset, sizeof(USB_interface_descriptor), (uint8_t *)&hidInterface);

		if (status != USBH_OK)
		{
			break;
		}

		if (hidInterface.bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE)
		{
			if (hidInterface.bInterfaceClass == USB_CLASS_HID)
			{
				if (hidInterface.bInterfaceNumber == ctx->hidInterfaceNumber)
				{
					// This is the HID descriptor we need to match the device.
					// It must immediately follow the interface descriptor.
					offset += hidInterface.bLength;

					// The HID Descriptor is a variable length descriptor. It can
					// in theory be up to 255 bytes long. Read it into the supplied
					// buffer.
					status = USBHX_get_config_descriptor(ctx->hHIDDevice, USB_DESCRIPTOR_TYPE_CONFIGURATION, 0,
							offset, size, buffer);

					return status;
				}
			}
		}
		offset += hidInterface.bLength;
    }

    return USBH_ERR_NOT_FOUND;
}

int8_t USBH_HID_get_report_descriptor(USBH_HID_context *ctx, uint8_t descIndex, size_t length, uint8_t *buffer)
{
    int8_t status = USBH_OK;
    int32_t result;

    USB_device_request devReq;

    devReq.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;
    devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_STANDARD |
			USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;

	// HID Report descriptors
    devReq.wValue = (USB_DESCRIPTOR_TYPE_REPORT << 8) | descIndex;
    devReq.wIndex = ctx->hidInterfaceNumber;
    devReq.wLength = length;

	result = USBH_device_setup_transfer(ctx->hHIDDevice, &devReq, buffer, 500);
	if (result < 0)
	{
		status = (int8_t)result;
	}

	return status;
}

