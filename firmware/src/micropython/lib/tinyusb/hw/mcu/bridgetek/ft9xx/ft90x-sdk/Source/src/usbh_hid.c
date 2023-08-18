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
#include <ft900_usbh_hid.h>

// Debugging only
#undef HID_DEBUG

#if defined(HID_DEBUG)
// Note printf does not work in library (only local builds).
#include "printf.h"
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
        printf("HID - Using interface number %d\n", ctx->hidInterfaceNumber);
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
                            printf("HID - Found OUT endpoint %d bytes\n", ctx->reportOutSize);
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
                            printf("HID - Found IN endpoint %d bytes\n", ctx->reportInSize);
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
#ifdef HID_DEBUG
    static int count = 0;
#endif // HID_DEBUG
    int8_t status = USBH_OK;
    int32_t count;

    count = USBH_transfer(ctx->hHIDEpIn, buffer, ctx->reportInSize, 1000);

    if (count < 0)
    {
    	status = (int8_t)count;
    }
#ifdef HID_DEBUG
    printf("Status: %x, Count: %d\n", status, count++);
#endif // HID_DEBUG

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
