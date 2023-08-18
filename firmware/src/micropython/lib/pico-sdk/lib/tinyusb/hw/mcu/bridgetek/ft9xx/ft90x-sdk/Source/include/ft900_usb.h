/**
    @file ft900_usb.h

    @brief 
    USB definitions.
  
	@details
    This contains USB definitions of constants and structures which are used in
    all USB implementations.

    Definitions for USB Devices based on USB Specification Revision 2.0 from
    http://www.usb.org/developers/docs/

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

#ifndef FT900_USB_H_
#define FT900_USB_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

#include <stdint.h>

/* CONSTANTS ***********************************************************************/

/** @name USB Standard Specification
 */
//@{

/**
    @name USB Classes
    @brief Definitions of USB Class Codes.
        http://www.usb.org/developers/defined_class
 */
//@{
/// Use class information in the Interface Descriptors
#define USB_CLASS_DEVICE                        0x00
/// Audio
#define USB_CLASS_AUDIO                         0x01
/// Communications and CDC Control
#define USB_CLASS_CDC_CONTROL                   0x02
/// Human Interface Device
#define USB_CLASS_HID                           0x03
/// Physical
#define USB_CLASS_PHYSICAL                      0x05
/// Image
#define USB_CLASS_IMAGE                         0x06
/// Printer
#define USB_CLASS_PRINTER                       0x07
/// Mass Storage
#define USB_CLASS_MASS_STORAGE                  0x08
/// Hub
#define USB_CLASS_HUB                           0x09
/// CDC-Data
#define USB_CLASS_CDC_DATA                      0x0a
/// Smart Card
#define USB_CLASS_SMART_CARD                    0x0b
/// Content Security
#define USB_CLASS_CONTENT_SECURITY              0x0d
/// Video
#define USB_CLASS_VIDEO                         0x0e
/// Personal Healthcare
#define USB_CLASS_PERSONAL_HEALTHCARE           0x0f
/// Audio/Video Devices
#define USB_CLASS_AUDIO_VIDEO                   0x10
/// Diagnostic Device
#define USB_CLASS_DIAGNOSTIC_DEVICE             0xdc
/// Wireless Controller
#define USB_CLASS_WIRELESS_CONTROLLER           0xe0
/// Miscellaneous
#define USB_CLASS_MISCELLANEOUS                 0xef
/// Application Specific
#define USB_CLASS_APPLICATION                   0xfe
/// Vendor Specific
#define USB_CLASS_VENDOR                        0xff
//@}

/**
    @name USB Device Class
    @brief Definitions of USB SubClass and Protocols for Device Class.
        http://www.usb.org/developers/defined_class
 */
//@{
#define USB_SUBCLASS_DEVICE                     0x00
#define USB_PROTOCOL_DEVICE                     0x00
//@}

/**
    @name USB Interface Association Subclasses and Protocols
    @brief Definitions of USB SubClass and Protocols for Device Class.
        http://www.usb.org/developers/defined_class
 */
//@{
#define USB_SUBCLASS_COMMON_CLASS               0x02
#define USB_PROTOCOL_INTERFACE_ASSOCIATION      0x01
//@}

/**
    @name USB Subclass and Protocols
    @brief Definitions of USB SubClass and Protocols various USB device Classes
        http://www.usb.org/developers/defined_class
 */
//@{
/// USB_CLASS_AUDIO
#define USB_SUBCLASS_AUDIO_UNDEFINED                 0x00
#define USB_SUBCLASS_AUDIO_AUDIOCONTROL              0x01
#define USB_SUBCLASS_AUDIO_AUDIOSTREAMING            0x02
#define USB_SUBCLASS_AUDIO_MIDISTREAMING             0x03
#define USB_PROTOCOL_AUDIO_UNDEFINED                 0x00
#define USB_PROTOCOL_AUDIO_VERSION_02_00             0x20

/// USB_CLASS_CDC_CONTROL
#define USB_SUBCLASS_CDC_CONTROL_DIRECT_LINE         0x01
#define USB_SUBCLASS_CDC_CONTROL_ABSTRACT            0x02
#define USB_SUBCLASS_CDC_CONTROL_TELEPHONE           0x03
#define USB_SUBCLASS_CDC_CONTROL_MULTI_CHANNEL       0x04
#define USB_SUBCLASS_CDC_CONTROL_CAPI                0x05
#define USB_SUBCLASS_CDC_CONTROL_ETHERNET_NETWORKING 0x06
#define USB_SUBCLASS_CDC_CONTROL_ATM_NETWORKING      0x07
#define USB_SUBCLASS_CDC_CONTROL_WIRELESS_HANDSET    0x08
#define USB_SUBCLASS_CDC_CONTROL_DEVICE_MANAGEMENT   0x09
#define USB_SUBCLASS_CDC_CONTROL_MOBILE_DIRECT_LINE  0x0a
#define USB_SUBCLASS_CDC_CONTROL_OBEX                0x0b
#define USB_SUBCLASS_CDC_CONTROL_ETHERNET_EMULATION  0x0c
#define USB_SUBCLASS_CDC_CONTROL_NCM				 0x0d
#define USB_PROTOCOL_CDC_CONTROL_NONE                0x00
#define USB_PROTOCOL_CDC_CONTROL_ITU_T_V250          0x01
#define USB_PROTOCOL_CDC_CONTROL_PCCA_101            0x02
#define USB_PROTOCOL_CDC_CONTROL_PCCA_101_ANNEX_O    0x03
#define USB_PROTOCOL_CDC_CONTROL_GSM_707             0x04
#define USB_PROTOCOL_CDC_CONTROL_3GPP_2707           0x05
#define USB_PROTOCOL_CDC_CONTROL_TIA_CS00170         0x06
#define USB_PROTOCOL_CDC_CONTROL_USBEEM              0x07
#define USB_PROTOCOL_CDC_CONTROL_EXTERNAL            0xFE
#define USB_PROTOCOL_CDC_CONTROL_VENDOR              0xFF

/// USB_CLASS_HID
#define USB_SUBCLASS_HID_NONE                        0x00
#define USB_SUBCLASS_HID_BOOT_INTERFACE              0x01
#define USB_PROTOCOL_HID_NONE                        0x00
#define USB_PROTOCOL_HID_KEYBOARD                    0x01
#define USB_PROTOCOL_HID_MOUSE                       0x02

/// USB_CLASS_IMAGE
#define USB_SUBCLASS_IMAGE_STILLIMAGE                0x01
#define USB_PROTOCOL_IMAGE_PIMA                      0x01

/// USB_CLASS_PRINTER
#define USB_SUBCLASS_PRINTER                         0x01
#define USB_PROTOCOL_PRINTER_UNIDIRECTIONAL          0x01
#define USB_PROTOCOL_PRINTER_BIDIRECTIONAL           0x02
#define USB_PROTOCOL_PRINTER_1284_4_BIDIRECTIONAL    0x03

/// USB_CLASS_MASS_STORAGE
#define USB_SUBCLASS_MASS_STORAGE_SCSI               0x06
#define USB_PROTOCOL_MASS_STORAGE_BOMS               0x50

/// USB_CLASS_HUB
#define USB_SUBCLASS_HUB                             0x00
#define USB_PROTOCOL_HUB_FULL_SPEED                  0x00
#define USB_PROTOCOL_HUB_HI_SPEED_S_TT               0x01
#define USB_PROTOCOL_HUB_HI_SPEED_M_TT               0x02

/// USB_CLASS_CDC_DATA
#define USB_SUBCLASS_CDC_DATA                        0x00
#define USB_PROTOCOL_CDC_DATA                        0x00
#define USB_PROTOCOL_CDC_NETWORK_TRANSFER_BLOCK      0x01
#define USB_PROTOCOL_CDC_ISDN_BRI                    0x30
#define USB_PROTOCOL_CDC_HDLC                        0x31
#define USB_PROTOCOL_CDC_TRANSPARENT                 0x32
#define USB_PROTOCOL_CDC_Q921M                       0x50
#define USB_PROTOCOL_CDC_Q921                        0x51
#define USB_PROTOCOL_CDC_Q921TM                      0x52
#define USB_PROTOCOL_CDC_DATA_COMPRESSION            0x90
#define USB_PROTOCOL_CDC_EURO_ISDN                   0x91
#define USB_PROTOCOL_CDC_V24_RATE_ADAPTATION         0x92
#define USB_PROTOCOL_CDC_CAPI                        0x93

/// USB_CLASS_VIDEO
#define USB_SUBCLASS_VIDEO_UNDEFINED                 0x00
#define USB_SUBCLASS_VIDEO_VIDEOCONTROL              0x01
#define USB_SUBCLASS_VIDEO_VIDEOSTREAMING            0x02
#define USB_SUBCLASS_VIDEO_INTERFACE_COLLECTION      0x03
#define USB_PROTOCOL_VIDEO_UNDEFINED                 0x00

/// USB_CLASS_APPLICATION
/// Device Firmware Upgrade Class
#define USB_SUBCLASS_DFU                             0x01
#define USB_PROTOCOL_DFU_RUNTIME                     0x01
#define USB_PROTOCOL_DFU_DFUMODE                     0x02
/// IrDA Bridge Class
#define USB_SUBCLASS_IRDA_BRIDGE                     0x02
/// Test and Measurement Class
#define USB_SUBCLASS_USBTMC                          0x03
#define USB_PROTOCOL_USBTMC                          0x00
#define USB_PROTOCOL_USBTMC_USB488                   0x01

/// USB_CLASS_VENDOR
#define USB_SUBCLASS_VENDOR_NONE                     0x00
/// Android Debugger interfaces all have the following interface subclass:
#define USB_SUBCLASS_VENDOR_ADB                      0x42
#define USB_SUBCLASS_VENDOR_VENDOR                   0xff
#define USB_PROTOCOL_VENDOR_NONE                     0x00
//@}

/**
    @name Bridgetek USB Version Information
    @brief Definitions of USB Version BCD codes.
 */
//@{
/// USB Version 2.0
#define USB_BCD_VERSION_2_0 0x0200 
//@}

/** 
    @name USB Standard Request Codes
    @brief Table 9-4. Standard Request Codes.
        Used as bRequest in USB_device_request and bDescriptorType 
        in USB_device_descriptor.
 */
//@{
/// 9.4.5 Get Status
#define USB_REQUEST_CODE_GET_STATUS                   0x00    
/// 9.4.1 Clear Feature
#define USB_REQUEST_CODE_CLEAR_FEATURE                0x01    
/// 9.4.9 Set Feature
#define USB_REQUEST_CODE_SET_FEATURE                  0x03    
/// 9.4.6 Set Address
#define USB_REQUEST_CODE_SET_ADDRESS                  0x05    
/// 9.4.3 Get Descriptor
#define USB_REQUEST_CODE_GET_DESCRIPTOR               0x06    
/// 9.4.8 Set Descriptor
#define USB_REQUEST_CODE_SET_DESCRIPTOR               0x07    
/// 9.4.2 Get Configuration
#define USB_REQUEST_CODE_GET_CONFIGURATION            0x08    
/// 9.4.7 Set Configuration
#define USB_REQUEST_CODE_SET_CONFIGURATION            0x09    
/// 9.4.4 Get Interface
#define USB_REQUEST_CODE_GET_INTERFACE                0x0A    
/// 9.4.10 Set Interface
#define USB_REQUEST_CODE_SET_INTERFACE                0x0B    
/// 9.4.11 Synch Frame
#define USB_REQUEST_CODE_SYNCH_FRAME                  0x0C    
//@}

/** 
    @name USB Standard Feature Selectors
    @brief Table 9-6 Standard Feature Selectors
    Used for wValue high byte in 9.4.1 Clear Feature, 9.4.9 Set Feature.
 */
//@{
/// Device
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP 1
/// Endpoint
#define USB_FEATURE_ENDPOINT_HALT 0
/// Device
#define USB_FEATURE_TEST_MODE 2

/**
    @name USB Standard Get Status Response
    @brief Figure 9-4 Information Returned by a GetStatus() Request to a Device and
        Figure 9-6 Information Returned by a GetStatus() Request to an Endpoint
    Used for 2 bytes of data returned by Get Status request.
 */
//@{
#define USB_GET_STATUS_DEVICE_REMOTE_WAKEUP 2
#define USB_GET_STATUS_DEVICE_SELF_POWERED 1
#define USB_GET_STATUS_ENDPOINT_HALT 1

/**
    @name USB Descriptor Types
    @brief Table 9-5. Descriptor Types.
    Used for wValue high byte in 9.4.3 Get Descriptor, 9.4.8 Set Descriptor.
 */
//@{
/// 9.6.1 Device
#define USB_DESCRIPTOR_TYPE_DEVICE                    0x01
/// 9.6.3 Configuration
#define USB_DESCRIPTOR_TYPE_CONFIGURATION             0x02
/// 9.6.7 String
#define USB_DESCRIPTOR_TYPE_STRING                    0x03
/// 9.6.5 Interface
#define USB_DESCRIPTOR_TYPE_INTERFACE                 0x04
/// 9.6.6 Interface
#define USB_DESCRIPTOR_TYPE_ENDPOINT                  0x05
/// 9.6.2 Device Qualifier
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER          0x06
/// 9.6.4 Other Speed Configuration
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION 0x07
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER           0x08
#define USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION     0x0B
//@}

/**
    @name USB bmRequestType Characteristics
    @brief Table 9-2. Format of Setup Data.
    Used for bmRequestType decoding.
 */
//@{
/// Data transfer direction
#define USB_BMREQUESTTYPE_DIR_MASK                    0x80
#define USB_BMREQUESTTYPE_DIR_HOST_TO_DEV             0x00
#define USB_BMREQUESTTYPE_DIR_DEV_TO_HOST             0x80
/// Transaction type
#define USB_BMREQUESTTYPE_TYPE_MASK                   0x60
#define USB_BMREQUESTTYPE_STANDARD                    0x00
#define USB_BMREQUESTTYPE_CLASS                       0x20
#define USB_BMREQUESTTYPE_VENDOR                      0x40
/// Transaction Recipient
#define USB_BMREQUESTTYPE_RECIPIENT_MASK              0x1F
#define USB_BMREQUESTTYPE_RECIPIENT_DEVICE            0x00
#define USB_BMREQUESTTYPE_RECIPIENT_INTERFACE         0x01
#define USB_BMREQUESTTYPE_RECIPIENT_ENDPOINT          0x02
// Hub Transaction Recipient
#define USB_BMREQUESTTYPE_RECIPIENT_HUB               0x00
#define USB_BMREQUESTTYPE_RECIPIENT_PORT              0x03
//@}

/**
    @name USB_config_descriptor bmAttributes
    @brief Config descriptor bmAttributes values.
 **/
//@{
#define USB_CONFIG_BMATTRIBUTES_REMOTE_WAKEUP     0x20
#define USB_CONFIG_BMATTRIBUTES_SELF_POWERED      0x40
#define USB_CONFIG_BMATTRIBUTES_RESERVED_SET_TO_1 0x80
//@}

/**
    @name USB_config_descriptor bEndpointAddress
    @brief Config descriptor bEndpointAddress values.
 **/
//@{
#define USB_ENDPOINT_DESCRIPTOR_EPADDR_MASK      0x80
#define USB_ENDPOINT_DESCRIPTOR_EPADDR_IN        0x80
#define USB_ENDPOINT_DESCRIPTOR_EPADDR_OUT       0x00
//@}

/**
    @name USB_config_descriptor bmAttributes
    @brief Config descriptor bmAttributes values.
 **/
//@{
#define USB_ENDPOINT_DESCRIPTOR_ATTR_MASK        0x03
#define USB_ENDPOINT_DESCRIPTOR_ATTR_CONTROL     0x00
#define USB_ENDPOINT_DESCRIPTOR_ATTR_ISOCHRONOUS 0x01
#define USB_ENDPOINT_DESCRIPTOR_ATTR_BULK        0x02
#define USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT   0x03
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_SYNC_MASK    0x0c
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_NO_SYNC      0x00
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_ASYNCHRONOUS 0x04
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_ADAPTIVE     0x08
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_SYNCHRONOUS  0x0c
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_USAGE_MASK              0x30
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_USAGE_DATA              0x00
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_USAGE_FEEDBACK          0x10
#define USB_ENDPOINT_DESCRIPTOR_ISOCHRONOUS_USAGE_IMPLICIT_FEEDBACK 0x20
//@}

/**
    @name USB_config_descriptor wMaxPacketSize Additional Transactions
    @brief Config descriptor wMaxPacketSize Additional Transaction values.
 **/
//@{
#define USB_ENDPOINT_DESCRIPTOR_MAXPACKET_ADDN_TRANSACTION_MASK    (0x3 << 11)
#define USB_ENDPOINT_DESCRIPTOR_MAXPACKET_ADDN_TRANSACTION_NONE    (0x0 << 11)
#define USB_ENDPOINT_DESCRIPTOR_MAXPACKET_ADDN_TRANSACTION_1       (0x1 << 11)
#define USB_ENDPOINT_DESCRIPTOR_MAXPACKET_ADDN_TRANSACTION_2       (0x2 << 11)
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

#define USB_MICROSOFT_WCID_FEATURE_WINDEX_COMPAT_ID	 	0x0004
#define USB_MICROSOFT_WCID_FEATURE_WINDEX_DEVICE_GUID 	0x0005

#define USB_MS_WCID_PROPERTY_DATATYPE_STRING_SINGLE		0x1
#define USB_MS_WCID_PROPERTY_DATATYPE_STRING_MULTI		0x7

//@}


/* TYPES ***************************************************************************/

// Pack structures to align on byte boundaries. Required for USB data structures.
#define PACK __attribute__ ((__packed__))

/**
    @struct USB_device_descriptor
    @brief Structure representing a USB Device Descriptor. USB Spec Table 9-8.
 **/
typedef struct PACK USB_device_descriptor
{
    /** Size of descriptor in bytes. **/
    uint8_t  bLength;
    /** DEVICE Descriptor Type (always 1). **/
    uint8_t  bDescriptorType;
    /** USB Specification Release Number. **/
    uint16_t bcdUSB;
    /** USB Class code. **/
    uint8_t  bDeviceClass;
    /** USB Subclass code. **/
    uint8_t  bDeviceSubClass;
    /** USB Protocol code. **/
    uint8_t  bDeviceProtocol;
    /** Max packet size for EP 0. **/
    uint8_t  bMaxPacketSize0;
    /** Vendor ID. **/
    uint16_t idVendor;
    /** Product ID. **/
    uint16_t idProduct;
    /** Device release number. **/
    uint16_t bcdDevice;
    /** Index of manufacturer string descriptor. **/
    uint8_t  iManufacturer;
    /** Index of product string descriptor. **/
    uint8_t  iProduct;
    /** Index of serial no. string descriptor. **/
    uint8_t  iSerialNumber;
    /** Number of possible configurations. **/
    uint8_t  bNumConfigurations;
} USB_device_descriptor;

/**
    @struct USB_device_descriptor
    @brief Structure representing a USB Device Descriptor. USB Spec Table 9-8.
 **/
typedef struct PACK USB_device_qualifier_descriptor
{
    /** Size of descriptor in bytes. **/
    uint8_t  bLength;
    /** DEVICE Descriptor Type (always 6). **/
    uint8_t  bDescriptorType;
    /** USB Specification Release Number. **/
    uint16_t bcdUSB;
    /** USB Class code. **/
    uint8_t  bDeviceClass;
    /** USB Subclass code. **/
    uint8_t  bDeviceSubClass;
    /** USB Protocol code. **/
    uint8_t  bDeviceProtocol;
    /** Max packet size for EP 0. **/
    uint8_t  bMaxPacketSize0;
    /** Number of possible configurations. **/
    uint8_t  bNumConfigurations;
    /** Reserved for future use, must be zero **/
    uint8_t  bReserved;
} USB_device_qualifier_descriptor;

/**
    @struct USB_configuration_descriptor
    @brief Structure representing a USB Configuration Descriptor.
           USB Spec Table 9-10.
 **/
typedef struct PACK USB_configuration_descriptor
{
    /** Size of descriptor in bytes. **/
    uint8_t  bLength;
    /** CONFIGURATION descriptor type (always 2). **/
    uint8_t  bDescriptorType;
    /** Total length of bytes returned for this configuration. **/
    uint16_t wTotalLength;
    /** Number of interfaces supported by this configuration. **/
    uint8_t  bNumInterfaces;
    /** Argument for \p SetConfiguration() request. **/
    uint8_t  bConfigurationValue;
    /** Index of configuration string descriptor. **/
    uint8_t  iConfiguration;
    /** Configuration characteristics. **/
    uint8_t  bmAttributes;
    /** Max power consumption of the USB device. **/
    uint8_t  bMaxPower;
} USB_configuration_descriptor;

/**
    @struct USB_interface_association_descriptor
    @brief Structure representing a USB Interface Association Descriptors.
           USB 2.0 Spec ECN.
 **/
typedef struct PACK USB_interface_association_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
} USB_interface_association_descriptor;

/**
    @struct USB_interface_descriptor
    @brief Structure representing a USB Interface Descriptor.
           USB Spec Table 9-12.
 **/
typedef struct PACK USB_interface_descriptor
{
    /** Size of descriptor in bytes. **/
    uint8_t  bLength;
    /** INTERFACE descriptor type (always 4). **/
    uint8_t  bDescriptorType;
    /** Zero-base interface number. **/
    uint8_t  bInterfaceNumber;
    /** Alternate setting for this interface. **/
    uint8_t  bAlternateSetting;
    /** Number of endpoints in this interface. **/
    uint8_t  bNumEndpoints;
    /** Interface class. **/
    uint8_t  bInterfaceClass;
    /** Interface subclass. **/
    uint8_t  bInterfaceSubClass;
    /** Interface protocol. **/
    uint8_t  bInterfaceProtocol;
    /** Interface String Descriptor. **/
    uint8_t  iInterface;
} USB_interface_descriptor;

/**
    @struct USB_endpoint_descriptor
    @brief Structure representing a USB Endpoint Descriptor.
           USB Spec Table 9-13.
 **/
typedef struct PACK USB_endpoint_descriptor
{
    /** Size of descriptor in bytes. **/
    uint8_t  bLength;
    /** ENDPOINT descriptor type (always 5). **/
    uint8_t  bDescriptorType;
    /** Endpoint address. **/
    uint8_t  bEndpointAddress;
    /** Endpoint attributes. **/
    uint8_t  bmAttributes;
    /** Endpoint maximum packet size. **/
    uint16_t wMaxPacketSize;
    /** Endpoint polling interval for interrupt endpoints. **/
    uint8_t  bInterval;
} USB_endpoint_descriptor;

/**
    @struct USB_string_descriptor
    @brief Structure representing a USB String Descriptor.
           USB Spec Table 9-xx.
           Add array of unicode characters after structure to bLength
 **/
typedef struct PACK USB_string_descriptor
{
    /** Size of descriptor in bytes. **/
    uint8_t  bLength;
    /** STRING descriptor type (always 3). **/
    uint8_t  bDescriptorType;
    /** Start of string pointer. Actual size bLength bytes. **/
    uint8_t  bString;
    // unsigned char chString[0];
    // unsigned char chString[...];
    // unsigned char chString[n];
} USB_string_descriptor;

/**
    @struct USB_device_request
    @brief Structure representing a USB Device Request.  USB Spec Table 9-2.
 **/
typedef struct PACK USB_device_request
{
    /**< \verbatim
        Table 9-2
        D7: Data transfer direction
            0 = Host-to-device
            1 = Device-to-host
        D6...5: Type
            0 = Standard
            1 = Class
            2 = Vendor
            3 = Reserved
        D4...0: Recipient
            0 = Device
            1 = Interface
            2 = Endpoint
            3 = Other
            4...31 = Reserved
    \endverbatim*/
    uint8_t  bmRequestType;
    /** Specific request. **/
    uint8_t  bRequest;
    /** Varies according to request. **/
    uint16_t wValue;
    /** Varies according to request. **/
    uint16_t wIndex;
    /** Number of bytes to transfer for data stage. **/
    uint16_t wLength;
} USB_device_request;
//@}

/**
    @struct Microsoft WCID Information
    @brief Definitions for WCID String Descriptors and vendor requests.
 **/
//@{
typedef struct PACK USB_WCID_feature_descriptor
{
	/** The length, in bytes, of the complete extended compat ID descriptor. **/
	uint32_t dwLength;
	/** The descriptor’s version number, in binary coded decimal (BCD) format. **/
	uint16_t bcdVersion;
	/** An index that identifies the particular OS feature descriptor. **/
	uint16_t wIndex;
	/** The number of custom property sections. **/
	uint8_t bCount;
	/** reserved - 7 bytes. **/
	uint8_t rsv1[7];
	/** The interface or function number. **/
	uint8_t bFirstInterfaceNumber;
	/** reserved - 1 byte. **/
	uint8_t rsv2;
	/** The function's compatible ID. **/
	uint8_t compatibleID[8];
	/** The function's subcompatible ID. **/
	uint8_t subCompatibleID[8];
	/** reserved - 6 bytes. **/
	uint8_t rsv3[6];
} USB_WCID_feature_descriptor;

typedef struct PACK USB_WCID_feature_descriptor_additional
{
	/** The interface or function number. **/
	uint8_t bFirstInterfaceNumber;
	/** reserved - 1 byte. **/
	uint8_t rsv2;
	/** The function's compatible ID. **/
	uint8_t compatibleID[8];
	/** The function's subcompatible ID. **/
	uint8_t subCompatibleID[8];
	/** reserved - 6 bytes. **/
	uint8_t rsv3[6];
} USB_WCID_feature_descriptor_additional;

/* Referred:
 * https://github.com/pbatard/libwdi/wiki/WCID-Devices
 */
typedef struct PACK USB_WCID_extended_property_descriptor
{
	/** The length, in bytes, of the complete extended property (Header and Property sections) descriptor (146 bytes) **/
	uint32_t dwLength;
	/** The descriptor’s version number, in binary coded decimal (BCD) format. **/
	uint16_t bcdVersion;
	/** An index that identifies the particular OS feature descriptor. **/
	uint16_t wIndex;
	/** The number of custom property sections. **/
	uint16_t wCount;
	/** The Length of this custom property section  (136 bytes)  **/
	uint32_t dwSize;
	/** Property value stores a Unicode string **/
	uint32_t dwPropertyDataType;
	/** Length of the property name string  (42 bytes) **/
	uint16_t wPropertyNameLength;
	/** Property name is “DeviceInterfaceGUIDs”. Null terminated **/
	uint8_t bPropertyName[42];
	/** Length of the property value string (80 bytes) **/
	uint32_t dwPropertyDataLength;
	/** Property value in unicode e.g.“{8FE6D4D7-49DD-41E7-9486-49AFC6BFE470}”. Double Null termination **/
	uint8_t bPropertyData[80];
} USB_WCID_extended_property_descriptor;
//@}

/** @name USB Language Identifiers
    @details For use with USB_REQUEST_CODE_GET_DESCRIPTOR for
 	 	 	 string descriptors (LANGID is the wIndex value)
 	 	 	 from USB Language Identifiers (LANGIDs) PDF Page 4
 **/
//@{
#define USB_LANGID_AFRIKAANS				  0x0436
#define USB_LANGID_ALBANIAN					  0x041c
#define USB_LANGID_ARABIC_SAUDI_ARABIA		  0x0401
#define USB_LANGID_ARABIC_IRAQ				  0x0801
#define USB_LANGID_ARABIC_EGYPT				  0x0c01
#define USB_LANGID_ARABIC_LIBYA				  0x1001
#define USB_LANGID_ARABIC_ALGERIA			  0x1401
#define USB_LANGID_ARABIC_MOROCCO			  0x1801
#define USB_LANGID_ARABIC_TUNISIA			  0x1c01
#define USB_LANGID_ARABIC_OMAN				  0x2001
#define USB_LANGID_ARABIC_YEMEN				  0x2401
#define USB_LANGID_ARABIC_SYRIA				  0x2801
#define USB_LANGID_ARABIC_JORDAN			  0x2c01
#define USB_LANGID_ARABIC_LEBANON			  0x3001
#define USB_LANGID_ARABIC_KUWAIT			  0x3401
#define USB_LANGID_ARABIC_UAE				  0x3801
#define USB_LANGID_ARABIC_BAHRAIN			  0x3c01
#define USB_LANGID_ARABIC_QATAR				  0x4001
#define USB_LANGID_ARMENIAN					  0x042b
#define USB_LANGID_ASSAMESE					  0x044d
#define USB_LANGID_AZERI_LATIN				  0x042c
#define USB_LANGID_AZERI_CYRILLIC			  0x082c
#define USB_LANGID_BASQUE					  0x042d
#define USB_LANGID_BELARUSSIAN				  0x0423
#define USB_LANGID_BENGALI					  0x0445
#define USB_LANGID_BULGARIAN				  0x0402
#define USB_LANGID_BURMESE					  0x0455
#define USB_LANGID_CATALAN					  0x0403
#define USB_LANGID_CHINESE_TAIWAN			  0x0404
#define USB_LANGID_CHINESE_PRC				  0x0804
#define USB_LANGID_CHINESE_HONG_KONG_SAR_PRC  0x0c04
#define USB_LANGID_CHINESE_SINGAPORE		  0x1004
#define USB_LANGID_CHINESE_MACAU_SAR		  0x1404
#define USB_LANGID_CROATIAN					  0x041a
#define USB_LANGID_CZECH					  0x0405
#define USB_LANGID_DANISH					  0x0406
#define USB_LANGID_DUTCH_NETHERLANDS		  0x0413
#define USB_LANGID_DUTCH_BELGIUM			  0x0813
#define USB_LANGID_ENGLISH_UNITED_STATES	  0x0409
#define USB_LANGID_ENGLISH_UNITED_KINGDOM	  0x0809
#define USB_LANGID_ENGLISH_AUSTRALIAN		  0x0c09
#define USB_LANGID_ENGLISH_CANADIAN			  0x1009
#define USB_LANGID_ENGLISH_NEW_ZEALAND		  0x1409
#define USB_LANGID_ENGLISH_IRELAND			  0x1809
#define USB_LANGID_ENGLISH_SOUTH_AFRICA		  0x1c09
#define USB_LANGID_ENGLISH_JAMAICA			  0x2009
#define USB_LANGID_ENGLISH_CARIBBEAN		  0x2409
#define USB_LANGID_ENGLISH_BELIZE			  0x2809
#define USB_LANGID_ENGLISH_TRINIDAD			  0x2c09
#define USB_LANGID_ENGLISH_ZIMBABWE			  0x3009
#define USB_LANGID_ENGLISH_PHILIPPINES		  0x3409
#define USB_LANGID_ESTONIAN					  0x0425
#define USB_LANGID_FAEROESE					  0x0438
#define USB_LANGID_FARSI					  0x0429
#define USB_LANGID_FINNISH					  0x040b
#define USB_LANGID_FRENCH_STANDARD			  0x040c
#define USB_LANGID_FRENCH_BELGIAN			  0x080c
#define USB_LANGID_FRENCH_CANADIAN			  0x0c0c
#define USB_LANGID_FRENCH_SWITZERLAND		  0x100c
#define USB_LANGID_FRENCH_LUXEMBOURG		  0x140c
#define USB_LANGID_FRENCH_MONACO			  0x180c
#define USB_LANGID_GEORGIAN					  0x0437
#define USB_LANGID_GERMAN_STANDARD			  0x0407
#define USB_LANGID_GERMAN_SWITZERLAND		  0x0807
#define USB_LANGID_GERMAN_AUSTRIA			  0x0c07
#define USB_LANGID_GERMAN_LUXEMBOURG		  0x1007
#define USB_LANGID_GERMAN_LIECHTENSTEIN		  0x1407
#define USB_LANGID_GREEK					  0x0408
#define USB_LANGID_GUJARATI					  0x0447
#define USB_LANGID_HEBREW					  0x040d
#define USB_LANGID_HINDI					  0x0439
#define USB_LANGID_HUNGARIAN				  0x040e
#define USB_LANGID_ICELANDIC				  0x040f
#define USB_LANGID_INDONESIAN				  0x0421
#define USB_LANGID_ITALIAN_STANDARD			  0x0410
#define USB_LANGID_ITALIAN_SWITZERLAND		  0x0810
#define USB_LANGID_JAPANESE					  0x0411
#define USB_LANGID_KANNADA					  0x044b
#define USB_LANGID_KASHMIRI_INDIA			  0x0860
#define USB_LANGID_KAZAKH					  0x043f
#define USB_LANGID_KONKANI					  0x0457
#define USB_LANGID_KOREAN					  0x0412
#define USB_LANGID_KOREAN_JOHAB				  0x0812
#define USB_LANGID_LATVIAN					  0x0426
#define USB_LANGID_LITHUANIAN				  0x0427
#define USB_LANGID_LITHUANIAN_CLASSIC		  0x0827
#define USB_LANGID_MACEDONIAN				  0x042f
#define USB_LANGID_MALAY_MALAYSIAN			  0x043e
#define USB_LANGID_MALAY_BRUNEI_DARUSSALAM	  0x083e
#define USB_LANGID_MALAYALAM				  0x044c
#define USB_LANGID_MANIPURI					  0x0458
#define USB_LANGID_MARATHI					  0x044e
#define USB_LANGID_NEPALI_INDIA				  0x0861
#define USB_LANGID_NORWEGIAN_BOKMAL			  0x0414
#define USB_LANGID_NORWEGIAN_NYNORSK		  0x0814
#define USB_LANGID_ORIYA					  0x0448
#define USB_LANGID_POLISH					  0x0415
#define USB_LANGID_PORTUGUESE_BRAZIL		  0x0416
#define USB_LANGID_PORTUGUESE_STANDARD		  0x0816
#define USB_LANGID_PUNJABI					  0x0446
#define USB_LANGID_ROMANIAN					  0x0418
#define USB_LANGID_RUSSIAN					  0x0419
#define USB_LANGID_SANSKRIT					  0x044f
#define USB_LANGID_SERBIAN_CYRILLIC			  0x0c1a
#define USB_LANGID_SERBIAN_LATIN			  0x081a
#define USB_LANGID_SINDHI					  0x0459
#define USB_LANGID_SLOVAK					  0x041b
#define USB_LANGID_SLOVENIAN				  0x0424
#define USB_LANGID_SPANISH_TRADITIONAL_SORT	  0x040a
#define USB_LANGID_SPANISH_MEXICAN			  0x080a
#define USB_LANGID_SPANISH_MODERN_SORT		  0x0c0a
#define USB_LANGID_SPANISH_GUATEMALA		  0x100a
#define USB_LANGID_SPANISH_COSTA_RICA		  0x140a
#define USB_LANGID_SPANISH_PANAMA			  0x180a
#define USB_LANGID_SPANISH_DOMINICAN_REPUBLIC 0x1c0a
#define USB_LANGID_SPANISH_VENEZUELA		  0x200a
#define USB_LANGID_SPANISH_COLOMBIA			  0x240a
#define USB_LANGID_SPANISH_PERU				  0x280a
#define USB_LANGID_SPANISH_ARGENTINA		  0x2c0a
#define USB_LANGID_SPANISH_ECUADOR			  0x300a
#define USB_LANGID_SPANISH_CHILE			  0x340a
#define USB_LANGID_SPANISH_URUGUAY			  0x380a
#define USB_LANGID_SPANISH_PARAGUAY			  0x3c0a
#define USB_LANGID_SPANISH_BOLIVIA			  0x400a
#define USB_LANGID_SPANISH_EL_SALVADOR		  0x440a
#define USB_LANGID_SPANISH_HONDURAS			  0x480a
#define USB_LANGID_SPANISH_NICARAGUA		  0x4c0a
#define USB_LANGID_SPANISH_PUERTO_RICO		  0x500a
#define USB_LANGID_SUTU						  0x0430
#define USB_LANGID_SWAHILI_KENYA			  0x0441
#define USB_LANGID_SWEDISH					  0x041d
#define USB_LANGID_SWEDISH_FINLAND			  0x081d
#define USB_LANGID_TAMIL					  0x0449
#define USB_LANGID_TATAR_TATARSTAN			  0x0444
#define USB_LANGID_TELUGU					  0x044a
#define USB_LANGID_THAI						  0x041e
#define USB_LANGID_TURKISH					  0x041f
#define USB_LANGID_UKRAINIAN				  0x0422
#define USB_LANGID_URDU_PAKISTAN			  0x0420
#define USB_LANGID_URDU_INDIA				  0x0820
#define USB_LANGID_UZBEK_LATIN				  0x0443
#define USB_LANGID_UZBEK_CYRILLIC			  0x0843
#define USB_LANGID_VIETNAMESE				  0x042a
#define USB_LANGID_HID_USAGE_DATA_DESCRIPTOR  0x04ff
#define USB_LANGID_HID_VENDOR_DEFINED_1		  0xf0ff
#define USB_LANGID_HID_VENDOR_DEFINED_2		  0xf4ff
#define USB_LANGID_HID_VENDOR_DEFINED_3		  0xf8ff
#define USB_LANGID_HID_VENDOR_DEFINED_4		  0xfcff
//@}

/* End of USB Standard Specification */
//@}


/** @name USB Hub Specification
 */
//@{

/* CONSTANTS ***********************************************************************/

/**
    @name USB_device_request bRequest for hubs
    @brief Hub bRequest values.
 **/
//@{
#define USB_HUB_REQUEST_CODE_GET_STATUS           0
#define USB_HUB_REQUEST_CODE_CLEAR_FEATURE        1
#define USB_HUB_REQUEST_CODE_SET_FEATURE          3
#define USB_HUB_REQUEST_CODE_GET_DESCRIPTOR       6
#define USB_HUB_REQUEST_CODE_SET_DESCRIPTOR       7
#define USB_HUB_REQUEST_CODE_CLEAR_TT_BUFFER      8
#define USB_HUB_REQUEST_CODE_RESET_TT             9
#define USB_HUB_REQUEST_CODE_GET_TT_STATE         10
#define USB_HUB_REQUEST_CODE_STOP_TT              11
//@}

/**
    @name USB_config_descriptor bmAttributes
    @brief Config descriptor bmAttributes value for hubs. Table 11-13. Hub Descriptor.
 **/
//@{
#define USB_DESCRIPTOR_TYPE_HUB                   0x29
//@}

/**
    @name USB_device_request Get/Set Feature values for hubs.
    @brief Request codes for hub features. Table 11-17. Hub Class Feature Selectors.
 **/
//@{
// Hub Features
#define USB_HUB_CLASS_FEATURE_C_HUB_LOCAL_POWER   0
#define USB_HUB_CLASS_FEATURE_C_HUB_OVER_CURRENT  1
// Port Features
#define USB_HUB_CLASS_FEATURE_PORT_CONNECTION     0
#define USB_HUB_CLASS_FEATURE_PORT_ENABLE         1
#define USB_HUB_CLASS_FEATURE_PORT_SUSPEND        2
#define USB_HUB_CLASS_FEATURE_PORT_OVER_CURRENT   3
#define USB_HUB_CLASS_FEATURE_PORT_RESET          4
#define USB_HUB_CLASS_FEATURE_PORT_POWER          8
#define USB_HUB_CLASS_FEATURE_PORT_LOW_SPEED      9
#define USB_HUB_CLASS_FEATURE_C_PORT_CONNECTION   16
#define USB_HUB_CLASS_FEATURE_C_PORT_ENABLE       17
#define USB_HUB_CLASS_FEATURE_C_PORT_SUSPEND      18
#define USB_HUB_CLASS_FEATURE_C_PORT_OVER_CURRENT 19
#define USB_HUB_CLASS_FEATURE_C_PORT_RESET        20
#define USB_HUB_CLASS_FEATURE_PORT_TEST           21
#define USB_HUB_CLASS_FEATURE_PORT_INDICATOR      22
//@}

/* TYPES ***************************************************************************/

/**
    @struct USB_hub_descriptor
    @brief Structure representing a USB HUB descriptor.  USB Spec Table 11-13.
 **/
typedef struct PACK USB_hub_descriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bNbrPorts;
    uint16_t wHubCharacteristics;
    uint8_t bPwrOn2PwrGood;
    uint8_t bHubContrCurrent;
    // from this point on there are variable size fields depending
    // on bNbrPorts. One bit per provided for compatibility reasons
    // with V1.0 hubs only. Minimum sizes of these are 1 byte.
    uint8_t DeviceRemovable[1];
    uint8_t PortPwrCtrlMask[1];
} USB_hub_descriptor;

/**
    @struct USB_hub_status
    @brief Structure representing a USB HUB status or change field.
    wHubStatus and wHubChange USB Spec Table 11-19 and 11-20.
 **/
typedef struct PACK USB_hub_status
{
    // first word - hub status
    uint16_t localPowerSource : 1;
    uint16_t overCurrent : 1;
    uint16_t resv1 : 14;
    // second word - hub status change
    uint16_t localPowerSourceChange : 1;
    uint16_t overCurrentChange : 1;
    uint16_t resv2 : 14;
} USB_hub_status;

/**
    @struct USB_hub_port_status
    @brief Structure representing a USB HUB port status or change field.
    wPortStatus USB Spec Table 11-21.
 **/
typedef struct PACK USB_hub_port_status
{
    // first word - port status
    uint16_t currentConnectStatus : 1;       // 0
    uint16_t portEnabled : 1;                // 1
    uint16_t portSuspend : 1;                // 2
    uint16_t portOverCurrent : 1;            // 3
    uint16_t portReset : 1;                  // 4
    uint16_t resv1 : 3;                      // 5..7
    uint16_t portPower : 1;                  // 8
    uint16_t portLowSpeed : 1;               // 9
    uint16_t portHighSpeed : 1;              // 10
    uint16_t portTest : 1;                   // 11
    uint16_t portIndicator : 1;              // 12
    uint16_t resv2 : 3;                      // 13..15
    // second word - port status change
    uint16_t currentConnectStatusChange : 1; // 0
    uint16_t portEnabledChange : 1;          // 1
    uint16_t portSuspendChange : 1;          // 2
    uint16_t portOverCurrentChange : 1;      // 3
    uint16_t portResetChange : 1;            // 4
    uint16_t resv3 : 3;                      // 5..7
    uint16_t portPowerChange : 1;            // 8
    uint16_t portLowSpeedChange : 1;         // 9
    uint16_t portHighSpeedChange : 1;        // 10
    uint16_t portTestChange : 1;             // 11
    uint16_t portIndicatorChange : 1;        // 12
    uint16_t resv4 : 3;                      // 13..15
} USB_hub_port_status;

/**
    @struct USB_hub_port_selector
    @brief Structure representing a USB HUB port selector.
 **/
typedef struct PACK USB_hub_port_selector {
    uint8_t hub_port;
    uint8_t selector;
} USB_hub_port_selector;

/* End of USB Hub Specification */
//@}

/** @name Common VID/PID Combinations
 */
//@{

/* CONSTANTS ***********************************************************************/

/**
    @name FTDI USB Information
    @brief Definitions of USB Vendor and Device IDs.
 */
//@{
/// FTDI Vendor ID
#define USB_VID_FTDI                            0x0403
/// FTDI Product ID for FT232 variants
#define USB_PID_FTDI_FT232                      0x6001
/// FTDI Product ID for FT2232 variants
#define USB_PID_FTDI_FT2232                     0x6010
/// FTDI Product ID for FT4232 variants
#define USB_PID_FTDI_FT4232                     0x6011
/// FTDI Product ID for FT232H
#define USB_PID_FTDI_FT232H 					0x6014
/// FTDI Product ID for FTX series
#define USB_PID_FTDI_FT_X_SERIES                0x6015

///FTDI predefined DFU Mode Product ID.
#if defined(__FT930__)
#define USB_DFU_PID_DFUMODE 					0x0fcf
#else
#define USB_DFU_PID_DFUMODE 					0x0fde
#endif
/* End of Common VID/PID Combinations */
//@}

/**
 @name DFU_USB_BCD_DEVICEID configuration.
 @brief FTDI predefined DFU Mode BCD Device ID.
 */
//@{
#define USB_DFU_BCD_DEVICEID	 	0x2300
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // FT900_USB_H_
