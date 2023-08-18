/**
    @file ft900_usb_rndis.h

    @brief
    USB definitions for USB device stack RNDIS on FT900.

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

#ifndef FT900_USB_RNDIS_H_
#define FT900_USB_RNDIS_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

#include <ft900_usb.h>


/**
 @brief Response definitions for RNDIS.
 */
//@{
#define RNDIS_RESPONSE_AVAILABLE 	  	  0x00000001
//@}

// The host and device use this to send network data to one another.
#define REMOTE_NDIS_PACKET_MSG 	  	  	  0x00000001
// Sent by the host to initialize the device.
#define REMOTE_NDIS_INITIALIZE_MSG 	  	  0x00000002
// Device response to an initialize message.
#define REMOTE_NDIS_INITIALIZE_CMPLT 	  0x80000002
// Sent by the host to halt the device. This does not have a response.
//It is optional for the device to send this message to the host.
#define REMOTE_NDIS_HALT_MSG 	  	  	  0x00000003
// Sent by the host to send a query OID.
#define REMOTE_NDIS_QUERY_MSG 	  	  	  0x00000004
// Device response to a query OID.
#define REMOTE_NDIS_QUERY_CMPLT 	  	  0x80000004
// Sent by the host to send a set OID.
#define REMOTE_NDIS_SET_MSG 	  	  	  0x00000005
// Device response to a set OID.
#define REMOTE_NDIS_SET_CMPLT 	  	  	  0x80000005
// Sent by the host to perform a soft reset on the device.
#define REMOTE_NDIS_RESET_MSG 	  	  	  0x00000006
// Device response to reset message.
#define REMOTE_NDIS_RESET_CMPLT 	  	  0x80000006
// Sent by the device to indicate its status or an error when
// an unrecognized message is received.
#define REMOTE_NDIS_INDICATE_STATUS_MSG   0x00000007
// During idle periods, sent every few seconds by the host to
// check that the device is still responsive. It is optional for
//the device to send this message to check if the host is active.
#define REMOTE_NDIS_KEEPALIVE_MSG 		  0x00000008
// The device response to a keepalive message. The host can respond
// with this message to a keepalive message from the device when
// the device implements the optional KeepAliveTimer.
#define REMOTE_NDIS_KEEPALIVE_CMPLT 	  0x80000008

// Type of RNDIS device.
#define RNDIS_DF_CONNECTIONLESS 		  0x00000001
#define RNDIS_DF_CONNECTION_ORIENTED 	  0x00000002

// Only supported medium for RNDIS.
#define RNDIS_MEDIUM_802_3 				  0x00000000

// Current version of RNDIS spec for the device.
#define RNDIS_VERSION_MAJOR 			  0x00000001
#define RNDIS_VERSION_MINOR 			  0x00000000

/* Speeds OID_GEN_LINK_SPEED */
#define NDIS_LINK_SPEED_10MBPS			  (10000000/100)
#define NDIS_LINK_SPEED_100MBPS			  (100000000/100)

/* Connection state. OID_GEN_HARDWARE_STATUS */
#define NDIS_MEDIA_STATE_CONNECTED		  0x00000000
#define NDIS_MEDIA_STATE_DISCONNECTED	  0x00000001

/* Power states. OID_PNP_SET_POWER OID_PNP_QUERY_POWER */
#define NDIS_DEVICE_POWER_STATE_D0 		  1
#define NDIS_DEVICE_POWER_STATE_D1 		  2
#define NDIS_DEVICE_POWER_STATE_D2 		  3
#define NDIS_DEVICE_POWER_STATE_D3 		  4

/* Medium OID_GEN_MEDIA_SUPPORTED */
#define NDIS_MEDIUM_802_3				  0x00000000

/* Packet Types. OID_GEN_CURRENT_PACKET_FILTER */
#define NDIS_PACKET_TYPE_DIRECTED		  0x00000001
#define NDIS_PACKET_TYPE_MULTICAST		  0x00000002
#define NDIS_PACKET_TYPE_ALL_MULTICAST	  0x00000004
#define NDIS_PACKET_TYPE_BROADCAST		  0x00000008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING	  0x00000010
#define NDIS_PACKET_TYPE_PROMISCUOUS	  0x00000020
#define NDIS_PACKET_TYPE_SMT			  0x00000040
#define NDIS_PACKET_TYPE_ALL_LOCAL		  0x00000080
#define NDIS_PACKET_TYPE_GROUP			  0x00000100
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL	  0x00000200
#define NDIS_PACKET_TYPE_FUNCTIONAL		  0x00000400
#define NDIS_PACKET_TYPE_MAC_FRAME		  0x00000800

/* MAC options. OID_GEN_MAC_OPTIONS */
#define NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA             0x00000001
#define NDIS_MAC_OPTION_RECEIVE_SERIALIZED              0x00000002
#define NDIS_MAC_OPTION_TRANSFERS_NOT_PEND              0x00000004
#define NDIS_MAC_OPTION_NO_LOOPBACK                     0x00000008
#define NDIS_MAC_OPTION_FULL_DUPLEX                     0x00000010
#define NDIS_MAC_OPTION_EOTX_INDICATION                 0x00000020
#define NDIS_MAC_OPTION_8021P_PRIORITY                  0x00000040
#define NDIS_MAC_OPTION_SUPPORTS_MAC_ADDRESS_OVERWRITE  0x00000080
#define NDIS_MAC_OPTION_RECEIVE_AT_DPC                  0x00000100
#define NDIS_MAC_OPTION_8021Q_VLAN                      0x00000200
#define NDIS_MAC_OPTION_RESERVED                        0x80000000
// Success
#define RNDIS_STATUS_SUCCESS 			  0x00000000
// Unspecified error
#define RNDIS_STATUS_FAILURE 			  0xC0000001
// Invalid data error
#define RNDIS_STATUS_INVALID_DATA 		  0xC0010015
// Unsupported request error
#define RNDIS_STATUS_NOT_SUPPORTED 		  0xC00000BB
// Device is connected to a network medium.
#define RNDIS_STATUS_MEDIA_CONNECT 		  0x4001000B
// Device is disconnected from the medium.
#define RNDIS_STATUS_MEDIA_DISCONNECT 	  0x4001000C
// Indicate a speed change of the medium.
#define RNDIS_STATUS_LINK_SPEED_CHANGE 	  0x40010013

/* Required General Object IDs (OIDs) */
#define OID_GEN_SUPPORTED_LIST            0x00010101
#define OID_GEN_HARDWARE_STATUS           0x00010102
#define OID_GEN_MEDIA_SUPPORTED           0x00010103
#define OID_GEN_MEDIA_IN_USE              0x00010104
#define OID_GEN_MAXIMUM_FRAME_SIZE        0x00010106
#define OID_GEN_LINK_SPEED                0x00010107
#define OID_GEN_TRANSMIT_BLOCK_SIZE       0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE        0x0001010B
#define OID_GEN_VENDOR_ID                 0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION        0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER     0x0001010E
#define OID_GEN_MAXIMUM_TOTAL_SIZE        0x00010111
#define OID_GEN_MEDIA_CONNECT_STATUS      0x00010114
#define OID_GEN_VENDOR_DRIVER_VERSION     0x00010116
/* Optional General OIDs */
#define OID_GEN_MAC_OPTIONS               0x00010113
#define OID_GEN_PHYSICAL_MEDIUM           0x00010202
#define OID_GEN_RNDIS_CONFIG_PARAMETER    0x0001021B

/* Required statistics OIDs */
#define OID_GEN_XMIT_OK                   0x00020101
#define OID_GEN_RCV_OK                    0x00020102
#define OID_GEN_XMIT_ERROR                0x00020103
#define OID_GEN_RCV_ERROR                 0x00020104
#define OID_GEN_RCV_NO_BUFFER             0x00020105
/* Optional statistics OIDs */
#define OID_GEN_DIRECTED_BYTES_XMIT       0x00020201
#define OID_GEN_DIRECTED_FRAMES_XMIT      0x00020202
#define OID_GEN_MULTICAST_BYTES_XMIT      0x00020203
#define OID_GEN_MULTICAST_FRAMES_XMIT     0x00020204
#define OID_GEN_BROADCAST_BYTES_XMIT      0x00020205
#define OID_GEN_BROADCAST_FRAMES_XMIT     0x00020206
#define OID_GEN_DIRECTED_BYTES_RCV        0x00020207
#define OID_GEN_DIRECTED_FRAMES_RCV       0x00020208
#define OID_GEN_MULTICAST_BYTES_RCV       0x00020209
#define OID_GEN_MULTICAST_FRAMES_RCV      0x0002020A
#define OID_GEN_BROADCAST_BYTES_RCV       0x0002020B
#define OID_GEN_BROADCAST_FRAMES_RCV      0x0002020C
#define OID_GEN_RCV_CRC_ERROR             0x0002020D
#define OID_GEN_TRANSMIT_QUEUE_LENGTH     0x0002020E

/* Required IEEE 802.3 (Ethernet) OIDs */
#define OID_802_3_PERMANENT_ADDRESS       0x01010101
#define OID_802_3_CURRENT_ADDRESS         0x01010102
#define OID_802_3_MULTICAST_LIST          0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE       0x01010104
/* Optional IEEE 802.3 (Ethernet) OIDs */
#define OID_802_3_MAC_OPTIONS             0x01010105

/* Optional IEEE 802.3 (Ethernet) statistics OIDs */
#define OID_802_3_XMIT_DEFERRED           0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS     0x01020202
#define OID_802_3_RCV_OVERRUN             0x01020203
#define OID_802_3_XMIT_UNDERRUN           0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE  0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST     0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS    0x01020207

/* Optional Power Management OIDs */
#define OID_PNP_CAPABILITIES			  0xFD010100
#define OID_PNP_SET_POWER			      0xFD010101
#define OID_PNP_QUERY_POWER			      0xFD010102

/* Optional Network Wake Up OIDs */
#define OID_PNP_ADD_WAKE_UP_PATTERN		  0xFD010103
#define OID_PNP_REMOVE_WAKE_UP_PATTERN    0xFD010104
#define OID_PNP_ENABLE_WAKE_UP			  0xFD010106

struct rndis_msg_header {
	// Type of message being sent.
	uint32_t MessageType;
	// Total length of this message in bytes.
	uint32_t MessageLength;
	// Remote NDIS message ID value.
	uint32_t RequestId;
};

struct rndis_initialise_msg {
	//Type of message being sent.
	uint32_t MessageType; // 0x00000002
	// Total length of this message in bytes.
	uint32_t MessageLength;
	// Remote NDIS message ID value.
	uint32_t RequestId;
	// Host Remote NDIS protocol version - expect 1.
	uint32_t MajorVersion;
	// Host Remote NDIS protocol version - expect 0.
	uint32_t MinorVersion;
	// The maximum size in bytes of any single bus data transfer
	// that the host expects to receive from the device.
	uint32_t MaxTransferSize;
};

struct rndis_initialise_cmplt {
	//Type of message being sent.
	uint32_t MessageType; // 0x80000002
	// Total length of this message in bytes.
	uint32_t MessageLength;
	// Remote NDIS message ID value.
	uint32_t RequestId;
	// RNDIS_STATUS_SUCCESS if the device initialised successfully;
	// otherwise, it specifies an error code that indicates the failure.
	uint32_t Status;
	// Device Remote NDIS protocol version - 1.
	uint32_t MajorVersion;
	// Device Remote NDIS protocol version - 0.
	uint32_t MinorVersion;
	// Device is connectionless or connection-oriented.
	uint32_t DeviceFlags;
	// Set to RNDIS_MEDIUM_802_3 (0x00000000)
	uint32_t Medium;
	// One message per transfer only please.
	uint32_t MaxPacketsPerMessage;
	//The maximum size in bytes of any single bus data transfer that
	// the device is able to send to the host.
	uint32_t MaxTransferSize;
	// Packet alignment factor as a power of 2.
	uint32_t PacketAlignmentFactor;
	// Reserved for connection-oriented devices. Set value to zero.
	uint32_t AFListOffset;
	//	Reserved for connection-oriented devices. Set value to zero.
	uint32_t AFListSize;
};

struct rndis_halt_msg {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	RequestID;
};

struct rndis_query_msg {
	//Type of message being sent.
	uint32_t MessageType; // 0x00000004
	// Total length of this message in bytes.
	uint32_t MessageLength;
	// Remote NDIS message ID value.
	uint32_t RequestId;
	// Specifies the NDIS OID that identifies the parameter being queried.
	uint32_t Oid;
	//Specifies in bytes the length of the input data for the query. Set
	// to zero when there is no OID input buffer.
	uint32_t InformationBufferLength;
	// Byte offset, from the beginning of the RequestId field, at which
	// input data for the query is located. Set to zero if there is no OID
	// input buffer.
	uint32_t InformationBufferOffset;
	// Reserved for connection-oriented devices. Set to zero.
	uint32_t DeviceVcHandle;
};

struct rndis_query_cmplt {
	//Type of message being sent.
	uint32_t MessageType; // 0x80000004
	// Total length of this message in bytes.
	uint32_t MessageLength;
	// Remote NDIS message ID value.
	uint32_t RequestId;
	// RNDIS_STATUS_SUCCESS if the device initialised successfully;
	// otherwise, it specifies an error code that indicates the failure.
	uint32_t Status;
	// The length in bytes of the response data for the query. Set to
	// zero when there is no OID result buffer.
	uint32_t InformationBufferLength;
	// Byte offset, from the beginning of the RequestId field, at which
	// response data for the query is located. Set to zero if there is no
	// response data.
	uint32_t InformationBufferOffset;
};

struct rndis_set_msg {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	RequestID;
	uint32_t	OID;
	uint32_t	InformationBufferLength;
	uint32_t	InformationBufferOffset;
	uint32_t	DeviceVcHandle;
};

struct rndis_set_cmplt {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	RequestID;
	uint32_t	Status;
};

struct rndis_reset_msg {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	Reserved;
};

struct rndis_reset_cmplt {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	Status;
	uint32_t	AddressingReset;
};

struct rndis_indicate_status_msg {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	Status;
	// Length of the status data in bytes.
	uint32_t	StatusBufferLength;
	// Offset to the start of the status data in bytes from the
	// beginning of the Status field.
	uint32_t	StatusBufferOffset;
};

struct rndis_keepalive_msg {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	RequestID;
};

struct rndis_keepalive_cmplt {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	RequestID;
	uint32_t	Status;
};

struct rndis_packet_msg {
	uint32_t	MessageType;
	uint32_t	MessageLength;
	uint32_t	DataOffset;
	uint32_t	DataLength;
	uint32_t	OOBDataOffset;
	uint32_t	OOBDataLength;
	uint32_t	NumOOBDataElements;
	uint32_t	PerPacketInfoOffset;
	uint32_t	PerPacketInfoLength;
	uint32_t	VcHandle;
	uint32_t	Reserved;
};

struct rndis_config_parameter {
	uint32_t	ParameterNameOffset;
	uint32_t	ParameterNameLength;
	uint32_t	ParameterType;
	uint32_t	ParameterValueOffset;
	uint32_t	ParameterValueLength;
};

union rndis_msg {
	uint8_t *raw;
	struct rndis_msg_header header ;
	struct rndis_initialise_msg initialise;
	struct rndis_initialise_cmplt initialise_cmplt;
	struct rndis_halt_msg halt;
	struct rndis_query_msg query;
	struct rndis_query_cmplt query_cmplt;
	struct rndis_set_msg set;
	struct rndis_set_cmplt set_cmplt;
	struct rndis_reset_msg reset;
	struct rndis_reset_cmplt reset_cmplt;
	struct rndis_indicate_status_msg indicate_status;
	struct rndis_keepalive_msg keepalive;
	struct rndis_keepalive_cmplt keepalive_cmplt;
	struct rndis_packet_msg packet;
	struct rndis_config_parameter config;
};

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USB_RNDIS_H_ */
