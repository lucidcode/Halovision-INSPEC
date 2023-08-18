/**
    @file ft900_usbh_cdcacm.h

    @brief
    CDC ACM devices on USB host stack API.

	@details
    API functions for USB Host stack. These functions provide additional
    functionality useful to implement a USB Host application.
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

#ifndef FT900_USBH_CDCACM_H_
#define FT900_USBH_CDCACM_H_

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include "ft900_usb_cdc.h"
#include <ft900_usbh.h>
#include "ft900_usbh_cdcacm.h"

/* CONSTANTS ***********************************************************************/

/**
    @name    Definitions for CDC return values
	@details Set of return values from USB Host API calls.
 */
//@{
/** @brief Success for CDC function.
 */
#define USBH_CDCACM_OK 0
/** @brief Parameter error in call to CDC function.
 */
#define USBH_CDCACM_ERR_PARAMETER -1
/** @brief Device class not supported.
 */
#define USBH_CDCACM_ERR_CLASS_NOT_SUPPORTED -2
/** @brief Class request not supported.
 */
#define USBH_CDCACM_ERR_CLASS -3
/** @brief Notification Endpoint not found or polling failed.
 */
#define USBH_CDCACM_ERR_NOTIFICATION_ENDPOINT -4
/** @brief Data Endpoints not found or polling failed.
 */
#define USBH_CDCACM_ERR_DATA_ENDPOINT -5
/** @brief Function descriptor not found.
 */
#define USBH_CDCACM_ERR_FUNCTIONAL_DESCRIPTOR -6
/** @brief Unexpected USB error occurred.
 */
#define USBH_CDCACM_ERR_USB -7
//@}

/**
 	 @name 	Driver flags
 */
//@{
/// Do not expect or poll the notification endpoint in the Communication Class Interface,
#define CDCACM_FLAG_NO_NOTIFICATION 		1
//@}

/**
 * @name CDC Data IN endpoint maximum packet size
 */
//@{
#ifndef CDCACM_IN_MAX_PACKET
#define CDCACM_IN_MAX_PACKET 512
#endif // CDCACM_IN_MAX_PACKET
//@}

/**
 * @name CDC receive buffer size
 */
//@{
#ifndef CDCACM_IN_BUFFER
#define CDCACM_IN_BUFFER 512
#endif // CDCACM_IN_BUFFER
//@}

/**
 * @name CDC notification circular buffer size
 */
//@{
#ifndef CDCACM_NOTIFICATION_BUFFER
#define CDCACM_NOTIFICATION_BUFFER 12
#endif // CDCACM_NOTIFICATION_BUFFER
//@}

/* TYPES ***************************************************************************/

// CDC Context
//
// Holds a context structure required by each instance of the CDCACM driver
typedef struct USBH_CDCACM_context {
	/// Handle to the CDC device. There may be multiple CDC interfaces
	/// on the same devices.
    USBH_device_handle hControlDevice;
    /// Interface handles for CDC CONTROL and DATA interfaces.
    USBH_interface_handle hControlInterface;
    USBH_interface_handle hDataInterface;
    // Endpoint handles for the CDC CONTROL and DATA interfaces.
    USBH_endpoint_handle hControlEpIn, hDataEpIn, hDataEpOut;
    /// Interface numbers for control and data interface.
    /// These are used in SETUP requests to identify the correct interface.
    uint8_t controlInterfaceNumber, dataInterfaceNumber;
    /// Call Management capabilities. Bitmap from Call Management Functional
    /// descriptor indicating the method for managing calls on the device.
    uint8_t callCapabilities;
    /// Abstract Control Management capabilities. Bitmap from Abstract Control
    /// Management Functional descriptor indicating support for Comm, Line
    /// Coding, Break and Network features.
    uint8_t acmCapabilities;
    /// Notification bitmap for the UART state received from device.
    USB_CDC_UartStateBitmap uartState;
    /// Notification bitmap for the network state received from device.
    USB_CDC_UartStateBitmap networkState;
    /// Response available notification flag indicating that an encapsulated
    /// response can be read from the interface.
    int8_t responseAvailable;
    /// Last status of CDC CONTROL notification poll from USB Host driver.
    int8_t notificationStatus;
    /// Buffer to receive notification structure from USB Host driver. This
    /// must be of type uint32_t to align with data buffers on EHCI controller.
    uint32_t notificationBuffer[CDCACM_NOTIFICATION_BUFFER/sizeof(uint32_t)];
    /// Last status of CDC DATA IN endpoint poll from USB Host driver.
    int8_t recvStatus;
    /// Buffer to receive data from the USB Host driver. This is exactly one MaxPacket
    /// size buffer. It must be of type uint32_t to follow alignment requirements
    /// of data buffers in USB Host driver.
    /// Note: can be of type uint8_t if it is qualified with:
    /// __attribute__ ((aligned (4)))
    uint32_t recvPacket[CDCACM_IN_MAX_PACKET/sizeof(uint32_t)];
    /// Circular buffer used to group packet data from the USB Host. This does not
    /// have any alignment issues.
    uint8_t recvBuffer[CDCACM_IN_BUFFER];
    /// Read, write and available pointers for circular buffer.
    volatile uint16_t recvBufferWrite;
    volatile uint16_t recvBufferRead;
    volatile uint16_t recvBufferAvail;
} USBH_CDCACM_context;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/**
    @brief      Initialise the CDC ACM driver.
    @details	Setup a context for the CDC driver to use the interfaces and settings
    			provided in the call.
    @param		hControlInterface - handle to the CDC CONTROL interface.
    @param		flags - CDCACM_FLAG_NO_NOTIFICATION to not poll notification endpoint.
    @param		ctx - structure instantiated in the application to hold the context
    			information for this instance of the driver.
    @returns    USBH_CDCACM_OK if successful
 **/
int8_t USBH_CDCACM_init(USBH_interface_handle hControlInterface,
		uint8_t flags, USBH_CDCACM_context *ctx);

/**
    @brief      Read data from the CDC ACM device.
    @details	Read a block of data from the CDC device DATA interface.
    			The data is buffered internally in the driver as it is produced by the CDC
    			device and polled by the USB host.
    			The buffer is designed to discard incoming data if the internal buffer fills.
    			Care must therefore be taken to ensure an adequate consumption rate of data
    			from the CDC device.
    @param		ctx - context information for this instance of the driver.
    @param                buffer - receiving buffer.
    @param		len - maximum length of data to read.
    @returns    Number of bytes transferred to the receiving buffer. This may be less than
     	 	 	the amount requested if insufficient data has been received from the CDC
     	 	 	device.
 **/
int32_t USBH_CDCACM_read(USBH_CDCACM_context *ctx, uint8_t *buffer, size_t len);

/**
    @brief      Write data to the CDC ACM device.
    @details	Write a block of data to the CDC device DATA interface.
				Data is written immediately to the device without buffering.
    @param		ctx - context information for this instance of the driver.
    @param                buffer - transmission buffer.
    @param		len - maximum length of data to write.
    @returns    Number of bytes transferred from the transmission buffer to the device.
 **/
int32_t USBH_CDCACM_write(USBH_CDCACM_context *ctx, uint8_t *buffer, size_t len);

/**
    @brief      Send an encapsulated command to the CDC ACM device.
    @details	Write a block of data to the CDC device control interface.
				Data is written immediately to the device without buffering.
    @param		ctx - context information for this instance of the driver.
    @param                cmd - transmission buffer containing command.
    @param		len - maximum length of data to write.
    @returns    Number of bytes transferred from the transmission buffer to the device.
 **/
int8_t USBH_CDCACM_send_encapsulated_command(USBH_CDCACM_context *ctx, char *cmd, size_t len);

/**
    @brief      Read an encapsulated command response from the CDC ACM device.
    @details	Read a block of data from the CDC device control interface.
				Data is read immediately to the device without buffering.
    @param		ctx - context information for this instance of the driver.
    @param                rsp - receive buffer for receiving response.
    @param		len - maximum length of data to receive.
    @returns    Number of bytes transferred from the device to the receive buffer.
 **/
int8_t USBH_CDCACM_get_encapsulated_response(USBH_CDCACM_context *ctx, char *rsp, size_t len);

/**
    @brief      Get a bitmap containing the currently set Comm Features.
    @details	The selector parameter is used to select between the abstract
     	 	 	state and country setting data.
    @param		ctx - context information for this instance of the driver.
    @param		selector - Abstract State or Country Setting:
    				CDC_ACM_FEATURE_SELECTOR_ABSTRACT_STATE,
					CDC_ACM_FEATURE_SELECTOR_COUNTRY_SETTING.
	@param		feature - bitmap to receive abstract state bitmap or country
					setting code.
    @returns    USBH_CDCACM_OK - if the interface supports the COMM Feature requests.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_CDCACM_get_comm_feature(USBH_CDCACM_context *ctx, uint16_t selector, uint16_t *feature);

/**
    @brief      Set Comm Features on the device.
    @details	The selector parameter is used to select between the abstract
     	 	 	state and country setting data.
    @param		ctx - context information for this instance of the driver.
    @param		selector - Abstract State or Country Setting:
    				CDC_ACM_FEATURE_SELECTOR_ABSTRACT_STATE,
					CDC_ACM_FEATURE_SELECTOR_COUNTRY_SETTING.
	@param		feature - bitmap to set the abstract state bitmap or country
					setting code.
    @returns    USBH_CDCACM_OK - if the interface supports the COMM Feature requests.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_CDCACM_set_comm_feature(USBH_CDCACM_context *ctx, uint16_t selector, uint16_t feature);
/**
    @brief      Clear the Comm Features on the device.
    @details	The selector parameter is used to select between the abstract
     	 	 	state and country setting data.
    @param		ctx - context information for this instance of the driver.
    @param		selector - Abstract State or Country Setting:
    				CDC_ACM_FEATURE_SELECTOR_ABSTRACT_STATE,
					CDC_ACM_FEATURE_SELECTOR_COUNTRY_SETTING.
    @returns    USBH_CDCACM_OK - if the interface supports the COMM Feature requests.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_CDCACM_clear_comm_feature(USBH_CDCACM_context *ctx, uint16_t selector);

/**
    @brief      Set Line Coding on the device.
    @details	The USB_CDC_line_coding structure is used to set the data
    			output format on the CDC device. This is on the 'UART' side
    			of the device.
    @param		ctx - context information for this instance of the driver.
    @param		coding - pointer to structure containing the requested
     	 	 	 	 parameters for formatting data.
    @returns    USBH_CDCACM_OK - if the interface supports the Line Coding Feature requests.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_CDCACM_set_line_coding(USBH_CDCACM_context *ctx, USB_CDC_line_coding *coding);

/**
    @brief      Get Current Line Coding settings from the device.
    @details	The USB_CDC_line_coding structure describes the data output
    			format on the 'UART' side of the CDC device. This will query
    			the settings and return them in the structure.
    @param		ctx - context information for this instance of the driver.
    @param		coding - pointer to structure containing the currently set
     	 	 	 	 parameters for formatting data.
    @returns    USBH_CDCACM_OK - if the interface supports the Line Coding Feature requests.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_CDCACM_get_line_coding(USBH_CDCACM_context *ctx, USB_CDC_line_coding *coding);

/**
    @brief      Set Control Line State on the device.
    @details	The USB_CDC_control_line_state structure is used to set the
    			state of the control lines on the CDC device. This is on the
    			'UART' side	of the device.
    @param		ctx - context information for this instance of the driver.
    @param		state - pointer to structure containing the requested
     	 	 	 	 state for the control lines.
    @returns    USBH_CDCACM_OK - if the interface supports the Line Coding Feature requests.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_CDCACM_set_control_line_state(USBH_CDCACM_context *ctx, USB_CDC_control_line_state *state);

/**
    @brief      Instructs the device to set a break state on the UART line.
    @param		ctx - context information for this instance of the driver.
    @param		duration - length of time in milliseconds to set the break state.
    @returns    USBH_CDCACM_OK - if the interface supports the Send_Break request.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_CDCACM_send_break(USBH_CDCACM_context *ctx, uint16_t duration);

/**
    @brief      Returns the UART state bitmap.
    @details	When a notification arrives updating the UART state this is
    			kept in the driver and can be queried by this command.
    @param		ctx - context information for this instance of the driver.
    @param		state - pointer to structure to receive last received state.
    @returns    USBH_CDCACM_OK - if the interface supports the Serial_State notification.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
uint8_t USBH_CDCACM_get_uart_state(USBH_CDCACM_context *ctx, USB_CDC_UartStateBitmap *state);
/**
    @brief      Returns the Network state bitmap.
    @details	When a notification arrives updating the Network state this is
    			kept in the driver and can be queried by this command.
    @param		ctx - context information for this instance of the driver.
    @param		state - pointer to structure to receive last received state.
    @returns    USBH_CDCACM_OK - if the interface supports the Network_Connection notification.
    			USBH_CDCACM_ERR_CLASS - if it does not support it.
 **/
uint8_t USBH_CDCACM_get_network_connection(USBH_CDCACM_context *ctx, USB_CDC_NetworkConnectionBitmap *state);
/**
    @brief      Indicates the response to an encapsulated command is waiting.
    @details	When a notification arrives indicating that a response to an
    			encapsulateed command is waiting to be read this is kept in
    			the driver and can be queried by this command.
    @param		ctx - context information for this instance of the driver.
    @returns    Non-zero if a encapsulated response is available.
 **/
int8_t USBH_CDCACM_get_response_available(USBH_CDCACM_context *ctx);

/**
    @brief      Returns the ACM Capabilities bitmap.
    @details	Returns the bmCapabilities value obtained from the Abstract
    			Control Management Functional Descriptor for the CDC ACM
    			interface.
    @param		ctx - context information for this instance of the driver.
    @returns    Values are defined as CDC_ACM_CAPABILITIES_*
 **/
int8_t USBH_CDCACM_get_acm_capabilities(USBH_CDCACM_context *ctx);

/**
    @brief      Returns the Call Capabilities bitmap.
    @details	Returns the bmCapabilities value obtained from the Call
    			Management Functional Descriptor for the CDC ACM
    			interface.
    @param		ctx - context information for this instance of the driver.
    @returns    Values are defined as CDC_CM_CAPABILITIES_*
 **/
int8_t USBH_CDCACM_get_call_capabilities(USBH_CDCACM_context *ctx);

/**
    @brief      Returns the last USB Host statuses for endpoint polling.
    @details	Each time an endpoint is polled the status is stored.
    			Both the notification endpoint and the data IN endpoint
    			values are stored and can be queried by this function.
    @param		ctx - context information for this instance of the driver.
    @param		notification_status - pointer to receive status of notification
    				endpoint polling.
    @param		data_status - pointer to receive status of data endpoint
    				polling.
 **/
void USBH_CDCACM_get_poll_status(USBH_CDCACM_context *ctx, int8_t *notification_status, int8_t *data_status);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBH_CDCACM_H_ */
