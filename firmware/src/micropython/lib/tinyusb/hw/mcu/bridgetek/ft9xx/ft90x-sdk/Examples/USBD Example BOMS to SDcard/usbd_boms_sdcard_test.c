/**
 @file usbd_boms_sdcard_test.c
 @brief

 This example program will create FT900 as USB mass storage device
 on the USB. Data for the mass storage device will be read/written
 to an SD card.
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
#include <string.h>
#include <ctype.h>

#include <ft900.h>
#include "ft900_sdhost.h"
#include "ft900_usb_boms.h"

#include <ft900_timers.h>
#include <ft900_sys.h>
#include <ft900_interrupt.h>
#include <ft900_gpio.h>
#include <ft900_usb.h>
#include <ft900_usb_dfu.h>
#include <ft900_usbd.h>
#include <ft900_usb_boms.h>
#include <ft900_usbd_dfu.h>
#include <ft900_delay.h>
#include <ft900_sdhost.h>
#include <registers/ft900_registers.h>
#include <ft900_usbdx.h>

/* UART support for printf output. */
#include <ft900_uart_simple.h>
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

#undef BOMS_DEBUG_REQUESTS
#undef BOMS_DEBUG_SCSI_COMMANDS
#undef BOMS_DEBUG_SCSI_READ_WRITE_COMMANDS
#define BOMS_DEBUG_ERRORS
#undef BOMS_DEBUG_MSC

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
 @brief Endpoint definitions for BOMS device.
 @details The application requires to have one BULK IN and one BULK
 	 	 OUT endpoint, both of size 512 bytes. This is checked against
 	 	 the declared block size of the SDHOST (i.e. SD Card interface).
 */
//@{
#define BOMS_EP_DATA_IN 				USBD_EP_1
#define BOMS_EP_DATA_OUT 				USBD_EP_2
#define BOMS_DATA_EP_SIZE				0x200
#define BOMS_DATA_USBD_EP_SIZE 			USBD_EP_SIZE_512
#if BOMS_DATA_EP_SIZE != SDHOST_BLK_SIZE
#error Endpoint size must be the same as the SD card sector size
#endif
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
 @name USB_PID_MASS_STORAGE configuration.
 @brief Run Time Product ID for keyboard function.
 */
//@{
#define USB_PID_MASS_STORAGE 0x0fd5
//@}

/**
 @name DFU_USB_PID_DFUMODE configuration.
 @brief FTDI predefined DFU Mode Product ID.
 */
//@{
#define DFU_USB_PID_DFUMODE 0x0fde
//@}

/**
 @name DFU_USB_INTERFACE configuration.
 @brief Run Time and DFU Mode Interface Numbers.
 */
//@{
#define DFU_USB_INTERFACE_RUNTIME 1
#define DFU_USB_INTERFACE_DFUMODE 0
//@}

/**
   @name Disable DFU function
   @brief Disable the DFU interface in this application.
   	   Normally enabled.
*/
#undef DISABLE_DFU

/**
 @name DFU Configuration
 @brief Determines the parts of the DFU specification which are supported
        by the DFU library code. Features can be disabled if required.
 */
//@{
#define DFU_ATTRIBUTES USBD_DFU_ATTRIBUTES
//@}

/**
 @name WCID_VENDOR_REQUEST_CODE for WCID.
 @brief Unique vendor request code for WCID OS Vendor Extension validation.
*/
//@{
#define WCID_VENDOR_REQUEST_CODE	 0x4A   /*same WCID code as that of B-DFU */
//@}

/**
 @name GPIO_SD_<signal> configuration.
 @brief GPIO mappings for SD Card pins.
 */
//@{
#if defined(__FT930__)
#define GPIO_SD_CLK  (0)
#define GPIO_SD_CMD  (1)
#define GPIO_SD_DAT3 (6)
#define GPIO_SD_DAT2 (5)
#define GPIO_SD_DAT1 (4)
#define GPIO_SD_DAT0 (3)
#define GPIO_SD_CD   (2)
#define GPIO_SD_WP   (7)
#else
#define GPIO_SD_CLK  (19)
#define GPIO_SD_CMD  (20)
#define GPIO_SD_DAT3 (21)
#define GPIO_SD_DAT2 (22)
#define GPIO_SD_DAT1 (23)
#define GPIO_SD_DAT0 (24)
#define GPIO_SD_CD   (25)
#define GPIO_SD_WP   (26)
#endif
//@}

/**
 * \brief Macro to swap CPU endian to BE used on USB.
 */
//@{
#define SWAP_16(x) \
    ((((x) & 0xff00) >> 8) | \
     (((x) & 0x00ff) << 8))
#define SWAP_32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >>  8) | \
     (((x) & 0x0000ff00) <<  8) | \
     (((x) & 0x000000ff) << 24))
//@}
#if BYTE_ORDER == LITTLE_ENDIAN
#define CPU2BOMS_16(A) (A)
#define BOMS2CPU_16(A) (A)
#define CPU2BOMS_32(A) (A)
#define BOMS2CPU_32(A) (A)
#define CPU2SCSI_16(A) SWAP_16(A)
#define SCSI2CPU_16(A) SWAP_16(A)
#define CPU2SCSI_32(A) SWAP_32(A)
#define SCSI2CPU_32(A) SWAP_32(A)
#else
#define CPU2BOMS_16(A) SWAP_16(A)
#define BOMS2CPU_16(A) SWAP_16(A)
#define CPU2BOMS_32(A) SWAP_32(A)
#define BOMS2CPU_32(A) SWAP_32(A)
#define CPU2SCSI_16(A) (A)
#define SCSI2CPU_16(A) (A)
#define CPU2SCSI_32(A) (A)
#define SCSI2CPU_32(A) (A)
#endif

#define BOMS_USBS BOMS2CPU_32(BOMS_SIG_USBS) // Little Endian converted to CPU
#define BOMS_USBC BOMS2CPU_32(BOMS_SIG_USBC) // Little Endian converted to CPU

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

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
 @name usbd_boms_device_interface_guid
 @brief Device Interface GUID for DFU interface in runtime.
 */
/* Referred:
 * https://github.com/pbatard/libwdi/wiki/WCID-Devices
 */
static const struct PACK USB_WCID_extended_property_descriptor usbd_boms_device_interface_guid =
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
	/* Property Value -> {B28892D3-61CE-4BF0-B89C-DDA4FA13533A} */
	{0x7b, 0x00, 0x42, 0x00, 0x32, 0x00, 0x38, 0x00, 0x38, 0x00,
			0x39, 0x00, 0x32, 0x00, 0x44, 0x00, 0x33, 0x00, 0x2d, 0x00,
			0x36, 0x00, 0x31, 0x00, 0x43, 0x00, 0x45, 0x00, 0x2d, 0x00,
			0x34, 0x00, 0x42, 0x00, 0x46, 0x00, 0x30, 0x00, 0x2d, 0x00,
			0x42, 0x00, 0x38, 0x00, 0x39, 0x00, 0x43, 0x00, 0x2d, 0x00,
			0x44, 0x00, 0x44, 0x00, 0x41, 0x00, 0x34, 0x00, 0x46, 0x00,
			0x41, 0x00, 0x31, 0x00, 0x33, 0x00, 0x35, 0x00, 0x33, 0x00,
			0x33, 0x00, 0x41, 0x00, 0x7d, 0x00,
	 0x00, 0x00, 0x00, 0x00}, /* bPropertyData */
};


/**
 @brief Timer operated signal that the SD card has been removed from the SDhost.
 */
static volatile int sd_card_connected = 0;
static volatile int sd_card_connect_change = 0;
static int sd_card_initialised = 0;

 /**
  @brief Sense key to return for SCSI REQUEST SENSE
  */
 static volatile uint8_t scsi_sense = SCSI_SENSE_KEY_NO_SENSE;
 static volatile uint16_t scsi_addn_sense = SCSI_SENSE_KEY_ADDN_NO_ERROR;

 /**
 @name string_descriptor
 @brief Table of USB String descriptors
 */
DESCRIPTOR_QUALIFIER /*__at(STRING_DESCRIPTOR_LOCATION)*/uint8_t string_descriptor[STRING_DESCRIPTOR_ALLOCATION] =
{
		// String 0 is actually a list of language IDs supported by the real strings.
		0x04, USB_DESCRIPTOR_TYPE_STRING, 0x09,
		0x04, // 0409 = English (US)

		// String 1 (Manufacturer): "FTDI"
		0x0a, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, 'D', 0x00, 'I',
		0x00,

		//	String 2 (Product): "Mass Storage Device"
		 0x28, USB_DESCRIPTOR_TYPE_STRING, 'M', 0x00, 'a', 0x00, 's', 0x00, 's',
		 0x00, ' ', 0x00, 'S', 0x00, 't', 0x00, 'o', 0x00, 'r', 0x00, 'a', 0x00, 'g', 0x00,
		 'e', 0x00, ' ', 0x00, 'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00,

		// String 3 (Serial Number): "FT424242" 8keys
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
		0x00 };

/**
 @name wcid_string_descriptor
 @brief USB String descriptor for WCID identification.
 */
DESCRIPTOR_QUALIFIER uint8_t wcid_string_descriptor[USB_MICROSOFT_WCID_STRING_LENGTH] = {
		USB_MICROSOFT_WCID_STRING(WCID_VENDOR_REQUEST_CODE)
};

/**
 @name device_descriptor_keyboard
 @brief Device descriptor for Run Time mode.
 */
DESCRIPTOR_QUALIFIER USB_device_descriptor device_descriptor_usbstorage =
{
	0x12, /* bLength */
	USB_DESCRIPTOR_TYPE_DEVICE, /* bDescriptorType */
	USB_BCD_VERSION_2_0, /* bcdUSB */          // V2.0
	USB_CLASS_DEVICE, /* bDeviceClass */       // Defined in interface
	USB_SUBCLASS_DEVICE, /* bDeviceSubClass */ // Defined in interface
	USB_PROTOCOL_DEVICE, /* bDeviceProtocol */ // Defined in interface
	USB_CONTROL_EP_MAX_PACKET_SIZE, /* bMaxPacketSize0 */
	USB_VID_FTDI, /* idVendor */   // idVendor: 0x0403 (FTDI)
	USB_PID_MASS_STORAGE, /* idProduct */ // idProduct: 0x0fed
	0x1100, /* bcdDevice */        // 1.1
	0x01, /* iManufacturer */      // Manufacturer
	0x02, /* iProduct */           // Product
	0x03, /* iSerialNumber */      // Serial Number
	0x01 /* bNumConfigurations */
};

/**
 @name config_descriptor_keyboard
 @brief Configuration descriptor for Run Time mode.
 */
//@{
struct PACK config_descriptor_mass_storage
{
	USB_configuration_descriptor configuration;
	USB_interface_descriptor interface;
	USB_endpoint_descriptor endpointIn;
	USB_endpoint_descriptor endpointOut;
#ifndef DISABLE_DFU
	USB_interface_descriptor dfu_interface;
	USB_dfu_functional_descriptor dfu_functional;
#endif // DISABLE_DFU
};


DESCRIPTOR_QUALIFIER struct config_descriptor_mass_storage config_descriptor_mass_storage =
{
	{
		sizeof(USB_configuration_descriptor), /* configuration.bLength */
		USB_DESCRIPTOR_TYPE_CONFIGURATION, /* configuration.bDescriptorType */
		sizeof(struct config_descriptor_mass_storage), /* configuration.wTotalLength */
#ifndef DISABLE_DFU
		0x02, /* configuration.bNumInterfaces */
#else
		0x01, /* configuration.bNumInterfaces */
#endif
		0x01, /* configuration.bConfigurationValue */
		0x00, /* configuration.iConfiguration */
		USB_CONFIG_BMATTRIBUTES_VALUE, /* configuration.bmAttributes */
		0xFA /* configuration.bMaxPower */           // 500mA
	},
	{
	//		USB_interface_descriptor
	    sizeof(USB_interface_descriptor), /* interface.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* interface.bDescriptorType */
		0, /* interface.bInterfaceNumber */
		0x00, /* interface.bAlternateSetting */
		0x02, /* interface.bNumEndpoints */
		USB_CLASS_MASS_STORAGE, /* interface.bInterfaceClass */ // HID Class
		USB_SUBCLASS_MASS_STORAGE_SCSI, /* interface.bInterfaceSubClass */
		USB_PROTOCOL_MASS_STORAGE_BOMS, /* interface.bInterfaceProtocol */
		0x00, /* interface.iInterface */               // "FT900 Keyboard"
	},
	{
		sizeof(USB_endpoint_descriptor), /* endpoint_data_in.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_IN | BOMS_EP_DATA_IN, /* endpoint.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_BULK, /* endpoint.bmAttributes */
		BOMS_DATA_EP_SIZE, /* endpoint.wMaxPacketSize */
		0xFF, /* endpoint.bInterval */
	},
	{
		sizeof(USB_endpoint_descriptor), /* endpoint_data_out.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_data_out.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_OUT | BOMS_EP_DATA_OUT, /* endpoint.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_BULK, /* endpoint.bmAttributes */
		BOMS_DATA_EP_SIZE, /* endpoint.wMaxPacketSize */
		0xFF, /* endpoint.bInterval */
	},
#ifndef DISABLE_DFU
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
	{
		sizeof(USB_dfu_functional_descriptor), /* dfu_functional.bLength */
		USB_DESCRIPTOR_TYPE_DFU_FUNCTIONAL, /* dfu_functional.bDescriptorType */
		DFU_ATTRIBUTES, /* dfu_functional.bmAttributes */
		DFU_TIMEOUT, /* dfu_functional.wDetatchTimeOut */ // wDetatchTimeOut
		DFU_TRANSFER_SIZE, /* dfu_functional.wTransferSize */ // wTransferSize
		USB_BCD_VERSION_DFU_1_1 /* dfu_functional.bcdDfuVersion */ // bcdDfuVersion: DFU Version 1.1
	}
#endif // DISABLE_DFU
};

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
	{0, 0, 0, 0, 0, 0} /* rsv3[6] */
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

// Fields marked with * are updated with information in device_config[]
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
		0x05 /* dfu_interface.iInterface */
	},
	{
		// ---- FUNCTIONAL DESCRIPTOR for DFU Interface ----
		0x09, /* dfu_functional.bLength */
		USB_DESCRIPTOR_TYPE_DFU_FUNCTIONAL, /* dfu_functional.bDescriptorType */
		DFU_ATTRIBUTES, /* dfu_functional.bmAttributes */
		DFU_TIMEOUT, /* dfu_functional.wDetatchTimeOut */ // wDetatchTimeOut
		DFU_TRANSFER_SIZE, /* dfu_functional.wTransferSize */     // wTransferSize
		USB_BCD_VERSION_DFU_1_1 /* dfu_functional.bcdDfuVersion */ // bcdDfuVersion: DFU Version 1.1
	}
};
//@}

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

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/** @name tfp_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
void tfp_putc(void* p, char c)
{
	uart_write((ft900_uart_regs_t*) p, (uint8_t) c);
}

void ISR_timer(void)
{
	static int prev_connected;
	static int timer_connected;

	if (timer_is_interrupted(timer_select_a))
	{
		// Call USB timer handler to action transaction and hub timers.
		// USBD_timer();

		// Check for removal and insertion of SD cards.
		// There is a debounce interval encoded here.
		prev_connected = sd_card_connected;
		sd_card_connected = sdhost_card_detect();

		if (prev_connected != sd_card_connected)
		{
			// Immediately remove card.
			if (sd_card_connected == SDHOST_CARD_REMOVED)
			{
				sd_card_connect_change = 1;
				timer_connected = 0;
			}
			// Delay connecting card.
			else
			{
				timer_connected = 10;
			}
		}

		if (timer_connected > 0)
		{
			timer_connected--;
			if (timer_connected == 0)
			{
				if (sd_card_connected == SDHOST_CARD_INSERTED)
				{
					sd_card_connect_change = 1;
				}
			}
		}

#ifndef DISABLE_DFU
		// Call the DFU timer to progress state machine.
		// The progression depends on the bmAttributes set in the configuration.
		USBD_DFU_timer();
#endif // DISABLE_DFU
	}
}

static struct pipe *get_pipe(uint8_t pipe)
{
	static struct pipe pipes[3];
	struct pipe *ret_val = NULL;

	switch (pipe) {
		case BOMS_EP_DATA_IN:
			ret_val = &pipes[0];
			break;
		case BOMS_EP_DATA_OUT:
			ret_val = &pipes[1];
			break;
		default:
			break;

	}
	assert(ret_val!=NULL);
	return ret_val;
}

void USBD_pipe_isr(uint16_t pipe_bitfields)
{
	if (pipe_bitfields & BIT(BOMS_EP_DATA_IN))
	{
		usbd_pipe_process(get_pipe(BOMS_EP_DATA_IN));
	}
	if (pipe_bitfields & BIT(BOMS_EP_DATA_OUT))
	{
		usbd_pipe_process(get_pipe(BOMS_EP_DATA_OUT));
	}
}

bool boms_out_on_data_ready(struct pipe *pp)
{
	return true;
}

int32_t get_out_buf(struct pipe *out, uint8_t *bomsdata, size_t len)
{
	int32_t status;

	CRITICAL_SECTION_BEGIN
	struct urb *urb = usbd_get_app_urb(out);

	if (likely(urb_owned_by_app(urb)))
	{
		/* got BOMS_DATA_EP_SIZE to write */
		status = len;

		uint16_t urb_len = urb_get_app_to_process(urb);

		if (likely(urb_len == status))
		{
			/* Can send out all URB buffer, queue URB back to USBD */
			asm_memcpy8(urb->ptr, bomsdata, status);
			usbd_submit_urb(out, urb);
			//tfp_printf("get_out_buf\r\n");
		}
		else
		{
			//tfp_printf("get_out_buf: partial URB available %d\r\n", urb_len);
			status = 0;
		}
	}
	else
	{
		//tfp_printf("get_out_buf: No BOMS data arrived\r\n");
		usbd_set_app_paused(out);
		status = 0;
	}

	CRITICAL_SECTION_END

	return status;
}


int32_t get_in_buf(struct pipe *in, uint8_t *bomsdata, size_t len)
{
	int32_t status = len;
	CRITICAL_SECTION_BEGIN
	/* Force acquire USB IN buffer */
	struct urb *urb = usbd_force_acquire_urb_for_app(in);

	uint16_t free = urb_get_app_to_process(urb);
	/* Don't have enough URB space */
	if (free >= len)
	{
		asm_memcpy8(bomsdata, urb->ptr, len);
		urb->ptr += len;
		usbd_submit_urb(in, urb);
	}
	else
	{
		status = 0;
	}
	CRITICAL_SECTION_END

	return status;
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
	uint8_t request = req->bRequest;
	uint8_t maxLun = 0;

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
				default:
					break;
				}
			}
			else
#endif // DISABLE_DFU
			{
				switch (request)
				{
				case BOMS_REQUEST_GET_MAX_LUN:
	#ifdef BOMS_DEBUG_REQUESTS
					tfp_printf("BOMS_REQUEST_GET_MAX_LUN\r\n");
	#endif // BOMS_DEBUG_REQUESTS

					USBD_transfer_ep0(USBD_DIR_IN, &maxLun,
							sizeof(maxLun), req->wLength);

					// ACK
					USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);

					status = USBD_OK;
					break;
				case BOMS_REQUEST_RESET:
	#ifdef BOMS_DEBUG_REQUESTS
					tfp_printf("BOMS_REQUEST_RESET\r\n");
	#endif // BOMS_DEBUG_REQUESTS

					USBD_set_state(USBD_STATE_DEFAULT);

					// ACK
					USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
					status = USBD_OK;
					break;
				default:
	#ifdef BOMS_DEBUG_REQUESTS
					tfp_printf("Don't supported request %02x\r\n", request);
	#endif // BOMS_DEBUG_REQUESTS
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
					//tfp_printf("USB_CLASS_REQUEST_DNLOAD:%d\r\n",interface);
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
	(void)interface;

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
 @param[in]	req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return		status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bmRequestType is not
 supported.
 **/
int8_t standard_req_get_descriptor(USB_device_request *req, uint8_t **buffer,
		uint16_t *len)
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
			src = (uint8_t *) &device_descriptor_usbstorage;
		}
		else
		{
			src = (uint8_t *) &device_descriptor_dfumode;
		}
#else
		src = (uint8_t *) &device_descriptor_usbstorage;
#endif // DISABLE_DFU

		if (length > sizeof(USB_device_descriptor)) // too many bytes requested
			length = sizeof(USB_device_descriptor); // Entire structure.
		break;

	case USB_DESCRIPTOR_TYPE_CONFIGURATION:
#ifndef DISABLE_DFU
		if (USBD_DFU_is_runtime())
		{
			src = (uint8_t *) &config_descriptor_mass_storage;
			if (length > sizeof(config_descriptor_mass_storage)) // too many bytes requested
				length = sizeof(config_descriptor_mass_storage); // Entire structure.
		}
		else
		{
			src = (uint8_t *) &config_descriptor_dfumode;
			if (length > sizeof(config_descriptor_dfumode)) // too many bytes requested
				length = sizeof(config_descriptor_dfumode); // Entire structure.
		}
#else // DISABLE_DFU
		src = (uint8_t *) &config_descriptor_mass_storage;
		if (length > sizeof(config_descriptor_mass_storage)) // too many bytes requested
			length = sizeof(config_descriptor_mass_storage); // Entire structure.
#endif // DISABLE_DFU

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
					if (length > sizeof(usbd_boms_device_interface_guid)) // too many bytes requested
					{
						length = sizeof(usbd_boms_device_interface_guid); // Entire structure.
					}
					// Return the extended property for the interface requested.
					USBD_transfer_ep0(USBD_DIR_IN, (uint8_t *)&usbd_boms_device_interface_guid,
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

void boms_populate_status(uint8_t status,
		boms_commandBlockWrapper_t *BOMSCmd,
		boms_commandStatusWrapper_t *BOMSStatus)
{
#ifdef BOMS_DEBUG_MSC
	int i;
#endif // BOMS_DEBUG_MSC

#ifdef BOMS_DEBUG_SCSI_COMMANDS
	tfp_printf("status = %d\r\n", status);
#endif // BOMS_DEBUG_SCSI_COMMANDS

	memset((void *)BOMSStatus, 0, sizeof(boms_commandStatusWrapper_t));
	BOMSStatus->dCSWTag = BOMSCmd->dCBWTag;
	BOMSStatus->dCSWSignature = BOMS_USBS;
	BOMSStatus->bCSWStatus = status;
	if (status != BOMS_STATUS_OK)
	{
		BOMSStatus->dCSWDataResidue = BOMSCmd->dCBWDataTransferLength;
	}
#ifdef BOMS_DEBUG_MSC
	tfp_printf("USBS: ");
	for (i = 0; i < sizeof(boms_commandStatusWrapper_t); i++)
		tfp_printf("%02x ", ((unsigned char *)BOMSStatus)[i]);
	tfp_printf("\r\n");
#endif // BOMS_DEBUG_MSC
}

uint8_t boms_send_status(uint8_t status,
		boms_commandBlockWrapper_t *BOMSCmd)
{
	boms_commandStatusWrapper_t BOMSStatus;

	boms_populate_status(status, BOMSCmd, &BOMSStatus);

	size_t length = 0;
	do {

		struct pipe *pp = get_pipe(BOMS_EP_DATA_IN);
		length = get_in_buf(pp, (uint8_t *)&BOMSStatus, sizeof(boms_commandStatusWrapper_t));
	}while (length == 0);

	return status;
}

void scsi_sense_usb_fail(void)
{
	// Report failed due to a USB error to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_ABORTED_COMMAND;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_ABORTED_COMMAND_NO_ADDITIONAL_CODE;
}

void scsi_sense_sdhost_fail(int dir)
{
	// Report failed due to an SD card error to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_MEDIUM_ERROR;
	if (dir == SDHOST_READ)
	{
		scsi_addn_sense = SCSI_SENSE_KEY_ADDN_MEDIUM_ERROR_UNRECOVERED_READ_ERROR;
	}
	else
	{
		scsi_addn_sense = SCSI_SENSE_KEY_ADDN_MEDIUM_ERROR_WRITE_FAULT;
	}
}

void scsi_sense_not_ready(void)
{
	// Report failed due to medium not present to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_NOT_READY;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_NOT_READY_MEDIUM_NOT_PRESENT;
}

void scsi_sense_lba_fail(void)
{
	// Report failed due to an invalid LBA error to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_ILLEGAL_REQUEST;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_LBA_OUT_OF_RANGE;
}

void scsi_sense_lun_fail(void)
{
	// Report failed due to an invalid LUN error to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_ILLEGAL_REQUEST;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_LUN;
}

void scsi_sense_illegal_request_command(void)
{
	// Report that the command is not supported to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_ILLEGAL_REQUEST;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_COMMAND;
}

void scsi_sense_illegal_request_cdb(void)
{
	// Report that the command was incorrectly formatted to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_ILLEGAL_REQUEST;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_FIELD_IN_CDB;
}

void scsi_sense_illegal_param_value(void)
{
	// Report that the command was incorrectly formatted to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_ILLEGAL_REQUEST;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_PARM_VALUE;
}

void scsi_sense_illegal_request_parm(void)
{
	// Report that the command was incorrectly formatted to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_ILLEGAL_REQUEST;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_FIELDS_IN_PARM;
}

void scsi_sense_ok(void)
{
	// Report no error to REQUEST SENSE.
	scsi_sense = SCSI_SENSE_KEY_NO_SENSE;
	scsi_addn_sense = SCSI_SENSE_KEY_ADDN_NO_ERROR;
}

int32_t scsi_inquiry(boms_commandBlockWrapper_t *BOMSCmd)
{
	int32_t status;
	uint8_t boms_status;

	boms_scsi_CRB_inquiry_t inq;

#ifdef BOMS_DEBUG_SCSI_COMMANDS
	tfp_printf("SCSI INQUIRY\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

	memset((void *)&inq, 0, sizeof(boms_scsi_CRB_inquiry_t));

	// byte 0
    inq.peripheral_dev_type = 0;
    inq.peripheral_qual = 0;
    // byte 1
    inq.device_type_modifier = 0;
    inq.rmb = 1;
    // byte 2
    inq.ansi = 2;
    inq.ecma = 0;
    inq.iso = 0;
    // byte 3
    inq.resp_data_format = 2;
    inq.trm_iop = 0;
    inq.aen = 0;
    // byte 4
    inq.adnl_len = 0x1f;
    // byte 7
    inq.sft_re = 0;
    inq.cmd_que = 0;
    inq.linked = 0;
    inq.sync = 0;
    inq.wbus16 = 0;
    inq.wbus32 = 0;
    inq.rel_adr = 0;
    // bytes 8-15
    memcpy(&inq.vid, "FTDI    ", 8);
    // bytes 16-31
    memcpy(&inq.pid, "USB Dev 2 SDCard", 16);
    // bytes 32-35
    memcpy(&inq.rev, "1.0 ", 4);

    {
		struct pipe *pp = get_pipe(BOMS_EP_DATA_IN);
		status = get_in_buf(pp, (uint8_t *)&inq, BOMS_SCSI_INQUIRY_TRANSFER_LEN);
    }


    if (status == BOMS_SCSI_INQUIRY_TRANSFER_LEN)
	{
		boms_status = BOMS_STATUS_OK;
	}
	else
	{
		boms_status = BOMS_STATUS_COMMAND_FAILED;
		scsi_sense_usb_fail();
	}

	return boms_send_status(boms_status, BOMSCmd);
}

int32_t scsi_request_sense(boms_commandBlockWrapper_t *BOMSCmd)
{
	int32_t status;
	uint8_t boms_status;

	boms_scsi_CRB_request_sense_t sense;

#ifdef BOMS_DEBUG_SCSI_COMMANDS
	tfp_printf("SCSI REQUEST SENSE\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

	memset((void *)&sense, 0, sizeof(boms_scsi_CRB_request_sense_t));

	// byte 0
	sense.rsp_code = 0x70;
	sense.valid = 0;
	// byte 1
	sense.segment = 0x00;
	// byte 2
	sense.sense_key = scsi_sense;
	sense.ili = 0;
	sense.eom = 0;
	sense.filemark = 0;
	// bytes 3-6
	sense.information = 0;
	// byte 7
	sense.adnl_len = 0x0a;
	// bytes 8-11
	sense.cmd_info = 0;
	// byte 12
	sense.adnl_sense_code = scsi_addn_sense >> 8;
	// byte 13
	sense.adnl_sense_code_qualifier = scsi_addn_sense & 0xff;
	// byte 14
	sense.fru = 0x00;
	// byte 15
	sense.sksv1 = 0;
	sense.sksv = 0;
	// byte 16
	sense.sksv2 = 0;
	// byte 17
	sense.sksv3 = 0;

	// Reset the sense key.
	scsi_sense_ok();

    {
		struct pipe *pp = get_pipe(BOMS_EP_DATA_IN);
		status = get_in_buf(pp, (uint8_t *) &sense, sizeof(boms_scsi_CRB_request_sense_t));
    }

	if (status == sizeof(boms_scsi_CRB_request_sense_t))
	{
		boms_status = BOMS_STATUS_OK;
	}
	else
	{
		boms_status = BOMS_STATUS_COMMAND_FAILED;
		scsi_sense_usb_fail();
	}

	return boms_send_status(boms_status, BOMSCmd);
}

int32_t scsi_mode_sense(boms_commandBlockWrapper_t *BOMSCmd)
{
	int32_t status;
	uint8_t boms_status;

	// This is how we return the pages in the mode sense response.
	struct  {
		boms_scsi_CRB_mode_sense_t sense;
		boms_scsi_CRB_mode_sense_page_read_write_error_recovery_t recovery;
		boms_scsi_CRB_mode_sense_page_flexible_disk_t flexible;
		boms_scsi_CRB_mode_sense_page_removable_block_access_capabilities_t capab;
		boms_scsi_CRB_mode_sense_page_fault_failure_reporting_control_t reporting;
	} response;

#ifdef BOMS_DEBUG_SCSI_COMMANDS
	tfp_printf("SCSI MODE SENSE\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

	memset((void *)&response, 0, sizeof(response));

	response.sense.mode_data_length = sizeof(response) - 1;
	response.sense.medium_type_code = 0;
	response.sense.device_specific_parameters = 0;
	response.sense.block_descriptor_length = 0;

	response.recovery.page_code = 0x01;
	response.recovery.page_length = 0x0a;
	response.recovery.read_retry_count = 3;
	response.recovery.write_retry_count = 128;

	response.flexible.page_code = 0x05;
	response.flexible.page_length = 0x1e;
	response.flexible.transfer_rate = CPU2SCSI_16(5000);
	response.flexible.sectors_per_track = 16;
	response.flexible.bytes_per_sector = CPU2SCSI_16(16128);
	response.flexible.number_of_cylinders = CPU2SCSI_16(15);
	response.flexible.motor_on_delay = 5;
	response.flexible.motor_off_delay = 30;
	response.flexible.rotation_rate = CPU2SCSI_16(360);

	response.capab.page_code = 0x1b;
	response.capab.page_length = 0x0a;
	response.capab.total_luns = 1;

	response.reporting.page_code = 0x1c;
	response.reporting.page_length = 0x06;
	response.reporting.mrie = 0x05;
	response.reporting.interval_timer = CPU2SCSI_32(28);

    {
		struct pipe *pp = get_pipe(BOMS_EP_DATA_IN);
		status = get_in_buf(pp, (uint8_t *) &response, sizeof(response));
    }

	if (status == sizeof(response))
	{
		boms_status = BOMS_STATUS_OK;
	}
	else
	{
		boms_status = BOMS_STATUS_COMMAND_FAILED;
		scsi_sense_usb_fail();
	}

	return boms_send_status(boms_status, BOMSCmd);
}

int32_t scsi_test_unit_ready(boms_commandBlockWrapper_t *BOMSCmd)
{
	uint8_t boms_status;

#ifdef BOMS_DEBUG_SCSI_COMMANDS
	tfp_printf("SCSI TEST UNIT READY\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

	if (sd_card_initialised)
	{
		boms_status = BOMS_STATUS_OK;
	}
	else
	{
		boms_status = BOMS_STATUS_COMMAND_FAILED;
		scsi_sense_not_ready();
	}

	return boms_send_status(boms_status, BOMSCmd);
}

int32_t scsi_prevent_allow_removal(boms_commandBlockWrapper_t *BOMSCmd)
{
#ifdef BOMS_DEBUG_SCSI_COMMANDS
	tfp_printf("SCSI REVENT ALLOW REMOVAL\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

	if (BOMSCmd->CBWCB.removal.prevent)
	{
		scsi_sense_illegal_request_cdb();
		// Not supported.
		return boms_send_status(BOMS_STATUS_COMMAND_FAILED, BOMSCmd);
	}

	return boms_send_status(BOMS_STATUS_OK, BOMSCmd);
}

int32_t scsi_read_capacity(boms_commandBlockWrapper_t *BOMSCmd)
{
	size_t status;
	uint8_t boms_status = BOMS_STATUS_OK;
	boms_scsi_CRB_read_capacity_16_t cap_16;
	boms_scsi_CRB_read_capacity_t cap_10;
	size_t len;

	uint8_t *cap;

	if (!sd_card_initialised)
	{
		boms_status = BOMS_STATUS_COMMAND_FAILED;
		scsi_sense_not_ready();
		USBD_stall_endpoint(BOMS_EP_DATA_IN);
	}

	if (boms_status == BOMS_STATUS_OK)
	{
		if (BOMSCmd->CBWCB.read_capacity.op_code == 0x9e)
		{
#ifdef BOMS_DEBUG_SCSI_COMMANDS
			tfp_printf("SCSI READ CAPACITY (16)\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

			cap = (uint8_t *)&cap_16;
			len = sizeof(cap_16);
			memset(cap, 0, sizeof(cap_16));

			cap_16.block_len_l = CPU2SCSI_32(0x200);
			cap_16.lba_l = CPU2SCSI_32(sdhost_get_capacity());
		}
		else
		{
#ifdef BOMS_DEBUG_SCSI_COMMANDS
			tfp_printf("SCSI READ CAPACITY (10)\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

			cap = (uint8_t *)&cap_10;
			len = sizeof(cap_10);

			cap_10.block_len = CPU2SCSI_32(0x200);
			cap_10.lba = CPU2SCSI_32(sdhost_get_capacity());
		}

	    {
			struct pipe *pp = get_pipe(BOMS_EP_DATA_IN);
			status = get_in_buf(pp, cap, len);
	    }

		if (status == len)
		{
			boms_status = BOMS_STATUS_OK;
		}
		else
		{
			boms_status = BOMS_STATUS_COMMAND_FAILED;
			scsi_sense_usb_fail();
		}
	}

	return boms_send_status(boms_status, BOMSCmd);
}

int32_t scsi_read_format_capacity(boms_commandBlockWrapper_t *BOMSCmd)
{
	size_t status;
	uint8_t boms_status = BOMS_STATUS_OK;
	boms_scsi_CRB_read_format_capacity_t fcap;
	size_t len;
	uint32_t blocks;
	uint32_t block_size;
	uint32_t erase_block_count;

	if (!sd_card_initialised)
	{
		boms_status = BOMS_STATUS_COMMAND_FAILED;
		scsi_sense_not_ready();
		USBD_stall_endpoint(BOMS_EP_DATA_IN);
	}

	if (boms_status == BOMS_STATUS_OK)
	{
#ifdef BOMS_DEBUG_SCSI_COMMANDS
		tfp_printf("SCSI READ FORMAT CAPACITY (10)\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

		memset(&fcap, 0, sizeof(fcap));

		// Maximum length of return data.
		len = SCSI2CPU_16(BOMSCmd->CBWCB.read_fcapacity.alloc_length);
		if (len > sizeof(boms_scsi_CRB_read_format_capacity_t))
		{
			len = sizeof(boms_scsi_CRB_read_format_capacity_t);
		}

		// Capacity is highest addressable sector.
		blocks = sdhost_get_capacity() + 1;
		block_size = sdhost_get_block_size();
		erase_block_count = sdhost_get_erase_block_count();

		if ((block_size) && (erase_block_count))
		{
			blocks = blocks / erase_block_count;

			fcap.cap_list = CPU2SCSI_32(8);
			fcap.blocks = CPU2SCSI_32(blocks);

#if BYTE_ORDER == LITTLE_ENDIAN
			// Shift to compensate for 24 bits from 32 being used.
			block_size = (erase_block_count * block_size) << 8;
#else
			block_size = (erase_block_count * block_size);
#endif

			fcap.block_len = CPU2SCSI_32(block_size);
			fcap.code = 2;

		    {
				struct pipe *pp = get_pipe(BOMS_EP_DATA_IN);
				status = get_in_buf(pp, (uint8_t *)&fcap, len);
		    }

			if (status == len)
			{
				boms_status = BOMS_STATUS_OK;
			}
			else
			{
				boms_status = BOMS_STATUS_COMMAND_FAILED;
				scsi_sense_usb_fail();
			}
		}
		else
		{
			boms_status = BOMS_STATUS_COMMAND_FAILED;
			scsi_sense_not_ready();
			USBD_stall_endpoint(BOMS_EP_DATA_IN);
		}
	}

	return boms_send_status(boms_status, BOMSCmd);
}

int32_t scsi_read(boms_commandBlockWrapper_t *BOMSCmd)
{
	uint8_t bufBOMSData[BOMS_DATA_EP_SIZE]  __attribute__ ((aligned (4))) ;
	int32_t status = BOMS_DATA_EP_SIZE;
	uint8_t boms_status = BOMS_STATUS_OK;
	uint32_t sector;
	int count;
	SDHOST_STATUS sdret = SDHOST_OK;

#ifdef BOMS_DEBUG_SCSI_COMMANDS
	tfp_printf("SCSI READ (10)\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

	if (!sd_card_initialised)
	{
		boms_status = BOMS_STATUS_COMMAND_FAILED;
		scsi_sense_not_ready();
		USBD_stall_endpoint(BOMS_EP_DATA_IN);
	}

	if (boms_status == BOMS_STATUS_OK)
	{
		sector = SCSI2CPU_32(BOMSCmd->CBWCB.read.lba);
		count = SCSI2CPU_16(BOMSCmd->CBWCB.read.length);

		if (count == 0)
		{
			boms_status = BOMS_STATUS_COMMAND_FAILED;
			scsi_sense_illegal_param_value();
			USBD_stall_endpoint(BOMS_EP_DATA_IN);
		}
		// Check range of LBA - capacity is highest addressable sector.
		else if ((count + sector) > (sdhost_get_capacity() + 1))
		{
			boms_status = BOMS_STATUS_COMMAND_FAILED;
			scsi_sense_lba_fail();
			USBD_stall_endpoint(BOMS_EP_DATA_IN);
		}
		// Check supported LUN
		else if (BOMSCmd->CBWCB.read.lun != 0)
		{
			boms_status = BOMS_STATUS_COMMAND_FAILED;
			scsi_sense_lun_fail();
			USBD_stall_endpoint(BOMS_EP_DATA_IN);
		}
		else
		{
#ifdef BOMS_DEBUG_SCSI_READ_WRITE_COMMANDS
			tfp_printf("Read %d blocks from sector %ld\r\n", count, sector);
#endif // BOMS_DEBUG_SCSI_READ_COMMANDS

			while (count)
			{
				if (sdret == SDHOST_OK)
				{
					sdret = sdhost_transfer_data(SDHOST_READ, (void*)bufBOMSData, BOMS_DATA_EP_SIZE, sector++);
				}

				{
					struct pipe *pp = get_pipe(BOMS_EP_DATA_IN);

					CRITICAL_SECTION_BEGIN
					/* Force acquire USB IN buffer */
					struct urb *urb = usbd_force_acquire_urb_for_app(pp);

					uint16_t free = urb_get_app_to_process(urb);
					/* Don't have enough URB space */
					if (free == status)
					{
						asm_memcpy32(bufBOMSData, urb->ptr, status);
						urb->ptr += status;
						usbd_submit_urb(pp, urb);
						if (count)
						{
							count--;
						}
					}
					else
					{
						status = 0; //No bytes could be copied
					}
					CRITICAL_SECTION_END

				}

			}

			if (status != BOMS_DATA_EP_SIZE)
			{
				boms_status = BOMS_STATUS_COMMAND_FAILED;
				scsi_sense_usb_fail();
			}
			else if (sdret != SDHOST_OK)
			{
				boms_status = BOMS_STATUS_COMMAND_FAILED;
				scsi_sense_sdhost_fail(SDHOST_READ);
			}
			else
			{
				boms_status = BOMS_STATUS_OK;
			}
		}

#ifdef BOMS_DEBUG_ERRORS
		if (boms_status != BOMS_STATUS_OK)
		{
			tfp_printf("sd status %d usbd status %ld\r\n", sdret, status);
		}
#endif // BOMS_DEBUG_ERRORS
	}

	return boms_send_status(boms_status, BOMSCmd);
}

int32_t scsi_write(boms_commandBlockWrapper_t *BOMSCmd)
{
	uint8_t bufBOMSData[BOMS_DATA_EP_SIZE] __attribute__ ((aligned (4))) ;
	int32_t status = BOMS_DATA_EP_SIZE;
	uint8_t boms_status = BOMS_STATUS_OK;
	uint32_t sector;
	int count;
	SDHOST_STATUS sdret = SDHOST_OK;

#ifdef BOMS_DEBUG_SCSI_COMMANDS
	tfp_printf("SCSI WRITE (10)\r\n");
#endif // BOMS_DEBUG_SCSI_COMMANDS

	if (!sd_card_initialised)
	{
		boms_status = BOMS_STATUS_COMMAND_FAILED;
		scsi_sense_not_ready();
		USBD_stall_endpoint(BOMS_EP_DATA_OUT);
	}

	if (boms_status == BOMS_STATUS_OK)
	{
		sector = SCSI2CPU_32(BOMSCmd->CBWCB.write.lba);
		count = SCSI2CPU_16(BOMSCmd->CBWCB.write.length);

		if (count == 0)
		{
			boms_status = BOMS_STATUS_COMMAND_FAILED;
			scsi_sense_illegal_param_value();
			USBD_stall_endpoint(BOMS_EP_DATA_IN);
		}
		// Check range of LBA - capacity is highest addressable sector.
		else if ((count + sector) > (sdhost_get_capacity() + 1))
		{
			boms_status = BOMS_STATUS_COMMAND_FAILED;
			scsi_sense_lba_fail();
			USBD_stall_endpoint(BOMS_EP_DATA_OUT);
		}
		// Check supported LUN
		else if (BOMSCmd->CBWCB.write.lun != 0)
		{
			boms_status = BOMS_STATUS_COMMAND_FAILED;
			scsi_sense_lun_fail();
			USBD_stall_endpoint(BOMS_EP_DATA_OUT);
		}
		else
		{
#ifdef BOMS_DEBUG_SCSI_READ_WRITE_COMMANDS
			tfp_printf("Write %d blocks to sector %ld\r\n", count, sector);
#endif // BOMS_DEBUG_SCSI_READ_COMMANDS

			while (count)
			{
				{
					struct pipe *pp = get_pipe(BOMS_EP_DATA_OUT);

					CRITICAL_SECTION_BEGIN
					struct urb *urb = usbd_get_app_urb(pp);

					if (likely(urb_owned_by_app(urb)))
					{
						/* got BOMS_DATA_EP_SIZE to write */
						status = BOMS_DATA_EP_SIZE;

						uint16_t urb_len = urb_get_app_to_process(urb);

						if (likely(urb_len == status))
						{
							/* Can send out all URB buffer, queue URB back to USBD */
							asm_memcpy32(urb->ptr, bufBOMSData, status);
							usbd_submit_urb(pp, urb);
							if (count)
							{
								count--;
							}
						}
						else
						{
							status = 0;
						}
					}
					else
					{
						usbd_set_app_paused(pp);
						status = 0;
					}

					CRITICAL_SECTION_END
				}

				if (sdret == SDHOST_OK)
				{
					if (status == BOMS_DATA_EP_SIZE)
					{
						sdret = sdhost_transfer_data(SDHOST_WRITE, (void*)bufBOMSData, BOMS_DATA_EP_SIZE, sector++);
					}
				}
			}

			if (status != BOMS_DATA_EP_SIZE)
			{
				boms_status = BOMS_STATUS_COMMAND_FAILED;
				scsi_sense_usb_fail();
			}
			else if (sdret != SDHOST_OK)
			{
				boms_status = BOMS_STATUS_COMMAND_FAILED;
				scsi_sense_sdhost_fail(SDHOST_WRITE);
			}
			else
			{
				boms_status = BOMS_STATUS_OK;
			}
		}

#ifdef BOMS_DEBUG_ERRORS
		if (boms_status != BOMS_STATUS_OK)
		{
			tfp_printf("sd status %d usbd status %ld\r\n", sdret, status);
		}
#endif // BOMS_DEBUG_ERRORS
	}

	return boms_send_status(boms_status, BOMSCmd);
}

void boms_sdcard_bridge(void)
{
#define BOMS_URB_IN_COUNT	4
#define BOMS_URB_OUT_COUNT	4

	uint32_t status = USBD_OK;
	boms_commandBlockWrapper_t BOMSCmd;
	int32_t read_bytes;
	SDHOST_STATUS sdret;

	USBD_create_endpoint(BOMS_EP_DATA_OUT, USBD_EP_BULK, USBD_DIR_OUT,
	BOMS_DATA_USBD_EP_SIZE, USBD_DB_ON, NULL /*ep_cb*/);
	USBD_create_endpoint(BOMS_EP_DATA_IN, USBD_EP_BULK, USBD_DIR_IN,
	BOMS_DATA_USBD_EP_SIZE, USBD_DB_ON, NULL /*ep_cb*/);

	static uint8_t boms_in_buf[BOMS_DATA_EP_SIZE * BOMS_URB_IN_COUNT] __attribute__ ((aligned (16)));
	static uint8_t boms_out_buf[BOMS_DATA_EP_SIZE * BOMS_URB_OUT_COUNT];

	static struct urb boms_in[BOMS_URB_IN_COUNT];
	static struct urb boms_out[BOMS_URB_OUT_COUNT];
	struct pipe *pp;

	/* BOMS */

	pp = get_pipe(BOMS_EP_DATA_IN);
	usbd_pipe_init(pp, BOMS_EP_DATA_IN, BOMS_EP_DATA_IN | 0x80,
			boms_in, boms_in_buf, BOMS_URB_IN_COUNT);

	pp = get_pipe(BOMS_EP_DATA_OUT);
	usbd_pipe_init(pp, BOMS_EP_DATA_OUT, BOMS_EP_DATA_OUT,
			boms_out, boms_out_buf, BOMS_URB_OUT_COUNT);

	register_on_usbd_ready(pp, boms_out_on_data_ready);

	memset((void *)&BOMSCmd, 0, sizeof(BOMSCmd));

	// Report that the medium is not present yet to REQUEST SENSE.
	scsi_sense_not_ready();

	do
	{
		// If data is available on the USB OUT endpoint and there is space
		// in the ring buffer to receive it then read it in from the host.

		struct pipe *pp = get_pipe(BOMS_EP_DATA_OUT);
		struct urb *urb = usbd_get_app_urb(pp);
		if (likely(urb_owned_by_app(urb)) && (urb_get_app_to_process(urb) == sizeof(BOMSCmd)))
		{

			read_bytes = get_out_buf(pp, (uint8_t *)&BOMSCmd, sizeof(BOMSCmd));

#ifdef BOMS_DEBUG_MSC
			// Write it out to the UART.
			tfp_printf("USBC: ");
			for (int i = 0; i < read_bytes; i++)
				tfp_printf("%02x ", ((unsigned char *)&BOMSCmd)[i]);
			tfp_printf("\r\n");
#endif // BOMS_DEBUG_MSC

			if (read_bytes == sizeof(BOMSCmd))
			{
				if (BOMSCmd.dCBWSignature == BOMS_USBC)
				{
					if (BOMSCmd.CBWCB.unit_ready.op_code == BOMS_SCSI_TEST_UNIT_READY) // SCSI Test Unit Ready
					{
						scsi_test_unit_ready(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.request_sense.op_code == BOMS_SCSI_READ) // SCSI Read
					{
						scsi_read(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.request_sense.op_code == BOMS_SCSI_WRITE) // SCSI Write
					{
						scsi_write(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.inquiry.op_code == BOMS_SCSI_INQUIRY) // SCSI Inquiry
					{
						scsi_inquiry(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.request_sense.op_code == BOMS_SCSI_REQUEST_SENSE) // SCSI Request Sense
					{
						scsi_request_sense(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.read_capacity.op_code == BOMS_SCSI_READ_CAPACITY) // SCSI Read Capacity (10)
					{
						scsi_read_capacity(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.read_capacity.op_code == BOMS_SCSI_READ_FORMAT_CAPACITY) // SCSI Read Format Capacity (10)
					{
						scsi_read_format_capacity(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.mode_sense.op_code == BOMS_SCSI_MODE_SENSE) // SCSI Mode Sense
					{
						scsi_mode_sense(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.request_sense.op_code == BOMS_SCSI_READ_CAPACITY_16) // SCSI Read Capacity (16)
					{
						scsi_read_capacity(&BOMSCmd);
					}
					else if (BOMSCmd.CBWCB.request_sense.op_code == BOMS_SCSI_PREVENT_ALLOW_REMOVAL) // SCSI Prevent Allow Removal
					{
						scsi_prevent_allow_removal(&BOMSCmd);
					}
					else
					{
						// Unknown command.
#ifdef BOMS_DEBUG_ERRORS
						tfp_printf("Unknown SCSI command 0x%02x\r\n", BOMSCmd.CBWCB.request_sense.op_code);
#endif // BOMS_DEBUG_ERRORS
						// Send IN a zero length packet.
						if (BOMSCmd.dCBWDataTransferLength)
						{
							USBD_stall_endpoint(BOMS_EP_DATA_IN);
							//USBD_transfer(BOMS_EP_DATA_IN, (uint8_t *)&BOMSCmd, 0);
						}
						boms_send_status(BOMS_STATUS_COMMAND_FAILED, &BOMSCmd);
						scsi_sense_illegal_request_command();
					}
				}
				else
				{
#ifdef BOMS_DEBUG_ERRORS
					tfp_printf("Unknown BOMS signature 0x%08x\r\n", BOMSCmd.dCBWSignature);
#endif // BOMS_DEBUG_ERRORS
					// Was not a USBC signature.
					boms_send_status(BOMS_STATUS_PHASE_ERROR, &BOMSCmd);
					scsi_sense_illegal_request_cdb();
				}
			}
			else
			{
#ifdef BOMS_DEBUG_ERRORS
				tfp_printf("Incorrect BOMS command length of %ld\r\n", read_bytes);
#endif // BOMS_DEBUG_ERRORS
				// Wrong command size.
				boms_send_status(BOMS_STATUS_COMMAND_FAILED, &BOMSCmd);
				scsi_sense_illegal_request_parm();
			}
		}
		else if (!urb_owned_by_app(urb))
		{
			usbd_set_app_paused(pp);
		}

		sdret = SDHOST_OK;
		CRITICAL_SECTION_BEGIN
		// If the SD card is removed update the status.
		if (sd_card_connect_change)
		{
			tfp_printf("Change...");
			// Changed to inserted from removed.
			if (sd_card_connected == SDHOST_CARD_INSERTED)
			{
				sdret = SDHOST_CARD_INSERTED;
				tfp_printf("connect card %d", sdret);
			}
			else
			{
				sdret = SDHOST_CARD_REMOVED;
				tfp_printf("no card");
			}
			tfp_printf("\r\n");

			sd_card_connect_change = 0;
		}

		// Send a not ready (medium removed) status to a
		// sense request.
		if (sd_card_connected != SDHOST_CARD_INSERTED)
		{
			scsi_sense_not_ready();
		}
		CRITICAL_SECTION_END

		// Transition from disconnected to connected.
		// Initialise the card.
		if (sdret == SDHOST_CARD_INSERTED)
		{
			sdret = sdhost_card_init();
			tfp_printf("Card initialise %d\r\n", sdret);
			if (sdret == SDHOST_OK)
			{
				sd_card_initialised = 1;
			}
		}
		// Transition from connected to disconnected.
		// Initialise the SD host controller.
		else if (sdret == SDHOST_CARD_REMOVED)
		{
			sdhost_init();
			sd_card_initialised = 0;
			tfp_printf("Host initialise\r\n");
		}

		status = (USBD_get_state() < USBD_STATE_DEFAULT)?USBD_ERR_NOT_CONFIGURED:USBD_OK;

	} while (status == USBD_OK);

	// Disconnected from host or SD card.
}

uint8_t usbd_mass_storage(void)
{
	int8_t status;
	USBD_ctx usb_ctx;

	memset(&usb_ctx, 0, sizeof(usb_ctx));

	usb_ctx.standard_req_cb = NULL;
	// usb_ctx.standard_req_cb = standard_req_cb;
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

	// Now instantiate the USB device.
	USBD_initialise(&usb_ctx);

	for (;;)
	{
		if (!USBD_is_connected())
		{
			continue;
		}
		/* Get USB speed */
		USBD_attach();
		/* Connect to Host */
		if (USBD_connect() == USBD_OK)
		{
			if (USBD_DFU_is_runtime())
			{
				// Start the BOMS to SD Card bridge.
				boms_sdcard_bridge();
			}
			else
			{
				// In DFU mode. Process USB requests.
				do{
					status = (USBD_get_state() < USBD_STATE_DEFAULT)?USBD_ERR_NOT_CONFIGURED:USBD_OK;
				}while (status == USBD_OK);
			}
			tfp_printf("Restarting\r\n");
		}
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
	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
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
	uart_open(UART0, 1,
	UART_DIVIDER_19200_BAUD, uart_data_bits_8, uart_parity_none,
			uart_stop_bits_1);


	/* Print out a welcome message... */
	uart_puts(UART0, "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
	);

	/* Enable tfp_printf() functionality... */
	init_printf(UART0, tfp_putc);

	sys_enable(sys_device_timer_wdt);

	/* Timer A = 1ms */
	timer_prescaler(timer_select_a, 1000);
	timer_init(timer_select_a, 100, timer_direction_down,
			timer_prescaler_select_on, timer_mode_continuous);
	timer_enable_interrupt(timer_select_a);
	timer_start(timer_select_a);

	interrupt_attach(interrupt_timers, (int8_t) interrupt_timers, ISR_timer);

	interrupt_enable_globally();

    /* Start up the SD Card */
    sys_enable(sys_device_sd_card);

    sdhost_init(); /* Need to do this before assigning IO to allow the peripheral
                      to start up properly */
    /* All SD Host pins except CLK need a pull-up to work. The MM900EV*A module does not have external pull-up, so enable internal one */
    gpio_function(GPIO_SD_CLK, pad_sd_clk); gpio_pull(GPIO_SD_CLK, pad_pull_none);
    gpio_function(GPIO_SD_CMD, pad_sd_cmd); gpio_pull(GPIO_SD_CMD, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT3, pad_sd_data3); gpio_pull(GPIO_SD_DAT3, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT2, pad_sd_data2); gpio_pull(GPIO_SD_DAT2, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT1, pad_sd_data1); gpio_pull(GPIO_SD_DAT1, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT0, pad_sd_data0); gpio_pull(GPIO_SD_DAT0, pad_pull_pullup);
    gpio_function(GPIO_SD_CD, pad_sd_cd); gpio_pull(GPIO_SD_CD, pad_pull_pullup);
    gpio_function(GPIO_SD_WP, pad_sd_wp); gpio_pull(GPIO_SD_WP, pad_pull_pullup);
    /* Start up the SD Card */
    sys_enable(sys_device_sd_card);
    sdhost_init();

	interrupt_attach(interrupt_0, (int8_t)interrupt_0, powermanagement_ISR);
	interrupt_enable_globally();

    tfp_printf("(C) Copyright, Bridgetek Pte Ltd \r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");
	tfp_printf("Welcome to USB Mass Storage to SD Card Test Application ... \r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");

	usbd_mass_storage();

	interrupt_detach(interrupt_timers);
	interrupt_disable_globally();

	sys_disable(sys_device_timer_wdt);

	// Wait forever...
	for (;;)
		;

	return 0;
}
