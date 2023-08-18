/**
    @file ft900_usbd_dfu.h

    @brief
    DFU device for USB device stack API.

    API functions for USB Device DFU interfaces. These functions provide functionality
    required to communicate with a DFU application through the USB Device interface.

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

#ifndef __FT900_USBD_DFU_H__
#define __FT900_USBD_DFU_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <ft900_usb_dfu.h>

/**
    @def DFU_MAX_BLOCK_SIZE
    @brief Set the maximum block size for a packet of data from
     the host in a DFU_DNLOAD request.
     A packet sent to the device for programming or a request for
     data frm the Flash cannot ever cross a page boundary.
     The maximum block size is therefore the page size. All requests
     for data must be entirely within a page.
 */
#define USBD_DFU_MAX_BLOCK_SIZE 256

/**
    @def DFU_TIMEOUT
    @brief Suggested timeout for DFU downloads.
 */
#define USBD_DFU_TIMEOUT 0x2000

/**
 @name USBD_DFU_ATTRIBUTES
 @brief Determines the parts of the DFU specification which are supported
        by this library.
        These can be a combination of:
        USB_DFU_BMATTRIBUTES_CANDNLOAD - Firmware download (program device).
        USB_DFU_BMATTRIBUTES_CANUPLOAD - Firmware upload (read device).
        USB_DFU_BMATTRIBUTES_WILLDETACH - Device perform a detach and
            attach sequence to change into DFU mode. If this is not
            supported then the host must reset the USB after a detach
            request. This will cause the device to reset and enter DFU mode.
        USB_DFU_BMATTRIBUTES_MANIFESTATIONTOLERANT - Continue to allow
        	DFU requests after a download has completed. This is not
        	turned on to force the default behaviour to be that the
        	device will run any new firmware after it is downloaded.
        Un-defining attributes will remove support for that feature
        from the library. Firmware may not need all the features
        and can disable them in the configuration descriptor.
 */
//@{
#ifndef USBD_DFU_ATTRIBUTES
#define USBD_DFU_ATTRIBUTES (USB_DFU_BMATTRIBUTES_CANDNLOAD |\
		USB_DFU_BMATTRIBUTES_WILLDETACH |\
		USB_DFU_BMATTRIBUTES_CANUPLOAD)
#endif // USBD_DFU_ATTRIBUTES
//@}

/**
 @brief     Determine current mode of DFU
 @return    Returns non-zero if the DFU state is runtime mode.
 **/
uint8_t USBD_DFU_is_runtime(void);

/**
 @brief     Determine if DFU waiting to reset
 @return    Returns non-zero if the DFU state machine is in
            dfuMANIFEST-WAIT-RESET and is therefore waiting
            for a host reset or detach/attach sequence.
            If the bmAttributes value set in the USBD_DFU_ATTRIBUTES
 	 	 	does support manifestation then this function should
 	 	 	not be required.
 **/
uint8_t USBD_DFU_is_wait_reset(void);

/**
 @brief     Force a transition into DFU mode.
 **/
void USBD_DFU_set_dfumode(void);

/**
 @brief      USB class request handler for DFU_DETACH

 @details    Move the state machine to appDETACH state from
             appIDLE and initialise a timeout within which
             time the host should set a USB reset on the bus.
             An ACK packet is sent on the USB control IN endpoint
             to the host to acknowledge successful completion of
             this request.
 	 	 	 The bmAttributes value set in the USBD_DFU_ATTRIBUTES
 	 	 	 determines the actions that are taken upon a detach.
 @param[in]  timeout - number of milliseconds timeout before reverting
             to appIDLE if no USB reset is forthcoming from the host.
 **/
void USBD_DFU_class_req_detach(uint16_t timeout);

/**
 @brief      USB class request handler for DFU_DNLOAD
 @details    Receive blocks of firmware from the host on the control
             OUT endpoint and program these into the MTP.
             If the state machine is in the dfuIDLE state then move to
             dfuDNLOAD_IDLE state.
             If zero length data is received indicating the end of
             the firmware then move the state machine to
             dfuMANIFEST_WAIT_RESET.
             If an address or data length error are detected then
             move to the dfuERROR state.
             An ACK packet is sent on the USB control IN endpoint
             to the host to acknowledge successful completion of
             this request.
             If the bmAttributes value set in the USBD_DFU_ATTRIBUTES
 	 	 	 does not support download then this function will have
 	 	 	 no body.
 @param[in]  address - starting address of data to program.
             It is up to the calling program to make sure this
             is calculated correctly.
 @param[in]  dataLength - Number of bytes to program. This can be
             between the control endpoint max packet size and
             DFU_MAX_BLOCK_SIZE.
 **/
void USBD_DFU_class_req_download(uint32_t block, uint16_t dataLength);

/**
 @brief      USB class request handler for DFU_UPLOAD
 @details    Receive blocks of firmware from the Flash to the control
             IN endpoint.
             If the state machine is in the dfuIDLE state then move to
             dfuUPLOAD_IDLE state.
             If an address or data length error are detected then
             move to the dfuERROR state.
             An ACK packet is sent on the USB control IN endpoint
             to the host to acknowledge successful completion of
             this request.
             If the bmAttributes value set in the USBD_DFU_ATTRIBUTES
 	 	 	 does not support upload then this function will have
 	 	 	 no body.
 @param[in]  address - starting address of data to read.
             It is up to the calling program to make sure this
             is calculated correctly.
 @param[in]  dataLength - Number of bytes to read. This can be
             between the control endpoint max packet size and
             DFU_MAX_BLOCK_SIZE.
 **/
void USBD_DFU_class_req_upload(uint32_t block, uint16_t dataLength);

/**
 @brief      USB class request handler for DFU_GETSTATUS
 @details    Return a structure to the host containing the
             current DFU state machine and status bytes. These
             are used by the application on the host to work out
             whether any errors have occurred and what the
             status of the device is.
             The structure is written via the control IN
             endpoint to the host.
 	 	 	 The bmAttributes value set in the USBD_DFU_ATTRIBUTES
 	 	 	 determines the actions that are taken upon a GET_STATUS.
 @param		 requestLen - number of bytes requested by the host.
 **/
void USBD_DFU_class_req_getstatus(uint16_t requestLen);

/**
 @brief      USB class request handler for DFU_CLRSTATUS
 @details    Clears an error state for the DFU state machine.
 **/
void USBD_DFU_class_req_clrstatus(void);

/**
 @brief      USB class request handler for DFU_ABORT
 @details    Aborts transaction and resets the DFU state machine.
 **/
void USBD_DFU_class_req_abort(void);

/**
 @brief      USB class request handler for DFU_GETSTATE
 @details    Return a single byte to the host containing the
             current DFU state machine byte.
             The data is written via the control IN endpoint
             to the host.
 @param		 requestLen - number of bytes requested by the host.
 **/
void USBD_DFU_class_req_getstate(uint16_t requestLen);

/**
 @brief      Implementation of USB reset state handler for DFU.
 @details    Move Reset or advance the DFU state machine when a
 	 	 	 USB reset is encountered. This will change the state
 	 	 	 to dfuIDLE if it was in appDETACH state before. It will
 	 	 	 change to dfuERROR if a download was in progress.
 	 	 	 Otherwise it will return to appIDLE.
             Return a  byte to the host indicating if the next state
             change of the DFU state machine byte requires code
             to be reloaded and run. i.e. a new program needs to
             be run.
 	 	 	 The bmAttributes value set in the USBD_DFU_ATTRIBUTES
 	 	 	 determines the actions that are taken upon a reset.
 @return     status - non-zero if new program is to be run.
 **/
uint8_t USBD_DFU_reset(void);

/**
 @brief      Decrements the detach_counter and adjusts state accordingly.
 @details    If the state is appDETACH moves to dfuIDLE state if we
 	 	 	 have been in the appDETACH state for longer than the
 	 	 	 attach timeout specified by the DFU_DETACH request.

             Note: This is run from INTERRUPT LEVEL as a handler for an ISR.
 	 	 	 The bmAttributes value set in the USBD_DFU_ATTRIBUTES
 	 	 	 determines the actions that are taken upon a timer event (i.e.
 	 	 	 may call a detach).
 @param		 attributes - the bmAttributes value set in the DFU
 	 	 	 functional descriptor. This determines the actions
 	 	 	 that are taken upon a reset.
 @return     Zero if timer running, non-zero if timer expired.
 **/
uint8_t USBD_DFU_timer(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __FT900_USBD_DFU_H__
