/**
	@file ft900_usbh_aoa.h

	@brief
	Google Open Android Accessory devices on USB host stack API.

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

#ifndef FT900_USBH_AOA_H_
#define FT900_USBH_AOA_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include "ft900_usb_aoa.h"
#include <ft900_usbh.h>

/* CONSTANTS ***********************************************************************/

/**
 @name    Definitions for AOA return values
 @details Set of return values from USB Host API calls.
 */
//@{
/** @brief Success for AOA function.
 */
#define USBH_AOA_OK 0
/** @brief AOA device detected in accessory mode.
 *  @details Device is ready to use.
 */
#define USBH_AOA_DETECTED 1
/** @brief AOA device detected in accessory mode.
 *  @details Device has been restarted and need re-enumerated.
 */
#define USBH_AOA_STARTED 2
/** @brief Parameter error in call to AOA function.
 */
#define USBH_AOA_ERR_PARAMETER -1
/** @brief Device class not supported.
 */
#define USBH_AOA_ERR_CLASS_NOT_SUPPORTED -2
/** @brief Class request not supported.
 */
#define USBH_AOA_ERR_CLASS -3
/** @brief AOA Protocol not supported.
 */
#define USBH_AOA_ERR_PROTOCOL -4
/** @brief AOA Protocol configuration error.
 */
#define USBH_AOA_ERR_CONFIG -5
/** @brief Data Endpoints not found or polling failed.
 */
#define USBH_AOA_ERR_ENDPOINT -6
/** @brief Unexpected USB error occurred.
 */
#define USBH_AOA_ERR_USB -7
//@}

/* TYPES ***************************************************************************/

// AOA Context
//
// Holds a context structure required by each instance of the AOA driver
typedef struct USBH_AOA_context {
	/// Handle to the AOA device. There may be multiple AOA interfaces
	/// on the same devices.
	USBH_device_handle hDevAccessory;
	/// Parent of AOA device. Used to poll for re-enumeration.
	USBH_device_handle hDevParent;
	/// Port of AOA device parent.
	uint8_t parentPort;

	/// VID and PID of device when in accessory mode -
	/// Can be used to work out support for different accessories.
	uint16_t vid, pid;
	/// Interface handle for AOA general interface.
	USBH_interface_handle hAccessoryInterface;
	/// Endpoint handles for the AOA general interface.
	USBH_endpoint_handle hAccessoryEpIn, hAccessoryEpOut;
	/// Interface handle for AOA adb interface.
	USBH_interface_handle hAdbInterface;
	/// Endpoint handles for the AOA adb interface.
	USBH_endpoint_handle hAdbEpIn, hAdbEpOut;
	/// Interface handle for AOA audio interface.
	USBH_interface_handle hAudioInterface;
	/// Audio device interface handle.
	USBH_endpoint_handle hAudioEp;
	/// Maximum packet size of audio isochronous endpoint.
	uint16_t maxIsoSize;
	/// Android Open Accessory Protocol version.
	uint16_t protocol;
	/// Size of HID descriptor.
	uint16_t HIDDescriptorSize;
} USBH_AOA_context;

typedef struct USBH_AOA_descriptors {
	char *manufacturer;
	char *model;
	char *description;
	char *version;
	char *uri;
	char *serial;
} USBH_AOA_descriptors;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/**
 @brief     Initialise the AOA driver.
 @details	Setup a context for the AOA driver to use the interfaces and settings
 	 	 	 provided in the call.
			The sequence for connecting to AOA devices is:
			1) Check AOA protocol is valid. This means that the special vendor SETUP
			command works and the return value is non-zero and matches a protocol
			version supported by the driver.
			2) Send string descriptors to the AOA device using the vendor SETUP
			commands. Then send a start accessory device vendor SETUP command.
			3) The device re-enumerates by doing a device reset.
			4) The host re-enumerates the device as an Android accessory.
			The device will need to be attached using USBH_AOA_attach() once it has
			been re-enumerated.
 @param		hAOAInterface - handle to the AOA interface to use.
 @param		ctx - structure instantiated in the application to hold the context
 	 	 	 information for this instance of the driver.
 @param		descriptors - pointer to a structure containing string descriptors
 	 	 	 to send to the AOA device.
 @param		audio - if the protocol supports audio then send the enable audio
 	 	 	 command to the accessory to enable the audio interface.
 @returns   USBH_AOA_STARTED if an Android accessory device in it's normal mode
 	 	 	 was detected. It will have been started as an accessory and will
 	 	 	 therefore perform a device reset and be re-enumerated.
			USBH_AOA_DETECTED if an Android accessory device in accessory mode
			 was detected. This device can now be attached and used.
			USBH_AOA_ERR_CLASS_NOT_SUPPORTED if a device which is not an Android
			 accessory was detected.
 **/
int8_t USBH_AOA_init(USBH_AOA_context *ctx, USBH_interface_handle hAOAInterface,
		USBH_AOA_descriptors *descriptors, int16_t audio);

//@{
/**
 @brief     Attaches to the AOA device.
 @details	Connects to the AOA device which is in accessory mode. It will
 	 	 	 decode the AOA protocol, VID and the PID to determine support for
 	 	 	 accessories, adb bridge and audio. Endpoints and size information
 	 	 	 is stored for use by the driver later.
 @param		ctx - context of AOA device to use.
 @returns   USBH_AOA_OK if successful
 	 	 	USBH_AOA_ERR_CONFIG if a device reporting to possess a particular
 			 interface does not, in fact, present that interface.
 			USBH_AOA_ERR_CLASS_NOT_SUPPORTED if a device which is not an Android
			 accessory was detected.
 **/
int8_t USBH_AOA_attach(USBH_AOA_context *ctx);

/**
 @brief     Detaches from the AOA device.
 @param		ctx - context of AOA device to use.
 @returns   USBH_AOA_OK if successful
 **/
int8_t USBH_AOA_detach(USBH_AOA_context *ctx);

/**
 @brief     Gets the protocol revision of the AOA device.
 @details	Returns the protocol revision for the AOA device. This can be
 used to determine support for the different types of
 accessories.
 @param		ctx - context of AOA device to use.
 @param		protocol - pointer to BCD protocol revision.
 @returns   USBH_AOA_OK if successful
 **/
int8_t USBH_AOA_get_protocol(USBH_AOA_context *ctx, uint16_t *protocol);
//@}

//@{
/**
 @brief     AOA device supports accessories.
 @details	Queries the type of device that can connect to the AOA device.
 @param		ctx - context of AOA device to use.
 @returns	This will return a positive value if the device supports the
 accessory class. Zero if it does not. Negative if there was an
 error.
 **/
int8_t USBH_AOA_has_accessory(USBH_AOA_context *ctx);

/**
 @brief     AOA device supports audio.
 @details	Queries the type of device that can connect to the AOA device.
 @param		ctx - context of AOA device to use.
 @returns	This will return a positive value if the device supports the
 audio class. Zero if it does not. Negative if there was an
 error.
 **/
int8_t USBH_AOA_has_audio(USBH_AOA_context *ctx);

/**
 @brief     AOA device supports adb.
 @details	Queries the type of device that can connect to the AOA device.
 @param		ctx - context of AOA device to use.
 @returns	This will return a positive value if the device supports the
 adb class. Zero if it does not. Negative if there was an
 error.
 **/
int8_t USBH_AOA_has_adb(USBH_AOA_context *ctx);
//@}

/**
 @brief     Gets a handle to the audio endpoint.
 @details	If the AOA device supports it then the audio isochronous endpoint
 can be obtained with this call.
 @param		ctx - context of AOA device to use.
 @param		hAudio - pointer to handle to receive audio endpoint.
 @param		maxSize - maximum size of data transferred on the audio endpoint.
 @returns    USBH_AOA_OK if successful
 **/
int8_t USBH_AOA_get_audio_endpoint(USBH_AOA_context *ctx,
		USBH_endpoint_handle *hAudio, uint16_t *maxSize);

//@{
/**
 @brief     Register a new HID device.
 @details	Tell the AOA device that a new HID is to be registered.
 @param		ctx - context of AOA device to use.
 @param		hidID - ID of new HID device.
 @param		descriptorSize - number of bytes in HIDs report descriptor. This
 is sent separately as it can be changed.
 @returns    USBH_AOA_OK if successful
 **/
int8_t USBH_AOA_register_hid(USBH_AOA_context *ctx, uint16_t hidID,
		uint16_t descriptorSize);

/**
 @brief     Unregister a HID device.
 @details	Tell the AOA device that the HID is to be unregistered.
 @param		ctx - context of AOA device to use.
 @param		hidID - ID of previously registered HID device.
 @returns    USBH_AOA_OK if successful
 **/
int8_t USBH_AOA_unregister_hid(USBH_AOA_context *ctx, uint16_t hidID);

/**
 @brief     Set the HID descriptor for a HID device.
 @details	Tell the AOA device the format of HID reports sent from the HID device.
 @param		ctx - context of AOA device to use.
 @param		hidID - ID of previously registered HID device.
 @param		descriptorOffset - used when the HID descriptor is sent in multiple
 	 	 	 packets. This is the offset to the position in the descriptor where
 	 	 	 this fragment goes.
 @param		descriptorLength - length of this section of HID descriptor. If the
 	 	 	 whole HID descriptor is sent in one packet then this will be the
 	 	 	 same as the length set in descriptorSize parameter of
 	 	 	 USBH_AOA_register_hid. If the descriptor is made up of multiple
 	 	 	 packets then the length will be smaller.
 @param		descriptor - HID descriptor.
 @returns    USBH_AOA_OK if successful
 **/
int8_t USBH_AOA_set_hid_report_descriptor(USBH_AOA_context *ctx, uint16_t hidID,
		uint16_t descriptorOffset, uint16_t descriptorLength, uint8_t *descriptor);

/**
 @brief     Send a report descriptor to the AOA device.
 @details	Sends a formatted report descriptor matching the last HID descriptor
 to the AOA device.
 @param		ctx - context of AOA device to use.
 @param		hidID - ID of HID device.
 @param		reportSize - number of bytes in HIDs report.
 @param		data - report data.
 @returns    USBH_AOA_OK if successful
 **/
int8_t USBH_AOA_send_hid_data(USBH_AOA_context *ctx, uint16_t hidID,
		uint16_t reportSize, uint8_t *data);

//@}

int32_t USBH_AOA_accessory_write(USBH_AOA_context *ctx, uint8_t *buffer, size_t len);
int32_t USBH_AOA_accessory_read(USBH_AOA_context *ctx, uint8_t *buffer, size_t len);
int32_t USBH_AOA_accessory_read_async(USBH_AOA_context *ctx, uint8_t *buffer, size_t len,
		uint32_t id, USBH_callback cb);
int32_t USBH_AOA_accessory_write_async(USBH_AOA_context *ctx, uint8_t *buffer, size_t len,
		uint32_t id, USBH_callback cb);

int32_t USBH_AOA_adb_write(USBH_AOA_context *ctx, uint8_t *buffer, size_t len);
int32_t USBH_AOA_adb_read(USBH_AOA_context *ctx, uint8_t *buffer, size_t len);

int32_t USBH_AOA_audio_read_async(USBH_AOA_context *ctx, uint8_t *buffer,
		uint32_t id, USBH_callback cb);
		
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBH_AOA_H_ */
