/**
    @file ft900_usbh_boms.h

    @brief 
    BOMS devices on USB host stack API.
  
    API functions for USB Host BOMS devices. These functions provide functionality
    required to communicate with a BOMS device through the USB Host interface.
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

#ifndef FT900_USBH_BOMS_H_
#define FT900_USBH_BOMS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

/* CONSTANTS ***********************************************************************/

/**
    @name    Definitions for BOMS return values
	@details Set of return values from USB Host API calls.
 */
//@{
/** @brief Success for BOMS function.
 */
#define USBH_BOMS_OK 0
/** @brief Parameter error in call to BOMS function.
 */
#define USBH_BOMS_ERR_PARAMETER -1
/** @brief Device class not supported..
 */
#define USBH_BOMS_ERR_CLASS_NOT_SUPPORTED -2
/** @brief USB error received during SCSI commands.
 */
#define USBH_BOMS_ERR_SCSI -3
/** @brief Error received during status phase.
 */
#define USBH_BOMS_ERR_STATUS -5
/** @brief BOMS class error during function.
 */
#define USBH_BOMS_ERR_CLASS -6
/** @brief Requested LUN is not available.
 */
#define USBH_BOMS_ERR_LUN -7
/** @brief SCSI Get Capacity request timed out.
 */
#define USBH_BOMS_ERR_CAPACITY_TIMEOUT -8
//@}

/**
    @name Block Sizes
    @details Block size(s) supported by library.
 */
//@{
#define USBH_BOMS_BLOCK_SIZE                512
//@}

/* TYPES ***************************************************************************/

/** @brief BOMS device context.
    @details Holds a context structure required by each instance of the driver
 */
typedef struct USBH_BOMS_context {
    USBH_device_handle hBomsDevice; /// USB host device handle for BOMS device.
    USBH_interface_handle hBomsInterface; /// USB host interface handle for BOMS device.
    uint8_t bomsInterfaceNumber; /// Interface number for BOMS device.
    USBH_endpoint_handle hBomsEpIn, hBomsEpOut; /// Handles for IN and OUT endpoints used by BOMS device.
    uint8_t maxLun; /// Maximum LUN supported on this BOMS device.
    uint8_t lun; /// Current LUN in use.
    // lba information
    uint32_t lba_count; /// Logical block count (number of sectors on device).
    uint16_t lba_size; /// Size of logical blocks (sector size).
    // usb device information
    uint16_t vid; /// VID of BOMS device.
    uint16_t pid; /// PID of BOMS device.
    // boms device information
    uint8_t vendorId[8]; /// String containing Vendor Name of BOMS device (may not be NULL terminated).
    uint8_t productId[16]; /// String containing Product Name of BOMS device (may not be NULL terminated).
    uint8_t rev[4]; /// Device specific revision information.
    // private transaction information
    uint32_t tag; /// Library Internal Use Tag.
} USBH_BOMS_context;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/**
    @brief      Initialise the BOMS driver.
    @details	Setup a context for the BOMS driver to use the interfaces and settings
    			provided in the call.
    @param		hBomsInterface - handle to the BOMS interface.
    @param		lun - Logical Unit Number on device to use.
    @param		ctx - structure instantiated in the application to hold the context
    			information for this instance of the driver.
    @returns    USBH_BOMS_OK if successful
 **/
int8_t USBH_BOMS_init(USBH_interface_handle hBomsInterface, uint8_t lun, USBH_BOMS_context *ctx);

/**
    @brief      Read sectors from the BOMS device.
    @details	Read one ore more sectors from the device. This blocks until the
    			required amount of data is read.
    @param		ctx - driver context.
    @param		lba - Logical Block Address (sector number) to commence read.
    @param		len - number of bytes to read. This must be a multiple of the
    			sector size.
    @param		buffer - memory to receive data from on-disk sectors.
    @returns    USBH_BOMS_OK if successful
    			USBH_BOMS_ERR_PARAMETER length is not a multiple of the sector size.
    			USBH_BOMS_ERR_SCSI if a SCSI (protocol error) occurred.
    			USBH_BOMS_ERR_STATUS the device returned a status error.
 **/
int8_t USBH_BOMS_read(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len, uint8_t *buffer);

/**
    @brief      Write sectors to the BOMS device.
    @details	Write one ore more sectors to the device. This blocks until the
    			required amount of data is written.
    @param		ctx - driver context.
    @param		lba - Logical Block Address (sector number) to commence write.
    @param		len - number of bytes to write. This must be a multiple of the
    			sector size.
    @param		buffer - memory to source data from.
    @returns    USBH_BOMS_OK if successful
    			USBH_BOMS_ERR_PARAMETER length is not a multiple of the sector size.
    			USBH_BOMS_ERR_SCSI if a SCSI (protocol error) occurred.
    			USBH_BOMS_ERR_STATUS the device returned a status error.
 **/
int8_t USBH_BOMS_write(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len, uint8_t *buffer);

/**
    @brief      Commence multiple sector reads from the BOMS device.
    @details	Start a read of multiple sectors from the device. The function
    			does not block allowing data to be processed as it is read. This
    			allows large amounts of data to be streamed from the device without
    			using large amounts of memory to hold the data for processing.
    			The USBH_BOMS_mult_read_data() function is used to perform the
    			read - which must take exactly the number of bytes requested -
    			before the USBH_BOMS_mult_end() function completes the read.
    			There must be no other BOMS operations while an multiple sector
    			read operation is in process.
    @param		ctx - driver context.
    @param		lba - Logical Block Address (sector number) to commence read.
    @param		len - number of bytes to read. This must be a multiple of the
    			sector size.
    @returns    USBH_BOMS_OK if successful
    			USBH_BOMS_ERR_PARAMETER length is not a multiple of the sector size.
 **/
int8_t USBH_BOMS_mult_read_start(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len);

/**
    @brief      Read sectors from the BOMS device during a multiple sector read.
    @details	Reads one or more sectors from a device after a multiple sector
    			read has been started with the USBH_BOMS_mult_read_start()
    			function.
    @param		ctx - driver context.
    @param		len - number of bytes to read. This must be a multiple of the
    			sector size.
    @param		buffer - memory to receive data.
    @returns    USBH_BOMS_OK if successful
    			USBH_BOMS_ERR_PARAMETER length is not a multiple of the sector size.
    			USBH_BOMS_ERR_SCSI if a SCSI (protocol error) occurred.
    			USBH_BOMS_ERR_STATUS the device returned a status error.
 **/
int8_t USBH_BOMS_mult_read_data(USBH_BOMS_context *ctx, uint32_t len, uint8_t *buffer);

/**
    @brief      Commence multiple sector writes to the BOMS device.
    @details	Start a write of multiple sectors to the device. The function
    			does not block allowing data to be generated as it is written. This
    			allows large amounts of data to be streamed to the device without
    			using large amounts of memory to hold the data after generating it.
    			The USBH_BOMS_mult_write_data() function is used to perform the
    			write - which must receive exactly the number of bytes requested -
    			before the USBH_BOMS_mult_end() function completes the write.
    			There must be no other BOMS operations while an multiple sector
    			write operation is in process.
    @param		ctx - driver context.
    @param		lba - Logical Block Address (sector number) to commence write.
    @param		len - number of bytes to write. This must be a multiple of the
    			sector size.
    @returns    USBH_BOMS_OK if successful
    			USBH_BOMS_ERR_PARAMETER length is not a multiple of the sector size.
 **/
int8_t USBH_BOMS_mult_write_start(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len);

/**
    @brief      Write sectors to the BOMS device during a multiple sector write.
    @details	Writes one or more sectors to a device after a multiple sector
    			write has been started with the USBH_BOMS_mult_write_start()
    			function.
    @param		ctx - driver context.
    @param		len - number of bytes to write. This must be a multiple of the
    			sector size.
    @param		buffer - memory to source data from.
    @returns    USBH_BOMS_OK if successful
    			USBH_BOMS_ERR_PARAMETER length is not a multiple of the sector size.
    			USBH_BOMS_ERR_SCSI if a SCSI (protocol error) occurred.
    			USBH_BOMS_ERR_STATUS the device returned a status error.
 **/
int8_t USBH_BOMS_mult_write_data(USBH_BOMS_context *ctx, uint32_t len, uint8_t *buffer);

/**
    @brief      Complete a multiple sector write or read.
    @details	Finishes a multiple sector write or read and checks the status.
    @param		ctx - driver context.
    @returns    USBH_BOMS_OK if successful
    			USBH_BOMS_ERR_SCSI if a SCSI (protocol error) occurred.
    			USBH_BOMS_ERR_STATUS the device returned a status error.
 **/
int8_t USBH_BOMS_mult_end(USBH_BOMS_context *ctx);

/**
    @brief      Get the device SCSI status.
    @details	Performs a SCSI Sense operation to retrieve the status of
    			the BOMS device.
    @param		ctx - driver context.
    @returns    USBH_BOMS_OK if successful
    			USBH_BOMS_ERR_SCSI if a SCSI (protocol error) occurred.
    			USBH_BOMS_ERR_STATUS the device returned a status error.
 **/
int8_t USBH_BOMS_status(USBH_BOMS_context *ctx);

/**
    @brief      Reset the BOMS device.
    @details	Performs a BOMS device reset operation.
    @param		ctx - driver context.
    @returns    USBH_BOMS_OK if successful
 **/
int8_t USBH_BOMS_reset(USBH_BOMS_context *ctx);

/**
    @brief      Gets the number of LUNs on the BOMS device.
    @details	Queries the device to find the number of Logical Units
    			on the device.
    @param		ctx - driver context.
    @param		maxLun - the number of the highest numbered LUN on the
    			device.
    @returns    USBH_BOMS_OK if successful
 **/
int8_t USBH_BOMS_get_max_lun(USBH_BOMS_context *ctx, uint8_t *maxLun);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // FT900_USBH_BOMS_H_
