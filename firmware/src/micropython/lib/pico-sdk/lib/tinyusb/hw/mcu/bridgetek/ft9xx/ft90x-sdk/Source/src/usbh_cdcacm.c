/**
    @file

    @brief
    Driver for CDC ACM devices on the FT900 USBH stack.

    API functions for CDC ACM devices.
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
#include <limits.h>

#include <ft900_usb.h>
#include <ft900_usb_cdc.h>
#include <ft900_usbh.h>
#include <ft900_usbhx.h>
#include <ft900_usbh_cdcacm.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTIONS ***********************************************************************/

/**
 @details Copy data from the data packet received from the CDC CONTROL
  	  	  interface into the a notification state buffer. The application
  	  	  can then read the notifications back on request.
  	  	  This is a callback function from the USB Host driver.
  	  	  This function will queue up another asynchronous read from
  	  	  the CDC CONTROL interface once it completes.
 @param   id - 32 bit value used as an identified of the originator of
 	 	 	 the asynchronous operation. This is the USBH_CDCACM_context
 	 	 	 structure of this instance of the driver.
 @param   status - return status of USB Host driver operation.
 @param   len - number of bytes read into data buffer.
 @param   buffer - pointer to buffer where received data has been copied.
 @return  Value ignored.
 */
static int8_t cdcacm_notification(uint32_t id, int8_t status, size_t len, uint8_t *buffer)
{
	USBH_CDCACM_context *ctx = (USBH_CDCACM_context *)id;
        USB_CDC_Notification *notification = (USB_CDC_Notification *)buffer;

    if (status == USBH_OK)
    {
		if (len >= sizeof(USB_CDC_Notification))
		{
			if (notification->bmRequestType == (USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
            USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE))
			{
				if (notification->bNotification == USB_CDC_NOTIFICATION_NETWORK_CONNECTION)
				{
					memcpy(&ctx->networkState, &notification[1],
							sizeof(USB_CDC_NetworkConnectionBitmap));
				}
				else if (notification->bNotification == USB_CDC_NOTIFICATION_RESPONSE_AVAILABLE)
				{
					ctx->responseAvailable = 1;
				}
				else if (notification->bNotification == USB_CDC_NOTIFICAITON_SERIAL_STATE)
				{
					memcpy(&ctx->uartState, &notification[1],
							sizeof(USB_CDC_UartStateBitmap));
				}
			}
		}
    }

    if ((status == USBH_OK) || (status == USBH_ERR_TIMEOUT))
    {
		// Send another asynchronous read to the CDC CONTROL interface.
		status = USBH_transfer_async(ctx->hControlEpIn,
                                buffer, CDCACM_NOTIFICATION_BUFFER,
				500, id, cdcacm_notification);
    }

    // Save last status of read.
    ctx->notificationStatus = status;

    return status;
}

/**
 @details Copy data from the data packet received from the CDC DATA
  	  	  interface into the ring buffer. The data is pulled by the
  	  	  application from the ring buffer.
  	  	  This is a callback function from the USB Host driver.
  	  	  This function will queue up another asynchronous read from
  	  	  the CDC DATA interface once it completes.
 @param   id - 32 bit value used as an identified of the originator of
 	 	 	 the asynchronous operation. This is the USBH_CDCACM_context
 	 	 	 structure of this instance of the driver.
 @param   status - return status of USB Host driver operation.
 @param   len - number of bytes read into data buffer.
 @param   buffer - pointer to buffer where received data has been copied.
 @return  Value ignored.
 */
static int8_t cdcacm_datain(uint32_t id, int8_t status, size_t len, uint8_t *buffer)
{
	USBH_CDCACM_context *ctx = (USBH_CDCACM_context *)id;
        uint8_t *p = (uint8_t *)buffer;

	if (status == USBH_OK)
    {
		// Do not overwrite data already in the buffer waiting to be
		// read by the application. This will cause out of order data to
		// be received. Better to stop receiving data.
		while (len--)
		{
			if (ctx->recvBufferAvail)
			{
				ctx->recvBuffer[ctx->recvBufferWrite++] = *p++;
				if (ctx->recvBufferWrite > CDCACM_IN_BUFFER)
					ctx->recvBufferWrite = 0;
				ctx->recvBufferAvail--;
			}
		}
    }

	if ((status == USBH_OK) || (status == USBH_ERR_TIMEOUT))
	{
		// Send another asynchronous read to the CDC DATA interface.
    	status = USBH_transfer_async(ctx->hDataEpIn,
                                buffer, CDCACM_IN_MAX_PACKET,
				500, id, cdcacm_datain);
    }

    // Save last status of read.
    ctx->recvStatus = status;

    return status;
}

int8_t USBH_CDCACM_init(USBH_interface_handle hControlInterface,
		uint8_t flags, USBH_CDCACM_context *ctx)
{
    int8_t status = USBH_CDCACM_ERR_CLASS_NOT_SUPPORTED;
    USBH_endpoint_info epInfo;
    USBH_interface_info ifInfo;
    USBH_interface_handle hDataInterface;
	// Buffer the maximum size of all configuration descriptors read.
    uint8_t buf[sizeof(USB_configuration_descriptor)];
	uint16_t offset = 0;
	uint16_t totalLen;
	// Interface numbers for the control interface and data interface.
    uint8_t controlInterface;
    uint8_t dataInterface = -1;
    // Endpoint handles for searching for data interface endpoints.
    USBH_endpoint_handle hEp1, hEp2;
    // Class variables used for searching for data interface.
	uint8_t usbClass, usbSubclass, usbProtocol;
    // The configuration descriptor will be read in multiple times to
	// find the important functional descriptors and pull the required
	// data out of these.
	// Note: Pointer confDesc casts buf to a configuration descriptor type.
	// This is clearer than casting as there are multiple accesses.
	USB_configuration_descriptor *confDesc =
			(USB_configuration_descriptor *)buf;
	// The interface descriptor is used to find the CDC control interface
	// we have been asked to use. The functional descriptors are a subset
	// of the interface.
	USB_interface_descriptor *ifaceDesc =
			(USB_interface_descriptor *)buf;
	// Find CDC class specific functional descriptors. Cast funcDesc
	// to the buffer receiving the config descriptor portions.
	USB_CDC_ClassSpecificDescriptorHeaderFormat *funcDesc =
			(USB_CDC_ClassSpecificDescriptorHeaderFormat *)buf;

    // Initialise context for CDC
    // NOTE: do not check the class/subclass/protocol. This is the application's job.
    ctx->hControlInterface = hControlInterface;
    ctx->hDataInterface = 0; // Invalid at this stage.
    // Endpoint polling statuses.
    ctx->recvStatus = USBH_OK;
    ctx->notificationStatus = USBH_OK;
    // Circular buffer management.
    ctx->recvBufferWrite = 0;
    ctx->recvBufferRead = 0;
    ctx->recvBufferAvail = CDCACM_IN_BUFFER;

#ifdef CDC_DEBUG
    tfp_printf("CDC - Initialising\r\n");
#endif // CDC_DEBUG

    if ((hControlInterface == 0) || (ctx == NULL))
    {
#ifdef CDC_DEBUG
    tfp_printf("CDC - Zero or null parameters\r\n", status);
#endif // CDC_DEBUG
    	return USBH_CDCACM_ERR_PARAMETER;
    }

    if (USBH_interface_get_info(hControlInterface, &ifInfo) != USBH_OK)
    {
#ifdef CDC_DEBUG
    tfp_printf("CDC - Could not get control interface info\r\n", status);
#endif // CDC_DEBUG
		return USBH_CDCACM_ERR_PARAMETER;
    }

	// Find the parent device of the control interface.
	ctx->hControlDevice = ifInfo.dev;
	// Find the interface number of the control interface passed to us.
	controlInterface  = ifInfo.interface_number;
	ctx->controlInterfaceNumber = controlInterface;

	ctx->callCapabilities = USB_CDC_CM_CAPABILITIES_NONE;
	ctx->acmCapabilities = USB_CDC_ACM_CAPABILITIES_NONE;

	// Default interface for data is the interface immediately
	// following the control interface.
	dataInterface = controlInterface + 1;

	// First read will take in the first configuration descriptor.
	// These use the USB Host extended function.
	status = USBHX_get_config_descriptor(ctx->hControlDevice,
			USB_DESCRIPTOR_TYPE_CONFIGURATION, 0, offset,
			sizeof(USB_configuration_descriptor), buf);

	if (status != USBH_OK)
	{
#ifdef CDC_DEBUG
    tfp_printf("CDC - USB Error reading partial config descriptor\r\n", status);
#endif // CDC_DEBUG
		return USBH_CDCACM_ERR_USB;
	}

	// Find the total length of the descriptors.
	totalLen = confDesc->wTotalLength;
	// This is the length of the first descriptor.
	offset = confDesc->bLength;

	// Scan through the entire config descriptor looking for functional
	// descriptors.
	while (offset < totalLen)
	{
		// Read the next portion of config descriptors. This will read in
		// the first 5 bytes of the descriptor. Note: The largest CDC ACM
		// functional descriptor is 5 bytes long.
		USBHX_get_config_descriptor(ctx->hControlDevice,
				USB_DESCRIPTOR_TYPE_CONFIGURATION, 0, offset,
				sizeof(USB_interface_descriptor), buf);

		if (ifaceDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE)
		{
			// Ensure we are reading the descriptor for the right interface.
			if (ifaceDesc->bInterfaceNumber == controlInterface)
			{
				offset += confDesc->bLength;

				while (offset < totalLen)
				{
					// Read the next portion of config descriptors. This will read in
					// the first 5 bytes of the descriptor. Note: The largest CDC ACM
					// functional descriptor is 5 bytes long.
					USBHX_get_config_descriptor(ctx->hControlDevice,
							USB_DESCRIPTOR_TYPE_CONFIGURATION, 0, offset,
							sizeof(USB_CDC_ClassSpecificDescriptorHeaderFormat), buf);

					// We are looking for interface functional descriptors for ACM.
					if (funcDesc->bDescriptorType == USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE)
					{
						// If this is a union functional descriptor for interface numbers.
						if (funcDesc->bDescriptorSubtype ==
								USB_CDC_DESCRIPTOR_SUBTYPE_UNION_FUNCTIONAL_DESCRIPTOR)
						{
							USB_CDC_UnionInterfaceFunctionalDescriptor *unionDesc =
									(USB_CDC_UnionInterfaceFunctionalDescriptor *)buf;
							// Check that this functional descriptor applies to the interface
							// we have been asked to use.
							if (unionDesc->bControlInterface == controlInterface)
							{
								// Keep the interface number of the subordinate interface
								// to use as the CDC data interface.
								dataInterface = unionDesc->bSubordinateInterface0;
							}
						}
						// If this is a abstract control management functional descriptor.
						else if (funcDesc->bDescriptorSubtype ==
								USB_CDC_DESCRIPTOR_SUBTYPE_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR)
						{
							USB_CDC_AbstractControlManagementFunctionalDescriptor *abstractDesc =
									(USB_CDC_AbstractControlManagementFunctionalDescriptor *)buf;
							ctx->acmCapabilities = abstractDesc->bmCapabilities;
						}
						// If this is a abstract control management functional descriptor.
						else if (funcDesc->bDescriptorSubtype ==
								USB_CDC_DESCRIPTOR_SUBTYPE_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR)
						{
							USB_CDC_CallManagementFunctionalDescriptor *callDesc =
									(USB_CDC_CallManagementFunctionalDescriptor *)buf;
							ctx->callCapabilities = callDesc->bmCapabilities;
						}
					}
					// Reached the end of the functional descriptor for CDC.
					if (confDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE)
					{
						break;
					}
					offset += confDesc->bLength;
				}
			}
		}
		offset += confDesc->bLength;
	}

	// Find the interface for CDC DATA which has either been specified in the Union
	// functional descriptor or is immediately following the CDC CONTROL interface.
	do
	{
		// Next interface on this device
		status = USBH_get_next_interface(hControlInterface, &hDataInterface);
		if (status == USBH_OK)
		{
			if (USBH_interface_get_class_info(hDataInterface, &usbClass, &usbSubclass, &usbProtocol) == USBH_OK)
			{
				// If it is a CDC DATA that matches the Union functional descriptor interface then use that...
				if (usbClass == USB_CLASS_CDC_DATA)
				{
					USBH_interface_get_info(hDataInterface, &ifInfo);
					if (ifInfo.interface_number == dataInterface)
					{
						// Matching CDC Data interface found.
						ctx->dataInterfaceNumber = dataInterface;
						ctx->hDataInterface = hDataInterface;
						break;
					}
				}
			}
		}
	} while (status == USBH_OK);

	if (status != USBH_OK)
	{
#ifdef CDC_DEBUG
    tfp_printf("CDC - No matching data endpoints found\r\n", status);
#endif // CDC_DEBUG
		return USBH_CDCACM_ERR_DATA_ENDPOINT;
	}

	// If we expect a notification endpoint then find it.
	if (!(flags & CDCACM_FLAG_NO_NOTIFICATION))
	{
		if (USBH_get_endpoint_list(ctx->hControlInterface, &ctx->hControlEpIn) != USBH_OK)
		{
#ifdef CDC_DEBUG
    tfp_printf("CDC - No endpoints found for notification endpoint\r\n", status);
#endif // CDC_DEBUG
			return USBH_CDCACM_ERR_NOTIFICATION_ENDPOINT;
		}
	}

	// Find the data endpoints and arrange them as IN and OUT.
	if (USBH_get_endpoint_list(ctx->hDataInterface, &hEp1) == USBH_OK)
	{
		USBH_get_next_endpoint(hEp1, &hEp2);
	}
	else
	{
#ifdef CDC_DEBUG
    tfp_printf("CDC - No endpoints found for data endpoints\r\n", status);
#endif // CDC_DEBUG
		return USBH_CDCACM_ERR_DATA_ENDPOINT;
	}

	if (USBH_endpoint_get_info(hEp1, &epInfo) == USBH_OK)
	{
		// Initialise context structure data endpoints.
		if (epInfo.direction == USBH_DIR_IN)
		{
			ctx->hDataEpIn = hEp1;
			ctx->hDataEpOut = hEp2;
		}
		else
		{
			ctx->hDataEpIn = hEp2;
			ctx->hDataEpOut = hEp1;
		}
	}
	else
	{
#ifdef CDC_DEBUG
    tfp_printf("CDC - No endpoint info for data endpoints\r\n", status);
#endif // CDC_DEBUG
		return USBH_CDCACM_ERR_DATA_ENDPOINT;
	}

	// If we have a notification endpoint then begin polling it.
	if (!(flags & CDCACM_FLAG_NO_NOTIFICATION))
	{
		status = USBH_transfer_async(ctx->hControlEpIn,
				(uint8_t *)ctx->notificationBuffer, CDCACM_NOTIFICATION_BUFFER,
				500, (uint32_t)ctx, cdcacm_notification);
	}

	status = USBH_transfer_async(ctx->hDataEpIn,
			(uint8_t *)ctx->recvPacket, CDCACM_IN_MAX_PACKET,
			500, (uint32_t)ctx, cdcacm_datain);

#ifdef CDC_DEBUG
    tfp_printf("CDC - Initialisation successful %x\r\n", status);
#endif // CDC_DEBUG
    return USBH_CDCACM_OK;
}

void USBH_CDCACM_get_poll_status(USBH_CDCACM_context *ctx, int8_t *notification_status, int8_t *data_status)
{
	*notification_status = ctx->notificationStatus;
	*data_status = ctx->recvStatus;
}

int32_t USBH_CDCACM_read(USBH_CDCACM_context *ctx, uint8_t *buffer, size_t len)
{
	int32_t count = 0;

	while (len--)
	{
	    if (ctx->recvBufferAvail < CDCACM_IN_BUFFER)
		{
            *buffer++ = ctx->recvBuffer[ctx->recvBufferRead++];
			if (ctx->recvBufferRead > CDCACM_IN_BUFFER)
				ctx->recvBufferRead = 0;
			ctx->recvBufferAvail++;
		    count++;
		}
	}
	return count;
}

int32_t USBH_CDCACM_write(USBH_CDCACM_context *ctx, uint8_t *buffer, size_t len)
{
	int32_t count;
	int32_t result;

	// Transmit data to USB OUT endpoint on Device.
	result = USBH_transfer(ctx->hDataEpOut, buffer, len, 50);
	if (result < 0)
	{
		count = -1;
	}
	else
	{
		count = result;
	}

    return count;
}

int8_t USBH_CDCACM_send_encapsulated_command(USBH_CDCACM_context *ctx, char *cmd, size_t len)
{
    int32_t status;
    USB_device_request devReq;

    devReq.bRequest = USB_CDC_REQUEST_SEND_ENCAPSULATED_COMMAND;
    devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
            USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
    devReq.wIndex = ctx->controlInterfaceNumber;
    devReq.wValue = 0;
    devReq.wLength = len;

	ctx->responseAvailable = 0;
    status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, (uint8_t *)cmd, 500);

    if (status < 0)
        return USBH_CDCACM_ERR_CLASS;
    return USBH_CDCACM_OK;
}

int8_t USBH_CDCACM_get_encapsulated_response(USBH_CDCACM_context *ctx, char *rsp, size_t len)
{
    int32_t status;
    USB_device_request devReq;

    devReq.bRequest = USB_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE;
    devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
            USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
    devReq.wIndex = ctx->controlInterfaceNumber;
    devReq.wValue = 0;
    devReq.wLength = len;

    status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, (uint8_t *)rsp, 500);

    if (status < 0)
        return USBH_CDCACM_ERR_CLASS;
    return USBH_CDCACM_OK;
}

int8_t USBH_CDCACM_get_comm_feature(USBH_CDCACM_context *ctx, uint16_t selector, uint16_t *feature)
{
    int32_t status;
    USB_device_request devReq;

    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_COMM_FEATURE)
    {
		devReq.bRequest = USB_CDC_REQUEST_GET_COMM_FEATURE;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
		devReq.wIndex = ctx->controlInterfaceNumber;
		devReq.wValue = selector;
		devReq.wLength = 2; // Always receive 2 bytes.

		status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, (uint8_t *)feature, 500);

	    if (status < 0)
	        return USBH_CDCACM_ERR_CLASS;
	    return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

int8_t USBH_CDCACM_set_comm_feature(USBH_CDCACM_context *ctx, uint16_t selector, uint16_t feature)
{
    int32_t status;
    USB_device_request devReq;

    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_COMM_FEATURE)
    {
		devReq.bRequest = USB_CDC_REQUEST_SET_COMM_FEATURE;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
		devReq.wIndex = ctx->controlInterfaceNumber;
		devReq.wValue = selector;
		devReq.wLength = 2; // Always send 2 bytes.

		status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, (uint8_t *)&feature, 500);

	    if (status < 0)
	        return USBH_CDCACM_ERR_CLASS;
	    return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

int8_t USBH_CDCACM_clear_comm_feature(USBH_CDCACM_context *ctx, uint16_t selector)
{
    int32_t status;
    USB_device_request devReq;

    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_COMM_FEATURE)
    {
		devReq.bRequest = USB_CDC_REQUEST_CLEAR_COMM_FEATURE;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
		devReq.wIndex = ctx->controlInterfaceNumber;
		devReq.wValue = selector;
		devReq.wLength = 0;

		status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, NULL, 500);

	    if (status < 0)
	        return USBH_CDCACM_ERR_CLASS;
	    return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

int8_t USBH_CDCACM_set_control_line_state(USBH_CDCACM_context *ctx, USB_CDC_control_line_state *state)
{
    int32_t status;
    USB_device_request devReq;
	// Control line state is a 16 bit value sent in wValue.
	uint16_t cpState;
	memcpy(&cpState, state, sizeof(uint16_t));

    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_LINE_STATE_CODING)
    {
		devReq.bRequest = USB_CDC_REQUEST_SET_CONTROL_LINE_STATE;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
		devReq.wIndex = ctx->controlInterfaceNumber;
		devReq.wValue = cpState;
		devReq.wLength = 0;

		status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, NULL, 500);

	    if (status < 0)
	        return USBH_CDCACM_ERR_CLASS;
	    return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

int8_t USBH_CDCACM_set_line_coding(USBH_CDCACM_context *ctx, USB_CDC_line_coding *coding)
{
    int32_t status;
    USB_device_request devReq;

    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_LINE_STATE_CODING)
    {
		devReq.bRequest = USB_CDC_REQUEST_SET_LINE_CODING;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
		devReq.wIndex = ctx->controlInterfaceNumber;
		devReq.wValue = 0;
		devReq.wLength = sizeof(USB_CDC_line_coding);

		status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, (uint8_t *)coding, 500);

	    if (status < 0)
	        return USBH_CDCACM_ERR_CLASS;
	    return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

int8_t USBH_CDCACM_get_line_coding(USBH_CDCACM_context *ctx, USB_CDC_line_coding *coding)
{
    int32_t status;
    USB_device_request devReq;

    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_LINE_STATE_CODING)
    {
		devReq.bRequest = USB_CDC_REQUEST_GET_LINE_CODING;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
		devReq.wIndex = ctx->controlInterfaceNumber;
		devReq.wValue = 0;
		devReq.wLength = sizeof(USB_CDC_line_coding);

		status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, (uint8_t *)coding, 500);

	    if (status < 0)
	        return USBH_CDCACM_ERR_CLASS;
	    return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

int8_t USBH_CDCACM_send_break(USBH_CDCACM_context *ctx, uint16_t duration)
{
	int32_t status;
    USB_device_request devReq;

    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_SEND_BREAK)
    {
		devReq.bRequest = USB_CDC_REQUEST_SEND_BREAK;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
		devReq.wIndex = ctx->controlInterfaceNumber;
		devReq.wValue = duration;
		devReq.wLength = 0;

		status = USBH_device_setup_transfer(ctx->hControlDevice, &devReq, NULL, 500);

	    if (status < 0)
	        return USBH_CDCACM_ERR_CLASS;
	    return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

uint8_t USBH_CDCACM_get_uart_state(USBH_CDCACM_context *ctx, USB_CDC_UartStateBitmap *state)
{
    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_LINE_STATE_CODING)
    {
    	memcpy(state, &ctx->uartState, sizeof(USB_CDC_UartStateBitmap));
		return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

uint8_t USBH_CDCACM_get_network_connection(USBH_CDCACM_context *ctx, USB_CDC_NetworkConnectionBitmap *state)
{
    if (ctx->acmCapabilities & USB_CDC_ACM_CAPABILITIES_NETWORK_CONNECTION)
    {
    	memcpy(state, &ctx->networkState, sizeof(USB_CDC_NetworkConnectionBitmap));
		return USBH_CDCACM_OK;
    }
    return USBH_CDCACM_ERR_CLASS;
}

int8_t USBH_CDCACM_get_response_available(USBH_CDCACM_context *ctx)
{
    return ctx->responseAvailable;
}

int8_t USBH_CDCACM_get_acm_capabilities(USBH_CDCACM_context *ctx)
{
	return ctx->acmCapabilities;
}

int8_t USBH_CDCACM_get_call_capabilities(USBH_CDCACM_context *ctx)
{
	return ctx->callCapabilities;
}

