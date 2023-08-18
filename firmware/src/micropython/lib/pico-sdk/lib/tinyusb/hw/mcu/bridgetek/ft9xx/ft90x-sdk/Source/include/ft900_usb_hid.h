/**
    @file ft900_usb_hid.h

    @brief 
    USB HID class definitions.
  
	@details
    This contains USB definitions of constants and structures which are used in
    USB HID implementations.

    Definitions for USB HID Devices based on USB Device Class Definitions for
    HID Revision 1.11 from http://www.usb.org/developers/docs/

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

#ifndef FT900_USB_HID_H_
#define FT900_USB_HID_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* INCLUDES ************************************************************************/

#include <ft900_usb.h>

/* CONSTANTS ***********************************************************************/

/** @name USB HID Specification
 */
//@{
/**
    @name USB HID Descriptor Types
    @brief Section 7.1. Descriptor Types for HID.
    Used for wValue high byte in 9.4.3 Get Descriptor, 9.4.8 Set Descriptor.
 **/
//@{
#define USB_DESCRIPTOR_TYPE_HID			  0x21
#define USB_DESCRIPTOR_TYPE_REPORT		  0x22
#define USB_DESCRIPTOR_TYPE_PHYSICAL	  0x23
//@}

/**
    @name USB HID Request Codes
    @brief Section 7.1. HID Request Codes.
        Used as bRequest in USB_device_request and bDescriptorType
        in USB_device_descriptor.
 */
//@{
#define USB_HID_REQUEST_CODE_GET_REPORT	  0x01
#define USB_HID_REQUEST_CODE_GET_IDLE	  0x02
#define USB_HID_REQUEST_CODE_GET_PROTOCOL 0x03
#define USB_HID_REQUEST_CODE_SET_REPORT	  0x09
#define USB_HID_REQUEST_CODE_SET_IDLE	  0x0A
#define USB_HID_REQUEST_CODE_SET_PROTOCOL 0x0B
//@}

/**
    @name   Bridgetek USB HID Version Information
    @brief  Definitions of USB HID Version BCD codes.
 */
//@{
/// Version 1.0 (deprecated)
#define USB_BCD_VERSION_HID_1_0 0x0100
/// Version 1.1
#define USB_BCD_VERSION_HID_1_1 0x0110
//@}

/**
    @name USB HID Language Codes
    @brief Section 6.2.1. HID Language Codes.
 */
//@{
#define USB_HID_LANG_NOT_SUPPORTED		 0
#define USB_HID_LANG_ARABIC				 1
#define USB_HID_LANG_BELGIAN			 2
#define USB_HID_LANG_CANADIAN_BILINGUAL	 3
#define USB_HID_LANG_CANADIAN_FRENCH	 4
#define USB_HID_LANG_CZECH_REPUBLIC		 5
#define USB_HID_LANG_DANISH				 6
#define USB_HID_LANG_FINNISH			 7
#define USB_HID_LANG_FRENCH				 8
#define USB_HID_LANG_GERMAN				 9
#define USB_HID_LANG_GREEK				 10
#define USB_HID_LANG_HEBREW				 11
#define USB_HID_LANG_HUNGARY			 12
#define USB_HID_LANG_INTERNATIONAL		 13
#define USB_HID_LANG_ITALIAN			 14
#define USB_HID_LANG_JAPAN				 15
#define USB_HID_LANG_KOREAN				 16
#define USB_HID_LANG_LATIN_AMERICAN		 17
#define USB_HID_LANG_NETHERLANDS		 18
#define USB_HID_LANG_NORWEGIAN			 19
#define USB_HID_LANG_PERSIAN			 20
#define USB_HID_LANG_POLAND				 21
#define USB_HID_LANG_PORTUGUESE			 22
#define USB_HID_LANG_RUSSIA				 23
#define USB_HID_LANG_SLOVAKIA			 24
#define USB_HID_LANG_SPANISH			 25
#define USB_HID_LANG_SWEDISH			 26
#define USB_HID_LANG_SWISS_FRENCH		 27
#define USB_HID_LANG_SWISS_GERMAN		 28
#define USB_HID_LANG_SWITZERLAND		 29
#define USB_HID_LANG_TAIWAN				 30
#define USB_HID_LANG_TURKISH_Q			 31
#define USB_HID_LANG_UK					 32
#define USB_HID_LANG_US					 33
#define USB_HID_LANG_YUGOSLAVIA			 34
#define USB_HID_LANG_TURKISH_F			 35
//@}

/**
    @name USB HID Physical Descriptor Bias
    @brief Section 6.2.3. HID Physical Descriptor Bias Values
 */
//@{
#define USB_HID_BIAS_NOT_APPLICABLE		 0x00
#define USB_HID_BIAS_RIGHT_HAND			 0x10
#define USB_HID_BIAS_LEFT_HAND			 0x20
#define USB_HID_BIAS_BOTH_HANDS			 0x30
#define USB_HID_BIAS_EITHER_HAND		 0x40
//@}

/**
    @name USB HID Physical Descriptor Bias
    @brief Section 6.2.3. HID Physical Descriptor Designator Values
 */
//@{
#define USB_HID_DESIGNATOR_NONE			 0x00
#define USB_HID_DESIGNATOR_HAND			 0x01
#define USB_HID_DESIGNATOR_EYEBALL		 0x02
#define USB_HID_DESIGNATOR_EYEBROW		 0x03
#define USB_HID_DESIGNATOR_EYELID		 0x04
#define USB_HID_DESIGNATOR_EAR			 0x05
#define USB_HID_DESIGNATOR_NOSE			 0x06
#define USB_HID_DESIGNATOR_MOUTH		 0x07
#define USB_HID_DESIGNATOR_UPPER_LIP	 0x08
#define USB_HID_DESIGNATOR_LOWER_LIP	 0x09
#define USB_HID_DESIGNATOR_JAW			 0x0A
#define USB_HID_DESIGNATOR_NECK			 0x0B
#define USB_HID_DESIGNATOR_UPPER_ARM	 0x0C
#define USB_HID_DESIGNATOR_ELBOW		 0x0D
#define USB_HID_DESIGNATOR_FOREARM		 0x0E
#define USB_HID_DESIGNATOR_WRIST		 0x0F
#define USB_HID_DESIGNATOR_PALM			 0x10
#define USB_HID_DESIGNATOR_THUMB		 0x11
#define USB_HID_DESIGNATOR_INDEX_FINGER	 0x12
#define USB_HID_DESIGNATOR_MIDDLE_FINGER 0x13
#define USB_HID_DESIGNATOR_RING_FINGER	 0x14
#define USB_HID_DESIGNATOR_LITTLE_FINGER 0x15
#define USB_HID_DESIGNATOR_HEAD			 0x16
#define USB_HID_DESIGNATOR_SHOULDER		 0x17
#define USB_HID_DESIGNATOR_HIP			 0x18
#define USB_HID_DESIGNATOR_WAIST		 0x19
#define USB_HID_DESIGNATOR_THIGH		 0x1A
#define USB_HID_DESIGNATOR_KNEE			 0x1B
#define USB_HID_DESIGNATOR_CALF			 0x1C
#define USB_HID_DESIGNATOR_ANKLE		 0x1D
#define USB_HID_DESIGNATOR_FOOT			 0x1E
#define USB_HID_DESIGNATOR_HEEL			 0x1F
#define USB_HID_DESIGNATOR_BALL_OF_FOOT	 0x20
#define USB_HID_DESIGNATOR_BIG_TOE		 0x21
#define USB_HID_DESIGNATOR_SECOND_TOE	 0x22
#define USB_HID_DESIGNATOR_THIRD_TOE	 0x23
#define USB_HID_DESIGNATOR_FOURTH_TOE	 0x24
#define USB_HID_DESIGNATOR_LITTLE_TOE	 0x25
#define USB_HID_DESIGNATOR_BROW			 0x26
#define USB_HID_DESIGNATOR_CHEEK		 0x27
//@}

/* TYPES ***************************************************************************/

/**
    @struct     USB_hid_descriptor
    @brief      Structure representing a USB HID Descriptor.
                Section 6.2.1. USB HID Descriptor

 **/
typedef struct PACK USB_hid_descriptor
{
    /** Size of descriptor in bytes. **/
    uint8_t  bLength;
    /** DFU_FUNCTIONAL descriptor type (always 0x21). **/
    uint8_t  bDescriptorType;
    /** Version of HID specification used. **/
    uint16_t bcdHID;
    /** Country code applied to device. **/
    uint8_t  bCountryCode;
    /** Number of descriptors following. **/
    uint8_t  bNumDescriptors;
    /** A HID device has at least 1 report descriptor.
     * Descriptor type is followed by length of descriptor then the data. **/
    uint8_t  bDescriptorType_0;
    uint16_t wDescriptorLength_0;
    /** from this point on there are variable size fields depending
     * on bNumDescriptors. **/
} USB_hid_descriptor;

/* End of USB HID Specification */
//@}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // FT900_USB_HID_H_
