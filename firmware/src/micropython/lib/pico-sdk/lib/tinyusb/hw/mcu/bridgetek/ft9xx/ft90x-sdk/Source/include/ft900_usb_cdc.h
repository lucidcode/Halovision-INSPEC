/**
    @file ft900_usb_cdc.h

    @brief
    USB CDC class USB definitions.

	@details
    This contains USB definitions of constants and structures which are used in
    USB CDC implementations.

    Definitions for USB CDC Devices based on USB Class Definitions for
    Communications Devices Specification Revision 1.1 from
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef FT900_USB_CDC_H_
#define FT900_USB_CDC_H_

/* INCLUDES ************************************************************************/

#include "ft900_usb.h"

/* CONSTANTS ***********************************************************************/

/**
    @name USB CDC Request Codes
    @brief Common to several CDC subclasses.
        Section 3.6.2.1 Table 4 Abstract Control Model Request Codes.
        Used as bRequest in USB_device_request and bDescriptorType
        in USB_device_descriptor.
 */
//@{
/// Issues a command in the format of the supported control protocol.
#define USB_CDC_REQUEST_SEND_ENCAPSULATED_COMMAND 0x00
/// Requests a response in the format of the supported control protocol.
#define USB_CDC_REQUEST_GET_ENCAPSULATED_RESPONSE 0x01
/// Controls the settings for a particular communication feature.
#define USB_CDC_REQUEST_SET_COMM_FEATURE 0x02
/// Returns the current settings for the communication feature.
#define USB_CDC_REQUEST_GET_COMM_FEATURE 0x03
/// Clears the settings for a particular communication feature.
#define USB_CDC_REQUEST_CLEAR_COMM_FEATURE 0x04
/// Configures DTE rate, stop-bits, parity, and number-of-character bits.
#define USB_CDC_REQUEST_SET_LINE_CODING 0x20
/// Requests current DTE rate, stop-bits, parity, and number-of-character bits.
#define USB_CDC_REQUEST_GET_LINE_CODING 0x21
/// RS-232 signal used to tell the DCE device the DTE device is now present.
#define USB_CDC_REQUEST_SET_CONTROL_LINE_STATE 0x22
/// Sends special carrier modulation used to specify RS-232 style break.
#define USB_CDC_REQUEST_SEND_BREAK 0x23
//@}

/**
    @name USB CDC ACM Notification Codes
    @brief Section 3.6.2.1 Table 5 Abstract Control Model Notification Codes.
 */
//@{
/// Notification to host of network connection status. Optional
#define USB_CDC_NOTIFICATION_NETWORK_CONNECTION 0x00
/// Notification to host to issue a GET_ENCAPSULATED_RESPONSE request.
#define USB_CDC_NOTIFICATION_RESPONSE_AVAILABLE 0x01
/// Auxilliary Jack Hook state changed.
#define USB_CDC_NOTIFICAITON_AUX_JACK_HOOK_STATE 0x08
/// Ring detected.
#define USB_CDC_NOTIFICAITON_RING_DETECT 0x09
/// Returns the current state of the carrier detect, DSR, break, and ring signal.
#define USB_CDC_NOTIFICAITON_SERIAL_STATE 0x20
/// Call state changed.
#define USB_CDC_NOTIFICAITON_CALL_STATE_CHANGE 0x28
/// Line state changed.
#define USB_CDC_NOTIFICAITON_LINE_STATE_CHANGE 0x29
/// Ethernet device connection speed change.
#define USB_CDC_NOTIFICAITON_CONNECTION_SPEED_CHANGE 0x2A
//@}

/**
    @name USB CDC Ethernet Request Codes
    @brief Table 6 Ethernet Request Codes.
        Used as bRequest in USB_device_request and bDescriptorType
        in USB_device_descriptor.
 */
//@{
/// Change settings of the Networking device's multicast filters.
#define USB_CDC_REQUEST_SET_ETHERNET_MULTICAST_FILTERS 0x40
/// Set the pattern filtering used to wake up the attached host
/// on demand.
#define USB_CDC_REQUEST_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER 0x41
/// Request the pattern used to wake up the attached host on demand.
#define USB_CDC_REQUEST_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER 0x42
/// Sets device filter for running a network analyser application
/// on the host machine.
#define USB_CDC_REQUEST_SET_ETHERNET_PACKET_FILTER 0x43
/// Retrieves Ethernet device statistics such as frames
/// transmitted, frames received, and bad frames received.
#define USB_CDC_REQUEST_GET_ETHERNET_STATISTIC 0x44
//@}

/**
    @name USB CDC NCM Request Codes
    @brief Table 6-2 Network Control Model Request Codes.
        Used as bRequest in USB_device_request and bDescriptorType
        in USB_device_descriptor.
 */
//@{
/// Requests the function to report parameters that characterize
/// the Network Control Block.
#define USB_CDC_REQUEST_GET_NTB_PARAMETERS 0x80
/// Requests the current EUI-48 network address.
#define USB_CDC_REQUEST_GET_NET_ADDRESS 0x81
/// Changes the current EUI-48 network address.
#define USB_CDC_REQUEST_SET_NET_ADDRESS 0x82
/// Get current NTB Format.
#define USB_CDC_REQUEST_GET_NTB_FORMAT 0x83
/// Select 16 or 32 bit Network Transfer Blocks.
#define USB_CDC_REQUEST_SET_NTB_FORMAT 0x84
/// Get the current value of maximum NTB input size.
#define USB_CDC_REQUEST_GET_NTB_INPUT_SIZE 0x85
/// Selects the maximum size of NTBs to be transmitted by the
/// function over the bulk IN pipe.
#define USB_CDC_REQUEST_SET_NTB_INPUT_SIZE 0x86
/// Requests the current maximum datagram size.
#define USB_CDC_REQUEST_GET_MAX_DATAGRAM_SIZE 0x87
/// Sets the maximum datagram size to a value other than the default.
#define USB_CDC_REQUEST_SET_MAX_DATAGRAM_SIZE 0x88
/// Requests the current CRC mode.
#define USB_CDC_REQUEST_GET_CRC_MODE 0x89
/// Sets the current CRC mode.
#define USB_CDC_REQUEST_SET_CRC_MODE 0x8A
//@}

/**
    @name USB CDC Line Coding
    @brief Section 6.2.13 Table 50 Line Coding.
 */
//@{
/// Data Bits
#define USB_CDC_DATA_BITS_5		5
#define USB_CDC_DATA_BITS_6		6
#define USB_CDC_DATA_BITS_7		7
#define USB_CDC_DATA_BITS_8		8
/// Stop Bits
#define USB_CDC_STOP_BITS_1		0
#define USB_CDC_STOP_BITS_1PT5	1
#define USB_CDC_STOP_BITS_2		2
/// Parity
#define USB_CDC_PARITY_NONE		0
#define USB_CDC_PARITY_ODD		1
#define USB_CDC_PARITY_EVEN		2
#define USB_CDC_PARITY_MARK		3
#define USB_CDC_PARITY_SPACE		4
//@}

/**
    @name USB CDC Control Line State
    @brief Section 6.2.14 Table 51 Control Signal Bitmap Values.
 */
//@{
#define USB_CDC_DTE_PRESENT		1
#define USB_CDC_ACTIVATE_CARRIER 	2
//@}

/**
    @name USB CDC Type Values
    @brief Section 5.2.3 Table 24 Type Values for Class specific
    Configuration Descriptors.
 */
//@{
#define USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE 0x24
#define USB_CDC_DESCRIPTOR_TYPE_CS_ENDPOINT 0x25
//@}

/**
    @name USB CDC Subtype Values
    @brief Section 5.2.3 Table 25 Subtype Values for Class specific
    Configuration Descriptors.
 */
//@{
#define USB_CDC_DESCRIPTOR_SUBTYPE_HEADER_FUNCTIONAL_DESCRIPTOR 0x00
#define USB_CDC_DESCRIPTOR_SUBTYPE_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR 0x01
#define USB_CDC_DESCRIPTOR_SUBTYPE_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR 0x02
#define USB_CDC_DESCRIPTOR_SUBTYPE_DIRECT_LINE_MANAGEMENT_FUNCTIONAL_DESCRIPTOR 0x03 /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_TELEPHONE_RINGER_FUNCTIONAL_DESCRIPTOR 0x04 /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_TELEPHONE_CALL_REPORTING_FUNCTIONAL_DESCRIPTOR 0x05 /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_UNION_FUNCTIONAL_DESCRIPTOR 0x06
#define USB_CDC_DESCRIPTOR_SUBTYPE_COUNTRY_SELECTION_FUNCTIONAL_DESCRIPTOR 0x07 /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_TELEPHONE_OPERATIONAL_MODES_FUNCTIONAL_DESCRIPTOR 0x08 /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_USB_TERMINAL_FUNCTIONAL_DESCRIPTOR 0x09 /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_NETWORK_CHANNEL_TERMINAL_FUNCTIONAL_DESCRIPTOR 0x0a /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_PROTOCOL_UNIT_FUNCTIONAL_DESCRIPTOR 0x0b /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_EXTENSION_UNIT_FUNCTIONAL_DESCRIPTOR 0x0c /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_MCM_FUNCTIONAL_DESCRIPTOR 0x0d /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_CAPI_FUNCTIONAL_DESCRIPTOR 0x0e /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_ETHERNET_NETWORKING_FUNCTIONAL_DESCRIPTOR 0x0f
#define USB_CDC_DESCRIPTOR_SUBTYPE_ATM_NETWORKING_FUNCTIONAL_DESCRIPTOR 0x10 /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_WIRELESS_HANDSET_CONTROL_MODEL_FUNCTIONAL_DESCRIPTOR 0x11 /// Not implemented
#define USB_CDC_DESCRIPTOR_SUBTYPE_MOBILE_DIRECT_LINE_MODEL_FUNCTIONAL_DESCRIPTOR 0x12
#define USB_CDC_DESCRIPTOR_SUBTYPE_MDLM_DETAIL__FUNCTIONAL_DESCRIPTOR 0x13
#define USB_CDC_DESCRIPTOR_SUBTYPE_DEVICE_MANAGEMENT_MODEL_FUNCTIONAL_DESCRIPTOR 0x14
#define USB_CDC_DESCRIPTOR_SUBTYPE_OBEX_FUNCTIONAL_DESCRIPTOR 0x15
#define USB_CDC_DESCRIPTOR_SUBTYPE_COMMAND_SET_FUNCTIONAL_DESCRIPTOR 0x16
#define USB_CDC_DESCRIPTOR_SUBTYPE_COMMAND_SET_DETAIL_FUNCTIONAL_DESCRIPTOR 0x17
#define USB_CDC_DESCRIPTOR_SUBTYPE_TELEPHONE_CONTROL_MODEL_FUNCTIONAL_DESCRIPTOR 0x18
#define USB_CDC_DESCRIPTOR_SUBTYPE_OBEX_SERVICE_IDENTIFIER_FUNCTIONAL_DESCRIPTOR 0x19
#define USB_CDC_DESCRIPTOR_SUBTYPE_NCM_FUNCTIONAL_DESCRIPTOR 0x1A
//@}

/**
    @name USB CDC Call Management Capabilities Values
    @brief Section 5.2.3.2 Table 27 Call Management Functional Descriptor.
 */
//@{
#define USB_CDC_CM_CAPABILITIES_NONE 0x00
#define USB_CDC_CM_CAPABILITIES_HANDLES_CALL_MANAGEMENT 0x01
#define USB_CDC_CM_CAPABILITIES_HANDLES_CALL_MANAGEMENT_OVER_DATA_INTERFACE 0x02
//@}

/**
    @name USB CDC Abstract Control Management Capabilities Values
    @brief Section 5.2.3.3 Table 28 Call Management Functional Descriptor.
 */
//@{
#define USB_CDC_ACM_CAPABILITIES_NONE 0x00
#define USB_CDC_ACM_CAPABILITIES_COMM_FEATURE 0x01
#define USB_CDC_ACM_CAPABILITIES_LINE_STATE_CODING 0x02
#define USB_CDC_ACM_CAPABILITIES_SEND_BREAK 0x04
#define USB_CDC_ACM_CAPABILITIES_NETWORK_CONNECTION 0x08
//@}

/**
    @name USB CDC Communication Feature Selection Codes
    @brief Section 6.2.4 Table 47 Communication Feature Selection Codes
 */
//@{
#define USB_CDC_ACM_FEATURE_SELECTOR_ABSTRACT_STATE 0x01
#define USB_CDC_ACM_FEATURE_SELECTOR_COUNTRY_SETTING 0x02
//@}

/**
    @name USB CDC Network Control Model Network Capabilities Values
    @brief CDC NCM Spec Table 5-2 NCM Functional Descriptor
 */
//@{
#define USB_CDC_NCM_NETCAPABILITIES_NONE 0x00
#define USB_CDC_NCM_NETCAPABILITIES_ETHERNET_PACKETFILTER 0x01
#define USB_CDC_NCM_NETCAPABILITIES_NET_ADDRESS 0x02
#define USB_CDC_NCM_NETCAPABILITIES_ENCAPSULATED_COMMANDS 0x04
#define USB_CDC_NCM_NETCAPABILITIES_MAX_DATAGRAM_SIZE 0x08
#define USB_CDC_NCM_NETCAPABILITIES_CRC_MODE 0x10
#define USB_CDC_NCM_NETCAPABILITIES_8_BYTE_NTB_INPUT_SIZE 0x20
//@}

/**
    @name USB CDC Ethernet Statistics Bitmap
    @brief ECM Spec Table 4 Ethernet Statistics Capabilities
 */
//@{
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_NONE 0x00
/* Frames transmitted without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_OK 0x00000001
/* Frames received without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_RVC_OK 0x00000002
/* Frames not transmitted, or transmitted with errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_ERROR 0x00000004
/* Frames received with errors that are not delivered to the USB host. */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_RCV_ERROR 0x00000008
/* Frame missed, no buffers */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_RCV_NO_BUFFER 0x00000010
/* Directed bytes transmitted without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_DIRECTED_BYTES_XMIT 0x00000020
/* Directed frames transmitted without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_DIRECTED_FRAMES_XMIT 0x00000040
/* Multicast bytes transmitted without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_MULTICAST_BYTES_XMIT 0x00000080
/* Multicast frames transmitted without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_MULTICAST_FRAMES_XMIT 0x00000100
/* Broadcast bytes transmitted without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_BROADCAST_BYTES_XMIT 0x00000200
/* Broadcast frames transmitted without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_BROADCAST_FRAMES_XMIT 0x00000400
/* Directed bytes received without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_DIRECTED_BYTES_RCV 0x00000800
/* Directed frames received without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_DIRECTED_FRAMES_RCV 0x00001000
/* Multicast bytes received without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_MULTICAST_BYTES_RCV 0x00002000
/* Multicast frames received without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_MULTICAST_FRAMES_RCV 0x00004000
/* Broadcast bytes received without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_BROADCAST_BYTES_RCV 0x00008000
/* Broadcast frames received without errors */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_BROADCAST_FRAMES_RCV 0x00010000
/* Frames received with circular redundancy check (CRC) or frame check sequence (FCS) error */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_RCV_CRC_ERROR 0x00020000
/* Length of transmit queue */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_TRANSMIT_QUEUE_LENGTH 0x00040000
/* Frames received with alignment error */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_RCV_ERROR_ALIGNMENT 0x00080000
/* Frames transmitted with one collision */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_ONE_COLLISION 0x00100000
/* Frames transmitted with more than one collision */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_MORE_COLLISIONS 0x00200000
/* Frames transmitted after deferral */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_DEFERRED 0x00400000
/* Frames not transmitted due to collisions */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_MAX_COLLISIONS 0x00800000
/* Frames not received due to overrun */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_RCV_OVERRUN 0x01000000
/* Frames not transmitted due to underrun */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_UNDERRUN 0x02000000
/* Frames transmitted with heartbeat failure */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_HEARTBEAT_FAILURE 0x04000000
/* Times carrier sense signal lost during transmission */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_TIMES_CRS_LOST 0x08000000
/* Late collisions detected */
#define USB_CDC_ETHERNET_STATS_CAPABILITIES_XMIT_LATE_COLLISIONS 0x10000000
//@}

/**
    @name USB CDC Ethernet Filter Support Bitmap
    @brief wNumberMCFilters value in Ethernet Networking Functional Descriptor
 */
//@{
/* Device supports perfect multicast filtering. */
#define USB_CDC_ETHERNET_MC_FILTERS_DEVICE_SUPPORT 0x0000
/* Number of multicast filters supported on device. */
#define USB_CDC_ETHERNET_NUMBER_MC_FILTERS_DEVICE_SUPPORT_MASK 0x7fff
/* Device uses imperfect multicast address filtering */
#define USB_CDC_ETHERNET_MC_FILTERS_NO_DEVICE_SUPPORT 0x8000
//@}

/**
    @name USB CDC Network Control Model NTB type Bitmap
    @brief bmNtbFormatsSupported in NTB Parameter Structure
 */
//@{
/* Device supports 16-bit NTBs */
#define USB_CDC_NCM_NTB_FORMAT_16_BIT 0x0001
/* Device supports 32-bit NTBs */
#define USB_CDC_NCM_NTB_FORMAT_32_BIT 0x0002
//@}

/**
    @name USB CDC Network Control Model Signatures
    @brief Signature values used for NTH16, NTH32, NDP16 and NDP32.
 */
//@{
#define USB_CDC_NCM_SIGNATURE_NTH 0x484D434E
#define USB_CDC_NCM_SIGNATURE_NDP_CRC 0x314D434E
#define USB_CDC_NCM_SIGNATURE_NDP_NOCRC 0x304D434E
//@}

/**
    @name USB CDC Ethernet Packet Filters
    @brief Packet filter masks for ethernet over CDC.
 */
//@{
/// All multicast packets enumerated in the device's multicast address
/// list are forwarded up to the host. (required)
#define USB_CDC_PACKET_TYPE_MULTICAST 0x0010
/// All broadcast packets received by the networking device are forwarded
/// up to the host. (required)
#define USB_CDC_PACKET_TYPE_BROADCAST 0x0008
/// Directed packets received containing a destination address equal
/// to the MAC address of the networking device are forwarded up to
/// the host. (required)
#define USB_CDC_PACKET_TYPE_DIRECTED 0x0004
/// ALL multicast frames received by the networking device are forwarded
/// up to the host, not just the ones enumerated in the device's multicast
/// address list. (required)
#define USB_CDC_PACKET_TYPE_ALL_MULTICAST 0x0002
/// ALL frames received by the networking device are forwarded up to the
/// host. (required)
#define USB_CDC_PACKET_TYPE_PROMISCUOUS 0x0001
//@}

/**
    @name USB CDC Network Connection States
    @brief Used in the NetworkConnection notification.
 */
//@{
#define USB_CDC_NETWORK_CONNECTED 0x0001
#define USB_CDC_NETWORK_DISCONNECTED 0x0000
//@}


/* TYPES ***************************************************************************/

/**
    @typedef CDC_network_enable_cb
    @brief Callback declaration for function to enable the receiver and set the
    receiver packet filter.
    @param[in]	rx_enable Non-zero to enable receive packets on the media.
    @param[in]	rx_filter A bitmap of filters for packet types requested by the
     	 	 	 host. These are defined as USB_CDC_PACKET_TYPE_*
 **/
typedef void (*CDC_network_enable_cb)(uint8_t rx_enable, uint16_t rx_filter);

/**
    @name USB CDC Notification Structure
 */
typedef struct PACK USB_CDC_Notification
{
	unsigned char bmRequestType;
	unsigned char bNotification;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
} USB_CDC_Notification;

/**
    @name USB CDC Line Coding
    @brief Section 6.2.13 Table 50 Line Coding.
 */
typedef struct PACK USB_CDC_line_coding
{
	unsigned long dwDTERate;           // Data terminal rate, in bits per second
	unsigned char bCharFormat;         // Stop bits: 0 - 1 Stop bit; 1 - 1.5 Stop bit; 2 - 2 Stop bits
	unsigned char bParityType;         // Parity: 0 - None; 1 - Odd; 2 - Even; 3 - Mark; 4 - Space
	unsigned char bDataBits;           // Data bites (5,6,7,8 or 16)
} USB_CDC_line_coding;

/**
    @name USB CDC Control Line State
    @brief Section 6.2.14 Table 51 Control Signal Bitmap Values.
 */
//@{
typedef struct PACK USB_CDC_control_line_state
{
	unsigned short dtePresent:1;           // Data terminal equipment present
	unsigned short activateCarrier:1;      // Activate carrier control for half duplex modems
	unsigned short resv:14;
} USB_CDC_control_line_state;
//@}

/**
    @name USB CDC UART State Bitmap Structure
    @details UART State bitmap for USB_CDC_NOTIFICAITON_SERIAL_STATE notifications.
 */
typedef        struct PACK USB_CDC_UartStateBitmap
{
	unsigned short bRxCarrier:1;
	unsigned short bTxCarrier:1;
	unsigned short bBreak:1;
	unsigned short bRingSignal:1;
	unsigned short bFraming:1;
	unsigned short bParity:1;
	unsigned short bOverRun:1;
	unsigned short reserved:9;
} USB_CDC_UartStateBitmap;

/**
    @name USB CDC NBT Parameter Structure
    @details Response structure for GET_NTB_PARAMETERS request.
 */
typedef        struct PACK USB_cdc_nbt_params
{
	/// Number Size of this structure, in bytes = 1Ch.
	unsigned short wLength;
	/// Bit 0: 16-bit NTB supported (set to 1)
	/// Bit 1: 32-bit NTB supported
	/// Bits 2 – 15: reserved (reset to zero; must be ignored by host)
	unsigned short bmNtbFormatsSupported;
	/// IN NTB Maximum Size in bytes
	unsigned long dwNtbInMaxSize;
	/// Divisor used for IN NTB Datagram payload alignment
	unsigned short wNdpInDivisor;
	/// Remainder used to align input datagram payload within the NTB:
	///  (Payload Offset) mod (wNdpInDivisor) = wNdpInPayloadRemainder
	unsigned short wNdpInPayloadRemainder;
	/// NDP alignment modulus for NTBs on the IN pipe. Shall be a power
	/// of 2, and shall be at least 4.
	unsigned short wNdpInAlignment;
	/// Padding, shall be transmitted as zero by function, and ignored by host.
	unsigned short reserved1;
	/// OUT NTB Maximum Size
	unsigned long dwNtbOutMaxSize;
	/// OUT NTB Datagram alignment modulus
	unsigned short wNdpOutDivisor;
	/// Remainder used to align output datagram	payload offsets within the NTB:
	///  (Payload Offset) mod (wNdpOutDivisor) = wNdpOutPayloadRemainder
	unsigned short wNdpOutPayloadRemainder;
	/// NDP alignment modulus for use in NTBs on the OUT pipe. Shall be a
	/// power of 2, and shall be at least 4.
	unsigned short wNdpOutAlignment;
	/// Maximum number of datagrams that the host may pack into a single OUT
	/// NTB. Zero means	that the device imposes no limit.
	unsigned short wNtbOutMaxDatagrams;
} USB_cdc_nbt_params;

/**
    @name USB CDC UART Network Connection NTB Input Size Structure
    @details Structure for GetNtbInputSize and SetNtbInputSize requests.
 */
typedef        struct PACK USB_CDC_NTBInputSize
{
	/// NTB Maximum size in bytes. The host shall select a size that
	/// is at least 2048, and no larger	than the maximum size permitted
	/// by the function, according to the value given in the NTB
	/// Parameter Structure.
	unsigned long dwNtbInMaxSize;
	/// Maximum number of datagrams within the IN NTB. Zero means no limit.
	unsigned short wNtbInMaxDatagrams;
	unsigned short reserved;
} USB_CDC_NTBInputSize;

/**
    @name USB CDC UART Network Connection Bitmap Structure
    @details State bitmap for USB_CDC_NOTIFICATION_NETWORK_CONNECTION notifications.
 */
typedef        struct PACK USB_CDC_NetworkConnectionBitmap
{
	unsigned short bConnected:1;
	unsigned short reserved:15;
} USB_CDC_NetworkConnectionBitmap;

/**
    @name USB CDC UART Network Speed Change Notification Structure
    @details Structure for USB_CDC_NOTIFICAITON_CONNECTION_SPEED_CHANGE notifications.
 */
typedef        struct PACK
{
	unsigned long DLBitRate;
	unsigned long ULBitRate;
} USB_CDC_ConnectionSpeedChange;

/**
    @name USB CDC Comm Feature Abstract State Selector Structure
    @details The bitmap used in SetCommFeature and GetCommFeature requests.
 */
typedef        struct PACK USB_CDC_AbstractStateSelectorBitmap
{
	unsigned short bIdleSetting:1;
	unsigned short bDataMultiplexedState:1;
	unsigned short reserved:14;
} USB_CDC_AbstractStateSelectorBitmap;

/**
    @name CDC Class-Specific Descriptor Header Format
    @details Class specific configuration descriptor header for CDC.
 */
typedef struct PACK USB_CDC_ClassSpecificDescriptorHeaderFormat
{
	unsigned char  bFunctionLength;
	unsigned char  bDescriptorType; // USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE
	unsigned char  bDescriptorSubtype; // USB_CDC_DESCRIPTOR_SUBTYPE_HEADER_FUNCTIONAL_DESCRIPTOR
	unsigned short  bcdCDC;
} USB_CDC_ClassSpecificDescriptorHeaderFormat;

/**
    @name CDC Class-Specific Call Management Functional Descriptor
    @details Class specific configuration descriptor for CDC.
 */
typedef struct PACK USB_CDC_CallManagementFunctionalDescriptor
{
	unsigned char  bFunctionLength;
	unsigned char  bDescriptorType; // USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE
	unsigned char  bDescriptorSubtype; // USB_CDC_DESCRIPTOR_SUBTYPE_CALL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR
	unsigned char  bmCapabilities;
	unsigned char  bDataInterface;
} USB_CDC_CallManagementFunctionalDescriptor;

/**
    @name CDC Class-Specific Abstract Control Management Functional Descriptor
    @details Class specific configuration descriptor for CDC.
 */
typedef struct PACK USB_CDC_AbstractControlManagementFunctionalDescriptor
{
	unsigned char  bFunctionLength;
	unsigned char  bDescriptorType; // USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE
	unsigned char  bDescriptorSubtype; // USB_CDC_DESCRIPTOR_SUBTYPE_ABSTRACT_CONTROL_MANAGEMENT_FUNCTIONAL_DESCRIPTOR
	unsigned char  bmCapabilities;
} USB_CDC_AbstractControlManagementFunctionalDescriptor;

/**
    @name CDC Class-Specific Union Interface Functional Descriptor
    @details Class specific configuration descriptor for CDC.
 */
typedef struct PACK USB_CDC_UnionInterfaceFunctionalDescriptor
{
	unsigned char  bFunctionLength;
	unsigned char  bDescriptorType; // USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE
	unsigned char  bDescriptorSubtype; // USB_CDC_DESCRIPTOR_SUBTYPE_UNION_FUNCTIONAL_DESCRIPTOR
	unsigned char  bControlInterface;
	unsigned char  bSubordinateInterface0;
} USB_CDC_UnionInterfaceFunctionalDescriptor;


/**
    @name CDC Class-Specific Ethernet Networking Functional Descriptor
    @details Class specific configuration descriptor for CDC.
 */
typedef struct PACK USB_CDC_EthernetNetworkingFunctionalDescriptor
{
	unsigned char  bFunctionLength;
	unsigned char  bDescriptorType; // USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE
	unsigned char  bDescriptorSubtype; // USB_CDC_DESCRIPTOR_SUBTYPE_ETHERNET_NETWORKING_FUNCTIONAL_DESCRIPTOR
	unsigned char  iMACAddress; // String index of MAC address in string descriptors table.
	unsigned long  bmEthernetStatistics; // USB_CDC_ETHERNET_STATS_CAPABILITIES_*
	unsigned short wMaxSegmentSize;
	unsigned short wNumberMCFilters;
	unsigned char  bNumberPowerFilters;
} USB_CDC_EthernetNetworkingFunctionalDescriptor;

/**
    @name CDC Class-Specific Network Control Model Functional Descriptor
    @details Class specific configuration descriptor for CDC.
 */
typedef struct PACK USB_CDC_NCMFunctionalDescriptor
{
	unsigned char  bFunctionLength;
	unsigned char  bDescriptorType; // USB_CDC_DESCRIPTOR_TYPE_CS_INTERFACE
	unsigned char  bDescriptorSubtype; // USB_CDC_DESCRIPTOR_SUBTYPE_NCM_FUNCTIONAL_DESCRIPTOR
	unsigned short bcdNcmVersion;
	unsigned char  bmNetworkCapabilities;
} USB_CDC_NCMFunctionalDescriptor;

/**
    @name CDC NCM Transfer Header (16 bit)
    @details Describes a transfer of ethernet frame(s) from device to host
    or host to device. This is the NTH16 structure from Table 3-1 of the
    CDC NCM spec.
 */
typedef struct PACK USB_CDC_NCMTransferHeader16
{
	unsigned long  dwSignature; // "ncmh" USB_CDC_NCM_SIGNATURE_NTH
	unsigned short wHeaderLength;
	unsigned short wSequence;
	unsigned short wBlockLength; // Size of the NTB in bytes.
	unsigned short wNdpIndex; // Offset to first NDP in transfer.
} USB_CDC_NCMTransferHeader16;

/**
    @name CDC NCM Datagram Pointer List
    @details
 */
//@{
typedef struct PACK USB_CDC_NCMDatagramPointerEntry16 {
	unsigned short wDatagramIndex;
	unsigned short wDatagramLength;
} USB_CDC_NCMDatagramPointerEntry16;

typedef struct PACK USB_CDC_NCMDatagramPointerList16
{
	unsigned long  dwSignature; // "NCM0" USB_CDC_NCM_SIGNATURE_NDP_NOCRC or
								// "NCM1" USB_CDC_NCM_SIGNATURE_NDP_CRC
	unsigned short wLength; // Minimum 0x0010
	unsigned short wNextNdpIndex;
	struct USB_CDC_NCMDatagramPointerEntry16 list[1];
} USB_CDC_NCMDatagramPointerList16;
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USB_CDC_H_ */
