/**
    @file usbd_dfu.c

    @brief
    API for USB device stack DFU on FT900.

    API functions for USB Device DFU stack. These functions provide all the functionality
    required to implement a USB Device DFU interface.

    @version 1.0.0

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
#include <stdlib.h>

/* INCLUDES ************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include <ft900_gpio.h>
#include <ft900_sys.h>
#include <ft900_usb.h>
#include <ft900_usb_dfu.h>
#include <ft900_usbd.h>
#include <ft900_usbd_dfu.h>
#include <ft900_memctl.h>

#include <ft900_delay.h>
#include <ft900_interrupt.h>
#include <registers/ft900_registers.h>
#include <registers/ft900_usbd_registers.h>

/* CONSTANTS ***********************************************************************/

/**
 @brief Page write size for Flash.
 @details The size of the page is defined for the EEPROM.
 	 A page is the smallest area of Flash that can be
 	 programmed. All programming operations must be aligned
 	 on a Flash page and be a multiple of the Flash page in
 	 length.
 */
#define FLASH_PAGE_SIZE 256

/**
 @brief Sector size for Flash.
 @details The size of the sector is defined by the EEPROM.
 	 This is the smallest area of Flash that can be erased.
 */
#define FLASH_SECTOR_SIZE 4096

/**
    @brief There are 16 sectors in a block.
    @details A whole block can be erased.
 */
#define FLASH_BLOCK_SIZE (16 * FLASH_SECTOR_SIZE)
#if defined(__FT930__)
/**
    @brief There are 2 blocks in the flash.
 */
#define FLASH_SIZE (2 * FLASH_BLOCK_SIZE)
#else
/**
    @brief There are 4 blocks in the flash.
 */
#define FLASH_SIZE (4 * FLASH_BLOCK_SIZE)
#endif
/**
 @brief Data memory alignment size required by Flash.
 */
#define MEMORY_ALIGN_SIZE 4

/**
    @brief The topmost 4 kB of the Flash are reserved for the bootloader.
 */
#define DFU_PROTECT_BOOTLOADER
#ifdef DFU_PROTECT_BOOTLOADER
#define FLASH_TOP_ADDRESS (FLASH_SIZE - FLASH_SECTOR_SIZE)
#define UPLOAD_FLASH_TOP_ADDRESS (FLASH_SIZE)
#else // DFU_PROTECT_BOOTLOADER
#define FLASH_TOP_ADDRESS (FLASH_SIZE)
#endif // DFU_PROTECT_BOOTLOADER

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
 @brief Countdown millisecond counter to return to DFUSTATE_APPIDLE
        from DFUSTATE_APPATTACH if a reset is not received within the
        time period specified in DFU_DETACH request. This is updated
        by the dfu_timer function. The application must call this
        function at millisecond intervals to ensure accurate timing.
 */
static volatile uint16_t dfuDetachCounter = 0;

/**
 @brief Current state machine position as defined in enum DFU_STATE.
 */
static uint8_t dfuState = DFUSTATE_APPIDLE;

/**
 @brief Current state machine status as defined in enum DFU_STATUS.
        This can be DFUSTATUS_OK or an error state.
 */
static uint8_t dfuStatus = DFUSTATUS_OK;

/**
 @name  DFU Flash Programming state
 @brief The amount of data received in a DNLOAD or UPLOAD operation
 	 	is normally less than the page size required by the Flash.
 	 	Therefore it is cached in the dfuFlashBuffer until either:
 	 	- A complete page of data has been received.
 	 	- A Zero-length packet is received indicating the end of
 	 	  data transfer.
 	 	- A download to a different page is requested.
 	 	If one of these happens then the buffer is committed to Flash.
 	 	The buffer MUST be MEMORY_ALIGN_SIZE bit aligned.
 	 	The smallest size of Flash which can be erased is a sector,
 	 	this is several pages in size. Therefore when a page is
 	 	written to in a different sector then that sector must be
 	 	erased before it is programmed.
 */
//@{
static uint8_t dfuCurrentSector = 0xff;
static uint32_t dfuCurrentPage = ((uint32_t)-1);
static uint8_t dfuFlashBuffer[FLASH_PAGE_SIZE] __attribute__ ((aligned(MEMORY_ALIGN_SIZE)));
//@}

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* API FUNCTIONS ************************************************************/

uint8_t USBD_DFU_is_runtime(void)
{
	return (dfuState < DFUSTATE_DFUIDLE);
}

uint8_t USBD_DFU_is_wait_reset(void)
{
	return (dfuState == DFUSTATE_MANIFEST_WAIT_RESET);
}

void USBD_DFU_set_dfumode(void)
{
	if (USBD_DFU_is_runtime())
	{
		dfuState = DFUSTATE_DFUIDLE;
	}
}

void USBD_DFU_class_req_detach(uint16_t timeout)
{
	if (dfuState == DFUSTATE_APPIDLE)
	{
		dfuState = DFUSTATE_APPDETACH;
		// Initialise timeout timer
		dfuDetachCounter = timeout;
	}

	/* Respond with an ACK. */
	USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);

#if USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_WILLDETACH
	dfuDetachCounter = 10;
#endif // USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_WILLDETACH
}


void USBD_DFU_class_req_download(uint32_t address, uint16_t dataLength)
{
#if USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_CANDNLOAD

	int8_t status = USBD_OK;

	if (dataLength == 0)
	{
		// Write remnants of any data
		if (dfuCurrentPage != ((uint32_t)-1))
		{
			memcpy_dat2flash(dfuCurrentPage * FLASH_PAGE_SIZE, dfuFlashBuffer, FLASH_PAGE_SIZE);
		}

		if (dfuState == DFUSTATE_DNLOAD_IDLE)
		{
			// Transfer finishes with a zero-length packet.
			dfuState = DFUSTATE_MANIFEST_SYNC;
		}
	}
	// If the data length is within the acceptable size limitations of a page.
	// The check that the address + length of data requested does not straddle
	// a page boundary.
	else if ((dataLength <= FLASH_PAGE_SIZE) &&
			((address % FLASH_PAGE_SIZE) + dataLength <= FLASH_PAGE_SIZE))
	{
		// Prevent writes above the top of MTP memory.
		if ((address + dataLength) <= FLASH_TOP_ADDRESS)
		{
			// Check if we are now programming a different page.
			if ((dfuCurrentPage != ((uint32_t)-1)) &&
					(dfuCurrentPage != (address / FLASH_PAGE_SIZE)))
			{
				// Commit the previous page to the previous sector in Flash.
				memcpy_dat2flash(dfuCurrentPage * FLASH_PAGE_SIZE , dfuFlashBuffer, FLASH_PAGE_SIZE);
			}
			// Calculate the new current page.
			dfuCurrentPage = address / FLASH_PAGE_SIZE;

			// Check if we have moved into a new sector.
			if (dfuCurrentSector != (address / FLASH_SECTOR_SIZE))
			{
				dfuCurrentSector = (address / FLASH_SECTOR_SIZE);
				//At the start of a sector erase the sector.
				flash_sector_erase(dfuCurrentSector);
			}

			// Now change state machine to downloading.
			dfuState = DFUSTATE_DNLOAD_BUSY;

			// Read in data from OUT endpoint.
			USBD_transfer_ep0(USBD_DIR_OUT, &dfuFlashBuffer[address % FLASH_PAGE_SIZE], dataLength, dataLength);

			// Wait for status to be read.
			dfuState = DFUSTATE_DNLOAD_SYNC;
		}
		else
		{
			status = -1;
			// Attempt to write past top block of MTP.
			dfuStatus = DFUSTATUS_ERR_ADDRESS;
		}
	}
	else
	{
		status = -1;
		// Attempt to write more than FLASH_PAGE_SIZE bytes at a time.
		dfuStatus = DFUSTATUS_ERR_WRITE;
	}

	// If failed then burn incoming data.
	if (status != USBD_OK)
	{
		// Read in data from OUT endpoint.
		while (dataLength > FLASH_PAGE_SIZE)
		{
			USBD_transfer_ep0(USBD_DIR_OUT, &dfuFlashBuffer[0], FLASH_PAGE_SIZE, FLASH_PAGE_SIZE);
			dataLength -= FLASH_PAGE_SIZE;
		}
		if (dataLength != 0)
		{
			USBD_transfer_ep0(USBD_DIR_OUT, &dfuFlashBuffer[0], dataLength, dataLength);
		}

		// Undo any changes to the Flash by copying the Shadow RAM back.
		flash_revert();

		dfuState = DFUSTATE_DFUERROR;
	}

	/* Respond with an ACK. */
	USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);

#endif // USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_CANDNLOAD
}

void USBD_DFU_class_req_upload(uint32_t address, uint16_t dataLength)
{
#if USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_CANUPLOAD

	int8_t status = USBD_OK;

	// Prevent writes above the top of MTP memory.
	if ((address + dataLength) <= UPLOAD_FLASH_TOP_ADDRESS)
	{
		if ((dataLength > 0) && (dataLength <= FLASH_PAGE_SIZE) &&
				((address % FLASH_PAGE_SIZE) + dataLength <= FLASH_PAGE_SIZE))
		{
			memcpy_flash2dat(dfuFlashBuffer, address, FLASH_PAGE_SIZE);

			// Read in data from OUT endpoint.
			USBD_transfer_ep0(USBD_DIR_IN, &dfuFlashBuffer[address % FLASH_PAGE_SIZE], dataLength, dataLength);

			dfuState = DFUSTATE_UPLOAD_IDLE;
		}
		else
		{
			status = -1;
			// Attempt to write more than FLASH_PAGE_SIZE bytes at a time.
			dfuStatus = DFUSTATUS_ERR_WRITE;
		}
	}
	else
	{
		// Attempt to write past top block of MTP. Return a short block.
		USBD_transfer_ep0(USBD_DIR_IN, &dfuFlashBuffer[0], 0, dataLength);
		dfuState = DFUSTATE_UPLOAD_IDLE;
	}

	// If failed then send trash data.
	if (status != USBD_OK)
	{
		// Read in data from OUT endpoint.
		while (dataLength > FLASH_PAGE_SIZE)
		{
			USBD_transfer_ep0(USBD_DIR_IN, &dfuFlashBuffer[0], FLASH_PAGE_SIZE, FLASH_PAGE_SIZE);
			dataLength -= FLASH_PAGE_SIZE;
		}
		if (dataLength != 0)
		{
			USBD_transfer_ep0(USBD_DIR_IN, &dfuFlashBuffer[0], dataLength, dataLength);
		}

		dfuState = DFUSTATE_DFUERROR;
	}

	/* Respond with an ACK. */
	USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);

#endif // USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_CANUPLOAD
}

void USBD_DFU_class_req_getstatus(uint16_t requestLen)
{
	// space to hold our response...
	USB_dfu_status buf;

	buf.bStatus = dfuStatus;
	// Ask that there are 1ms between GetStatus requests.
	buf.bwPollTimeout[0] = 1;
	buf.bwPollTimeout[1] = 0;
	buf.bwPollTimeout[2] = 0;
	buf.bState = dfuState;
	buf.iString = 0x00;

	USBD_transfer_ep0(USBD_DIR_IN, (uint8_t *) &buf, sizeof(USB_dfu_status), requestLen);

	if (dfuState == DFUSTATE_DNLOAD_SYNC)
	{
		dfuState = DFUSTATE_DNLOAD_IDLE;
	}
	else if (dfuState == DFUSTATE_MANIFEST_SYNC)
	{
#if !(USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_MANIFESTATIONTOLERANT)
		// Choose not to allow further communications with DFU utility.
		dfuState = DFUSTATE_MANIFEST_WAIT_RESET;
#else // USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_MANIFESTATIONTOLERANT
		// There is no actual manifestation phase which requires action.
		// Move straight to dfuIDLE.
		dfuState = DFUSTATE_DFUIDLE;
#endif // USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_MANIFESTATIONTOLERANT
	}

	/* Respond with an ACK. */
	USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);

#if !(USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_MANIFESTATIONTOLERANT)
	if (USBD_DFU_is_wait_reset())
	{
		USBD_DFU_reset();
	}
#endif // !(USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_MANIFESTATIONTOLERANT)
}

void USBD_DFU_class_req_clrstatus(void)
{
	if (dfuState == DFUSTATE_DFUERROR)
	{
		dfuState = DFUSTATE_DFUIDLE;
	}
	dfuStatus = DFUSTATUS_OK;

	/* Respond with an ACK. */
	USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
}

void USBD_DFU_class_req_abort(void)
{
	if (dfuState > DFUSTATE_DFUIDLE)
	{
		dfuState = DFUSTATE_DFUIDLE;
	}

	/* Respond with an ACK. */
	USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
}

void USBD_DFU_class_req_getstate(uint16_t requestLen)
{
	// space to hold our response...
	uint8_t bState;

	bState = dfuState;

	USBD_transfer_ep0(USBD_DIR_IN, &bState, sizeof(bState), requestLen);

	/* Respond with an ACK. */
	USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);
}

uint8_t USBD_DFU_reset()
{
	if (dfuState == DFUSTATE_APPDETACH)
	{
		// Move to DFU mode.
		dfuState = DFUSTATE_DFUIDLE;
	}
	else if (dfuState == DFUSTATE_MANIFEST_WAIT_RESET)
	{
		dfuState = DFUSTATE_APPIDLE;

#if USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_WILLDETACH
		delayms(50);//while (USBD_ep0_busy(USBD_DIR_OUT) != 0);
		USBD_finalise();
		// Reboot the chip to force a load of Shadow RAM from
		// Flash and restart the firmware with the new code.
		chip_reboot();
#endif // USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_WILLDETACH

		// If not the let the calling program know to perform
		// some 'end of firmware update' action.
		return 1;
	}
	else if (dfuState == DFUSTATE_DNLOAD_IDLE)
	{
		// Incomplete firmware download, report error, reload
		// Shadow RAM into Flash.
		flash_revert();

		dfuState = DFUSTATE_DFUERROR;
	}
	//else if (dfuState == DFUSTATE_DFUIDLE)
	//{
	// Back to starting mode (no reload of firmware).
	// NOTE: This transition is disabled as ft900dfu application (libusb)
	// sends multiple USB bus resets on a usb_reset_ex() call. On a detach
	// This will transition from DFUSTATE_APPIDLE to DFUSTATE_DFUIDLE and
	// back to DFUSTATE_APPIDLE thus preventing DFU mode from being
	// entered.
	//dfuState = DFUSTATE_APPIDLE;
	//}
	// If state is DFUSTATE_DFUERROR then remain in that state until
	// successful firmware download occurs.

	return 0;
}

uint8_t USBD_DFU_timer(void)
{
	uint8_t ret_val = 0;

	if (dfuDetachCounter)
	{
		dfuDetachCounter--;

		if (dfuDetachCounter == 0)
		{
			if (dfuState == DFUSTATE_APPDETACH)
			{
			// If the device is able to detach then we do not need the
			// detach timer to revert to the APPIDLE state.
#if !(USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_WILLDETACH)
			// if detach counter expires then move back to idle state
				dfuState = DFUSTATE_APPIDLE;
				ret_val = 1;
#else // (USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_WILLDETACH)
				static uint8_t force_reset = 0;
			// Disable USB device to force a reset
				if (force_reset == 0)
				{
					force_reset = 1;
					USBD_detach();
					dfuDetachCounter = 10;
				}
				else
				{
					USBD_attach();
					force_reset = 0;
				}
#endif // USBD_DFU_ATTRIBUTES & USB_DFU_BMATTRIBUTES_WILLDETACH
			}
		}
	}
	return ret_val;
}
