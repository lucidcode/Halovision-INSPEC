/**
    @file ft900_usbh_hid.h

    @brief 
    HID devices on USB host stack API.
  
    API functions for USB Host HID devices. These functions provide functionality
    required to communicate with a HID device through the USB Host interface.
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

#ifndef FT900_USBH_HID_H_
#define FT900_USBH_HID_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/**
 *  @brief HID Context which stores details of HID device
 * Holds a context structure required by each instance of the driver.
 */
typedef struct USBH_HID_context {
    USBH_device_handle hHIDDevice; /// USB host device handle for HID device.
    USBH_interface_handle hHIDInterface; /// USB host interface handle for HID device.
    uint8_t hidInterfaceNumber; /// Interface number for HID device.
    USBH_endpoint_handle hHIDEpIn, hHIDEpOut; /// USB host endpoint handles for HID device IN (and OUT) endpoints.
    uint8_t reportInSize, reportOutSize; /// Report sizes in bytes for IN and OUT reports.
} USBH_HID_context;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/**
    @brief      Initialise HID device.
    @details    Initialises the instance of the HID device and stores information in
                the USBH_HID_context structure passed from the application. This allows
                individual instances of the HID device to be accessed independently.
    @param[in]  hHIDDev Handle of HID device on USB bus.
    @param[in]  hHIDInterface Handle of interface on hHIDDev to use for driver. There
                may be more than one HID interface on a device.
    @param[out] ctx Pointer to HID context.
    @returns    Zero if successful, non-zero if not.
  **/
int8_t USBH_HID_init(USBH_device_handle hHIDDev, USBH_interface_handle hHIDInterface, USBH_HID_context *ctx);

/**
    @brief      Send a SET IDLE request to the HID device.
    @details    Forms and sends a SET IDLE request to the control endpoint of the
                HID device. The interface number of the HID interface is included to
                tell the device which of possible multiple interfaces to idle.
    @param[in]  ctx Pointer to HID context.
    @param[in]  idle Timeout for idle, zero is infinite.
    @returns    Zero if successful, non-zero if not.
  **/
int8_t USBH_HID_set_idle(USBH_HID_context *ctx, uint16_t idle);

/**
    @brief      Gets the IN report size for the HID device.
    @details    Allows the application to discover the size of reports sent by the
                HID device.
    @param[in]  ctx Pointer to HID context.
    @returns    Zero if fail, non-zero for report size.
  **/
int8_t USBH_HID_get_report_size_in(USBH_HID_context *ctx);

/**
    @brief      Gets the OUT report size for the HID device.
    @details    Allows the application to discover the size of reports to send to the
                HID device.
    @param[in]  ctx Pointer to HID context.
    @returns    Zero if fail, non-zero for report size.
  **/
int8_t USBH_HID_get_report_size_out(USBH_HID_context *ctx);

/**
    @brief      Gets an IN report from the HID device.
    @details    Returns a report from the device's IN endpoint into the buffer
                pointed to in the parameters. The buffer must be large enough
                for the number of bytes returned in USBH_HID_get_report_size_in()
    @param[in]  ctx Pointer to HID context.
    @param[out] buffer Buffer to receive data.
    @returns    Zero if successful, non-zero if not.
  **/
int8_t USBH_HID_get_report(USBH_HID_context *ctx, uint8_t *buffer);

/**
    @brief      Sends an OUT report to the HID device.
    @details    Transmits a report to the device's OUT endpoint from the buffer
                pointed to in the parameters. The buffer must contain at least
                the number of bytes returned in USBH_HID_get_report_size_out()
    @param[in]  ctx Pointer to HID context.
    @param[out] buffer Buffer providing data.
    @returns    Zero if successful, non-zero if not.
  **/
int8_t USBH_HID_set_report(USBH_HID_context *ctx, uint8_t *buffer);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBH_HID_H_ */
