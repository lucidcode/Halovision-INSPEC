/**
  @file usbh_cdcacm_test_1.c
  @brief
  USB CDC ACM Device Test

  This example program will bridge data from the UART to a CDC ACM device
  on the USB.
 */
/*
 * ============================================================================
 * History
 * =======
 * GDM 21 Jan 2015 : Created
 *
 * (C) Copyright, Bridgetek Pte Ltd
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
#include <string.h>
#include <ctype.h>

#include <ft900.h>
#include <ft900_usb.h>
#include <ft900_usb_cdc.h>
#include <ft900_usb_dfu.h>
#include <ft900_usbd_dfu.h>
#include <ft900_usbdx.h>

/* UART support for printf output. */
#include "tinyprintf.h"

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
 @name USB and Hub Configuration
 @brief Indication of how the USB device is powered and the size of
 	 the control endpoint Max Packets.
 */
//@{
// USB Bus Powered - set to 1 for self-powered or 0 for bus-powered
#ifndef USB_SELF_POWERED
#define USB_SELF_POWERED 0
#endif // USB_SELF_POWERED
#if USB_SELF_POWERED == 1
#define USB_CONFIG_BMATTRIBUTES_VALUE (USB_CONFIG_BMATTRIBUTES_SELF_POWERED | USB_CONFIG_BMATTRIBUTES_RESERVED_SET_TO_1)
#else // USB_SELF_POWERED
#define USB_CONFIG_BMATTRIBUTES_VALUE USB_CONFIG_BMATTRIBUTES_RESERVED_SET_TO_1
#endif // USB_SELF_POWERED
// USB Endpoint Zero packet size (both must match)
#define USB_CONTROL_EP_MAX_PACKET_SIZE 64
#define USB_CONTROL_EP_SIZE USBD_EP_SIZE_64
//@}


/**
 @name DFU Configuration
 @brief Determines the parts of the DFU specification which are supported
        by the DFU library code. Features can be disabled if required.
 */
//@{
#define DFU_ATTRIBUTES USBD_DFU_ATTRIBUTES
//@}

/**
 @name Device Configuration Areas
 @brief Size and location reserved for string descriptors.
 Leaving the allocation size blank will make an array exactly the size
 of the string allocation.
 Note: Specifying the location is not supported by the GCC compiler.
 */
//@{
// String descriptors - allow a maximum of 256 bytes for this
#define STRING_DESCRIPTOR_LOCATION 0x80
#define STRING_DESCRIPTOR_ALLOCATION 0x100
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
#define DFU_TRANSFER_SIZE USBD_DFU_MAX_BLOCK_SIZE
#define DFU_TIMEOUT USBD_DFU_TIMEOUT
//@}

/**
 @name USB_PID_CDCACM configuration.
 @brief Run Time Product ID for CDC function.
 */
//@{
#define USB_PID_CDCACM 0x0fd1
//@}

/**
 @name DFU_USB_INTERFACE configuration..
 @brief Run Time and DFU Mode Interface Numbers.
 */
//@{
#define DFU_USB_INTERFACE_RUNTIME 2
#define DFU_USB_INTERFACE_DFUMODE 0
//@}

/**
   @name Disable DFU function
   @brief Disable the DFU interface in this application.
   	   Normally enabled.
*/
#undef DISABLE_DFU

/**
 @name WCID_VENDOR_REQUEST_CODE for WCID.
 @brief Unique vendor request code for WCID OS Vendor Extension validation.
*/
//@{
#define WCID_VENDOR_REQUEST_CODE	 0x4A   /*same WCID code as that of B-DFU */
//@}

/**
    @name Microsoft WCID Information
    @brief Definitions for WCID String Descriptors and vendor requests.
 **/
//@{
#define USB_MICROSOFT_WCID_VERSION 				 0x0100

#define USB_MICROSOFT_WCID_STRING_DESCRIPTOR	 0xEE
#define USB_MICROSOFT_WCID_STRING_LENGTH		 0x12
#define USB_MICROSOFT_WCID_STRING_SIGNATURE		 'M', 0x00, 'S', 0x00, \
	'F', 0x00, 'T', 0x00, '1', 0x00, '0', 0x00, '0', 0x00,

#define USB_MICROSOFT_WCID_STRING(A) \
		USB_MICROSOFT_WCID_STRING_LENGTH, \
		USB_DESCRIPTOR_TYPE_STRING, USB_MICROSOFT_WCID_STRING_SIGNATURE \
		A, 0x00,

#define USB_MICROSOFT_WCID_FEATURE_WINDEX_COMPAT_ID	 0x0004
//@}

/**
 @brief Endpoint definitions for CDCACM.
 */
//@{
#define CDC_EP_NOTIFICATION 			USBD_EP_1
#define CDC_EP_DATA_IN 					USBD_EP_2
#define CDC_EP_DATA_OUT 				USBD_EP_3
#define CDC_NOTIFICATION_EP_SIZE 		0x10
#define CDC_NOTIFICATION_USBD_EP_SIZE 	USBD_EP_SIZE_16
#define CDC_DATA_EP_SIZE_FS				64
#define CDC_DATA_USBD_EP_SIZE_FS		USBD_EP_SIZE_64
#define CDC_DATA_EP_SIZE_HS				512
#define CDC_DATA_USBD_EP_SIZE_HS		USBD_EP_SIZE_512
//@}

/**
 @brief Change this value in order to modify the size of the Tx and Rx ring buffers
  used to implement UART buffering. Make the ring buffer size 2^n for optimumisation.
 */
#define RINGBUFFER_SIZE (CDC_DATA_EP_SIZE*2)

// TODO: change TX watermark to 16, but need to verify if
// write less than 16 what will happen
#define TX_WATERMARK	0
#define RX_WATERMARK	112
#define TX_FIFO_DEPTH	(128 - TX_WATERMARK)

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
 @name usbd_cdcacm_device_interface_guid
 @brief Device Interface GUID for DFU interface in runtime.
 */
/* Referred:
 * https://github.com/pbatard/libwdi/wiki/WCID-Devices
 */
static const struct PACK USB_WCID_extended_property_descriptor usbd_cdcacm_device_interface_guid =
{
	sizeof(struct USB_WCID_extended_property_descriptor), /* dwLength */
	USB_MICROSOFT_WCID_VERSION, /* bcdVersion */
	USB_MICROSOFT_WCID_FEATURE_WINDEX_DEVICE_GUID, /* wIndex */
	1, /* wCount */
	136, /* dwSize */
	USB_MS_WCID_PROPERTY_DATATYPE_STRING_MULTI, /* dwPropertyDataType */
	42, /* wPropertyNameLength */
	{'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00,
			'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00,
			'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00, 0x00 }, /* null terminated PropertyName */
	80, /* dwPropertyDataLength */
	/* Property Value -> {E7DEB67E-C172-430F-8B54-3D1DCD25FBD8} */
	{0x7b, 0x00, 0x45, 0x00, 0x37, 0x00, 0x44, 0x00, 0x45, 0x00, 0x42, 0x00,
			0x36, 0x00, 0x37, 0x00, 0x45, 0x00, 0x2d, 0x00, 0x43, 0x00, 0x31, 0x00,
			0x37, 0x00, 0x32, 0x00, 0x2d, 0x00, 0x34, 0x00, 0x33, 0x00, 0x30, 0x00,
			0x46, 0x00, 0x2d, 0x00, 0x38, 0x00, 0x42, 0x00, 0x35, 0x00, 0x34, 0x00,
			0x2d, 0x00, 0x33, 0x00, 0x44, 0x00, 0x31, 0x00, 0x44, 0x00, 0x43, 0x00,
			0x44, 0x00, 0x32, 0x00, 0x35, 0x00, 0x46, 0x00, 0x42, 0x00, 0x44, 0x00,
			0x38, 0x00, 0x7d, 0x00,
	 0x00, 0x00, 0x00, 0x00}, /* bPropertyData */
};
/**
 @name string_descriptor
 @brief Table of USB String descriptors

 This is placed at a fixed location in the const section allowing
 up-to 256 bytes of string descriptors to be defined. These can be
 modified or replaced by a utility or binary editor without
 requiring a recompilation of the firmware.
 They are placed at offset 0x100 and reserve 0x100 bytes.
 The data is not stored in section and is therefore
 regarded as const.
 */
DESCRIPTOR_QUALIFIER /*__at(STRING_DESCRIPTOR_LOCATION)*/ uint8_t string_descriptor[STRING_DESCRIPTOR_ALLOCATION] =
{
		// String 0 is actually a list of language IDs supported by the real strings.
		0x04, USB_DESCRIPTOR_TYPE_STRING, 0x09,
		0x04, // 0409 = English (US)


		// String 1 (Manufacturer): "FTDI"
		0x0a, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, 'D', 0x00, 'I',
		0x00,
#if defined(__FT930__)
		// String 2 (Product): "FT930 CDCACM"
		0x1C, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '9', 0x00, '3',
		0x00, '0', 0x00, ' ', 0x00, 'C', 0x00, 'D', 0x00, 'C', 0x00, ' ', 0x00,
		'A', 0x00, 'C', 0x00, 'M', 0x00,
#else
		// String 2 (Product): "FT900 CDCACM"
		0x1C, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '9', 0x00, '0',
		0x00, '0', 0x00, ' ', 0x00, 'C', 0x00, 'D', 0x00, 'C', 0x00, ' ', 0x00,
		'A', 0x00, 'C', 0x00, 'M', 0x00,
#endif

		// String 3 (Serial Number): "FT424242"
		0x12, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '4', 0x00, '2',
		0x00, '4', 0x00, '2', 0x00, '4', 0x00, '2', 0x00,

#if defined(__FT930__)
			// String 4 (DFU Product Name): "FT930 DFU Mode"
			0x1E, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '9', 0x00, '3',
			0x00, '0', 0x00, ' ', 0x00, 'D', 0x00, 'F', 0x00, 'U', 0x00, ' ', 0x00,
			'M', 0x00, 'o', 0x00, 'd', 0x00, 'e', 0x00,
#else
		// String 4 (DFU Product Name): "FT900 DFU Mode"
		0x1E, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '9', 0x00, '0',
		0x00, '0', 0x00, ' ', 0x00, 'D', 0x00, 'F', 0x00, 'U', 0x00, ' ', 0x00,
		'M', 0x00, 'o', 0x00, 'd', 0x00, 'e', 0x00,
#endif

		// String 5 (Interface Name): "DFU Interface"
		0x1c, USB_DESCRIPTOR_TYPE_STRING, 'D', 0x00, 'F', 0x00, 'U', 0x00, ' ',
		0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,
		'a', 0x00, 'c', 0x00, 'e', 0x00,

		// END OF STRINGS
		0x00
};

/**
 @name wcid_string_descriptor
 @brief USB String descriptor for WCID identification.
 */
DESCRIPTOR_QUALIFIER uint8_t wcid_string_descriptor[USB_MICROSOFT_WCID_STRING_LENGTH] = {
		USB_MICROSOFT_WCID_STRING(WCID_VENDOR_REQUEST_CODE)
};

/**
 @name device_descriptor_cdcacm
 @brief Device descriptor for Run Time mode.
 */
DESCRIPTOR_QUALIFIER USB_device_descriptor device_descriptor_cdcacm =
{
		0x12, /* bLength */
		USB_DESCRIPTOR_TYPE_DEVICE, /* bDescriptorType */
		USB_BCD_VERSION_2_0, /* bcdUSB */          // V2.0
		USB_CLASS_DEVICE, /* bDeviceClass */       // Defined in interface
		USB_SUBCLASS_DEVICE, /* bDeviceSubClass */ // Defined in interface
		USB_PROTOCOL_DEVICE, /* bDeviceProtocol */ // Defined in interface
		USB_CONTROL_EP_MAX_PACKET_SIZE, /* bMaxPacketSize0 */
		USB_VID_FTDI, /* idVendor */   // idVendor: 0x0403 (FTDI)
		USB_PID_CDCACM, /* idProduct */ // idProduct: 0x0fe1
		0x0101, /* bcdDevice */        // 1.1
		0x01, /* iManufacturer */      // Manufacturer
		0x02, /* iProduct */           // Product
		0x03, /* iSerialNumber */      // Serial Number
		0x01 /* bNumConfigurations */
};

/**
 @name config_descriptor_cdcacm
 @brief Configuration descriptor for Run Time mode.
 */
//@{
struct PACK config_descriptor_cdcacm
{
	USB_configuration_descriptor configuration;
	USB_interface_association_descriptor interface_association;
	USB_interface_descriptor interface_control;
	USB_CDC_ClassSpecificDescriptorHeaderFormat cdcCPDHF;
	USB_CDC_UnionInterfaceFunctionalDescriptor cdcUIFD;
	USB_CDC_CallManagementFunctionalDescriptor cdcCMFD;
	USB_CDC_AbstractControlManagementFunctionalDescriptor cdcACMFD;
	USB_endpoint_descriptor endpoint_control;
	USB_interface_descriptor interface_data;
	USB_endpoint_descriptor endpoint_data_out;
	USB_endpoint_descriptor endpoint_data_in;
	USB_interface_descriptor dfu_interface;
	USB_dfu_functional_descriptor dfu_functional;
};

/**
 @brief Storage for Configuration Descriptors.
 @details Configuration descriptors may need to be modified to turn from type
 USB_DESCRIPTOR_TYPE_CONFIGURATION to USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION.
 */
union config_descriptor_buffer {
	struct config_descriptor_cdcacm hs;
	struct config_descriptor_cdcacm fs;
} config_descriptor_buffer;

// Fields marked with * are updated with information in device_config[]
DESCRIPTOR_QUALIFIER struct config_descriptor_cdcacm config_descriptor_cdcacm_hs =
{
	{
		sizeof(USB_configuration_descriptor), /* configuration.bLength */
		USB_DESCRIPTOR_TYPE_CONFIGURATION, /* configuration.bDescriptorType */
		sizeof(struct config_descriptor_cdcacm), /* configuration.wTotalLength */
#ifndef DISABLE_DFU
		0x03, /* configuration.bNumInterfaces */
#else
		0x02, /* configuration.bNumInterfaces */
#endif
		0x01, /* configuration.bConfigurationValue */
		0x00, /* configuration.iConfiguration */
		 USB_CONFIG_BMATTRIBUTES_VALUE, /* configuration.bmAttributes */
		0xFA /* configuration.bMaxPower */           // 500mA
	},

	{
		sizeof(USB_interface_association_descriptor), /* interface_association.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION, /* interface_association.bDescriptorType */
		0, /* interface_association.bFirstInterface */
		2, /* interface_association.bInterfaceCount */
		USB_CLASS_CDC_CONTROL, /* interface_association.bFunctionClass */
		USB_SUBCLASS_CDC_CONTROL_ABSTRACT, /* interface_association.bFunctionSubClass */
		USB_PROTOCOL_CDC_CONTROL_ITU_T_V250, /* interface_association.bFunctionProtocol */
		2 /* interface_association.iFunction */  // "FT900 CDCACM"
	},

	// ---- INTERFACE DESCRIPTOR for CDCACM Control ----
	{
		sizeof(USB_interface_descriptor), /* interface_control.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* interface_control.bDescriptorType */
		0, /* interface_control.bInterfaceNumber */
		0x00, /* interface_control.bAlternateSetting */
		0x01, /* interface_control.bNumEndpoints */
		USB_CLASS_CDC_CONTROL, /* interface_control.bInterfaceClass */ // CDC Class
		USB_SUBCLASS_CDC_CONTROL_ABSTRACT, /* interface_control.bInterfaceSubClass */ // Abstract Control Model
		USB_PROTOCOL_CDC_CONTROL_ITU_T_V250, /* interface_control.bInterfaceProtocol */ // No built-in protocol
		0x00 /* interface_control.iInterface */ // Unused
	},

	// ---- FUNCTIONAL DESCRIPTORS for CDCACM ----
	{
		sizeof(USB_CDC_ClassSpecificDescriptorHeaderFormat), /* cdcCPDHF.bFunctionLength */
		USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE, /* cdcCPDHF.bDescriptorType */
		USB_CDC_DESCRIPTOR_SUBTYPE_HEADER_FUNCTIONAL_DESCRIPTOR, /* cdcCPDHF.bDescriptorSubtype */
		0x0110 /* cdcCPDHF.bcdCDC */
	},

	{
		sizeof(USB_CDC_UnionInterfaceFunctionalDescriptor), /* cdcUIFD.bFunctionLength */
		USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE, /* cdcUIFD.bDescriptorType */
		USB_CDC_DESCRIPTOR_SUBTYPE_UNION_FUNCTIONAL_DESCRIPTOR, /* cdcUIFD.bDescriptorSubtype */
		0, /* cdcUIFD.bControlInterface */
		1 /* cdcUIFD.bSubordinateInterface0 */
	},

	{
		sizeof(USB_CDC_CallManagementFunctionalDescriptor), /* cdcCMFD.bFunctionLength */
		USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE, /* cdcCMFD.bDescriptorType */
		USB_CDC_DESCRIPTOR_SUBTYPE_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR, /* cdcCMFD.bDescriptorSubtype */
		USB_CDC_CM_CAPABILITIES_NONE, /* cdcCMFD.bmCapabilities */
		0x01 /* cdcCMFD.bDataInterface */
	},

	{
		sizeof(USB_CDC_AbstractControlManagementFunctionalDescriptor), /* cdcACMFD.bFunctionLength */
		USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE, /* cdcACMFD.bDescriptorType */
		USB_CDC_DESCRIPTOR_SUBTYPE_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR, /* cdcACMFD.bDescriptorSubtype */
		USB_CDC_ACM_CAPABILITIES_LINE_STATE_CODING /* cdcACMFD.bmCapabilities */
	},

	// ---- ENDPOINT DESCRIPTOR for CDCACM Control ----
	{
		sizeof(USB_endpoint_descriptor), /* endpoint_control.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_control.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_IN | CDC_EP_NOTIFICATION, /* endpoint_control.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT, /* endpoint_control.bmAttributes */
		CDC_NOTIFICATION_EP_SIZE, /* endpoint_control.wMaxPacketSize */
		0x0a /* endpoint_control.bInterval */
	},

	// ---- INTERFACE DESCRIPTOR for CDCACM Data ----
	{
		sizeof(USB_interface_descriptor), /* interface_data.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* interface_data.bDescriptorType */
		1, /* interface_data.bInterfaceNumber */
		0x00, /* interface_data.bAlternateSetting */
		0x02, /* interface_data.bNumEndpoints */
		USB_CLASS_CDC_DATA, /* interface_data.bInterfaceClass */ // CDC Data Class
		USB_SUBCLASS_CDC_DATA, /* interface_data.bInterfaceSubClass */ // Abstract Control Model
		USB_PROTOCOL_CDC_DATA, /* interface_data.bInterfaceProtocol */ // No built-in protocol
		0x00 /* interface_data.iInterface */               // Unused
	},

	// ---- ENDPOINT DESCRIPTORS for CDCACM Data ----
	{
		sizeof(USB_endpoint_descriptor), /* endpoint_data_out.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_data_out.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_OUT | CDC_EP_DATA_OUT, /* endpoint_data_out.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_BULK, /* endpoint_data_out.bmAttributes */
		CDC_DATA_EP_SIZE_HS, /* endpoint_data_out.wMaxPacketSize */
		0x0a /* endpoint_data_out.bInterval */
	},

	{
		sizeof(USB_endpoint_descriptor), /* endpoint_data_in.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_data_in.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_IN | CDC_EP_DATA_IN, /* endpoint_data_in.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_BULK, /* endpoint_data_in.bmAttributes */
		CDC_DATA_EP_SIZE_HS, /* endpoint_data_in.wMaxPacketSize */
		0x0a /* endpoint_data_in.bInterval */
	},
#ifndef DISABLE_DFU
	// ---- INTERFACE DESCRIPTOR for DFU Interface ----
	{
		sizeof(USB_interface_descriptor), /* dfu_interface.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* dfu_interface.bDescriptorType */
		DFU_USB_INTERFACE_RUNTIME, /* dfu_interface.bInterfaceNumber */
		0x00, /* dfu_interface.bAlternateSetting */
		0x00, /* dfu_interface.bNumEndpoints */
		USB_CLASS_APPLICATION, /* dfu_interface.bInterfaceClass */ // bInterfaceClass: Application Specific Class
		USB_SUBCLASS_DFU, /* dfu_interface.bInterfaceSubClass */ // bInterfaceSubClass: Device Firmware Update
		USB_PROTOCOL_DFU_RUNTIME, /* dfu_interface.bInterfaceProtocol */ // bInterfaceProtocol: Runtime Protocol
		0x05 /* dfu_interface.iInterface */       // * iInterface: "DFU Interface"
	},

	// ---- FUNCTIONAL DESCRIPTOR for DFU Interface ----
	{
		sizeof(USB_dfu_functional_descriptor), /* dfu_functional.bLength */
		USB_DESCRIPTOR_TYPE_DFU_FUNCTIONAL, /* dfu_functional.bDescriptorType */
		DFU_ATTRIBUTES, /* dfu_functional.bmAttributes */  	// bmAttributes
		DFU_TIMEOUT, /* dfu_functional.wDetatchTimeOut */ // wDetatchTimeOut
		DFU_TRANSFER_SIZE, /* dfu_functional.wTransferSize */     // wTransferSize
		USB_BCD_VERSION_DFU_1_1 /* dfu_functional.bcdDfuVersion */ // bcdDfuVersion: DFU Version 1.1
	}
#endif // DISABLE_DFU
};

DESCRIPTOR_QUALIFIER struct config_descriptor_cdcacm config_descriptor_cdcacm_fs =
{
	{
		sizeof(USB_configuration_descriptor), /* configuration.bLength */
		USB_DESCRIPTOR_TYPE_CONFIGURATION, /* configuration.bDescriptorType */
		sizeof(struct config_descriptor_cdcacm), /* configuration.wTotalLength */
#ifndef DISABLE_DFU
		0x03, /* configuration.bNumInterfaces */
#else
		0x02, /* configuration.bNumInterfaces */
#endif
		0x01, /* configuration.bConfigurationValue */
		0x00, /* configuration.iConfiguration */
		 USB_CONFIG_BMATTRIBUTES_VALUE, /* configuration.bmAttributes */
		0xFA /* configuration.bMaxPower */           // 500mA
	},

	{
		sizeof(USB_interface_association_descriptor), /* interface_association.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION, /* interface_association.bDescriptorType */
		0, /* interface_association.bFirstInterface */
		2, /* interface_association.bInterfaceCount */
		USB_CLASS_CDC_CONTROL, /* interface_association.bFunctionClass */
		USB_SUBCLASS_CDC_CONTROL_ABSTRACT, /* interface_association.bFunctionSubClass */
		USB_PROTOCOL_CDC_CONTROL_ITU_T_V250, /* interface_association.bFunctionProtocol */
		2 /* interface_association.iFunction */  // "FT900 CDCACM"
	},

	// ---- INTERFACE DESCRIPTOR for CDCACM Control ----
	{
		sizeof(USB_interface_descriptor), /* interface_control.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* interface_control.bDescriptorType */
		0, /* interface_control.bInterfaceNumber */
		0x00, /* interface_control.bAlternateSetting */
		0x01, /* interface_control.bNumEndpoints */
		USB_CLASS_CDC_CONTROL, /* interface_control.bInterfaceClass */ // CDC Class
		USB_SUBCLASS_CDC_CONTROL_ABSTRACT, /* interface_control.bInterfaceSubClass */ // Abstract Control Model
		USB_PROTOCOL_CDC_CONTROL_ITU_T_V250, /* interface_control.bInterfaceProtocol */ // No built-in protocol
		0x00 /* interface_control.iInterface */ // Unused
	},

	// ---- FUNCTIONAL DESCRIPTORS for CDCACM ----
	{
		sizeof(USB_CDC_ClassSpecificDescriptorHeaderFormat), /* cdcCPDHF.bFunctionLength */
		USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE, /* cdcCPDHF.bDescriptorType */
		USB_CDC_DESCRIPTOR_SUBTYPE_HEADER_FUNCTIONAL_DESCRIPTOR, /* cdcCPDHF.bDescriptorSubtype */
		0x0110 /* cdcCPDHF.bcdCDC */
	},

	{
		sizeof(USB_CDC_UnionInterfaceFunctionalDescriptor), /* cdcUIFD.bFunctionLength */
		USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE, /* cdcUIFD.bDescriptorType */
		USB_CDC_DESCRIPTOR_SUBTYPE_UNION_FUNCTIONAL_DESCRIPTOR, /* cdcUIFD.bDescriptorSubtype */
		0, /* cdcUIFD.bControlInterface */
		1 /* cdcUIFD.bSubordinateInterface0 */
	},

	{
		sizeof(USB_CDC_CallManagementFunctionalDescriptor), /* cdcCMFD.bFunctionLength */
		USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE, /* cdcCMFD.bDescriptorType */
		USB_CDC_DESCRIPTOR_SUBTYPE_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR, /* cdcCMFD.bDescriptorSubtype */
		USB_CDC_CM_CAPABILITIES_NONE, /* cdcCMFD.bmCapabilities */
		0x01 /* cdcCMFD.bDataInterface */
	},

	{
		sizeof(USB_CDC_AbstractControlManagementFunctionalDescriptor), /* cdcACMFD.bFunctionLength */
		USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE, /* cdcACMFD.bDescriptorType */
		USB_CDC_DESCRIPTOR_SUBTYPE_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR, /* cdcACMFD.bDescriptorSubtype */
		USB_CDC_ACM_CAPABILITIES_LINE_STATE_CODING /* cdcACMFD.bmCapabilities */
	},

	// ---- ENDPOINT DESCRIPTOR for CDCACM Control ----
	{
		sizeof(USB_endpoint_descriptor), /* endpoint_control.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_control.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_IN | CDC_EP_NOTIFICATION, /* endpoint_control.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT, /* endpoint_control.bmAttributes */
		CDC_NOTIFICATION_EP_SIZE, /* endpoint_control.wMaxPacketSize */
		0x0a /* endpoint_control.bInterval */
	},

	// ---- INTERFACE DESCRIPTOR for CDCACM Data ----
	{
		sizeof(USB_interface_descriptor), /* interface_data.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* interface_data.bDescriptorType */
		1, /* interface_data.bInterfaceNumber */
		0x00, /* interface_data.bAlternateSetting */
		0x02, /* interface_data.bNumEndpoints */
		USB_CLASS_CDC_DATA, /* interface_data.bInterfaceClass */ // CDC Data Class
		USB_SUBCLASS_CDC_DATA, /* interface_data.bInterfaceSubClass */ // Abstract Control Model
		USB_PROTOCOL_CDC_DATA, /* interface_data.bInterfaceProtocol */ // No built-in protocol
		0x00 /* interface_data.iInterface */               // Unused
	},

	// ---- ENDPOINT DESCRIPTORS for CDCACM Data ----
	{
		sizeof(USB_endpoint_descriptor), /* endpoint_data_out.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_data_out.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_OUT | CDC_EP_DATA_OUT, /* endpoint_data_out.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_BULK, /* endpoint_data_out.bmAttributes */
		CDC_DATA_EP_SIZE_FS, /* endpoint_data_out.wMaxPacketSize */
		0x0a /* endpoint_data_out.bInterval */
	},

	{
		sizeof(USB_endpoint_descriptor), /* endpoint_data_in.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_data_in.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_IN | CDC_EP_DATA_IN, /* endpoint_data_in.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_BULK, /* endpoint_data_in.bmAttributes */
		CDC_DATA_EP_SIZE_FS, /* endpoint_data_in.wMaxPacketSize */
		0x0a /* endpoint_data_in.bInterval */
	},
#ifndef DISABLE_DFU
	// ---- INTERFACE DESCRIPTOR for DFU Interface ----
	{
		sizeof(USB_interface_descriptor), /* dfu_interface.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* dfu_interface.bDescriptorType */
		DFU_USB_INTERFACE_RUNTIME, /* dfu_interface.bInterfaceNumber */
		0x00, /* dfu_interface.bAlternateSetting */
		0x00, /* dfu_interface.bNumEndpoints */
		USB_CLASS_APPLICATION, /* dfu_interface.bInterfaceClass */ // bInterfaceClass: Application Specific Class
		USB_SUBCLASS_DFU, /* dfu_interface.bInterfaceSubClass */ // bInterfaceSubClass: Device Firmware Update
		USB_PROTOCOL_DFU_RUNTIME, /* dfu_interface.bInterfaceProtocol */ // bInterfaceProtocol: Runtime Protocol
		0x05 /* dfu_interface.iInterface */       // * iInterface: "DFU Interface"
	},

	// ---- FUNCTIONAL DESCRIPTOR for DFU Interface ----
	{
		sizeof(USB_dfu_functional_descriptor), /* dfu_functional.bLength */
		USB_DESCRIPTOR_TYPE_DFU_FUNCTIONAL, /* dfu_functional.bDescriptorType */
		DFU_ATTRIBUTES, /* dfu_functional.bmAttributes */  	// bmAttributes
		DFU_TIMEOUT, /* dfu_functional.wDetatchTimeOut */ // wDetatchTimeOut
		DFU_TRANSFER_SIZE, /* dfu_functional.wTransferSize */     // wTransferSize
		USB_BCD_VERSION_DFU_1_1 /* dfu_functional.bcdDfuVersion */ // bcdDfuVersion: DFU Version 1.1
	}
#endif // DISABLE_DFU
};

//@}
#ifndef DISABLE_DFU
/**
 @name wcid_feature_runtime
 @brief WCID Compatible ID for DFU interface in runtime.
 */
//@{
DESCRIPTOR_QUALIFIER USB_WCID_feature_descriptor wcid_feature_runtime =
{
	sizeof(struct USB_WCID_feature_descriptor), /* dwLength */
	USB_MICROSOFT_WCID_VERSION, /* bcdVersion */
	USB_MICROSOFT_WCID_FEATURE_WINDEX_COMPAT_ID, /* wIndex */
	1, /* bCount */
	{0, 0, 0, 0, 0, 0, 0,}, /* rsv1 */
	DFU_USB_INTERFACE_RUNTIME, /* bFirstInterfaceNumber */
	1, /* rsv2 - set to 1 */
	{'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,}, /* compatibleID[8] */
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}, /* subCompatibleID[8] */
	{0, 0, 0, 0, 0, 0,} /* rsv3[6] */
};

/**
 @name device_descriptor_dfumode
 @brief Device descriptor for DFU Mode.
 */
DESCRIPTOR_QUALIFIER USB_device_descriptor device_descriptor_dfumode =
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
		USB_DFU_BCD_DEVICEID, /* bcdDevice */        // 1.1
		0x01, /* iManufacturer */      // Manufacturer
		0x04, /* iProduct */           // Product
		0x03, /* iSerialNumber */      // Serial Number
		0x01 /* bNumConfigurations */
};

/**
 @name config_descriptor_dfumode
 @brief Config descriptor for DFU Mode.
 */
//@{
struct PACK config_descriptor_dfumode
{
	USB_configuration_descriptor configuration;
	USB_interface_descriptor dfu_interface;
	USB_dfu_functional_descriptor dfu_functional;
};

DESCRIPTOR_QUALIFIER struct config_descriptor_dfumode config_descriptor_dfumode =
{
	{
		0x09, /* configuration.bLength */
		USB_DESCRIPTOR_TYPE_CONFIGURATION, /* configuration.bDescriptorType */
		sizeof(struct config_descriptor_dfumode), /* configuration.wTotalLength */
		0x01, /* configuration.bNumInterfaces */
		0x01, /* configuration.bConfigurationValue */
		0x00, /* configuration.iConfiguration */
		USB_CONFIG_BMATTRIBUTES_VALUE, /* configuration.bmAttributes */
		0xFA /* configuration.bMaxPower */ // 500mA
	},

	// ---- INTERFACE DESCRIPTOR for DFU Interface ----
	{
		0x09, /* dfu_interface.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* dfu_interface.bDescriptorType */
		DFU_USB_INTERFACE_DFUMODE, /* dfu_interface.bInterfaceNumber */
		0x00, /* dfu_interface.bAlternateSetting */
		0x00, /* dfu_interface.bNumEndpoints */
		USB_CLASS_APPLICATION, /* dfu_interface.bInterfaceClass */ // bInterfaceClass: Application Specific Class
		USB_SUBCLASS_DFU, /* dfu_interface.bInterfaceSubClass */ // bInterfaceSubClass: Device Firmware Update
		USB_PROTOCOL_DFU_DFUMODE, /* dfu_interface.bInterfaceProtocol */ // bInterfaceProtocol: Runtime Protocol
		0x05 /* dfu_interface.iInterface */
	},

	// ---- FUNCTIONAL DESCRIPTOR for DFU Interface ----
	{
		0x09, /* dfu_functional.bLength */
		USB_DESCRIPTOR_TYPE_DFU_FUNCTIONAL, /* dfu_functional.bDescriptorType */
		DFU_ATTRIBUTES, /* dfu_functional.bmAttributes */  	// bmAttributes
		DFU_TIMEOUT, /* dfu_functional.wDetatchTimeOut */ // wDetatchTimeOut
		DFU_TRANSFER_SIZE, /* dfu_functional.wTransferSize */     // wTransferSize
		USB_BCD_VERSION_DFU_1_1 /* dfu_functional.bcdDfuVersion */ // bcdDfuVersion: DFU Version 1.1
	}
};
//@}

/**
 @name device_qualifier_descriptor_cdc_comp
 @brief Device qualifier descriptor for Run Time mode.
 */
DESCRIPTOR_QUALIFIER USB_device_qualifier_descriptor device_qualifier_descriptor_cdc_comp =
{
		sizeof(USB_device_qualifier_descriptor), /* bLength */
		USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, /* bDescriptorType */
		USB_BCD_VERSION_2_0, /* bcdUSB */          // V2.0
		USB_CLASS_MISCELLANEOUS, /* bDeviceClass */       // Defined in interface
		USB_SUBCLASS_COMMON_CLASS, /* bDeviceSubClass */ // Defined in interface
		USB_PROTOCOL_INTERFACE_ASSOCIATION, /* bDeviceProtocol */ // Defined in interface
		USB_CONTROL_EP_MAX_PACKET_SIZE, /* bMaxPacketSize0 */
		1, /* bNumConfigurations */
		0
};

/**
 @name wcid_feature_dfumode
 @brief WCID Compatible ID for DFU interface in DFU mode.
 */
//@{
DESCRIPTOR_QUALIFIER USB_WCID_feature_descriptor wcid_feature_dfumode =
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
	{0, 0, 0, 0, 0, 0,} /* rsv3[6] */
};
#endif // DISABLE_DFU
/**
 @name USBD_cdc_line_coding
 @brief UART characteristics set by CDC ACM driver on host.
 */
static USB_CDC_line_coding cdc_line_coding =
{
		115200, /* dwDTERate */
		USB_CDC_STOP_BITS_1, /* bCharFormat */
		USB_CDC_PARITY_NONE, /* bParityType */
		USB_CDC_DATA_BITS_8, /* bDataBits */
};


/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/
void cdc_send_response_notification(void);
void cdc_send_serial_state_notification(void);
size_t uart0Tx(uint8_t *data, size_t len);
size_t uart0Rx(uint8_t *data, size_t len);
void uart0ISR(void);

static struct USBDX_pipe *get_pipe(uint8_t pipe)
{
	static struct USBDX_pipe pipes[4];
	struct USBDX_pipe *ret_val = NULL;

	switch (pipe) {
		case CDC_EP_NOTIFICATION:
			ret_val = &pipes[0];
			break;
		case CDC_EP_DATA_IN:
			ret_val = &pipes[1];
			break;
		case CDC_EP_DATA_OUT:
			ret_val = &pipes[2];
			break;
		default:
			break;

	}
	assert(ret_val!=NULL);
	return ret_val;
}

void USBDX_pipe_isr(uint16_t pipe_bitfields)
{
	if (pipe_bitfields & BIT(CDC_EP_NOTIFICATION))
		USBDX_pipe_process(get_pipe(CDC_EP_NOTIFICATION));
	if (pipe_bitfields & BIT(CDC_EP_DATA_IN))
		USBDX_pipe_process(get_pipe(CDC_EP_DATA_IN));
	if (pipe_bitfields & BIT(CDC_EP_DATA_OUT))
		USBDX_pipe_process(get_pipe(CDC_EP_DATA_OUT));
}

static inline void uart_spr_write(ft900_uart_regs_t *uart,
		uint8_t location, uint8_t val)
{
	uart->SPR_XOFF2 = location;
	uart->LSR_ICR_XON2 = val;
	__asm__("" ::: "memory");
}

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

		// Call the DFU timer to progress state machine.
		// The progression depends on the bmAttributes set in the configuration.
		USBD_DFU_timer();
	}
}

static inline void uart_tx_direct(uint8_t *ptr, uint16_t len)
{
	asm("streamout.b %0,%1,%2"
				::"r"(&UART0->RHR_THR_DLL), "r"(ptr), "r"(len));
}

static void uart_rx_direct(uint8_t *ptr, uint16_t len)
{
	asm("streamini.b %0,%1,%2" ::"r"(ptr), "r"(&UART1->RHR_THR_DLL), "r"(len));
}

static uint8_t *uart_rx_try(uint8_t *ptr, const uint8_t *end)
{
	do {
		*ptr++ = UART0->RHR_THR_DLL;
		if (ptr == end)
			break;
	} while (uart_rx_has_data(UART0));
	return ptr;
}

static inline uint8_t uart_get_intr_status(const ft900_uart_regs_t *dev)
{
	return dev->ISR_FCR_EFR & 0x3F;
}

static void uart_tx(struct USBDX_pipe *pp, struct USBDX_urb *urb)
{
	/* UART got at least TX_SPACE to write */
	uint8_t to_write = TX_FIFO_DEPTH;

	do {
		uint16_t urb_len = usbdx_urb_get_app_to_process(urb);

		if (likely(urb_len > to_write)) {
			/* Part of the URB buffer */
			uart_tx_direct(urb->ptr, to_write);
			urb->ptr += to_write;
			//tfp_printf("UART TX%d len:%d\r\n", urb->id, to_write);
			break;
		} else {
			/* Can send out all URB buffer, queue URB back to USBD */
			uart_tx_direct(urb->ptr, urb_len);
			//tfp_printf("UART TX%d len:%d\r\n", urb->id, urb_len);
			USBDX_submit_urb(pp, urb);
			to_write -= urb_len;
			/* Try read next available URB buffer */
			urb = usbdx_get_app_urb(pp);
			if (!usbdx_urb_owned_by_app(urb))
				break;
		}
	} while(to_write);
}

/**
 The Interrupt which handles asynchronous transmission and reception
 of data into the ring buffer
 */
void uart0ISR(void)
{
	enum UART_INTR_STATUS {
		UART_TX_EMPTY = 0b0010,
		UART_RX_TIMEOUT = 0b1100,
		UART_RX_FULL = 0b0100,
	};

	if (uart_get_intr_status(UART0) == UART_TX_EMPTY) {
		struct USBDX_pipe *pp = get_pipe(CDC_EP_DATA_OUT);

		CRITICAL_SECTION_BEGIN
		struct USBDX_urb *urb = usbdx_get_app_urb(pp);

		if (likely(usbdx_urb_owned_by_app(urb)))
			uart_tx(pp, urb);
		else {
			//tfp_printf("UART TX paused\r\n");
			usbdx_set_app_paused(pp);
			uart_disable_interrupt(UART0, uart_interrupt_tx);
		}
		CRITICAL_SECTION_END
	}

	/* Check RX */
	if (uart_get_intr_status(UART0) == UART_RX_FULL) {
		uint8_t to_read = RX_WATERMARK;
		struct USBDX_pipe *pp = get_pipe(CDC_EP_DATA_IN);

		CRITICAL_SECTION_BEGIN
		/* Got at least RX_WATERMARK bytes to read */
		do {
			/* Force acquire USB IN buffer */
			struct USBDX_urb *urb = USBDX_force_acquire_urb_for_app(pp);

			uint16_t free = usbdx_urb_get_app_to_process(urb);
			/* Don't have enough URB space */
			if (free <= to_read) {
				uart_rx_direct(urb->ptr, free);
				urb->ptr += free;
				to_read -= free;
				//tfp_printf("UART RX%d submit total:%d\r\n",
				//		urb->id, urb_get_app_consumed(urb));
				USBDX_submit_urb(pp, urb);
				continue;
			}

			/* Buffer all */
			uart_rx_direct(urb->ptr, to_read);
			urb->ptr += to_read;
			break;
		} while (to_read);
		CRITICAL_SECTION_END
	}

	if (uart_get_intr_status(UART0) == UART_RX_TIMEOUT) {
		struct USBDX_pipe *pp = get_pipe(CDC_EP_DATA_IN);

		CRITICAL_SECTION_BEGIN
		do {
			struct USBDX_urb *urb = USBDX_force_acquire_urb_for_app(pp);

			/* Buffer is not fully filled */
			urb->ptr = uart_rx_try(urb->ptr, urb->end);
			//tfp_printf("UART RX%d timeout, send total:%d\r\n",
			//		urb->id, urb_get_app_consumed(urb));
			USBDX_submit_urb(pp, urb);

			if (urb->end != urb->ptr)
				break;
		} while (uart_rx_has_data(UART0));
		CRITICAL_SECTION_END
	}
}
void uartCoding(void)
{
	uint16_t divisor;
	uint8_t prescaler;
	uint8_t dataBits;
	uint8_t parity;
	uint8_t stop;

	uart_calculate_baud(cdc_line_coding.dwDTERate, 4, 100000000, &divisor, &prescaler);

	switch(cdc_line_coding.bDataBits)
	{
	// Data bites (5,6,7,8 or 16) NB 16 bits not supported
	default:
	case 8: dataBits = uart_data_bits_8; break;
	case 7: dataBits = uart_data_bits_7; break;
	case 6: dataBits = uart_data_bits_6; break;
	case 5: dataBits = uart_data_bits_5; break;
	};
	switch(cdc_line_coding.bParityType)
	{
	// Parity: 0 - None; 1 - Odd; 2 - Even; 3 - Mark; 4 - Space
	// NB Mark and Space not supported.
	default:
	case 0: parity = uart_parity_none; break;
	case 1: parity = uart_parity_odd; break;
	case 2: parity = uart_parity_even; break;
	};
	switch(cdc_line_coding.bCharFormat)
	{
	// Stop bits: 0 - 1 Stop bit; 1 - 1.5 Stop bit; 2 - 2 Stop bits
	default:
	case 0: stop = uart_stop_bits_1; break;
	case 1: stop = uart_stop_bits_1_5; break;
	case 2: stop = uart_stop_bits_2; break;
	};
	uart_open(UART0, prescaler, divisor, dataBits, parity, stop);

	/* Init 950 mode */
	uint8_t lcr = UART0->LCR_RFL;
	UART0->LCR_RFL = 0xbf;
	__asm__("" ::: "memory");
	UART0->ISR_FCR_EFR |= 1 << 4; /* Enhanced mode */
	UART0->LCR_RFL = lcr;
	__asm__("" ::: "memory");

	UART0->ISR_FCR_EFR = 0x07; /*  FIFO_EN */
	__asm__("" ::: "memory");

#define ICR_ACR 0x00
#define ICR_TTL 0x04
#define ICR_RTL 0x05
	uart_spr_write(UART0, ICR_ACR, 1 << 5);  /* ignore FCR[4:7] */
	uart_spr_write(UART0, ICR_TTL, TX_WATERMARK);
	uart_spr_write(UART0, ICR_RTL, RX_WATERMARK);

	UART0->MCR_XON1_TFL |= (1 << 1);

}

/**
 @brief      USB class request handler
 @details    Handle class requests from the host application.
 The bRequest value is parsed and the appropriate
 action or function is performed. Additional values
 from the USB_device_request structure are decoded
 and the DFU state machine and status updated as
 required.
 @param[in]	req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return		status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bRequest is not
 supported.
 **/
int8_t class_req_cb(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;
	uint8_t interface = LSB(req->wIndex) & 0x0F;
	static uint8_t command[USB_CONTROL_EP_MAX_PACKET_SIZE];
	uint8_t request = req->bRequest;
	uint8_t response[10] = {'\r','\n','O','K','\r','\n','\0','\0','\0','\0'};

	// For DFU requests the SETUP packet must be addressed to the
	// the DFU interface on the device.

	// For DFU requests ensure the recipient is an interface...
	if ((req->bmRequestType & USB_BMREQUESTTYPE_RECIPIENT_MASK) ==
			USB_BMREQUESTTYPE_RECIPIENT_INTERFACE)
	{
		// ...and that the interface is the correct Runtime interface
		if (USBD_DFU_is_runtime())
		{
#ifndef DISABLE_DFU
			if (interface == DFU_USB_INTERFACE_RUNTIME)
			{
				// Handle only DFU_DETATCH, DFU_GETSTATE and DFU_GETSTATUS
				// when in Runtime mode. Table 3.2 DFU_DETACH is mandatory
				// in Runtime mode, DFU_GETSTATE and DFU_GETSTATUS are
				// optional.
				switch (request)
				{
				case USB_CLASS_REQUEST_DETACH:
					USBD_DFU_class_req_detach(req->wValue);
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
				}
			}
			else
#endif // DISABLE_DFU
			{
				switch (request)
				{
				case USB_CDC_REQUEST_SEND_ENCAPSULATED_COMMAND:
					// Read and ignore encapsulated command.
					USBD_transfer_ep0(USBD_DIR_OUT, command, USB_CONTROL_EP_MAX_PACKET_SIZE, req->wLength);
					// ACK
					USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
					tfp_printf("Received encapsulated command \"%s\"\r\n",command);
					cdc_send_response_notification();
					break;
				case USB_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE:
					if ((command[0] == 'A') && (command[1] == 'T')&& (command[2] == 'Z'))
					{
						USBD_transfer_ep0(USBD_DIR_IN, (uint8_t*)&response, sizeof(response), req->wLength);
						// ACK
						USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);
						status = USBD_OK;
						tfp_printf("Sent encapsulated response!\r\n");
					}
					break;
				case USB_CDC_REQUEST_SET_COMM_FEATURE:
				case USB_CDC_REQUEST_GET_COMM_FEATURE:
				case USB_CDC_REQUEST_CLEAR_COMM_FEATURE:
					break;

				case USB_CDC_REQUEST_SET_LINE_CODING:
					USBD_transfer_ep0(USBD_DIR_OUT, (uint8_t*)&cdc_line_coding, sizeof(USB_CDC_line_coding), req->wLength);
					// ACK
					USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
					// Update UART settings
					tfp_printf("set baud %ld, par %d, stop %d, data %d\r\n",
							cdc_line_coding.dwDTERate,
							cdc_line_coding.bParityType,
							cdc_line_coding.bCharFormat,
							cdc_line_coding.bDataBits);
					uartCoding();
					cdc_send_serial_state_notification();
					status = USBD_OK;
					break;

				case USB_CDC_REQUEST_GET_LINE_CODING:
					USBD_transfer_ep0(USBD_DIR_IN, (uint8_t*)&cdc_line_coding, sizeof(USB_CDC_line_coding), req->wLength);
					// ACK
					USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);
					status = USBD_OK;
					tfp_printf("get baud %ld, par %d, stop %d, data %d\r\n",
							cdc_line_coding.dwDTERate,
							cdc_line_coding.bParityType,
							cdc_line_coding.bCharFormat,
							cdc_line_coding.bDataBits);
					break;

				case USB_CDC_REQUEST_SET_CONTROL_LINE_STATE:
					// Ignore the control line state
					// ACK
					USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
					status = USBD_OK;
					break;

				case USB_CDC_REQUEST_SEND_BREAK :
					//USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
					break;

				}
			}
		}
		// ...or the correct DFU Mode interface
		else
		{
#ifndef DISABLE_DFU
			if (interface == DFU_USB_INTERFACE_DFUMODE)
			{
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
#endif			
		}
	}

	return status;
}
#define CLAMP(t, max) if ((t) > (max)) (t) = (max)
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
 @param[in]	req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return		status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bmRequestType is not
 supported.
 **/
int8_t standard_req_get_descriptor(USB_device_request *req, uint8_t **buffer, uint16_t *len)
{
	uint8_t *src = NULL;
	uint16_t length = req->wLength;
	uint8_t hValue = req->wValue >> 8;
	uint8_t lValue = req->wValue & 0x00ff;
	uint8_t i;
	uint8_t slen;

	switch (hValue)
	{
	case USB_DESCRIPTOR_TYPE_DEVICE:
#ifndef DISABLE_DFU
		if (USBD_DFU_is_runtime())
		{
			src = (uint8_t *) &device_descriptor_cdcacm;
		}
		else
		{
			src = (uint8_t *) &device_descriptor_dfumode;
		}
#else
		src = (uint8_t *) &device_descriptor_cdcacm;
#endif // DISABLE_DFU

		if (length > sizeof(USB_device_descriptor)) // too many bytes requested
				length = sizeof(USB_device_descriptor); // Entire structure.
		break;

	case USB_DESCRIPTOR_TYPE_CONFIGURATION:
#ifndef DISABLE_DFU
		if (USBD_DFU_is_runtime())
		{
			src = (uint8_t *) &config_descriptor_buffer.hs;
			if (USBD_get_bus_speed() == USBD_SPEED_HIGH)
			{
				memcpy((void *)&config_descriptor_buffer.hs, (void *)&config_descriptor_cdcacm_hs, sizeof(config_descriptor_cdcacm_hs));
				if (length > sizeof(config_descriptor_cdcacm_hs)) // too many bytes requested
					length = sizeof(config_descriptor_cdcacm_hs); // Entire structure.
			}
			else
			{
				memcpy((void *)&config_descriptor_buffer.fs, (void *)&config_descriptor_cdcacm_fs, sizeof(config_descriptor_cdcacm_fs));
				if (length > sizeof(config_descriptor_cdcacm_fs)) // too many bytes requested
					length = sizeof(config_descriptor_cdcacm_fs); // Entire structure.
			}
			config_descriptor_buffer.hs.configuration.bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION;
		}
		else
		{
			src = (uint8_t *) &config_descriptor_dfumode;
			if (length > sizeof(config_descriptor_dfumode)) // too many bytes requested
				length = sizeof(config_descriptor_dfumode); // Entire structure.
		}
#else // DISABLE_DFU
		src = (uint8_t *) &config_descriptor_buffer.hs;
		if (USBD_get_bus_speed() == USBD_SPEED_HIGH)
		{
			memcpy((void *)&config_descriptor_buffer.hs, (void *)&config_descriptor_cdcacm_hs, sizeof(config_descriptor_cdcacm_hs));
			if (length > sizeof(config_descriptor_cdcacm_hs)) // too many bytes requested
				length = sizeof(config_descriptor_cdcacm_hs); // Entire structure.
		}
		else
		{
			memcpy((void *)&config_descriptor_buffer.fs, (void *)&config_descriptor_cdcacm_fs, sizeof(config_descriptor_cdcacm_fs));
			if (length > sizeof(config_descriptor_cdcacm_fs)) // too many bytes requested
				length = sizeof(config_descriptor_cdcacm_fs); // Entire structure.
		}
		config_descriptor_buffer.hs.configuration.bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION;
#endif // DISABLE_DFU
		break;

	case USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION:
#ifndef DISABLE_DFU
		if (USBD_DFU_is_runtime())
		{
			src = (uint8_t *) &config_descriptor_buffer.hs;
			if (USBD_get_bus_speed() == USBD_SPEED_HIGH)
			{
				memcpy((void *)&config_descriptor_buffer.fs, (void *)&config_descriptor_cdcacm_fs, sizeof(config_descriptor_cdcacm_fs));
				if (length > sizeof(config_descriptor_cdcacm_fs)) // too many bytes requested
					length = sizeof(config_descriptor_cdcacm_fs); // Entire structure.
			}
			else
			{
				memcpy((void *)&config_descriptor_buffer.hs, (void *)&config_descriptor_cdcacm_hs, sizeof(config_descriptor_cdcacm_hs));
				if (length > sizeof(config_descriptor_cdcacm_hs)) // too many bytes requested
					length = sizeof(config_descriptor_cdcacm_hs); // Entire structure.
			}
			config_descriptor_buffer.hs.configuration.bDescriptorType = USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION;
		}
		else
		{
			src = (uint8_t *) &config_descriptor_dfumode;
			if (length > sizeof(config_descriptor_dfumode)) // too many bytes requested
				length = sizeof(config_descriptor_dfumode); // Entire structure.
		}
#else // DISABLE_DFU
		src = (uint8_t *) &config_descriptor_buffer.hs;
		if (USBD_get_bus_speed() == USBD_SPEED_HIGH)
		{
			memcpy((void *)&config_descriptor_buffer.fs, (void *)&config_descriptor_cdcacm_fs, sizeof(config_descriptor_cdcacm_fs));
			if (length > sizeof(config_descriptor_cdcacm_fs)) // too many bytes requested
				length = sizeof(config_descriptor_cdcacm_fs); // Entire structure.
		}
		else
		{
			memcpy((void *)&config_descriptor_buffer.hs, (void *)&config_descriptor_cdcacm_hs, sizeof(config_descriptor_cdcacm_hs));
			if (length > sizeof(config_descriptor_cdcacm_hs)) // too many bytes requested
				length = sizeof(config_descriptor_cdcacm_hs); // Entire structure.
		}
		config_descriptor_buffer.hs.configuration.bDescriptorType = USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION;
#endif // DISABLE_DFU
		break;

	case USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER:
		src = (uint8_t *) &device_qualifier_descriptor_cdc_comp;
		CLAMP(length, sizeof(USB_device_qualifier_descriptor));
		break;

	case USB_DESCRIPTOR_TYPE_STRING:
#ifndef DISABLE_DFU
		// Special case: WCID descriptor
		if (lValue == USB_MICROSOFT_WCID_STRING_DESCRIPTOR)
		{
			src = (uint8_t *)wcid_string_descriptor;
			length = sizeof(wcid_string_descriptor);
			break;
		}
#endif // DISABLE_DFU
		// Find the nth string in the string descriptor table
		i = 0;
		while ((slen = string_descriptor[i]) > 0)
		{
			// Point to start of string descriptor in __code segment
			// Typecast prevents warning for losing const when USBD_transfer
			// is called
			src = (uint8_t *) &string_descriptor[i];
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
 @param[in]	req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return		status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bRequest is not
 supported.
 **/
int8_t vendor_req_cb(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;
#ifndef DISABLE_DFU
	uint16_t length = req->wLength;

	// For Microsoft WCID only.
	// Request for Compatible ID Feature Descriptors.
	// Check the request code and wIndex.
	if ((req->bRequest == WCID_VENDOR_REQUEST_CODE) && (req->bmRequestType & USB_BMREQUESTTYPE_DIR_DEV_TO_HOST))
	{
		if (req->wIndex == USB_MICROSOFT_WCID_FEATURE_WINDEX_COMPAT_ID)
		{
			if (length > sizeof(wcid_feature_runtime)) // too many bytes requested
				length = sizeof(wcid_feature_runtime); // Entire structure.
			// Return a compatible ID feature descriptor.
			if (USBD_DFU_is_runtime())
			{
				USBD_transfer_ep0(USBD_DIR_IN, (uint8_t *) &wcid_feature_runtime,
						length, length);
			}
			else
			{
				USBD_transfer_ep0(USBD_DIR_IN, (uint8_t *) &wcid_feature_dfumode,
						length, length);
			}
			// ACK packet
			USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);
			status = USBD_OK;
		}
		else if (req->wIndex == USB_MICROSOFT_WCID_FEATURE_WINDEX_DEVICE_GUID)
		{
			/*  as opposed to the Compatible ID, Device GUID can be queried using an Interface or/and Device Request */
			if (((req->bmRequestType & USB_BMREQUESTTYPE_RECIPIENT_MASK) == USB_BMREQUESTTYPE_RECIPIENT_DEVICE)
					|| ((req->bmRequestType & USB_BMREQUESTTYPE_RECIPIENT_MASK) == USB_BMREQUESTTYPE_RECIPIENT_INTERFACE))
			{
				if (LSB(req->wValue) == DFU_USB_INTERFACE_RUNTIME)
				{
					if (length > sizeof(usbd_cdcacm_device_interface_guid)) // too many bytes requested
					{
						length = sizeof(usbd_cdcacm_device_interface_guid); // Entire structure.
					}
					// Return the extended property for the interface requested.
					USBD_transfer_ep0(USBD_DIR_IN, (uint8_t *)&usbd_cdcacm_device_interface_guid,
							length, length);
					// ACK packet
					USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);
					status = USBD_OK;
				}
			}
		}
	}
#endif // DISABLE_DFU
	(void)req;

	return status;
}

/**
 @brief      USB suspend callback
 @details    Called when the USB bus enters the suspend state.
 @param[in]	status - unused.
 @return		N/A
 **/
void suspend_cb(uint8_t status)
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
void resume_cb(uint8_t status)
{
	(void) status;
	return;
}

/**
 @brief      USB reset callback
 @details    Called when the USB bus enters the reset state.
 @param[in]	status - unused.
 @return		N/A
 **/
void reset_cb(uint8_t status)
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

/* For debugging endpoint transactions. */
void ep_cb(USBD_ENDPOINT_NUMBER ep_number)
{
	tfp_printf("EP%d\r\n", ep_number);
}

void cdc_send_serial_state_notification(void)
{
	struct { // Note: structure must be packed
		USB_CDC_Notification header;
		union {
			uint16_t buffer;
			USB_CDC_UartStateBitmap serialState;
		};
	} notification;

	notification.header.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_CLASS |
			USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
	notification.header.bNotification = USB_CDC_NOTIFICAITON_SERIAL_STATE;
	notification.header.wValue = 0;
	notification.header.wIndex = 0; // CDC ACM interface
	notification.header.wLength = 2;
	notification.buffer = 0;
	// All Serial State bits are set to zero (disabled).
	notification.serialState.bRxCarrier = 0;
	notification.serialState.bTxCarrier = 0;

	struct USBDX_pipe *pp = get_pipe(CDC_EP_NOTIFICATION);

	CRITICAL_SECTION_BEGIN
		/* Force acquire USB IN buffer */
		struct USBDX_urb *urb = USBDX_force_acquire_urb_for_app(pp);

		uint16_t free = usbdx_urb_get_app_to_process(urb);
		/* Don't have enough URB space */
		if (free >= sizeof(notification)) {
			memcpy(urb->ptr, &notification, sizeof(notification));
			urb->ptr += sizeof(notification);
			USBDX_submit_urb(pp, urb);
		}
	CRITICAL_SECTION_END
}

void cdc_send_response_notification(void)
{
	USB_CDC_Notification header;

	header.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_CLASS |
			USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
	header.bNotification = USB_CDC_NOTIFICATION_RESPONSE_AVAILABLE;
	header.wValue = 0;
	header.wIndex = 0; // CDC ACM interface
	header.wLength = 2;

	struct USBDX_pipe *pp = get_pipe(CDC_EP_NOTIFICATION);

	CRITICAL_SECTION_BEGIN
		/* Force acquire USB IN buffer */
		struct USBDX_urb *urb = USBDX_force_acquire_urb_for_app(pp);

		uint16_t free = usbdx_urb_get_app_to_process(urb);
		/* Don't have enough URB space */
		if (free >= sizeof(USB_CDC_Notification)) {
			memcpy(urb->ptr, &header, sizeof(USB_CDC_Notification));
			urb->ptr += sizeof(USB_CDC_Notification);
			USBDX_submit_urb(pp, urb);
		}
	CRITICAL_SECTION_END
}

bool acm_out_on_data_ready(struct USBDX_pipe *pp)
{
	//tfp_printf("UART TX resumed\r\n");
	uart_tx(pp, usbdx_get_app_urb(pp));
	/* Enable the UART to fire interrupts when transmitting data... */
	uart_enable_interrupt(UART0, uart_interrupt_tx);
	return true;
}

void cdc_uart_init(void)
{
#define ACM_URB_INT_COUNT	4
#define ACM_URB_IN_COUNT	4
#define ACM_URB_OUT_COUNT	4

	USBD_create_endpoint(CDC_EP_NOTIFICATION, USBD_EP_INT, USBD_DIR_IN,
			CDC_NOTIFICATION_USBD_EP_SIZE, USBD_DB_OFF, NULL /*ep_cb*/);
	if (USBD_get_bus_speed() == USBD_SPEED_HIGH)
	{
		USBD_create_endpoint(CDC_EP_DATA_OUT, USBD_EP_BULK, USBD_DIR_OUT,
				CDC_DATA_USBD_EP_SIZE_HS, USBD_DB_ON, NULL /*ep_cb*/);
		USBD_create_endpoint(CDC_EP_DATA_IN, USBD_EP_BULK, USBD_DIR_IN,
				CDC_DATA_USBD_EP_SIZE_HS, USBD_DB_ON, NULL /*ep_cb*/);
	}
	else
	{
		USBD_create_endpoint(CDC_EP_DATA_OUT, USBD_EP_BULK, USBD_DIR_OUT,
				CDC_DATA_USBD_EP_SIZE_FS, USBD_DB_ON, NULL /*ep_cb*/);
		USBD_create_endpoint(CDC_EP_DATA_IN, USBD_EP_BULK, USBD_DIR_IN,
				CDC_DATA_USBD_EP_SIZE_FS, USBD_DB_ON, NULL /*ep_cb*/);
	}
	static uint8_t acm_int_buf[CDC_NOTIFICATION_EP_SIZE * ACM_URB_INT_COUNT] __attribute__ ((aligned (16)));
	static uint8_t acm_in_buf[CDC_DATA_EP_SIZE_HS * ACM_URB_IN_COUNT];
	static uint8_t acm_out_buf[CDC_DATA_EP_SIZE_HS * ACM_URB_OUT_COUNT];
	static struct USBDX_urb acm_int[ACM_URB_INT_COUNT];
	static struct USBDX_urb acm_in[ACM_URB_IN_COUNT];
	static struct USBDX_urb acm_out[ACM_URB_OUT_COUNT];
	struct USBDX_pipe *pp;

	/* ACM */
	pp = get_pipe(CDC_EP_NOTIFICATION);
	USBDX_pipe_init(pp, CDC_EP_NOTIFICATION, CDC_EP_NOTIFICATION | 0x80,
			acm_int, acm_int_buf, ACM_URB_INT_COUNT);

	pp = get_pipe(CDC_EP_DATA_IN);
	USBDX_pipe_init(pp, CDC_EP_DATA_IN, CDC_EP_DATA_IN | 0x80,
			acm_in, acm_in_buf, ACM_URB_IN_COUNT);

	pp = get_pipe(CDC_EP_DATA_OUT);
	USBDX_pipe_init(pp, CDC_EP_DATA_OUT, CDC_EP_DATA_OUT,
			acm_out, acm_out_buf, ACM_URB_OUT_COUNT);
	USBDX_register_on_ready(pp, acm_out_on_data_ready);
}

void cdc_uart_bridge(void)
{
	int8_t status;

	cdc_send_serial_state_notification();

	do
	{
		status = (USBD_get_state() < USBD_STATE_DEFAULT)?USBD_ERR_NOT_CONFIGURED:USBD_OK;

	} while (status == USBD_OK);

	// Disconnected from host.
}

uint8_t usbd_testing(void)
{
	int8_t status;
	USBD_ctx usb_ctx;

	memset(&usb_ctx, 0, sizeof(usb_ctx));

	usb_ctx.standard_req_cb = NULL;
	usb_ctx.get_descriptor_cb = standard_req_get_descriptor;
	usb_ctx.class_req_cb = class_req_cb;
	usb_ctx.vendor_req_cb = vendor_req_cb;
	usb_ctx.suspend_cb = suspend_cb;
	usb_ctx.resume_cb = resume_cb;
	usb_ctx.reset_cb = reset_cb;
	usb_ctx.lpm_cb = NULL;
	usb_ctx.speed = USBD_SPEED_HIGH;

	// Initialise the USB device with a control endpoint size
	// of 8 to 64 bytes. This must match the size for bMaxPacketSize0
	// defined in the device descriptor.
	usb_ctx.ep0_size = USB_CONTROL_EP_SIZE;
	//usb_ctx.ep0_cb = ep_cb;

	USBD_initialise(&usb_ctx);

	for (;;)
	{
		if (!USBD_is_connected())
		{
			continue;
		}
		/* Get USB speed */
		USBD_attach();
		USBD_connect();
		delayms(1000); //1s delay to get the device configured
		cdc_uart_init();
		if (USBD_get_state() >= USBD_STATE_DEFAULT)
		{
			if (USBD_DFU_is_runtime())
			{
				// Start the CDC emulation code.
				cdc_uart_bridge();
			}
			else
			{
				// In DFU mode. Process USB requests.
				do{
					status = (USBD_get_state() < USBD_STATE_DEFAULT)?USBD_ERR_NOT_CONFIGURED:USBD_OK;
				}while (status == USBD_OK);
			}
		}
		tfp_printf("Restarting\r\n");
	}

	return 0;
}

void powermanagement_ISR(void)
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

/* FUNCTIONS ***********************************************************************/

int main(void)
{
	sys_reset_all();

	/**** acm init ****/
#if defined(__FT930__)
    /* Make GPIO23 function as UART0_TXD and GPIO22 function as UART0_RXD... */
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#else
	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#endif

	// Open the UART using the coding required.
	uartCoding();

	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
	);

	/* Enable tfp_printf() functionality... */
	init_printf(UART0, tfp_putc);

	sys_enable(sys_device_timer_wdt);

	//interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, ISR_timer);
	/* Attach the interrupt so it can be called... */
	interrupt_attach(interrupt_uart0, (uint8_t) interrupt_uart0, uart0ISR);
	/* Enable the UART to fire interrupts when receiving data... */
	uart_enable_interrupt(UART0, uart_interrupt_rx);
	/* Enable interrupts to be fired... */
	uart_enable_interrupts_globally(UART0);

	/* Timer A = 1ms */
	timer_prescaler(timer_select_a, 1000);
	timer_init(timer_select_a, 100, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
	timer_enable_interrupt(timer_select_a);
	timer_start(timer_select_a);

	interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, ISR_timer);

	interrupt_attach(interrupt_0, (int8_t)interrupt_0, powermanagement_ISR);
	interrupt_enable_globally();

	tfp_printf("(C) Copyright, Bridgetek Pte Ltd \r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");
	tfp_printf("Welcome to USBDCDC ACM Tester Example 1... \r\n");
	tfp_printf("\r\n");
	tfp_printf("Emulate a CDC ACM device connected to the USB Device Port.\r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");

	usbd_testing();

	interrupt_detach(interrupt_timers);
	interrupt_disable_globally();

	sys_disable(sys_device_timer_wdt);

	// Wait forever...
	for (;;);

	return 0;
}


