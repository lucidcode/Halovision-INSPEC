/**
  @file usbd_dfu_startup.c
  @brief
  Standalone function called from an application to initiate the USB device
  port and allow a DFU firmware update.
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
 * This source code ("the Software") is provided by Future Technology Devices
 * International Limited ("Bridgetek") subject to the licence terms set out
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
#include <string.h>
#include <ctype.h>

#include <ft900.h>
#include <ft900_timers.h>
#include <ft900_sys.h>
#include <ft900_interrupt.h>
#include <ft900_usb.h>
#include <ft900_usb_dfu.h>
#include <ft900_usbd.h>
#include <ft900_usbd_dfu.h>

//#define STARTUP_UART_OUTPUT
#ifdef STARTUP_UART_OUTPUT
/* UART support for printf output. */
#include <ft900_uart_simple.h>
#endif // STARTUP_UART_OUTPUT

/* For MikroC const qualifier will place variables in Flash
 * not just make them constant.
 */
#if defined(__GNUC__)
#define DESCRIPTOR_QUALIFIER const
#elif defined(__MIKROC_PRO_FOR_FT90x__)
#define DESCRIPTOR_QUALIFIER data
#endif

/* CONSTANTS ***********************************************************************/

/**
 @name USB Configuration
 @brief Indication of how the USB device is powered and the size of
          the control endpoint Max Packets.
 */
//@{
/// USB Bus Powered - set to 1 for self-powered or 0 for bus-powered
#ifndef USB_SELF_POWERED
#define USB_SELF_POWERED 0
#endif // USB_SELF_POWERED
#if USB_SELF_POWERED == 1
#define USB_CONFIG_BMATTRIBUTES_VALUE (USB_CONFIG_BMATTRIBUTES_SELF_POWERED | USB_CONFIG_BMATTRIBUTES_RESERVED_SET_TO_1)
#else // USB_SELF_POWERED
#define USB_CONFIG_BMATTRIBUTES_VALUE USB_CONFIG_BMATTRIBUTES_RESERVED_SET_TO_1
#endif // USB_SELF_POWERED
/// USB Endpoint Zero packet size (both must match)
#define USB_CONTROL_EP_MAX_PACKET_SIZE 64
#define USB_CONTROL_EP_SIZE USBD_EP_SIZE_64
//@}


/**
 @name DFU Configuration
 @brief Determines the parts of the DFU specification which are supported
        by this firmware.
        These can be a combination of:
        USB_DFU_BMATTRIBUTES_CANDNLOAD - Firmware download (program device).
        USB_DFU_BMATTRIBUTES_CANUPLOAD - Firmware upload (read device).
        USB_DFU_BMATTRIBUTES_WILLDETACH - Device perform a detach and
            attach sequence to change into DFU mode. If this is not
            supported then the host must reset the USB after a detach
            request. This will cause the device to reset and enter DFU mode.
        USB_DFU_BMATTRIBUTES_MANIFESTATIONTOLERANT - Not supported by the
            DFU library.
 */
//@{
#define DFU_ATTRIBUTES USBD_DFU_ATTRIBUTES
//@}

/**
 @name Device Configuration Areas
 @brief Size reserved for string descriptors.
 Leaving the allocation size blank will make an array exactly the size
 of the string allocation.
 */
//@{
// String descriptors - allow only the exact size of the string descriptor.
#define STRING_DESCRIPTOR_ALLOCATION
//@}

/**
 @name DFU_TRANSFER_SIZE definition.
 @brief Number of bytes in block, sent in each DFU_DNLOAD request
 from the DFU update program on the host. This is simplified
 in that the meaning of a block is an arbitrary number of
 bytes. This is intentionally a multiple of the maximum
 packet size for the control endpoints.
 It is used in the DFU functional descriptor as wTransferSize.
 The maximum size supported by the DFU library is 256 bytes
 which is the size of a page of Flash.
 */
//@{
#define DFU_TRANSFER_SIZE 256
#undef DFU_TIMEOUT
#define DFU_TIMEOUT 10000
//@}

/**
 @name DFU_USB_INTERFACE configuration..
 @brief DFU Mode Interface Numbers.
 */
//@{
#define DFU_USB_INTERFACE_DFUMODE 0
//@}

/**
 @name DFU Activity Timeouts
 @brief Number of milliseconds to wait after DFU is started for no
 	 activity to reset the device.
 	 10 seconds should be more than enough.
 */
//@{
#define DFU_ACTIVITY_TIMEOUT 10000
//@}

/**
 @name WCID_VENDOR_REQUEST_CODE for WCID.
 @brief Unique vendor request code for WCID OS Vendor Extension validation.
*/
//@{
#define WCID_VENDOR_REQUEST_CODE	 0x4A   /*same WCID code as that of B-DFU */
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
 @name STARTUP_string_descriptor
 @brief Table of USB String descriptors
 */
static DESCRIPTOR_QUALIFIER uint8_t STARTUP_string_descriptor[STRING_DESCRIPTOR_ALLOCATION] =
{
		// String 0 is actually a list of language IDs supported by the real strings.
		0x04, USB_DESCRIPTOR_TYPE_STRING, 0x09,
		0x04, // 0409 = English (US)

		// String 1 (Manufacturer): "FTDI"
		0x0a, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, 'D', 0x00, 'I',
		0x00,

#if defined(__FT930__)
		// String 2 (Serial Number): "FT434343"
		0x12, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '4', 0x00, '3',
		0x00, '4', 0x00, '3', 0x00, '4', 0x00, '3', 0x00,
#else
		// String 2 (Serial Number): "FT424242"
		0x12, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '4', 0x00, '2',
		0x00, '4', 0x00, '2', 0x00, '4', 0x00, '2', 0x00,
#endif

		// String 3 (DFU Product Name): "FT900 DFU Mode"
		0x1E, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '9', 0x00, '0',
		0x00, '0', 0x00, ' ', 0x00, 'D', 0x00, 'F', 0x00, 'U', 0x00, ' ', 0x00,
		'M', 0x00, 'o', 0x00, 'd', 0x00, 'e', 0x00,

		// String 4 (Interface Name): "DFU Interface"
		0x1c, USB_DESCRIPTOR_TYPE_STRING, 'D', 0x00, 'F', 0x00, 'U', 0x00, ' ',
		0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,
		'a', 0x00, 'c', 0x00, 'e', 0x00,

		// END OF STRINGS
		0x00
};

#ifndef DISABLE_WCID
/**
 @name wcid_string_descriptor
 @brief USB String descriptor for WCID identification.
 */
static DESCRIPTOR_QUALIFIER uint8_t wcid_string_descriptor[USB_MICROSOFT_WCID_STRING_LENGTH] = {
		USB_MICROSOFT_WCID_STRING(WCID_VENDOR_REQUEST_CODE)
};
#endif // DISABLE_WCID

/**
 @name STARTUP_device_descriptor
 @brief Device descriptor for DFU Mode.
 */
static DESCRIPTOR_QUALIFIER USB_device_descriptor STARTUP_device_descriptor =
{
		0x12, /* bLength */
		USB_DESCRIPTOR_TYPE_DEVICE, /* bDescriptorType */
		USB_BCD_VERSION_2_0, /* bcdUSB */          // V2.00
		USB_CLASS_DEVICE, /* bDeviceClass */       // Defined in interface
		USB_SUBCLASS_DEVICE, /* bDeviceSubClass */ // Defined in interface
		USB_PROTOCOL_DEVICE, /* bDeviceProtocol */ // Defined in interface
		USB_CONTROL_EP_MAX_PACKET_SIZE, /* bMaxPacketSize0 */    // 8
		USB_VID_FTDI, /* idVendor */   // idVendor: 0x0403 (FTDI)
		USB_DFU_PID_DFUMODE, /* idProduct */ // idProduct: 0x0fee
		USB_DFU_BCD_DEVICEID/*0x0101*/, /* bcdDevice */        // 1.1
		0x01, /* iManufacturer */      // Manufacturer
		0x03, /* iProduct */           // Product
		0x02, /* iSerialNumber */      // Serial Number
		0x01, /* bNumConfigurations */
};

/**
 @name STARTUP_config_descriptor
 @brief Config descriptor for DFU Mode.
 */
//@{
struct PACK STARTUP_config_descriptor
{
	USB_configuration_descriptor configuration;
	USB_interface_descriptor dfu_interface;
	USB_dfu_functional_descriptor dfu_functional;
};

// Fields marked with * are updated with information in device_config[]
static const DESCRIPTOR_QUALIFIER struct STARTUP_config_descriptor STARTUP_config_descriptor =
{
	{
		0x09, /* configuration.bLength */
		USB_DESCRIPTOR_TYPE_CONFIGURATION, /* configuration.bDescriptorType */
		sizeof(struct STARTUP_config_descriptor), /* configuration.wTotalLength */
		0x01, /* configuration.bNumInterfaces */
		0x01, /* configuration.bConfigurationValue */
		0x00, /* configuration.iConfiguration */
		USB_CONFIG_BMATTRIBUTES_VALUE, /* configuration.bmAttributes */
		0xFA, /* configuration.bMaxPower */ // 500mA
	},
	{
		// ---- INTERFACE DESCRIPTOR for DFU Interface ----
		0x09, /* dfu_interface.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* dfu_interface.bDescriptorType */
		DFU_USB_INTERFACE_DFUMODE, /* dfu_interface.bInterfaceNumber */
		0x00, /* dfu_interface.bAlternateSetting */
		0x00, /* dfu_interface.bNumEndpoints */
		USB_CLASS_APPLICATION, /* dfu_interface.bInterfaceClass */ // bInterfaceClass: Application Specific Class
		USB_SUBCLASS_DFU, /* dfu_interface.bInterfaceSubClass */ // bInterfaceSubClass: Device Firmware Update
		USB_PROTOCOL_DFU_DFUMODE, /* dfu_interface.bInterfaceProtocol */ // bInterfaceProtocol: Runtime Protocol
		0x04, /* dfu_interface.iInterface */
	},
	{
		// ---- FUNCTIONAL DESCRIPTOR for DFU Interface ----
		0x09, /* dfu_functional.bLength */
		USB_DESCRIPTOR_TYPE_DFU_FUNCTIONAL, /* dfu_functional.bDescriptorType */
		DFU_ATTRIBUTES, /* dfu_functional.bmAttributes */
		DFU_TIMEOUT, /* dfu_functional.wDetatchTimeOut */ // wDetatchTimeOut
		DFU_TRANSFER_SIZE, /* dfu_functional.wTransferSize */     // wTransferSize
		USB_BCD_VERSION_DFU_1_1, /* dfu_functional.bcdDfuVersion */ // bcdDfuVersion: DFU Version 1.1
	}
};
//@}

#ifndef DISABLE_WCID
/**
 @name wcid_feature_dfumode
 @brief WCID Compatible ID for DFU interface in DFU mode.
 */
//@{
static const DESCRIPTOR_QUALIFIER USB_WCID_feature_descriptor wcid_feature_dfumode =
{
	sizeof(struct USB_WCID_feature_descriptor), /* dwLength */
	USB_MICROSOFT_WCID_VERSION, /* bcdVersion */
	USB_MICROSOFT_WCID_FEATURE_WINDEX_COMPAT_ID, /* wIndex */
	1, /* bCount */
	{0, 0, 0, 0, 0, 0, 0,}, /* rsv1 */
	DFU_USB_INTERFACE_DFUMODE, /* bFirstInterfaceNumber */
	1, /* rsv2 - set to 1 */
	{'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,}, /* compatibleID[8] */
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}, /* subCompatibleID[8] */
	{0, 0, 0, 0, 0, 0,}, /* rsv3[6] */
};
//@}
#endif // DISABLE_WCID


/**
 @brief Count down ms timer to indicate timeout of DFU function.
 */
static volatile uint16_t STARTUP_timer;

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static void STARTUP_DFU_go(uint16_t timeout);
static void STARTUP_DFU_end(void);

/** @name STARTUP_DFU_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
#ifdef STARTUP_UART_OUTPUT
void STARTUP_DFU_putc(void* p, char c)
{
	uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}
#endif // STARTUP_UART_OUTPUT

static void STARTUP_DFU_ISR_timer(void)
{
	if (timer_is_interrupted(timer_select_a))
	{
		// Countdown for connection and activity timeouts.
		if (STARTUP_timer > 0)
		{
			STARTUP_timer--;
		}

		// Call the DFU timer to progress state machine.
		// The progression depends on the bmAttributes set in the configuration.
		USBD_DFU_timer();
	}
}

/**
 @brief      USB class request handler
 @details    Handle class requests from the host application.
 The bRequest value is parsed and the appropriate
 action or function is performed. Additional values
 from the USB_device_request structure are decoded
 and the DFU state machine and status updated as
 required.
 @param[in]        req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return                status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bRequest is not
 supported.
 **/
static int8_t STARTUP_class_req_cb(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;
	//uint8_t interface = LSB(req->wIndex) & 0x0F;

	// For DFU requests ensure the recipient is an interface...
	if ((req->bmRequestType & USB_BMREQUESTTYPE_RECIPIENT_MASK) ==
			USB_BMREQUESTTYPE_RECIPIENT_INTERFACE)
	{
		// Reset the activity timeout now that a DFU command has been
		// received.
		STARTUP_timer = DFU_ACTIVITY_TIMEOUT;

		// Handle remaining DFU class requests when in DFU Mode.
		// Table 3.2 DFU_DETACH is not supported in DFU Mode.
		switch (req->bRequest)
		{
		case USB_CLASS_REQUEST_DNLOAD:
			/* Block number passed in wValue gives the start address of
			 * to program based on the size of the transfer size.
			 */
			USBD_DFU_class_req_download(req->wValue * DFU_TRANSFER_SIZE,
					req->wLength);
			status = USBD_OK;
			break;

		case USB_CLASS_REQUEST_UPLOAD:
			/* Block number passed in wValue gives the start address of
			 * to program based on the size of the transfer size.
			 */
			USBD_DFU_class_req_upload(req->wValue * DFU_TRANSFER_SIZE,
					req->wLength);
			status = USBD_OK;
			break;

		case USB_CLASS_REQUEST_GETSTATUS:
			USBD_DFU_class_req_getstatus(req->wLength);
			status = USBD_OK;
			break;

		case USB_CLASS_REQUEST_GETSTATE:
			USBD_DFU_class_req_getstate(req->wLength);
			status = USBD_OK;
			break;
		case USB_CLASS_REQUEST_CLRSTATUS:
			USBD_DFU_class_req_clrstatus();
			status = USBD_OK;
			break;
		case USB_CLASS_REQUEST_ABORT:
			USBD_DFU_class_req_abort();
			status = USBD_OK;
			break;

		default:
			// Unknown or unsupported request.
			break;
		}
	}

	return status;
}

/**
 @brief      USB standard request handler for GET_DESCRIPTOR
 @details    Handle standard GET_DESCRIPTOR requests from the host
 application.
 The hValue is parsed and the appropriate device,
 configuration or string descriptor is selected.
 For device and configuration descriptors the DFU
 state machine determines which descriptor to use.
 For string descriptors the lValue selects which
 string from the string_descriptors table to use.
 The string table is automatically traversed to find
 the correct string and the length is taken from the
 string descriptor.
 @param[in]        req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return                status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bmRequestType is not
 supported.
 **/
static int8_t STARTUP_standard_req_get_descriptor(USB_device_request *req, uint8_t **buffer, uint16_t *len)
{
	uint8_t *src = NULL;
	uint16_t length = req->wLength;
	uint8_t hValue = req->wValue >> 8;
	uint8_t lValue = req->wValue & 0x00ff;
	uint16_t i;
	uint8_t slen;

	switch (hValue)
	{
	case USB_DESCRIPTOR_TYPE_DEVICE:
		src = (uint8_t *) &STARTUP_device_descriptor;
		if (length > sizeof(USB_device_descriptor)) // too many bytes requested
			length = sizeof(USB_device_descriptor); // Entire structure.
		break;

	case USB_DESCRIPTOR_TYPE_CONFIGURATION:
		src = (uint8_t *) &STARTUP_config_descriptor;
		if (length > sizeof(STARTUP_config_descriptor)) // too many bytes requested
			length = sizeof(STARTUP_config_descriptor); // Entire structure.
		break;

	case USB_DESCRIPTOR_TYPE_STRING:
#ifndef DISABLE_WCID
		// Special case: WCID descriptor
		if (lValue == USB_MICROSOFT_WCID_STRING_DESCRIPTOR)
		{
			src = (uint8_t *)wcid_string_descriptor;
			length = sizeof(wcid_string_descriptor);
			break;
		}
#endif // DISABLE_WCID

		// Find the nth string in the string descriptor table
		i = 0;
		while ((slen = STARTUP_string_descriptor[i]) > 0)
		{
			src = (uint8_t *)&STARTUP_string_descriptor[i];
			if (lValue == 0)
			{
				break;
			}
			i += slen;
			lValue--;
		}
		if (lValue > 0)
		{
			// String not found
			return USBD_ERR_NOT_SUPPORTED;
		}
		// Update the length returned only if it is less than the requested
		// size
		if (length > slen)
		{
			length = slen;
		}
		break;

	default:
		return USBD_ERR_NOT_SUPPORTED;
	}

	*buffer = src;
	*len = length;

	return USBD_OK;
}

/**
 @brief      USB vendor request handler
 @details    Handle vendor requests from the host application.
 The bRequest value is parsed and the appropriate
 action or function is performed. Additional values
 from the USB_device_request structure are decoded
 and provided to other handlers.
 There are no vendor requests requiring handling in
 this firmware.
 @param[in]        req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return                status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bRequest is not
 supported.
 **/
static int8_t STARTUP_vendor_req_cb(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;
#ifndef DISABLE_WCID
	uint16_t length = req->wLength;

	// For Microsoft WCID only.
	// Request for Compatible ID Feature Descriptors.
	// Check the request code and wIndex.
	if (req->bRequest == WCID_VENDOR_REQUEST_CODE)
	{
		if (req->wIndex == USB_MICROSOFT_WCID_FEATURE_WINDEX_COMPAT_ID)
		{
			if (req->bmRequestType & USB_BMREQUESTTYPE_DIR_DEV_TO_HOST)
			{
				if (length > sizeof(wcid_feature_dfumode)) // too many bytes requested
					length = sizeof(wcid_feature_dfumode); // Entire structure.
				// Return a compatible ID feature descriptor.
				USBD_transfer_ep0(USBD_DIR_IN, (uint8_t *) &wcid_feature_dfumode,
						length, length);
				// ACK packet
				USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);
				status = USBD_OK;
			}
		}
	}
#endif // DISABLE_WCID

	return status;
}

/**
 @brief      USB suspend callback
 @details    Called when the USB bus enters the suspend state.
 @param[in]        status - unused.
 @return                N/A
 **/
static void STARTUP_suspend_cb(uint8_t status)
{
	(void) status;
	return;
}

/**
 @brief      USB resume callback
 @details    Called when the USB bus enters the resume state
 prior to restating after a suspend.
 @param[in]  status - unused.
 @return     N/S
 **/
static void STARTUP_resume_cb(uint8_t status)
{
	(void) status;
	return;
}

/**
 @brief      USB reset callback
 @details    Called when the USB bus enters the reset state.
 @param[in]        status - unused.
 @return                N/A
 **/
static void STARTUP_reset_cb(uint8_t status)
{
	(void) status;

	USBD_set_state(USBD_STATE_DEFAULT);

	// If we are at DFUSTATE_MANIFEST_WAIT_RESET stage and do
	// not support detach then the DFU will reset the chip and
	// run the new firmware.
	// From the DFUSTATE_APPIDLE state advance to DFUSTATE_DFUIDLE.
	// Other states will not advance the state machine or may
	// move this to DFUSTATE_DFUERROR if it is inappropriate to
	// find a reset at that stage.
	USBD_DFU_reset();

	return;
}

/**
 @brief      Power Management Event Interrupt Handler
 @details    Called when a power management event occurs by the interrupt
             handler.
 @param[in]        status - unused.
 @return                N/A
 **/
static void STARTUP_DFU_powermanagement_ISR(void)
{
	if (SYS->PMCFG_H & MASK_SYS_PMCFG_DEV_CONN_DEV)
	{
		// Clear connection interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_CONN_DEV;
		USBD_attach();
	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_DEV_DIS_DEV)
	{
		// Clear disconnection interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_DIS_DEV;
		USBD_detach();
	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_HOST_RST_DEV)
	{
		// Clear Host Reset interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_HOST_RST_DEV;
		USBD_resume();
	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_HOST_RESUME_DEV)
	{
		// Clear Host Resume interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_HOST_RESUME_DEV;
		if(! (SYS->MSC0CFG & MASK_SYS_MSC0CFG_DEV_RMWAKEUP))
		{
			// If we are driving K-state on Device USB port;
			// We must maintain the 1ms requirement before resuming the phy
			USBD_resume();
		}
	}
}

static void STARTUP_DFU_go(uint16_t timeout)
{
	USBD_ctx usb_ctx;
	int8_t status;

	memset(&usb_ctx, 0, sizeof(usb_ctx));

	usb_ctx.standard_req_cb = NULL;
	usb_ctx.get_descriptor_cb = STARTUP_standard_req_get_descriptor;
	usb_ctx.class_req_cb = STARTUP_class_req_cb;
	usb_ctx.vendor_req_cb = STARTUP_vendor_req_cb;
	usb_ctx.suspend_cb = STARTUP_suspend_cb;
	usb_ctx.resume_cb = STARTUP_resume_cb;
	usb_ctx.reset_cb = STARTUP_reset_cb;
	usb_ctx.lpm_cb = NULL;
	usb_ctx.speed = USBD_SPEED_FULL;

	// Initialise the USB device with a control endpoint size
	// of 8 to 64 bytes. This must match the size for bMaxPacketSize0
	// defined in the device descriptor.
	usb_ctx.ep0_size = USB_CONTROL_EP_SIZE;

	USBD_initialise(&usb_ctx);

	// Force the state of DFU state machine into DFU mode.
	USBD_DFU_set_dfumode();

	do
	{
		// Wait for a connection for a specific amount of time.
#ifdef STARTUP_UART_OUTPUT
		tfp_printf("Timeout: %dms, Device Attached: %d\r\n"
					"Looking for connection...\r\n",timeout, USBD_is_connected());
#endif // STARTUP_UART_OUTPUT

		status = USBD_ERR_NOT_CONFIGURED;
		STARTUP_timer = timeout;
		do
		{
			if (USBD_is_connected())
			{
				/* Get USB speed */
				USBD_attach();
				if (USBD_connect() == USBD_OK)
				{
					status = (USBD_get_state() <  USBD_STATE_DEFAULT)? USBD_ERR_NOT_CONFIGURED:USBD_OK;
					// Wait for configured or error state.
					if (status == USBD_OK)
						break;
					// Loop until the timeout expires or the timeout
					// is initially zero (i.e. infinite).
				}
			}
		} while ((STARTUP_timer > 0) || (timeout == 0));

		if (status == USBD_OK)
		{
#ifdef STARTUP_UART_OUTPUT
			tfp_printf("Connected\r\n");
#endif // STARTUP_UART_OUTPUT
			// We have a connection within the specified timeout period.
			do
			{
				status = (USBD_get_state() <  USBD_STATE_DEFAULT)? USBD_ERR_NOT_CONFIGURED:USBD_OK;
				if (status != USBD_OK)
				{
#ifdef STARTUP_UART_OUTPUT
					tfp_printf("Error\r\n");
#endif // STARTUP_UART_OUTPUT
					STARTUP_timer = 0;
					break;
				}
			} while ((STARTUP_timer > 0) || (timeout == 0));
		}
#ifdef STARTUP_UART_OUTPUT
		tfp_printf("Finished\r\n");
#endif // STARTUP_UART_OUTPUT
	} while (timeout == 0);

	// Disable the USB Device.
	USBD_finalise();
}

static void STARTUP_DFU_end(void)
{
	// Restore state
	interrupt_disable_globally();
	interrupt_detach(interrupt_timers);

#ifdef STARTUP_UART_OUTPUT
	uart_close(UART0);
	sys_disable(sys_device_uart0);
#endif // STARTUP_UART_OUTPUT
	sys_disable(sys_device_timer_wdt);
}

/* FUNCTIONS ***********************************************************************/

void startup_dfu(int timeout)
{
#ifdef STARTUP_UART_OUTPUT
	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
#if defined(__FT930__)
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#else
	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#endif

	uart_open(UART0,                    /* Device */
			1,                        /* Prescaler = 1 */
			UART_DIVIDER_19200_BAUD,  /* Divider = 217 */
			uart_data_bits_8,         /* No. Data Bits */
			uart_parity_none,         /* Parity */
			uart_stop_bits_1);        /* No. Stop Bits */

	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
	);

	/* Enable tfp_printf() functionality... */
	init_printf(UART0, STARTUP_DFU_putc);
#endif // STARTUP_UART_OUTPUT

	sys_enable(sys_device_timer_wdt);

	interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, STARTUP_DFU_ISR_timer);
	interrupt_enable_globally();

	/* Timer A = 1ms */
	timer_prescaler(timer_select_a, 1000);
	
	timer_init(timer_select_a, 100, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
	timer_enable_interrupt(timer_select_a);
	timer_start(timer_select_a);

#ifdef STARTUP_UART_OUTPUT
	tfp_printf("Copyright (C) Bridgetek Pte Ltd \r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");
	tfp_printf("DFU STARTUP Function enabled...\r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");

	uart_disable_interrupt(UART0, uart_interrupt_tx);
	uart_disable_interrupt(UART0, uart_interrupt_rx);
#endif // STARTUP_UART_OUTPUT

    //handle the power management interrupts w.r.t to USB device
	interrupt_attach(interrupt_0, (int8_t)interrupt_0, STARTUP_DFU_powermanagement_ISR);
	// Run the DFU code.
	STARTUP_DFU_go(timeout);

	STARTUP_DFU_end();
	interrupt_detach(interrupt_0);
}
