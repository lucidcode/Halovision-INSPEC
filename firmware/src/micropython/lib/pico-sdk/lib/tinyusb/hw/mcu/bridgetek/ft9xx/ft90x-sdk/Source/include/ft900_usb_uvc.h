/**
    @file ft900_usb_uvc.h

    @brief
    USB UVC class definitions

    
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

#ifndef FT900_USBD_UVC_H_
#define FT900_USBD_UVC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

#include "ft900_usb.h"

/* CONSTANTS ***********************************************************************/

/**
    @name USB UVC Protocol Codes
    @brief Section A.3 Table A-3 Protocol Codes.
 */
//@{
#define USB_UVC_PC_PROTOCOL_UNDEFINED 0x00
#define USB_UVC_PC_PROTOCOL_15 0x01
//@}

/**
    @name USB UVC Type Values
    @brief Section A.4 Table A-4 Type Values for Class specific
    Configuration Descriptors.
 */
//@{
#define USB_UVC_DESCRIPTOR_TYPE_CS_UNDEFINED 0x20
#define USB_UVC_DESCRIPTOR_TYPE_CS_DEVICE 0x21
#define USB_UVC_DESCRIPTOR_TYPE_CS_CONFIGURATION 0x22
#define USB_UVC_DESCRIPTOR_TYPE_CS_STRING 0x23
#define USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE 0x24
#define USB_UVC_DESCRIPTOR_TYPE_CS_ENDPOINT 0x25
//@}

/**
    @name USB UVC VC Subtype Values
    @brief Section A.5 Table A-5 Subtype Values for Class specific VC
    Configuration Descriptors.
 */
//@{
#define USB_UVC_DESCRIPTOR_SUBTYPE_VC_DESCRIPTOR_UNDEFINED 0x00
#define USB_UVC_DESCRIPTOR_SUBTYPE_VC_HEADER 0x01
#define USB_UVC_DESCRIPTOR_SUBTYPE_VC_INPUT_TERMINAL 0x02
#define USB_UVC_DESCRIPTOR_SUBTYPE_VC_OUTPUT_TERMINAL 0x03
#define USB_UVC_DESCRIPTOR_SUBTYPE_VC_SELECTOR_UNIT 0x04
#define USB_UVC_DESCRIPTOR_SUBTYPE_VC_PROCESSING_UNIT 0x05
#define USB_UVC_DESCRIPTOR_SUBTYPE_VC_EXTENSION_UNIT 0x06
#define USB_UVC_DESCRIPTOR_SUBTYPE_VC_ENCODING_UNIT 0x07
//@}

/**
    @name USB UVC VS Subtype Values
    @brief Section A.6 Table A-6 Subtype Values for Class specific VS
    Configuration Descriptors.
 */
//@{
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_UNDEFINED 0x00
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_INPUT_HEADER 0x01
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_OUTPUT_HEADER 0x02
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_STILL_IMAGE_FRAME 0x03
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_UNCOMPRESSED 0x04
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_UNCOMPRESSED 0x05
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_MJPEG 0x06
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_MJPEG 0x07
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_MPEG2TS 0x0A
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_DV 0x0C
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_COLORFORMAT 0x0D
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_FRAME_BASED 0x10
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_FRAME_BASED 0x11
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_STREAM_BASED 0x12
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_H264 0x13
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_H264 0x14
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_H264_SIMULCAST 0x15
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_VP8 0x16
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_VP8 0x17
#define USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_VP8_SIMULCAST 0x18
//@}

/**
    @name USB UVC Endpoint Descriptor Subtype Values
    @brief Section A.7 Table A-7 Subtype Values for Class specific Endpoint
    Configuration Descriptors.
 */
//@{
#define USB_UVC_DESCRIPTOR_SUBTYPE_EP_UNDEFINED 0x00
#define USB_UVC_DESCRIPTOR_SUBTYPE_EP_GENERAL 0x01
#define USB_UVC_DESCRIPTOR_SUBTYPE_EP_ENDPOINT 0x02
#define USB_UVC_DESCRIPTOR_SUBTYPE_EP_INTERRUPT 0x03
//@}

/**
    @name USB UVC Request Codes
    @brief Common to several UVC subclasses.
        Section A.8 Table A-8 Class Specific Request Codes.
 */
//@{
#define USB_UVC_REQUEST_RC_UNDEFINED 0x00
#define USB_UVC_REQUEST_SET_CUR 0x01
#define USB_UVC_REQUEST_SET_CUR_ALL 0x11
#define USB_UVC_REQUEST_GET_CUR 0x81
#define USB_UVC_REQUEST_GET_MIN 0x82
#define USB_UVC_REQUEST_GET_MAX 0x83
#define USB_UVC_REQUEST_GET_RES 0x84
#define USB_UVC_REQUEST_GET_LEN 0x85
#define USB_UVC_REQUEST_GET_INFO 0x86
#define USB_UVC_REQUEST_GET_DEF 0x87
#define USB_UVC_REQUEST_GET_CUR_ALL 0x91
#define USB_UVC_REQUEST_GET_MIN_ALL 0x92
#define USB_UVC_REQUEST_GET_MAX_ALL 0x93
#define USB_UVC_REQUEST_GET_RES_ALL 0x94
#define USB_UVC_REQUEST_GET_DEF_ALL 0x97
//@}

/**
    @name USB UVC Video Control Interface Control Selectors
    @brief Section A.9.1 Table A-9 Video Control Interface Control Selectors
 */
//@{
#define USB_UVC_VC_CONTROL_UNDEFINED 0x00
/** Power mode setting for device.
 * Response is bitmap defined in Table 4-6. */
#define USB_UVC_VC_VIDEO_POWER_MODE_CONTROL 0x01
/** Responses for USB_UVC_VC_VIDEO_POWER_MODE_CONTROL **/
//@{
#define USB_UVC_REQUEST_POWER_MODE_SETTING_FULL_POWER 0x00 /// Full Power
#define USB_UVC_REQUEST_POWER_MODE_SETTING_DEVICE_DEPENDENT 0x01 /// Device Dependent Power Mode
#define USB_UVC_REQUEST_POWER_MODE_SETTING_DEVICE_DEPENDENT_SUPPORTED 0x10 /// Device Dependent Power Mode Supported
#define USB_UVC_REQUEST_POWER_MODE_SETTING_USB_POWER 0x00 /// Device Uses USB Power
#define USB_UVC_REQUEST_POWER_MODE_SETTING_BATTERY_POWER 0x00 /// Device Uses Battery Power
#define USB_UVC_REQUEST_POWER_MODE_SETTING_AC_POWER 0x00 /// Device Uses AC Power
//@}
/** Error code control setting. Last error.
 * Response is number defined in Table 4-7. */
#define USB_UVC_VC_REQUEST_ERROR_CODE_CONTROL 0x02
/** Responses for USB_UVC_VC_REQUEST_ERROR_CODE_CONTROL */
//@{
#define USB_UVC_REQUEST_ERROR_CODE_CONTROL_NO_ERROR 0x00 /// No error
#define USB_UVC_REQUEST_ERROR_CODE_CONTROL_NOT_READY 0x01 /// Not ready
#define USB_UVC_REQUEST_ERROR_CODE_CONTROL_WRONG_STATE 0x02 /// Wrong state
#define USB_UVC_REQUEST_ERROR_CODE_CONTROL_POWER 0x03 /// Power
#define USB_UVC_REQUEST_ERROR_CODE_CONTROL_OUT_OF_RANGE 0x04 /// Out of range
#define USB_UVC_REQUEST_ERROR_CODE_CONTROL_INVALID_UNIT 0x05 /// Invalid unit
#define USB_UVC_REQUEST_ERROR_CODE_CONTROL_INVALID_CONTROL 0x06 /// Invalid control
#define USB_UVC_REQUEST_ERROR_CODE_CONTROL_INVALID_REQUEST 0x07 /// Invalid Request
///0x08-0xFE: Reserved for future use
#define USB_UVC_REQUEST_ERROR_CODE_CONTROLUNKNOWN 0xFF /// Unknown
//@}

//@}

/**
    @name USB UVC Terminal Control Selectors
    @brief Section A.9.2 Table A-10 Terminal Control Selectors.
 */
//@{
#define USB_UVC_TE_CONTROL_UNDEFINED 0x00
//@}

/**
    @name USB UVC Selector Unit Control Selectors
    @brief Section A.9.3 Table A-11 Selector Unit Control Selectors
 */
//@{
#define USB_UVC_SU_CONTROL_UNDEFINED 0x00
#define USB_UVC_SU_INPUT_SELECT_CONTROL 0x01
//@}

/**
    @name USB UVC Camera Terminal Control Selectors
    @brief Section A.9.4 Table A-12 Camera Terminal Control Selectors
 */
//@{
#define USB_UVC_CT_CONTROL_UNDEFINED 0x00
/** Camera scanning mode control.
 * Response is number defined in Table 4-9. */
#define USB_UVC_CT_SCANNING_MODE_CONTROL 0x01
/** Responses for USB_UVC_CT_SCANNING_MODE_CONTROL */
//@{
#define USB_UVC_REQUEST_CT_SCANNING_MODE_INTERLACED 0x00 /// Device uses interlaced scanning
#define USB_UVC_REQUEST_CT_SCANNING_MODE_PROGRESSIVE 0x01 /// Device uses progressive scanning
//@}
/** Camera auto exposure mode control.
 * Response is number defined in Table 4-10. */
#define USB_UVC_CT_AE_MODE_CONTROL 0x02
/** Responses for USB_UVC_CT_AE_MODE_CONTROL */
//@{
#define USB_UVC_REQUEST_CT_AE_MODE_MANUAL 0x01 /// Manual exposure, manual iris.
#define USB_UVC_REQUEST_CT_AE_MODE_AUTO 0x02 /// Auto exposure, Auto iris.
#define USB_UVC_REQUEST_CT_AE_MODE_SHUTTER_PRIORITY 0x04 /// manual Exposure Time, auto Iris.
#define USB_UVC_REQUEST_CT_AE_MODE_APERTURE_PRIORITY 0x81 /// auto Exposure Time, manual Iris
//@}
/** Camera auto exposure priority control.
 * Response is number defined in Table 4-11. */
#define USB_UVC_CT_AE_PRIORITY_CONTROL 0x03
/** Responses for USB_UVC_CT_AE_PRIORITY_CONTROL */
//@{
#define USB_UVC_REQUEST_CT_AE_PRIORITY_ENABLE 0x01 /// Frame rate can change for auto exposure
//@}
/** Camera auto exposure time (absolute) control.
 * Response is number defined in Table 4-12. */
#define USB_UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL 0x04
/** Camera auto exposure time (relative) control.
 * Response is number defined in Table 4-13. */
#define USB_UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL 0x05
/** Camera focus (absolute) control.
 * Response is number defined in Table 4-14. */
#define USB_UVC_CT_FOCUS_ABSOLUTE_CONTROL 0x06
/** Camera focus (relative) control.
 * Response is number defined in Table 4-15. */
#define USB_UVC_CT_FOCUS_RELATIVE_CONTROL 0x07
/** Camera focus auto control.
 * Response is number defined in Table 4-16. */
#define USB_UVC_CT_FOCUS_AUTO_CONTROL 0x08
/** Camera iris (absolute) control.
 * Response is number defined in Table 4-17. */
#define USB_UVC_CT_IRIS_ABSOLUTE_CONTROL 0x09
/** Camera iris (relative) control.
 * Response is number defined in Table 4-18. */
#define USB_UVC_CT_IRIS_RELATIVE_CONTROL 0x0A
/** Camera zoom (absolute) control.
 * Response is number defined in Table 4-19. */
#define USB_UVC_CT_ZOOM_ABSOLUTE_CONTROL 0x0B
/** Camera zoom (relative) control.
 * Response is number defined in Table 4-20. */
#define USB_UVC_CT_ZOOM_RELATIVE_CONTROL 0x0C
/** Camera pan/tilt (absolute) control.
 * Response is number defined in Table 4-21. */
#define USB_UVC_CT_PANTILT_ABSOLUTE_CONTROL 0x0D
/** Camera pan/tilt (relative) control.
 * Response is number defined in Table 4-22. */
#define USB_UVC_CT_PANTILT_RELATIVE_CONTROL 0x0E
/** Camera roll (absolute) control.
 * Response is number defined in Table 4-23. */
#define USB_UVC_CT_ROLL_ABSOLUTE_CONTROL 0x0F
/** Camera roll (relative) control.
 * Response is number defined in Table 4-24. */
#define USB_UVC_CT_ROLL_RELATIVE_CONTROL 0x10
/** Camera privacy control.
 * Response is number defined in Table 4-25. */
#define USB_UVC_CT_PRIVACY_CONTROL 0x11
/** Camera focus (simple) control.
 * Response is number defined in Table 4-26. Spec 1.5 only.*/
#define USB_UVC_CT_FOCUS_SIMPLE_CONTROL 0x12
/** Camera window control.
 * Response is number defined in Table 4-27. Spec 1.5 only.*/
#define USB_UVC_CT_WINDOW_CONTROL 0x13
/** Camera region of interest control.
 * Response is number defined in Table 4-28. Spec 1.5 only.*/
#define USB_UVC_CT_REGION_OF_INTEREST_CONTROL 0x14
//@}

/**
    @name USB UVC Processing Unit Control Selectors
    @brief Section A.9.4 Table A-12 Processing Unit Control Selectors
 */
//@{
#define USB_UVC_PU_CONTROL_UNDEFINED 0x00
#define USB_UVC_PU_BACKLIGHT_COMPENSATION_CONTROL 0x01
#define USB_UVC_PU_BRIGHTNESS_CONTROL 0x02
#define USB_UVC_PU_CONTRAST_CONTROL 0x03
#define USB_UVC_PU_GAIN_CONTROL 0x04
#define USB_UVC_PU_POWER_LINE_FREQUENCY_CONTROL 0x05
#define USB_UVC_PU_HUE_CONTROL 0x06
#define USB_UVC_PU_SATURATION_CONTROL 0x07
#define USB_UVC_PU_SHARPNESS_CONTROL 0x08
#define USB_UVC_PU_GAMMA_CONTROL 0x09
#define USB_UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL 0x0A
#define USB_UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL 0x0B
#define USB_UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL 0x0C
#define USB_UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL 0x0D
#define USB_UVC_PU_DIGITAL_MULTIPLIER_CONTROL 0x0E
#define USB_UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL 0x0F
#define USB_UVC_PU_HUE_AUTO_CONTROL 0x10
#define USB_UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL 0x11
#define USB_UVC_PU_ANALOG_LOCK_STATUS_CONTROL 0x12
#define USB_UVC_PU_CONTRAST_AUTO_CONTROL 0x13
//@}

/**
    @name USB UVC Encoding Unit Control Selectors
    @brief Section A.9.5 Table A-13 Encoding Unit Control Selectors
 */
//@{
#define USB_UVC_EU_CONTROL_UNDEFINED 0x00
#define USB_UVC_EU_SELECT_LAYER_CONTROL 0x01
#define USB_UVC_EU_PROFILE_TOOLSET_CONTROL 0x02
#define USB_UVC_EU_VIDEO_RESOLUTION_CONTROL 0x03
#define USB_UVC_EU_MIN_FRAME_INTERVAL_CONTROL 0x04
#define USB_UVC_EU_SLICE_MODE_CONTROL 0x05
#define USB_UVC_EU_RATE_CONTROL_MODE_CONTROL 0x06
#define USB_UVC_EU_AVERAGE_BITRATE_CONTROL 0x07
#define USB_UVC_EU_CPB_SIZE_CONTROL 0x08
#define USB_UVC_EU_PEAK_BIT_RATE_CONTROL 0x09
#define USB_UVC_EU_QUANTIZATION_PARAMS_CONTROL 0x0A
#define USB_UVC_EU_SYNC_REF_FRAME_CONTROL 0x0B
#define USB_UVC_EU_LTR_BUFFER_ CONTROL 0x0C
#define USB_UVC_EU_LTR_PICTURE_CONTROL 0x0D
#define USB_UVC_EU_LTR_VALIDATION_CONTROL 0x0E
#define USB_UVC_EU_LEVEL_IDC_LIMIT_CONTROL 0x0F
#define USB_UVC_EU_SEI_PAYLOADTYPE_CONTROL 0x10
#define USB_UVC_EU_QP_RANGE_CONTROL 0x11
#define USB_UVC_EU_PRIORITY_CONTROL 0x12
#define USB_UVC_EU_START_OR_STOP_LAYER_CONTROL 0x13
#define USB_UVC_EU_ERROR_RESILIENCY_CONTROL 0x14
//@}

/**
    @name USB UVC Extension Unit Control Selectors
    @brief Section A.9.6 Table A-14 Extension Unit Control Selectors
 */
//@{
#define USB_UVC_XU_CONTROL_UNDEFINED 0x00
//@}

/**
    @name USB UVC Video Streaming Interface Control Selectors
    @brief Section A.9.8 Table A-16 Video Streaming Interface Control Selectors
 */
//@{
#define USB_UVC_VS_CONTROL_UNDEFINED 0x00
#define USB_UVC_VS_PROBE_CONTROL 0x01
#define USB_UVC_VS_COMMIT_CONTROL 0x02
/** Definitions for bmHint for USB_UVC_VS_PROBE_CONTROL */
//@{
/// bmHint.dwFrameInterval bit. Probe/Commit response contains dwFrameInterval value.
#define USB_UVC_VS_PROBE_COMMIT_CONTROL_BMHINT_FRAMINGINFO 0x01
/// bmHint.wKeyFrameRate bit. Probe/Commit response contains wKeyFrameRate value.
#define USB_UVC_VS_PROBE_COMMIT_CONTROL_BMHINT_KEYFRAMERATE 0x02
/// bmHint.wPFrameRate bit. Probe/Commit response contains wPFrameRate value.
#define USB_UVC_VS_PROBE_COMMIT_CONTROL_BMHINT_PFRAMERATE 0x04
/// bmHint.wCompQuality bit. Probe/Commit response contains wCompQuality value.
#define USB_UVC_VS_PROBE_COMMIT_CONTROL_BMHINT_COMPQUALITY 0x08
/// bmHint.wCompWindowSize bit. Probe/Commit response contains wCompWindowSize value.
#define USB_UVC_VS_PROBE_COMMIT_CONTROL_BMHINT_COMPWINDOWSIZE 0x10
//@}
/** Definitions for bmFramingInfo for USB_UVC_VS_PROBE_CONTROL */
//@{
/// bmFramingInfo.FrameIDField bit. Indicates Frame ID is required in the payload header.
#define USB_UVC_VS_PROBE_COMMIT_CONTROL_BMFRAMINGINFO_FRAMEIDFIELD 0x01
/// bmFramingInfo.EOFField bit. Indicates the payload header may contain an EOF Field.
#define USB_UVC_VS_PROBE_COMMIT_CONTROL_BMFRAMINGINFO_EOFFIELD 0x02
//@}
#define USB_UVC_VS_STILL_PROBE_CONTROL 0x03
#define USB_UVC_VS_STILL_COMMIT_CONTROL 0x04
#define USB_UVC_VS_STILL_IMAGE_TRIGGER_CONTROL 0x05
#define USB_UVC_VS_STREAM_ERROR_CODE_CONTROL 0x06
#define USB_UVC_VS_GENERATE_KEY_FRAME_CONTROL 0x07
#define USB_UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL 0x08
#define USB_UVC_VS_SYNCH_DELAY_CONTROL 0x09
//@}

/**
    @name USB UVC USB Terminal Types
    @brief Section B.1 Table B-1 USB Terminal Types
 */
//@{
/** A Terminal dealing with a signal carried over a vendor-specific interface.
 * The vendor-specific interface descriptor must contain a field that references the Terminal.
 */
#define USB_UVC_TT_VENDOR_SPECIFIC 0x0100
/** A Terminal dealing with a signal carried over an endpoint in a VideoStreaming interface.
 * The VideoStreaming interface descriptor points to the associated Terminal through the
 * bTerminalLink field.
 */
#define USB_UVC_TT_STREAMING 0x0101
//@}

/**
    @name USB UVC Input Terminal Types
    @brief Section B.2 Table B-2 Input Terminal Types
 */
//@{
/** Vendor-Specific Input Terminal.
 */
#define USB_UVC_ITT_VENDOR_SPECIFIC 0x0200
/** Camera sensor. To be used only in Camera Terminal descriptors.
 */
#define USB_UVC_ITT_CAMERA 0x0201
/** Sequential media. To be used only in Media Transport Terminal Descriptors.
 */
#define USB_UVC_ITT_MEDIA_TRANSPORT_INPUT 0x0202
//@}

/**
    @name USB UVC Output Terminal Types
    @brief Section B.3 Table B-3 Output Terminal Types
 */
//@{
/** Vendor-Specific Output Terminal.
 */
#define USB_UVC_OTT_VENDOR_SPECIFIC 0x0300
/** Generic display (LCD, CRT, etc.).
 */
#define USB_UVC_OTT_DISPLAY 0x0301
/** Sequential media. To be used only in Media Transport Terminal Descriptors.
 */
#define USB_UVC_OTT_MEDIA_TRANSPORT_OUTPUT 0x0302
//@}

/**
    @name USB UVC External Terminal Types
    @brief Section B.4 Table B-4 External Terminal Types
 */
//@{
/** Vendor-Specific External Terminal.
 */
#define USB_UVC_EXTERNAL_VENDOR_SPECIFIC 0x0400
/** Composite video connector.
  */
#define USB_UVC_COMPOSITE_CONNECTOR 0x0401
/** S-video connector.
  */
#define USB_UVC_SVIDEO_CONNECTOR 0x0402
/** Component video connector
  */
#define USB_UVC_COMPONENT_CONNECTOR 0x0403
//@}

/**
    @name USB UVC Uncompressed Compression Formats GUIDs
    @brief Uncompressed Payload Section 2.2
 */
//@{
/** YUY2 32595559-0000-0010-8000-00AA00389B71 **/
#define USB_UVC_GUID_YUY2 {0x59, 0x55, 0x59, 0x32, \
		0x00, 0x00, 0x10, 0x00, \
		0x80, 0x00, 0x00, 0xaa, \
		0x00, 0x38, 0x9b, 0x71,}
/** NV12 3231564E-0000-0010-8000-00AA00389B71 **/
#define USB_UVC_GUID_NV12 {0x4e, 0x56, 0x31, 0x32, \
		0x00, 0x00, 0x10, 0x00, \
		0x80, 0x00, 0x00, 0xaa, \
		0x00, 0x38, 0x9b, 0x71,}
/** M420 3032344D-0000-0010-8000-00AA00389B71 **/
#define USB_UVC_GUID_M420 {0x4d, 0x34, 0x32, 0x30, \
		0x00, 0x00, 0x10, 0x00, \
		0x80, 0x00, 0x00, 0xaa, \
		0x00, 0x38, 0x9b, 0x71,}
/** I420 30323449-0000-0010-8000-00AA00389B71 **/
#define USB_UVC_GUID_I420 {0x49, 0x34, 0x32, 0x30, \
		0x00, 0x00, 0x10, 0x00, \
		0x80, 0x00, 0x00, 0xaa, \
		0x00, 0x38, 0x9b, 0x71,}
//@}

/**
    @name USB UVC GET_INFO response
    @brief Bitmap of GET_INFO response. Table 4-3.
 */
//@{
#define USB_UVC_GET_INFO_RESPONSE_SUPPORTS_GET 0x01
#define USB_UVC_GET_INFO_RESPONSE_SUPPORTS_SET 0x03
#define USB_UVC_GET_INFO_RESPONSE_DISABLED 0x04
#define USB_UVC_GET_INFO_RESPONSE_AUTOUPDATE_CONTROL 0x08
#define USB_UVC_GET_INFO_RESPONSE_ASYNCHRONOUS_CONTROL 0x10
//@}




/* TYPES ***************************************************************************/

/**
 *  @brief UVC Interface Control Request
    @details Structure representing a UVC Interface Control Request.
    	This is identical to a USB Device Request (USB Spec Table 9-2) however
     	the fields have been renamed to clarify the use of control selectors and
     	interfaces.
 **/
typedef struct PACK USB_UVC_InterfaceControlRequest
{
    /** Request Type. See USB Spec Table 9-2.
     * Type is "Class" and recipient is "Interface". **/
    uint8_t  bmRequestType;
    /** Request. Can be one of USB_UVC_REQUEST_*. **/
    uint8_t  bRequest;
    /** Control Selector in high byte, zero in low byte. **/
    uint16_t wCS;
    /** Interface in low byte, entity ID or zero in high byte. **/
    uint16_t wInterface;
    /** Number of bytes to transfer for data stage. **/
    uint16_t wLength;
} USB_UVC_InterfaceControlRequest;
//@}

/**
 *  @brief UVC Unit and Terminal Control Request
    @details Structure representing a UVC Unit and Terminal Control Request.
    	This is identical to a USB Device Request (USB Spec Table 9-2) however
     	the fields have been renamed to clarify the use of control selectors and
     	interfaces.
 **/
typedef struct PACK USB_UVC_UnitControlRequest
{
    /** Request Type. See USB Spec Table 9-2.
     * Type is "Class" and recipient is "Interface". **/
    uint8_t  bmRequestType;
    /** Request. Can be one of USB_UVC_REQUEST_*. **/
    uint8_t  bRequest;
    /** Control Selector in high byte, zero in low byte. **/
    uint16_t wCS;
    /** Interface in low byte, Unit or Terminal ID in high byte. **/
    uint16_t wUnit;
    /** Number of bytes to transfer for data stage. **/
    uint16_t wLength;
} USB_UVC_UnitControlRequest;
//@}

/**
 *  @brief UVC Extension Unit Control Request
    @details Structure representing a UVC Extension Unit Control Request.
    	This is identical to a USB Device Request (USB Spec Table 9-2) however
    	the fields have been renamed to clarify the use of control selectors and
    	interfaces.
 **/
typedef struct PACK USB_UVC_ExtensionUnitControlRequest
{
    /** Request Type. See USB Spec Table 9-2.
     * Type is "Class" and recipient is "Interface". **/
    uint8_t  bmRequestType;
    /** Request. Can be one of USB_UVC_REQUEST_*. **/
    uint8_t  bRequest;
    /** Control Selector in high byte, zero in low byte. **/
    uint16_t wCS;
    /** Interface in low byte, Extension Unit ID in high byte. **/
    uint16_t wExtensionUnit;
    /** Number of bytes to transfer for data stage. **/
    uint16_t wLength;
} USB_UVC_ExtensionUnitControlRequest;
//@}

/**
 *  @brief UVC Video Probe and Commit Controls
    @details Definition of structure used to pass streaming parameters
    	from the host to the device and pass the current settings back to
    	the host.
 **/
typedef struct PACK USB_UVC_VideoProbeAndCommitControls
{
    /** Bitfield control indicating to the function what fields shall be kept fixed
    	(indicative only):
    	D0: dwFrameInterval
		D1: wKeyFrameRate
		D2: wPFrameRate
		D3: wCompQuality
		D4: wCompWindowSize
		D15..5: Reserved (0)
		The hint bitmap indicates to the video streaming interface which fields shall
		be kept constant during stream parameter negotiation. */
    uint16_t bmHint;
    /** Video format index from a format descriptor.
    	Select a specific video stream format by setting this field to the one-based
    	index of the associated format descriptor. */
    uint8_t bFormatIndex;
    /** Video frame index from a frame descriptor.
		This field selects the video frame resolution from the array of resolutions
		supported by the selected stream. */
    uint8_t bFrameIndex;
    /** Frame interval in 100 ns units.
		This field sets the desired video frame interval for the selected video stream
		and frame index. The frame interval value is specified in 100 ns units. */
    uint32_t dwFrameInterval;
    /** Key frame rate in key-frame per videoframe units.
		This field is only applicable to sources (and formats) capable of streaming
		video with adjustable compression parameters. */
    uint16_t wKeyFrameRate;
    /** PFrame rate in PFrame/key frame units.
		This field is only applicable to sources (and formats) capable of streaming
		video with adjustable compression parameters. */
    uint16_t wPFrameRate;
    /** Compression quality control in abstract units 0 (lowest) to 10000 (highest).
		This field is only applicable to sources (and formats) capable of streaming
		video with adjustable compression parameters. */
    uint16_t wCompQuality;
    /** Window size for average bit rate control.
		This field is only applicable to sources (and formats) capable of streaming
		video with adjustable compression parameters. */
    uint16_t wCompWindowSize;
    /** Internal video streaming interface latency in ms from video data capture
		to presentation on the USB. */
    uint16_t wDelay;
    /** Maximum video frame or codec specific segment size in bytes.
		For frame-based formats, this field indicates the maximum size of a single
		video frame.
		For stream-based formats, and when this behavior is enabled via the
		bmFramingInfo field (below), this field indicates the maximum size of a
		single codec-specific segment. */
    uint32_t dwMaxVideoFrameSize;
    /** Specifies the maximum number of bytes that the device can transmit or
		receive in a single payload transfer. */
    uint32_t dwMaxPayloadTransferSize;
    /** The device clock frequency in Hz for the specified format. */
    uint32_t dwClockFrequency;
    /** Bitfield control supporting the following values:
		D0: If set to 1, the Frame ID (FID) field is required in the
			Payload Header (see description of D0 in section 2.4.3.3, ?Video and Still
			Image Payload Headers?).
			The sender is required to toggle the Frame ID at least every dwMaxVideoFrameSize
			bytes (see above).
		D1: If set to 1, indicates that the End of Frame (EOF) field may be present in the
			Payload Header (see description of D1 in section 2.4.3.3, ?Video and Still
			Image Payload Headers?). It is an error to specify this bit without also specifying D0.
			D7..2: Reserved (0)
		This control indicates to the function whether payload transfers will contain
		out-of-band framing information in the Video Payload Header (see section
		2.4.3.3, ?Video and Still Image Payload Headers?). */
	uint8_t bmFramingInfo;
	/** The preferred payload format version supported by the host or device for the
		specified bFormatIndex value. */
	uint8_t bPreferedVersion;
	/** The minimum payload format version supported by the device for the
		specified bFormatIndex value. */
	uint8_t bMinVersion;
	/** The maximum payload format version supported by the device for the
		specified bFormatIndex value. */
	uint8_t bMaxVersion;
} USB_UVC_VideoProbeAndCommitControls;
//@}

/**
    @name UVC Standard Video Interface Collection IAD
    @details Table 3-1
    Note: Identical to USB Interface Association Descriptor.
 */
typedef struct PACK USB_UVC_interface_association_descriptor
{
	unsigned char  bLength;
	/** USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION **/
	unsigned char  bDescriptorType;
	/** Interface number of the first VideoControl interface that is associated with this function **/
	unsigned char  bFirstInterface;
	/** Number of contiguous VideoStreaming	interfaces that are associated with this function. **/
	/** The count includes the first VideoControl interface and all its	associated VideoStreaming interfaces. **/
	unsigned char  bInterfaceCount;
	/** USB_CLASS_VIDEO **/
	unsigned char  bFunctionClass;
	/** USB_SUBCLASS_VIDEO_INTERFACE_COLLECTION **/
	unsigned char  bFunctionSubclass;
	/** Not used. Must be USB_UVC_PC_PROTOCOL_UNDEFINED. **/
	unsigned char  bFunctionProtocol;
	/** Index of a string descriptor that describes this interface. **/
	unsigned char  iFunction;
} USB_UVC_interface_association_descriptor;

/**
    @name UVC Standard VC Interface Descriptor
    @details Table 3-2
    Note: Identical to USB Interface Descriptor.
	bInterfaceClass = USB_CLASS_VIDEO
	bInterfaceSubClass = USB_SUBCLASS_VIDEO_VIDEOCONTROL
	bInterfaceProtocol = USB_UVC_PC_PROTOCOL_15
 */
typedef struct PACK USB_interface_descriptor
	USB_UVC_VC_StandardInterfaceDescriptor;

/**
    @name UVC Class Specific VC Interface Descriptor
    @details Table 3-3
 */
//@{
#define USB_UVC_VC_CSInterfaceHeaderDescriptor(A) \
struct PACK { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE */ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VC_HEADER */ \
	unsigned char  bDescriptorSubType; \
	/** Video Device Class Specification release number in binary-coded decimal. */ \
	unsigned short  bcdUVC; \
	/** Total number of bytes returned for the class-specific VideoControl interface descriptor. */ \
	/** Includes the combined length of this descriptor header and all Unit and	Terminal descriptors. */ \
	unsigned short  wTotalLength; \
	/** Use of this field has been deprecated. */ \
	unsigned long  dwClockFrequency; \
	/** The number of VideoStreaming interfaces	in the Video Interface Collection to which */ \
	/** this VideoControl interface belongs: n */ \
	unsigned char  bInCollection; \
	/** Interface numbers of VideoStreaming interfaces in this collection. n entries. */ \
	unsigned char  baInterfaceNr[A]; \
}
//@}

/**
    @name UVC Input Terminal Descriptor
    @details Table 3-4
 */
typedef struct PACK USB_UVC_VC_InputTerminalDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VC_INPUT_TERMINAL **/
	unsigned char  bDescriptorSubType;
	/** A non-zero constant that uniquely identifies the Terminal within the video function. **/
	/** This value is used in all requests to address this Terminal. **/
	unsigned char  bTerminalID;
	/** Constant that characterizes the type of Terminal. **/
	unsigned short  wTerminalType;
	/** ID of the Output Terminal to which this Input Terminal is associated, **/
	/** or zero (0) if no such association exists. **/
	unsigned char  bAssocTerminal;
	/** Index of a string descriptor that describes this terminal. **/
	unsigned char  iTerminal;
} USB_UVC_VC_InputTerminalDescriptor;

/**
    @name UVC Output Terminal Descriptor
    @details Table 3-5
 */
typedef struct PACK USB_UVC_VC_OutputTerminalDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VC_OUTPUT_TERMINAL **/
	unsigned char  bDescriptorSubType;
	/** A non-zero constant that uniquely identifies the Terminal within the video function. **/
	/** This value is used in all requests to address this Terminal. **/
	unsigned char  bTerminalID;
	/** Constant that characterizes the type of Terminal. **/
	unsigned short  wTerminalType;
	/** ID of the Input Terminal to which this Input Terminal is associated, **/
	/** or zero (0) if no such association exists. **/
	unsigned char  bAssocTerminal;
	/** ID of the Unit or Terminal to which this Terminal is connected. **/
	unsigned char  bSourceID;
	/** Index of a string descriptor that describes this terminal. **/
	unsigned char  iTerminal;
} USB_UVC_VC_OutputTerminalDescriptor;

/**
    @name UVC Camera Terminal Descriptor
    @details Table 3-6
 */
#define USB_UVC_VC_CameraTerminalDescriptor(A) \
struct PACK { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VC_INPUT_TERMINAL **/ \
	unsigned char  bDescriptorSubType; \
	/** A non-zero constant that uniquely identifies the Terminal within the video function. **/ \
	/** This value is used in all requests to address this Terminal. **/ \
	unsigned char  bTerminalID; \
	/** Constant that characterizes the type of Terminal. This is USB_UVC_ITT_CAMERA. **/ \
	unsigned short  wTerminalType; \
	/** ID of an Output Terminal to which this Input Terminal is associated, **/ \
	/** or zero (0) if no such association exists. **/ \
	unsigned char  bAssocTerminal; \
	/** Index of a string descriptor that describes this terminal. **/ \
	unsigned char  iTerminal; \
	/** The value of Lmin If Optical Zoom is not supported; this field shall be	set to 0. **/ \
	unsigned short wObjectiveFocalLengthMin; \
	/** The value of Lmax If Optical Zoom is not supported; this field shall be	set to 0. **/ \
	unsigned short wObjectiveFocalLengthMax; \
	/** The value of Locular If Optical	Zoom is not supported; this field shall be set to 0. **/ \
	unsigned short wOcularFocalLength; \
	/** Size in bytes of the bmControls field. **/ \
	unsigned char bControlSize; \
	/** A bit set to 1 indicates that the mentioned Control is supported for the video stream. **/ \
	/** D0: Scanning Mode **/ \
	/** D1: Auto-Exposure Mode  **/ \
	/** D2: Auto-Exposure Priority **/ \
	/** D3: Exposure Time (Absolute) **/ \
	/** D4: Exposure Time (Relative) **/ \
	/** D5: Focus (Absolute) **/ \
	/** D6 : Focus (Relative) **/ \
	/** D7: Iris (Absolute) **/ \
	/** D8 : Iris (Relative) **/ \
	/** D9: Zoom (Absolute) **/ \
	/** D10: Zoom (Relative) **/ \
	/** D11: PanTilt (Absolute) **/ \
	/** D12: PanTilt (Relative) **/ \
	/** D13: Roll (Absolute) **/ \
	/** D14: Roll (Relative) **/ \
	/** D15: Reserved **/ \
	/** D16: Reserved **/ \
	/** D17: Focus, Auto **/ \
	/** D18: Privacy **/ \
	/** D19: Focus, Simple **/ \
	/** D20: Window **/ \
	/** D21: Region of Interest **/ \
	/** D22 ? D23: Reserved, set to zero **/ \
	unsigned char bmControls[A]; \
}

typedef struct PACK USB_UVC_VC_CameraTerminalDescriptor
	USB_UVC_VC_CameraTerminalDescriptor_1_5;

/**
    @name UVC Selector Unit Descriptor
    @details Table 3-7
 */
//@{
#define USB_UVC_VC_SelectorUnitDescriptor(A) \
struct PACK { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE */ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VC_SELECTOR_UNIT */ \
	unsigned char  bDescriptorSubType; \
	/** A non-zero constant that uniquely identifies the Unit within the video function. */ \
	/** This value is used in all requests to address this Terminal. */ \
	unsigned char  bUnitID; \
	/** Number of Input Pins of this Unit: p */ \
	unsigned char  bNrInPins; \
	/* ID of the Units or Terminals in this collection. p entries. */ \
	unsigned char  baSourceID[A]; \
	/** Index of a string descriptor that describes this selector unit. */ \
	unsigned char  iSelector;\
}
//@}

/**
    @name UVC Processing Unit Descriptor
    @details Table 3-8
 */
#define USB_UVC_VC_ProcessingUnitDescriptor(A) \
struct PACK  { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VC_PROCESSING_UNIT **/ \
	unsigned char  bDescriptorSubType; \
	/** A non-zero constant that uniquely identifies the Unit within the video function. **/ \
	/** This value is used in all requests to address this Terminal. **/ \
	unsigned char  bUnitID; \
	/** ID of the Unit or Terminal to which this Terminal is connected. **/ \
	unsigned char  bSourceID; \
	/** If the Digital Multiplier control is supported,	this field indicates the maximum digital **/ \
	/** magnification, multiplied by 100. **/ \
	/** If the Digital Multiplier control is not supported, this field shall be set	to 0. **/ \
	unsigned short wMaxMultiplier; \
	/** Size of the bmControls field, in bytes **/ \
	unsigned char  bControlSize; \
	/** A bit set to 1 indicates that the mentioned	Control is supported for the video stream. **/ \
	/** D0: Brightness **/ \
	/** D1: Contrast **/ \
	/** D2: Hue **/ \
	/** D3: Saturation **/ \
	/** D4: Sharpness **/ \
	/** D5: Gamma **/ \
	/** D6: White Balance Temperature **/ \
	/** D7: White Balance Component **/ \
	/** D8: Backlight Compensation **/ \
	/** D9: Gain **/ \
	/** D10: Power Line Frequency **/ \
	/** D11: Hue, Auto **/ \
	/** D12: White Balance Temperature, Auto **/ \
	/** D13: White Balance Component, Auto **/ \
	/** D14: Digital Multiplier **/ \
	/** D15: Digital Multiplier Limit **/ \
	/** D16: Analog Video Standard **/ \
	/** D17: Analog Video Lock Status **/ \
	/** D18: Contrast, Auto **/ \
	/** D19 ? D23: Reserved. Set to zero. **/ \
	unsigned char bmControls[A]; \
	/** Index of a string descriptor that describes this processing unit. **/ \
	unsigned char  iProcessing; \
	/** A bitmap of all analog video standards supported by the Processing Unit. **/ \
	/** A value of zero indicates that this bitmap should be ignored. **/ \
	/** D0: None **/ \
	/** D1: NTSC ? 525/60 **/ \
	/** D2: PAL ? 625/50 **/ \
	/** D3: SECAM ? 625/50 **/ \
	/** D4: NTSC ? 625/50 **/ \
	/** D5: PAL ? 525/60 **/ \
	/** D6-D7: Reserved. Set to zero. **/ \
	unsigned char bmVideoStandards; \
}

/**
    @name UVC Encoding Unit Descriptor
    @details Table 3-9
 */
#define USB_UVC_VC_EncodingUnitDescriptor(A) \
struct PACK  { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VC_ENCODING_UNIT **/ \
	unsigned char  bDescriptorSubType; \
	/** A non-zero constant that uniquely identifies the Unit within the video function. **/ \
	/** This value is used in all requests to address this Terminal. **/ \
	unsigned char  bUnitID; \
	/** ID of the Unit or Terminal to which this Unit is connected. **/ \
	unsigned char  bSourceID; \
	/** Index of a string descriptor that describes this encoding unit. **/ \
	unsigned char  iEncoding; \
	/** Size of the bmControls field, in bytes. **/ \
	unsigned char  bControlSize; \
	/** A bit set to 1 indicates that the specified	control is supported for initialization: **/ \
	/** D0: Select Layer **/ \
	/** D1: Profile and Toolset **/ \
	/** D2: Video Resolution **/ \
	/** D3: Minimum Frame Interval **/ \
	/** D4: Slice Mode **/ \
	/** D5: Rate Control Mode **/ \
	/** D6: Average Bit Rate **/ \
	/** D7: CPB Size **/ \
	/** D8: Peak Bit Rate **/ \
	/** D9: Quantization Parameter **/ \
	/** D10: Synchronization and Long-Term **/ \
	/** Reference Frame **/ \
	/** D11: Long-Term Buffer **/ \
	/** D12: Picture Long-Term Reference **/ \
	/** D13: LTR Validation **/ \
	/** D14: Level IDC **/ \
	/** D15: SEI Message **/ \
	/** D16: QP Range **/ \
	/** D17: Priority ID **/ \
	/** D18: Start or Stop Layer/View **/ \
	/** D19: Error Resiliency **/ \
	/** D20 - 23: Reserved; set to zero **/ \
	unsigned char bmControls[A]; \
	/** A bit set to 1 indicates that the mentioned	control is supported during runtime: **/ \
	/** D0: Select Layer **/ \
	/** D1: Profile and Toolset. **/ \
	/** D2: Video Resolution **/ \
	/** D3: Minimum Frame Interval **/ \
	/** D4: Slice Mode. **/ \
	/** D5: Rate Control Mode **/ \
	/** D6: Average Bit Rate **/ \
	/** D7: CPB Size **/ \
	/** D8: Peak Bit Rate. **/ \
	/** D9: Quantization Parameter **/ \
	/** D10: Synchronization and Long-Term **/ \
	/** Reference Frame **/ \
	/** D11: Long-Term Buffer **/ \
	/** D12: Picture Long-Term Reference **/ \
	/** D13: LTRValidation **/ \
	/** D14: Level IDC **/ \
	/** D15: SEI Message **/ \
	/** D16: QP Range **/ \
	/** D17: Priority ID. **/ \
	/** D18: Start or Stop Layer/View **/ \
	/** D19: Error Resiliency **/ \
	/** D20 ? D23: Reserved, set to zero **/ \
	unsigned char bmControlsRuntime[A]; \
};

/**
    @name UVC Extension Unit Descriptor
    @details Table 3-10
 */
//@{
#define USB_UVC_VC_ExtensionUnitDescriptor(A, B) \
struct PACK { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE */ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VC_EXTENSION_UNIT */ \
	unsigned char  bDescriptorSubType; \
	/** A non-zero constant that uniquely identifies the Unit within the video function. */ \
	/** This value is used in all requests to address this Terminal. */ \
	unsigned char  bUnitID; \
	/** Vendor-specific code identifying the Extension Unit */ \
	unsigned char  guidExtensionCode[16]; \
	/** Number of controls in this extension unit. */ \
	unsigned char  bNumControls; \
	/** Number of Input Pins of this Unit: p */ \
	unsigned char  bNrInPins; \
	/** ID of the Units or Terminals to which this extension unit is connected. p entries. */ \
	unsigned char  baSourceID[A]; \
	/** Size of the bmControls field, in bytes. */ \
	/** Note that this will move depending on bNrInPins. */ \
	unsigned char  bControlSize; \
	/** A bit set to 1 indicates that the mentioned	Control is supported. Vendor specific. */ \
	/** Note that this will move depending on bNrInPins. */ \
	unsigned char bmControls[B]; \
	/** Index of a string descriptor that describes this extension unit. */ \
	/** Note that this will move depending on bNrInPins and bControlSize. */ \
	unsigned char  iExtension; \
}
//@}

/**
    @name UVC Standard VC Interrupt Endpoint Descriptor
    @details Table 3-11
    Note: Identical to USB Endpoint Descriptor.
	bmAtributes: Synchronisation Type = 00, Transfer Type = 11 (Interrupt).
 */
typedef struct PACK USB_endpoint_descriptor
	USB_UVC_VC_StandardInterruptEndpointDescriptor;

/**
    @name UVC Class Specific VC Interrupt Endpoint Descriptor
    @details Table 3-12
 */
typedef struct PACK USB_UVC_VC_CSEndpointDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_ENDPOINT **/
	unsigned char  bDescriptorType;
	/** USB_DESCRIPTOR_TYPE_ENDPOINT **/
	unsigned char  bDescriptorSubType;
	/** Maximum interrupt structure size this endpoint is capable of sending. **/
	unsigned short wMaxTransferSize;
} USB_UVC_VC_CSEndpointDescriptor;

/**
    @name UVC Standard VS Interface Descriptor
    @details Note: Identical to USB Interface Descriptor.
	bInterfaceClass = USB_CLASS_VIDEO
	bInterfaceSubClass = USB_SUBCLASS_VIDEO_VIDEOSTREAMING
	bInterfaceProtocol = USB_UVC_PC_PROTOCOL_15
 */
typedef struct PACK USB_interface_descriptor
	USB_UVC_VS_StandardInterfaceDescriptor;

/**
    @name UVC Class Specific VS Interface Input Header Descriptor
    @details Table 3-14
 */
//@{
#define USB_UVC_VS_CSInterfaceInputHeaderDescriptor(A) \
struct PACK { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE */ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_INPUT_HEADER */ \
	unsigned char  bDescriptorSubType; \
	/** Number of video payload Format descriptors following for this interface */ \
	/** (excluding video Frame descriptors): p */ \
	unsigned char bNumFormats; \
	/** Total number of bytes returned for the class-specific VideoStreaming interface */ \
	/** descriptors including this header descriptor. */ \
	unsigned short wTotalLength; \
	/** The address of the isochronous or bulk endpoint used for video data. */ \
	unsigned char bEndpointAddress; \
	/** Indicates the capabilities of this VideoStreaming interface: */ \
	/** D0: Dynamic Format Change supported */ \
	/** D7..1: Reserved, set to zero */ \
	unsigned char bmInfo; \
	/** The terminal ID of the Output Terminal to which the video endpoint of this interface */ \
	/** is connected. */ \
	unsigned char bTerminalLink; \
	/** Method of still image capture supported as described in section 2.4.2.4, "Still Image */ \
	/** Capture": */ \
	/** 0: None (Host software will not support	any form of still image capture) */ \
	/** 1: Method 1 */ \
	/** 2: Method 2 */ \
	/** 3: Method 3 */ \
	unsigned char bStillCaptureMethod; \
	/** Specifies if hardware triggering is supported through this interface */ \
	/** 0: Not supported */ \
	/** 1: Supported */ \
	unsigned char bTriggerSupport; \
	/** Specifies how the host software shall respond to a hardware trigger interrupt */ \
	/** event from this interface. This is ignored if the bTriggerSupport field is zero. */ \
	/** 0: Initiate still image capture */ \
	/** 1: General purpose button event. Host driver will notify client application of */ \
	/**    button press and button release events. */ \
	unsigned char bTriggerUsage; \
	/** Size of each bmaControls(x) field, in bytes: n */ \
	unsigned char bControlSize; \
	/** For bits D3..0, a bit set to 1 indicates that the named field is supported by the Video */ \
	/** Probe and Commit Control when bFormatIndex is 1: */ \
	/** D0: wKeyFrameRate */ \
	/** D1: wPFrameRate */ \
	/** D2: wCompQuality */ \
	/** D3: wCompWindowSize */ \
	/** For bits D5..4, a bit set to 1 indicates that the named control is supported by the */ \
	/** device when bFormatIndex is 1: */ \
	/** D4: Generate Key Frame */ \
	/** D5: Update Frame Segment */ \
	/** D6..(n*8-1): Reserved, set to zero */ \
	unsigned char bmaControls[A]; \
}
//@}

/**
    @name UVC Class Specific VS Interface Output Header Descriptor
    @details Table 3-15
 */
typedef struct PACK USB_UVC_VS_CSInterfaceOutputHeaderDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_OUTPUT_HEADER **/
	unsigned char  bDescriptorSubType;
	/** Number of video payload Format descriptors following for this interface **/
	/** (excluding video Frame descriptors): p **/
	unsigned char bNumFormats;
	/** Total number of bytes returned for the class-specific VideoStreaming interface **/
	// descriptors including this header descriptor.
	unsigned short wTotalLength;
	/** The address of the isochronous or bulk endpoint used for video data. **/
	unsigned char bEndpointAddress;
	/** The terminal ID of the Output Terminal to which the video endpoint of this interface **/
	/** is connected. **/
	unsigned char bTerminalLink;
	/** Size of each bmaControls(x) field, in bytes: n **/
	unsigned char bControlSize;
	/** For bits D3..0, a bit set to 1 indicates that the named field is supported by the Video **/
	/** Probe and Commit Control when bFormatIndex is 1: **/
	/** D0: wKeyFrameRate **/
	/** D1: wPFrameRate **/
	/** D2: wCompQuality **/
	/** D3: wCompWindowSize **/
	/** D4..(n*8-1): Reserved, set to zero **/
	unsigned char bmaControls[1];
} USB_UVC_VS_CSInterfaceOutputHeaderDescriptor;

/**
    @name UVC Still Image Frame Descriptor
    @details Table 3-18
 */
typedef struct PACK USB_UVC_VS_CSStillImageFrameDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_STILL_IMAGE_FRAME **/
	unsigned char  bDescriptorSubType;
	/** If method 3 of still image capture is used,	this contains the address of the bulk **/
	/** endpoint used for still image capture. The address is encoded as follows: **/
	/**  D7: Direction. (set to 1 = IN endpoint) **/
	/**  D6..4: Reserved, reset to zero **/
	/**  D3..0: The endpoint number, determined by the designer **/
	/** If method 2 of still image capture is used,	this field shall be set to zero. **/
	unsigned char bEndpointAddress;
	/** Number of Image Size patterns of this format: n **/
	unsigned short bNumImageSizePatterns;
	/** Width of the still image in pattern. **/
	unsigned short wWidth;
	/** Height of the still image in pattern. **/
	unsigned short wHeight;
	/** Number of Compression pattern of this format: m **/
	unsigned short bNumCompressionPattern;
	/** Compression of the still image in pattern. **/
	unsigned char bCompression;
} USB_UVC_VS_CSStillImageFrameDescriptor;

/**
    @name UVC Color Matching Descriptor
    @details Table 3-19
 */
typedef struct PACK USB_UVC_VS_CSColorMatchingDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_COLORFORMAT **/
	unsigned char  bDescriptorSubType;
	/** This defines the color primaries and the reference white. **/
	/** 0: Unspecified (Image characteristics unknown) **/
	/** 1: BT.709, sRGB (default) **/
	/** 2: BT.470-2 (M) **/
	/** 3: BT.470-2 (B, G) **/
	/** 4: SMPTE 170M **/
	/** 5: SMPTE 240M **/
	/** 6-255: Reserved **/
	unsigned char bColorPrimaries;
	/** This field defines the optoelectronic transfer characteristic of **/
	/** the source picture also called the gamma function. **/
	/** 0: Unspecified (Image characteristics unknown) **/
	/** 1: BT.709 (default) **/
	/** 2: BT.470-2 M **/
	/** 3: BT.470-2 B, G **/
	/** 4: SMPTE 170M **/
	/** 5: SMPTE 240M **/
	/** 6: Linear (V = Lc) **/
	/** 7: sRGB (very similar to BT.709) **/
	/** 8-255: Reserved **/
	unsigned char bTransferCharacteristics;
	/** Matrix used to compute luma and	chroma values from the color primaries. **/
	/** 0: Unspecified (Image characteristics unknown) **/
	/** 1: BT. 709 **/
	/** 2: FCC **/
	/** 3: BT.470-2 B, G **/
	/** 4: SMPTE 170M (BT.601, default) **/
	/** 5: SMPTE 240M **/
	/** 6-255: Reserved **/
	unsigned char bMatrixCoefficients;
} USB_UVC_VS_CSColorMatchingDescriptor;

/**
    @name UVC Standard VS Isochronous Endpoint Descriptor
    @details Table 3-20
    Note: Identical to USB Endpoint Descriptor.
	bmAtributes: Synchronisation Type = 01, Transfer Type = 01 (Isochronous).
 */
typedef struct PACK USB_endpoint_descriptor
	USB_UVC_VS_IsochronousVideoDataEndpointDescriptor;

/**
    @name UVC Standard VS Bulk Endpoint Descriptor
    @details Table 3-21
    Note: Identical to USB Endpoint Descriptor.
	bmAtributes: Transfer Type = 10 (Bulk).
 */
typedef struct PACK USB_endpoint_descriptor
	USB_UVC_VS_BulkVideoDataEndpointDescriptor;

/**
    @name UVC Uncompressed Video Format Descriptor
    @details Uncompressed Payload Table 3-1
 */
typedef struct PACK USB_UVC_VS_UncompressedVideoFormatDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_UNCOMPRESSED **/
	unsigned char  bDescriptorSubType;
	/** Index of this format descriptor. **/
	unsigned char  bFormatIndex;
	/** Number of frame descriptors following that correspond to this format. **/
	unsigned char  bNumFrameDescriptors;
	/** Globally Unique Identifier used to identify stream-encoding format. **/
	unsigned char  guidFormat[16];
	/** Number of bits per pixel used to specify color in the decoded video frame. **/
	unsigned char  bBitsPerPixel;
	/** Optimum Frame Index (used to select resolution) for this stream **/
	unsigned char  bDefaultFrameIndex;
	/** The X dimension of the picture aspect ratio. **/
	unsigned char  bAspectRatioX;
	/** The Y dimension of the picture aspect ratio. **/
	unsigned char  bAspectRatioY;
	/** Specifies interlace information. If the	scanning mode control in the Camera **/
	/** Terminal is supported for this stream, this field shall reflect the field format **/
	/** used in interlaced mode. **/
	/** (Top field in PAL is field 1, top field in	NTSC is field 2.): **/
	/** D0: Interlaced stream or variable. 1 =	Yes **/
	/** D1: Fields per frame. 0= 2 fields, 1 = 1 field **/
	/** D2: Field 1 first. 1 = Yes **/
	/** D3: Reserved **/
	/** D5..4: Field pattern **/
	/**   00 = Field 1 only **/
	/**   01 = Field 2 only **/
	/**   10 = Regular pattern of fields 1 and 2 **/
	/**   11 = Random pattern of fields 1 and 2 **/
	/** D7..6: Reserved. Do not use **/
	unsigned char  bmInterlaceFlags;
	/** Specifies whether duplication of the video stream is restricted: **/
	/** 0: No restrictions **/
	/** 1: Restrict duplication **/
	unsigned char  bCopyProtect;
} USB_UVC_VS_UncompressedVideoFormatDescriptor;

/**
    @name UVC Uncompressed Video Format Descriptor
    @details Uncompressed Payload Table 3-2, Table 3-3, Table 3-4
 */
//@{
typedef struct PACK  USB_UVC_VS_UncompressedVideoFrameDescriptorContinuous {
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_UNCOMPRESSED **/
	unsigned char  bDescriptorSubType;
	/** Index of this frame descriptor. **/
	unsigned char  bFrameIndex;
	/** D0: Still image supported. Specifies whether still images are **/
	/** supported at this frame setting. This is only applicable for VS **/
	/** interfaces with	an IN video endpoint using Still Image Capture **/
	/** Method 1, and should be set to 0 in all other cases. **/
	/** D1: Fixed frame-rate. Specifies whether the device provides **/
	/** a fixed frame rate on a stream associated with this frame descriptor. **/
	/** Set to 1 if fixed rate is enabled; otherwise, set to 0. **/
	/** D7..2: Reserved, set to 0 **/
	unsigned char  bmCapabilities;
	/** Width of decoded bitmap frame in pixels **/
	unsigned short wWidth;
	/** Height of decoded bitmap frame in pixels **/
	unsigned short wHeight;
	/** Specifies the minimum bit rate at the longest frame interval in units of bps **/
	/** at which the data can be transmitted **/
	unsigned long dwMinBitRate;
	/** Specifies the maximum bit rate at the shortest frame interval in units of bps **/
	/** at which the data can be transmitted **/
	unsigned long dwMaxBitRate;
	/** Use of this field has been deprecated. **/
	unsigned long dwMaxVideoFrameBufferSize;
	/** Specifies the frame interval the device would like to indicate for use as a **/
	/** default. This must be a valid frame interval described in the fields below. **/
	unsigned long dwDefaultFrameInterval;
	/** Indicates how the frame interval can be programmed: **/
	/** 0: Continuous frame interval **/
	/** 1..255: The number of discrete frame intervals supported (n) **/
	unsigned char  bFrameIntervalType;
	/** Shortest frame interval supported (at highest frame rate), in 100 ns units **/
	unsigned long dwMinFrameInterval;
	/** Longest frame interval supported (at lowest frame rate), in 100 ns units. **/
	unsigned long dwMaxFrameInterval;
	/** Indicates granularity of frame interval range, in 100 ns units **/
	unsigned long dwFrameIntervalStep;
} USB_UVC_VS_UncompressedVideoFrameDescriptorContinuous;

#define USB_UVC_VS_UncompressedVideoFrameDescriptorDiscrete(A) \
struct PACK { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE */ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_UNCOMPRESSED */ \
	unsigned char  bDescriptorSubType; \
	/** Index of this frame descriptor. */ \
	unsigned char  bFrameIndex; \
	/** D0: Still image supported. Specifies whether still images are */ \
	/** supported at this frame setting. This is only applicable for VS */ \
	/** interfaces with	an IN video endpoint using Still Image Capture */ \
	/** Method 1, and should be set to 0 in all other cases. */ \
	/** D1: Fixed frame-rate. Specifies whether the device provides */ \
	/** a fixed frame rate on a stream associated with this frame descriptor. */ \
	/** Set to 1 if fixed rate is enabled; otherwise, set to 0. */ \
	/** D7..2: Reserved, set to 0 */ \
	unsigned char  bmCapabilities; \
	/** Width of decoded bitmap frame in pixels */ \
	unsigned short wWidth; \
	/** Height of decoded bitmap frame in pixels */ \
	unsigned short wHeight; \
	/** Specifies the minimum bit rate at the longest frame interval in units of bps */ \
	/** at which the data can be transmitted */ \
	unsigned long dwMinBitRate; \
	/** Specifies the maximum bit rate at the shortest frame interval in units of bps */ \
	/** at which the data can be transmitted */ \
	unsigned long dwMaxBitRate; \
	/** Use of this field has been deprecated. */ \
	unsigned long dwMaxVideoFrameBufferSize; \
	/** Specifies the frame interval the device would like to indicate for use as a */ \
	/** default. This must be a valid frame interval described in the fields below. */ \
	unsigned long dwDefaultFrameInterval; \
	/** Indicates how the frame interval can be programmed: */ \
	/** 0: Continuous frame interval */ \
	/** 1..255: The number of discrete frame intervals supported (n) */ \
	unsigned char  bFrameIntervalType; \
	/** Sortest to Longest Frame interval supported (at lowest to highest frame */ \
	/** rate), in 100 ns units */ \
	unsigned long dwFrameInterval[A]; \
}
//@}

/**
    @name UVC Motion JPEG Video Format Descriptor
    @details Motion JPEG Table 3-1
 */
typedef struct PACK USB_UVC_VS_MJPEGVideoFormatDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_FORMAT_MJPEG **/
	unsigned char  bDescriptorSubType;
	/** Index of this format descriptor. **/
	unsigned char  bFormatIndex;
	/** Number of frame descriptors following that correspond to this format. **/
	unsigned char  bNumFrameDescriptors;
	/** Specifies characteristics of this format.
	 * D0: Fixed size samples. 1 = Yes **/
	unsigned char  bmFlags;
	/** Optimum Frame Index (used to select resolution) for this stream **/
	unsigned char  bDefaultFrameIndex;
	/** The X dimension of the picture aspect ratio. **/
	unsigned char  bAspectRatioX;
	/** The Y dimension of the picture aspect ratio. **/
	unsigned char  bAspectRatioY;
	/** Specifies interlace information. If the	scanning mode control in the Camera **/
	/** Terminal is supported for this stream, this field shall reflect the field format **/
	/** used in interlaced mode. **/
	/** (Top field in PAL is field 1, top field in	NTSC is field 2.): **/
	/** D0: Interlaced stream or variable. 1 =	Yes **/
	/** D1: Fields per frame. 0= 2 fields, 1 = 1 field **/
	/** D2: Field 1 first. 1 = Yes **/
	/** D3: Reserved **/
	/** D5..4: Field pattern **/
	/**   00 = Field 1 only **/
	/**   01 = Field 2 only **/
	/**   10 = Regular pattern of fields 1 and 2 **/
	/**   11 = Random pattern of fields 1 and 2 **/
	/** D7..6: Reserved. Do not use **/
	unsigned char  bmInterlaceFlags;
	/** Specifies whether duplication of the video stream is restricted: **/
	/** 0: No restrictions **/
	/** 1: Restrict duplication **/
	unsigned char  bCopyProtect;
} USB_UVC_VS_MJPEGVideoFormatDescriptor;

/**
    @name UVC MJPEG Video Format Descriptor
    @details MJPEG Table 3-2, Table 3-3, Table 3-4
 */
//@{
typedef struct PACK  USB_UVC_VS_MJPEGVideoFrameDescriptorContinuous {
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_MJPEG **/
	unsigned char  bDescriptorSubType;
	/** Index of this frame descriptor. **/
	unsigned char  bFrameIndex;
	/** D0: Still image supported. Specifies whether still images are **/
	/** supported at this frame setting. This is only applicable for VS **/
	/** interfaces with	an IN video endpoint using Still Image Capture **/
	/** Method 1, and should be set to 0 in all other cases. **/
	/** D1: Fixed frame-rate. Specifies whether the device provides **/
	/** a fixed frame rate on a stream associated with this frame descriptor. **/
	/** Set to 1 if fixed rate is enabled; otherwise, set to 0. **/
	/** D7..2: Reserved, set to 0 **/
	unsigned char  bmCapabilities;
	/** Width of decoded bitmap frame in pixels **/
	unsigned short wWidth;
	/** Height of decoded bitmap frame in pixels **/
	unsigned short wHeight;
	/** Specifies the minimum bit rate at the longest frame interval in units of bps **/
	/** at which the data can be transmitted **/
	unsigned long dwMinBitRate;
	/** Specifies the maximum bit rate at the shortest frame interval in units of bps **/
	/** at which the data can be transmitted **/
	unsigned long dwMaxBitRate;
	/** Use of this field has been deprecated. **/
	unsigned long dwMaxVideoFrameBufferSize;
	/** Specifies the frame interval the device would like to indicate for use as a **/
	/** default. This must be a valid frame interval described in the fields below. **/
	unsigned long dwDefaultFrameInterval;
	/** Indicates how the frame interval can be programmed: **/
	/** 0: Continuous frame interval **/
	/** 1..255: The number of discrete frame intervals supported (n) **/
	unsigned char  bFrameIntervalType;
	/** Shortest frame interval supported (at highest frame rate), in 100 ns units **/
	unsigned long dwMinFrameInterval;
	/** Longest frame interval supported (at lowest frame rate), in 100 ns units. **/
	unsigned long dwMaxFrameInterval;
	/** Indicates granularity of frame interval range, in 100 ns units **/
	unsigned long dwFrameIntervalStep;
} USB_UVC_VS_MJPEGVideoFrameDescriptorContinuous;

#define USB_UVC_VS_MJPEGVideoFrameDescriptorDiscrete(A) \
struct PACK { \
	unsigned char  bLength; \
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE */ \
	unsigned char  bDescriptorType; \
	/** USB_UVC_DESCRIPTOR_SUBTYPE_VS_FRAME_MJPEG */ \
	unsigned char  bDescriptorSubType; \
	/** Index of this frame descriptor. */ \
	unsigned char  bFrameIndex; \
	/** D0: Still image supported. Specifies whether still images are */ \
	/** supported at this frame setting. This is only applicable for VS */ \
	/** interfaces with	an IN video endpoint using Still Image Capture */ \
	/** Method 1, and should be set to 0 in all other cases. */ \
	/** D1: Fixed frame-rate. Specifies whether the device provides */ \
	/** a fixed frame rate on a stream associated with this frame descriptor. */ \
	/** Set to 1 if fixed rate is enabled; otherwise, set to 0. */ \
	/** D7..2: Reserved, set to 0 */ \
	unsigned char  bmCapabilities; \
	/** Width of decoded bitmap frame in pixels */ \
	unsigned short wWidth; \
	/** Height of decoded bitmap frame in pixels */ \
	unsigned short wHeight; \
	/** Specifies the minimum bit rate at the longest frame interval in units of bps */ \
	/** at which the data can be transmitted */ \
	unsigned long dwMinBitRate; \
	/** Specifies the maximum bit rate at the shortest frame interval in units of bps */ \
	/** at which the data can be transmitted */ \
	unsigned long dwMaxBitRate; \
	/** Use of this field has been deprecated. */ \
	unsigned long dwMaxVideoFrameBufferSize; \
	/** Specifies the frame interval the device would like to indicate for use as a */ \
	/** default. This must be a valid frame interval described in the fields below. */ \
	unsigned long dwDefaultFrameInterval; \
	/** Indicates how the frame interval can be programmed: */ \
	/** 0: Continuous frame interval */ \
	/** 1..255: The number of discrete frame intervals supported (n) */ \
	unsigned char  bFrameIntervalType; \
	/** Sortest to Longest Frame interval supported (at lowest to highest frame */ \
	/** rate), in 100 ns units */ \
	unsigned long dwFrameInterval[A]; \
}
//@}

/**
    @name UVC Color Matching Descriptor
    @details UVC Specification Table 3-18
 */
typedef struct PACK USB_UVC_ColorMatchingDescriptor
{
	unsigned char  bLength;
	/** USB_UVC_DESCRIPTOR_TYPE_CS_INTERFACE **/
	unsigned char  bDescriptorType;
	/** VS_COLORFORMAT **/
	unsigned char  bDescriptorSubType;
	/** This defines the colour primaries and the reference white.
		0: Unspecified (Image characteristics unknown)
		1: BT.709, sRGB (default)
		2: BT.470-2 (M)
		3: BT.470-2 (B, G)
		4: SMPTE 170M
		5: SMPTE 240M
		6-255: Reserved
	 **/
	unsigned char  bColorPrimaries;
	/** This field defines the optoelectronic transfer characteristic of
		the source picture also called the gamma function.
		0: Unspecified (Image characteristics unknown)
		1: BT.709 (default)
		2: BT.470-2 M
		3: BT.470-2 B, G
	 **/
	unsigned char  bTransferCharacteristics;
	/** Matrix used to compute luma and chroma values from the color
		primaries.
		0: Unspecified (Image characteristics unknown)
		1: BT. 709
		2: FCC
		3: BT.470-2 B, G
		4: SMPTE 170M (BT.601, default)
		5: SMPTE 240M
		6-255: Reserved
	 **/
	unsigned char  bMatrixCoefficients;
} USB_UVC_ColorMatchingDescriptor;

/**
    @name UVC Uncompressed Video Payload Header
    @details Uncompressed Payload Table 2-2. There are three structures defined here:
    	1) USB_UVC_Payload_Header - No Presentation Time Stamp or Source Clock Reference.
    	2) USB_UVC_Payload_Header_PTS - Presentation Time Stamp, no Source Clock Reference.
    	3) USB_UVC_Payload_Header_PTS_SCR - Presentation Time Stamp and Source Clock Reference.
    	The actual structure is defined in bits D2 and D3 of the bmHeaderInfo field.
 */
//@{
typedef struct PACK USB_UVC_Payload_Header {
	unsigned char  bHeaderLength;
	/** Provides information on the sample data following the header, as well as the */
	/** availability of optional header fields in this header. */
	/** D0: Frame ID ? For frame-based formats, this bit toggles between 0 and 1 every time a
		new video frame begins. For stream-basedformats, this bit toggles between 0 and 1 at
		the start of each new codec-specific segment. */
	/** D1: End of Frame ? This bit is set if the following payload data marks the end of the
		current video or still image frame (for framebased formats), or to indicate the end of a
		codec-specific segment (for stream-based formats). */
	/** D2: Presentation Time ? Set to zero as this bit is only set if the dwPresentationTime
		field is being sent as part of the header. */
	/** D3: Source Clock Reference ? Set to zero as this bit is only set if the dwSourceClock
		field is being sent as part of the header. */
	/** D5: Still Image ? This bit is set if the following data is part of a still image frame,
		and is only used for methods 2 and 3 of still image capture. */
	/** D6: Error ? This bit is set if there was an	error in the video or still image transmission
		for this payload. The Stream Error Code	control would reflect the cause of the error. */
	/** D7: End of header ? This bit is set if this is the last header group in the packet, where the
		header group refers to this field and any optional fields identified by the bits in this
		field (Defined for future extension). */
	unsigned char bmHeaderInfo;
} USB_UVC_Payload_Header;

typedef struct PACK USB_UVC_Payload_Header_PTS {
	unsigned char  bHeaderLength;
	/** Provides information on the sample data following the header, as well as the */
	/** availability of optional header fields in this header. */
	/** D0: Frame ID ? For frame-based formats, this bit toggles between 0 and 1 every time a
		new video frame begins. For stream-basedformats, this bit toggles between 0 and 1 at
		the start of each new codec-specific segment. */
	/** D1: End of Frame ? This bit is set if the following payload data marks the end of the
		current video or still image frame (for framebased formats), or to indicate the end of a
		codec-specific segment (for stream-based formats). */
	/** D2: Presentation Time ? This bit is set if the dwPresentationTime field is being sent as
		part of the header. */
	/** D3: Source Clock Reference ? Set to zero as this bit is only set if the dwSourceClock
		field is being sent as part of the header. */
	/** D5: Still Image ? This bit is set if the following data is part of a still image frame,
		and is only used for methods 2 and 3 of still image capture. */
	/** D6: Error ? This bit is set if there was an	error in the video or still image transmission
		for this payload. The Stream Error Code	control would reflect the cause of the error. */
	/** D7: End of header ? This bit is set if this is the last header group in the packet, where the
		header group refers to this field and any optional fields identified by the bits in this
		field (Defined for future extension). */
	unsigned char bmHeaderInfo;
	/** Presentation Time Stamp (PTS). The source clock time in native device
		clock units when the raw frame capture begins. */
	unsigned long dwPresentationTime;
} USB_UVC_Payload_Header_PresentationTime;

typedef struct PACK USB_UVC_Payload_Header_PTS_SCR{
	unsigned char  bHeaderLength;
	/** Provides information on the sample data following the header, as well as the */
	/** availability of optional header fields in this header. */
	/** D0: Frame ID ? For frame-based formats, this bit toggles between 0 and 1 every time a
		new video frame begins. For stream-basedformats, this bit toggles between 0 and 1 at
		the start of each new codec-specific segment. */
	/** D1: End of Frame ? This bit is set if the following payload data marks the end of the
		current video or still image frame (for framebased formats), or to indicate the end of a
		codec-specific segment (for stream-based formats). */
	/** D2: Presentation Time ? This bit is set if the dwPresentationTime field is being sent as
		part of the header. */
	/** D3: Source Clock Reference ? This bit is set if the dwSourceClock field is being sent as
		part of the header. */
	/** D5: Still Image ? This bit is set if the following data is part of a still image frame,
		and is only used for methods 2 and 3 of still image capture. */
	/** D6: Error ? This bit is set if there was an	error in the video or still image transmission
		for this payload. The Stream Error Code	control would reflect the cause of the error. */
	/** D7: End of header ? This bit is set if this is the last header group in the packet, where the
		header group refers to this field and any optional fields identified by the bits in this
		field (Defined for future extension). */
	unsigned char bmHeaderInfo;
	/** Presentation Time Stamp (PTS). The source clock time in native device
		clock units when the raw frame capture begins. */
	unsigned long dwPresentationTime;
	/**  Source Time Clock in native device clock units. */
	unsigned long scrSourceClockTimeClock;
	/**  1KHz SOF token counter. Top 5 bits are zero. */
	unsigned short scrSourceClockSOFCounter;
} USB_UVC_Payload_Header_PresentationTime_SourceClock;
//@}


/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBD_UVC_H_ */
