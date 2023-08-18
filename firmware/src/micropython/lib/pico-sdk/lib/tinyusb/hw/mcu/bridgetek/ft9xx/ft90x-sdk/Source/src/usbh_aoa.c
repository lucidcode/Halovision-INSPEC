/**
    @file usbh_aoa.c

    @brief 
    API for Android Open Accessory interface for FT900 using USB Host stack.
  
    API functions for AOA interface on top of USB Host stack. These functions
    provide all the functionality required to implement an AOA application.
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <ft900_usb.h>
#include "ft900_usb_aoa.h"
#include <ft900_usbh.h>
#include <ft900_delay.h>
#include <ft900_usbh_aoa.h>

#if defined(__GNUC__)
#include <machine/endian.h>
#elif defined(__MIKROC_PRO_FOR_FT90x__)
#define BYTE_ORDER 1234
#define LITTLE_ENDIAN 1234
#endif

// Debugging only
#undef USBH_AOA_DEBUG

#if defined(USBH_AOA_DEBUG)
// Note printf does not work in library (only local builds).
//#include "printf.h"
#include <ft900_uart_simple.h>
#include "../tinyprintf/tinyprintf.h"
#endif

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* TYPES ***************************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static int8_t usbh_aoa_get_protocol(USBH_device_handle hDevAccessory, uint16_t *protocol)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_AOA_REQUEST_CODE_GET_PROTOCOL;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = 0;
    devReq.wLength = 2;

    status = USBH_device_setup_transfer(hDevAccessory, &devReq, (uint8_t *)protocol, 500);
#ifdef USBH_AOA_DEBUG
    tfp_printf("AOA - Get Protocol Status %d\r\n", status);
#endif // USBH_AOA_DEBUG
    if (status < 0)
        return USBH_AOA_ERR_CLASS;
    return USBH_AOA_OK;
}

static int8_t usbh_aoa_send_string(USBH_device_handle hDevAccessory, uint16_t id, char *string)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_AOA_REQUEST_CODE_SEND_STRING;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = id;
	devReq.wValue = 0;
	devReq.wLength = strlen((char *)string) + 1;

	status = USBH_device_setup_transfer(hDevAccessory, &devReq, (uint8_t *)string, 5000);
	if (status < 0)
        return USBH_AOA_ERR_CLASS;
    return USBH_AOA_OK;
}

static int8_t usbh_aoa_set_audio_mode(USBH_device_handle hDevAccessory, uint16_t mode)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_AOA_REQUEST_SET_AUDIO_MODE;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = mode;
	devReq.wLength = 0;

	status = USBH_device_setup_transfer(hDevAccessory, &devReq, 0, 500);
    if (status < 0)
        return USBH_AOA_ERR_CLASS;
    return USBH_AOA_OK;
}

static int8_t usbh_aoa_send_start(USBH_device_handle hDevAccessory)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_AOA_REQUEST_CODE_START;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = 0;
	devReq.wLength = 0;

	status = USBH_device_setup_transfer(hDevAccessory, &devReq, 0, 500);
    if (status < 0)
        return USBH_AOA_ERR_CLASS;
    return USBH_AOA_OK;
}

/* FUNCTIONS ***********************************************************************/

int8_t USBH_AOA_init(USBH_AOA_context *ctx, USBH_device_handle hAOADevice, USBH_AOA_descriptors *descriptors, int16_t audio)
{
    int8_t status = USBH_AOA_ERR_CLASS_NOT_SUPPORTED;
    uint16_t aoaProtocolRev;
    uint16_t vid, pid;
    USBH_device_info devInfo = {0};

	// Copy aligned value to return value.
	ctx->hDevAccessory = hAOADevice;
#ifdef USBH_AOA_DEBUG
	tfp_printf("AOA - device %x\r\n", ctx->hDevAccessory);
#endif // USBH_AOA_DEBUG

	// Switch to AOA mode only if required.
	USBH_device_get_vid_pid(ctx->hDevAccessory, &vid, &pid);
	ctx->vid = vid;
	ctx->pid = pid;
#ifdef USBH_AOA_DEBUG
	tfp_printf("AOA - vid %04x pid %04x\r\n", ctx->vid, ctx->pid);
#endif // USBH_AOA_DEBUG

	// Read the protocol from this device.
	if (usbh_aoa_get_protocol(hAOADevice, &aoaProtocolRev) == USBH_OK)
	{
		ctx->protocol = aoaProtocolRev;

#ifdef USBH_AOA_DEBUG
		tfp_printf("AOA - AOA protocol %04x\r\n", ctx->protocol);
#endif // USBH_AOA_DEBUG

		if (ctx->protocol != 0)
		{
			USBH_device_get_info(hAOADevice, &devInfo);
			ctx->parentPort = devInfo.port_number;
			ctx->hDevParent = devInfo.dev;

			// Some devices seem to want to have a delay here.
			delayms(10);

			if ((!descriptors->manufacturer) && (ctx->protocol < USB_AOA_PROTOCOL_2))
			{
				// Only allow app-less accessories in protocol version 2.
				 status = USBH_AOA_ERR_CLASS_NOT_SUPPORTED;
			}
			else
			{
				// Switch to AOA mode. May not want to send manufacturer or model.
				// Not sending these prevents an app from starting.
#ifdef USBH_AOA_DEBUG
				tfp_printf("M: %s\r\n", descriptors->manufacturer);
#endif // USBH_AOA_DEBUG
				if (descriptors->manufacturer)
					status = usbh_aoa_send_string(ctx->hDevAccessory, USB_AOA_STRING_MANUFACTURER, descriptors->manufacturer);
#ifdef USBH_AOA_DEBUG
				tfp_printf("%d,M: %s\r\n", status, descriptors->model);
#endif // USBH_AOA_DEBUG
				if (descriptors->model)
					status = usbh_aoa_send_string(ctx->hDevAccessory, USB_AOA_STRING_MODEL, descriptors->model);
#ifdef USBH_AOA_DEBUG
				tfp_printf("%d,D: %s\r\n", status, descriptors->description);
#endif // USBH_AOA_DEBUG

				status = usbh_aoa_send_string(ctx->hDevAccessory, USB_AOA_STRING_DESCRIPTION, descriptors->description);
#ifdef USBH_AOA_DEBUG
				tfp_printf("%d,V: %s\r\n", status, descriptors->version);
#endif // USBH_AOA_DEBUG
				status = usbh_aoa_send_string(ctx->hDevAccessory, USB_AOA_STRING_VERSION, descriptors->version);
#ifdef USBH_AOA_DEBUG
				tfp_printf("%d,U: %s\r\n", status, descriptors->uri);
#endif // USBH_AOA_DEBUG
				status = usbh_aoa_send_string(ctx->hDevAccessory, USB_AOA_STRING_URI, descriptors->uri);
#ifdef USBH_AOA_DEBUG
				tfp_printf("%d,S: %s\r\n", status, descriptors->serial);
#endif // USBH_AOA_DEBUG
				status = usbh_aoa_send_string(ctx->hDevAccessory, USB_AOA_STRING_SERIAL_NUMBER, descriptors->serial);
#ifdef USBH_AOA_DEBUG
				tfp_printf("%d,", status);
#endif // USBH_AOA_DEBUG
				if (audio != USB_AOA_SET_AUDIO_MODE_NONE)
				{
					if (ctx->protocol >= USB_AOA_PROTOCOL_2)
					{
						usbh_aoa_set_audio_mode(ctx->hDevAccessory, audio);
					}
				}
				status = usbh_aoa_send_start(ctx->hDevAccessory);
#ifdef USBH_AOA_DEBUG
				tfp_printf("usbh_aoa_send_start returned %d\r\n", status);
#endif // USBH_AOA_DEBUG
					// Another short wait.
					delayms(100);

				status = USBH_AOA_STARTED;
			}
		}
	}
/*	else
	{
		USBH_endpoint_handle ep0;

		USBH_get_control_endpoint(hAOADevice, &ep0);
		USBH_interface_clear_host_halt(ep0);
	}
*/
    return status;
}

int8_t USBH_AOA_attach(USBH_AOA_context *ctx)
{
    int8_t status = USBH_AOA_OK;
    USBH_endpoint_handle hEp1, hEp2;
    USBH_endpoint_info epInfo;
    USBH_interface_handle hInterface;
    USBH_interface_info ifInfo;
    uint8_t class, subclass, protocol;
    USBH_device_handle hDev;
    USBH_device_info devInfo;
    uint16_t vid, pid;

	if (ctx->protocol == 0)
	{
#ifdef USBH_AOA_DEBUG
		tfp_printf("AOA - AOA Accessory Protocol 0\r\n");
#endif // USBH_AOA_DEBUG
		return USBH_AOA_ERR_PROTOCOL;
	}

	status = USBH_AOA_STARTED;
	do
	{
		USBH_process();
		// Get any child devices of this device (i.e. this is a hub).
		status = USBH_get_device_list(ctx->hDevParent, &hDev);
		if (status != USBH_OK)
		{
#ifdef USBH_AOA_DEBUG
			tfp_printf("USBH_get_device_list returned %d\r\n", status);
#endif // USBH_AOA_DEBUG
			return USBH_AOA_ERR_USB;
		}

		while (hDev)
		{
			status = USBH_device_get_info(hDev, &devInfo);

		    if ((ctx->parentPort == devInfo.port_number)
					&& (ctx->hDevParent == devInfo.dev))
			{
				ctx->hDevAccessory = hDev;
				status = USBH_AOA_DETECTED;
				break;
			}
		    if ((status = USBH_get_next_device(hDev, &hDev)) != USBH_OK)
		    {
#ifdef USBH_AOA_DEBUG
			tfp_printf("USBH_get_next_device returned %d\r\n", status);
#endif // USBH_AOA_DEBUG
		    	break;
		    }
		};

	} while (status != USBH_AOA_DETECTED);

	if (status != USBH_AOA_DETECTED)
	{
    	return USBH_AOA_ERR_CLASS_NOT_SUPPORTED;
	}

	status = USBH_device_get_vid_pid(ctx->hDevAccessory, &vid, &pid);
	ctx->vid = vid;
	ctx->pid = pid;

	if ((ctx->vid == USB_VID_GOOGLE) && (ctx->protocol > 0))
	{
#ifdef USBH_AOA_DEBUG
		tfp_printf("VID google\r\n");
#endif
		// check ranges for protocol version 1
		if (ctx->protocol == USB_AOA_PROTOCOL_1)
		{
			if ((pid >= USB_PID_ANDROID_ACCESSORY_AVAILABLE) &&
					(pid <= USB_PID_ANDROID_ACCESSORY_ADB_AVAILABLE))
			{
				status = USBH_AOA_DETECTED;
#ifdef USBH_AOA_DEBUG
				tfp_printf("Protocol 1 google\r\n");
#endif
			}
		}
		// check ranges for protocol version 2
		else if (ctx->protocol == USB_AOA_PROTOCOL_2)
		{
			if ((pid >= USB_PID_ANDROID_ACCESSORY_AVAILABLE) &&
					(pid <= USB_PID_ANDROID_ACCESSORY_AUDIO_ADB_AVAILABLE))
			{
				status = USBH_AOA_DETECTED;
#ifdef USBH_AOA_DEBUG
				tfp_printf("Protocol 2 google\r\n");
#endif
			}
		}
	}
	else
	{
#ifdef USBH_AOA_DEBUG
		tfp_printf("USBH_device_get_vid_pid returned %d\r\n", status);
#endif // USBH_AOA_DEBUG
    	return USBH_AOA_ERR_CLASS_NOT_SUPPORTED;
	}

    // Initialise context for AOA
    // NOTE: do not check the class/subclass/protocol. This is the application's job.
    status = USBH_get_interface_list(ctx->hDevAccessory, &hInterface);

#ifdef USBH_AOA_DEBUG
	tfp_printf("AOA - device %x if %x status %d\r\n", ctx->hDevAccessory, hInterface, status);
#endif // USBH_AOA_DEBUG

	while (hInterface)
    {
		if (USBH_interface_get_class_info(hInterface, &class, &subclass, &protocol) == USBH_OK)
		{
#ifdef USBH_AOA_DEBUG
			tfp_printf("AOA - class %x subclass %x\r\n", class, subclass);
#endif // USBH_AOA_DEBUG
			// Confirm this is the accessory interface.
			if ((class == USB_CLASS_VENDOR) &&
					(subclass == USB_SUBCLASS_VENDOR_VENDOR))
			{
#ifdef USBH_AOA_DEBUG
				tfp_printf("AOA - AOA Accessory Interface\r\n");
#endif // USBH_AOA_DEBUG
				ctx->hAccessoryInterface = hInterface;

				if (USBH_get_endpoint_list(ctx->hAccessoryInterface, &hEp1) == USBH_OK)
				{
					USBH_get_next_endpoint(hEp1, &hEp2);
				}

				if (USBH_endpoint_get_info(hEp1, &epInfo) == USBH_OK)
				{
					if (epInfo.type == USBH_EP_BULK)
					{
						if (epInfo.direction == USBH_DIR_IN)
						{
							ctx->hAccessoryEpIn = hEp1;
							ctx->hAccessoryEpOut = hEp2;
						}
						else
						{
							ctx->hAccessoryEpIn = hEp2;
							ctx->hAccessoryEpOut = hEp1;
						}
					}
				}
			}
			else if ((class == USB_CLASS_AUDIO) &&
					(subclass == USB_SUBCLASS_AUDIO_AUDIOSTREAMING))
			{
#ifdef USBH_AOA_DEBUG
				tfp_printf("AOA - Audio Streaming Interface\r\n");
#endif // USBH_AOA_DEBUG

				USBH_interface_get_info(hInterface, &ifInfo);
				//USBH_set_interface(hInterface, ifInfo.alt);

				// Ignore interfaces without endpoints.
				if (USBH_get_endpoint_list(hInterface, &hEp1) == USBH_OK)
				{
					if (USBH_endpoint_get_info(hEp1, &epInfo) == USBH_OK)
					{
						if (epInfo.type == USBH_EP_ISOC)
						{
							// For 44.100 kHz the sample rate (mono) must be
							// 88 or 90 bytes per channel per millisecond.
							// Therefore only interfaces of 180 and above
							// bytes can support this.
							if (epInfo.max_packet_size >= 176)
							{
								ctx->hAudioInterface = hInterface;
								ctx->hAudioEp = hEp1;
								ctx->maxIsoSize = epInfo.max_packet_size;

#ifdef USBH_AOA_DEBUG
								tfp_printf("AOA - Audio Streaming Endpoint %d\r\n", epInfo.index);
#endif // USBH_AOA_DEBUG
							}
						}
					}
				}
			} else if ((class == USB_CLASS_VENDOR) &&
					(subclass == USB_SUBCLASS_VENDOR_ADB))
			{
#ifdef USBH_AOA_DEBUG
				tfp_printf("AOA - ADB Interface\r\n");
#endif // USBH_AOA_DEBUG

				ctx->hAdbInterface = hInterface;

				if (USBH_get_endpoint_list(ctx->hAdbInterface, &hEp1) == USBH_OK)
				{
					USBH_get_next_endpoint(hEp1, &hEp2);
				}

				if (USBH_endpoint_get_info(hEp1, &epInfo) == USBH_OK)
				{
					if (epInfo.type == USBH_EP_BULK)
					{
						if (epInfo.direction == USBH_DIR_IN)
						{
							ctx->hAdbEpIn = hEp1;
							ctx->hAdbEpOut = hEp2;
						}
						else
						{
							ctx->hAdbEpIn = hEp2;
							ctx->hAdbEpOut = hEp1;
						}
					}
				}
			}
		}

	    if (USBH_get_next_interface(hInterface, &hInterface) != USBH_OK)
	    {
	    	break;
	    }
    }

	status = USBH_AOA_OK;

    if ((USBH_AOA_has_accessory(ctx) == USBH_AOA_DETECTED) &&
    		((ctx->hAccessoryInterface == 0) ||
    		 (ctx->hAccessoryEpIn == 0) ||
			 (ctx->hAccessoryEpOut == 0)))
    {
#ifdef USBH_AOA_DEBUG
		tfp_printf("AOA - Error Accessory Interface and Endpoints not found!\r\n");
		tfp_printf("AOA - IF %x EPIN %x EPOUT %x\r\n",ctx->hAccessoryInterface, ctx->hAccessoryEpIn, ctx->hAccessoryEpOut);
#endif // USBH_AOA_DEBUG
    	status = USBH_AOA_ERR_CONFIG;
    }

    if ((USBH_AOA_has_adb(ctx) == USBH_AOA_DETECTED) &&
    		((ctx->hAdbInterface == 0) ||
    		 (ctx->hAdbEpIn == 0) ||
			 (ctx->hAdbEpOut == 0)))
    {
#ifdef USBH_AOA_DEBUG
    	tfp_printf("AOA - Error adb Interface and Endpoints not found!\r\n");
#endif // USBH_AOA_DEBUG
    	status = USBH_AOA_ERR_CONFIG;
    }

    if ((USBH_AOA_has_audio(ctx) == USBH_AOA_DETECTED) &&
    		((ctx->hAudioInterface == 0) ||
    		 (ctx->hAudioEp == 0)))
    {
#ifdef USBH_AOA_DEBUG
    	tfp_printf("AOA - Error Audio Interface and Endpoints not found!\r\n");
#endif // USBH_AOA_DEBUG
    	status = USBH_AOA_ERR_CONFIG;
    }
    else
    {
		USBH_interface_get_info(ctx->hAudioInterface, &ifInfo);
		USBH_set_interface(ctx->hAudioInterface, ifInfo.alt);
    }

#ifdef USBH_AOA_DEBUG
    printf("AOA - Initialisation finished %x\r\n", status);
#endif // USBH_AOA_DEBUG
    return status;
}

int8_t USBH_AOA_detach(USBH_AOA_context *ctx)
{
	ctx->protocol = 0;
	return USBH_AOA_OK;
}

int8_t USBH_AOA_get_protocol(USBH_AOA_context *ctx, uint16_t *protocol)
{
	if (protocol)
	{
		*protocol = ctx->protocol;
		return USBH_AOA_OK;
	}
	return USBH_AOA_ERR_PROTOCOL;
}

int8_t USBH_AOA_has_accessory(USBH_AOA_context *ctx)
{
	if (ctx->protocol)
	{
		if (ctx->vid == USB_VID_GOOGLE)
		{
			if ((ctx->pid == USB_PID_ANDROID_ACCESSORY_AVAILABLE) ||
				(ctx->pid == USB_PID_ANDROID_ACCESSORY_ADB_AVAILABLE) ||
				(ctx->pid == USB_PID_ANDROID_ACCESSORY_AUDIO_AVAILABLE) ||
				(ctx->pid == USB_PID_ANDROID_ACCESSORY_AUDIO_ADB_AVAILABLE))
			{
				return USBH_AOA_DETECTED;
			}
		}

		return USBH_AOA_OK;
	}

	return USBH_AOA_ERR_PROTOCOL;
}

int8_t USBH_AOA_has_audio(USBH_AOA_context *ctx)
{
	if (ctx->protocol)
	{
		if (ctx->vid == USB_VID_GOOGLE)
		{
			if ((ctx->pid == USB_PID_ANDROID_AUDIO_AVAILABLE) ||
				(ctx->pid == USB_PID_ANDROID_AUDIO_ADB_AVAILABLE) ||
				(ctx->pid == USB_PID_ANDROID_ACCESSORY_AUDIO_AVAILABLE) ||
				(ctx->pid == USB_PID_ANDROID_ACCESSORY_AUDIO_ADB_AVAILABLE))
			{
				return USBH_AOA_DETECTED;
			}
		}

		return USBH_AOA_OK;
	}

	return USBH_AOA_ERR_PROTOCOL;
}

int8_t USBH_AOA_has_adb(USBH_AOA_context *ctx)
{
	if (ctx->protocol)
	{
		if (ctx->vid == USB_VID_GOOGLE)
		{
			if ((ctx->pid == USB_PID_ANDROID_ACCESSORY_ADB_AVAILABLE) ||
				(ctx->pid == USB_PID_ANDROID_AUDIO_ADB_AVAILABLE) ||
				(ctx->pid == USB_PID_ANDROID_ACCESSORY_AUDIO_ADB_AVAILABLE))
			{
				return USBH_AOA_DETECTED;
			}
		}

		return USBH_AOA_OK;
	}

	return USBH_AOA_ERR_PROTOCOL;
}

int8_t USBH_AOA_get_audio_endpoint(USBH_AOA_context *ctx,
		USBH_endpoint_handle *hAudio, uint16_t *maxSize)
{
	int8_t status;

    // Check we are in Accessory Mode and audio is enabled.
    if (USBH_AOA_has_audio(ctx) == USBH_AOA_DETECTED)
	{
    	// Check audio initialised.
        if ((ctx->hAudioInterface == 0) || (ctx->hAudioEp == 0) || (ctx->maxIsoSize == 0))
    	{
        	return USBH_AOA_ERR_CONFIG;
    	}

        // Check audio endpoint still exits. i.e. handle is not stale.
		status = USBH_interface_get_info(ctx->hAudioInterface, NULL);
		if (status != USBH_OK)
		{
			return status;
		}

		// Update return values.
		if (hAudio)
		{
			*hAudio = ctx->hAudioEp;
		}
		if (maxSize)
		{
			*maxSize = ctx->maxIsoSize;
		}

		return USBH_AOA_OK;
	}

    // Audio not detected therefore a protocol error.
	return USBH_AOA_ERR_PROTOCOL;
}

int32_t USBH_AOA_audio_read_async(USBH_AOA_context *ctx, uint8_t *buffer,
		uint32_t id, USBH_callback cb)
{
    // Check we are in Accessory Mode and audio is enabled.
    if (USBH_AOA_has_audio(ctx) == USBH_AOA_DETECTED)
	{
    	// Check audio initialised.
        if ((ctx->hAudioInterface == 0) || (ctx->hAudioEp == 0) || (ctx->maxIsoSize == 0))
    	{
        	return USBH_AOA_ERR_CONFIG;
    	}

		// Read audio data from USB IN endpoint on Device.
		return USBH_transfer_async(ctx->hAudioEp, buffer, ctx->maxIsoSize, 0, id, cb);
	}

    // Audio not detected therefore a protocol error.
	return USBH_AOA_ERR_PROTOCOL;
}

int32_t USBH_AOA_accessory_write(USBH_AOA_context *ctx, uint8_t *buffer, size_t len)
{
	if ((ctx->protocol == 0) || (ctx->hAccessoryEpOut == 0))
	{
    	return USBH_AOA_ERR_CONFIG;
	}

	// Transmit data to USB OUT endpoint on Device.
	return USBH_transfer(ctx->hAccessoryEpOut, buffer, len, 50);
}

int32_t USBH_AOA_accessory_read(USBH_AOA_context *ctx, uint8_t *buffer, size_t len)
{
    if ((ctx->protocol == 0) || (ctx->hAccessoryEpIn == 0))
	{
    	return USBH_AOA_ERR_CONFIG;
	}

    // Transmit data to USB OUT endpoint on Device.
    return USBH_transfer(ctx->hAccessoryEpIn, buffer, len, 50);
}

int32_t USBH_AOA_accessory_read_async(USBH_AOA_context *ctx, uint8_t *buffer, size_t len,
		uint32_t id, USBH_callback cb)
{
    if ((ctx->protocol == 0) || (ctx->hAccessoryEpIn == 0))
	{
    	return USBH_AOA_ERR_CONFIG;
	}

    // Transmit data to USB OUT endpoint on Device.
    return USBH_transfer_async(ctx->hAccessoryEpIn, buffer, len, 500, id, cb);
}

int32_t USBH_AOA_accessory_write_async(USBH_AOA_context *ctx, uint8_t *buffer, size_t len,
		uint32_t id, USBH_callback cb)
{
    if ((ctx->protocol == 0) || (ctx->hAccessoryEpOut == 0))
	{
    	return USBH_AOA_ERR_CONFIG;
	}

    // Transmit data to USB OUT endpoint on Device.
    return USBH_transfer_async(ctx->hAccessoryEpOut, buffer, len, 500, id, cb);
}

int32_t USBH_AOA_adb_write(USBH_AOA_context *ctx, uint8_t *buffer, size_t len)
{
    if ((ctx->protocol == 0) || (ctx->hAdbEpOut == 0))
	{
    	return USBH_AOA_ERR_CONFIG;
	}
	// Transmit data to USB OUT endpoint on Device.
	return USBH_transfer(ctx->hAdbEpOut, buffer, len, 50);
}

int32_t USBH_AOA_adb_read(USBH_AOA_context *ctx, uint8_t *buffer, size_t len)
{
	if ((ctx->protocol == 0) || (ctx->hAdbEpIn == 0))
	{
		return USBH_AOA_ERR_CONFIG;
	}
	// Transmit data to USB OUT endpoint on Device.
    return USBH_transfer(ctx->hAdbEpIn, buffer, len, 50);
}

int8_t USBH_AOA_register_hid(USBH_AOA_context *ctx, uint16_t hidID,
		uint16_t descriptorSize)
{
	int32_t status;
	USB_device_request devReq;

    if (ctx->protocol < USB_AOA_PROTOCOL_2)
	{
    	return USBH_AOA_ERR_CONFIG;
	}

	devReq.bRequest = USB_AOA_REQUEST_REGISTER_HID;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = descriptorSize;
	devReq.wValue = hidID;
	devReq.wLength = 0;

	status = USBH_device_setup_transfer(ctx->hDevAccessory, &devReq, 0, 500);
    if (status < 0)
        return USBH_AOA_ERR_CLASS;

    ctx->HIDDescriptorSize = descriptorSize;
    return USBH_AOA_OK;
}

int8_t USBH_AOA_unregister_hid(USBH_AOA_context *ctx, uint16_t hidID)
{
	int32_t status;
	USB_device_request devReq;

    if (ctx->protocol < USB_AOA_PROTOCOL_2)
	{
    	return USBH_AOA_ERR_CONFIG;
	}

    // No register HID sent.
    if (ctx->HIDDescriptorSize == 0)
	{
    	return USBH_AOA_ERR_CONFIG;
	}

    devReq.bRequest = USB_AOA_REQUEST_UNREGISTER_HID;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = hidID;
	devReq.wLength = 0;

	status = USBH_device_setup_transfer(ctx->hDevAccessory, &devReq, 0, 500);
    if (status < 0)
        return USBH_AOA_ERR_CLASS;

    ctx->HIDDescriptorSize = 0;
    return USBH_AOA_OK;
}

int8_t USBH_AOA_set_hid_report_descriptor(USBH_AOA_context *ctx, uint16_t hidID,
		uint16_t descriptorOffset, uint16_t descriptorLength, uint8_t *descriptor)
{
	int32_t status;
	USB_device_request devReq;

    if (ctx->protocol < USB_AOA_PROTOCOL_2)
	{
    	return USBH_AOA_ERR_CONFIG;
	}

    // No register HID sent.
    if (ctx->HIDDescriptorSize == 0)
	{
    	return USBH_AOA_ERR_CONFIG;
	}

	devReq.bRequest = USB_AOA_REQUEST_SET_HID_REPORT_DESC;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = descriptorOffset;
	devReq.wValue = hidID;
	devReq.wLength = descriptorLength;

	status = USBH_device_setup_transfer(ctx->hDevAccessory, &devReq, descriptor, 500);
    if (status < 0)
        return USBH_AOA_ERR_CLASS;
    return USBH_AOA_OK;
}

int8_t USBH_AOA_send_hid_data(USBH_AOA_context *ctx, uint16_t hidID,
		uint16_t reportSize, uint8_t *data)
{
	int32_t status;
	USB_device_request devReq;

    if (ctx->protocol < USB_AOA_PROTOCOL_2)
	{
    	return USBH_AOA_ERR_CONFIG;
	}

    // No register HID sent.
    if (ctx->HIDDescriptorSize == 0)
	{
    	return USBH_AOA_ERR_CONFIG;
	}

	devReq.bRequest = USB_AOA_REQUEST_SEND_HID_EVENT;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_VENDOR | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = hidID;
	devReq.wLength = reportSize;

	status = USBH_device_setup_transfer(ctx->hDevAccessory, &devReq, data, 500);
    if (status < 0)
        return USBH_AOA_ERR_CLASS;
    return USBH_AOA_OK;
}
