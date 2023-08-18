/**
    @file ft900_usbd_rndis.h

    @brief
    API for USB device stack RNDIS on FT900.

	@details
    API functions for USB device RNDIS stack. These functions provide all
    the functionality required to implement a USB Device RNDIS interface.
    Only one instance of the USB device RNDIS driver may exist.

    @version 1.0.0
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

#ifndef FT900_USBD_RNDIS_H_
#define FT900_USBD_RNDIS_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

#include <stdint.h>

#include <ft900_usbd.h>

/* CONSTANTS ***********************************************************************/

/**
    @name    Definitions for RNDIS return values
    @details Set of return values from RNDIS API calls.
 */
//@{
/** @brief Success for RNDIS function.
 */
#define USBD_RNDIS_OK 0x00
/** @brief An error occurred reading a packet from the USB.
 */
#define USBD_RNDIS_ERROR -1
/** @brief No data available. Not an error.
 */
#define USBD_RNDIS_NO_DATA 1
/** @brief Packet filter disabled.
 */
#define USBD_RNDIS_NOT_ENABLED 2
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
 * This current state can be queried by USBD_RNDIS_state function.
 */
#define RNDIS_STATE_UNINITIALIZED 0
#define RNDIS_STATE_INITIALIZED 1
#define RNDIS_STATE_DATA_INITIALIZED 2

/** @name Stats counter types for USBD_RNDIS_stat function.
 * @brief The statistics are an integral part of the host software for networking
 * and need to be maintained accurately. The driver reports the values of the
 * various stats to the host software but the application uses the values here as
 * a parameter in a call USBD_RNDIS_stat. The function will increment the stats
 * counter.
 */
#define USBD_RNDIS_STAT_TX_PACKETS 1
#define USBD_RNDIS_STAT_RX_PACKETS 2
#define USBD_RNDIS_STAT_TX_ERROR 3
#define USBD_RNDIS_STAT_RX_ERROR 4
#define USBD_RNDIS_STAT_TX_DROPPED 5
#define USBD_RNDIS_STAT_RX_DROPPED 6

/* TYPES ***************************************************************************/

/**
    @typedef RNDIS_rx_enable_cb
    @brief Callback declaration for function to enable the receiver and set the
    receiver packet filter.
    @param[in]	rx_enable Non-zero to enable receive packets on the media.
    @param[in]	rx_filter A bitmap of filters for packet types requested by the
     	 	 	 host. These are defined as NDIS_PACKET_TYPE_*
 **/
typedef void (*RNDIS_rx_enable_cb)(uint8_t rx_enable, uint32_t rx_filter);

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/**
    @name Device initialisation.
 */
//@{
/**
    @brief     Initialise RNDIS driver.
    @details   Setup the RNDIS driver with the settings provided by the application.
    @param[in] mac Pointer to a 6 byte array containing the MAC address of the device.
    @param[in] vendor_id Pointer to a 3 byte array containing a IEEE vendor ID.
    @param[in] vendor_desc Pointer to a null-terminated string with a description of
    			the vendor.
    @param[in] mtu_size Value of Maximum Transfer Unit (MTU) to use.
    @param[in] max_total_size Maximum packet size which can be transmitted. This may
				be larger than the MTU. Larger values use more buffering.
    @param[in] ep_n Endpoint handle for the RNDIS notification endpoint.
    @param[in] ep_out Endpoint handle for the RNDIS data OUT endpoint.
    @param[in] ep_in Endpoint handle for the RNDIS data IN endpoint.
	@param[in] rx_enable_cb Callback made when receiving data.
 **/
 void USBD_RNDIS_initialise(const uint8_t *mac, const uint8_t *vendor_id, const char *vendor_desc,
 		size_t mtu_size, size_t max_total_size,
 		USBD_ENDPOINT_NUMBER ep_n, USBD_ENDPOINT_NUMBER ep_out, USBD_ENDPOINT_NUMBER ep_in,
 		RNDIS_rx_enable_cb rx_enable_cb);
//@}

/**
    @name Device operation.
 */
//@{
/**
    @brief     Timer handler.
    @details   Currently unused timer handler.
 **/
void USBD_RNDIS_timer(void);

/**
    @brief     Process RNDIS transactions on the USB device.
    @details   Checks for any received encapsulated commands or notifications
    		   requiring action.
    		   Encapsulated commands are processed and responses generated. There
    		   are no actual USB transfers required in this part of the function
    		   as the reception of commands and the transmission of responses are
    		   driven by control endpoint requests that are handled by the
    		   application.
    		   Any notifications requiring action will trigger a packet to be sent
    		   on the USB through the notification endpoint.
 **/
void USBD_RNDIS_process(void);

/**
    @brief     Complete a USB SETUP request to receive an encapsulated command.
    @details   The application will receive a request to send an encapsulated
    		   command to the device. It will call this function to perform the
    		   DATA and ACK phase of the SETUP transaction.
    		   The data received will be copied into an internal buffer in the
    		   driver and processed by the USBD_RNDIS_process function. This is
    		   to allow the SETUP handler in the application to continue.
    @param[in] length The number of bytes of data sent as part of the SETUP.
    @returns   USBD_OK if successful.
    		   USBD_ERR_* if not successful.
 **/
int8_t USBD_RNDIS_encap_command(size_t length);

/**
    @brief     Complete a USB SETUP request to send an encapsulated response.
    @details   The application will receive a request to send an encapsulated
    		   response from the device. It will call this function to perform
    		   the DATA and ACK phase of the SETUP transaction.
    		   The data sent will be copied from an internal buffer in the
    		   driver. If there is no response then an ACK will be sent.
    @param[in] length The maximum number of bytes of data sent as part of the SETUP.
    @returns   USBD_OK if successful.
    		   USBD_ERR_* if not successful.
 **/
int8_t USBD_RNDIS_encap_response(size_t length);

/**
    @brief     RNDIS driver state.
    @details   The initialisation state of the driver. The uninitialised state
    		   is active before a REMOTE_NDIS_INITIALIZE_MSG has been processed.
    		   Then it stays in the initialised state when the packet filter is
    		   turned off. No packets are reported to the host software.
    		   When the packet filter is on then it moves to the data state.
    		   Now packets which pass the packet filter are sent to the host
    		   software.
    @returns   RNDIS_STATE_UNINITIALIZED - not initialised.
     	 	   RNDIS_STATE_INITIALIZED - packet filter is off.
     	 	   RNDIS_STATE_DATA_INITIALIZED - packet filter enabled.
 **/
uint8_t USBD_RNDIS_state(void);

/**
    @brief     RNDIS connected state.
    @details   The current connection state of the RNDIS. If media is connected
    		   as set by USBD_RNDIS_link_connected AND the driver state is
    		   RNDIS_STATE_DATA_INITIALIZED then this will report that the
    		   RNDIS driver is connected.
    @returns   NDIS_MEDIA_STATE_DISCONNECTED - packet filter is disabled or
    			   no media is connected.
     	 	   NDIS_MEDIA_STATE_CONNECTED - packet filter is on and media
     	 	   	   is connected.
 **/
uint8_t USBD_RNDIS_connected_state(void);

/**
    @brief     Notify RNDIS that the ethernet link is connected.
    @details   Sets the media link status of RNDIS to connected and sets the
    		   speed of the link to either 10 or 100 Mb/sec.
    @param[in] speed The connection speed. Either 0, 10 or 100. This is
    				reported to the host software. If the speed is zero
    				then the link is disconnected.
 **/
void USBD_RNDIS_link(int speed);

/**
    @brief     Notify RNDIS that an ethernet event occurred.
    @details   The statistics are collected about successful packets, error
    		   and dropped packets for both transmission (Tx) and reception
    		   (Rx). Call with a parameter of USBD_RNDIS_STAT_* to
    		   increment that counter. Dropped and error packets will
    		   also increment the total packet counter.
    @param[in] stat Statistic counter to increment. Can be one of:
				USBD_RNDIS_STAT_TX_PACKETS, USBD_RNDIS_STAT_RX_PACKETS,
				USBD_RNDIS_STAT_TX_ERROR, USBD_RNDIS_STAT_RX_ERROR
				USBD_RNDIS_STAT_TX_DROPPED, USBD_RNDIS_STAT_RX_DROPPED
 **/
void USBD_RNDIS_stat(int8_t stat);

/**
    @brief     Query RNDIS for a count of ethernet events that have occurred.
    @details   The statistics are collected about successful packets, error
    		   and dropped packets for both transmission (Tx) and reception
    		   (Rx). Call with a parameter of USBD_RNDIS_STAT_* to
    		   query that counter.
    @param[in] stat Statistic counter to query. Can be one of:
				USBD_RNDIS_STAT_TX_PACKETS, USBD_RNDIS_STAT_RX_PACKETS,
				USBD_RNDIS_STAT_TX_ERROR, USBD_RNDIS_STAT_RX_ERROR
				USBD_RNDIS_STAT_TX_DROPPED, USBD_RNDIS_STAT_RX_DROPPED
    @returns   Number of ethernet events for a query.
 **/
uint32_t USBD_RNDIS_get_stat(int8_t stat);

/**
    @brief     Write data to the RNDIS host software on the data channel.
    @details   Writes a buffer of data containing an ethernet packet to the
    		   host software on the data IN endpoint. The packet will be
    		   formatted as an RNDIS packet message.
    @param[in] buffer Pointer to a buffer containing the data.
    @param[in] len Length of the buffer containing the data.
    @returns   USBD_OK if successful.
    		   USBD_ERR_* if not successful.
 **/
int8_t USBD_RNDIS_write(uint8_t *buffer, size_t len);

/**
    @brief     Read data from the RNDIS host software data channel.
    @details   Reads a buffer of data containing an ethernet packet from
    		   the host software on the data OUT endpoint. The packet will
    		   stripped of the RNDIS packet message formatting ready to
    		   transmit as an ethernet packet.
    		   Therefore every buffer supplied must be able to handle 1518
    		   bytes (1500 data bytes, 16 ethernet header bytes and
 	 		   2 payload length bytes).
    @param[in] buffer Pointer to a buffer containing the data.
    @param[out] len Pointer to a variable to receiver the length of the
    		   buffer containing the data.
    @returns   USBD_OK if successful.
    		   USBD_ERR_* if not successful.
 **/
int8_t USBD_RNDIS_read(uint8_t *buffer, size_t *len);

//@}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBD_RNDIS_H_ */
