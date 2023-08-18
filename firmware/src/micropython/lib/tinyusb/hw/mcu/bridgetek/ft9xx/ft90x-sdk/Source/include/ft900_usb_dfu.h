/**
	@file ft900_usb_dfu.h

    @brief
    USB DFU class definitions

    This contains USB definitions of constants and structures which are used in
    USB DFU implementations.

    Definitions for USB DFU Devices based on USB Device Firmware Upgrade
    Specification Revision 1.1 from http://www.usb.org/developers/docs/

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

#ifndef FT900_USB_DFU_H
#define FT900_USB_DFU_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

#include "ft900_usb.h"

/* CONSTANTS ***********************************************************************/

/**
    @name   USB Device Class
    @brief  Definitions of USB SubClass and Protocols for DFU Class.
            http://www.usb.org/developers/defined_class
 */
//@{
/// Subclass for all DFU devices
#define USB_SUBCLASS_DFU 0x01
/// Protocol for DFU device in Run Time Mode
#define USB_PROTOCOL_DFU_RUNTIME 0x01
/// Protocol for DFU device in DFU Mode
#define USB_PROTOCOL_DFU_DFUMODE 0x02
//@}

/**
    @name   Bridgetek USB DFU Version Information
    @brief  Definitions of USB DFU Version BCD codes.
 */
//@{
/// Version 1.0 (deprecated)
#define USB_BCD_VERSION_DFU_1_0 0x0100
/// Version 1.1
#define USB_BCD_VERSION_DFU_1_1 0x0110
//@}

/**
    @name   DFU Request Codes
    @brief  Table 3.1 Summary of DFU Class-Specific Requests
            Definitions of DFU request codes.
 */
//@{
/// 5.1. The DFU_DETACH Request
#define USB_CLASS_REQUEST_DETACH     0x00
/// 6.1.1 DFU_DNLOAD Request
#define USB_CLASS_REQUEST_DNLOAD     0x01
/// 6.2.1 DFU_UPLOAD Request
#define USB_CLASS_REQUEST_UPLOAD     0x02
/// 6.1.2 DFU_GETSTATUS Request
#define USB_CLASS_REQUEST_GETSTATUS  0x03
/// 6.1.3 DFU_CLRSTATUS Request
#define USB_CLASS_REQUEST_CLRSTATUS  0x04
/// 6.1.5 DFU_GETSTATE Request
#define USB_CLASS_REQUEST_GETSTATE   0x05
/// 6.1.4 DFU_ABORT Request
#define USB_CLASS_REQUEST_ABORT      0x06
//@}


/**
    @enum   DFU_STATE
    @brief  DFU State Codes. Unnamed table page 22. Device state definitions.
 */
enum DFU_STATE {
    /// Device is running its normal application.
    DFUSTATE_APPIDLE = 0,
    /// Device is running its normal application, has received the
    /// DFU_DETACH request, and is waiting for a USB reset.
    DFUSTATE_APPDETACH = 1,
    /// Device is operating in the DFU mode and is waiting for
    /// requests.
    DFUSTATE_DFUIDLE = 2,
    /// Device has received a block and is waiting for the host to
    /// solicit the status via DFU_GETSTATUS.
    DFUSTATE_DNLOAD_SYNC = 3,
    /// Device is programming a control-write block into its
    /// nonvolatile memories.
    DFUSTATE_DNLOAD_BUSY = 4,
    /// Device is processing a download operation. Expecting
    /// DFU_DNLOAD requests.
    DFUSTATE_DNLOAD_IDLE = 5,
    /// Device has received the final block of firmware from the host.
    DFUSTATE_MANIFEST_SYNC = 6,
    /// Device is in the Manifestation phase.
    DFUSTATE_MANIFEST = 7,
    /// Device has programmed its memories and is waiting for a
    /// USB reset or a power on reset.
    DFUSTATE_MANIFEST_WAIT_RESET = 8,
    /// The device is processing an upload operation. Expecting
    /// DFU_UPLOAD requests.
    DFUSTATE_UPLOAD_IDLE = 9,
    /// An error has occurred. Awaiting the DFU_CLRSTATUS
    /// request.
    DFUSTATE_DFUERROR = 10,
};

/**
    @enum   DFU_STATUS
    @brief  DFU Status Codes. Unnamed table page 21. Device status and error definitions.
 */
enum DFU_STATUS {
    /// No error condition is present.
    DFUSTATUS_OK = 0,
    /// File is not targeted for use by this device.
    DFUSTATUS_ERR_TARGET = 1,
    /// File is for this device but fails some vendor-specific
    /// verification test.
    DFUSTATUS_ERR_FILE = 2,
    ///Device is unable to write memory.
    DFUSTATUS_ERR_WRITE = 3,
    ///Memory erase function failed.
    DFUSTATUS_ERR_ERASE = 4,
    /// Memory erase check failed.
    DFUSTATUS_ERR_CHECK_ERASED = 5,
    /// Program memory function failed.
    DFUSTATUS_ERR_PROG = 6,
    /// Programmed memory failed verification.
    DFUSTATUS_ERR_VERIFY = 7,
    /// Cannot program memory due to received address that is
    /// out of range.
    DFUSTATUS_ERR_ADDRESS = 8,
    /// Received DFU_DNLOAD with wLength = 0, but device does
    /// not think it has all of the data yet.
    DFUSTATUS_ERR_NOTDONE = 9,
    /// Device's firmware is corrupt. It cannot return to run-time
    /// (non-DFU) operations.
    DFUSTATUS_ERR_FIRMWARE = 10,
    /// iString indicates a vendor-specific error.
    DFUSTATUS_ERR_VENDOR = 11,
    /// Device detected unexpected USB reset signaling.
    DFUSTATUS_ERR_USBR = 12,
    /// Device detected unexpected power on reset.
    DFUSTATUS_ERR_POR = 13,
    /// Something went wrong, but the device does not know what it was.
    DFUSTATUS_ERR_UNKNOWN = 14,
    /// Device stalled an unexpected request.
    DFUSTATUS_ERR_STALLEDPKT = 15,
};


/**
    @def    USB_DESCRIPTOR_TYPE_DFU_FUNCTIONAL
    @brief  Identified a Configuration Descriptor as a DFU Function Descriptor with
            fields as laid out in USB_dfu_functional_descriptor.
 **/
#define USB_DESCRIPTOR_TYPE_DFU_FUNCTIONAL 0x21

/**
    @name   USB Functional Descriptor Attributes
    @brief  Bitmap of features supported by the DFU. Used in bmAttributes
    		field of USB_dfu_functional_descriptor.
 **/
//@{
#define USB_DFU_BMATTRIBUTES_CANDNLOAD  			0x01
#define USB_DFU_BMATTRIBUTES_CANUPLOAD  			0x02
#define USB_DFU_BMATTRIBUTES_MANIFESTATIONTOLERANT	0x04
#define USB_DFU_BMATTRIBUTES_WILLDETACH  			0x08
//@}

/* TYPES ***************************************************************************/

/**
    @struct    USB_dfu_status
    @brief      Unnamed table page 20. Structure representing a USB DFU GetStatus return data.
                Structure returned by DFU_GETSTATUS request.
 **/
typedef struct PACK USB_dfu_status {
    /// DFU_STATUS
    unsigned char bStatus;
    /// Minimum time before host should send next DFU_GETSTATUS request.
    unsigned char bwPollTimeout[3];
    /// DFU_STATE
    unsigned char bState;
    /// Index of status description in string table.
    unsigned char iString;
} USB_dfu_status;

/**
    @struct     USB_dfu_functional_descriptor
    @brief      Structure representing a USB DFU Functional Descriptor.
                See USB DFU Spec Table 4.2
 **/
typedef struct PACK USB_dfu_functional_descriptor
{
    /** Size of descriptor in bytes. **/
    uint8_t  bLength;
    /** DFU_FUNCTIONAL descriptor type (always 0x21). **/
    uint8_t  bDescriptorType;
    /** DFU attributes. **/
    uint8_t  bmAttributes;
    /** Detach Timeout. **/
    uint16_t wDetatchTimeOut;
    /** DFU Transfer size. **/
    uint16_t wTransferSize;
    /** DFU Version. **/
    uint16_t bcdDfuVersion;
} USB_dfu_functional_descriptor;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USB_DFU_H_ */
