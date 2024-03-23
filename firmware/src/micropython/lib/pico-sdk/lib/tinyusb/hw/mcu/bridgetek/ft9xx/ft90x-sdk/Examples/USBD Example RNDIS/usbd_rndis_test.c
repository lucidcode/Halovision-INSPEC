/**
  @file usbd_rndis_test_1.c
  @brief
  USB RNDIS Device Test

  This example program will bridge data from the UART to a Microsoft Remote
  NDIS (RNDIS) device.
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
#include <ft900_usb_rndis.h>
#include <ft900_usbd_rndis.h>
#include <ft900_usbd_dfu.h>

/* For MikroC const qualifier will place variables in Flash
 * not just make them constant.
 */
#if defined(__GNUC__)
#define DESCRIPTOR_QUALIFIER const
#elif defined(__MIKROC_PRO_FOR_FT90x__)
#define DESCRIPTOR_QUALIFIER data
#endif

#define RNDIS_DEBUG
#ifdef RNDIS_DEBUG
/* UART support for printf output. */
#include <ft900_uart_simple.h>
#include "tinyprintf.h"
#define RNDIS_STATE_DEBUG
#define RNDIS_LINK_DEBUG
#define RNDIS_ERROR_DEBUG
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
 @name USB_PID_RNDIS configuration.
 @brief Run Time Product ID for RNDIS function.
 */
//@{
#define USB_PID_RNDIS 0x0fd3
//@}

/**
 @name DFU_USB_INTERFACE_RNDIS configuration.
 @brief Interface number and count for RNDIS function.
 */
//@{
#define DFU_USB_INTERFACE_RNDIS 0
#define DFU_USB_INTERFACE_COUNT_RNDIS 2
//@}

/**
 @name DFU_USB_PID_DFUMODE configuration.
 @brief FTDI predefined DFU Mode Product ID.
 */
//@{
#define DFU_USB_PID_DFUMODE 0x0fde
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
#define WCID_VENDOR_REQUEST_CODE	 0xF1
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
 @brief Endpoint definitions for RNDIS.
 */
//@{
#define RNDIS_EP_NOTIFICATION 			USBD_EP_1
#define RNDIS_EP_DATA_IN 				USBD_EP_2
#define RNDIS_EP_DATA_OUT 				USBD_EP_3
#define RNDIS_NOTIFICATION_EP_SIZE 		0x08
#define RNDIS_NOTIFICATION_USBD_EP_SIZE USBD_EP_SIZE_8
#define RNDIS_DATA_EP_SIZE				0x200
#define RNDIS_DATA_USBD_EP_SIZE 		USBD_EP_SIZE_512
//@}

/**
 @brief Address of EEPROM on I2C bus.
 */
#define NET_EEPROM_ADDR 0xA0

/**
 @brief Offset in EEPROM for MAC address.
*/
#define NET_EEPROM_OFFSET_MACADDRESS 0xfa

/** @name Ethernet packet size definitions.
 * @details The MTU (maximum transmission unit) is the largest data
 * 		packet that can be transmitted. The size does not include the
 * 		ethernet header so this must be added onto the MTU to calculate
 * 		the largest frame that can be received and transmitted.
 * 		The definitions here include the buffersize for receiving and
 * 		transmitting packets. This includes 2 bytes for the data size
 * 		required by the ethernet library to maintain 32-bit alignment.
 */
//@{
/** @brief MTU size for Ethernet V2.
 *  @details Allows most ethernet implementations to work with IP V4 and
 *  IP V6 packets. This is the data payload within an ethernet frame.
 */
#define RNDIS_MTU_SIZE			1500
/** @brief Largest size for a frame in Ethernet V2.
 *  @details The size includes the ethernet header, the data payload (MTU)
 *  and a 4 byte checksum. The checksum is not passed from or returned by
 *  the ethernet library unless this is specifically enabled.
 */
#define RNDIS_MAX_FRAME_SIZE	(ETHERNET_WRITE_HEADER + RNDIS_MTU_SIZE + 4)
/** @brief Largest buffer size required for a frame.
 *  @details The size includes the ethernet frame and 2 bytes used by the
 *  ethernet library to maintian 32 bit alignment.
 */
#define RNDIS_PACKET_BUFFERSIZE (2 + RNDIS_MAX_FRAME_SIZE)
//@}

/** @name Ethernet packet buffers.
 * @details
 *
 *		Only one packet may be sent at a time.
 */
//@{
/** @brief The template for recieving incoming ethernet packets.
 * @details The structure used to store packets coming and going from the device.
 * Note that structure members are specifically aligned on 32 bit boundaries.
 * This may be the case by default but it is spelled out here.
 * The "len" member
 */
struct rndis_buffer
{
    uint16_t len;
    uint8_t packet[RNDIS_PACKET_BUFFERSIZE];
}  __attribute__ ((aligned (4))) __attribute__ ((packed));

/** @brief The number of bytes in the receive buffer for incoming ethernet packets.
 * @details This is similar to a circular buffer but with an overflow area.
 * It is not possible to break an incoming frame up to fit into a traditional
 * circular buffer so this scheme has an area at the end of the buffer (overflow
 * area) where the last packet is stored.
 * The largest expected data to be received is the size of struct rndis_buffer.
 * Therefore once a packet writes into the overflow area - defined as the size
 * of the largest possible packet to be received the next packet will be stored
 * at the start of the buffer again - assuming there is room.
 * Note that in ethernet there is no need to maintain the order of packets but
 * this scheme does maintain the order well.
 * If there is no space left to receive data then the ethernet receive data buffer
 * is flushed and the data lost.
 * Make the receive buffer a 2^n. It is a global due to the size of the buffer.
 */
#define RNDIS_RX_BUFFER_SIZE (32 * 1024)
static uint8_t rndis_rxbuff[RNDIS_RX_BUFFER_SIZE];

/** @brief Packet receive buffer write index.
 */
static struct rndis_buffer *rndis_rxbuff_wr = (struct rndis_buffer *)rndis_rxbuff;
/** @brief Packet receive buffer read index.
 */
static struct rndis_buffer *rndis_rxbuff_rd = (struct rndis_buffer *)0;
/** @brief Packet transmit buffer.
 */
static struct rndis_buffer rndis_txbuff = {0, {0}};
//@}

/** @name Interrupt flags
 * @details Flags signalled by the interrupt handler for the ethernet.
 * These are set by the top-half and used in the main code (bottom-half).
 * They must only be read or altered in the main code when within a
 * critical section.
 */
//@{
/** @brief Previous ethernet transmit resulted in an error.
 */
static volatile uint8_t rndis_txerr = 0;
/** @brief Ethernet transmit in progress.
 */
static volatile uint8_t rndis_txactive = 0;
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/
static const struct PACK USB_WCID_extended_property_descriptor usbd_rndis_device_interface_guid =
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
	/* Property Value -> {86326531-F36A-4E18-BCBD-4F5962940205} */
	{0x7b, 0x00, 0x38, 0x00, 0x36, 0x00, 0x33, 0x00, 0x32, 0x00, 0x36, 0x00, 0x35, 0x00,
			0x33, 0x00, 0x31, 0x00, 0x2d, 0x00, 0x46, 0x00, 0x33, 0x00, 0x36, 0x00, 0x41, 0x00,
			0x2d, 0x00, 0x34, 0x00, 0x45, 0x00, 0x31, 0x00, 0x38, 0x00, 0x2d, 0x00, 0x42, 0x00,
			0x43, 0x00, 0x42, 0x00, 0x44, 0x00, 0x2d, 0x00, 0x34, 0x00, 0x46, 0x00, 0x35, 0x00,
			0x39, 0x00, 0x36, 0x00, 0x32, 0x00, 0x39, 0x00, 0x34, 0x00, 0x30, 0x00, 0x32, 0x00,
			0x30, 0x00, 0x35, 0x00, 0x7d, 0x00,
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

		// String 2 (Product): "FT900 RNDIS"
		0x18, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '9', 0x00, '0',
		0x00, '0', 0x00, ' ', 0x00, 'R', 0x00, 'N', 0x00, 'D', 0x00, 'I', 0x00,
		'S', 0x00,

		// String 3 (Serial Number): "FT424242"
		0x12, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '4', 0x00, '2',
		0x00, '4', 0x00, '2', 0x00, '4', 0x00, '2', 0x00,

		// String 4 (DFU Product Name): "FT900 DFU Mode"
		0x1E, USB_DESCRIPTOR_TYPE_STRING, 'F', 0x00, 'T', 0x00, '9', 0x00, '0',
		0x00, '0', 0x00, ' ', 0x00, 'D', 0x00, 'F', 0x00, 'U', 0x00, ' ', 0x00,
		'M', 0x00, 'o', 0x00, 'd', 0x00, 'e', 0x00,

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
 @name device_descriptor_rndis
 @brief Device descriptor for Run Time mode.
 */
DESCRIPTOR_QUALIFIER USB_device_descriptor device_descriptor_rndis =
{
		0x12, /* bLength */
		USB_DESCRIPTOR_TYPE_DEVICE, /* bDescriptorType */
		USB_BCD_VERSION_2_0, /* bcdUSB */          // V2.0
		USB_CLASS_DEVICE, /* bDeviceClass */       // Defined in interface
		USB_SUBCLASS_DEVICE, /* bDeviceSubClass */ // Defined in interface
		USB_PROTOCOL_DEVICE, /* bDeviceProtocol */ // Defined in interface
		//USB_CLASS_CDC_CONTROL, /* bDeviceClass */  // Communication Device Class
		//0x00, /* bDeviceSubClass */ // Unused
		//0x00, /* bDeviceProtocol */ // Unused
		USB_CONTROL_EP_MAX_PACKET_SIZE, /* bMaxPacketSize0 */
		USB_VID_FTDI, /* idVendor */   // idVendor: 0x0403 (FTDI)
		USB_PID_RNDIS, /* idProduct */ // idProduct: 0x0fe2
		0x0101, /* bcdDevice */        // 1.1
		0x01, /* iManufacturer */      // Manufacturer
		0x02, /* iProduct */           // Product
		0x03, /* iSerialNumber */      // Serial Number
		0x01, /* bNumConfigurations */
};

/**
 @name config_descriptor_rndis
 @brief Configuration descriptor for Run Time mode.
 */
//@{
struct PACK config_descriptor_rndis
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
	USB_endpoint_descriptor endpoint_data_in;
	USB_endpoint_descriptor endpoint_data_out;
	USB_interface_descriptor dfu_interface;
	USB_dfu_functional_descriptor dfu_functional;
};

// Fields marked with * are updated with information in device_config[]
DESCRIPTOR_QUALIFIER struct config_descriptor_rndis config_descriptor_rndis =
{
	{
		sizeof(USB_configuration_descriptor), /* configuration.bLength */
		USB_DESCRIPTOR_TYPE_CONFIGURATION, /* configuration.bDescriptorType */
		sizeof(struct config_descriptor_rndis), /* configuration.wTotalLength */
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
		DFU_USB_INTERFACE_RNDIS, /* interface_association.bFirstInterface */
		DFU_USB_INTERFACE_COUNT_RNDIS, /* interface_association.bInterfaceCount */
		USB_CLASS_CDC_CONTROL, /* interface_association.bFunctionClass */
		USB_SUBCLASS_CDC_CONTROL_ABSTRACT, /* interface_association.bFunctionSubClass */
		USB_PROTOCOL_CDC_CONTROL_VENDOR, /* interface_association.bFunctionProtocol */
		2 /* interface_association.iFunction */  // "FT900 RNDIS"
	},

	// ---- INTERFACE DESCRIPTOR for RNDIS Control ----
	{
		sizeof(USB_interface_descriptor), /* interface_control.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* interface_control.bDescriptorType */
		DFU_USB_INTERFACE_RNDIS, /* interface_control.bInterfaceNumber */
		0x00, /* interface_control.bAlternateSetting */
		0x01, /* interface_control.bNumEndpoints */
		USB_CLASS_CDC_CONTROL, /* interface_control.bInterfaceClass */ // CDC Class
		USB_SUBCLASS_CDC_CONTROL_ABSTRACT, /* interface_control.bInterfaceSubClass */ // Abstract Control Model
		USB_PROTOCOL_CDC_CONTROL_VENDOR, /* interface_control.bInterfaceProtocol */ // No built-in protocol
		0x00 /* interface_control.iInterface */
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
		0 /* cdcACMFD.bmCapabilities */
	},

	// ---- ENDPOINT DESCRIPTOR for RNDIS Control ----
	{
		sizeof(USB_endpoint_descriptor), /* endpoint_control.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_control.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_IN | RNDIS_EP_NOTIFICATION, /* endpoint_control.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT, /* endpoint_control.bmAttributes */
		RNDIS_NOTIFICATION_EP_SIZE, /* endpoint_control.wMaxPacketSize */
		0x08 /* endpoint_control.bInterval */
	},

	// ---- INTERFACE DESCRIPTOR for RNDIS Data ----
	{
		sizeof(USB_interface_descriptor), /* interface_data.bLength */
		USB_DESCRIPTOR_TYPE_INTERFACE, /* interface_data.bDescriptorType */
		(DFU_USB_INTERFACE_RNDIS + 1), /* interface_data.bInterfaceNumber */
		0x00, /* interface_data.bAlternateSetting */
		0x02, /* interface_data.bNumEndpoints */
		USB_CLASS_CDC_DATA, /* interface_data.bInterfaceClass */ // CDC Data Class
		USB_SUBCLASS_CDC_DATA, /* interface_data.bInterfaceSubClass */ // Abstract Control Model
		USB_PROTOCOL_CDC_DATA, /* interface_data.bInterfaceProtocol */ // No built-in protocol
		0x00 /* interface_data.iInterface */ // Unused
	},

	{
		sizeof(USB_endpoint_descriptor), /* endpoint_data_in.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_data_in.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_IN | RNDIS_EP_DATA_IN, /* endpoint_data_in.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_BULK, /* endpoint_data_in.bmAttributes */
		RNDIS_DATA_EP_SIZE, /* endpoint_data_in.wMaxPacketSize */
		0x00 /* endpoint_data_in.bInterval */
	},

	// ---- ENDPOINT DESCRIPTORS for RNDIS Data ----
	{
		sizeof(USB_endpoint_descriptor), /* endpoint_data_out.bLength */
		USB_DESCRIPTOR_TYPE_ENDPOINT, /* endpoint_data_out.bDescriptorType */
		USB_ENDPOINT_DESCRIPTOR_EPADDR_OUT | RNDIS_EP_DATA_OUT, /* endpoint_data_out.bEndpointAddress */
		USB_ENDPOINT_DESCRIPTOR_ATTR_BULK, /* endpoint_data_out.bmAttributes */
		RNDIS_DATA_EP_SIZE, /* endpoint_data_out.wMaxPacketSize */
		0x00 /* endpoint_data_out.bInterval */
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
		0x05 /* dfu_interface.iInterface */ // * iInterface: "DFU Interface"
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
 @brief WCID Compatible ID for RNDIS and DFU interface in runtime.
 */
//@{
DESCRIPTOR_QUALIFIER struct {
		USB_WCID_feature_descriptor first;
		USB_WCID_feature_descriptor_additional second;
} wcid_feature_runtime = {
	sizeof(struct USB_WCID_feature_descriptor) +
		sizeof(struct USB_WCID_feature_descriptor_additional), /* dwLength */
	USB_MICROSOFT_WCID_VERSION, /* bcdVersion */
	USB_MICROSOFT_WCID_FEATURE_WINDEX_COMPAT_ID, /* wIndex */
	2, /* bCount */
	(uint8_t []) {0, 0, 0, 0, 0, 0, 0,}, /* rsv1 */
	DFU_USB_INTERFACE_RNDIS, /* bFirstInterfaceNumber */
	1, /* rsv2 - set to 1 */
	{'R', 'N', 'D', 'I', 'S', 0x00, 0x00, 0x00,}, /* compatibleID[8] */
	{'5', '1', '6', '2', '0', '0', '1', 0x00,}, /* subCompatibleID[8] */
	{0, 0, 0, 0, 0, 0,}, /* rsv3[6] */
	DFU_USB_INTERFACE_RUNTIME, /* bFirstInterfaceNumber */
	1, /* rsv2 - set to 1 */
	(uint8_t []) {'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,}, /* compatibleID[8] */
	(uint8_t []) {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}, /* subCompatibleID[8] */
	(uint8_t []) {0, 0, 0, 0, 0, 0,} /* rsv3[6] */
};

/**
 @name wcid_feature_rndis
 @brief WCID Compatible ID for RNDIS interface.
 */
//@{
DESCRIPTOR_QUALIFIER USB_WCID_feature_descriptor wcid_feature_rndis =
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
		DFU_USB_PID_DFUMODE, /* idProduct */ // idProduct: 0x0fee
		0x0101, /* bcdDevice */        // 1.1
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
 @name Ethernet device configuration
 @brief MAC address and vendor information for ethernet device. Used by
 	 both the ethernet library and the RNDIS library.
 	 MAC address by default is a locally administered address (bit 1 of
 	 second nibble set to 1).
 */
//@{
static uint8_t my_mac[6] = {0xa0, 0x00, 0x00, 0xc0, 0xff, 0xee};
static const uint8_t my_vendor_id[3] = {0xaa, 0x00, 0x00};
static const char *my_vendor_description = "FTDI FT900 USB to Ethernet Bridge";
//@}

#define ETHERNET_LINK_CHECK_TIMER 500 // 500ms.
static uint16_t eth_link_check_timer;

#define ETHERNET_STAT_TIMER 5000 // 5000ms.
static uint16_t eth_stat_timer;

uint8_t eth_link = 0;

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static void eth_setup(void);
static void eth_rx_enable_cb(uint8_t rx_enable, uint32_t rx_filter);

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
		// Call USB timer handler.
		USBD_timer();

		// Call the DFU timer to progress state machine.
		// The progression depends on the bmAttributes set in the configuration.
		USBD_DFU_timer();

		// Call the RNDIS timer handler.
		USBD_RNDIS_timer();

		// Countdown to checking for link status on the ethernet.
		if (eth_link_check_timer)
		{
			eth_link_check_timer--;
		}

		if (eth_stat_timer)
		{
			eth_stat_timer--;
		}
	}
}

/* Power management ISR */
void ISR_powermanagement(void)
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

/** @name ISR_ethernet
 *  @details Top-half interrupt handler for ethernet interrupts.
 *  Clear interrupt sources and signal to main code when they occur.
 */
void ISR_ethernet(void)
{
    uint8_t isr;
    //uint8_t i;

    isr = ETH->ETH_INT_STATUS;

    if (isr & MASK_ETH_IACK_RX_ERR)
    {
    	USBD_RNDIS_stat(USBD_RNDIS_STAT_RX_ERROR);
    	ETH->ETH_INT_STATUS = MASK_ETH_IACK_RX_ERR;
    }

    if (isr & MASK_ETH_IACK_FIFO_OV)
    {
    	USBD_RNDIS_stat(USBD_RNDIS_STAT_RX_DROPPED);
    	ETH->ETH_INT_STATUS = MASK_ETH_IACK_FIFO_OV;
    }

    if (isr & MASK_ETH_IACK_TX_ERR)
    {
    	USBD_RNDIS_stat(USBD_RNDIS_STAT_TX_ERROR);
    	ETH->ETH_INT_STATUS = MASK_ETH_IACK_TX_ERR;
    	rndis_txerr = 1;
    	rndis_txactive = 0;
    }

    if (isr & MASK_ETH_IACK_TX_EMPTY)
    {
    	USBD_RNDIS_stat(USBD_RNDIS_STAT_TX_PACKETS);
    	ETH->ETH_INT_STATUS = MASK_ETH_IACK_TX_EMPTY;
    	rndis_txerr = 0;
    	rndis_txactive = 0;
    }

    if (isr & MASK_ETH_IACK_RX_INT)
    {
		uint32_t rlen;

		/* Pointer to the write pointer in buffer. */
		register uint32_t *dst;
		/* Rx FIFO read. */
		register volatile uint32_t *data_reg = &ETH->ETH_DATA;
		/* Variable to receive first longword of packet which
		 * contains the packet length. This is a register.
		 */
		register uint32_t w0;
		/* Number of packets waiting in hardware Rx FIFO. */
		uint32_t c0;

		/* Collect any packets in Rx FIFO into RAM. */

		/* This utilises labels and gotos to improve speed. */
		loopstart:

		/* How many packets have been received and are
		 * waiting in the Rx FIFO.
		 */
		c0 = ETH->ETH_NUM_PKT;

		/* No packets waiting. */
		if (c0 == 0) goto loopstop;

		/* Maximum length of data which can be put into
		 * the receive buffer before the end of buffer. */
		rlen = RNDIS_RX_BUFFER_SIZE - ((uint32_t)rndis_rxbuff_wr - (uint32_t)rndis_rxbuff);

		/* We require enough room to store a maximum sized packet
		 * in the receive buffer. This is calculated as 1500 bytes
		 * of payload, an ethernet header, the size of the packet
		 * length.
		 */
		if (rlen < RNDIS_PACKET_BUFFERSIZE)
		{
			/* No room left for a full packet. Try and start writing at the start. */
			rndis_rxbuff_wr = (struct rndis_buffer *)rndis_rxbuff;

			/* See if we will overwrite unread packets. */
			rlen = ((uint32_t)rndis_rxbuff_rd - (uint32_t)rndis_rxbuff);

			/* Full buffer. */
			if (rlen < RNDIS_PACKET_BUFFERSIZE + sizeof(uint32_t))
			{
				/* Turn off receive interrupts. It will be turned on again
				 * when the packets are processed.
				 */
				ETH->ETH_INT_ENABLE = MASK_ETH_IMR_RX_ERR_MASK |
						MASK_ETH_IACK_FIFO_OV |
						MASK_ETH_IACK_TX_EMPTY |
						MASK_ETH_IACK_TX_ERR;
				goto loopstop;
			}
		}

		/* Read first dword of packet. This contains the length
		 * of the first packet. */
		w0 = *data_reg;

		/* Move pointer to next dword in destination buffer. */
		dst = (uint32_t *)rndis_rxbuff_wr;
		*dst = w0;
		dst++;

		/* Calculate length of packet (excluding first dword). */
		w0 &= 0x07ff;
		w0 -= 1;
		w0 &= (~3);

		/* Get the remaining packet data in using the streamin.l opcode. */
		__asm__("streamin.l %0,%1,%2" : :"r"(dst), "r"(data_reg), "r"(w0));

		/* Copy first dword to destination buffer len. */
		rndis_rxbuff_wr->len = (uint16_t)w0;

		/* Update destination pointer to point to the end
		 * of the loaded packet. The typecasts are used to
		 * reduce the outputted code size by avoiding a shift. */
		rndis_rxbuff_wr = (struct rndis_buffer *)((uint32_t)rndis_rxbuff_wr + w0);

		/* Read any more packets available. */
		goto loopstart;

		loopstop:

		/* Need something for compiler to label here. */
		(void)rlen;
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
	uint16_t length = req->wLength;
	uint8_t request = req->bRequest;

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
					USBD_DFU_class_req_getstatus(length);
					status = USBD_OK;
					break;
				case USB_CLASS_REQUEST_GETSTATE:
					USBD_DFU_class_req_getstate(length);
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
					status = USBD_RNDIS_encap_command(length);
					break;
				case USB_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE:
					status = USBD_RNDIS_encap_response(length);
					break;
				}
			}
		}
		// ...or the correct DFU Mode interface
#ifndef DISABLE_DFU		
		else
		{
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
							length);
					status = USBD_OK;
					break;

				case USB_CLASS_REQUEST_UPLOAD:
					/* Block number passed in wValue gives the start address of
					 * to program based on the size of the transfer size.
					 */
					USBD_DFU_class_req_upload(req->wValue * DFU_TRANSFER_SIZE,
							length);
					status = USBD_OK;
					break;

				case USB_CLASS_REQUEST_GETSTATUS:
					USBD_DFU_class_req_getstatus(length);
					status = USBD_OK;
					break;

				case USB_CLASS_REQUEST_GETSTATE:
					USBD_DFU_class_req_getstate(length);
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
		}
#endif	// DISABLE_DFU 	
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
 @param[in]	req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return		status - USBD_OK if successful or USBD_ERR_*
 if there is an error or the bmRequestType is not
 supported.
 **/
int8_t standard_req_get_descriptor_cb(USB_device_request *req, uint8_t **buffer, uint16_t *len)
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
			src = (uint8_t *) &device_descriptor_rndis;
		}
		else
		{
			src = (uint8_t *) &device_descriptor_dfumode;
		}
#else
		src = (uint8_t *) &device_descriptor_rndis;
#endif // DISABLE_DFU

		if (length > sizeof(USB_device_descriptor)) // too many bytes requested
				length = sizeof(USB_device_descriptor); // Entire structure.
		break;

	case USB_DESCRIPTOR_TYPE_CONFIGURATION:
#ifndef DISABLE_DFU
		if (USBD_DFU_is_runtime())
		{
			src = (uint8_t *) &config_descriptor_rndis;
			if (length > sizeof(config_descriptor_rndis)) // too many bytes requested
				length = sizeof(config_descriptor_rndis); // Entire structure.
		}
		else
		{
			src = (uint8_t *) &config_descriptor_dfumode;
			if (length > sizeof(config_descriptor_dfumode)) // too many bytes requested
				length = sizeof(config_descriptor_dfumode); // Entire structure.
		}
#else // DISABLE_DFU
		src = (uint8_t *) &config_descriptor_rndis;
		if (length > sizeof(config_descriptor_rndis)) // too many bytes requested
			length = sizeof(config_descriptor_rndis); // Entire structure.
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
	uint16_t length = req->wLength;

#ifndef DISABLE_DFU
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
					if (length > sizeof(usbd_rndis_device_interface_guid)) // too many bytes requested
					{
						length = sizeof(usbd_rndis_device_interface_guid); // Entire structure.
					}
					// Return the extended property for the interface requested.
					USBD_transfer_ep0(USBD_DIR_IN, (uint8_t *)&usbd_rndis_device_interface_guid,
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

	tfp_printf("Restarting\r\n");
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


static void rndis_eth_bridge(void)
{
	int8_t status;
	size_t rx_bytes, tx_bytes;
	size_t rx_buf_remain;
	uint8_t tx_err, tx_active;
	struct rndis_buffer *buffer_rd;
	struct rndis_buffer *buffer_wr;


#ifdef RNDIS_STATE_DEBUG
	tfp_printf("Enumerated\r\n");
#endif // RNDIS_STATE_DEBUG

	// Create endpoints for
	USBD_create_endpoint(RNDIS_EP_NOTIFICATION, USBD_EP_INT, USBD_DIR_IN,
			RNDIS_NOTIFICATION_USBD_EP_SIZE, USBD_DB_OFF, NULL /*ep_cb*/);
	USBD_create_endpoint(RNDIS_EP_DATA_OUT, USBD_EP_BULK, USBD_DIR_OUT,
			RNDIS_DATA_USBD_EP_SIZE, USBD_DB_ON, NULL /*ep_cb*/);
	USBD_create_endpoint(RNDIS_EP_DATA_IN, USBD_EP_BULK, USBD_DIR_IN,
			RNDIS_DATA_USBD_EP_SIZE, USBD_DB_ON, NULL /*ep_cb*/);

    USBD_RNDIS_initialise(my_mac, my_vendor_id, my_vendor_description,
    		RNDIS_MTU_SIZE, RNDIS_MAX_FRAME_SIZE,
    		RNDIS_EP_NOTIFICATION, RNDIS_EP_DATA_OUT, RNDIS_EP_DATA_IN,
			eth_rx_enable_cb);

#ifdef RNDIS_LINK_DEBUG
    if (!ethernet_is_link_up())
    {
    	tfp_printf("Please plug in your Ethernet cable\r\n");
    }
#endif // RNDIS_LINK_DEBUG

    /* Reset volatiles used in ISRs. */
    CRITICAL_SECTION_BEGIN
	rndis_txactive = 0;
	rndis_txerr = 0;
	rndis_rxbuff_wr = rndis_rxbuff_rd = (struct rndis_buffer *)rndis_rxbuff;
	CRITICAL_SECTION_END

    ethernet_tx_enable(1);

	do
	{
		/* Receive data from the ethernet and buffer it in system RAM.
    	 * The goal is to clear the RX RAM in the ethernet as soon
		 * as possible. */
		CRITICAL_SECTION_BEGIN
		buffer_rd = rndis_rxbuff_rd;
		buffer_wr = rndis_rxbuff_wr;
		CRITICAL_SECTION_END

		/* Send any data waiting in RAM to the host using RNDIS protocol.
		 * The data has been buffered in RAM (allowing the ethernet to
		 * continue receiving data in it's RX RAM) so start at the read
		 * pointer and send the packets up to the host. */

		if ((uint32_t)buffer_wr != (uint32_t)buffer_rd)
		{
			/* If the current read pointer is in the overflow area of
			 * the buffer then start again at the beginning of the
			 * buffer. */
			rx_buf_remain = RNDIS_RX_BUFFER_SIZE - ((uint32_t)buffer_rd - (uint32_t)rndis_rxbuff);

			/* We require enough room to store a maximum sized packet
			 * in the receive buffer. This is calculated as 1500 bytes
			 * of payload, an ethernet header, the size of the packet
			 * length.
			 */
			if (rx_buf_remain < RNDIS_PACKET_BUFFERSIZE)
			{
				buffer_rd = (struct rndis_buffer *)rndis_rxbuff;
			}

			/* If there has been data received on the ethernet and the
			 * USB IN endpoint is empty then write data from the ethernet
			 * to the host via RNDIS. */
			rx_bytes = buffer_rd->len;

			if ((rx_bytes == 0) || (rx_bytes > RNDIS_PACKET_BUFFERSIZE))
			{
				/* An error has occurred.
				 * This is an incorrect packet length. */
				USBD_RNDIS_stat(USBD_RNDIS_STAT_RX_ERROR);
			    CRITICAL_SECTION_BEGIN
				rndis_rxbuff_wr = rndis_rxbuff_rd = (struct rndis_buffer *)rndis_rxbuff;
				CRITICAL_SECTION_END
			}
			else
			{
				/* If the endpoint can send data to the host then continue.
				 * Otherwise we keep the data in RAM until the next pass. */
				if (!USBD_ep_buffer_full(RNDIS_EP_DATA_IN))
				{
					/* Send via RNDIS protocol and update stats. */
					USBD_RNDIS_write((uint8_t *)&buffer_rd->packet, rx_bytes);
					USBD_RNDIS_stat(USBD_RNDIS_STAT_RX_PACKETS);

					/* Move the read pointer on by the length of the data
					 * and then to the next 32 bit boundary. */
					buffer_rd = (struct rndis_buffer *)
							((uint32_t)buffer_rd + ((rx_bytes + 3) & (~3)));

					CRITICAL_SECTION_BEGIN
					rndis_rxbuff_rd = buffer_rd;
					/* Not a full buffer. */
				    ETH->ETH_INT_ENABLE = MASK_ETH_IMR_RX_ERR_MASK |
							MASK_ETH_IACK_FIFO_OV |
							MASK_ETH_IACK_TX_EMPTY |
							MASK_ETH_IACK_TX_ERR |
							MASK_ETH_IACK_RX_INT;
					CRITICAL_SECTION_END
				}
			}
		}

    	/* Transmit RNDIS packets from USB to ethernet. */

    	/* Copy the transmit active state from the ISR. */
		CRITICAL_SECTION_BEGIN
    	tx_active = rndis_txactive;
		CRITICAL_SECTION_END

		/* This section is the only section that will start a transmission
    	 * and set rndis_txactive. The ISR top half will clear this once a
    	 * transmission is complete allowing us to send more data. */
		if (tx_active == 0)
    	{
			/* If data is available on the USB OUT endpoint then read it in
			 * via RNDIS and send it to the ethernet. */
    		status = USBD_RNDIS_OK;

    		/* Copy the error state of the last transmission. */
    		CRITICAL_SECTION_BEGIN
	    	tx_err = rndis_txerr;
			CRITICAL_SECTION_END

    		/* Retransmit data if it was not successfully transmitted the
			 * last time. Failures happen frequently due to collisions so
			 * cope gracefully with them.
			 * If the transmission was successful then get new data from
			 * the host. */
			if (tx_err == 0)
    		{
    			status = USBD_RNDIS_read((uint8_t *)&rndis_txbuff.packet, &tx_bytes);
    		}

			/* If no data to transmit then return value is USBD_RNDIS_NO_DATA. */
			if (status == USBD_RNDIS_OK)
			{
				/* We have new data to transmit.
				 * Fill out the first 2 bytes of the transmit buffer with
				 * the data payload size as required by ethernet_write
				 * function. */
				rndis_txbuff.len = (tx_bytes - ETHERNET_WRITE_HEADER);

				/* Indicate an active transmission. This will prevent us from
				 * trying to read more data from the host or send more data
				 * until the ISR top half clears this. */
				CRITICAL_SECTION_BEGIN
	    		rndis_txactive = 1;
	    		rndis_txerr = 0;
				CRITICAL_SECTION_END

				/* Transmit the data packet on the ethernet - add 2 bytes to
				 * the size of the buffer for the packet length bytes at the
				 * start of the transmit buffer. */
				status = ethernet_write((uint8_t *)&rndis_txbuff, tx_bytes + 2);
				if (status < 0)
				{
					/* A negative return value indicates that the transmitter
					 * is disabled. */
					USBD_RNDIS_stat(USBD_RNDIS_STAT_TX_DROPPED);
					/* Drop the packet - do not try to resend. */
					CRITICAL_SECTION_BEGIN
		    		rndis_txactive = 0;
		    		rndis_txerr = 0;
					CRITICAL_SECTION_END
					/* If the transmitter flags a TX error later then
					 * the appropriate stat will be incremented in the
					 * ISR top half. */
				}
			}
			else if (status == USBD_RNDIS_ERROR)
			{
#ifdef RNDIS_ERROR_DEBUG
				tfp_printf("RNDIS ERROR\r\n");
#endif // RNDIS_ERROR_DEBUG
				USBD_RNDIS_stat(USBD_RNDIS_STAT_TX_DROPPED);
			}
    	}

		/* Process RNDIS commands on the control interface. */
		USBD_RNDIS_process();

		/* Check the link status of the ethernet on a timer.
		 * We do not want to do this often as the check requires
		 * using the MII bus that takes time and needs to wait
		 * for quiescence on the PHY interface. */
		if (eth_link_check_timer == 0)
		{
			/* Only check for connections when we are initialised. */
			if (USBD_RNDIS_state() != RNDIS_STATE_UNINITIALIZED)
			{
				/* Get the link state from the PHY. */
				if (ethernet_is_link_up())
				{
					/* If the link is up and it wasn't up the last time
					 * we checked then this is a connection event. */
					if (eth_link == 0)
					{
						/* Get the current link speed. */
						eth_link = ethernet_link_speed();
#ifdef RNDIS_LINK_DEBUG
						tfp_printf("Media connected %d Mb/sec\r\n", eth_link);
#endif // RNDIS_LINK_DEBUG
						/* Tell the RNDIS driver what speed we are connected at. */
						USBD_RNDIS_link(eth_link);

						CRITICAL_SECTION_BEGIN
			    		rndis_txactive = 0;
			    		rndis_txerr = 0;
						CRITICAL_SECTION_END

						/* Start the ethernet device. */
						ethernet_rx_enable(1);
						ethernet_tx_enable(1);
					}
					/* If we are still up (i.e. the link was up the last time
					 * we checked), then make sure the speed of the link is
					 * the same. */
					else
					{
						/* Check for a change in the link speed. */
						if (eth_link != ethernet_link_speed())
						{
							/* Get the new link speed. */
							eth_link = ethernet_link_speed();
#ifdef RNDIS_LINK_DEBUG
							tfp_printf("Media speed change to %d Mb/sec\r\n", eth_link);
#endif // RNDIS_LINK_DEBUG
							/* Tell the RNDIS driver what speed we are connected at now. */
							USBD_RNDIS_link(eth_link);
						}
					}
				}
				else
				{
					/* If the link was up the last time we checked then this
					 * is a disconnection event. */
					if (eth_link != 0)
					{
#ifdef RNDIS_LINK_DEBUG
						tfp_printf("Media disconnected\r\n");
#endif // RNDIS_LINK_DEBUG
						USBD_RNDIS_link(0);
						ethernet_rx_enable(0);
						ethernet_tx_enable(0);
						eth_link = 0;
					}
				}
			}

			/* Heartbeat. */
#ifdef RNDIS_STATE_DEBUG
			tfp_printf(".");
#endif // RNDIS_STATE_DEBUG
			/* Restart timer. */
			eth_link_check_timer = ETHERNET_LINK_CHECK_TIMER;
		}

		if (eth_stat_timer == 0)
		{
#ifdef RNDIS_LINK_DEBUG
			tfp_printf("Rx %ld Err %ld Dr %ld Tx %ld Err %ld Dr %ld\r\n",
					USBD_RNDIS_get_stat(USBD_RNDIS_STAT_RX_PACKETS),
					USBD_RNDIS_get_stat(USBD_RNDIS_STAT_RX_ERROR),
					USBD_RNDIS_get_stat(USBD_RNDIS_STAT_RX_DROPPED),
					USBD_RNDIS_get_stat(USBD_RNDIS_STAT_TX_PACKETS),
					USBD_RNDIS_get_stat(USBD_RNDIS_STAT_TX_ERROR),
					USBD_RNDIS_get_stat(USBD_RNDIS_STAT_TX_DROPPED));
#endif // RNDIS_LINK_DEBUG
			eth_stat_timer = ETHERNET_STAT_TIMER;
		}

		/* Update USBH status and perform callbacks if required. */
		status = (USBD_get_state() < USBD_STATE_DEFAULT)?USBD_ERR_NOT_CONFIGURED:USBD_OK;

	}while (status == USBD_OK);

	/* Disconnected from host. */
}

/* Callback from the RNDIS library to enable the receiver on the ethernet
 * and change the packet filter.
 * The packet filter definitions are defined as part of NDIS.
 * These are routed through to the ethernet library.
 */
static void eth_rx_enable_cb(uint8_t rx_enable, uint32_t rx_filter)
{
	if (rx_enable)
	{
		ethernet_rx_enable(1);
		// Set the interface to receive promiscuiously if directed.
		ethernet_set_promiscuous(rx_filter & NDIS_PACKET_TYPE_PROMISCUOUS);
		ethernet_accept_multicast(rx_filter & (NDIS_PACKET_TYPE_MULTICAST | NDIS_PACKET_TYPE_ALL_MULTICAST));
#ifdef RNDIS_LINK_DEBUG
        tfp_printf("Packet filter ON P %c M %c\r\n",
        		rx_filter & NDIS_PACKET_TYPE_PROMISCUOUS?'Y':'N',
        				rx_filter & (NDIS_PACKET_TYPE_MULTICAST | NDIS_PACKET_TYPE_ALL_MULTICAST)?'Y':'N');
#endif // RNDIS_LINK_DEBUG
	}
	else
	{
		ethernet_rx_enable(0);
#ifdef RNDIS_LINK_DEBUG
        tfp_printf("Packet filter OFF\r\n");
#endif // RNDIS_LINK_DEBUG
	}
}

static void eth_setup(void)
{
    sys_enable(sys_device_ethernet);

	if (i2cm_read(NET_EEPROM_ADDR, NET_EEPROM_OFFSET_MACADDRESS, my_mac, 6) != 0)
	{
		tfp_printf("Default ");
	}

	tfp_printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\r\n",
    	    my_mac[0],
		    my_mac[1],
			my_mac[2],
			my_mac[3],
			my_mac[4],
			my_mac[5]);

    ethernet_init(my_mac);

    /* Setup LED 0 to be the TX LED */
    gpio_function(4, pad_enet_led0);
    //ethernet_led_mode(0, ethernet_led_mode_tx);
    ethernet_led_mode(0, ethernet_led_mode_link);
    /* Setup LED 1 to be the RX LED */
    gpio_function(5, pad_enet_led1);
    //ethernet_led_mode(1, ethernet_led_mode_rx);
    ethernet_led_mode(1, ethernet_led_mode_spd);

	ethernet_rx_enable(0);
	ethernet_tx_enable(0);

	/* Attach an interrupt to received packets */
    interrupt_attach(interrupt_ethernet, (uint8_t)interrupt_ethernet, ISR_ethernet);
    ethernet_enable_interrupt(MASK_ETH_IMR_RX_ERR_MASK |
    		MASK_ETH_IACK_FIFO_OV |
			MASK_ETH_IACK_TX_ERR |
			MASK_ETH_IACK_TX_EMPTY |
			MASK_ETH_IACK_RX_INT);
}

uint8_t usbd_testing(void)
{
	int8_t status;
	USBD_ctx usb_ctx;

	memset(&usb_ctx, 0, sizeof(usb_ctx));

	usb_ctx.standard_req_cb = NULL;
	usb_ctx.get_descriptor_cb = standard_req_get_descriptor_cb;
	usb_ctx.class_req_cb = class_req_cb;
	usb_ctx.vendor_req_cb = vendor_req_cb;
	usb_ctx.suspend_cb = suspend_cb;
	usb_ctx.resume_cb = resume_cb;
	usb_ctx.reset_cb = reset_cb;
	usb_ctx.lpm_cb = NULL;
	usb_ctx.speed = USBD_SPEED_HIGH;

	/* Initialise the USB device with a control endpoint size
	 * of 8 to 64 bytes. This must match the size for bMaxPacketSize0
	 * defined in the device descriptor. */
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
		if (USBD_connect() == USBD_OK)
		{
			if (USBD_DFU_is_runtime())
			{
				// Start the emulation code.
				rndis_eth_bridge();
			}
			else
			{
				// In DFU mode. Process USB requests.
				do{
					status = (USBD_get_state() < USBD_STATE_DEFAULT)?USBD_ERR_NOT_CONFIGURED:USBD_OK;
				}while (status == USBD_OK);
			}
			USBD_detach();
			tfp_printf("Restarting\r\n");
		}
	}

	return 0;
}

/* FUNCTIONS ***********************************************************************/

int main(void)
{
	sys_reset_all();
    sys_disable(sys_device_ethernet);
    sys_disable(sys_device_usb_device);
    sys_disable(sys_device_i2c_master);

    /* Enable the UART Device... */
	sys_enable(sys_device_uart0);
	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */

	// Open the UART using the coding required.
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

	/* Timer A = 1ms */
	timer_prescaler(timer_select_a, 1000);
	timer_init(timer_select_a, 100, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
	timer_enable_interrupt(timer_select_a);
	timer_start(timer_select_a);

	tfp_printf("(C) Copyright, Bridgetek Pte Ltd \r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");
	tfp_printf("Welcome to USBD RNDIS Tester Example 1... \r\n");
	tfp_printf("\r\n");
	tfp_printf("Emulate a RNDIS device connected to the USB Device Port.\r\n");
	tfp_printf("--------------------------------------------------------------------- \r\n");

	interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, ISR_timer);
	/* Enable power management interrupts. Primarily to detect resume signalling
	 * from the USB host. */
	interrupt_attach(interrupt_0, (int8_t)interrupt_0, ISR_powermanagement);

    /* Set up I2C */
    sys_enable(sys_device_i2c_master);

    /* Setup I2C channel 1 pins for EEPROM */
    gpio_function(44, pad_i2c0_scl); /* I2C0_SCL */
    gpio_function(45, pad_i2c0_sda); /* I2C0_SDA */
    gpio_function(46, pad_i2c1_scl); /* I2C1_SCL */
    gpio_function(47, pad_i2c1_sda); /* I2C1_SDA */
	i2cm_init(I2CM_NORMAL_SPEED, 100000);
    /* Use sys_i2c_swop(1) to activate. */
	sys_i2c_swop(1);

	interrupt_enable_globally();

	eth_setup();
	usbd_testing();

	interrupt_detach(interrupt_timers);
	interrupt_disable_globally();

	sys_disable(sys_device_timer_wdt);

	// Wait forever...
	for (;;);

	return 0;
}


