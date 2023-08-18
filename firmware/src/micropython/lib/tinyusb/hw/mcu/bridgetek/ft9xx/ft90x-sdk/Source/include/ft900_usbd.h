/**
    @file

    @brief
    USB Device API.

    This contains USB Device API function definitions, constants and structures
    which are exposed in the API.

    Note that as this is a USB device all transaction nomenclature is from the
    point of view from the host. If the device sends data to the host then it
    is called an IN transaction, if it receives data from the host then it is
    an OUT transaction.
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

#ifndef FT900_USBD_H_
#define FT900_USBD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <ft900_usb.h>

/* CONSTANTS ***********************************************************************/

/**
    @name    Definitions for USB Device return values
        @details Set of return values from USB Device API calls.
 */
//@{
/** @brief Success for USB Host function.
 */
#define USBD_OK 0x00
/** @brief Invalid parameter supplied to API function.
 */
#define USBD_ERR_INVALID_PARAMETER -1
/** @brief Endpoint not configured.
 */
#define USBD_ERR_NOT_CONFIGURED -2
/** @brief Operation not supported.
 */
#define USBD_ERR_NOT_SUPPORTED -3
/** @brief Not enough endpoint resources.
 */
#define USBD_ERR_RESOURCES -4
/** @brief Incomplete/interrupted transfer.
 */
#define USBD_ERR_INCOMPLETE -5
/** @brief Device not configured by host.
 */
//#define USBD_ERR_DISCONNECT_EVENT -9
/** @brief Device physically disconnected from host.
 */
#define USBD_ERR_DISCONNECTED -10
//@}

/* TYPES ***************************************************************************/

/**
    @enum USBD_STATE
    @brief USB States. USB Spec section 9.1.
 **/
typedef enum
{
	/** Device is not attached. **/
	USBD_STATE_NONE,
	/** Device is attached to USB. **/
	USBD_STATE_ATTACHED,
	/** Device is attached and powered. **/
	USBD_STATE_POWERED,
	/** Device is attached, has power and has been reset. **/
	USBD_STATE_DEFAULT,
	/** Unique device address has not been set. **/
	USBD_STATE_ADDRESS,
	/** Unique device address is now assigned.  Device can be used by host. **/
	USBD_STATE_CONFIGURED,
	/** Device has been suspended. **/
	USBD_STATE_SUSPENDED,
} USBD_STATE;

/**
    @enum USBD_ENDPOINT_NUMBER
    @brief USB Endpoint Numbers.
 **/
typedef enum
{
	/** Endpoint 0 (Control Endpoint) **/
	USBD_EP_0,
	/** Endpoint 1. **/
	USBD_EP_1,
	/** Endpoint 2. **/
	USBD_EP_2,
	/** Endpoint 3. **/
	USBD_EP_3,
	/** Endpoint 4. **/
	USBD_EP_4,
	/** Endpoint 5. **/
	USBD_EP_5,
	/** Endpoint 6. **/
	USBD_EP_6,
	/** Endpoint 7. **/
	USBD_EP_7,
#if defined(__FT930__)
	/** Endpoint 8. **/
	USBD_EP_8,
	/** Endpoint 9. **/
	USBD_EP_9,
	/** Endpoint 10. **/
	USBD_EP_10,
	/** Endpoint 11. **/
	USBD_EP_11,
	/** Endpoint 12. **/
	USBD_EP_12,
	/** Endpoint 13. **/
	USBD_EP_13,
	/** Endpoint 14. **/
	USBD_EP_14,
	/** Endpoint 15. **/
	USBD_EP_15
#endif
}
USBD_ENDPOINT_NUMBER;

/**
    @enum USBD_ENDPOINT_TYPE
    @brief USB Endpoint Types.
 **/
typedef enum
{
	/** Disabled. **/
	USBD_EP_TYPE_DISABLED = 0,
	/** Bulk Endpoint. **/
	USBD_EP_BULK = 1,
	/** Interrupt Endpoint. **/
	USBD_EP_INT = 2,
	/** Isochronous Endpoint. **/
	USBD_EP_ISOC = 3,
	/** Control Endpoint. **/
	USBD_EP_CTRL = 4
} USBD_ENDPOINT_TYPE;

/**
    @enum USBD_ENDPOINT_SPEED
    @brief USB Endpoint Speed.
 **/
typedef enum
{
	/** Full speed. **/
	USBD_SPEED_FULL,
	/** High speed. **/
	USBD_SPEED_HIGH,
} USBD_DEVICE_SPEED;

/**
    @enum USBD_ENDPOINT_DIR
    @brief USB Endpoint Direction.
 **/
typedef enum
{
	/**Direction host to device. **/
	USBD_DIR_OUT,
	/**Direction device to host. **/
	USBD_DIR_IN,
} USBD_ENDPOINT_DIR;

/**
    @enum USBD_ENDPOINT_SIZE
    @brief USB Endpoint Sizes.
 **/
typedef enum
{
	/** 8 Bytes **/
	USBD_EP_SIZE_8   = 0x00,
	/** 16 Bytes **/
	USBD_EP_SIZE_16  = 0x01,
	/** 32 Bytes **/
	USBD_EP_SIZE_32  = 0x02,
	/** 64 Bytes **/
	USBD_EP_SIZE_64  = 0x03,
	/** 128 Bytes.  Only available on High Speed endpoints. **/
	USBD_EP_SIZE_128 = 0x04,
	/** 256 Bytes.  Only available on High Speed endpoints. **/
	USBD_EP_SIZE_256 = 0x05,
	/** 512 Bytes.  Only available on High Speed endpoints. **/
	USBD_EP_SIZE_512 = 0x06,
	/** 1023 Bytes.  Only available on Full Speed ISO endpoints. **/
	USBD_EP_SIZE_1023 = 0x07,
	/** 1024 Bytes.  Only available on High Speed ISO and Interrupt endpoints. **/
	USBD_EP_SIZE_1024 = 0x08,
} USBD_ENDPOINT_SIZE;

/**
    @enum USBD_ENDPOINT_DB
    @brief USB Endpoint Double Buffering Enable.
 **/
typedef enum
{
	/**Disabled. **/
	USBD_DB_OFF,
	/**Enabled. **/
	USBD_DB_ON,
} USBD_ENDPOINT_DB;

/**
    @enum USBD_TESTMODE_SELECT
    @brief Enums used to select the test modes.For more information refer to Section 7.1.20 of USB2.0 Specification.
 **/
typedef enum
{
	/**Test mode Test_J **/
	USBD_TEST_J = 1,
	/**Test mode Test_K **/
	USBD_TEST_K,
	/**Test mode Test_SE0_NAK **/
	USBD_TEST_SE0_NAK,
	/**Test mode Test_PACKET **/
	USBD_TEST_PACKET
} USBD_TESTMODE_SELECT;

/**
    @typedef   USBD_request_callback
    @brief     Callback declaration for Vendor, Class and optionally
                       Standard USB requests.
    @param[in] req USB request.
    @return    USBD_OK if the request was handled successfully; any other
               return value (such as FT9XX_FAILED) causes the USB driver
               to stall the control endpoints.
 **/
typedef int8_t (*USBD_request_callback)(USB_device_request *req);

/**
    @typedef   USBD_descriptor_callback
    @brief     Callback declaration for standard get descriptor requests
                       to obtain descriptor data.
    @param[in] req USB request.
    @param[in] buffer Data buffer containing descriptor.
    @return    USBD_OK if the request was handled successfully; any other
               return value (such as FT9XX_FAILED) causes the USB driver
               to stall the control endpoints.
 **/
typedef int8_t (*USBD_descriptor_callback)(USB_device_request *req, uint8_t **buffer, uint16_t *len);

/**
    @typedef   USBD_set_configuration_callback
    @brief     Callback declaration for set configuration standard requests.
    @param[in] req USB request.
    @return    USBD_OK if the request is valid; any other
               return value (such as FT9XX_FAILED) causes the USB driver
               to stall the control endpoints.
 **/
typedef int8_t (*USBD_set_configuration_callback)(USB_device_request *req);
/**
    @typedef   USBD_set_interface_callback
    @brief     Callback declaration for set interface standard requests.
    @param[in] req USB request.
    @return    USBD_OK if the request is valid; any other
               return value (such as FT9XX_FAILED) causes the USB driver
               to stall the control endpoints.
 **/
typedef int8_t (*USBD_set_interface_callback)(USB_device_request *req);
/**
    @typedef   USBD_get_interface_callback
    @brief     Callback declaration for get interface standard requests.
    @param[in] req USB request.
    @return    USBD_OK if the request is valid; any other
               return value (such as FT9XX_FAILED) causes the USB driver
               to stall the control endpoints.
 **/
typedef int8_t (*USBD_get_interface_callback)(USB_device_request *req, uint8_t *val);

/**
    @typedef USBD_suspend_callback
    @brief Callback declaration for a suspend/resume.
 **/
typedef void (*USBD_suspend_callback)(uint8_t status);

/**
    @typedef USBD_reset_callback
    @brief Callback declaration for a host reset.
 **/
typedef void (*USBD_reset_callback)(uint8_t status);

/**
    @typedef USBD_sof_callback
    @brief Callback declaration for a SOF event.
 **/
typedef void (*USBD_sof_callback)(uint16_t frame);

/**
    @typedef USBD_ep_callback
    @brief Callback declaration for transaction completion on endpoint.
    The endpoint number is passed to the callback to allow the same function
    to handle multiple endpoints.
 **/
typedef void (*USBD_ep_callback)(USBD_ENDPOINT_NUMBER ep_number);

/**
    @struct USBD_ctx
    @brief Struct containing callback functions for the USB upper layer
           driver and callback functions for USB suspend/resume and
           USB reset. Sets USBD configuration information.
 **/
typedef struct USBD_ctx
{
	/** [Optional] Standard request callback function.
	 * @details Handler for USB standard requests. This is used
	 * for overriding the built-in standard request handler to
	 * customise the responses to standard requests. If it is
	 * not set then the built-in handler will be used and the
	 * descriptor_cb function used to obtain descriptors.
	 **/
	USBD_request_callback standard_req_cb;
	/** [Optional] Descriptor callback function.
	 * @details Handler function to obtain descriptors (device,
	 * configuration, string, HID, Hub etc.) for use with the
	 * built-in USB standard request handler. This must be
	 * present if the standard request handler callback is not
	 * used. **/
	USBD_descriptor_callback get_descriptor_cb;
	/** [Optional] Set Configuration callback function.
	 * @details Handler function to check set configuration is
	 * valid for application. For use with the built-in USB
	 * standard request handler. If this is not present then
	 * the default handling of the request will occur. **/
	USBD_set_configuration_callback set_configuration_cb;
	/** [Optional] Set Interface callback function.
	 * @details Handler function to set the alternate settings
	 * for an interface. For use with the built-in USB
	 * standard request handler. If this is not present then
	 * the request will be stalled. **/
	USBD_set_interface_callback set_interface_cb;
	/** [Optional] Get Interface callback function.
	 * @details Handler function to return the alternate
	 * settings for an interface. For use with the built-in USB
	 * standard request handler. If this is not present then
	 * the request will be stalled. **/
	USBD_get_interface_callback get_interface_cb;
	/** [Optional] class request callback function. **/
	USBD_request_callback class_req_cb;
	/** [Optional] vendor request callback function. **/
	USBD_request_callback vendor_req_cb;
	/** [Optional] Endpoint Feature request callback function. **/
	USBD_request_callback ep_feature_req_cb;
	/** [Optional] Device Feature (Remote Wakeup) request callback function. **/
	USBD_request_callback feature_req_cb;
	/** [Optional] USB bus suspend callback function. **/
	USBD_suspend_callback suspend_cb;
	/** [Optional] USB bus suspend callback function. **/
	USBD_suspend_callback resume_cb;
	/** [Optional] USB bus LPM (Link Power Management) callback function. **/
	USBD_suspend_callback lpm_cb;
	/** [Optional] USB bus reset callback function. **/
	USBD_reset_callback reset_cb;
	/** [Optional] USB SOF callback function.
	 * Note that this is called at interrupt level. Caution is required when
	 * using this callback. **/
	USBD_sof_callback sof_cb;

	/** Endpoint size for control endpoints. Section 3.3.1 FT900 USB Program Manual.
    Sets DC_EP0_CONTROL register in Table 3.6.\n
        0: 8 bytes.
        1: 16 bytes.
        2: 32 bytes.
        3: 64 bytes.
	 **/
	USBD_ENDPOINT_SIZE ep0_size;
	/** Callback function for a data transfer to or from the control endpoint.
	 **/
	USBD_ep_callback ep0_cb;

	/** Device configuration section.
    High speed/full speed select. Section 3.2.2 FT900 USB Program Manual.\n
        0: Full speed only.
        1: High speed if available.
	 **/
	USBD_DEVICE_SPEED speed;

	/** Device power control.
    Engage power saving mode if bus-powered and suspend state entered.\n
        0: Disable.
        1: Enabled.
	 **/
	uint8_t /*USBD_POWER_CONTROL_SELECT*/ lowPwrSuspend;
} USBD_ctx;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS ****************************************************************/

/* Helper macro to get most-significant byte of a 2-byte variable. */
#ifndef MSB
#define MSB(x) (uint8_t)(((uint16_t)(x) >> 8) & 0x00ff)
#endif /* MSB */

/* Helper macro to get least-significant byte of a 2-byte variable. */
#ifndef LSB
#define LSB(x) (uint8_t)((uint16_t)(x) & 0x00ff)
#endif /* LSB */


/* FUNCTION PROTOTYPES *************************************************************/

/**
    @name Device initialisation.
 */
//@{
/**
    @brief     Initialise USB hardware.
    @details   Performs a software reset and intialises the USB hardware.\n
               The USBD_ctx contains function pointers to the protocol layer
               handling USB requests.  Appropriate USB requests will be
               routed to the correct handler, whether that be Standard, Class
               or Vendor requests.  A device may not need a handler for Vendor
               or Class requests depending on the device configuration.\n
               Optional function pointers are also available for USB suspend
               and resume callbacks and bus resets issued by the host.\n
               This function MUST be called prior to any further call to the
               USB functions.
    @param[in] ctx USB context.
 **/
void USBD_initialise(USBD_ctx *ctx);

/**
    @brief     Finalise USB hardware.
    @details   Releases any resources associated with the USB driver and
               disables the hardware.
 **/
void USBD_finalise(void);
//@}

/**
    @brief     Attach USB hardware.
    @details   Attaches the USB device to the USB host after a USB_detach
               call.
 **/
void USBD_attach(void);

/**
    @brief     Detach USB hardware.
    @details   Detaches the USB device from the USB host. This will look like
    		   device disconnect to the host and it will act like the device
    		   is removed.
 **/
void USBD_detach(void);


/**
    @name Physical connection detection.
 */
//@{
/**
    @brief     Check if the device is connected to a host (or external power source).
    @details   Checks the VBUS detect line for a host connected.
 **/
int8_t USBD_is_connected(void);

/**
    @brief     Connect to a USB host.
    @details   Checks the VBUS detect line for a host connected and proceed to
                         allow the device to negotiate a connection to the host.
    @return    USBD_OK on success.\n
               USBD_ERR_INVALID_PARAMETER if req is invalid.
 **/
int8_t USBD_connect(void);
//@}

/**
    @name Device USB Standard Request support.
 */
//@{
/**
    @brief     Handles SET_ADDRESS request.
    @details   Places the device in the ADDRESS state. The application
               has to respond with SETUP ACK or STALL.
               The default standard request handler will call this function,
               if the handler is overridden then the application must
               call this when this request is received.
    @param[in] req USB request.
    @return    USBD_OK on success.\n
               USBD_ERR_INVALID_PARAMETER if req is invalid.
 **/
int8_t USBD_req_set_address(USB_device_request *req);

/**
    @brief     Handles SET_CONFIGURATION request.
    @details   Places the device in the CONFIGURED state or puts it back
               into the ADDRESS state. The application has to respond with
               SETUP ACK or STALL.
               The default standard request handler will call this function,
               if the handler is overridden then the application must
               call this when this request is received.
               TODO: Possible future enhancement may be a method of checking
               configuration value defined by application.
    @param[in] req USB request.
    @return    USBD_OK on success.\n
               USBD_ERR_INVALID_PARAMETER if req is invalid.
 **/
int8_t USBD_req_set_configuration(USB_device_request *req);

/**
    @brief     Handles GET_CONFIGURATION request.
    @details   Handles the DATA phase of a GET_CONFIGURATION request from host.
               The application has to respond with SETUP ACK or STALL.
               The default standard request handler will call this function,
               if the handler is overridden then the application must
               call this when this request is received.
    @return    USBD_OK on success.\n
               USBD_ERR_INVALID_PARAMETER if req is invalid.
 **/
int8_t USBD_req_get_configuration(void);
//@}

/**
    @name Device endpoint control support.
 */
//@{
/**
    @brief     Stall endpoint.
    @details   Stalls the specified endpoint.
               The default standard request handler will call this function
               for a SET_FEATURE endpoint request.
    @param[in] ep_number USB endpoint number.
    @return    USBD_OK if successful
               USBD_ERR_NOT_CONFIGURED if the endpoint is not configured.
               USBD_ERR_INVALID_PARAMETER if the endpoint number is not allowed.
 **/
int8_t USBD_stall_endpoint(USBD_ENDPOINT_NUMBER  ep_number);

/**
    @brief     Clears endpoint stall.
    @details   Clears a stall from the specified endpoint.
               The default standard request handler will call this function
               for a CLEAR_FEATURE endpoint request.
    @param[in] ep_number USB endpoint number.
    @return    USBD_OK if successful
               USBD_ERR_NOT_CONFIGURED if the endpoint is not configured.
               USBD_ERR_INVALID_PARAMETER if the endpoint number is not allowed.
 **/
int8_t USBD_clear_endpoint(USBD_ENDPOINT_NUMBER  ep_number);
//@}

/**
    @name Device USB State machine support.
 */
//@{
/**
    @brief      Get USB state.
    @details    Returns the current state of the current USB device.  Please
                refer to section 9.1 of the USB 2.0 spec for more information.
    @return     Current state of the current USB device.
 **/
USBD_STATE USBD_get_state(void);

/**
    @brief      Set USB state.
    @details    Sets the current state of the current USB device.  Please
                refer to section 9.1 of the USB 2.0 spec for more information.
    @param[in]  state The new state of the current USB device.
 **/
void USBD_set_state(USBD_STATE state);
//@}

/**
    @name Device endpoint status support.
 */
//@{
/**
    @brief      Get USB endpoint buffer status.
    @details    Returns the current buffer status of an endpoint using the SELECT_ENDPOINT call.
    @param[in]  ep_number USB endpoint number.
    @return     Current state of the endpoint buffer. TRUE if full, FALSE if empty.
 **/
int8_t USBD_ep_buffer_full(USBD_ENDPOINT_NUMBER ep_number);

/**
    @brief      Get USB endpoint stall status.
    @details    Returns the current stall status of an endpoint using the SELECT_ENDPOINT call.
    @param[in]  ep_number USB endpoint number.
    @return     Current stall state of the endpoint. >0 if stalled, zero if not stalled.
                USBD_ERR_NOT_CONFIGURED if the endpoint is not configured.
                USBD_ERR_INVALID_PARAMETER if the endpoint number is not allowed.
 **/
int8_t USBD_get_ep_stalled(USBD_ENDPOINT_NUMBER ep_number);
//@}

/**
    @name Device remote wakeup support.
 */
//@{
/**
    @brief      Get USB remote wakeup feature status.
    @details    Returns the current feature status of remote wakeup.
    @return     Current remote wakeup feature status. TRUE if enabled, FALSE if not enabled.
 **/
uint8_t USBD_get_remote_wakeup(void);

/**
    @brief      Set USB remote wakeup feature status.
 **/
void USBD_set_remote_wakeup(void);

/**
    @brief      Clear USB remote wakeup feature status.
 **/
void USBD_clear_remote_wakeup(void);

/**
    @brief      Resume signalling for remote wakeup.
    @details    Drive resume signalling upstream when remote wakeup is enabled.
 **/
void USBD_wakeup(void);

/**
    @brief      Resume from Suspend.
    @details    When USB related events like host resume and host reset are detected,
     	 	    PM irq will be received if it is enabled. The firmware needs to remove
     	 	    the SUSPEND from the PHY by calling this function.
 **/
void USBD_resume(void);
//@}

/**
    @name Device endpoint creation support.
 */
//@{
/**
    @brief      Create a USB endpoint.
    @details    Creates an endpoint with the requested properties.\n
				There is a total of 4 kB of RAM for FT90x Rev B, 6KB for FT90x Rev C
				and 8KB for FT930. This total RAM is for all the IN and OUT endpoints.
				Therefore the total max packet for all IN endpoints and OUT endpoints
				must be less than this figure. If double buffering is employed for an
				endpoint then it will use twice the amount of RAM.
    @param[in]  ep_number USB endpoint number. (N/A for control endpoints).
    @param[in]  ep_type USB endpoint type: BULK, ISO or INT. (N/A for control endpoints).
    @param[in]  ep_dir Endpoint direction, In or Out. (
    @param[in]  ep_size USB endpoint max packet size in bytes.
    @param[in]  ep_db USB endpoint double buffering enable. (N/A for control endpoints).
    @param[in]  ep_cb Callback function for this endpoint. This function will be called
                when an event concerned with the endpoint has occurred.
				This can be used for receiving notification of transaction to or from
				the endpoint heralding the availability of data (OUT endpoints) or the
				completion of a transmission of data (IN endpoints). However, the
				USBD_ep_buffer_full() function can be polled to determine the same
                status if callbacks are inappropriate.
    @return     USBD_OK if successful.
                            USBD_ERR_NOT_SUPPORTED if an endpoint higher than the maximum number
                                of endpoints is requested.
                            USBD_ERR_INVALID_PARAMETER if an illegal endpoint size is requested.
                            USBD_ERR_RESOURCES if there is not enough endpoint RAM for the
                                    endpoint size requested.
 **/
int8_t USBD_create_endpoint(USBD_ENDPOINT_NUMBER   ep_number,
		USBD_ENDPOINT_TYPE     ep_type,
		USBD_ENDPOINT_DIR      ep_dir,
		USBD_ENDPOINT_SIZE     ep_size,
		USBD_ENDPOINT_DB                 ep_db,
		USBD_ep_callback ep_cb);

/**
    @brief      Free USB endpoint.
    @details    Disable and free the specified endpoint.
    @param[in]  ep USB endpoint handle.
    @return     USBD_OK if successful
                USBD_ERR_NOT_CONFIGURED if the endpoint is not configured.
                USBD_ERR_INVALID_PARAMETER if the endpoint number is not allowed.
 **/
int8_t USBD_free_endpoint(USBD_ENDPOINT_NUMBER ep_number);

/**
    @brief      Find Max Packet Size of USB endpoint.
    @details    Return the maximum number of bytes which can be sent or received
     	 	 	in a single USB packets for an endpoint.
    @param[in]  ep USB endpoint handle.
    @return     Number of bytes if successful
                USBD_ERR_NOT_CONFIGURED if the endpoint is not configured.
                USBD_ERR_INVALID_PARAMETER if the endpoint number is not allowed.
 **/
uint16_t USBD_ep_max_size(USBD_ENDPOINT_NUMBER ep_number);
//@}

/**
    @brief      Transfer data to/from a non-control USB endpoint with options.
    @details    USB IN or OUT request is implied from the settings of the
                endpoint passed as a parameter. The end-of-packet will
                not be sent when the data from the buffer parameter is sent.
                This will allow a follow-on USBD_transfer_ex call to either
                send more data (with the part parameter non-zero and a
                correct offset set) or an end-of-packet with part not set.
                This allows a USB data packet to a non-control endpoint to
                be formed from multiple calls with data from potentially
                different places.
    @param[in]  ep_number USB endpoint number.
    @param[in]  buffer Appropriately sized buffer for the transfer.
    @param[in]  length For IN transfers, the number of bytes to be sent.
                       For OUT transfers, the maximum number of bytes to
                       read.
    @param[in]	part   Signifies that this is a partial transfer. Set one of
    					USBD_TRANSFER_EX_PART_NORMAL or
    					USBD_TRANSFER_EX_PART_NO_SEND.
    @param[in]	offset Offset (within the current packet) from where to
     	 	 	 	   continue for subsequent calls when using partial
     	 	 	 	   packets.
    @return     The number of bytes actually transferred.
                           USBD_ERR_NOT_CONFIGURED if endpoint is not configured.
                       USBD_ERR_INVALID_PARAMETER if endpoint number not allowed.
 **/
int32_t USBD_transfer_ex(USBD_ENDPOINT_NUMBER   ep_number,
		uint8_t       *buffer,
		size_t 	 	  length,
		uint8_t  	  part,
		size_t 		  offset);

/** @brief		Send packet normally.
    @details	This will send the data passed immediately, copying it into the
    			transmission buffer and sending it. If the length falls on a
    			wMaxPacketSize boundary then a ZLP will be sent after the packet
    			(this will not happen for interrupt endpoints).
 **/
#define USBD_TRANSFER_EX_PART_NORMAL 0

/** @brief		Send packet normally.
    @details	This will pass data to the transmission buffer but not transmit
    			it unless the data falls on or passes a wMaxPacketSize boundary.
    			If this happens a packet of wMaxPacketSize length will be sent
    			and any remaining data paassed to the transmission buffer. No
    			ZLP will be sent under any circumstances. The last packet sent
    			in the sequence therefore cannot be sent with this method.
 **/
#define USBD_TRANSFER_EX_PART_NO_SEND 1

/**
    @brief      Transfer data to/from a non-control USB endpoint.
    @details    USB IN or OUT request is implied from the settings of the
                endpoint passed as a parameter.
    @param[in]  ep_number USB endpoint number.
    @param[in]  buffer Appropriately sized buffer for the transfer.
    @param[in]  length For IN transfers, the number of bytes to be sent.
                       For OUT transfers, the maximum number of bytes to
                       read.
    @return     The number of bytes actually transferred.
                           USBD_ERR_NOT_CONFIGURED if endpoint is not configured.
                       USBD_ERR_INVALID_PARAMETER if endpoint number not allowed.
 **/
int32_t USBD_transfer(USBD_ENDPOINT_NUMBER   ep_number,
		uint8_t       *buffer,
		size_t 		  length);

/**
    @brief      Transfer data to/from a USB control endpoint.
    @details    Endpoint number is asusmed to be zero.
    @param[in]  ep_dir Control endpoint data direction.
    @param[in]  buffer Appropriately sized buffer for the transfer.
    @param[in]  dataLength For IN transfers, the number of bytes to be sent.
                       For OUT transfers, the maximum number of bytes to
                       read.
    @param[in]  requestLength The number of bytes requested by the host
    				   in the wLength field of the SETUP packet.
    @return     The number of bytes actually transferred.
                           USBD_ERR_NOT_CONFIGURED if endpoint is not configured.
 **/
int32_t USBD_transfer_ep0( USBD_ENDPOINT_DIR dir,
		uint8_t       *buffer,
		size_t 		  dataLength,
		size_t 		  requestLength);

/**
    @brief      USB process (Deprecated)
    @details    To be continuously called by the user application or USB device
                thread. Checks for control endpoint transfer activity and
                invokes relevant callback. Manages suspend and resume states
                and power management.
    @return     Non-zero if USB transaction has been processed.
 **/
__attribute__ ((deprecated)) int8_t USBD_process(void);

/**
    @brief      USB timer
    @details    To be called every millisecond from an interrupt handler to
                    provide timeout support for USB device transactions.
                                This will check all pending transfers, decrement
                                timeout values and expire any timed out transactions.
 **/
void USBD_timer(void);

/**
    @brief      USBD_get_bus_speed
    @details    To be called to get the current USB bus speed at which USBD operates
    @return     returns Full or High Speed.
 **/
USBD_DEVICE_SPEED USBD_get_bus_speed(void);

/**
    @brief      USBD_ep_data_rx_count
    @details    Provides the size of the OUT packet that is yet to be read
    @param[in]  ep_number USB endpoint number.
    @return     The number of bytes actually received.
                USBD_ERR_NOT_CONFIGURED if endpoint is not configured.
 **/
int32_t USBD_ep_data_rx_count(USBD_ENDPOINT_NUMBER ep_number);

/**
    @brief      USBD_set_test_mode
    @details    To be called to enter into Test Mode.
    @param[in]  the type of test to be performed in the Test Mode.
 **/
void USBD_set_test_mode(USBD_TESTMODE_SELECT test_selector);

/**
    @brief      Suspend USB device.
    @details    When device is initialized and no bus activity for certain time, this API can be
     	 	    called to put the USB device to suspend.
     	 	    This API takes the USBD state to USBD_STATE_SUSPENDED
 **/
void USBD_suspend_device(void);
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBD_H_ */
