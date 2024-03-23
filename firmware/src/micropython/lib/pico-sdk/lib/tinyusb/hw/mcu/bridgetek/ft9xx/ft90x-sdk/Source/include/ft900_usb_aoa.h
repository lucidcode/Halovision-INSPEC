/**
    @file ft900_usb_aoa.h

    @brief
    USB AOA class USB definitions.

	@details
    This contains USB definitions of constants and structures which are used in
    USB AOA implementations.

    Definitions for USB AOA Devices based on Android Open Accessory Version 2.0
    from http://source.android.com/accessories/aoa2.html
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

#ifndef FT900_USB_AOA_H_
#define FT900_USB_AOA_H_

/* INCLUDES ************************************************************************/
#include "ft900_usb.h"

/* CONSTANTS ***********************************************************************/

// Definitions obtained from http://developer.android.com/guide/topics/usb/adk.html

// Android Accessory interfaces have
//	interface class USB_CLASS_VENDOR (0xff)
//	interface subclass USB_SUBCLASS_VENDOR_VENDOR (0xff)
//	interface protocol USB_PROTOCOL_VENDOR_NONE (0x00)

/**
    @name VID and PID definitions for Android Accessories
    @brief
 */
//@{
#define USB_VID_GOOGLE					        		0x18D1
#define USB_PID_ANDROID_ACCESSORY_AVAILABLE        		0x2D00
#define USB_PID_ANDROID_ACCESSORY_ADB_AVAILABLE         0x2D01
#define USB_PID_ANDROID_AUDIO_AVAILABLE                 0x2D02
#define USB_PID_ANDROID_AUDIO_ADB_AVAILABLE             0x2D03
#define USB_PID_ANDROID_ACCESSORY_AUDIO_AVAILABLE       0x2D04
#define USB_PID_ANDROID_ACCESSORY_AUDIO_ADB_AVAILABLE   0x2D05
//@}

/**
    @name USB AOA Support Bitmap
    @brief Bitmap describing the types of AOA accessories that the AOA
    	device supports.
 */
//@{
#define USB_AOA_SUPPORT_MASK_ACCESSORY 	0x01
#define USB_AOA_SUPPORT_MASK_AUDIO		0x02 // AOA 2.0 and above
#define USB_AOA_SUPPORT_MASK_ADB		0x80
//@}

/**
    @name USB AOA Request Codes
    @brief
 */
//@{
#define USB_AOA_REQUEST_CODE_GET_PROTOCOL 0x33
#define USB_AOA_REQUEST_CODE_SEND_STRING	0x34
#define USB_AOA_REQUEST_CODE_START		0x35
#define USB_AOA_REQUEST_SET_AUDIO_MODE	0x3A
#define USB_AOA_REQUEST_REGISTER_HID		0x36
#define USB_AOA_REQUEST_UNREGISTER_HID	0x37
#define USB_AOA_REQUEST_SET_HID_REPORT_DESC	0x38
#define USB_AOA_REQUEST_SEND_HID_EVENT	0x39
//@}

/**
    @name Android Accessory Audio Modes
    @brief Defines the mode of the audio stream passed from the
     AOA device to the host.
 */
//@{
#define USB_AOA_SET_AUDIO_MODE_NONE		0
#define USB_AOA_SET_AUDIO_MODE_16BIT_PCM_2CH_44100KHz	1
//@}

/**
    @name Android Accessory string indices
    @brief
 */
//@{
#define USB_AOA_STRING_MANUFACTURER		0
#define USB_AOA_STRING_MODEL			1
#define USB_AOA_STRING_DESCRIPTION		2
#define USB_AOA_STRING_VERSION			3
#define USB_AOA_STRING_URI				4
#define USB_AOA_STRING_SERIAL_NUMBER	5
//@}

/**
    @name Android Accessory protocol versions
    @brief
 */
//@{
#define USB_AOA_PROTOCOL_1 0x0001
#define USB_AOA_PROTOCOL_2 0x0002
//@}

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USB_AOA_H_ */
