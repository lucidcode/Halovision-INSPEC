/**
    @file ft900_usbd_hbw.h

    @brief
    USB Device High Bandwidth Isochronous IN support API on FT90x Rev C onwards.

	@details
    This contains USB Device High Bandwidth Isochronous IN support
    API function definitions, constants and structures
    which are exposed in the API.

    Note that as this is a USB device all transaction nomenclature is from the
    point of view from the host. If the device sends data to the host then it
    is called an IN transaction, if it receives data from the host then it is
    an OUT transaction.
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

#ifndef FT900_USBD_HBW_H_
#define FT900_USBD_HBW_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <ft900_usbd.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/**
    @enum USBD_HBW_HBWMODE
    @brief Enums used to configure whether the endpoint handles one or two or three 1024-byte packets per microframe.
 **/
typedef enum
{
    USBD_HBW_TRANSACTION_1 = 1,		/* Expect 1 ISO IN. (DATA0) */
	USBD_HBW_TRANSACTION_2 = 2,		/* Expect 2 ISO IN. (DATA1/0) */
	USBD_HBW_TRANSACTION_3 = 3,		/* Expect 2 ISO IN. (DATA2/1/0) */
}USBD_HBW_HBWMODE;


/* GLOBAL VARIABLES ****************************************************************/

/* MACROS ****************************************************************/
/**
 @brief Enable Automatic header mode.
 @details Automatically inserts the UVC header (USB_UVC_Payload_Header_PTS)
          and transfers the existing FIFO data to USB bus.
 */
#undef USBD_HBW_ISOCHRONOUS_AUTOHEADER

/* FUNCTION PROTOTYPES *************************************************************/
//@{
/**
    @brief      Initializes HBW pipe and hooks up to a logical endpoint.
    @details    Initializes HBW pipe and hooks up to a logical endpoint.\n
    			This function need to be called after creation of IN endpoint
    			using USBD_create_endpoint()
                         There is a total of 6 kB of RAM for all the endpoints EP1-7
                         (excluding the RAM allocated to endpoint 0).
                         Out of 6kB, upto a maximum of 4kB of fifo size can be
                         configured for HBW isochronous IN pipe.
    @param[in]  ep_number USB IN endpoint number. (N/A for control and OUT endpoints).
    @param[in]  fifo_size Define the FIFO size for HBW ISO IN pipe allocated in SRAM.
    @param[mode] Number of ISO IN transactions in a USB microframe (USBD_HBW_HBWMODE).
 **/
void USBD_HBW_init_endpoint(USBD_ENDPOINT_NUMBER   ep_number,
							uint16_t fifo_size,
							USBD_HBW_HBWMODE mode);

/**
    @brief      Transfer data from a HBW USB endpoint with options.
    @details    The data to be sent on the IN endpoint is copied to the 
				FIFO in SRAM whenever atleast there 1 packet of data space 
				is available.  The offset is useful in case UVC header 
				information is passed from the application (within the current packet) 
				and the data following the header to be copied at an offset of 
				header bytes.
    @param[in]  ep_number USB IN endpoint number.
    @param[in]  buffer Appropriately sized buffer for the transfer.
    @param[in]  length the number of bytes to be sent.
    @param[in]	part   UNUSED
    @param[in]	offset Offset (within the current packet) from where to
     	 	 	 	   continue for subsequent calls when using partial
     	 	 	 	   packets.
    @return     The number of bytes actually transferred.
 **/
int32_t USBD_HBW_iso_transfer(USBD_ENDPOINT_NUMBER   ep_number,
		uint8_t *buffer,
		size_t length,
		uint8_t part,
		size_t offset);
//@}
/**
    @brief      Reads from HW register and indicates HBW FIFO status
    @return     Returns 1 if status of HBW FIFO is full.
    			Returns 0 if not full
 **/
int8_t USBD_HBW_is_fifo_full(void);
/**
    @brief      Reads from HW register and indicates if atleast 1 burst space (1024) available
    @return     Returns 1 if at least 1 burst space for data available.
    			Returns 0 if not enough space
 **/
int8_t USBD_HBW_is_space_avail(void);

#ifdef USBD_HBW_ISOCHRONOUS_AUTOHEADER
/**
    @brief      Sets SEQEND to terminate a video frame so that the hardware automatically generates the frame end payload for UVC
    @return     None
 **/
void USBD_HBW_send_end_of_frame(void);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBD_HBW_H_ */
