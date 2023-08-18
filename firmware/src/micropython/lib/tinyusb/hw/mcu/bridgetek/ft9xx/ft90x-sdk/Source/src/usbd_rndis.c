/**
    @file usbd_rndis.c

    @brief
    API for USB device stack RNDIS on FT900.

    API functions for USB device RNDIS stack. These functions provide all
    the functionality required to implement a USB Device RNDIS interface.
    Only one instance of the USB device RNDIS driver may exist.

    @version 1.0.0

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
#include <stdlib.h>

/* INCLUDES ************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include <ft900_gpio.h>
#include <ft900_sys.h>
#include <ft900_usb.h>
#include <ft900_usb_rndis.h>
#include <ft900_usbd_rndis.h>
#include <ft900_eth.h>
#include <ft900_usbd.h>
#include <ft900_memctl.h>

#include <ft900_delay.h>
#include <ft900_interrupt.h>
#include <registers/ft900_registers.h>
#include <registers/ft900_usbd_registers.h>

/* CONSTANTS ***********************************************************************/

// Encapsulated command handling.
#define RNDIS_ENCAPSULATED_COMMAND_LEN 512

/* Supported OIDs */
static const uint32_t oid_supported_list[] = {
		// Mandatory information.
		OID_GEN_SUPPORTED_LIST, // Query
		OID_GEN_HARDWARE_STATUS, // Query
		OID_GEN_MEDIA_SUPPORTED, // Query
		OID_GEN_MEDIA_IN_USE, // Query
		OID_GEN_MAXIMUM_FRAME_SIZE, // Query
		OID_GEN_LINK_SPEED, // Query
		OID_GEN_TRANSMIT_BLOCK_SIZE, // Query
		OID_GEN_RECEIVE_BLOCK_SIZE, // Query
		OID_GEN_VENDOR_ID, // Query
		OID_GEN_VENDOR_DESCRIPTION, // Query
		OID_GEN_VENDOR_DRIVER_VERSION, // Query
		OID_GEN_CURRENT_PACKET_FILTER, // Query and Set
		OID_GEN_MAXIMUM_TOTAL_SIZE, // Query
		OID_GEN_MEDIA_CONNECT_STATUS, // Query
		OID_GEN_PHYSICAL_MEDIUM, // Query
		// Mandatory stats.
		OID_GEN_XMIT_OK, // Query
		OID_GEN_RCV_OK, // Query
		OID_GEN_XMIT_ERROR, // Query
 		OID_GEN_RCV_ERROR, // Query
		OID_GEN_RCV_NO_BUFFER, // Query
		// 802.3 queries.
		OID_802_3_CURRENT_ADDRESS, // Query
		OID_802_3_PERMANENT_ADDRESS, // Query
		OID_802_3_MULTICAST_LIST, // Query and Set
		OID_802_3_MAC_OPTIONS, // Query
		OID_802_3_MAXIMUM_LIST_SIZE, // Query
		// Power Management.
		OID_PNP_CAPABILITIES, // Query
		OID_PNP_SET_POWER, // Set
		OID_PNP_QUERY_POWER, // Query
};

/// Calculate the offset of the RNDIS packet message header.
#define RNDIS_PACKET_MSG_OFFSET 2
/// Calculate the offset of the ethernet packet data.
#define RNDIS_ETHERNET_PACKET_OFFSET (sizeof(struct rndis_packet_msg))

/**
	@name    USB Macros to enable/disable debugging output of the driver
	@details These macros will enable debug output to a linked printf function.
 */
//@{
// Debugging only
#undef RNDIS_LIBRARY_DEBUG_ENABLED
#ifdef RNDIS_LIBRARY_DEBUG_ENABLED
#undef RNDIS_COMMAND_DEBUG
#define RNDIS_OID_DEBUG
#define RNDIS_STATE_DEBUG
#undef RNDIS_PACKET_DEBUG
#define RNDIS_ERROR_DEBUG
#include <ft900_uart_simple.h>
#include "../tinyprintf/tinyprintf.h"
#endif // RNDIS_LIBRARY_DEBUG_ENABLED
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/** @name	RNDIS buffers for encapsulated commands, responses and notifications.
 * @details These buffers and lengths implement a single command/response pair for
 * 			encapsulated commands received on the RNDIS control endpoint. The
 * 			commands are processed when the buffer length is non-zero and an
 * 			appropriate response is placed in the response buffer. The host will
 * 			read the response again via the control endpoint.
 * 			A connection change response is used to signal that the media state
 * 			has changed.
 * 			The notification available flag signals that at least one notification is
 * 			waiting for the host to read. It is sent to the interrupt IN endpoint.
 * 			Command responses are always sent before connection change responses
 * 			and only one response is sent per notification.
 */
//{@
/* @brief Encapsulated command buffer.
 * @details The incoming command is stored here.
 */
static uint8_t rndis_command[RNDIS_ENCAPSULATED_COMMAND_LEN];
/* @brief Encapsulated command buffer length.
 * @details The length of the command in the command buffer. If it is zero then
 * there is no current command to process.
 */
static volatile uint16_t rndis_commandLen;
/* @brief Encapsulated response buffer.
 * @details The outgoing response is stored here.
 */
static uint8_t rndis_response[RNDIS_ENCAPSULATED_COMMAND_LEN];
/* @brief Encapsulated response buffer length.
 * @details The length of the response in the response buffer. If it is zero then
 * there is no current response to return to the host.
 */
static volatile uint16_t rndis_responseLen;
/* @brief Connection change status buffer.
 * @details A response packet containing a status indicating the type of
 * connection change is built to be sent to the host.
 */
static uint8_t rndis_connection[sizeof(struct rndis_indicate_status_msg) + 4];
static uint8_t rndis_speed_change[sizeof(struct rndis_indicate_status_msg) + 4];
/* @brief Connection change status buffer length.
 * @details The length of the reponse in the connection change status buffer.
 * If it is zero then there is no current response to return to the host.
 */
static volatile uint16_t rndis_connectionLen;
static volatile uint16_t rndis_speed_changeLen;
/* @brief Notification available flag.
 * @details If this is non-zero then the driver will send a notification packet
 * to the host at the next opportunity. There may be multiple responses to send,
 * e.g. an encapsulated response and a connection change status at the same
 * time. The encapsulated response will be sent first and the connection change
 * at the next available opportunity.
 */
static volatile uint8_t rndis_notificationAvailableResp;
static volatile uint8_t rndis_notificationAvailableConnection;
static volatile uint8_t rndis_notificationAvailableChange;
//@}

/** @name	RNDIS device information.
 * @details Pointers to information that RNDIS requires to provide information
 * about the device to the host. The MAC, vendor ID and vendor description are
 * requested by the host driver via GET OIDs. The MAC is required by host
 * software to allow the host to format ethernet packets properly.
 * The vender ID and description are informational only.
 */
//@{
/** @brief The MAC address of the device being implemented.
 */
static uint8_t const * rndis_current_mac;
/** @brief The vendor ID of the device being implemented.
 * @details These are IEEE asigned codes and are usually the same as the
 * first 3 btyes of the MAC address. Vendors without an IEEE-registered code
 * should use the value 0xFFFFFF.
 */
static uint8_t const * rndis_vendor_id;
/** @brief A vendor description to report to host software.
 */
static char const * rndis_vendor_description;
/** @brief The version of RNDIS supported in this implementation.
 */
static const uint32_t rndis_driver_version = 1;
/** @brief The maximum size of an encapsulated command supported by this
 * implementation.
 * @details The host software may reduce this size if it requires to but
 * this is the maximum length of an encapsulated command or response that
 * can be accomodated in this driver.
 */
static uint16_t rndis_host_transfer_max = RNDIS_ENCAPSULATED_COMMAND_LEN;
//@}

/** @name RNDIS device state.
 * @brief The initialisation state of this device.
 * @details It will start in the uninitialised state (RNDIS_STATE_UNINITIALIZED)
 * and move to the initialised state (RNDIS_STATE_INITIALIZED) after a
 * REMOTE_NDIS_INITIALIZE_MSG message has been replied to. It stays here until
 * the host software activates a valid packet filter when it moves to the
 * RNDIS_STATE_DATA_INITIALIZED state.
 * Disabling the packet filter will return it to the RNDIS_STATE_INITIALIZED
 * state.
 */
static uint8_t rndis_state = RNDIS_STATE_UNINITIALIZED;

/** @name RNDIS network configuration.
 * @brief Current state of the RNDIS network interface.
 */
//@{
static uint32_t rndis_mtu_size = 0;
static uint32_t rndis_max_total_size = 0;
static uint32_t rndis_connected = NDIS_MEDIA_STATE_DISCONNECTED;
static uint32_t rndis_link_speed = NDIS_LINK_SPEED_100MBPS;
static uint32_t rndis_packet_filter = 0; // NDIS_PACKET_TYPE_*
static uint32_t rndis_saved_packet_filter; // Copy of packet filter when in suspend mode.
//@}

/** @name RNDIS receive enable callback.
 * @brief Notify the application to change the receive enable and packet filter
 *		for the network medium.
 */
RNDIS_rx_enable_cb rndis_rx_enable_cb;

/** @name RNDIS USB endpoints.
 * @brief Endpoint handles for the notification interrupt IN endpoint, data
 * interface bulk IN and OUT endpoints.
 */
//@{
/// Notification endpoint (interrupt IN)
USBD_ENDPOINT_NUMBER	rndis_ep_not;
/// Data interface transmit endpoint (bulk OUT)
USBD_ENDPOINT_NUMBER	rndis_ep_out;
/// Data interface receive endpoint (bulk IN)
USBD_ENDPOINT_NUMBER	rndis_ep_in;
//@}

/** @name RNDIS statistics.
 * @brief Counters used to provide packet counting to the host software.
 */
struct rndis_stats {
	volatile uint32_t tx_packets;
	volatile uint32_t rx_packets;
	volatile uint32_t tx_errors;
	volatile uint32_t rx_errors;
	volatile uint32_t tx_dropped;
	volatile uint32_t rx_dropped;
} rndis_stats = {0, 0, 0, 0, 0, 0,};

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

// RNDIS functions.
static uint32_t rndis_query_handler(uint32_t OID, uint8_t *set, uint32_t setLen, struct rndis_query_cmplt *resp);
static uint32_t rndis_set_handler(uint32_t OID, uint8_t *set, uint32_t setLen, struct rndis_set_cmplt *resp);
static void rndis_handle_command();
static void rndis_notification();

static void rndis_send_status(uint32_t status)
{
	struct rndis_indicate_status_msg *resp = (struct rndis_indicate_status_msg *)rndis_connection;

	resp->MessageType = REMOTE_NDIS_INDICATE_STATUS_MSG;
	resp->MessageLength = sizeof(struct rndis_indicate_status_msg);
	resp->StatusBufferLength = 0;
	resp->StatusBufferOffset = 0;
	resp->Status = status;

	rndis_connectionLen = resp->MessageLength;
	rndis_notificationAvailableConnection = 1;
}

static void rndis_send_speed_change()
{
	struct rndis_indicate_status_msg *resp = (struct rndis_indicate_status_msg *)rndis_speed_change;
	uint32_t *pPayload = (uint32_t *)&rndis_speed_change[sizeof(struct rndis_indicate_status_msg)];

	resp->MessageType = REMOTE_NDIS_INDICATE_STATUS_MSG;
	resp->MessageLength = sizeof(struct rndis_indicate_status_msg) + 4;
	resp->StatusBufferLength = 0x04;
	resp->StatusBufferOffset = 0x0c;
	resp->Status = RNDIS_STATUS_LINK_SPEED_CHANGE;

	*pPayload = rndis_link_speed;

	rndis_speed_changeLen = resp->MessageLength;
	rndis_notificationAvailableChange = 1;
}

static void rndis_handle_command()
{
	union rndis_msg *msg = (union rndis_msg *)rndis_command;
	union rndis_msg *resp = (union rndis_msg *)rndis_response;

#ifdef RNDIS_COMMAND_DEBUG
	tfp_printf("CMD: ");
	for (int i = 0; i < rndis_commandLen; i++)
		tfp_printf("%02x%s", rndis_command[((i/4)*4)+3-(i%4)], i%4==3?" ":"");
	tfp_printf("\r\n");
#endif // RNDIS_COMMAND_DEBUG

	if (msg->header.MessageLength != rndis_commandLen)
	{
		// The one packet per transfer is being ignored?
	}
	// Stop us coming in here again.
	rndis_commandLen = 0;

	// Return request ID of processed command in the response message.
	resp->header.RequestId = msg->header.RequestId;

	switch (msg->header.MessageType)
	{
	case REMOTE_NDIS_INITIALIZE_MSG:
#ifdef RNDIS_COMMAND_DEBUG
		tfp_printf("REMOTE_NDIS_INITIALIZE_MSG\r\n");
#endif // RNDIS_COMMAND_DEBUG

		resp->initialise_cmplt.Status = RNDIS_STATUS_SUCCESS;
		resp->initialise_cmplt.MessageType = REMOTE_NDIS_INITIALIZE_CMPLT;
		resp->initialise_cmplt.MessageLength = sizeof(struct rndis_initialise_cmplt);

		// Check the size of the incoming message.
		if (msg->initialise.MessageLength != sizeof(struct rndis_initialise_msg))
		{
			resp->initialise_cmplt.Status = RNDIS_STATUS_FAILURE;
		}
		// Update the host's maximum receive buffer length if it is smaller
		// than the buffer we have created for outgoing messages.
		if (msg->initialise.MaxTransferSize < rndis_host_transfer_max)
		{
			rndis_host_transfer_max = msg->initialise.MaxTransferSize;
		}

		// Set the interface to be intialised.
		rndis_state = RNDIS_STATE_INITIALIZED;

		// Fill in initialise results.
		resp->initialise_cmplt.MajorVersion = RNDIS_VERSION_MAJOR;
		resp->initialise_cmplt.MinorVersion = RNDIS_VERSION_MINOR;
		resp->initialise_cmplt.DeviceFlags = RNDIS_DF_CONNECTIONLESS;
		resp->initialise_cmplt.Medium = RNDIS_MEDIUM_802_3;
		resp->initialise_cmplt.MaxPacketsPerMessage = 1;
		resp->initialise_cmplt.MaxTransferSize = rndis_max_total_size;
		resp->initialise_cmplt.PacketAlignmentFactor = 8;
		resp->initialise_cmplt.AFListOffset = 0;
		resp->initialise_cmplt.AFListSize = 0;

		// Tell main loop to transmit response message.
		rndis_responseLen = resp->initialise_cmplt.MessageLength;
		rndis_notificationAvailableResp = 1;
		break;

	case REMOTE_NDIS_HALT_MSG:
#ifdef RNDIS_COMMAND_DEBUG
		tfp_printf("REMOTE_NDIS_HALT_MSG\r\n");
#endif // RNDIS_COMMAND_DEBUG

		// Set the interface to be uninitalised.
		rndis_state = RNDIS_STATE_UNINITIALIZED;

		// Tell main loop to transmit response message.
		rndis_responseLen = 0;
		rndis_notificationAvailableResp = 0;
		break;

	case REMOTE_NDIS_QUERY_MSG:
#ifdef RNDIS_COMMAND_DEBUG
		tfp_printf("REMOTE_NDIS_QUERY_MSG\r\n");
#endif // RNDIS_COMMAND_DEBUG

		resp->query_cmplt.Status = RNDIS_STATUS_SUCCESS;
		resp->query_cmplt.MessageType = REMOTE_NDIS_QUERY_CMPLT;
		resp->query_cmplt.MessageLength = sizeof(struct rndis_query_cmplt);

		// Check the size of the incoming message.
		if (msg->query.MessageLength < sizeof(struct rndis_query_msg))
		{
			resp->query_cmplt.Status = RNDIS_STATUS_FAILURE;
		}
		else
		{
			rndis_query_handler(msg->query.Oid,
					((uint8_t *)msg) + 8 + msg->query.InformationBufferOffset,
					msg->query.InformationBufferLength,
					(struct rndis_query_cmplt *)resp);
		}

		rndis_responseLen = resp->query_cmplt.MessageLength;

		// Tell main loop to transmit response message.
		rndis_notificationAvailableResp = 1;
		break;

	case REMOTE_NDIS_SET_MSG:
#ifdef RNDIS_COMMAND_DEBUG
		tfp_printf("REMOTE_NDIS_SET_MSG ");
#endif // RNDIS_COMMAND_DEBUG

		resp->set_cmplt.Status = RNDIS_STATUS_SUCCESS;
		resp->set_cmplt.MessageType = REMOTE_NDIS_SET_CMPLT;
		resp->set_cmplt.MessageLength = sizeof(struct rndis_set_cmplt);

		// Check the size of the incoming message.
		if (msg->set.MessageLength < sizeof(struct rndis_set_msg))
		{
#ifdef RNDIS_COMMAND_DEBUG
			tfp_printf("Failed\r\n");
#endif // RNDIS_COMMAND_DEBUG
			resp->set_cmplt.Status = RNDIS_STATUS_FAILURE;
		}
		else
		{
#ifdef RNDIS_COMMAND_DEBUG
			tfp_printf("Handling\r\n");
#endif // RNDIS_COMMAND_DEBUG
			rndis_set_handler(msg->query.Oid,
					((uint8_t *)msg) + 8 + msg->set.InformationBufferOffset,
					msg->set.InformationBufferLength,
					(struct rndis_set_cmplt *)resp);
		}

		rndis_responseLen = resp->set_cmplt.MessageLength;

		// Tell main loop to transmit response message.
		rndis_notificationAvailableResp = 1;
		break;

	case REMOTE_NDIS_RESET_MSG:
#ifdef RNDIS_COMMAND_DEBUG
		tfp_printf("REMOTE_NDIS_RESET_MSG ");
#endif // RNDIS_COMMAND_DEBUG

		resp->reset_cmplt.Status = RNDIS_STATUS_SUCCESS;
		resp->reset_cmplt.MessageType = REMOTE_NDIS_RESET_CMPLT;
		resp->reset_cmplt.MessageLength = sizeof(struct rndis_reset_cmplt);

		// Check the size of the incoming message.
		if (msg->reset.MessageLength < sizeof(struct rndis_reset_msg))
		{
#ifdef RNDIS_COMMAND_DEBUG
			tfp_printf("Failed\r\n");
#endif // RNDIS_COMMAND_DEBUG
			resp->reset_cmplt.Status = RNDIS_STATUS_FAILURE;
			resp->reset_cmplt.AddressingReset = 0;
		}
		else
		{
#ifdef RNDIS_COMMAND_DEBUG
		tfp_printf("Addressing Reset\r\n");
#endif // RNDIS_COMMAND_DEBUG
			resp->reset_cmplt.AddressingReset = 1;
		}

		rndis_responseLen = resp->reset_cmplt.MessageLength;

		// Tell main loop to transmit response message.
		rndis_notificationAvailableResp = 1;
		break;

	case REMOTE_NDIS_KEEPALIVE_MSG:
#ifdef RNDIS_COMMAND_DEBUG
		tfp_printf("REMOTE_NDIS_KEEPALIVE_MSG\r\n");
#endif // RNDIS_COMMAND_DEBUG

		resp->keepalive_cmplt.Status = RNDIS_STATUS_SUCCESS;
		resp->keepalive_cmplt.MessageType = REMOTE_NDIS_KEEPALIVE_CMPLT;
		resp->keepalive_cmplt.MessageLength = 0x10;

		// Check the size of the incoming message.
		if (msg->reset.MessageLength < sizeof(struct rndis_keepalive_msg))
		{
			resp->keepalive_cmplt.Status = RNDIS_STATUS_FAILURE;
		}
		else
		{
		}

		rndis_responseLen = resp->keepalive_cmplt.MessageLength;

		// Tell main loop to transmit response message.
		rndis_notificationAvailableResp = 1;
		break;

	default:
#ifdef RNDIS_COMMAND_DEBUG
		tfp_printf("REMOTE_NDIS_MSG UNKNOWN!\r\n");
#endif // RNDIS_COMMAND_DEBUG
		break;
	}

#ifdef RNDIS_COMMAND_DEBUG
	tfp_printf("RESP: ");
	for (int i = 0; i < rndis_responseLen; i++)
		tfp_printf("%02x%s", rndis_response[((i/4)*4)+3-(i%4)], i%4==3?" ":"");
	tfp_printf("\r\n");
	#endif // RNDIS_COMMAND_DEBUG
}

static uint32_t rndis_query_handler(uint32_t OID, uint8_t *query, uint32_t queryLen, struct rndis_query_cmplt *resp)
{
	uint32_t status = RNDIS_STATUS_NOT_SUPPORTED;
	uint32_t bufferLen = 0;
	uint32_t *pBuf32 = (uint32_t *)&resp[1];
	uint8_t *pBuf8 = (uint8_t *)&resp[1];
	unsigned int i;

	// Not currently used.
	(void)query;
	(void)queryLen;

	switch (OID)
	{
	case OID_GEN_SUPPORTED_LIST:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_SUPPORTED_LIST\r\n");
#endif // RNDIS_OID_DEBUG
		// oid_supported_list is const and only initialised with uint32_t
		for (i = 0; i < (sizeof(oid_supported_list)/sizeof(uint32_t)); i++)
		{
			*pBuf32 = oid_supported_list[i];
			pBuf32++;
			bufferLen += sizeof(uint32_t);
		}
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_HARDWARE_STATUS:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_HARDWARE_STATUS\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32 = 0;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_PHYSICAL_MEDIUM:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_PHYSICAL_MEDIUM\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32 = NDIS_MEDIUM_802_3;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_MEDIA_IN_USE:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_MEDIA_IN_USE\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32 = NDIS_MEDIUM_802_3;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_MAXIMUM_FRAME_SIZE:
	case OID_GEN_TRANSMIT_BLOCK_SIZE:
	case OID_GEN_RECEIVE_BLOCK_SIZE:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_MAXIMUM_FRAME/TRANSMIT/RECEIVE_SIZE %d\r\n", rndis_mtu_size);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_mtu_size;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_LINK_SPEED:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_LINK_SPEED %d\r\n", rndis_link_speed);
#endif // RNDIS_OID_DEBUG
		//if (rndis_connected == NDIS_MEDIA_STATE_CONNECTED)
		//{
			*pBuf32 = rndis_link_speed;
		//}
		//else
		//{
			//*pBuf32 = 0;
		//}
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_MEDIA_CONNECT_STATUS:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_MEDIA_CONNECT_STATUS %x\r\n", rndis_connected);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_connected;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;


	case OID_GEN_CURRENT_PACKET_FILTER:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_CURRENT_PACKET_FILTER %x\r\n", rndis_packet_filter);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_packet_filter;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_802_3_MAXIMUM_LIST_SIZE:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_802_3_MAXIMUM_LIST_SIZE\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32 = 1;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_802_3_CURRENT_ADDRESS:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_802_3_CURRENT_ADDRESS\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf8++ = rndis_current_mac[0];
		*pBuf8++ = rndis_current_mac[1];
		*pBuf8++ = rndis_current_mac[2];
		*pBuf8++ = rndis_current_mac[3];
		*pBuf8++ = rndis_current_mac[4];
		*pBuf8++ = rndis_current_mac[5];
		bufferLen += 6;
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_802_3_PERMANENT_ADDRESS:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_802_3_PERMANENT_ADDRESS\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf8++ = rndis_current_mac[0];
		*pBuf8++ = rndis_current_mac[1];
		*pBuf8++ = rndis_current_mac[2];
		*pBuf8++ = rndis_current_mac[3];
		*pBuf8++ = rndis_current_mac[4];
		*pBuf8++ = rndis_current_mac[5];
		bufferLen += 6;
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_MAXIMUM_TOTAL_SIZE:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_MAXIMUM_TOTAL_SIZE %d\r\n", rndis_max_total_size);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_max_total_size;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_802_3_MULTICAST_LIST:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_802_3_MULTICAST_LIST\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32 = 0xE0000000;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_802_3_MAC_OPTIONS:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_802_3_MAC_OPTIONS\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32 = NDIS_MAC_OPTION_SUPPORTS_MAC_ADDRESS_OVERWRITE |
				NDIS_MAC_OPTION_NO_LOOPBACK;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_XMIT_OK:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_XMIT_OK %d\r\n", rndis_stats.tx_packets - rndis_stats.tx_errors - rndis_stats.tx_dropped);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_stats.tx_packets - rndis_stats.tx_errors - rndis_stats.tx_dropped;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_RCV_OK:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_RCV_OK %d\r\n", rndis_stats.rx_packets - rndis_stats.rx_errors - rndis_stats.rx_dropped);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_stats.rx_packets - rndis_stats.rx_errors - rndis_stats.rx_dropped;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_XMIT_ERROR:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_XMIT_ERROR %d\r\n", rndis_stats.tx_errors);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_stats.tx_errors;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_RCV_ERROR:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_RCV_ERROR %d\r\n", rndis_stats.rx_errors);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_stats.rx_errors;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_RCV_NO_BUFFER:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_RCV_NO_BUFFER %d\r\n", rndis_stats.rx_dropped);
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_stats.rx_dropped;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_MEDIA_SUPPORTED:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_MEDIA_SUPPORTED\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32 = NDIS_MEDIUM_802_3;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_PNP_CAPABILITIES:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_PNP_CAPABILITIES\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32++ = 0; // Flags
		*pBuf32++ = 0; // Magic Packet Wakeup
		*pBuf32++ = 0; // Pattern Wakeup
		*pBuf32++ = 0; // Link Change Wakeup
		bufferLen += 0x10;
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_PNP_QUERY_POWER:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_PNP_QUERY_POWER\r\n");
#endif // RNDIS_OID_DEBUG
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_VENDOR_DESCRIPTION:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_VENDOR_DESCRIPTION\r\n");
#endif // RNDIS_OID_DEBUG
		strcpy((char *)pBuf8, rndis_vendor_description);
		bufferLen += (strlen(rndis_vendor_description) + 1);
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_VENDOR_ID:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_VENDOR_ID\r\n");
#endif // RNDIS_OID_DEBUG
		memcpy(pBuf8, rndis_vendor_id, 3);
		bufferLen += 3;
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_GEN_VENDOR_DRIVER_VERSION:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_VENDOR_DRIVER_VERSION\r\n");
#endif // RNDIS_OID_DEBUG
		*pBuf32 = rndis_driver_version;
		bufferLen += sizeof(uint32_t);
		status = RNDIS_STATUS_SUCCESS;
		break;

	default:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID QUERY UNKNOWN!\r\n");
#endif // RNDIS_OID_DEBUG
		break;
	}

	resp->InformationBufferLength = bufferLen;
	resp->InformationBufferOffset = 0x10;

	// Fill in response message.
	resp->MessageLength = sizeof(struct rndis_query_cmplt) + bufferLen;

	return status;
}

static void rndis_update_packet_filter()
{
    if (rndis_packet_filter != 0)
	{
    	// This will also intialise the interface and allow data
    	// to start flowing.
		rndis_state = RNDIS_STATE_DATA_INITIALIZED;
		// Callback to the application to enable receiving packets and
		// set-up the packet filter.
		if (rndis_rx_enable_cb)
		{
			rndis_rx_enable_cb(1, rndis_packet_filter);
		}
#ifdef RNDIS_STATE_DEBUG
        tfp_printf("Packet filter ON P %c M %c\r\n",
				rndis_packet_filter & NDIS_PACKET_TYPE_PROMISCUOUS?'Y':'N',
				rndis_packet_filter & NDIS_PACKET_TYPE_MULTICAST?'Y':'N');
#endif // RNDIS_STATE_DEBUG
	}
	else
	{
		// This will also disable the interface and prevent the
		// device sending data to the host.
		rndis_state = RNDIS_STATE_INITIALIZED;
		// Callback to the application to disable receiving packets.
		if (rndis_rx_enable_cb)
		{
			rndis_rx_enable_cb(0, rndis_packet_filter);
		}
#ifdef RNDIS_STATE_DEBUG
        tfp_printf("Packet filter OFF\r\n");
#endif // RNDIS_STATE_DEBUG
	}
}

static uint32_t rndis_set_handler(uint32_t OID, uint8_t *set, uint32_t setLen, struct rndis_set_cmplt *resp)
{
	uint32_t status = RNDIS_STATUS_NOT_SUPPORTED;
	uint32_t *pSet32 = (uint32_t *)set;

	// Not currently used.
	(void)setLen;

	switch (OID)
	{
	case OID_GEN_CURRENT_PACKET_FILTER:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_GEN_CURRENT_PACKET_FILTER\r\n");
#endif // RNDIS_OID_DEBUG
		rndis_packet_filter = *pSet32;
		rndis_update_packet_filter();
#ifdef RNDIS_OID_DEBUG
		tfp_printf("Packet filter set to %x\r\n", rndis_packet_filter);
#endif // RNDIS_OID_DEBUG
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_802_3_MULTICAST_LIST:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_802_3_MULTICAST_LIST ignored\r\n");
#endif // RNDIS_OID_DEBUG
		// Case for ignoring this is strong.
		status = RNDIS_STATUS_SUCCESS;
		break;

	case OID_PNP_SET_POWER:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID_PNP_SET_POWER\r\n");
#endif // RNDIS_OID_DEBUG
		switch (*pSet32)
		{
		case NDIS_DEVICE_POWER_STATE_D0:
			rndis_packet_filter = rndis_saved_packet_filter;
			rndis_update_packet_filter();
#ifdef RNDIS_STATE_DEBUG
			if (rndis_packet_filter != 0)
			{
				tfp_printf("Device State D0 ON\r\n");
			}
			else
			{
				tfp_printf("Device State D0 OFF\r\n");
			}
#endif // RNDIS_STATE_DEBUG
			break;
		case NDIS_DEVICE_POWER_STATE_D1:
		case NDIS_DEVICE_POWER_STATE_D2:
		case NDIS_DEVICE_POWER_STATE_D3:
			pSet32++;
			rndis_packet_filter = *pSet32;
			rndis_update_packet_filter();
#ifdef RNDIS_OID_DEBUG
			tfp_printf("Packet filter set to %x\r\n", rndis_packet_filter);
#endif // RNDIS_OID_DEBUG
#ifdef RNDIS_STATE_DEBUG
			tfp_printf("Device State D1-3\r\n");
#endif // RNDIS_STATE_DEBUG
			break;
		}
		status = RNDIS_STATUS_SUCCESS;
		break;


	default:
#ifdef RNDIS_OID_DEBUG
		tfp_printf("OID SET UNKNOWN!\r\n");
#endif // RNDIS_OID_DEBUG
		break;
	}

	// Fill in response message.
	resp->MessageLength = sizeof(struct rndis_set_cmplt);

	return status;
}

struct rndis_notification_msg {
		uint32_t notification;
		uint32_t reserved;
};

static void rndis_notification()
{
	static const struct rndis_notification_msg	responseAvail = { RNDIS_RESPONSE_AVAILABLE, 0};

	if (USBD_ep_buffer_full(rndis_ep_not) == 0)
	{
		USBD_transfer(rndis_ep_not, (uint8_t *) &responseAvail, sizeof(struct rndis_notification_msg));
#ifdef RNDIS_NOTIFICATION_DEBUG
		tfp_printf("Notification sent.\r\n");
#endif // RNDIS_NOTIFICATION_DEBUG
	}
}

/* API FUNCTIONS ************************************************************/

void USBD_RNDIS_initialise(const uint8_t *mac, const uint8_t *vendor_id, const char *vendor_desc,
		size_t mtu_size, size_t max_total_size,
		USBD_ENDPOINT_NUMBER ep_n, USBD_ENDPOINT_NUMBER ep_out, USBD_ENDPOINT_NUMBER ep_in,
		RNDIS_rx_enable_cb rx_enable_cb)
{
	// Store the initialisation parameters into file-scope variables.
	rndis_current_mac = mac;
	rndis_vendor_id = vendor_id;
	rndis_vendor_description = vendor_desc;
	rndis_mtu_size = (uint32_t)mtu_size;
	rndis_max_total_size = (uint32_t)max_total_size;
	rndis_ep_not = ep_n;
	rndis_ep_out = ep_out;
	rndis_ep_in = ep_in;
	rndis_rx_enable_cb = rx_enable_cb;
}

// Currently no actions required.
void USBD_RNDIS_timer(void)
{
}

int8_t USBD_RNDIS_encap_command(size_t length)
{
	// Receive encapsulated command.
	length = USBD_transfer_ep0(USBD_DIR_OUT, rndis_command, length, length);
	rndis_commandLen = length;
	USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);

	return USBD_OK;
}

int8_t USBD_RNDIS_encap_response(size_t length)
{
	// Reply with encapsulated response.
	if (rndis_responseLen)
	{
		if (length < rndis_responseLen)
		{
			rndis_responseLen = length;
		}
		USBD_transfer_ep0(USBD_DIR_IN, rndis_response, rndis_responseLen, length);
		rndis_responseLen = 0;
		rndis_notificationAvailableResp = 0;
	}
	else if (rndis_connectionLen)
	{
		if (length < rndis_connectionLen)
		{
			rndis_connectionLen = length;
		}
		USBD_transfer_ep0(USBD_DIR_IN, rndis_connection, rndis_connectionLen, length);
		rndis_connectionLen = 0;
		rndis_notificationAvailableConnection = 0;
	}
	else if (rndis_speed_changeLen)
	{
		if (length < rndis_speed_changeLen)
		{
			rndis_speed_changeLen = length;
		}
		USBD_transfer_ep0(USBD_DIR_IN, rndis_speed_change, rndis_speed_changeLen, length);
		rndis_speed_changeLen = 0;
		rndis_notificationAvailableChange = 0;
	}

	// ACK
	USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);

	return USBD_OK;
}

// Process RNDIS encapsulated commands and send notifications.
void USBD_RNDIS_process(void)
{
	if (rndis_commandLen)
	{
		rndis_handle_command();
	}

	if (rndis_responseLen | rndis_connectionLen | rndis_speed_changeLen)
	{
		if (rndis_notificationAvailableResp)
		{
			rndis_notification();
			rndis_notificationAvailableResp = 0;
		}
		else if (rndis_notificationAvailableConnection)
		{
			rndis_notification();
			rndis_notificationAvailableConnection = 0;
		}
		else if (rndis_notificationAvailableChange)
		{
			rndis_notification();
			rndis_notificationAvailableChange = 0;
		}
	}
}

uint8_t USBD_RNDIS_state(void)
{
	return rndis_state;
}

uint8_t USBD_RNDIS_connected_state(void)
{
	if (rndis_state == RNDIS_STATE_DATA_INITIALIZED)
	{
		return rndis_connected;
	}
	return NDIS_MEDIA_STATE_DISCONNECTED;
}

void USBD_RNDIS_link(int speed)
{
	if (speed > 0)
	{
#ifdef RNDIS_STATE_DEBUG
		tfp_printf("Cable connected %d Mb/sec\r\n", speed);
#endif // RNDIS_STATE_DEBUG
		if (speed == 100)
			rndis_link_speed = NDIS_LINK_SPEED_100MBPS;
		else
			rndis_link_speed = NDIS_LINK_SPEED_10MBPS;

		if (rndis_connected != NDIS_MEDIA_STATE_CONNECTED)
		{
			rndis_connected = NDIS_MEDIA_STATE_CONNECTED;
			if (rndis_state != RNDIS_STATE_UNINITIALIZED)
			{
				rndis_send_status(RNDIS_STATUS_MEDIA_CONNECT);
				rndis_send_speed_change();
			}
		}
	}
	else
	{
#ifdef RNDIS_STATE_DEBUG
		tfp_printf("Cable disconnected\r\n");
#endif // RNDIS_STATE_DEBUG
		if (rndis_connected == NDIS_MEDIA_STATE_CONNECTED)
		{
			rndis_connected = NDIS_MEDIA_STATE_DISCONNECTED;
			if (rndis_state != RNDIS_STATE_UNINITIALIZED)
			{
				rndis_send_status(RNDIS_STATUS_MEDIA_DISCONNECT);
			}
		}
	}
}

// Update statistics for host software.
void USBD_RNDIS_stat(int8_t stat)
{
	switch (stat)
	{
	case USBD_RNDIS_STAT_TX_PACKETS:
		rndis_stats.tx_packets++;
		break;
	case USBD_RNDIS_STAT_RX_PACKETS:
		rndis_stats.rx_packets++;
		break;
	case USBD_RNDIS_STAT_TX_ERROR:
		rndis_stats.tx_errors++;
		rndis_stats.tx_packets++;
		break;
	case USBD_RNDIS_STAT_RX_ERROR:
		rndis_stats.rx_errors++;
		rndis_stats.rx_packets++;
		break;
	case USBD_RNDIS_STAT_TX_DROPPED:
		rndis_stats.tx_dropped++;
		rndis_stats.tx_packets++;
		break;
	case USBD_RNDIS_STAT_RX_DROPPED:
		rndis_stats.rx_dropped++;
		rndis_stats.rx_packets++;
		break;
	}
}

// Get statistics for host software.
uint32_t USBD_RNDIS_get_stat(int8_t stat)
{
	switch (stat)
	{
	case USBD_RNDIS_STAT_TX_PACKETS:
		return rndis_stats.tx_packets;
		break;
	case USBD_RNDIS_STAT_RX_PACKETS:
		return rndis_stats.rx_packets;
		break;
	case USBD_RNDIS_STAT_TX_ERROR:
		return rndis_stats.tx_errors;
		break;
	case USBD_RNDIS_STAT_RX_ERROR:
		return rndis_stats.rx_errors;
		break;
	case USBD_RNDIS_STAT_TX_DROPPED:
		return rndis_stats.tx_dropped;
		break;
	case USBD_RNDIS_STAT_RX_DROPPED:
		return rndis_stats.rx_dropped;
		break;
	}
	return 0;
}

int8_t USBD_RNDIS_write(uint8_t *buffer, size_t len)
{
	struct rndis_packet_msg msg;
	int8_t status = USBD_OK;

    if (rndis_state != RNDIS_STATE_DATA_INITIALIZED)
    {
    	return USBD_RNDIS_NOT_ENABLED;
    }

#ifdef RNDIS_PACKET_DEBUG
	tfp_printf("Rx: ");
	for (i = 0; i < len; i++)
	{
		tfp_printf("[%x]%02x ", i, buffer[i]);
	}
	tfp_printf("\r\n");
#endif // RNDIS_PACKET_DEBUG

	// Create a packet message header.
	memset(&msg, 0, sizeof(struct rndis_packet_msg));
	msg.MessageType = REMOTE_NDIS_PACKET_MSG;
	msg.MessageLength = len + RNDIS_ETHERNET_PACKET_OFFSET;
	msg.DataLength = len;
	msg.DataOffset = RNDIS_ETHERNET_PACKET_OFFSET - 8;

	// Write part packet of data to the DATA interface. This
	// contains only the rndis packet message header. The 'part'
	// parameter is set to one to enable a follow-on packet
	// containing the actual data.
	USBD_transfer_ex(rndis_ep_in,
			(uint8_t *)&msg, sizeof(struct rndis_packet_msg), 1, 0);

	// Write the packet data to the DATA interface and complete
	// the transaction by setting the 'part' parameter to zero.
	// The offset is the size of rndis packet message header.
	USBD_transfer_ex(rndis_ep_in,
			buffer,
			len, 0, sizeof(struct rndis_packet_msg));

	return status;
}

int8_t USBD_RNDIS_read(uint8_t *buffer, size_t *len)
{
	struct rndis_packet_msg msg;
	size_t read_bytes;
	size_t data_len;
	size_t maxp = USBD_ep_max_size(rndis_ep_out);
	int8_t status = USBD_RNDIS_NO_DATA;
	size_t transferred;

	if (USBD_ep_buffer_full(rndis_ep_out))
	{
		// Read in a part packet of data from the DATA interface.
		read_bytes = USBD_transfer_ex(rndis_ep_out, (uint8_t *)&msg,
				sizeof(struct rndis_packet_msg), 1, 0);

		if (read_bytes != sizeof(struct rndis_packet_msg))
		{
			*len = 0;
			return USBD_RNDIS_ERROR;
		}

		// Number of bytes in a packet which can be received before moving to the
		// next packet. This is used for recovery.
		data_len = maxp - read_bytes;

		// Only need to handle packet messages on the data interface.
		if (msg.MessageType == REMOTE_NDIS_PACKET_MSG)
		{
			// Find the length of the data to read from the header.
			data_len = msg.DataLength;

			// There's a maximum data size which we can handle. That is the data
			// packet maximum payload size.
			if ((data_len <= rndis_mtu_size) && (msg.MessageLength <= rndis_max_total_size))
			{
				// Read the rest of the packet data from the DATA interface.
				// The size of the data to read is only contained in the packet
				// message header. It must be read first and decoded. Complete
				// the transaction by setting the 'part' parameter to zero.
				// The offset is the size of rndis packet message header.
				transferred = USBD_transfer_ex(rndis_ep_out, buffer,
						msg.MessageLength - read_bytes, 0,
						read_bytes);

				if (transferred != msg.MessageLength - read_bytes)
				{
#ifdef RNDIS_ERROR_DEBUG
			tfp_printf("Recovery received packet too short: %d\r\n", transferred);
#endif // RNDIS_ERROR_DEBUG
					data_len = msg.MessageLength - read_bytes - transferred;
					read_bytes = read_bytes + transferred;
					goto recovery;
				}
				*len = data_len;

				status = USBD_RNDIS_OK;

#ifdef RNDIS_PACKET_DEBUG
				tfp_printf("Tx: ");
				for (i = 0; i < data_len; i++)
				{
					tfp_printf("%02x ", buffer[i]);
				}
				tfp_printf("\r\n");
#endif // RNDIS_PACKET_DEBUG
			}
			else
			{
#ifdef RNDIS_ERROR_DEBUG
				tfp_printf("Recovery packet length too long: %d\r\n", read_bytes);
#endif // RNDIS_ERROR_DEBUG
				goto recovery;
			}
		}
		else
		{
#ifdef RNDIS_ERROR_DEBUG
			tfp_printf("Recovery message type incorrect: %08x\r\n", msg.MessageType);
#endif // RNDIS_ERROR_DEBUG
			// read until end of packet.
			data_len = maxp - read_bytes;
			goto recovery;
		}
	}

	return status;

recovery:
	// We are going to receive read_bytes more data from the host. Need to save
	// that away in our buffer and destroy it.
	*len = 0;
	while (data_len)
	{
		USBD_transfer_ex(rndis_ep_out, (uint8_t *)buffer,
				maxp, 0, read_bytes);
		read_bytes = 0;
		if (data_len < maxp)
		{
			break;
		}
		data_len -= maxp;
	};

	return USBD_RNDIS_ERROR;
}
