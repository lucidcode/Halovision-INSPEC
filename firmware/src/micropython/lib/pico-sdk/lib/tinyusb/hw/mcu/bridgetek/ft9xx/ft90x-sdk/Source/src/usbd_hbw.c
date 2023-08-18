/**
    @file

    @brief
    API for USB High Bandwidth Isochronous IN endpoint support on FT90x Rev C onwards.

    API functions for USB High Bandwidth IN ISO. These functions in addition to the one
    in usbd.c provide all the functionality required to implement a
    USB Device application that uses High Bandwidth ISO
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

/* INCLUDES ************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <ft900.h>
#include <registers/ft900_registers.h>
#include <ft900_gpio.h>
#include <ft900_sys.h>
#include <ft900_delay.h>
#include <ft900_interrupt.h>
#include <ft900_usb.h>
#include <ft900_usbd.h>

#include <ft900_usbd_hbw.h>

/* CONSTANTS ***********************************************************************/
/**
    @name    USB Macros to enable/disable debugging output of the driver
    @details These macros will enable debug output to a linked printf function.
 */
//@{
// Debugging only
#undef USBD_HBW_DEBUG_ISO_TRANSFER
//@}

/**
 @brief Enable Automatic header mode.
 @details Automatically inserts the UVC header (USB_UVC_Payload_Header_PTS)
          and transfers the existing FIFO data to USB bus.
 */
#define USBD_HBW_REG_HBWMODE_SET			0x03
#define	USBD_HBW_REG_MAX_TRANSFER_SIZE		0x0f

#ifdef USBD_HBW_ISOCHRONOUS_AUTOHEADER
#define USBD_HBW_REG_FIFO_SIZE				0x30
#else
#define USBD_HBW_REG_FIFO_SIZE				0x40
#endif

/* TYPES ***************************************************************************/
/* GLOBAL VARIABLES ****************************************************************/
/* LOCAL VARIABLES *****************************************************************/
/* MACROS **************************************************************************/
/* LOCAL FUNCTIONS / INLINES *******************************************************/
/* from USBD.C */
int32_t usbd_in_request(uint8_t ep_number, const uint8_t *buffer, size_t length);

/* API FUNCTIONS ************************************************************/

#ifdef USBD_HBW_ISOCHRONOUS_AUTOHEADER
void USBD_HBW_send_end_of_frame(void)
{
	//Preserve endpoint number hooked and
	//Set SEQDATA_END bit
    USBD_HBW->ctrl3 = ((USBD_HBW->ctrl3) & MASK_USBD_HBW_CTRL3_ENDP_NUM) | MASK_USBD_HBW_CTRL3_SEQDATA_END;
}
#endif

int8_t USBD_HBW_is_space_avail(void)
{
	return (USBD_HBW->ctrl3 & MASK_USBD_HBW_CTRL3_HAVESPACE);
}

int8_t USBD_HBW_is_fifo_full(void)
{
	return (USBD_HBW->ctrl3 & MASK_USBD_HBW_CTRL3_FULL);
}

int32_t USBD_HBW_iso_transfer(USBD_ENDPOINT_NUMBER   ep_number,
		uint8_t *buffer,
		size_t length,
		uint8_t part,
		size_t offset)
{
	size_t packetLen;
	size_t totalLen = length;
	uint8_t *pdata = buffer;
	size_t max_bytes = 1024;
	int32_t transferred = 0;
#ifdef USBD_HBW_DEBUG_ISO_TRANSFER
	int	loop_count = 0;
#endif //USBD_DEBUG_TRANSFER

	// Offset is only interesting if we are part way through a packet.
	// It doesn't matter how many packets through we are.
	offset = offset % max_bytes;

	do
	{
		// Work out the number of bytes to transfer in this packet.
		// Can be up to wMaxPacket size for the endpoint. However, if
		// this is a partial transfer (i.e. will read only a part of a
		// packet) there may be an initial offset to account for.
		packetLen = totalLen;
		if (packetLen > (max_bytes - offset))
		{
			packetLen = (max_bytes - offset);
		}
		// Decrement the total length remaining in the transfer.
		totalLen = totalLen - packetLen;

		{
			// Wait for buffer space to become available.
			while (USBD_HBW->ctrl3 & MASK_USBD_HBW_CTRL3_FULL)
			{
#ifdef USBD_HBW_DEBUG_ISO_TRANSFER
				loop_count++;
				if (loop_count > 500000)
				{
					loop_count = 0;
					tfp_printf ("%d: sr=0x%x, exceeded loop_count\n", ep_number, USBD_HBW->ctrl3);
				}
#endif //USBD_DEBUG_TRANSFER
			}
#ifdef USBD_HBW_DEBUG_ISO_TRANSFER
			tfp_printf ("ctrl3=0x%x,tlen:%d, plen: %d\n", USBD_HBW->ctrl3, totalLen, packetLen);
#endif
			transferred += usbd_in_request(ep_number, pdata, packetLen);

			// If this is the last packet.
			if (totalLen == 0)
			{
				if (part == 0)
				{
					// Acknowledge end of packet if flag is set.
					//USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_INPRDY);
#ifdef USBD_HBW_DEBUG_ISO_TRANSFER
			tfp_printf ("INPRDY set\n");
#endif
#ifndef USBD_HBW_ISOCHRONOUS_AUTOHEADER
					USBD_HBW->ctrl3 |= MASK_USBD_HBW_CTRL3_INPRDY;
#endif
				}
				break;
			}

			// There are still more data to send so just set the INPRDY bit.
			//USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_INPRDY);
		}
		// Move pointer to next chunk of data to send.
		pdata += (max_bytes - offset);
		// By definition offset will now be zero as we have taken that into
		// account at the start of the first packet we send.
		offset = 0;

		// Until all data has been transfered.
	} while (totalLen > 0);

	return transferred;
}

void USBD_HBW_init_endpoint(USBD_ENDPOINT_NUMBER   ep_number,
							uint16_t fifo_size,
							USBD_HBW_HBWMODE mode)
{
	uint32_t reg_data = 0;

	/* Trigger reset to HBW ISO-IN pipe */
	reg_data = MASK_USBD_HBW_CTRL0_HBW_INI;
#ifdef USBD_HBW_ISOCHRONOUS_AUTOHEADER
	/* Enable the insertion of UVC header and transfer the existing FIFO data to
	 * USB bus
	 */
	reg_data |= MASK_USBD_HBW_CTRL0_AUTO_HEADER;
#endif
	
	USBD_HBW->ctrl0 = reg_data;

	/* FIFO size for HBW ISO IN pipe allocated in SRAM (FIFO_SIZE * 64) Bytes */
	USBD_HBW->ctrl1 = ((fifo_size >> 6) & MASK_USBD_HBW_CTRL1_FIFO_SIZE); /* in terms of 64 bytes */

	/* set transaction Mode & Max packet size
	 * Max packet size = (MAXPKTSIZE[3:0] + 1) * 64 Bytes */
	reg_data = USBD_ep_max_size(ep_number);
	reg_data = ((reg_data >> 6) - 1)<<BIT_USBD_HBW_CTRL2_MAXPKTSIZE; /* in terms of 64 bytes */
    reg_data |= (mode << BIT_USBD_HBW_CTRL2_HBWMODE);
    USBD_HBW->ctrl2 = reg_data;

    /* hook up the HBW ISO IN pipe to endpoint number */
	USBD_HBW->ctrl3 = ep_number;

}
