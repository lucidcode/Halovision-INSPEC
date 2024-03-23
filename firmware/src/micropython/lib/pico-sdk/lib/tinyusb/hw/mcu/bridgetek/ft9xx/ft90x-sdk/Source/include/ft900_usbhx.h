/**
    @file ft900_usbhx.h

    @brief
    USB host API extensions.

	@details
    API functions for extensions to the USB Host stack. These functions
    provide additional functionality useful to implement a USB Host application.
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

#ifndef FT900_USBHX_H_
#define FT900_USBHX_H_

#include "ft900_usbh.h"

/* INCLUDES ************************************************************************/

/* CONSTANTS ***********************************************************************/


/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/**
    @brief      Find the first device with a specific VID and PID.
    @details    Get the VID and PID of a device.
    @param[in]  phDev Handle to a device.
    @param[out] usbVid Vendor ID value from Device Descriptor.
    @param[out] usbPid Product ID value from Device Descriptor.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
 **/
int8_t USBHX_find_by_vid_pid(USBH_device_handle *phDev,
		uint16_t usbVid, uint16_t usbPid);

/**
    @brief      Get interface class, subclass and protocol.
    @details    Get the class information of an interface.
    @param[in]  phDev Handle to a device.
    @param[in]  phInterface Handle to an interface.
    @param[out] usbClass USB class value for interface.
    @param[out] usbSubclass USB subclass value for interface.
    @param[out] usbProtocol USB protocol value for interface.
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if interface handle is invalid.
 **/
int8_t USBHX_find_by_class(USBH_device_handle *phDev, USBH_interface_handle *phInterface,
		uint8_t usbClass, uint8_t usbSubclass, uint8_t usbProtocol);

/**
    @brief      Get a partial descriptor from a device.
    @details    Sends a GET_DESCRIPTOR request to a device and returns a
    			section of the data.
    @param[in]  device Handle to a device.
    @param[in]  type Configuration descriptor type.
    @param[in]  index Index of descriptor.
    @param[in]	offset Start position in descriptor to read.
    @param[in]  len Number of bytes to read from position "offset".
    @param[in]  buf Location to copy descriptor into (must be minimum size of "len").
    @returns    USBH_OK if successful.
                USBH_ERR_NOT_FOUND if device handle is invalid.
                USBH_ERR_RESOURCES if there are insufficient resources.
                USBH_ERR_* depending on USB bus errors.
 **/
int8_t USBHX_get_config_descriptor(USBH_device_handle device,
		uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf);

/**
    @brief      Waits for a connection to the root hub and enumerates the device.
    @details    Will block until a device is connected to the root hub and then
    			proceed to enumerate it and any downstream devices. Once this
    			is complete then it will check the enumeration result and return.
    			Will never return USBH_STATE_NOTCONNECTED.
    @returns    USBH_STATE_CONNECTED - a device is connected but there was a
    				general failure to enumerate.
    			USBH_STATE_ENUMERATED - Device connected and enumerated properly.
    			USBH_STATE_ENUMERATED_PARTIAL - Device connected and enumeration
    				started. Enumeration did not complete so some devices,
    				interfaces or endpoints may be missing.
 **/
USBH_STATE USBHX_enumerate_wait(void);

/**
    @brief      Tests if a device is connected to the root hub.
    @returns    zero - No device connected.
    			non-zero - A device is connected but may not be enumerated.
 **/
int8_t USBHX_root_connected(void);

/**
    @brief      Tests if a device is connected to the root hub and enumerated.
    @returns    zero - No device enumerated.
    			non-zero - A device is connected and enumerated. The device can
    			be used with the USBH driver.
 **/
int8_t USBHX_root_enumerated(void);

/**
    @brief      Tests if the enumeration worked correctly.
    @returns    zero - Device(s) enumerated correctly.
    			non-zero - No device connected, a device is connected but not
    			enumerated or the device may have not been enumerated
    			completely.
 **/
int8_t USBHX_root_enumeration_failed(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBHX_H_ */
