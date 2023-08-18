/**
    @file ft900_usbh.h

    @brief 
    USB host stack API.
  
    This contains USB Host API function definitions, constants and structures which
    are exposed in the API.
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

#ifndef FT900_USBH_H_
#define FT900_USBH_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Align structures/variables on 32-bit boundaries. Required for variables passed to
// certain functions. Place this after variable declarations.
#define ALIGN32 __attribute__ ((aligned(4)))

/* INCLUDES ************************************************************************/

#include <stddef.h>
#include <stdint.h>

/* CONSTANTS ***********************************************************************/

/**
    @name    Definitions for USB Host return values
	@details Set of return values from USB Host API calls.
 */
//@{
/** @brief Success for USB Host function.
 */
#define USBH_OK 0x00
/** @brief No change in enumeration
 */
#define USBH_ENUM_NO_CHANGE 1
/** @brief Partial enumeration only
 */
#define USBH_ENUM_PARTIAL 2
/** @brief Lack of resources to perform USB Host function.
 */
#define USBH_ERR_RESOURCES -1
/** @brief Host controller completed and reported an error.
 */
#define USBH_ERR_USBERR -2
/** @brief Host controller halted.
 */
#define USBH_ERR_HOST_HALTED -3
/** @brief Endpoint, Device or Interface not found.
 */
#define USBH_ERR_NOT_FOUND -4
/** @brief Endpoint, Device or Interface removed.
 */
#define USBH_ERR_REMOVED -5
/** @brief Endpoint stalled.
 */
#define USBH_ERR_STALLED -6
/** @brief Endpoint transaction timeout.
 */
#define USBH_ERR_TIMEOUT -8

/** @brief Request parameter error.
 */
#define USBH_ERR_PARAMETER -15
/** @brief Transaction completed with halted state.
 */
#define USBH_ERR_HALTED -16
/** @brief Endpoint data underun or overrun.
 */
#define USBH_ERR_DATA_BUF -17
/** @brief Endpoint data babble detected.
 */
#define USBH_ERR_BABBLE -18
/** @brief Endpoint data transaction error.
 */
#define USBH_ERR_XACTERR -19
/** @brief Endpoint data missed microframe.
 */
#define USBH_ERR_MISSED_MICROFRAME -20
/** @brief Endpoint split transaction state.
 */
#define USBH_ERR_SPLITX -21
/** @brief Endpoint ping state.
 */
#define USBH_ERR_PING_ERR -22

//@}

/**
    @name    Definitions for USB Hubs
	@details Set of values for defining access to the hubs.
 */
//@{
/** @brief Handle used to access Root hub
 */
#define USBH_ROOT_HUB_HANDLE 0
/** @brief Port used to access Root hub
 */
#define USBH_ROOT_HUB_PORT 0
/** @brief Port used to access all devices on a hub
 */
#define USBH_HUB_ALL_PORTS 0
//@}

/* TYPES ***************************************************************************/

/**
    @enum USBH_STATE
    @brief USB Root Hub Connection States.
 **/
typedef enum 
{
    /** No device is attached to USB root hub. **/
    USBH_STATE_NOTCONNECTED,
    /** Device is attached to USB root hub. **/
    USBH_STATE_CONNECTED,
    /** Device is attached successfully enumerated. All downstream
     * devices have also been successfully enumerated. **/
    USBH_STATE_ENUMERATED,
    /** Device is attached and has been partially enumerated. There may
     * be more devices, interfaces or endpoints connected than configured.
     * Some devices may be missing interfaces and/or endpoints.
     * It is conceivable that some downstream devices may not be configured
     * at all. **/
    USBH_STATE_ENUMERATED_PARTIAL,
} USBH_STATE;

/**
    @typedef USBH_CONTROLLER_STATE
    @brief USB Host Controller State describing if the host is operational
        or suspending or resuming. Used to control transitions between these
        state.
 **/
typedef enum 
{
    /** Controller reset and uninitialised. **/
    USBH_CONTROLLER_STATE_RESET,
    /** Controller initialised and operational. **/
    USBH_CONTROLLER_STATE_OPERATIONAL,
    /** Controller performing a suspend. Transitioning from operational to 
        suspend. **/
    USBH_CONTROLLER_STATE_SUSPENDING,
    /** Controller in suspend state. No SOFs generated. **/
    USBH_CONTROLLER_STATE_SUSPEND,
    /** Controller performing a resume. Transitioning from suspend to 
        operational. **/
    USBH_CONTROLLER_STATE_RESUME,
} USBH_CONTROLLER_STATE;

/**
    @typedef USBH_ENDPOINT_NUMBER
    @brief USB Endpoint Numbers.
 **/
typedef uint8_t USBH_ENDPOINT_NUMBER;

/**
    @enum USBH_ENDPOINT_TYPE
    @brief USB Endpoint Types.
 **/
typedef enum 
{
    /** Disabled. **/
    USBH_EP_TYPE_DISABLED = 0,
    /** Bulk Endpoint. **/
    USBH_EP_BULK = 1,
    /** Interrupt Endpoint. **/
    USBH_EP_INT = 2,
    /** Isochronous Endpoint. **/
    USBH_EP_ISOC = 3,
    /** Control Endpoint. **/
    USBH_EP_CTRL = 4,
} USBH_ENDPOINT_TYPE;

/**
    @enum USBH_ENDPOINT_DIR
    @brief USB Endpoint Direction.
 **/
typedef enum 
{
    /** Direction host to device. **/
    USBH_DIR_OUT,
    /** Direction device to host. **/
    USBH_DIR_IN,
	/** Force a SETUP PID to a control endpoint. **/
	USBH_DIR_SETUP,
} USBH_ENDPOINT_DIR;

/**
    @enum USBH_ENDPOINT_SPEED
    @brief USB Endpoint Speed.
 **/
typedef enum
{
    /** Low speed. **/
    USBH_SPEED_LOW,
    /** Full speed. **/
    USBH_SPEED_FULL,
    /** High speed. **/
    USBH_SPEED_HIGH,
} USBH_ENDPOINT_SPEED;

/**
    @typedef USBH_ENDPOINT_SIZE
    @brief USB Endpoint Sizes.
 **/
typedef uint16_t USBH_ENDPOINT_SIZE;

/**
    @typedef   USBH_callback
    @brief     USB callback used when completing a transaction or receiving a
			   notification from the USBH library.
			   It is not permissible to make a call to USBH_transfer_async()
			   and specify a callback function. This will produce unspecified
			   results.
    @param[in] id Identifier for completed transaction
    @param[in] status Status of operation that caused callback
    @param[in] len Size of data buffer
    @param[in] buffer Pointer to data buffer
    @return    USBH_OK if the request was handled successfully.
               USBH_ERR_* depending on function.
 **/
typedef int8_t (*USBH_callback)(uint32_t id, int8_t status, size_t len, uint8_t *buffer);

/** 
    @name Device, Endpoint and Interface Handles.
    @details Handles are used to pass devices, interfaces and endpoints to the 
        application.
 */
//@{ 
/**
    @typedef USBH_device_handle
    @brief Structure that is used to pass a handle to a device to the
        application. It is made up of a pointer to the device structure
        and a fairly unique value to detect enumeration changes and hence
        stale handles.
 **/
typedef uint32_t USBH_device_handle;

/**
    @typedef USBH_interface_handle
    @brief Structure that is used to pass a handle to an interface to the
        application. It is made up of a pointer to the interface structure
        and a fairly unique value to detect enumeration changes and hence
        stale handles.
 **/
typedef uint32_t USBH_interface_handle;

/**
    @typedef USBH_endpoint_handle
    @brief Structure that is used to pass a handle to an endpoint to the
        application. It is made up of a pointer to the endpoint structure
        and a fairly unique value to detect enumeration changes and hence
        stale handles.
 **/
typedef uint32_t USBH_endpoint_handle;
//@}

/**
    @struct USBH_ctx
    @brief Struct containing configuration data for the USB EHCI controller,
	       USBH memory space allocation, callback functions for USB events.
 **/
typedef struct USBH_ctx
{
    /** NOT CURRENTLY IMPLEMENTED.
        Enumeration state callback function. Optional.
		TBD: return a code and maybe a structure to indicate what has 
		changed and how. **/
    USBH_callback enumeration_change;
    /** delay is micro seconds needed for AOA initialization in some
     * android devices. For non-AOA applications this delay can be
     *  configured zero. **/
    uint8_t aoa_compatibility_delay;
    /** Hub configuration. TBD. **/
} USBH_ctx;

/**
    @struct USBH_device_info
    @brief Struct containing current information about a device.
 **/
typedef struct USBH_device_info {
    /** Handle to the parent device of this device. **/
    USBH_device_handle dev;
    /** Port number on parent hub. **/
    uint8_t port_number;
    /** Configured address on USB bus. **/
    uint8_t addr;
    /** Current USB bus speed for device. Definitions in
     * USBH_ENDPOINT_SPEED.
     * 0 - Low speed.
     * 1 - Full speed.
     * 2 - High speed. **/
    uint8_t speed;
    /** Active configuration for this device currently set with
        SET_CONFIGURATION. **/
    uint8_t configuration;
    /** Total number of configurations for this device. **/
    uint8_t num_configurations;
} USBH_device_info;

/**
    @struct USBH_interface_info
    @brief Struct containing current information about an interface.
 **/
typedef struct USBH_interface_info {
    /** Handle to the parent device of this interface. **/
    USBH_device_handle dev;
    /** Interface number from Interface Descriptor. **/
    uint8_t interface_number;
    /** Alternate setting for this interface currently set with
        SET_INTERFACE. **/
    uint8_t alt;
} USBH_interface_info;

/**
    @typedef USBH_endpoint_info
    @brief Struct containing current information about an endpoint.
 **/
typedef struct USBH_endpoint_info  {
    /** Handle to the parent interface of this endpoint. **/
    USBH_interface_handle iface;
    /** Encodes USB endpoint number (0-127). **/
    USBH_ENDPOINT_NUMBER index;
    /** IN or OUT endpoint **/
    USBH_ENDPOINT_DIR direction;
    /** Endpoint max packet size **/
    USBH_ENDPOINT_SIZE max_packet_size;
    /** BULK, ISO, INT or CTRL endpoint **/
    USBH_ENDPOINT_TYPE type;
} USBH_endpoint_info;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** 
    @name USB Library Initialisation
 */
//@{ 
/**
    @brief      Initialise USB hardware.
    @details    Performs a software reset and initialises the USB host hardware.
    			The USBH_ctx contains function pointers to the application for
    			handling USB events. Currently only and enumeration change event
    			is implemented.
                This function MUST be called prior to any further call to the 
                USB functions.
    @param[in]  ctx USB context.
 **/
void USBH_initialise(USBH_ctx *ctx);

/**
    @brief      Finalise USB hardware.
    @details    Releases any resources associated with the USB driver and 
                disables the hardware.
 **/
void USBH_finalise(void);
//@}

/** 
    @name HUB class requests
    @brief Determine and control root and downstream hubs.
 */
//@{ 
/**
    @brief      Return number of ports on specified hub.
    @details    For the hub pointed to by the handle, return the number of ports
                that are available. For the root hub the handle will be NULL.
    @param[in]  hub Handle to hub device.
    @param[out] count Number of ports on hub.
    @return     USBH_OK on success.\n
                USBH_ERR_NOT_FOUND if hub handle is invalid.
 **/
int8_t USBH_get_hub_port_count(USBH_device_handle hub,
                                     uint8_t *count);
/**
    @brief      Return status specified hub.
    @details    For the hub pointed to by the handle, return the status of the
                hub. For the root hub the handle will be NULL.
    @param[in]  hub Handle to hub device.
    @param[out] status Hub status. As described in Table 11-19 and Table 11-20
                of Section 11.24.2.6 in USB Specification. Status in low word
                and change in high word.
    @return     USBH_OK on success.\n
                USBH_ERR_NOT_FOUND if hub handle is invalid.
                USBH_ERR_* an error occurred querying a USB hub.
 **/
int8_t USBH_get_hub_status(USBH_device_handle hub,
                                USB_hub_status *status);
/**
    @brief      Return the status of the specified port on the hub.
    @details    For the hub pointed to by the handle, return the status of the
                numbered port. For the root hub the handle will be NULL.
    @param[in]  hub Handle to hub device.
    @param[in]  port Port number on hub.
    @param[out] status Port status. As described in Table 11-21 of Section
                11.24.2.7 in USB Specification.
    @return     USBH_OK on success.\n
                USBH_ERR_NOT_FOUND if hub handle is invalid.
                USBH_ERR_* an error occurred querying a USB hub.
 **/
int8_t USBH_get_hub_port_status(USBH_device_handle hub,
                                      const uint8_t port, 
                                      USB_hub_port_status *status);

/**
    @brief      Set/Clear Features on hub.
    @details    For the hub pointed to by the handle, send a set or clear
                feature. For the root hub the handle will be NULL.
    			Set or Clear feature operation described in Section
    			11.24.2.12 & 11.24.2.1 of the USB Specification.
    			NOTE: Split from USBH_hub_feature(). 8/12/2014
    @param[in]  hub Handle to hub device.
    @param[in]  feature Port feature. As described in Table 11-17 of Section
                11.24.2 in the USB Specification.
    @return     USBH_OK on success.\n
                USBH_ERR_NOT_FOUND if hub handle is invalid.
                USBH_ERR_* an error occurred sending the request to a
                USB hub.
 **/
//@{
int8_t USBH_hub_set_feature(USBH_device_handle hub,
                              const uint16_t feature);
int8_t USBH_hub_clear_feature(USBH_device_handle hub,
                              const uint16_t feature);
//@}

/**
    @brief      Set/Clear Port Features on hub.
    @details    For the hub pointed to by the handle, send a set or clear port
                feature. For the root hub the handle will be NULL.
    			Set or Clear Port feature operation described in Section
    			11.24.2.13 & 11.24.2.2 of the USB Specification.
    			NOTE: Split from USBH_hub_port_feature(). 8/12/2014
    @param[in]  hub Handle to hub device.
    @param[in]  port Port number on hub.
    @param[in]  feature Port feature. As described in Table 11-17 of Section
                11.24.2 in the USB Specification.
    @return     USBH_OK on success.\n
                USBH_ERR_NOT_FOUND if hub handle is invalid.
                USBH_ERR_* an error occurred sending the request to a
                USB hub.
 **/
//@{
int8_t USBH_hub_set_port_feature(USBH_device_handle hub,
                              const uint8_t port,
                              const uint16_t feature);
int8_t USBH_hub_clear_port_feature(USBH_device_handle hub,
                              const uint8_t port, 
                              const uint16_t feature);
//@}
//@}

/** 
    @name Host Controller Control and Status Commands
    @brief Connection and Enumeration related functions.
 */
//@{ 
/**
    @brief      Determine if a hub port has a downstream connection.
    @details    Select a hub and a port to query.
                For the root hub the handle will be NULL and the port zero.
    @param[in]  hub Handle to hub device.
    @param[in]  port Port number on hub.
    @param[out] state USBH_STATE enumeration for current state of hub port connection.
    @return     USBH_OK if successful.
                USBH_ERR_NOT_FOUND if hub handle is invalid.
                USBH_ERR_* an error occurred sending the request to a
                USB hub.
 **/
int8_t USBH_get_connect_state(USBH_device_handle hub,
                               const uint8_t port,
                               USBH_STATE *state);

/**
    @brief      Get host controller state.
    @details    Get the state of the host controller. This may be used by an
                application to check if the controller is in suspend or
                operational state. There are intermediate states which can
                be found during transitions from operational to suspend and 
                back. Recommended to use explicit state tests, i.e. "if state
                is suspended" rather than "if state is not operational".
    @param[out] state State enum for host.
    @return     USBH_OK if successful.
 **/
int8_t USBH_get_controller_state(USBH_CONTROLLER_STATE *state);

/**
    @brief      Get frame number.
    @details    Get the current frame number. This increments when the host is
                operational and will cease to increment when suspended. This 
                number is sent in the SOF.
    @returns    Frame number (14 bit value). 
 **/
uint16_t USBH_get_frame_number(void);

/**
    @brief      Schedule a re-enumeration of a hub.
    @details    Select a hub to force re-enumeration.
                To enumerate the root hub the handle is set to USBH_ROOT_HUB_HANDLE
                or zero.
                To enumerate all ports on a hub set the port value to
                USBH_HUB_ALL_PORTS or zero.
                NOTE: The USB_process call will monitor the Root hub and
                downstream hubs to manage the connection/removal of devices
                after the initial enumeration is complete.
    @param[in]  hub Handle to hub device.
    @param[in]  port Port on hub.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if hub handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBH_enumerate(USBH_device_handle hub, uint8_t port);
//@}

/** 
    @name Finding Devices
    @brief Discovering Devices after Enumeration.
 */
//@{ 
/**
    @brief      Get device count.
    @details    Get the count of child device enumerated for a device.
    			For devices on the root hub the handle is set to USBH_ROOT_HUB_HANDLE.
    @param[in]  device Count child devices on this device
    @param[out]  count Number of child devices
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
 **/
int8_t USBH_get_device_count(USBH_device_handle device, uint8_t *count);
/**
    @brief      Get device list.
    @details    Get the first child device of a device. The function will return
                a handle to a device if there is one or more child devices.
    			For devices on the root hub the handle is set to USBH_ROOT_HUB_HANDLE.
                If there are no interface then a NULL is returned.
    @param[in]  device Handle to a device.
    @param[out] child Handle to first child device.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
 **/
int8_t USBH_get_device_list(USBH_device_handle device, USBH_device_handle *child);
/**
    @brief      Get next device in list.
    @details    Get the next device in the list. The function will return
                a handle to the device if there are more devices.
                If there are no more devices then a NULL is returned.
    @param[in]  device Handle to a device.
    @param[in]  device Handle to a device.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
 **/
int8_t USBH_get_next_device(USBH_device_handle device, USBH_device_handle *next);
//@}

/** 
    @name Finding Interfaces
    @brief Discovering Interfaces of a Device after Enumeration.
 */
//@{ 
/**
    @brief      Get interface count.
    @details    Get the count of interfaces enumerated for a device.
    @param[in]  device Count interface on this device
    @param[out]  count Number of interfaces on device
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if the device handle is invalid.
 **/
int8_t USBH_get_interface_count(USBH_device_handle device, uint8_t *count);
/**
    @brief      Get interface list.
    @details    Get the first interface of a device. The function will return
                a handle to the interface if there is one or more interfaces.
                If there are no interface then a NULL is returned.
    @param[in]  device Handle to a device.
    @param[out] interface Handle to the first interface.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
 **/
int8_t USBH_get_interface_list(USBH_device_handle device, USBH_interface_handle *interface);
/**
    @brief      Get next interface in list.
    @details    Get the next interface in the list. The function will return
                a handle to the interface if there are more interfaces.
                If there are no more interfaces then a NULL is returned.
    @param[in]  interface Handle to an interface.
    @param[out] next Handle to the next interface.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if interface handle is invalid.
 **/
int8_t USBH_get_next_interface(USBH_interface_handle interface, USBH_interface_handle *next);
//@}

/** 
    @name Finding Endpoints
    @brief Discovering Endpoints of a Device or Interface after Enumeration.
 */
//@{ 
/**
    @brief      Get control endpoint.
    @details    Get the control endpoint of a device. The function will return
                a handle to the control endpoint.
    @param[in]  device Handle to a device.
    @param[out] endpoint Handle to a control endpoint.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
 **/
int8_t USBH_get_control_endpoint(USBH_device_handle device, USBH_endpoint_handle *endpoint);
/**
    @brief      Get endpoint count.
    @details    Get the count of endpoints enumerated for an interface.
    @param[in]  interface Count endpoints on this interface
    @param[out]  count Number of endpoints on interface
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if interface handle is invalid.
 **/
int8_t USBH_get_endpoint_count(USBH_interface_handle interface, uint8_t *count);
/**
    @brief      Get endpoint list.
    @details    Get the first endpoint of an interface. The function will return
                a handle to the endpoint if there is one or more endpoints.
                If there are no endpoint then a NULL is returned.
    @param[in]  interface Handle to an interface.
    @param[out] next Handle to first endpoint.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if interface handle is invalid.
 **/
int8_t USBH_get_endpoint_list(USBH_interface_handle interface, USBH_endpoint_handle *endpoint);
/**
    @brief      Get next endpoint in list.
    @details    Get the next endpoint in the list. The function will return
                a handle to the endpoint if there are more endpoints.
                If there are no more endpoints then a NULL is returned.
    @param[in]  endpoint Handle to an endpoint.
    @param[out] next Handle to next endpoint.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if endpoint handle is invalid.
 **/
int8_t USBH_get_next_endpoint(USBH_endpoint_handle endpoint, USBH_endpoint_handle *next);
//@}

/** 
    @name Querying Device, Interface and Endpoint information.
    @brief Discovering information of devices, interfaces and endpoints.
 */
//@{ 
/**
    @brief      Get device VID and PID.
    @details    Get the VID and PID of a device.
    @param[in]  device Handle to a device.
    @param[out] vid Vendor ID value from Device Descriptor.
    @param[out] pid Product ID value from Device Descriptor.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
 **/
 int8_t USBH_device_get_vid_pid(USBH_device_handle device,
                                     uint16_t *vid, 
                                     uint16_t *pid);
/**
    @brief      Get interface class, subclass and protocol.
    @details    Get the class information of an interface.
    @param[in]  interface Handle to an interface.
    @param[out] devClass USB class value for the interface.
    @param[out] devSubclass USB subclass value for the interface.
    @param[out] devProtocol USB protocol value for the interface.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if interface handle is invalid.
 **/
int8_t USBH_interface_get_class_info(USBH_interface_handle interface,
                                           uint8_t *devClass,
                                           uint8_t *devSubclass,
                                           uint8_t *devProtocol);

/**
    @brief      Get device information.
    @details    Get information of a device.
    @param[in]  device Handle to a device.
    @param[out] info Structure to receive device information.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
 **/
int8_t USBH_device_get_info(USBH_device_handle device,
                                  USBH_device_info *info);

/**
    @brief      Get interface information.
    @details    Get information of an interface.
    @param[in]  interface Handle to an interface.
    @param[out] info Structure to receive interface information.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if interface handle is invalid.
 **/
int8_t USBH_interface_get_info(USBH_interface_handle interface,
                                  USBH_interface_info *info);

/**
    @brief      Get endpoint information.
    @details    Get information of an endpoint.
    @param[in]  endpoint Handle to an endpoint.
    @param[out] info Structure to receive endpoint information.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if endpoint handle is invalid.
 **/
int8_t USBH_endpoint_get_info(USBH_endpoint_handle endpoint,
                                    USBH_endpoint_info *info);
//@}

/** 
    @name Low level control of interfaces and endpoints.
    @brief Create requests to modify the behaviour of devices, interfaces and 
        endpoints.
 */
//@{ 
/**
    @brief      Set the alternate configuration of an interface.
    @details    Sends a SET_INTERFACE request to an interface.
    			This function is currently NOT IMPLEMENTED.
    @param[in]  interface Handle to an interface.
    @param[in]  alt Alternate setting to send.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if interface handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBH_set_interface(USBH_interface_handle interface,
                                const uint8_t alt);
/**
    @brief      Gets the current configuration value of a device.
    @details    Sends a GET_CONFIGURATION request to a device.
                NOTE: Not strictly ever required.
    @param[in]  device Handle to a device.
    @param[out] conf Current configuration value.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBH_device_get_configuration(USBH_device_handle device,
                                           uint8_t *conf);
/**
    @brief      Sets the current configuration value of a device.
    @details    Sends a SET_CONFIGURATION request to a device.
                NOTE: Should strictly only be done during enumeration.
    @param[in]  device Handle to a device.
    @param[in]  conf New configuration value.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if the device handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBH_device_set_configuration(USBH_device_handle device,
                                           const uint8_t conf);

/**
    @brief      Get a descriptor from a device.
    @details    Sends a GET_DESCRIPTOR request to a device.
    @param[in]  device Handle to a device.
    @param[in]  type Configuration descriptor type.
    @param[in]  index Index of descriptor.
    @param[in]  len Configuration descriptor len (or number of bytes to read).
    @param[in]  buf Location to copy descriptor into.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBH_device_get_descriptor(USBH_device_handle device,
		uint8_t type, uint8_t index, size_t len, uint8_t *buf);

/**
    @brief      Sets or clears an endpoint halt feature request to an endpoint.
    @details    Sends a SET_FEATURE request to a endpoint.
    @param[in]  endpoint Handle to an endpoint.
    @param[in]  request Set or Clear Port feature. Described in Table 9-4 in
                Section 9.4 of the USB Specification.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if endpoint handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBH_endpoint_halt(USBH_endpoint_handle endpoint,
                                const uint8_t request);
/**
    @brief      Sets or clears a remote wakeup feature request to a device.
    @details    Sends a SET_FEATURE request to a device.
    			This function is currently NOT IMPLEMENTED.
    @param[in]  device Handle to an device.
    @param[in]  request Set or Clear Port feature. Described in Table 9-4 in
                Section 9.4 of the USB Specification.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBH_device_remote_wakeup(USBH_device_handle device,
                                       const uint8_t request);
//@}

/** 
    @name Low level control of host controller.
    @brief Modify the behaviour of the host's connection to devices, interfaces and 
        endpoints.
 */
//@{ 
/**
    @brief      Set the host controller to stop an endpoint.
    @details    Instruct the USB host controller to halt an endpoint.
                This can only be used on BULK, INTERRUPT and CONTROL endpoint.
    @param[in]  endpoint Handle to an endpoint.
    @returns    USBH_OK if successful.
    			USBH_ERR_REMOVED endpoint removed.
    			USBH_ERR_PARAMETER endpoint handle invalid.
    			USBH_ERR_USBERR attempt to access an ISOCHRONOUS endpoint.
                USBH_ERR_NOT_FOUND if endpoint handle is invalid.
 **/
int8_t USBH_interface_set_host_halt(USBH_endpoint_handle endpoint);

/**
    @brief      Clear a halted flag on an endpoint in the host controller.
    @details    Modify the ECHI endpoint data to remove the halt flag from
                a queue for an endpoint.
                This can only be used on BULK, INTERRUPT and CONTROL endpoint.
    @param[in]  endpoint Handle to an endpoint.
    @returns    USBH_OK if successful.
    			USBH_ERR_REMOVED endpoint removed.
    			USBH_ERR_PARAMETER endpoint handle invalid.
    			USBH_ERR_USBERR attempt to access an ISOCHRONOUS endpoint.
                USBH_ERR_NOT_FOUND if endpoint handle is invalid.
 **/
int8_t USBH_interface_clear_host_halt(USBH_endpoint_handle endpoint);

/**
    @brief      Set the host controller to clear all transfers for an endpoint.
    @details    Clear the ECHI transaction queue for an endpoint.
                All transfers will be removed from the queue and invalidated.
    @param[in]  endpoint Handle to an endpoint.
    @returns    USBH_OK if successful.
    			USBH_ERR_REMOVED endpoint removed.
 **/
int8_t USBH_clear_endpoint_transfers(USBH_endpoint_handle endpoint);
//@}

/** 
    @name USB Transactions
    @brief Functions to transfer data to or from Endpoints.
 */
//@{ 
/**
    @brief      Transfer data to/from a USB control endpoint.
    @details    USB \p IN or \p OUT request is implied from the \p req 
                parameter.
                The length of the transfer is implied from the dwLength
                member of the USB_device_request structure.
                For \p IN transfers, length is the number of bytes to be sent.
                For \p OUT transfers, length is the maximum number of bytes to
                       read.
    @param[in]  device Device to address.
    @param[in]  req USB Device Request to send in SETUP token.
    @param[in]  buffer Appropriately sized buffer for the transfer.
    @param[in]  timeout Number of milliseconds to wait for response.
    @returns    Number of bytes transferred if successful. (i.e. >= 0)
                USBH_ERR_NOT_FOUND if device handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int32_t USBH_device_setup_transfer(USBH_device_handle device,
                                   USB_device_request *req,
                                   uint8_t *buffer,
								   int16_t timeout);
/**
    @brief      Transfer data to/from a USB endpoint.
    @details    USB \p IN or \p OUT request is implied from the \p ep 
                parameter. This is a blocking call to complete a transaction.
    @param[in]  endpoint Endpoint to address.
    @param[in]  buffer Appropriately sized buffer for the transfer.
    @param[in]  length For \p IN transfers, the number of bytes to be sent.
                       For \p OUT transfers, the maximum number of bytes to 
                       read.
    @param[in]  timeout Number of milliseconds to wait for response.
    @returns    Number of bytes transferred if successful. (i.e. >= 0)
                USBH_ERR_NOT_FOUND if endpoint handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int32_t USBH_transfer(USBH_endpoint_handle endpoint,
                      uint8_t *buffer,
                      const size_t length,
					  uint16_t timeout);

/**
    @brief      Transfer data to/from a USB endpoint. (EXPERIMENTAL)
    @details    USB \p IN or \p OUT request is implied from the \p ep
                parameter. This is a non-blocking call to queue multiple
                transactions on an endpoint until a USBH_transfer is used
                to start the transfer queue.
    			This function is currently NOT IMPLEMENTED.
    @param[in]  endpoint Endpoint to address.
    @param[in]  buffer Appropriately sized buffer for the transfer.
    @param[in]  length For \p IN transfers, the number of bytes to be sent.
                       For \p OUT transfers, the maximum number of bytes to
                       read.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if endpoint handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBH_queue_transfer(USBH_endpoint_handle endpoint,
                      uint8_t *buffer,
                      const size_t length); /* EXPERIMENTAL */


/**
    @brief      Asynchronously transfer data to/from a USB endpoint.
    @details    USB \p IN or \p OUT request is implied from the \p ep 
                parameter. This is a blocking call to complete a transaction.
    @param[in]  endpoint Endpoint to address.
    @param[in]  buffer Appropriately sized buffer for the transfer.
    @param[in]  length For \p IN transfers, the number of bytes to be sent.
                       For \p OUT transfers, the maximum number of bytes to 
                       read.
    @param[in]  timeout Number of milliseconds to wait for response. Zero for
				infinite timeout.
	@param[in]  id Identifier for asynchronous transaction. Passed to the
	            callback function.
	@param[in]	cb Callback function to notify application of completion of
				asynchronous transfer. 
				Parameters for callback function are defined in the 
				USBH_callback typedef. The status of the transaction and any
				pending data (from an IN) will be returned to the callback
				function.
				The function must return with minimum processing. When it 
				returns the USB_xfer structure is discarded and invalidated.
				It is not permissible to make further calls to this function
				from with the callback function.This will produce unspecified
			    results. SETUP and blocking calls are allowed but may have
			    a performance penalty on application code.
    @returns    No callback returns the number of bytes transferred if
                successful. (i.e. >= 0)
                With callback USBH_OK if successful.
                USBH_ERR_NOT_FOUND if endpoint handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int32_t USBH_transfer_async(USBH_endpoint_handle endpoint,
                            uint8_t       *buffer,
							const size_t length,
							uint16_t      timeout,
							uint32_t      id,
							USBH_callback cb
							);
//@}

                  
/**
    @brief      USB process
    @details    To be continuously called by the user application. Checks
				for asynchronous transfer completions and root hub events.
				When a root hub connection is detected then the enumeration
				routine is called automatically.
    @param[in]  Nothing
    @return     Non-zero if USB transaction has been processed.
 **/                      
int8_t USBH_process(void);

/**
    @brief      USB timer
    @details    To be called every millisecond from an interrupt handler to
	            provide timeout support for USB host transactions.
				This will check all pending transfers, decrement
				timeout values and expire any timed out transactions.
 **/                      
void USBH_timer(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // FT900_USBH_H_
