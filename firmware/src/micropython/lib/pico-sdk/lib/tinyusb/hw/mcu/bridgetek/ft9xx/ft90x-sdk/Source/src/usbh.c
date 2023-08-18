/**
    @file

    @brief 
    API for USB Host stack on FT900.

    API functions for USB Host stack. These functions provide all the functionality
    required to implement a USB Host application.
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
#include <ft900_asm.h>
#include <registers/ft900_registers.h>
#include <ft900_gpio.h>
#include <ft900_sys.h>
#include <ft900_interrupt.h>
#include <ft900_delay.h>
#include <ft900_usb.h>
#include <private/ft900_usbh_internal.h>
#include <ft900_usbh.h>

/* CONSTANTS ***********************************************************************/


/* USBH Memory Map
 *
 * There is 8kB of RAM For USB Host. This covers the following used:
 *  Periodic Frame List - schedule for periodic transfers for INTERRUPT and
 *      ISOCHRONOUS transfers. (1kB, 2kB or 4Kb in size)
 *  Async Transfer List - list of queue heads for BULK and CONTROL transfers.
 *      Various sizes depending on number of endpoints supported.
 *  Transfer Descriptors - descriptors for ISOCHRONOUS, INTERRUPT, BULK and
 *      CONTROL endpoint transfers. Always one per endpoint.
 *      Various sizes depending on number of endpoints supported and number
 *      of concurrent transfers.
 *  Endpoint buffers - memory for receiving data to and sending data from
 *      for each endpoint. This is a multiple of the maximum packet size for
 *      each endpoint, depending on the number of cuncurrent transactions
 *      for the endpoint.
 *
 * EHCI Offset  CPU Offset  (dec)   Suggest
 * 0x0000       0x11000     0kB     Periodic Frame List (256 entries)
 * 0x0400       0x11400     1kB     Queue Element Transfer Descriptors,
 * 0x0800       0x11800     2kB      or Queue Head Descriptors,
 * 0x0C00       0x11C00     3kB      or Isochronous Transfer Descriptors.
 * 0x1000       0x12000     4kB      or Endpoint buffers (7kB total).
 * 0x1400       0x12400     5kB
 * 0x1800       0x12800     6kB
 * 0x1C00       0x12C00     7kB
 *
 *
 *
 */
#undef EHCI_MEM
#undef EHCI_REG
#define EHCI_MEM(A) (*(uint32_t *)&(A))
#define EHCI_REG(A) (*(uint32_t volatile *)&(EHCI->A))

/**
    @name    USB Device, Interface and Endpoint allocation presets.
    @details These macros are used to calculate how many entries are
             needed for USBH_interface, USBH_device and USBH_endpoint
             structures.
 */
//@{
#ifndef USBH_COUNT_DEVICES
/** Suggested size is for one hub with several devices connected.
 * This can be altered by recompiling to suit the application.
 */
#define USBH_COUNT_DEVICES 8
#endif // USBH_COUNT_DEVICES
#ifndef USBH_COUNT_INTERFACES
/** Suggested size is based on two interfaces per device.
 * Typical devices: Flash disk - one interface; CDC - 2 interfaces;
 * HIDs - one or two interfaces; FT232 - one interface; FT4232H -
 * four interfaces.
 */
#define USBH_COUNT_INTERFACES (USBH_COUNT_DEVICES * 4)
#endif // USBH_COUNT_INTERFACES
#ifndef USBH_COUNT_ENDPOINTS
/** Suggested size is sufficient for one control endpoint per
 *  device and a pair of IN and OUT endpoints for each interface.
 */
#define USBH_COUNT_ENDPOINTS (USBH_COUNT_DEVICES + (USBH_COUNT_INTERFACES * 2))
#endif // USBH_COUNT_ENDPOINTS
#ifndef USBH_COUNT_TRANSFERS
/** Suggested size is sufficient for one asynchronous or periodic
 *  transaction per interface.
 *  */
#define USBH_COUNT_TRANSFERS (USBH_COUNT_INTERFACES * 2)
#endif // USBH_COUNT_TRANSFERS
//@}

/**
    @name    USB Macros to enable/disable features of the driver
    @details These macros will enable fetures that are either not normally
    		 required or are optional in the specification.
 */
//@{
/** Perform indicator changed SetPortFeature when a hub is detected. TODO!
 */
#define USBH_ENABLE_HUB_INDICATOR
/** Disable filling in all fields in root hub status response in the structure
 * USB_hub_port_status. Not all bits are mandatory and optional ones are excluded
 * by undefining this macro.
 */
#define USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
//@}

/**
	@details Configure driver to use interrupts (define) or polling mode (undef).
 */
#define USBH_USE_INTERRUPTS

/**
    @name    USB Macros to enable/disable debugging output of the driver
    @details These macros will enable debug output to a linked printf function.
 */
//@{
// Debugging only
#undef USBH_LIBRARY_DEBUG_ENABLED
#ifdef USBH_LIBRARY_DEBUG_ENABLED
#undef USBH_DEBUG_ENUM
#undef USBH_DEBUG_SETUP
#undef USBH_DEBUG_ERROR
#undef USBH_DEBUG_LISTS
#undef USBH_DEBUG_CONFIG_DESC
#undef USBH_DEBUG_PERIODIC_LIST
#undef USBH_DEBUG_MEM
#undef USBH_DEBUG_ALLOC
#undef USBH_DEBUG_XFER
#undef USBH_DEBUG
#undef USBH_DEBUG_ISO_ALLOC
#include <ft900_uart_simple.h>
#include "../tinyprintf/tinyprintf.h"
#endif // USBH_LIBRARY_DEBUG_ENABLED
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/** @brief  Pointer to first device.
 *  The first device connected to the root hub forms the start of the list of
 *  devices. If this is a hub then it will have children and/or peers. If a non-hub
 *  device is connected then there may be peers.
 */
static USBH_device *usbh_firstDev;

/** @brief  List of active Control and Bulk USB transfers.
 *  This list contains a description of each active transfer. It is used to calculate
 *  timeouts and find callback functions when transfers complete. Transfers are
 *  added to this list when they are queued and removed from this list when they
 *  complete, timeout or are removed.
 */
static USBH_xfer *usbh_xferList;

/** @brief  List of active Periodic USB transfers.
 *  This list contains a description of each active transfer. It is used to calculate
 *  timeouts and find callback functions when transfers complete. Transfers are
 *  added to this list when they are queued and removed from this list when they
 *  complete, timeout or are removed.
 */
static USBH_xfer *usbh_xferListPeriodic;

/** @brief  Periodic list of QHs.
 *  The periodic list contains a list of QHs ordered from long polling intervals to
 *  short intervals. This list is traversed by the periodic list. This list
 *  terminates with MASK_EHCI_QUEUE_HEAD_TERMINATE bit set.
 *  This list is 'live' so care must be taken when inserting and removing entries.
 *  Address is a CPU address.
 */
/*static*/ USBH_qh *usbh_periodic_header_qh;

/** @brief  Asynchronous list of QHs.
 *  The asynchronous list contains a list of QHs in no particular order. It is
 *  traversed by the asynchronous list. This list must wrap around from the last
 *  entry to the first entry to enable the asynchronous list to access all
 *  devices.
 *  This list is 'live' so care must be taken when inserting and removing entries.
 *  Address is a CPU address.
 */
static USBH_qh *usbh_async_header_qh;

/** @brief  Period list of Endpoints.
 *  This list is analogous to the usbh_periodic_header_qh and the 'queue' member
 *  of each of the entries in this list points to an entry in the
 *  usbh_periodic_header_qh list. These lists MUST be in the same order.
 *  Entries in this list are linked by the 'list_next' member, NOT the 'next'
 *  member.
 */
/*static*/ USBH_endpoint *usbh_periodic_ep_list;

/** @brief  Asynchronous list of Endpoints.
 *  This list is analogous to the usbh_async_header_qh and the 'queue' member
 *  of each of the entries in this list points to an entry in the
 *  usbh_async_header_qh list. This list MUST be in the same order as the
 *  usbh_async_header_qh list.
 *  Entries in this list are linked by the 'list_next' member, NOT the 'next'
 *  member. This list must NOT wrap from the last entry to the first entry.
 */
static USBH_endpoint *usbh_async_ep_list;

/**
    @name    EHCI Root hub and port statuses
    @details Keep track of changes in root hub and root hub port status.
 **/
//@{
static USB_hub_port_status currentPortStatus;
static USB_hub_status currentHubStatus;
//@}

/**
    @name    ECHI RAM addresses.
    @details Definitions for default EHCI RAM layout.
             These are used by the library and will not be required to be
             used in the API.
 */
//@{
static uint32_t ehci_periodic_addr = EHCI_PERIODIC_ADDR;
static uint32_t ehci_periodic_size = EHCI_PERIODIC_SIZE;
//@}

/**
    @name    ECHI RAM allocation.
    @details The EHCI RAM can be allocated to qTDs, QHs, iTD etc and
             endpoint buffers. The RAM is aligned on 32 byte boundaries
             to suit alignment restrictions on structures. Software
             requests a block for a transaction and releases it after.
             The rules for alignment are as follows:
             QHs must be aligned on 64 byte boundaries
             qTDs must be aligned on 32 byte boundaries
 */
//@{
/// Maximum size of RAM is EHCI_RAM_SIZE bytes.
static uint8_t ehci_ram_alloc[EHCI_RAM_SIZE / EHCI_RAM_ALIGN];
//@}

/**
    @name    USB Device, Interface and Endpoint allocation.
    @details These arrays are used to allocate storage for
             USBH_interface, USBH_device and USBH_endpoint
             structures. This is in normal data section and
             not in the EHCI RAM area.
             Allocation is checked using the enumValue field,
             a non-zero value indicating that the array
             entry is used.
 */
static USBH_device usbh_device_alloc[USBH_COUNT_DEVICES];
static USBH_interface usbh_iface_alloc[USBH_COUNT_INTERFACES];
static USBH_endpoint usbh_endpoint_alloc[USBH_COUNT_ENDPOINTS];
static USBH_xfer usbh_transfer_alloc[USBH_COUNT_TRANSFERS];
//@}

/**
    @name    Enumeration variables
    @details An enumerated device is allocated a unique address
             within the USB bus. The devAddr is a reasonable method
             of doing this. Really, only a device repeatedly plugged
             and unplugged from a downstream hub will eventually be
             allocated a duplicate address.
             The enumeration value (uniqueEnumValue) is a fairly
             unique method of telling if an enumeration event has
             removed or reused a handle.
             The enumResourcesFull flag indicates theat there were
             insufficient devices, interface, endpoint or transfer
             resources to complete enumeration. It may be set
             asynchronously after devices downstream of a hub are
             enumerated.
 */
//@{
static uint16_t uniqueEnumValue = 1;
static uint8_t devAddr = 1;
static uint8_t enumResourcesFull = 0;
static uint8_t enumInProgress = 0;
//@}

/**
    @name    Enumeration callback
    @details When a device is enumerated the function specified
    		 during USBH_initialise will be called. This will allow
    		 the application to find new devices being added to the
    		 USB host port or check if devices already there have
    		 been removed.
 */
USBH_callback usbh_enum_change_callback;

/**
    @name    Interrupt signal variables
    @details Volatile flags to signal that an interrupt of a
    		 particular type has been received. These are set by
    		 the ISR function and cleared (with interrupts
    		 disabled temporarily) when the interrupt signal has
    		 been received by the USB processing function.
 */
//@{
static volatile uint8_t usbh_intr_xfer;
static volatile uint8_t usbh_intr_port;
//@}

/**
    @name    Callback active blocker
    @details The callback function of asynchronous transfers allows
    		 a handler function to perform SETUP or blocking transfers
    		 on the USB. This must be prevented to stop nested callbacks
    		 being performed. If this flag is set then do NOT complete
    		 transfers with callbacks.
 */
//@{
static uint8_t usbh_xfer_callback_active;
//@}
/**
    @name    AOA device compatibility delay
    @details For some AOA devices delay is needed in the USBH_Process()
    		 before signalling all the completed transactions for non-Period transfers.
    		 The delay is in micro seconds and is provided through USBH_ctx.
 */
//@{
static uint8_t usbh_aoa_compat_delay = 0;
//@}

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static inline void usbh_memcpy32_mem2hc(void *dst0, void *src0, uint32_t len)
{
	asm_streamouti32(src0, dst0, len);
}

static inline void usbh_memcpy8_hc2mem(void *dst0, void *src0, uint32_t len)
{
	asm_streamini8(src0, dst0, len);
}

static inline void usbh_memcpy8_mem2hc(void *dst0, void *src0, uint32_t len)
{
	asm_streamouti32(src0, dst0, len);
}

static inline uint32_t usbh_get_periodic_size(void)
{
	return ehci_periodic_size;
}

static inline uint32_t usbh_get_periodic_addr(void)
{
	return ehci_periodic_addr;
}

static inline void usbh_write_periodic_list(uint32_t offset, uint32_t pointer)
{
	EHCI_RAM[EHCI_RAM_OFFSET(usbh_get_periodic_addr()) + offset] = pointer;
}

static inline uint32_t usbh_read_periodic_list(uint32_t offset)
{
	return EHCI_RAM[EHCI_RAM_OFFSET(usbh_get_periodic_addr()) + offset];
}


/* FUNCTIONS ***********************************************************************/

/** @brief Interrupt handler for USB Host stack.
 */
static void ISR_usbh(void)
{
#ifdef USBH_USE_INTERRUPTS
	uint32_t intStatus;

	// Clear complete and error bits
	intStatus = usbh_interrupt();

	if (intStatus & (MASK_EHCI_USBSTS_USBERR_INT | MASK_EHCI_USBSTS_USB_INT))
	{
		usbh_intr_xfer = 1;
	}

	if (intStatus & MASK_EHCI_USBSTS_PO_CHG_DET)
	{
		usbh_intr_port = 1;
	}

#endif // USBH_USE_INTERRUPTS
}

static void usbh_unlink_qtd(USBH_xfer *xferThis)
{
	uint32_t token;
	USBH_qh *hc_this_qh = xferThis->hc_entry.qh;
	USBH_qtd *hc_this_qtd = xferThis->hc_qtd;
	USBH_endpoint *epThis = xferThis->endpoint;
	USBH_qtd *hc_prev_qtd;

	// Check transfer endpoint queues match.
	if (hc_this_qh != epThis->hc_entry.qh)
	{
#ifdef USBH_DEBUG_XFER
		tfp_printf("Unlink qTD Error: Endpoint pointer to qTD does not match Xfer\r\n");
#endif // USBH_DEBUG_XFER
		return;
	}

	// Deactivate target qTD.
	//CRITICAL_SECTION_BEGIN
	token = EHCI_MEM(hc_this_qtd->token);
	token &= (~EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE);
	// Deactivate it.
	EHCI_MEM(hc_this_qtd->token) = token;
	//CRITICAL_SECTION_END

	// Check if the qh->current_qtd points to the qTD of the transfer.
	// If it does then the overlay is from the qTD we need to unlink.
	if (EHCI_HC_TO_CPU(EHCI_MEM(hc_this_qh->qtd_current)) == (uint32_t)hc_this_qtd)
	{
		// Stop the transfer in the overlay.
		token &= (~MASK_EHCI_QUEUE_TD_TOKEN_TOGGLE);
		token |= (EHCI_MEM(hc_this_qh->transfer_overlay.token) & MASK_EHCI_QUEUE_TD_TOKEN_TOGGLE);

		//CRITICAL_SECTION_BEGIN
		EHCI_MEM(hc_this_qh->transfer_overlay.token) = token;
		//CRITICAL_SECTION_END
		// Wait for a short time to ensure that the controller passes this
		// qTD and writes back the overlay.
		delayms(1);
		// Re-write the overlay.
		EHCI_MEM(hc_this_qh->transfer_overlay.token) = token;
	};


	// Go through queue headers list of qTDs to find the right qTD to remove.
	hc_prev_qtd = epThis->hc_head_qtd;

	if (hc_prev_qtd == hc_this_qtd)
	{
		// The qTD to unlink is the first one in the endpoint's list.
#ifdef USBH_DEBUG_XFER
		tfp_printf("Unlink qTD: first entry unlinked (overlay)\r\n");
#endif // USBH_DEBUG_XFER
	}
	else
	{
		do {
			// If the next entry is the qTD to unlink then stop.
			if (EHCI_NEXT_TD(EHCI_MEM(hc_prev_qtd->next)) == EHCI_CPU_TO_HC(hc_this_qtd))
			{
				break;
			}
			if (EHCI_TERMINATE_TD(EHCI_MEM(hc_prev_qtd->next)))
			{
				break;
			}
			hc_prev_qtd = (USBH_qtd *)EHCI_HC_TO_CPU(EHCI_NEXT_TD(EHCI_MEM(hc_prev_qtd->next)));
			// Stop if we reach the right qTD.
		} while (hc_prev_qtd != hc_this_qtd);

		if (hc_prev_qtd == hc_this_qtd)
		{
#ifdef USBH_DEBUG_XFER
			tfp_printf("Unlink qTD Error: could not find xfer qtd\r\n");
#endif // USBH_DEBUG_XFER
			return;
		}

		// Bypass this qTD in the list.
		EHCI_MEM(hc_prev_qtd->next) = EHCI_NEXT_TD(EHCI_MEM(hc_this_qtd->next)) |
				EHCI_TERMINATE_TD(EHCI_MEM(hc_prev_qtd->next));

#ifdef USBH_DEBUG_XFER
		tfp_printf("Unlink qTD: unlinked mid-list entry\r\n");
#endif // USBH_DEBUG_XFER

		// Deactivate QH current aTD overlay.
		if (EHCI_MEM(hc_this_qh->qtd_current) == EHCI_HC_TO_CPU(hc_this_qtd))
		{
			EHCI_MEM(hc_this_qh->transfer_overlay.next) = EHCI_MEM(hc_this_qh->next);

#ifdef USBH_DEBUG_XFER
			tfp_printf("Unlink qTD: unlinked mid-list overlay\r\n");
#endif // USBH_DEBUG_XFER
		}
	}
}

static uint16_t usbh_xfer_iso_td_insert(USBH_xfer *xferNew, uint16_t advance)
{
	uint32_t itd_link;
	uint32_t prev_link;
	uint16_t this_frindex;
	uint16_t last_frindex;
	uint16_t earliest_frindex;
	uint16_t next_frame;
	uint16_t next_microframe;
	uint32_t token;

	// Add the iTD or siTD into the periodic list.

	// Work out the new link pointer in the periodic list.
	itd_link = EHCI_CPU_TO_HC(xferNew->hc_entry.iTD);
	if (xferNew->hc_entry.type == USBH_list_entry_type_siTD)
	{
		itd_link |= MASK_EHCI_QUEUE_HEAD_TYPE_siTD;
		// Note: Do not allow siTDs to change split starts from the
		// first microframe in a frame to increase the likelihood of
		// TT completing all before the end of the frame.
		token = 0;
	}
	else //if (xferNew->hc_entry.type == USBH_list_entry_type_iTD)
	{
		itd_link |= MASK_EHCI_QUEUE_HEAD_TYPE_iTD;
		// Allow the transaction to start in any microframe.
		token = EHCI_MEM(xferNew->hc_entry.iTD->status_control_list[0]);
	}

	// Can add transfer to a minimum of 2 microframes into the future.
	// uAdvnance and fAdvance set to zero means as soon as possible
	// therefore the next timeslot to use is in 2 microframe times.
	if (advance == 0)
	{
		advance = 2;
	}

	// Next allowable polling slot will begin at or after the last
	// scheduled transaction for the endpoint + the polling interval.
	last_frindex = xferNew->endpoint->last_microframe;
	earliest_frindex = xferNew->endpoint->last_microframe +
			xferNew->endpoint->interval;
	// Normalise frindex.
	earliest_frindex &= MASK_EHCI_FRINDEX;

	CRITICAL_SECTION_BEGIN

	// Get the current frame/microframe.
	this_frindex = USBH_get_frame_number();
	// Target time slot is a number of microframes in the future.
	this_frindex += advance;
	// Normalise frindex.
	this_frindex &= MASK_EHCI_FRINDEX;

	// The calcuations here have base values within the 14-bits of
	// the FRINDEX register, the 16-bit calculation is safe from
	// overflow.
	if ((this_frindex >= last_frindex)
			&& (this_frindex < earliest_frindex))
	{
		// Target time slot is the next allowable timeslot.
		this_frindex = earliest_frindex;
	}
	else if (earliest_frindex < last_frindex)
	{
		if ((this_frindex >= last_frindex)
				|| (this_frindex < earliest_frindex))
		{
			// Target time slot is the next allowable timeslot.
			this_frindex = earliest_frindex;
		}
	}

	// Get the frame number for the next time slot.
	next_frame = (this_frindex >> 3);
	// Make sure we do wrap at the end of the list of frames.
	next_frame &= (usbh_get_periodic_size() - 1);
	// Get the microframe number for the next time slot.
	next_microframe = this_frindex & 7;

	// Move the active token to the right microframe for the transfer.
	// Since token is zero fo siTD this is not performed for split transfers.
	if (next_microframe != 0)
	{
		if (xferNew->hc_entry.type == USBH_list_entry_type_iTD)
		{
			EHCI_MEM(xferNew->hc_entry.iTD->status_control_list[0]) = 0;
			EHCI_MEM(xferNew->hc_entry.iTD->status_control_list[next_microframe]) = token;
		}
	}
	// Update the (s)iTD to the existing pointer in the periodic list.
	prev_link = usbh_read_periodic_list(next_frame);
	EHCI_MEM(xferNew->hc_entry.iTD->next) = prev_link;
	usbh_write_periodic_list(next_frame, itd_link);

	CRITICAL_SECTION_END

	// Store this microframe number for later use.
	xferNew->microframe = this_frindex;
	xferNew->endpoint->last_microframe = this_frindex;

#ifdef USBH_DEBUG_PERIODIC_LIST
	tfp_printf("Insert iTD: added iso to periodic schedule\r\n");
#endif // USBH_DEBUG_PERIODIC_LIST

	return next_frame;
}

static uint8_t usbh_xfer_iso_td_remove(USBH_xfer *xferOld)
{
	int8_t status = USBH_ERR_NOT_FOUND;
	uint32_t link;
	USBH_itd *hc_prev_itd;
	uint16_t this_frame;
	uint16_t this_microframe;

	this_microframe = xferOld->microframe;
	this_frame = this_microframe >> 3;
	this_frame &= (usbh_get_periodic_size() - 1);

	link = usbh_read_periodic_list(this_frame);

	if (EHCI_NEXT_QH(link) == EHCI_CPU_TO_HC((uint32_t)xferOld->hc_entry.iTD))
	{
		// This is the first entry in the frame list.
		link = EHCI_MEM(xferOld->hc_entry.iTD->next);
		usbh_write_periodic_list(this_frame, link);

#ifdef USBH_DEBUG_PERIODIC_LIST
		tfp_printf("Remove iTD: removed first\r\n");
#endif // USBH_DEBUG_PERIODIC_LIST

		status = USBH_OK;
	}
	else
	{
		// Follow until the end.
		hc_prev_itd = (USBH_itd *)EHCI_HC_TO_CPU(EHCI_NEXT_QH(link));

		do {
			link = EHCI_MEM(hc_prev_itd->next);

			if (EHCI_NEXT_QH(link) == EHCI_CPU_TO_HC(xferOld->hc_entry.iTD))
			{
				EHCI_MEM(hc_prev_itd->next) = EHCI_MEM(xferOld->hc_entry.iTD->next);
				status = USBH_OK;

#ifdef USBH_DEBUG_PERIODIC_LIST
				tfp_printf("Remove iTD: remove mid-list\r\n");
#endif // USBH_DEBUG_PERIODIC_LIST
				break;
			}
			hc_prev_itd = (USBH_itd *)EHCI_HC_TO_CPU(EHCI_NEXT_QH(link));

		} while ((EHCI_TYPE_QH(link) != MASK_EHCI_QUEUE_HEAD_TYPE_QH)
				&& (EHCI_TERMINATE_QH(link) == 0));
	}
#ifdef USBH_DEBUG_PERIODIC_LIST
	tfp_printf("Remove iTD: remove done\r\n");
#endif // USBH_DEBUG_PERIODIC_LIST

	return status;
}

static void usbh_update_xfer_timeouts(USBH_xfer *list)
{
	// NOTE: At interrupt level
	USBH_xfer *xferThis;
	uint32_t token;
	uint8_t microframe;

	// Update all timeouts for non-Periodic transfers
	xferThis = list;

	// Decrement timeout on all current transactions
	while (xferThis)
	{

		if (xferThis->timeout > 0)
		{
			xferThis->timeout--;
			if (xferThis->timeout == 0)
			{
				if (xferThis->hc_entry.type == USBH_list_entry_type_qH)
				{
					token = EHCI_MEM(xferThis->hc_qtd->token);
					if (token & EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE)
					{
						EHCI_MEM(xferThis->hc_qtd->token) = token & ~EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE;

						// Timeout expired.
						usbh_unlink_qtd(xferThis);

						xferThis->status = USBH_ERR_TIMEOUT;
						// Expired transaction is removed by USBH_process

#ifdef USBH_DEBUG_XFER
						tfp_printf("Xfer: timeout qH %08x qTD %08x token %08x\r\n", xferThis->hc_entry.qh, xferThis->hc_qtd, token);
#endif // USBH_DEBUG_XFER
					}
				}
				else if (xferThis->hc_entry.type == USBH_list_entry_type_siTD)
				{
					token = EHCI_MEM(xferThis->hc_entry.siTD->transfer_state);

					if (token & EHCI_SPLIT_ISO_TD_STATUS_ACTIVE)
					{
						EHCI_MEM(xferThis->hc_entry.siTD->transfer_state) = token & ~EHCI_SPLIT_ISO_TD_STATUS_ACTIVE;

						xferThis->status = USBH_ERR_TIMEOUT;
						// Expired transaction is removed by USBH_process

#ifdef USBH_DEBUG_XFER
						tfp_printf("Xfer: timeout siTD %08x token %08x\r\n", xferThis->hc_entry.siTD, token);
#endif // USBH_DEBUG_XFER
					}
				}
				else if (xferThis->hc_entry.type == USBH_list_entry_type_iTD)
				{
					// Check all 8 microframe entries.
					token = 0;
					for (microframe = 0; microframe < 8; microframe++)
					{
						token |= EHCI_MEM(xferThis->hc_entry.iTD->status_control_list[microframe]);
					}

					if (token & EHCI_ISO_TD_STATUS_ACTIVE)
					{
						// Deactivate all microframes.
						for (microframe = 0; microframe < 8; microframe++)
						{
							token = EHCI_MEM(xferThis->hc_entry.iTD->status_control_list[microframe]);
							EHCI_MEM(xferThis->hc_entry.iTD->status_control_list[microframe]) = token & ~EHCI_ISO_TD_STATUS_ACTIVE;
						}

						xferThis->status = USBH_ERR_TIMEOUT;
						// Expired transaction is removed by USBH_process
#ifdef USBH_DEBUG_XFER
						tfp_printf("Xfer: timeout iTD %08x token %08x\r\n", xferThis->hc_entry.iTD, token);
#endif // USBH_DEBUG_XFER
					}
				}

				//CRITICAL_SECTION_BEGIN
				// Signal an interrupt to ensure USBH_process picks this up
				// and removed expired transactions.
				usbh_intr_xfer = 1;
				//CRITICAL_SECTION_END
			}
		}
		xferThis = xferThis->next;
	};
}

static int8_t usbh_hub_change(uint32_t id, int8_t status, size_t len, uint8_t *change)
{
	uint8_t hubPort;
	USBH_device *hubDev = (USBH_device *)id;
	uint8_t mask;
	USBH_device *devNew = NULL;
	USBH_interface *ifaceHub;
	USBH_endpoint *epHub;

	// Unused parameter.
	(void)len;

	// Entered as a callback when a hub notifies us that has a change pending.
	if (status == USBH_OK)
	{
		// Received change notification from hub.
		// Work out which port on the hub has changed state.
		for (hubPort = 1; hubPort <= hubDev->portCount; hubPort++)
		{
			mask = change[hubPort / 8];
			mask >>= (hubPort & 7);

			if (mask & 1)
			{
				// And update that port.
#ifdef USBH_DEBUG_ENUM
				tfp_printf("Enumeration: Hub notification enumeration change port %d\r\n", hubPort);
#endif
				usbh_hub_port_update(hubDev, hubPort, 0, &devNew);

#ifdef USBH_DEBUG_ENUM
				tfp_printf("Enumeration: Hub notification enumeration done\r\n");
#endif
			}
		}

		// Resubmit job.
		ifaceHub = hubDev->interfaces;
		if (ifaceHub)
		{
			if (ifaceHub->set)
			{
				// Get Status Change Endpoint
				epHub = ifaceHub->endpoints;

				if (epHub)
				{
					if (usbh_transfer_queued_worker(epHub,
							(uint8_t *)&hubDev->hubStatusChange, // Memory location for result.
							epHub->max_packet_size,
							0, // No timeout.
							(uint32_t)hubDev, // "ID" received by the callback function.
							&usbh_hub_change) // Callback function.
							!= USBH_OK)
					{
#ifdef USBH_DEBUG_ERROR
						tfp_printf("Hub: could not submit new read status change\r\n");
#endif // USBH_DEBUG_ERROR
					}
				}
			}
		}
	}

	return USBH_OK;
}

static void usbh_update_transactions(USBH_xfer **list)
{
	USBH_xfer *xferThis;

	// Signal all the completed transactions for non-Period transfers
	xferThis = *list;
	int8_t status;

	while (xferThis)
	{
		// Is xferThis completed?
		status = usbh_xfer_complete(xferThis);
		if (status != USBH_NOT_COMPLETE)
		{
			// If the transaction was successful...
			if (xferThis->status == USBH_OK)
			{
				if (xferThis->endpoint)
				{
					// If this is an IN request then copy data
					if (xferThis->endpoint->direction == USBH_DIR_IN)
					{
						// Copy data from EHCI RAM to program RAM.
						if (xferThis->callback)
						{
							usbh_memcpy8_hc2mem(xferThis->dest_buf, xferThis->hc_buf, xferThis->hc_len);
						}
					}
				}
			}

			// Free the EHCI RAM used to store the response
			if ((xferThis->callback) && (xferThis->hc_buf))
			{
				usbh_free_hc_buffer(xferThis->hc_buf, xferThis->hc_len);
			}

			// Perform callback for completion
			if (xferThis->callback)
			{
				// Only perform a callback if there are no active
				// callbacks to prevent runaway nesting.
				//CRITICAL_SECTION_BEGIN
				if (!usbh_xfer_callback_active)
				{
					USBH_endpoint *epThis;

					usbh_xfer_callback_active = 1;
					//CRITICAL_SECTION_END

					epThis = xferThis->endpoint;

					if (epThis->hc_entry.type != USBH_list_entry_type_qH)
					{
						// Remove the transfer's siTD or iTD from the period frame list.
						usbh_xfer_iso_td_remove(xferThis);
					}

					if (epThis->hc_entry.type == USBH_list_entry_type_qH)
					{
						if (epThis->hc_head_qtd == xferThis->hc_qtd)
						{
							epThis->hc_head_qtd = (USBH_qtd *)EHCI_HC_TO_CPU(EHCI_NEXT_TD(EHCI_MEM(xferThis->hc_qtd->next)));
						}

						usbh_free_hc_1_block(xferThis->hc_qtd);
					}
					else if (epThis->hc_entry.type == USBH_list_entry_type_iTD)
					{
						usbh_free_hc_2_blocks(xferThis->hc_entry.iTD);
					}
					else //if (epThis->hc_entry.type == USBH_list_entry_type_siTD)
					{
						usbh_free_hc_1_block(xferThis->hc_entry.siTD);
					}

					xferThis->callback(xferThis->id, xferThis->status,
							xferThis->dest_len, xferThis->dest_buf);

					//CRITICAL_SECTION_BEGIN
					usbh_xfer_callback_active = 0;
					//CRITICAL_SECTION_END
#ifdef USBH_DEBUG_XFER
					tfp_printf("Xfer: updated qH/siTD/iTD %08x\r\n", xferThis->hc_entry.qh);
#endif // USBH_DEBUG_XFER

					usbh_xfer_remove(xferThis, list);
					usbh_free_transfer(xferThis);
				}
				else
				{
					//CRITICAL_SECTION_END
				}
			}
		}

		// Move to next item in list
		xferThis = xferThis->next;
	}; // while
}

/** @brief Periodic List initialisation.
 */
/*
 * Periodic list is 256 entries long.
 * This entries in this list provide a resolution of one frame (1 mS). Giving
 * a total span of 1 mS to 256 mS. The FRINDEX counter bits 11:4 give the offset
 * from the PERIODICLISTBASE to a pointer for a QH or iTD. Bits 3:0 are used in
 * the QH or iTD to do some interframe scheduling with the S-Mask bitmask.
 * Therefore each periodic (INTERRUPT) entry must be arranged depending on the
 * bInterval of the endpoint (smaller values toward the end, larger ones to the
 * start) and the PERIODICLIST making links into the list at the required places
 * to accurately simulate the polling intervals.
 * It's assumed that every endpoint will act like a High Speed endpoint and support
 * a power of 2 polling interval. This will be rounded up on Low/Full Speed
 * endpoints to the next smaller power of 2. e.g. 10 mS will actually poll at
 * 8 mS.
 */
static void usbh_periodic_init(void)
{
	uint32_t i;
	uint32_t reg;

	// Start the periodic list with a dummy entry
	usbh_periodic_ep_list = usbh_alloc_endpoint();
	usbh_periodic_ep_list->enumValue = -1;
	usbh_periodic_ep_list->type = USBH_EP_INT;
	// Set the interval to maximum
	usbh_periodic_ep_list->interval = -1;

	// Assign a dummy QH to this dummy endpoint entry
	usbh_periodic_header_qh = (USBH_qh *)usbh_alloc_hc_2_blocks();
	// Assign the dummy QH to this dummy endpoint entry
	usbh_periodic_ep_list->hc_entry.qh = usbh_periodic_header_qh;
	usbh_periodic_ep_list->hc_entry.type = USBH_list_entry_type_qH;
	// Allocate a dummy aTD for this endpoint (qH).
	usbh_periodic_ep_list->hc_dummy_qtd = (USBH_qtd *)usbh_alloc_hc_1_block();
	usbh_init_qtd(usbh_periodic_ep_list->hc_dummy_qtd);
	usbh_periodic_ep_list->hc_head_qtd = usbh_periodic_ep_list->hc_dummy_qtd;

	// Terminate list
	EHCI_MEM(usbh_periodic_header_qh->next) = EHCI_CPU_TO_HC(usbh_periodic_header_qh) |
			MASK_EHCI_QUEUE_HEAD_TYPE_QH | MASK_EHCI_QUEUE_HEAD_TERMINATE;

#ifdef USBH_DEBUG_SETUP
	tfp_printf("Setup: Initialising Periodic EP list at %08x\r\n", (uint32_t)usbh_periodic_ep_list);
	tfp_printf("Setup: Initialising Periodic QH list at %08x\r\n", (uint32_t)usbh_periodic_header_qh);
#endif // USBH_DEBUG_SETUP

	if (usbh_get_periodic_size() == 0x400)
	{
#ifdef USBH_DEBUG_SETUP
		tfp_printf("Setup: Periodic Table Size 4 kB, 1k entries\r\n");
#endif // USBH_DEBUG_SETUP
		reg = EHCI_USBCMD_FRL_SIZE_1024;
	}
	else if (usbh_get_periodic_size() == 0x200)
	{
#ifdef USBH_DEBUG_SETUP
		tfp_printf("Setup: Periodic Table Size 2 kB, 512 entries\r\n");
#endif // USBH_DEBUG_SETUP
		reg = EHCI_USBCMD_FRL_SIZE_512;
	}
	else // usbh_get_periodic_size() == 0x100
	{
#ifdef USBH_DEBUG_SETUP
		tfp_printf("Setup: Periodic Table Size 1 kB, 256 entries\r\n");
#endif // USBH_DEBUG_SETUP
		reg = EHCI_USBCMD_FRL_SIZE_256;
	}

	// Set the periodic frame element size to required size.
	EHCI_REG(periodiclistaddr) = usbh_get_periodic_addr();
	EHCI_REG(usbcmd) = EHCI_REG(usbcmd) | reg;

	// Initialise a dummy Periodic frame list of 256 entries starting
	// immediately after the frame list.
	reg = EHCI_CPU_TO_HC(usbh_periodic_header_qh) | MASK_EHCI_QUEUE_HEAD_TYPE_QH | MASK_EHCI_QUEUE_HEAD_TERMINATE;

	for (i = 0; i < usbh_get_periodic_size(); i++)
		usbh_write_periodic_list(i, reg);

	if (EHCI_REG(usbsts) & MASK_EHCI_USBSTS_PSCH_STS)
	{
#ifdef USBH_DEBUG_SETUP
		tfp_printf("Setup: Periodic Schedule Running\r\n");
#endif // USBH_DEBUG_SETUP
	}
	else
	{
		EHCI_REG(usbcmd) = EHCI_REG(usbcmd) | MASK_EHCI_USBCMD_PSCH_EN;
#ifdef USBH_DEBUG_SETUP
		tfp_printf("Setup: Periodic Schedule Enabled\r\n");
#endif // USBH_DEBUG_SETUP
	}
}

/** @brief Asynchronous List initialisation.
 */
static void usbh_asynch_init(void)
{
	usbh_async_ep_list = usbh_alloc_endpoint();
	usbh_async_ep_list->enumValue = -1;

	// Assign a dummy QH to this dummy endpoint entry
	usbh_async_header_qh = (USBH_qh *)usbh_alloc_hc_2_blocks();
	usbh_async_ep_list->hc_entry.qh = usbh_async_header_qh;
	usbh_async_ep_list->hc_entry.type = USBH_list_entry_type_qH;
	// Set the interval to maximum
	usbh_async_ep_list->interval = -1;

	// Allocate a dummy qTD for this endpoint (qH).
	usbh_async_ep_list->hc_dummy_qtd = 0;
	usbh_async_ep_list->hc_head_qtd = usbh_async_ep_list->hc_dummy_qtd;

#ifdef USBH_DEBUG_SETUP
	tfp_printf("Setup: Initialising Async EP list at 0x%08x\r\n", (uint32_t)usbh_async_ep_list);
	tfp_printf("Setup: Initialising Async QH list at 0x%08x\r\n", (uint32_t)usbh_async_header_qh);
#endif // USBH_DEBUG_SETUP

	// Point to self
	EHCI_MEM(usbh_async_header_qh->next) = EHCI_CPU_TO_HC(usbh_async_header_qh) |
			MASK_EHCI_QUEUE_HEAD_TYPE_QH;
	EHCI_MEM(usbh_async_header_qh->ep_char_1) = (1 << BIT_EHCI_QUEUE_HEAD_EP_CHAR_H);
	EHCI_MEM(usbh_async_header_qh->ep_capab_2) = 0x00000000;

	EHCI_MEM(usbh_async_header_qh->transfer_overlay.next) = MASK_EHCI_QUEUE_TD_TERMINATE;//EHCI_CPU_TO_HC(usbh_async_ep_list->dummy);
	EHCI_MEM(usbh_async_header_qh->transfer_overlay.alt_next) = MASK_EHCI_QUEUE_TD_TERMINATE;//EHCI_CPU_TO_HC(usbh_async_ep_list->dummy);
	EHCI_MEM(usbh_async_header_qh->transfer_overlay.token) = EHCI_QUEUE_TD_TOKEN_STATUS_HALTED;

	EHCI_REG(asynclistaddr) = EHCI_CPU_TO_HC(usbh_async_header_qh);

	if (EHCI_REG(usbsts) & MASK_EHCI_USBSTS_ASCH_STS)
	{
#ifdef USBH_DEBUG_SETUP
		tfp_printf("Setup: Asynchronous Schedule Running\r\n");
#endif // USBH_DEBUG_SETUP
	}
	else
	{
		EHCI_REG(usbcmd) = EHCI_REG(usbcmd) | MASK_EHCI_USBCMD_ASCH_EN;
#ifdef USBH_DEBUG_SETUP
		tfp_printf("Setup: Asynchronous Schedule Enabled\r\n");
#endif // USBH_DEBUG_SETUP
	}
}

static int8_t usbh_ep_list_add(USBH_endpoint *epThis, USBH_endpoint *epListHead, uint32_t listType)
{
	// Derive queue header to add to list from endpoint
	USBH_qh *hc_this_qh = epThis->hc_entry.qh;
	// Derive starting queue header
	USBH_qh *hc_end_qh = epListHead->hc_entry.qh;
	USBH_qh *hc_prev_qh = NULL;
	USBH_endpoint *epEnd = epListHead;
	USBH_endpoint *epPrev = NULL;

	// Move through the endpoint list and find where this endpoint needs added.
	// This will be, for interrupt endpoints, in decending order of the value
	// of the interval member of USBH_endpoint.
	// All endpoint lists start with a dummy entry (so there is no need to pass
	// pointers to pointers around). On the periodic list, the dummy entry
	// terminates the QH list, but is the start of the endpoint list.
	while (epEnd)
	{
		// Check the list consistency. The queue of the endpoint must match
		// the queue header in the queue header list.
		if (epEnd->hc_entry.qh != hc_end_qh)
		{
			// List consistency error
#ifdef USBH_DEBUG_LISTS
			tfp_printf("List add error: list inconsistent\r\n");
#endif // USBH_DEBUG_LISTS
			return USBH_ERR_USBERR;
		}

		if (epEnd->type == USBH_EP_INT)
		{
			if (epEnd->interval < epThis->interval)
			{
#ifdef USBH_DEBUG_LISTS
				tfp_printf("List add: Interrupt interval new %d previous %d\r\n", epThis->interval, epEnd->interval);
#endif // USBH_DEBUG_LISTS
				// The new endpoint to add has an interval less than the point Insert before end_ep in the list.
				// If this is the first pass then add to end of list (after dummy entry).
				break;
			}
		}

		epPrev = epEnd;

		hc_prev_qh = hc_end_qh;
		hc_end_qh = (USBH_qh *)EHCI_HC_TO_CPU(EHCI_NEXT_QH(EHCI_MEM(hc_end_qh->next)));

		epEnd = epEnd->list_next;
	}

	if (epEnd->type != USBH_EP_INT)
	{
		EHCI_MEM(hc_this_qh->next) |= MASK_EHCI_QUEUE_HEAD_TERMINATE;
	}

	if (/*(epEnd != NULL) &&*/ (epPrev != NULL))
	{
		epThis->list_next = epPrev->list_next;
		epPrev->list_next = epThis;

		if (hc_prev_qh != NULL)
		{
			// Insert or add QH into the QH list
			// This referencing type is set to the type of the previous entry.
			EHCI_MEM(hc_this_qh->next) = EHCI_NEXT_QH(EHCI_MEM(hc_prev_qh->next)) |
					EHCI_TERMINATE_QH(EHCI_MEM(hc_this_qh->next)) |
					EHCI_TYPE_QH(EHCI_MEM(hc_prev_qh->next));

			// Set the terminate bit if we are pointing to the list head qH.
			if (EHCI_NEXT_QH(EHCI_MEM(hc_prev_qh->next)) == EHCI_CPU_TO_HC(hc_end_qh))
			{
				if (epThis->type == USBH_EP_INT)
				{
					EHCI_MEM(hc_this_qh->next) |= MASK_EHCI_QUEUE_HEAD_TERMINATE;
					EHCI_MEM(hc_prev_qh->next) &= (~MASK_EHCI_QUEUE_HEAD_TERMINATE);
				}
			}

			// Previous forward referencing type is set to the type of this entry.
			EHCI_MEM(hc_prev_qh->next) = EHCI_CPU_TO_HC(hc_this_qh) |
					EHCI_TERMINATE_QH(EHCI_MEM(hc_prev_qh->next)) |
					EHCI_TYPE_QH(listType);

			return USBH_OK;
		}
	}

	// Didn't find either the end of either list or the start of endpoint list.
#ifdef USBH_DEBUG_LISTS
	tfp_printf("List add error: list not found\r\n");
#endif // USBH_DEBUG_LISTS
	return USBH_ERR_NOT_FOUND;
}

static int8_t usbh_ep_list_remove(USBH_endpoint *epThis, USBH_endpoint *epListHead)
{
	USBH_qh *hc_this_qh = epThis->hc_entry.qh;
	USBH_qh *hc_end_qh = epListHead->hc_entry.qh;
	USBH_qh *hc_prev_qh = NULL;
	USBH_endpoint *epEnd = epListHead;
	USBH_endpoint *epPrev = NULL;
	uint32_t link;
	uint32_t i;

	// Move through the endpoint list and find where this endpoint is located.
	// All endpoint lists start with a dummy entry (so there is no need to pass
	// pointers to pointers around). On the periodic list, the dummy entry
	// terminates the QH list, but is the start of the endpoint list.
	while (epEnd)
	{
		// Check the list consistency. The queue of the endpoint must match
		// the queue header in the queue header list.
		if (epEnd->hc_entry.qh != hc_end_qh)
		{
#ifdef USBH_DEBUG_LISTS
			tfp_printf("List Remove error: list inconsistent\r\n");
#endif // USBH_DEBUG_LISTS
			// List consistency error
			return USBH_ERR_USBERR;
		}

		if (epThis == epEnd)
		{
			// Found endpoint to remove
			break;
		}

		hc_prev_qh = hc_end_qh;
		hc_end_qh = (USBH_qh *)EHCI_HC_TO_CPU(EHCI_NEXT_QH(EHCI_MEM(hc_end_qh->next)));

		epPrev = epEnd;
		epEnd = epEnd->list_next;
	}

	if ((epEnd != NULL) && (epPrev != NULL))
	{
		// Miss out this endpoint in the list.
		// This does not require any synchronisation with the host controller.
		epPrev->list_next = epThis->list_next;

		if (hc_prev_qh != NULL)
		{
			// Bypass this QH on the QH list
			// Previous forward referencing type is set to the type of this entry.
			// Preserve a terminate bit from either the removed item or the
			// next item.
			EHCI_MEM(hc_prev_qh->next) = EHCI_NEXT_QH(EHCI_MEM(hc_this_qh->next))
											| EHCI_TYPE_QH(EHCI_MEM(hc_prev_qh->next))
											| 	(EHCI_TERMINATE_QH(EHCI_MEM(hc_this_qh->next))
											| EHCI_TERMINATE_QH(EHCI_MEM(hc_prev_qh->next))
											);

			// Point this QH to somewhere that will continue to be here.
			link = EHCI_MEM(hc_this_qh->next);

			// Doorbell ringing only required for asynchronous lists.
			if (epListHead == usbh_async_ep_list)
			{
				usbh_hw_wait_for_doorbell();
			}

			// If this is a periodic entry then remove it from any slots that is
			// appears in in the periodic list.
			if (epListHead == usbh_periodic_ep_list)
			{
				for (i = 0; i < usbh_get_periodic_size(); i++)
				{
					if (EHCI_NEXT_QH(usbh_read_periodic_list(i)) == EHCI_CPU_TO_HC(hc_this_qh))
					{
						usbh_write_periodic_list(i, link);
#ifdef USBH_DEBUG_PERIODIC_LIST
						tfp_printf("List remove: Changed periodic entry %d to %08x\r\n", i, link);
#endif // USBH_DEBUG_PERIODIC_LIST
					}
				}
			}
#ifdef USBH_DEBUG_LISTS
			tfp_printf("List remove: done\r\n");
#endif // USBH_DEBUG_LISTS

#if 0
			// DOUBLE CHECK START
			epEnd = epListHead;
			hc_end_qh = epListHead->hc_entry.qh;
			while (epEnd)
			{
				if (epEnd->hc_entry.qh != hc_end_qh)
				{
					tfp_printf("List remove error: list inconsistent\r\n");
					// List consistency error
					return USBH_ERR_USBERR;
				}

				hc_prev_qh = hc_end_qh;
				hc_end_qh = (USBH_qh *)EHCI_HC_TO_CPU(EHCI_NEXT_QH(EHCI_MEM(hc_end_qh->next)));

				epPrev = epEnd;
				epEnd = epEnd->list_next;
			}
			// DOUBLE CHECK END
#endif
			return USBH_OK;
		}
	}

	// Didn't find either the end of either list or the start of endpoint list.
#ifdef USBH_DEBUG_LISTS
	tfp_printf("List remove error: list not found\r\n");
#endif // USBH_DEBUG_LISTS
	return USBH_ERR_NOT_FOUND;
}

static USBH_endpoint *usbh_ep_list_find_periodic(uint16_t interval)
{
	USBH_endpoint *end_ep = usbh_periodic_ep_list;

	// Move through the endpoint list and find where the first endpoint with the
	// required interval (or less) is found.
	while (end_ep)
	{
		if (end_ep->interval <= ((interval * 2) - 1))
		{
			// Insert before end_ep in the list
			// If this is the first pass then add to end of list (after dummy entry).
			break;
		}

		end_ep = end_ep->list_next;
	}

	return end_ep;
}

static int8_t usbh_init_ep_qh(USBH_list_entry *hc_this_entry, USBH_qh *hc_ctrl_qh, USBH_endpoint *this_ep)
{
	int8_t status = USBH_ERR_PARAMETER;

	// Only works for control, bulk and interrupt endpoints.
	// Isochronous use iTD and siTDs.
	if (this_ep->type != USBH_EP_ISOC)
	{
		uint32_t epchar;
		uint32_t epcapab;

		// Add a queued endpoint for BULK or INTERRUPT.
		hc_this_entry->type = USBH_list_entry_type_qH;

		// Forward referencing type will be updated when this is inserted in a list.
		EHCI_MEM(hc_this_entry->qh->next) = MASK_EHCI_QUEUE_HEAD_TYPE_QH;
		// Take control endpoint characteristics and mask out those that change for
		// this non-control endpoint.
		epchar = EHCI_MEM(hc_ctrl_qh->ep_char_1);
		epchar &= (~(MASK_EHCI_QUEUE_HEAD_EP_CHAR_ENDPOINT |
				MASK_EHCI_QUEUE_HEAD_EP_CHAR_MAX_PACKET |
				MASK_EHCI_QUEUE_HEAD_EP_CHAR_C |
				MASK_EHCI_QUEUE_HEAD_EP_CHAR_DTC));
		// Respond to all microframe offsets for interrupt endpoints.
		epcapab = EHCI_MEM(hc_ctrl_qh->ep_capab_2);
		// Preserve only hub and port from control endpoint.
		epcapab &= (MASK_EHCI_QUEUE_HEAD_EP_CAPAB_HUB_ADDR | MASK_EHCI_QUEUE_HEAD_EP_CAPAB_HUB_PORT);

		if (this_ep->type != USBH_EP_BULK)
		{
			// For interrupt endpoints set microframe masks.
			if ((EHCI_MEM(hc_ctrl_qh->ep_char_1) & MASK_EHCI_QUEUE_HEAD_EP_CHAR_EPS) == EHCI_QUEUE_HEAD_EP_CHAR_EPS_HS)
			{
				// Periodic entry actioned on one of the 8 microframes only.
				// Use the endpoint number as a shortcut to this.
				epcapab = epcapab | (1 << ((this_ep->index - 1) & 0x07))
						| ((0x1c1c << ((this_ep->index - 1) & 0x07)) & MASK_EHCI_QUEUE_HEAD_EP_CAPAB_UFRAME_CMASK);
				// Turn off the NAK counter reload for interrupt endpoints
				epchar &= (~MASK_EHCI_QUEUE_HEAD_EP_CHAR_RL);
			}
			else
			{
				epcapab = epcapab | (1 << BIT_EHCI_QUEUE_HEAD_EP_CAPAB_UFRAME_SMASK);
				// Turn off the NAK counter reload for interrupt endpoints
				epchar &= (~MASK_EHCI_QUEUE_HEAD_EP_CHAR_RL);
			}
		}
		// Update missing parts in the endpoint characteristics.
		EHCI_MEM(hc_this_entry->qh->ep_char_1) = epchar
				| (this_ep->max_packet_size << BIT_EHCI_QUEUE_HEAD_EP_CHAR_MAX_PACKET)
				| (this_ep->index << BIT_EHCI_QUEUE_HEAD_EP_CHAR_ENDPOINT);

		//epcapab |= MASK_EHCI_QUEUE_HEAD_EP_CAPAB_UFRAME_CMASK;
		//epcapab |= (1 << BIT_EHCI_QUEUE_HEAD_EP_CAPAB_UFRAME_SMASK);
		EHCI_MEM(hc_this_entry->qh->ep_capab_2) = ((this_ep->additional + 1) << BIT_EHCI_QUEUE_HEAD_EP_CAPAB_MULT) | epcapab;

		// Add a dummy qTD for the endpoint qH
		this_ep->hc_dummy_qtd = (USBH_qtd *)usbh_alloc_hc_1_block();
		usbh_init_qtd(this_ep->hc_dummy_qtd);
		this_ep->hc_head_qtd = this_ep->hc_dummy_qtd;

		// Setup transfer overlay to point to dummy qTD
		EHCI_MEM(hc_this_entry->qh->transfer_overlay.next) = EHCI_CPU_TO_HC(this_ep->hc_dummy_qtd);
		EHCI_MEM(hc_this_entry->qh->transfer_overlay.alt_next) = EHCI_CPU_TO_HC(this_ep->hc_dummy_qtd);

		// For BULK endpoints link in to list of asynch QHs
		if (this_ep->type == USBH_EP_INT)
		{
			// For Periodic endpoints link in to list of periodic QHs
			status = usbh_ep_list_add(this_ep, usbh_periodic_ep_list, MASK_EHCI_QUEUE_HEAD_TYPE_QH);
		}
		else // if ((this_ep->type == USBH_EP_BULK) || (this_ep->type == USBH_EP_CTRL))
		{
			status = usbh_ep_list_add(this_ep, usbh_async_ep_list, MASK_EHCI_QUEUE_HEAD_TYPE_QH);
		}
	}

	return status;
}

static void usbh_update_ctrlep(USBH_qh *hc_ctrl_qh,
		uint8_t speed, uint8_t newaddr,
		uint8_t hub, uint8_t hubPort)
{
	uint32_t imaxpktlen;
	uint32_t control;
	uint32_t hssp_type;
	uint32_t ep_char;

	// Default max packet sizes. Overridden by device descriptor.
	imaxpktlen = (8 << BIT_EHCI_QUEUE_HEAD_EP_CHAR_MAX_PACKET);
	control = (1 << BIT_EHCI_QUEUE_HEAD_EP_CHAR_C);

	if (speed == USBH_SPEED_FULL)
	{
		hssp_type = EHCI_QUEUE_HEAD_EP_CHAR_EPS_FS;
	}
	if (speed == USBH_SPEED_LOW)
	{
		hssp_type = EHCI_QUEUE_HEAD_EP_CHAR_EPS_LS;
	}
	if (speed == USBH_SPEED_HIGH)
	{
		hssp_type = EHCI_QUEUE_HEAD_EP_CHAR_EPS_HS;
		imaxpktlen = (64 << BIT_EHCI_QUEUE_HEAD_EP_CHAR_MAX_PACKET);
		control = 0;
	}

	// Characteristics (preserve H-bit).
	ep_char = EHCI_MEM(hc_ctrl_qh->ep_char_1) & MASK_EHCI_QUEUE_HEAD_EP_CHAR_H;
	ep_char |= (imaxpktlen |
			control | // Control endpoint
			(MASK_EHCI_QUEUE_HEAD_EP_CHAR_DTC) |
			hssp_type |
			//NOT(MASK_EHCI_QUEUE_HEAD_EP_CHAR_ENDPOINT) |
			(newaddr << BIT_EHCI_QUEUE_HEAD_EP_CHAR_ADDRESS) |
			MASK_EHCI_QUEUE_HEAD_EP_CHAR_RL);
	EHCI_MEM(hc_ctrl_qh->ep_char_1) = ep_char;

	// Capabilities: one transaction to be issued in per micro frame
	EHCI_MEM(hc_ctrl_qh->ep_capab_2) = (1 << BIT_EHCI_QUEUE_HEAD_EP_CAPAB_MULT) |
			(hubPort << BIT_EHCI_QUEUE_HEAD_EP_CAPAB_HUB_PORT) |
			(hub << BIT_EHCI_QUEUE_HEAD_EP_CAPAB_HUB_ADDR) |
			MASK_EHCI_QUEUE_HEAD_EP_CAPAB_UFRAME_CMASK;
}

static int8_t usbh_init_ctrlep(USBH_endpoint *ep0, USBH_qh *hc_ctrl_qh,
		uint8_t speed, uint8_t newaddr,
		uint8_t hub, uint8_t hubPort)
{
	uint8_t status = USBH_OK;

	// Next queue (bits 1 and 2) are type of queue
	// Forward referencing type will be updated when this is inserted in a list.
	EHCI_MEM(hc_ctrl_qh->next) = MASK_EHCI_QUEUE_HEAD_TYPE_QH |
			EHCI_CPU_TO_HC(hc_ctrl_qh);

	usbh_update_ctrlep(hc_ctrl_qh, speed, newaddr, hub, hubPort);
	// Add this queue to the endpoint structure
	ep0->hc_entry.qh = hc_ctrl_qh;

	// Add a dummy qTD for the endpoint qH
	ep0->hc_dummy_qtd = (USBH_qtd *)usbh_alloc_hc_1_block();
	usbh_init_qtd(ep0->hc_dummy_qtd);
	ep0->hc_head_qtd = ep0->hc_dummy_qtd;

	// Setup transfer overlay to point to dummy qTD
	EHCI_MEM(hc_ctrl_qh->transfer_overlay.next) = EHCI_CPU_TO_HC(ep0->hc_dummy_qtd);
	EHCI_MEM(hc_ctrl_qh->transfer_overlay.alt_next) = EHCI_CPU_TO_HC(ep0->hc_dummy_qtd);

	// Link in to lists at correct place
	status = usbh_ep_list_add(ep0, usbh_async_ep_list, MASK_EHCI_QUEUE_HEAD_TYPE_QH);

	return status;
}

static void usbh_xfer_init(USBH_xfer *xferInit)
{
	memset(xferInit, 0, sizeof(USBH_xfer));

	xferInit->status = USBH_NOT_COMPLETE;
}

static void usbh_xfer_add(USBH_xfer *xferAdd, USBH_xfer **list)
{
	USBH_xfer *xferThis;

	// Set starting values for status (USBH_OK or USBH_ERR_*) and
	// a copy of the status bits of the aTD token.
	xferAdd->status = USBH_NOT_COMPLETE;
	//xferAdd->token = EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE;

	xferAdd->next = NULL;

	CRITICAL_SECTION_BEGIN

	xferThis = *list;

	if (xferThis)
	{
		// Add to end of existing list
		while (xferThis->next)
		{
			xferThis = xferThis->next;
		};
		xferThis->next = xferAdd;
#ifdef USBH_DEBUG_XFER
		tfp_printf("Xfer add: add to end of list\r\n");
#endif // USBH_DEBUG_XFER
	}
	else
	{
#ifdef USBH_DEBUG_XFER
		tfp_printf("Xfer add: add to empty list\r\n");
#endif // USBH_DEBUG_XFER
		// Add to empty list
		*list = xferAdd;
	}

	CRITICAL_SECTION_END
}

static void usbh_xfer_remove(USBH_xfer *xferRemove, USBH_xfer **list)
{
	USBH_xfer *xferThis;
	USBH_xfer *xferPrev = NULL;

	CRITICAL_SECTION_BEGIN

	xferThis = *list;

	while (xferThis)
	{
		if (xferThis == xferRemove)
		{
			if (xferPrev)
			{
				// Removing non-first member of list
				xferPrev->next = xferThis->next;
#ifdef USBH_DEBUG_XFER
				tfp_printf("Xfer remove: removing mid-list entry\r\n");
#endif // USBH_DEBUG_XFER
			}
			else
			{
				// Removing first member of list
				*list = xferThis->next;
#ifdef USBH_DEBUG_XFER
				tfp_printf("Xfer remove: removing first entry\r\n");
#endif // USBH_DEBUG_XFER
			}
			break;
		}
		xferPrev = xferThis;
		xferThis = xferThis->next;
	};

	CRITICAL_SECTION_END
}

static int8_t usbh_xfer_complete(USBH_xfer *xferComplete)
{
	uint32_t token;
	int8_t status = USBH_OK;
	uint8_t microframe;

	// Only check if the status is incomplete.
	// Once this xfer is complete then we do not check.
	if (xferComplete->status == USBH_NOT_COMPLETE)
	{
		// Test BULK, INTERRUPT and SETUP transfers. These have a qTD associated
		// with the transfer. ISOCHRONOUS transfers do not.
		if (xferComplete->hc_entry.type == USBH_list_entry_type_qH)
		{
			token = EHCI_MEM(xferComplete->hc_qtd->token);

			if (token & EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE)
			{
				return USBH_NOT_COMPLETE;
			}

			// Calculate exactly how many bytes copied.
			// The transaction length is written back by the host controller
			// when the transfer is retired (for IN transactions). This is not
			// (as the spec says) the number transferred, rather it is the
			// number of bytes left compared to the original value in this
			// register.
			xferComplete->dest_len = xferComplete->hc_len -
					((token & MASK_EHCI_QUEUE_TD_TOKEN_TOTAL_TO_TRANSFER) >> BIT_EHCI_QUEUE_TD_TOKEN_TOTAL_TO_TRANSFER);

			// Only report an error when there are no retries left.
			if (token & MASK_EHCI_QUEUE_TD_TOKEN_STATUS)
			{
				USBH_qh *hc_this_qh = xferComplete->hc_entry.qh;

				if (token & EHCI_QUEUE_TD_TOKEN_STATUS_HALTED)
				{
					if (token & EHCI_QUEUE_TD_TOKEN_STATUS_BABBLE)
					{
						// Endpoint babble detected error code.
						status = USBH_ERR_BABBLE;
					}
					else if (token & MASK_EHCI_QUEUE_TD_TOKEN_CERR)
					{
						// Endpoint stalled error code. When there are
						// retries remaining then only a STALL or
						// babble can be the culprits. Babble has been
						// checked.
						status = USBH_ERR_STALLED;
					}
					else if (token & EHCI_QUEUE_TD_TOKEN_STATUS_MISSED_MICROFRAME)
					{
						// Endpoint missed microframe error code. Ignore if this
						// endpoint is not in the periodic list.
						status = USBH_ERR_MISSED_MICROFRAME;
					}
					else if (token & EHCI_QUEUE_TD_TOKEN_STATUS_DATA_BUF_ERR)
					{
						status = USBH_ERR_DATA_BUF;
					}
					else if (token & EHCI_QUEUE_TD_TOKEN_STATUS_XACTERR)
					{
						// Endpoint transaction error code
						status = USBH_ERR_XACTERR;
					}
					else if (token & EHCI_QUEUE_TD_TOKEN_STATUS_P_ERR)
					{
						token = EHCI_MEM(hc_this_qh->ep_char_1);
						if ((token & EHCI_QUEUE_HEAD_EP_CHAR_EPS_HS) == 0)
						{
							// Endpoint split transaction error only if NOT High-speed
							status = USBH_ERR_SPLITX;
						}
					}
					else
					{
						status = USBH_ERR_HALTED;
					}
				}

				if ((hc_this_qh->ep_char_1 & MASK_EHCI_QUEUE_HEAD_EP_CHAR_ENDPOINT) == 0)
				{
					// Check if the qh->current_qtd points to the qTD of the transfer.
					// If it does then the overlay is from the qTD we need to unlink.
					if (EHCI_HC_TO_CPU(EHCI_MEM(hc_this_qh->qtd_current)) == (uint32_t)xferComplete->hc_qtd)
					{
						CRITICAL_SECTION_BEGIN
						token = EHCI_MEM(hc_this_qh->transfer_overlay.token);
						// Unhalt the transfer in the overlay.
						token &= (~EHCI_QUEUE_TD_TOKEN_STATUS_HALTED);
						EHCI_MEM(hc_this_qh->transfer_overlay.token) = token;
						CRITICAL_SECTION_END
						// Wait for a short time to ensure that the controller passes this
						// qTD and writes back the overlay.
						delayms(1);
						// Re-write the overlay.
						EHCI_MEM(hc_this_qh->transfer_overlay.token) = token;
					}
				}
			}
		}
		else if (xferComplete->hc_entry.type == USBH_list_entry_type_siTD)
		{
			// Full speed ISO.
			token = EHCI_MEM(xferComplete->hc_entry.siTD->transfer_state);

			if (token & EHCI_SPLIT_ISO_TD_STATUS_ACTIVE)
			{
				return USBH_NOT_COMPLETE;
			}

			// Calculate exactly how many bytes copied.
			// The transaction length is written back by the host controller
			// when the transfer is retired (for IN transactions). This is not
			// (as the spec says) the number transferred, rather it is the
			// number of bytes left compared to the original value in this
			// register.
			xferComplete->dest_len = xferComplete->hc_len -
					((token	& MASK_EHCI_SPLIT_ISO_TD_TRANSACTION_LENGTH) >> BIT_EHCI_SPLIT_ISO_TD_TRANSACTION_LENGTH);

			// Split-X is not an error state, so remove it from the choices.
			if (token & (MASK_EHCI_SPLIT_ISO_TD_STATUS & (~EHCI_SPLIT_ISO_TD_STATUS_SPLITX)))
			{
				// Default error code
				status = USBH_ERR_USBERR;
				if (token & EHCI_SPLIT_ISO_TD_STATUS_DATA_BUF_ERR)
				{
					status = USBH_ERR_DATA_BUF;
				}
				else if (token & EHCI_SPLIT_ISO_TD_STATUS_BABBLE)
				{
					// Endpoint halted error code
					status = USBH_ERR_BABBLE;
				}
				else if (token & EHCI_SPLIT_ISO_TD_STATUS_XACTERR)
				{
					// Endpoint halted error code
					status = USBH_ERR_XACTERR;
				}
				else if (token & EHCI_SPLIT_ISO_TD_STATUS_MISSED_MICROFRAME)
				{
					// Endpoint halted error code
					status = USBH_ERR_MISSED_MICROFRAME;
				}
				else if (token & EHCI_SPLIT_ISO_TD_STATUS_ERR)
				{
					// Transaction translator error code
					status = USBH_ERR_USBERR;
				}
			}
		}
		else if (xferComplete->hc_entry.type == USBH_list_entry_type_iTD)
		{
			// High speed ISO.
			token = 0;
			for (microframe = 0; microframe < 8; microframe++)
			{
				token |= EHCI_MEM(xferComplete->hc_entry.iTD->status_control_list[microframe]);
			}

			// One or more microframes waiting to complete.
			if (token & EHCI_ISO_TD_STATUS_ACTIVE)
			{
				return USBH_NOT_COMPLETE;
			}

			// Calculate exactly how many bytes copied.
			// The transaction length is written back by the host controller
			// when the transfer is retired (for IN transactions). This is not
			// (as the spec says) the number transferred, rather it is the
			// number of bytes left compared to the original value in this
			// register.
			xferComplete->dest_len = 0;
			for (microframe = 0; microframe < 8; microframe++)
			{
				token = EHCI_MEM(xferComplete->hc_entry.iTD->status_control_list[microframe]);
				// If this microframe was used then the IOC bit MUST be set.
				if (token & MASK_EHCI_ISO_TD_IOC)
				{
					xferComplete->dest_len += (xferComplete->hc_len -
							((token & MASK_EHCI_ISO_TD_TRANSACTION_LENGTH) >> BIT_EHCI_ISO_TD_TRANSACTION_LENGTH));

					if (token & MASK_EHCI_ISO_TD_STATUS)
					{
						// Default error code
						//status = USBH_ERR_USBERR;
						if (token & EHCI_ISO_TD_STATUS_DATA_BUF_ERR)
						{
							status = USBH_ERR_DATA_BUF;
						}
						else if (token & EHCI_ISO_TD_STATUS_BABBLE)
						{
							// Endpoint halted error code
							status = USBH_ERR_BABBLE;
						}
						else if (token & EHCI_ISO_TD_STATUS_XACTERR)
						{
							// Endpoint halted error code
							token = EHCI_MEM(xferComplete->hc_entry.iTD->pointer_list[1]);
							if (token & MASK_EHCI_ISO_TD_DIRECTION)
							{
								// Set for IN direction only.
								//status = USBH_ERR_XACTERR;
							}
						}
					}
				}
			}
		}

		// Store new status in the transfer.
		xferComplete->status = status;
	}

	return xferComplete->status;
}

static int32_t usbh_setup_transfer(USBH_endpoint *ep0, USB_device_request *devReq, uint8_t *devData, size_t len, uint16_t timeout)
{
	int8_t status = USBH_OK;
	int32_t result = 0;
	USBH_qh *hc_ctrl_qh = ep0->hc_entry.qh;
	uint32_t token;

	// Mirrors for request and data in EHCI memory
	USB_device_request *hc_devReq = 0;
	uint8_t *hc_devData = 0;

	// Use stack to prototype FIRST qTD in chain.
	// This is then copied into the current dummy at the last moment
	USBH_qtd setup_qtd;

	// Allocate blocks in EHCI RAM for Queue Transfer Descriptors
	USBH_qtd *hc_setup_qtd = 0; // former dummy qTD
	USBH_qtd *hc_data_qtd = 0;
	USBH_qtd *hc_ack_qtd = 0;
	USBH_qtd *hc_dummy_qtd = 0;

	// Create transfer structures on the stack as the transactions for
	// a SETUP are not asynchronous.
	USBH_xfer xferSetup;
	USBH_xfer xferData;
	USBH_xfer xferAck;

#ifdef USBH_DEBUG_SETUP
	tfp_printf("Setup: request %d of length %d\r\n", devReq->bRequest, len);
#endif // USBH_DEBUG_SETUP

	do {
		status = USBH_ERR_RESOURCES;

		if (len > 0)
		{
			hc_data_qtd = (USBH_qtd *)usbh_alloc_hc_1_block();
			usbh_init_qtd(hc_data_qtd);
			if (hc_data_qtd == NULL) break;

			hc_devData = (uint8_t *)usbh_alloc_hc_buffer(len);
			if (hc_devData == NULL) break;
		}

		// Allocate ACK qTD
		hc_ack_qtd = (USBH_qtd *)usbh_alloc_hc_1_block();
		usbh_init_qtd(hc_ack_qtd);
		if (hc_ack_qtd == NULL) break;

		// Allocate new dummy qTD
		hc_dummy_qtd = (USBH_qtd *)usbh_alloc_hc_1_block();
		usbh_init_qtd(hc_dummy_qtd);
		if (hc_dummy_qtd == NULL) break;

		hc_devReq = (USB_device_request *)usbh_alloc_hc_buffer(sizeof(USB_device_request));
		if (hc_devReq == NULL) break;
		usbh_memcpy8_mem2hc(hc_devReq, devReq, sizeof(USB_device_request));

		status = USBH_OK;
	} while (0);

	if (status == USBH_OK)
	{
		// NOTE: The setup, data and ack qTDs are initialised out of the way of the
		// host controller, so no need for usbh_memcpy yet.
		memset(&setup_qtd, 0, sizeof(USBH_qtd));

		// SETUP token data location
		setup_qtd.alt_next = EHCI_CPU_TO_HC(hc_ack_qtd);
		// Transfer data + ioc clear + 3 error retire + setup token
		// Set HALTED bit to ensure this does not get processed yet.
		setup_qtd.token =
				EHCI_QUEUE_TD_TOKEN_TOTAL_TO_TRANSFER(8) |
				MASK_EHCI_QUEUE_TD_TOKEN_IOC |
				(1 << BIT_EHCI_QUEUE_TD_TOKEN_CERR) |
				EHCI_QUEUE_TD_TOKEN_PID_SETUP |
				EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE;
		// Note: a SETUP packet is 0x20 bytes and cannot span a 4kB page.
		setup_qtd.buf[0] = EHCI_CPU_TO_HC(hc_devReq);

		if (len > 0)
		{
			// DATA phase required - Next qtd pointer is DATA packet
			setup_qtd.next = EHCI_CPU_TO_HC(hc_data_qtd);
			setup_qtd.alt_next = EHCI_CPU_TO_HC(hc_dummy_qtd);

			// Next qtd pointer is ACK packet
			hc_data_qtd->next = EHCI_CPU_TO_HC(hc_ack_qtd);
			hc_data_qtd->alt_next = EHCI_CPU_TO_HC(hc_ack_qtd);

			//data toggle + 64Bytes + 3 error retire + in/out token + active
			token = MASK_EHCI_QUEUE_TD_TOKEN_TOGGLE |
					EHCI_QUEUE_TD_TOKEN_TOTAL_TO_TRANSFER(len) |
					MASK_EHCI_QUEUE_TD_TOKEN_IOC |
					(1 << BIT_EHCI_QUEUE_TD_TOKEN_CERR) |
					EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE;
			if ((devReq->bmRequestType & USB_BMREQUESTTYPE_DIR_MASK) == USB_BMREQUESTTYPE_DIR_DEV_TO_HOST)
			{
				// If this is an IN request then change PID to IN token
				token |= EHCI_QUEUE_TD_TOKEN_PID_IN;
			}
			else
			{
				// If this is an OUT request then change PID to OUT token
				token |= EHCI_QUEUE_TD_TOKEN_PID_OUT;
				// Copy data to send to the HC memory
				usbh_memcpy8_mem2hc(hc_devData, devData, len);
			}
			hc_data_qtd->token = token;
			// Get description data from this buffer address
			// Note: the DATA part of a setup request may span a 4kB page.
			hc_data_qtd->buf[0] = EHCI_CPU_TO_HC(hc_devData); //---> DATA buffer
			hc_data_qtd->buf[1] = (EHCI_CPU_TO_HC(hc_devData + 4096) & MASK_EHCI_QUEUE_TD_BUFFER_CURRENT_POINTER); //---> DATA buffer
		}
		else
		{
			// No DATA phase - Next qtd pointer is ACK packet
			setup_qtd.next = EHCI_CPU_TO_HC(hc_ack_qtd); //---> ACK
		}

		// Note: the ACK packet is zero bytes and cannot span a 4kB page.
		hc_ack_qtd->buf[0] = 0;//EHCI_CPU_TO_HC(dummy); //---> DUMMY buffer

		hc_ack_qtd->next = EHCI_CPU_TO_HC(hc_dummy_qtd);
		hc_ack_qtd->alt_next = EHCI_CPU_TO_HC(hc_dummy_qtd);

		token = MASK_EHCI_QUEUE_TD_TOKEN_TOGGLE |
				EHCI_QUEUE_TD_TOKEN_TOTAL_TO_TRANSFER(0) |
				MASK_EHCI_QUEUE_TD_TOKEN_IOC |
				(1 << BIT_EHCI_QUEUE_TD_TOKEN_CERR) |
				EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE;
		if ((devReq->bmRequestType & USB_BMREQUESTTYPE_DIR_MASK) == USB_BMREQUESTTYPE_DIR_DEV_TO_HOST)
		{
			// If this is an IN request then change PID to IN token
			token |= EHCI_QUEUE_TD_TOKEN_PID_OUT;
		}
		else
		{
			// If this is an OUT request then change PID to OUT token
			token |= EHCI_QUEUE_TD_TOKEN_PID_IN;
		}
		hc_ack_qtd->token = token;

		// Initialise the transfer structures in advance
		usbh_xfer_init(&xferSetup);
		xferSetup.endpoint = NULL;
		xferSetup.hc_entry.type = USBH_list_entry_type_qH;
		xferSetup.hc_entry.qh = hc_ctrl_qh;
		//setup_xfer.hc_qtd = 0; // to be filled in later (once current dummy is determined)
		xferSetup.callback = NULL;
		xferSetup.timeout = timeout;

		if (len)
		{
			usbh_xfer_init(&xferData);
			xferData.endpoint = NULL;
			xferData.hc_entry.type = USBH_list_entry_type_qH;
			xferData.hc_entry.qh = hc_ctrl_qh;
			xferData.hc_qtd = hc_data_qtd;
			xferData.callback = NULL;
			xferData.timeout = timeout;
			xferData.hc_buf = hc_devData;
			xferData.hc_len = len;
		}

		usbh_xfer_init(&xferAck);
		xferAck.endpoint = NULL;
		xferAck.hc_entry.type = USBH_list_entry_type_qH;
		xferAck.hc_entry.qh = hc_ctrl_qh;
		xferAck.hc_qtd = hc_ack_qtd;
		xferAck.callback = NULL;
		xferAck.timeout = timeout;

		// LOCK
		CRITICAL_SECTION_BEGIN

		// Use the dummy qTD from previous transaction as the setup qTD.
		// This will always be valid.
		hc_setup_qtd = ep0->hc_dummy_qtd;
		ep0->hc_dummy_qtd = hc_dummy_qtd;
		ep0->hc_head_qtd = hc_setup_qtd;

		CRITICAL_SECTION_END
		// UNLOCK

		xferSetup.hc_qtd = hc_setup_qtd;

		// Preserve the value of the token field of the SETUP request.
		token = setup_qtd.token;
		// Disable the SETUP request qTD for now.
		setup_qtd.token = EHCI_QUEUE_TD_TOKEN_STATUS_HALTED;
		// Copy SETUP request qTD to dummy qTD from previous transaction...
		usbh_memcpy32_mem2hc((void *)&hc_setup_qtd->alt_next, (void *)&setup_qtd.alt_next,
				sizeof(USBH_qtd) - sizeof(uint32_t));
		hc_setup_qtd->next = setup_qtd.next;
		// Update transaction to the new setup qTD
		xferSetup.hc_qtd = hc_setup_qtd;

		// Add each transaction to the transaction list
		usbh_xfer_add(&xferSetup, &usbh_xferList);
		if (len)
			usbh_xfer_add(&xferData, &usbh_xferList);
		usbh_xfer_add(&xferAck, &usbh_xferList);

		// Kick off the transfer by setting the active bit in the setup qTD.
		// NOTE: setting the token in the qTD MUST be atomic
		EHCI_MEM(hc_setup_qtd->token) = token;

		// Wait for completion of the SETUP token.
		status = usbh_wait_complete(&xferSetup);

		if (status != USBH_OK)
		{
			// Handle or report error
#ifdef USBH_DEBUG_ERROR
			tfp_printf("Setup error: SETUP error occurred %d!!!\r\n", status);
#endif // USBH_DEBUG_ERROR
		}
		else
		{
			if (len)
			{
				// Wait for completion of the DATA packet (if any).
				status = usbh_wait_complete(&xferData);

				if ((devReq->bmRequestType & USB_BMREQUESTTYPE_DIR_MASK) == USB_BMREQUESTTYPE_DIR_DEV_TO_HOST)
				{
					// Copy data received from the HC memory
					usbh_memcpy8_hc2mem(devData, hc_devData, len);
				}

				// Number of bytes actually transferred.
				result = xferData.dest_len;
			}
			else
			{
				result = 0;
			}
			if (status != USBH_OK)
			{
				// Handle or report error
#ifdef USBH_DEBUG_ERROR
				tfp_printf("Setup: DATA Error occurred %d!!!\r\n", status);
#endif // USBH_DEBUG_ERROR
			}
			else
			{
				// Wait for completion of the ACK
				status = usbh_wait_complete(&xferAck);
				if (status != USBH_OK)
				{
					// Handle or report error
#ifdef USBH_DEBUG_ERROR
					tfp_printf("Setup: ACK Error occurred %d!!!\r\n", status);
#endif // USBH_DEBUG_ERROR
				}
			}
		}

		usbh_xfer_remove(&xferSetup, &usbh_xferList);
		if (len)
			usbh_xfer_remove(&xferData, &usbh_xferList);
		usbh_xfer_remove(&xferAck, &usbh_xferList);

		// Restore the head qTD to point to the dummy qTD.
		ep0->hc_head_qtd = ep0->hc_dummy_qtd;
	}

	if (hc_setup_qtd)
	{
		usbh_free_hc_1_block(hc_setup_qtd);
	}
	if (hc_data_qtd)
	{
		usbh_free_hc_1_block(hc_data_qtd);
	}
	if (hc_ack_qtd)
	{
		usbh_free_hc_1_block(hc_ack_qtd);
	}
	if (hc_devData)
	{
		usbh_free_hc_buffer(hc_devData, len);
	}
	if (hc_devReq)
	{
		usbh_free_hc_buffer(hc_devReq, sizeof(USB_device_request));
	}

	if (status != USBH_OK)
	{
		// Promote int8_t to int32_t.
		result = status;
	}

	return result;
}

static uint32_t usbh_interrupt(void)
{
	uint32_t status;

	// Read interrupt status.
	status = EHCI_REG(usbsts);
	// Clear active (important) interrupts.
	EHCI_REG(usbsts) = status | (MASK_EHCI_USBSTS_USB_INT |
			MASK_EHCI_USBSTS_USBERR_INT |
			MASK_EHCI_USBSTS_PO_CHG_DET);

	return status;
}

static int8_t usbh_wait_complete(USBH_xfer *xferWait)
{
	int8_t status = USBH_OK;

	while (1)
	{
		// Run an update of pending transactions.
		USBH_process();
		status = xferWait->status;
		// Our qTD must be active (completed to continue).
		if (status != USBH_NOT_COMPLETE)
		{
			break;
		}
	}

	return status;
}

static int32_t usbh_transfer_queued_worker(USBH_endpoint  *ep,
		uint8_t       *buffer,
		const size_t  length,
		uint16_t      timeout,
		uint32_t      id,
		USBH_callback cb)
{
	int8_t status = USBH_OK;
	int32_t result = 0;
	// Buffer for transfer source or result
	uint8_t *hc_data_buf = NULL;
	// qTD for transfer
	USBH_qtd data_qtd;
	USBH_qtd *hc_data_qtd = NULL;
	// qTD dummy
	USBH_qtd *hc_dummy_qtd = NULL;
	// Token for qTD
	uint32_t token;
	// Async structure for transfer results
	USBH_xfer *xferNew = NULL;

	// Check we are acting on an queue header type endpoint.
	if (ep->hc_entry.type != USBH_list_entry_type_qH)
	{
		return USBH_ERR_PARAMETER;
	}

	if (length)
	{
		hc_data_buf = (uint8_t *)usbh_alloc_hc_buffer(length);
		if (hc_data_buf == NULL)
		{
			status = USBH_ERR_RESOURCES;
		}
	}

	if (status == USBH_OK)
	{
		hc_dummy_qtd = (USBH_qtd *)usbh_alloc_hc_1_block();
		usbh_init_qtd(hc_dummy_qtd);
		if (hc_dummy_qtd == NULL)
		{
			status = USBH_ERR_RESOURCES;
		}
		else
		{
			usbh_memset32_hc(hc_dummy_qtd, 0, sizeof(USBH_qtd));
			xferNew = usbh_alloc_transfer();
			if (xferNew == NULL)
			{
				status = USBH_ERR_RESOURCES;
			}
		}
	}

	if (status == USBH_OK)
	{
		data_qtd.next = EHCI_CPU_TO_HC(hc_dummy_qtd);;
		data_qtd.alt_next = EHCI_CPU_TO_HC(hc_dummy_qtd);;
		//data toggle + 64Bytes + 3 error retire + in/out token + active
		token = EHCI_QUEUE_TD_TOKEN_TOTAL_TO_TRANSFER(length) |
				MASK_EHCI_QUEUE_TD_TOKEN_IOC |
				EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE;
		if (ep->direction == USBH_DIR_SETUP)
		{
			// If this is a SETUP request then change PID to SETUP token
			token |= EHCI_QUEUE_TD_TOKEN_PID_SETUP;
			token |= (1 << BIT_EHCI_QUEUE_TD_TOKEN_CERR);
			// Copy data from user to EHCI RAM
			usbh_memcpy8_mem2hc(hc_data_buf, buffer, length);
		}
		else
		{
			if (ep->direction == USBH_DIR_IN)
			{
				// If this is an IN request then change PID to IN token
				token |= EHCI_QUEUE_TD_TOKEN_PID_IN;
				token |= (3 << BIT_EHCI_QUEUE_TD_TOKEN_CERR);
			}
			else // (ep->direction == USBH_DIR_OUT)
			{
				// If this is an OUT request then change PID to OUT token
				token |= EHCI_QUEUE_TD_TOKEN_PID_OUT;
				token |= (3 << BIT_EHCI_QUEUE_TD_TOKEN_CERR);
				// Copy data from user to EHCI RAM
				usbh_memcpy8_mem2hc(hc_data_buf, buffer, length);
			}

			if (ep->type == USBH_EP_CTRL)
			{
				token |= MASK_EHCI_QUEUE_TD_TOKEN_TOGGLE;
				token |= (1 << BIT_EHCI_QUEUE_TD_TOKEN_CERR);
			}
		}

		data_qtd.token = token;
		// Get description data from this buffer address
		// Note: the payload may span a 4kB page.
		data_qtd.buf[0] = EHCI_CPU_TO_HC(hc_data_buf); //---> DATA buffer
		data_qtd.buf[1] = EHCI_CPU_TO_HC(hc_data_buf + 4096); //---> DATA buffer

		// LOCK
		CRITICAL_SECTION_BEGIN

		// Use the dummy qTD from previous transaction. This will always
		// be valid.
		hc_data_qtd = ep->hc_dummy_qtd;
		// Make new endpoint dummy the terminating qTD for this
		ep->hc_dummy_qtd = hc_dummy_qtd;

		// If the first qTD is pointing to the dummy one then point the
		// head to the current qTD (which is the new first qTD).
		if (ep->hc_head_qtd == ep->hc_dummy_qtd)
		{
			ep->hc_head_qtd = hc_data_qtd;
		}

		CRITICAL_SECTION_END
		// UNLOCK

		// Preserve the value of the token field of the SETUP request.
		token = data_qtd.token;
		// Disable the SETUP request qTD for now.
		data_qtd.token = EHCI_QUEUE_TD_TOKEN_STATUS_HALTED;
		// Copy SETUP request qTD to dummy qTD from previous transaction...
		usbh_memcpy32_mem2hc(hc_data_qtd, &data_qtd, sizeof(USBH_qtd));

		usbh_xfer_init(xferNew);
		// Fill in the basic fields required for all transactions.
		xferNew->hc_qtd = hc_data_qtd;
		xferNew->timeout = timeout;
		xferNew->next = NULL;
		xferNew->endpoint = ep;

		// Update QH to point to new setup transfer
		xferNew->hc_entry.type = USBH_list_entry_type_qH;
		xferNew->hc_entry.qh = ep->hc_entry.qh;
		xferNew->hc_buf = hc_data_buf;
		xferNew->hc_len = length;

		if (cb)
		{
			// If we are starting an asynchronous transaction then fill in
			// the fields required.
			xferNew->id = id;
			xferNew->callback = cb;
			xferNew->dest_buf = buffer;
			xferNew->dest_len = 0;
		}

		// Kick off the transfer by setting the active bit in the setup qTD.
		// NOTE: setting the token in the qTD MUST be atomic
		EHCI_MEM(hc_data_qtd->token) = token;

		// Add this transfer to list of outstanding transfers.
		if ((ep->type == USBH_EP_BULK) || (ep->type == USBH_EP_CTRL))
		{
			usbh_xfer_add(xferNew, &usbh_xferList);
		}
		else // if (ep->type == USBH_EP_INTERRUPT)
		{
			usbh_xfer_add(xferNew, &usbh_xferListPeriodic);
		}

		if (cb == NULL)
		{
			// Wait for completion if we are blocking
			status = usbh_wait_complete(xferNew);

			if (hc_data_buf)
			{
				// Set number of bytes copied.
				result = xferNew->dest_len;
			}

			if ((ep->type == USBH_EP_BULK) || (ep->type == USBH_EP_CTRL))
			{
				usbh_xfer_remove(xferNew, &usbh_xferList);
			}
			else
			{
				usbh_xfer_remove(xferNew, &usbh_xferListPeriodic);
			}

			// If this is an IN request then copy data.
			if (ep->direction == USBH_DIR_IN)
			{
				// Copy data from EHCI RAM to program RAM
				if (hc_data_buf)
				{
					usbh_memcpy8_hc2mem(buffer, hc_data_buf, length);
				}
			}
		}
	}
	// Clean up only if there is an error or there is no callback.
	if ((cb == NULL) || (status == USBH_ERR_RESOURCES))
	{
		if (hc_data_buf)
			usbh_free_hc_buffer(hc_data_buf, length);

		if (status == USBH_ERR_RESOURCES)
		{
			if (hc_dummy_qtd)
				usbh_free_hc_1_block(hc_dummy_qtd);
		}
		else
		{
			if (hc_data_qtd)
				usbh_free_hc_1_block(hc_data_qtd);
		}
		if (xferNew)
			usbh_free_transfer(xferNew);
	}

	// xferNew is freed when it is completed.
	if (status != USBH_OK)
	{
		// Promote int8_t to int32_t.
		result = status;
	}

	return result;
}

static int32_t usbh_transfer_iso_worker(USBH_endpoint  *ep,
		uint8_t       *buffer,
		const size_t  length,
		uint16_t      timeout,
		uint32_t      id,
		USBH_callback cb)
{
	int8_t status = USBH_OK;
	int32_t result = 0;
	// Buffer for transfer source or result
	uint8_t *hc_data_buf = NULL;
	// Async structure for transfer results
	USBH_xfer *xferNew = NULL;
	USBH_itd *hc_itd = NULL;
	USBH_sitd *hc_sitd = NULL;

	// Check we are acting on an isochronous endpoint.
	if ((ep->hc_entry.type != USBH_list_entry_type_iTD)
			&& (ep->hc_entry.type != USBH_list_entry_type_siTD))
	{
		return USBH_ERR_PARAMETER;
	}

	if (length)
	{
		hc_data_buf = (uint8_t *)usbh_alloc_hc_buffer(length);
		if (hc_data_buf == NULL)
		{
			status = USBH_ERR_RESOURCES;
		}
	}

	if (status == USBH_OK)
	{
		xferNew = usbh_alloc_transfer();
		if (xferNew == NULL)
		{
			status = USBH_ERR_RESOURCES;
		}
	}

	if (status == USBH_OK)
	{
		usbh_xfer_init(xferNew);

		if (ep->hc_entry.type == USBH_list_entry_type_iTD)
		{
			hc_itd = (USBH_itd *)usbh_alloc_hc_2_blocks();
			if (hc_itd == NULL)
			{
				status = USBH_ERR_RESOURCES;
			}
			xferNew->hc_entry.iTD = hc_itd;
		}
		else
		{
			hc_sitd = (USBH_sitd *)usbh_alloc_hc_1_block();
			if (hc_sitd == NULL)
			{
				status = USBH_ERR_RESOURCES;
			}
			xferNew->hc_entry.siTD = hc_sitd;
		}

		xferNew->hc_entry.type = ep->hc_entry.type;
	}

	if (status == USBH_OK)
	{
		// Fill in the basic fields required for all transactions.
		xferNew->hc_qtd = NULL;
		xferNew->timeout = timeout;
		xferNew->next = NULL;
		xferNew->endpoint = ep;
		xferNew->hc_buf = hc_data_buf;
		xferNew->hc_len = length;

		if (cb)
		{
			// If we are starting an asynchronous transaction then fill in
			// the fields required.
			xferNew->id = id;
			xferNew->callback = cb;
			xferNew->dest_buf = buffer;
			xferNew->dest_len = 0;
		}

		if (ep->direction == USBH_DIR_OUT)
		{
			// Copy data from user to EHCI RAM
			usbh_memcpy8_mem2hc(hc_data_buf, buffer, length);
		}

		// High-speed endpoint.
		if (ep->hc_entry.type == USBH_list_entry_type_iTD)
		{
			uint32_t page0;
			uint32_t page1;

			// Add a HIGH SPEED ISOCHRONOUS endpoint.

			// Forward referencing type will be updated when this is inserted in a list.
			EHCI_MEM(hc_itd->next) = MASK_EHCI_QUEUE_HEAD_TYPE_iTD;
			EHCI_MEM(hc_itd->status_control_list[0]) = EHCI_ISO_TD_TRANSACTION_LENGTH(length)
											| MASK_EHCI_ISO_TD_IOC // IOC bit MUST be set to enable this microframe.
											| (0 << BIT_EHCI_ISO_TD_PG)
											| EHCI_ISO_TD_STATUS_ACTIVE
											| (EHCI_CPU_TO_HC((uint32_t)hc_data_buf) & MASK_EHCI_ISO_TD_OFFSET);
			// Disable all tranasactions.

			// Setup Device Address and Endpoint Number.
			page0 = (EHCI_CPU_TO_HC((uint32_t)hc_data_buf) & MASK_EHCI_ISO_TD_BUFFER_POINTER)
											| (ep->interface->device->address << BIT_EHCI_ISO_TD_ADDRESS)
											| (ep->index << BIT_EHCI_ISO_TD_EP_NUMBER);
			EHCI_MEM(hc_itd->pointer_list[0]) = page0;

			page1 = ep->max_packet_size
					| (EHCI_CPU_TO_HC((uint32_t)hc_data_buf + 4096) & MASK_EHCI_ISO_TD_BUFFER_POINTER);
			if (ep->direction == USBH_DIR_IN)
			{
				page1 |= MASK_EHCI_ISO_TD_DIRECTION;
			}
			EHCI_MEM(hc_itd->pointer_list[1]) = page1;

			EHCI_MEM(hc_itd->pointer_list[2]) = 1 << BIT_EHCI_ISO_TD_MULTI;

			EHCI_MEM(hc_itd->pointer_list[3]) = 0;
			EHCI_MEM(hc_itd->pointer_list[4]) = 0;
			EHCI_MEM(hc_itd->pointer_list[5]) = 0;
			EHCI_MEM(hc_itd->pointer_list[6]) = 0;

#ifdef USBH_DEBUG_XFER
			tfp_printf("iTD next %08x\r\n", hc_itd->next);
			tfp_printf("iTD status_control_list %08x\r\n", hc_itd->status_control_list[0]);
			tfp_printf("iTD status_control_list %08x\r\n", hc_itd->status_control_list[1]);
			tfp_printf("iTD status_control_list %08x\r\n", hc_itd->status_control_list[2]);
			tfp_printf("iTD status_control_list %08x\r\n", hc_itd->status_control_list[3]);
			tfp_printf("iTD status_control_list %08x\r\n", hc_itd->status_control_list[4]);
			tfp_printf("iTD status_control_list %08x\r\n", hc_itd->status_control_list[5]);
			tfp_printf("iTD status_control_list %08x\r\n", hc_itd->status_control_list[6]);
			tfp_printf("iTD status_control_list %08x\r\n", hc_itd->status_control_list[7]);
			tfp_printf("iTD pointer_list %08x\r\n", hc_itd->pointer_list[0]);
			tfp_printf("iTD pointer_list %08x\r\n", hc_itd->pointer_list[1]);
			tfp_printf("iTD pointer_list %08x\r\n", hc_itd->pointer_list[2]);
			tfp_printf("iTD pointer_list %08x\r\n", hc_itd->pointer_list[3]);
			tfp_printf("iTD pointer_list %08x\r\n", hc_itd->pointer_list[4]);
			tfp_printf("iTD pointer_list %08x\r\n", hc_itd->pointer_list[5]);
			tfp_printf("iTD pointer_list %08x\r\n", hc_itd->pointer_list[6]);
#endif // USBH_DEBUG_XFER
		}
		// Full-speed endpoint.
		else //if (xferNew->hc_entry.type == USBH_list_entry_type_siTD)
		{
			uint32_t epchar;
			uint32_t epcapab;
			uint32_t hubport, hub;
			uint32_t bp1;

			// Add a FULL SPEED ISOCHRONOUS endpoint.

			// Forward referencing type will be updated when this is inserted in a list.
			EHCI_MEM(hc_sitd->next) = MASK_EHCI_QUEUE_HEAD_TYPE_siTD;
			// Get the Port, Hub
			hubport = ep->interface->device->parentPort;
			if (ep->interface->device->parent != NULL)
			{
				hub = ep->interface->device->parent->address;
			}
			else
			{
				hub = 0;
				hubport = 0;
			}
			// Setup Device Address and Endpoint Number.
			epchar = (ep->interface->device->address << BIT_EHCI_SPLIT_ISO_TD_ADDRESS)
											| (ep->index << BIT_EHCI_SPLIT_ISO_TD_EP_NUMBER)
											| (hub << BIT_EHCI_SPLIT_ISO_TD_HUB_ADDR)
											| (hubport << BIT_EHCI_SPLIT_ISO_TD_PORT_NUMBER);
			if (ep->direction == USBH_DIR_IN)
			{
				epchar |= MASK_EHCI_SPLIT_ISO_TD_DIRECTION;
			}
			// Perform extrememly simple scheduling.
			epcapab = (0x1 << BIT_EHCI_SPLIT_ISO_TD_UFRAME_SMASK);
			if (length < 188)
				epcapab |= (0xe << BIT_EHCI_SPLIT_ISO_TD_UFRAME_CMASK);
			if (length >= 188)
				epcapab |= (0x1e << BIT_EHCI_SPLIT_ISO_TD_UFRAME_CMASK);
			if (length >= 188 * 2)
				epcapab |= (0x3e << BIT_EHCI_SPLIT_ISO_TD_UFRAME_CMASK);
			if (length >= 188 * 4)
				epcapab |= (0x7e << BIT_EHCI_SPLIT_ISO_TD_UFRAME_CMASK);
			if (length >= 188 * 5)
				epcapab |= (0x7e << BIT_EHCI_SPLIT_ISO_TD_UFRAME_CMASK);
			// Update missing parts in the endpoint characteristics.
			EHCI_MEM(hc_sitd->ep_char_1) = epchar;
			EHCI_MEM(hc_sitd->ep_capab_2) = epcapab;
			// Back pointer is not supported.
			EHCI_MEM(hc_sitd->back) = MASK_EHCI_SPLIT_ISO_TD_TERMINATE;

			EHCI_MEM(hc_sitd->pointer_list[0]) = EHCI_CPU_TO_HC((uint32_t)hc_data_buf);
			bp1 = EHCI_CPU_TO_HC(((uint32_t)hc_data_buf + 4096) & MASK_EHCI_SPLIT_ISO_TD_BUFFER_POINTER);
			if (length <= 188)
			{
				// All transaction can be acheived in one split.
				bp1 |= (0 << BIT_EHCI_SPLIT_ISO_TD_TRANSACTION_POS);
			}
			else
			{
				// Begin split signal to be sent. From here it's managed by the host controller.
				bp1 |= (1 << BIT_EHCI_SPLIT_ISO_TD_TRANSACTION_POS);
			}
			if (ep->direction == USBH_DIR_OUT)
			{
				// Number of OUT start splits required.
				bp1 |= (((length / 188) + 1) << BIT_EHCI_SPLIT_ISO_TD_T_COUNT);
			}
			EHCI_MEM(hc_sitd->pointer_list[1]) = bp1;
			// Program in the length, interrupt on completion bit and active bit.
			// This will start the transaction.
			EHCI_MEM(hc_sitd->transfer_state) = EHCI_SPLIT_ISO_TD_TRANSACTION_LENGTH(length)
													| MASK_EHCI_SPLIT_ISO_TD_IOC
													| EHCI_SPLIT_ISO_TD_STATUS_ACTIVE;

#ifdef USBH_DEBUG_XFER
			tfp_printf("siTD next %08x\r\n", hc_sitd->next);
			tfp_printf("siTD ep_char_1 %08x\r\n", hc_sitd->ep_char_1);
			tfp_printf("siTD ep_capab_2 %08x\r\n", hc_sitd->ep_capab_2);
			tfp_printf("siTD transfer_state %08x\r\n", hc_sitd->transfer_state);
			tfp_printf("siTD pointer_list %08x\r\n", hc_sitd->pointer_list[0]);
			tfp_printf("siTD pointer_list %08x\r\n", hc_sitd->pointer_list[1]);
			tfp_printf("siTD back %08x\r\n", hc_sitd->back);
#endif // USBH_DEBUG_XFER

		}

		// Add the transfer's siTD or iTD to the period frame list.
		// Zero means at the next allowable polling time slot.
		usbh_xfer_iso_td_insert(xferNew, 0);

		// Add this transfer to list of outstanding transfers.
		// Starts timeouts.
		usbh_xfer_add(xferNew, &usbh_xferListPeriodic);

		if (cb == NULL)
		{
			// Wait for completion if we are blocking
			status = usbh_wait_complete(xferNew);

			if (hc_data_buf)
			{
				// Set number of bytes copied.
				result = xferNew->dest_len;
			}

			// Remove the transfer's siTD or iTD from the period frame list.
			usbh_xfer_iso_td_remove(xferNew);

			usbh_xfer_remove(xferNew, &usbh_xferListPeriodic);

			// If this is an IN request then copy data.
			if (ep->direction == USBH_DIR_IN)
			{
				// Copy data from EHCI RAM to program RAM
				if (hc_data_buf)
				{
					usbh_memcpy8_hc2mem(buffer, hc_data_buf, length);
				}
			}
		}
	}
	// Clean up only if there is an error or there is no callback.
	if ((cb == NULL) || (status == USBH_ERR_RESOURCES))
	{
		if (hc_data_buf)
		{
			usbh_free_hc_buffer(hc_data_buf, length);
		}
		if (xferNew)
		{
			usbh_free_transfer(xferNew);
		}
		if (hc_itd)
		{
			usbh_free_hc_2_blocks(hc_itd);
		}
		if (hc_sitd)
		{
			usbh_free_hc_1_block(hc_sitd);
		}
	}

	// xferNew is freed when it is completed.
	if (status != USBH_OK)
	{
		// Promote int8_t to int32_t.
		result = status;
	}

	return result;
}


/* ENUMERATION FUNCTIONS ***********************************************************/

/** @brief Interrupt handler for USB Host stack.
 */
static int8_t usbh_req_get_descriptor(USBH_endpoint *ep0, uint8_t type, uint8_t index, size_t len, uint8_t *buf)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_STANDARD | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = (type << 8) | index; // Descriptor Type in High byte
	devReq.wLength = len;

	status = usbh_setup_transfer(ep0, &devReq, buf, len, 500);

	if (status < 0)
		return (int8_t)status;
	return USBH_OK;
}

static int8_t usbh_req_set_address(USBH_endpoint *ep0, uint8_t addr)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_REQUEST_CODE_SET_ADDRESS;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_STANDARD | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = addr;
	devReq.wLength = 0;

	status = usbh_setup_transfer(ep0, &devReq, 0, 0, 500);

	if (status < 0)
		return (int8_t)status;
	return USBH_OK;
}

static int8_t usbh_req_set_configuration(USBH_endpoint *ep0, uint8_t num)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_REQUEST_CODE_SET_CONFIGURATION;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_STANDARD | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = num;
	devReq.wLength = 0;

	status = usbh_setup_transfer(ep0, &devReq, 0, 0, 500);

	if (status < 0)
		return (int8_t)status;
	return USBH_OK;
}

static int8_t usbh_req_set_interface(USBH_endpoint *ep0, uint8_t iface, uint8_t alt)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_REQUEST_CODE_SET_INTERFACE;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_STANDARD | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
	devReq.wIndex = iface;
	devReq.wValue = alt;
	devReq.wLength = 0;

	status = usbh_setup_transfer(ep0, &devReq, 0, 0, 500);

	if (status < 0)
		return (int8_t)status;
	return USBH_OK;
}

static uint8_t usbh_get_hub_status_worker(USBH_device *hubDev, uint8_t index, USB_hub_port_status *buf)
{
	int32_t status;
	USB_device_request devReq;

	if (hubDev == NULL)
	{
		usbh_hw_update_root_hub_port_status();
		memcpy(buf, &currentPortStatus, sizeof(USB_hub_port_status));
	}
	else
	{
		devReq.bRequest = USB_REQUEST_CODE_GET_STATUS;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
				USB_BMREQUESTTYPE_CLASS;
		if (index > 0) devReq.bmRequestType |= USB_BMREQUESTTYPE_RECIPIENT_PORT;
		devReq.wIndex = index;
		devReq.wValue = 0;
		devReq.wLength = sizeof(USB_hub_port_status);

		status = usbh_setup_transfer(hubDev->endpoint0, &devReq, (uint8_t *)buf, sizeof(USB_hub_port_status), 500);

		if (status < 0)
			return (int8_t)status;
	}
	return USBH_OK;
}

static uint8_t usbh_get_hub_descriptor(USBH_device *hubDev, USB_hub_descriptor *buf)
{
	int32_t status;
	USB_device_request devReq;

	devReq.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;
	devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
	devReq.wIndex = 0;
	devReq.wValue = (USB_DESCRIPTOR_TYPE_HUB << 8) | 0; // Descriptor Type in High byte
	devReq.wLength = sizeof(USB_hub_descriptor);

	status = usbh_setup_transfer(hubDev->endpoint0, &devReq, (uint8_t *)buf, sizeof(USB_hub_descriptor), 500);

	if (status < 0)
		return (int8_t)status;
	return USBH_OK;
}

//TODO: currently not used
#if 0
static uint8_t usbh_get_hub_port_speed(USBH_device *hubDev, uint8_t hubPort)
{
	if (hubDev != NULL)
	{
		USB_hub_port_status portStatus;
		usbh_get_hub_status_worker(hubDev, hubPort, &portStatus);

		if (portStatus.portLowSpeed) return USBH_SPEED_LOW;
		if (portStatus.portHighSpeed) return USBH_SPEED_HIGH;
		return USBH_SPEED_FULL;
	}

	// Root hub speed
	return usbh_hw_get_device_speed();
}
#endif // 0

static uint8_t usbh_set_hub_port_feature(USBH_device *hubDev, uint8_t hubPort, uint16_t feature)
{
	int32_t status;
	USB_device_request devReq;

	if (hubDev != NULL)
	{
		devReq.bRequest = USB_HUB_REQUEST_CODE_SET_FEATURE;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_PORT;
		devReq.wIndex = hubPort;
		devReq.wValue = feature;
		devReq.wLength = 0;

		status = usbh_setup_transfer(hubDev->endpoint0, &devReq, 0, 0, 500);
	}
	else
	{
		status = USBH_OK;
		if (hubPort == 0)
		{
			// Hub Features
			switch (feature)
			{
			case USB_HUB_CLASS_FEATURE_C_HUB_LOCAL_POWER:
				break;
			case USB_HUB_CLASS_FEATURE_C_HUB_OVER_CURRENT:
				break;
			default:
				status = USBH_ERR_USBERR;
				break;
			}
		}
		else
		{
			uint32_t val;
			// Port Features
			switch (feature)
			{
			case USB_HUB_CLASS_FEATURE_PORT_RESET:
				CRITICAL_SECTION_BEGIN
				// Assert port reset.
				val = EHCI_REG(portsc);
				val |= MASK_EHCI_PORTSC_PO_RESET;
				val &= (~MASK_EHCI_PORTSC_PO_EN);
				EHCI_REG(portsc) = val;
				CRITICAL_SECTION_END
				currentPortStatus.portResetChange = 1;
				break;
			case USB_HUB_CLASS_FEATURE_PORT_SUSPEND:
				currentPortStatus.portSuspend = 1;
				currentPortStatus.portSuspendChange = 1;
				break;
			case USB_HUB_CLASS_FEATURE_PORT_POWER:
				// Turn on VBus
				EHCI_REG(busctrl) &= (~MASK_EHCI_BUSCTRL_VBUS_OFF);
				// Not used by driver
				currentPortStatus.portPower = 1;
				currentPortStatus.portPowerChange = 1;
				break;
			case USB_HUB_CLASS_FEATURE_PORT_TEST:
#ifdef USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
				// Not used by driver
				currentPortStatus.portTest = 1;
				currentPortStatus.portTestChange = 1;
#endif // USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
				break;
			case USB_HUB_CLASS_FEATURE_PORT_INDICATOR:
#ifdef USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
				// Not used by driver
				currentPortStatus.portIndicator = 1;
				currentPortStatus.portIndicatorChange = 1;
#endif // USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
				break;
			case USB_HUB_CLASS_FEATURE_PORT_ENABLE:
				EHCI_REG(portsc) |= MASK_EHCI_PORTSC_PO_EN; //set port enable
				currentPortStatus.portEnabled = 1;
				currentPortStatus.portEnabledChange = 1;
				break;
			default:
				status = USBH_ERR_USBERR;
				break;
			}
		}
	}

	if (status < 0)
		return (int8_t)status;
	return USBH_OK;
}

static uint8_t usbh_clear_hub_port_feature(USBH_device *hubDev, uint8_t hubPort, uint16_t feature)
{
	int32_t status;
	USB_device_request devReq;

	if (hubDev != NULL)
	{
		if (feature == USB_HUB_CLASS_FEATURE_PORT_RESET)
		{
			// ClearPortFeature(PORT_RESET) not to be used on hubs.
			status = USBH_OK;
		}
		else
		{
			devReq.bRequest = USB_HUB_REQUEST_CODE_CLEAR_FEATURE;
			devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
					USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_PORT;
			devReq.wIndex = hubPort;
			devReq.wValue = feature;
			devReq.wLength = 0;

			status = usbh_setup_transfer(hubDev->endpoint0, &devReq, 0, 0, 500);
		}
	}
	else
	{
		status = USBH_OK;
		if (hubPort == 0)
		{
			// Hub Features
			switch (feature)
			{
			case USB_HUB_CLASS_FEATURE_C_HUB_LOCAL_POWER:
				break;
			case USB_HUB_CLASS_FEATURE_C_HUB_OVER_CURRENT:
				break;
			default:
				status = USBH_ERR_USBERR;
				break;
			}
		}
		else
		{
			uint32_t val;

			// Port Features
			switch (feature)
			{
			case USB_HUB_CLASS_FEATURE_PORT_RESET:
				// Deassert port reset.
				// After clear operation, start command must be called in less than 3ms
				// or host will go into suspend state.
				val = EHCI_REG(portsc);
				val &= (~MASK_EHCI_PORTSC_PO_RESET);
				EHCI_REG(portsc) = val;
				break;
			case USB_HUB_CLASS_FEATURE_PORT_ENABLE:
				EHCI_REG(portsc) &= (~MASK_EHCI_PORTSC_PO_EN); //clear port enable
				currentPortStatus.portEnabled = 0;
				currentPortStatus.portEnabledChange = 1;
				break;
			case USB_HUB_CLASS_FEATURE_PORT_SUSPEND:
				currentPortStatus.portSuspend = 0;
				currentPortStatus.portSuspendChange = 1;
				break;
			case USB_HUB_CLASS_FEATURE_PORT_POWER:
				EHCI_REG(busctrl) |= (MASK_EHCI_BUSCTRL_VBUS_OFF);
				currentPortStatus.portPowerChange = 1;
				break;
			case USB_HUB_CLASS_FEATURE_C_PORT_CONNECTION:
				usbh_hw_root_c_connect_change();
				currentPortStatus.currentConnectStatusChange = 0;
				break;
			case USB_HUB_CLASS_FEATURE_C_PORT_RESET:
				currentPortStatus.portResetChange = 0;
				break;
			case USB_HUB_CLASS_FEATURE_C_PORT_ENABLE:
				usbh_hw_root_c_enable_change();
				currentPortStatus.portEnabledChange = 0;
				break;
			case USB_HUB_CLASS_FEATURE_C_PORT_SUSPEND:
				currentPortStatus.portSuspendChange = 0;
				break;
			case USB_HUB_CLASS_FEATURE_C_PORT_OVER_CURRENT:
				currentPortStatus.portOverCurrentChange = 0;
				break;
			case USB_HUB_CLASS_FEATURE_PORT_TEST:
#ifdef USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
				// Not used by driver
				currentPortStatus.portTest = 0;
				currentPortStatus.portTestChange = 1;
#endif // USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
				break;
			case USB_HUB_CLASS_FEATURE_PORT_INDICATOR:
#ifdef USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
				// Not used by driver
				currentPortStatus.portIndicator = 0;
				currentPortStatus.portIndicatorChange = 1;
#endif // USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
				break;
			default:
				status = USBH_ERR_USBERR;
				break;
			}
		}
	}

	if (status < 0)
		return (int8_t)status;
	return USBH_OK;
}

/* Enumeration Routines *****************************************************/

static int8_t usbh_hub_enumerate(USBH_device *hubDev, uint8_t configurationNum)
{
	int8_t status = USBH_OK;
	USBH_device *devNew = NULL;
	uint8_t i;
	int8_t twopass = 0;

#ifdef USBH_DEBUG_ENUM
	tfp_printf("Enumeration: starting...\r\n");
#endif
	// Special case of enumeration of the root hub
	if (hubDev == NULL)
	{
		// Restart addressing.
		devAddr = 1;

		// Port 1 is on root hub
		status = usbh_hub_port_update(NULL, 1, configurationNum, &devNew);
		// Root hub is always the first device.
		// check for a hub on usbh_firstDev and enumerate from there.
		if (status == USBH_OK)
		{
			hubDev = devNew;
		}
		else if (status == USBH_ENUM_NO_CHANGE)
		{
			// Act on the top-level device.
			hubDev = usbh_firstDev;
		}
	}

	if (hubDev)
	{
		if (hubDev->descriptor.bDeviceClass == USB_CLASS_HUB)
		{
			// Wait for configuration to hit.
			delayms(2);

			do {
				// Run once, but if power was turned on to hub ports then
				// run twice.
				twopass = 0;

				// Enumerate downstream hub ports.
				for (i = 1; i <= hubDev->portCount; i++)
				{
					devNew = NULL;
					status = usbh_hub_port_update(hubDev, i, configurationNum, &devNew);

					if (status == USBH_ENUM_HUB_POWER)
					{
						twopass = 1;
						continue;
					}

					// Recurse through any new downstream hubs
					if (status == USBH_OK)
					{
						if (devNew)
						{
							if (devNew->descriptor.bDeviceClass == USB_CLASS_HUB)
							{
								// Recursively enumerate child hubs.
								status = usbh_hub_enumerate(devNew, configurationNum);
							}
						}
					}
				}

				if (status == USBH_ENUM_HUB_POWER)
				{
					// Wait for TSIGATT
					delayms(100);
				}
			} while (twopass == 1);

			usbh_hub_poll_start(hubDev);
		}
	}

	return status;
}

static int8_t usbh_hub_port_update(USBH_device *hubDev, uint8_t hubPort, uint8_t configurationNum, USBH_device **devNew)
{
	int8_t status = USBH_ENUM_NO_CHANGE;
	// Hub port status
	USB_hub_port_status portStatus;
	uint8_t connectChange = 0;
	uint8_t connectStatus = 0;

	// Check for connection changes on the selected hub and port
	if (hubDev)
	{
		status = usbh_get_hub_status_worker(hubDev, hubPort, &portStatus);

		if (!portStatus.portPower)
		{
#ifdef USBH_DEBUG_ENUM
			tfp_printf("Enumeration: Turn on Downstream Hub power for port %d\r\n", hubPort);
#endif
			status = usbh_set_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_PORT_POWER);

			status = usbh_get_hub_status_worker(hubDev, hubPort, &portStatus);

			return USBH_ENUM_HUB_POWER;
		}
	}
	else
	{
		usbh_hw_update_root_hub_port_status();
		//Note: struct copy operation
		portStatus = currentPortStatus;

		status = USBH_OK;
	}

	if (portStatus.currentConnectStatusChange)
	{
		connectChange = 1;

		usbh_clear_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_C_PORT_CONNECTION);

		if (hubDev)
		{
			status = usbh_get_hub_status_worker(hubDev, hubPort, &portStatus);
		}
		else
		{
			usbh_hw_update_root_hub_port_status();
			//Note: struct copy operation
			portStatus = currentPortStatus;
		}

		connectStatus = portStatus.currentConnectStatus;
	}

	// Note: Clear any other change requests which are pending.
	if (portStatus.portEnabledChange)
	{
		usbh_clear_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_C_PORT_ENABLE);
	}
	if (portStatus.portSuspendChange)
	{
		usbh_clear_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_C_PORT_SUSPEND);
	}
	if (portStatus.portOverCurrentChange)
	{
		usbh_clear_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_C_PORT_OVER_CURRENT);
	}
	if (portStatus.portResetChange)
	{
		usbh_clear_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_C_PORT_RESET);
	}

	if (connectChange)
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Hub port connection change\r\n");
#endif

		if (!portStatus.currentConnectStatus)
		{
#ifdef USBH_DEBUG_ENUM
			tfp_printf("Enumeration: Removing hub port %d\r\n", hubPort);
#endif
			status = usbh_hub_port_remove(hubDev, hubPort);
			*devNew = NULL;
		}
		else
		{
#ifdef USBH_DEBUG_ENUM
			tfp_printf("Enumeration: Connected hub port %d\r\n", hubPort);
#endif
			status = usbh_hub_port_enumerate(hubDev, hubPort, configurationNum, devNew);
		}

		// Enumeration change(s)... make the callback.
		if (usbh_enum_change_callback)
		{
			usbh_enum_change_callback(uniqueEnumValue, connectStatus, 0, NULL);
		}
	}

	return status;
}

static int8_t usbh_hub_port_remove(USBH_device *hubDev, uint8_t hubPort)
{
	int8_t status = USBH_OK;

	// Check for connection changes on the selected hub and port
	if (hubDev == NULL)
	{
		// If the root device is enumerated then disconnect everything.

#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Root device removed\r\n");
#endif // USBH_DEBUG_ENUM

		if (usbh_firstDev)
		{
			// Act on the first device.
			hubDev = (USBH_device *)usbh_firstDev;
			usbh_firstDev = NULL;

#ifdef USBH_DEBUG_ENUM
			tfp_printf("Enumeration: Removing devices on root hub\r\n");
#endif // USBH_DEBUG_ENUM

			usbh_dev_disconnect(hubDev);
		}

		// Stop EHCI hardware.
		usbh_hw_stop();
		// Delay for at least 16 microframes to scheduler to park
		// at dummy qH.
		delayms(2);

		// Check it has halted.
		while ((EHCI_REG(usbsts) & MASK_EHCI_USBSTS_HCHalted) == 0);

		// Reset addressing
		devAddr = 1;
	}
	else
	{
		USBH_device *devChild, *devNext, *devPrev;
		devChild = hubDev->children;

#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Removing device on downstream hub\r\n");
#endif // USBH_DEBUG_ENUM

		devPrev = NULL;
		while (devChild)
		{
			devNext = devChild->next;
			if (devChild->parentPort == hubPort)
			{
				if (hubDev->children == devChild)
				{
					hubDev->children = devNext;
				}
				else if (devPrev)
				{
					devPrev->next = devNext;
				}
				// Disconnect this port on the hub
				usbh_dev_disconnect(devChild);
			}
			devPrev = devChild;
			devChild = devNext;
		}
	}

	return status;
}

static void usbh_update_periodic_tree(void)
{
	// Device address
	uint32_t i;
	uint32_t count;
	uint32_t link;
	// Temporary endpoint pointer to find an endpoint with a polling interval.
	USBH_endpoint *epInterval;

	// LOCK
	CRITICAL_SECTION_BEGIN

	for (i = 0; i < usbh_get_periodic_size(); i++)
	{
		usbh_write_periodic_list(i, EHCI_CPU_TO_HC(usbh_periodic_header_qh) |
				MASK_EHCI_QUEUE_HEAD_TYPE_QH | MASK_EHCI_QUEUE_HEAD_TERMINATE);
	}

	// Make periodic list based on timings. Start at 8 milli-frames and work
	// upwards. Period list is based on milliseconds.
	// Current implementation does not implement periods of less than one frame.
	// Endpoints doing this would need to have their S-Mask value modified.
	count = 8;
	// Count is shifted left 1 (count *= 2) each pass to make a populated list with
	// each entry appearing
	while (count <= (1 << 15))
	{
		// Find first endpoint in the (ordered) list which has a polling interval
		// equal to or less than the endpoint being added.
		epInterval = usbh_ep_list_find_periodic(count);
		if (epInterval != NULL)
		{
			link = EHCI_CPU_TO_HC(epInterval->hc_entry.qh);
			if (epInterval->hc_entry.type == USBH_list_entry_type_qH)
			{
				link |= MASK_EHCI_QUEUE_HEAD_TYPE_QH;
			}
			else if (epInterval->hc_entry.type == USBH_list_entry_type_siTD)
			{
				link |= MASK_EHCI_QUEUE_HEAD_TYPE_siTD;
			}
			else if (epInterval->hc_entry.type == USBH_list_entry_type_iTD)
			{
				link |= MASK_EHCI_QUEUE_HEAD_TYPE_iTD;
			}

			// If the period is within the range addressable by the configuration
			// (the number of entries in the periodic list) then add it to the list.
			// Otherwise add it as the first entry and it can poll more often than
			// strictly required.
			if (count < usbh_get_periodic_size())
			{
				// Add an entry to the periodic list every (n) slots. (n) is the
				// the value 2 ^ count.
				for (i = 0; i < usbh_get_periodic_size(); i += (count >> 3))
				{
					usbh_write_periodic_list(i, link);
#ifdef USBH_DEBUG_PERIODIC_LIST
					tfp_printf("List: Changed periodic entry %d to %08x\r\n", i, link);
#endif // USBH_DEBUG_PERIODIC_LIST
				}
			}
			else
			{
				// Add to first entry in periodic list.
				usbh_write_periodic_list(0, link);
			}
		}

		count <<= 1;
	}

	CRITICAL_SECTION_END
}

static int8_t usbh_enumerate_parse_config_desc(USBH_endpoint *epZero, uint8_t *buf0, uint8_t configurationNum, size_t confDescLen, USBH_device *devNew)
{
	int8_t status = USBH_OK;
	// Pointer to current interface to attach endpoints to.
	USBH_interface *ifaceNew = NULL;
	// Temporary interface pointer to find the end of list.
	USBH_interface *ifaceNext = NULL;
	uint8_t ifCount = 0;
	// Pointer to current endpoint to attach queue headers to.
	USBH_endpoint *epNew;
	// Temporary endpoint pointer to find end of list.
	USBH_endpoint *epNext;
	// Configuration descriptor pointers (data stored in buf0).
	USB_configuration_descriptor *confDesc;
	size_t confDescOffset;
	uint32_t count;

	confDesc = (USB_configuration_descriptor *)&buf0[0];

#ifdef USBH_DEBUG_ENUM
	tfp_printf("Enumeration: Get Config Descriptor FULL %d bytes\r\n", confDescLen);
#endif
	status = usbh_req_get_descriptor(epZero, USB_DESCRIPTOR_TYPE_CONFIGURATION,
			configurationNum, confDescLen, (uint8_t *)buf0);

	if (status >= 0)
	{
		confDescOffset = 0;

#ifdef USBH_DEBUG_CONFIG_DESC
		tfp_printf("Descriptor: Total length: %d\r\n", confDescLen);
#endif
		while (confDescOffset < confDescLen)
		{
#ifdef USBH_DEBUG_CONFIG_DESC
			tfp_printf("Descriptor: Offset: %d\r\n", confDescOffset);
#endif
			confDesc = (USB_configuration_descriptor *)&buf0[confDescOffset];

			if (confDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_CONFIGURATION)
			{
#ifdef USBH_DEBUG_CONFIG_DESC
				tfp_printf("Descriptor: Parsing Configuration Descriptor\r\n");
#endif
				// There is no EHCI Structure associated with this type of descriptor.
			}
			else if (confDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION)
			{
#ifdef USBH_DEBUG_CONFIG_DESC
				tfp_printf("Descriptor: Parsing Interface Association Descriptor\r\n");
#endif
			}
			else if (confDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_INTERFACE)
			{
				USB_interface_descriptor *confInterfaceDesc = (USB_interface_descriptor *)&buf0[confDescOffset];

				// Make a new interface for the device.
#ifdef USBH_DEBUG_CONFIG_DESC
				tfp_printf("Descriptor: Parsing Interface Descriptor\r\n");
#endif
				ifaceNew = usbh_alloc_interface();
				if (ifaceNew == NULL)
				{
#ifdef USBH_DEBUG_CONFIG_DESC
					tfp_printf("Descriptor: Exhaused Interface Resources\r\n");
#endif
					status = USBH_ENUM_PARTIAL;
					break;
				}
				else
				{
					ifaceNew->enumValue = uniqueEnumValue;
					// Back pointer to device
					ifaceNew->device = devNew;
					// Copy data from interface descriptor
					ifaceNew->number = confInterfaceDesc->bInterfaceNumber;
					ifaceNew->alt = confInterfaceDesc->bAlternateSetting;
					// Select this interface only if it is Alternate Setting zero.
					// Otherwise a SET_INTERFACE is needed to select it.
					ifaceNew->set = (confInterfaceDesc->bAlternateSetting == 0);
					ifaceNew->ep_count = confInterfaceDesc->bNumEndpoints;
					ifaceNew->usb_class = confInterfaceDesc->bInterfaceClass;
					ifaceNew->usb_subclass = confInterfaceDesc->bInterfaceSubClass;
					ifaceNew->usb_protocol = confInterfaceDesc->bInterfaceProtocol;
					// Initialise links from this interface.
					ifaceNew->endpoints = NULL;
					ifaceNew->next = NULL;

					// Initialise link from device to this interface. Always add it LAST.
					ifaceNext = devNew->interfaces;
					if (ifaceNext)
					{
						while (ifaceNext->next)
						{
							ifaceNext = ifaceNext->next;
						}
						ifaceNext->next = ifaceNew;
					}
					else
					{
						devNew->interfaces = ifaceNew;
					}

					// There is no EHCI Structure associated with this type of descriptor.
					ifCount++;
				}
			}
			else if (confDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_ENDPOINT)
			{
				USB_endpoint_descriptor *confEndpointDesc = (USB_endpoint_descriptor *)&buf0[confDescOffset];

				// Make a new endpoint on the interface.
#ifdef USBH_DEBUG_CONFIG_DESC
				tfp_printf("Descriptor: Parsing Endpoint Descriptor\r\n");
#endif
				if (ifaceNew != 0)
				{
					// Make a new USBH_qh for the endpoint.
					// Add QH to the asynch or periodic schedule.
					epNew = usbh_alloc_endpoint();
					if (epNew == NULL)
					{
#ifdef USBH_DEBUG_CONFIG_DESC
						tfp_printf("Descriptor: Exhaused Endpoint Resources\r\n");
#endif
						status = USBH_ENUM_PARTIAL;
						break;
					}
					else
					{
						epNew->enumValue = uniqueEnumValue;
						// Back pointer to interface
						epNew->interface = ifaceNew;
						// Copy data from endpoint descriptor
						if ((confEndpointDesc->bEndpointAddress & USB_ENDPOINT_DESCRIPTOR_EPADDR_MASK) ==
								USB_ENDPOINT_DESCRIPTOR_EPADDR_IN)
						{
							epNew->direction = USBH_DIR_IN;
						}
						else
						{
							epNew->direction = USBH_DIR_OUT;
						}

						epNew->index = (confEndpointDesc->bEndpointAddress & (~USB_ENDPOINT_DESCRIPTOR_EPADDR_MASK));
#ifdef USBH_DEBUG_CONFIG_DESC
						tfp_printf("Descriptor: Endpoint index: %d %s ", epNew->index, epNew->direction == USBH_DIR_IN?"in":"out");
#endif // USBH_DEBUG_CONFIG_DESC

						epNew->interval = confEndpointDesc->bInterval;
						if ((confEndpointDesc->bmAttributes & USB_ENDPOINT_DESCRIPTOR_ATTR_MASK) ==
								USB_ENDPOINT_DESCRIPTOR_ATTR_BULK)
						{
#ifdef USBH_DEBUG_CONFIG_DESC
							tfp_printf("bulk ");
#endif // USBH_DEBUG_CONFIG_DESC
							epNew->type = USBH_EP_BULK;
						}
						else if ((confEndpointDesc->bmAttributes & USB_ENDPOINT_DESCRIPTOR_ATTR_MASK) ==
								USB_ENDPOINT_DESCRIPTOR_ATTR_INTERRUPT)
						{
#ifdef USBH_DEBUG_CONFIG_DESC
							tfp_printf("int ");
#endif // USBH_DEBUG_CONFIG_DESC
							epNew->type = USBH_EP_INT;
							if (devNew->speed == USBH_SPEED_HIGH)
							{
								// Recalculate interval for High speed interrupt endpoints
								count = epNew->interval;
								epNew->interval = 1;
								if (count > 0)
								{
									// Answer is 2 ^ (bInterval - 1)
									count--;
									while (count > 0) {
										count--;
										epNew->interval <<= 1;
									}
								}
							}
							else
							{
								// Recalculate interval for Low and Full speed interrupt endpoints.
								// Interval is in microframes (for future proofing).
								epNew->interval <<= 3;
							}
						}
						else if ((confEndpointDesc->bmAttributes & USB_ENDPOINT_DESCRIPTOR_ATTR_MASK) ==
								USB_ENDPOINT_DESCRIPTOR_ATTR_ISOCHRONOUS)
						{
#ifdef USBH_DEBUG_CONFIG_DESC
							tfp_printf("iso ");
#endif // USBH_DEBUG_CONFIG_DESC
							epNew->type = USBH_EP_ISOC;
							count = epNew->interval;
							epNew->interval = 1;
							if (count > 0)
							{
								// Answer is 2 ^ (bInterval - 1)
								count--;
								while (count > 0) {
									count--;
									epNew->interval <<= 1;
								}
							}
						}

						epNew->max_packet_size = (confEndpointDesc->wMaxPacketSize & 0x3ff);
#ifdef USBH_DEBUG_CONFIG_DESC
						tfp_printf("max %d ", epNew->max_packet_size);
#endif // USBH_DEBUG_CONFIG_DESC

						epNew->additional = (confEndpointDesc->wMaxPacketSize >> 11);
						epNew->next = NULL;

						if (epNew->type != USBH_EP_ISOC)
						{
							epNew->hc_entry.type = USBH_list_entry_type_qH;

							// Initialise and insert this endpoint QH to either the asynchronous
							// or periodic list of QHs at an appropriate point. Only require to do
							// this if the endpoint is selected with SET_INTERFACE.
							if (ifaceNew->set)
							{
								// Setup EHCI information for this endpoint.
								// This can be qH only.
								epNew->hc_entry.qh = (USBH_qh *)usbh_alloc_hc_2_blocks();
								// Initialise the QH.
								usbh_init_ep_qh(&epNew->hc_entry, epZero->hc_entry.qh, epNew);
							}
						}
						else
						{
							// No standing iTD/siTD for isochronous.
							epNew->hc_entry.qh = NULL;
							if (devNew->speed == USBH_SPEED_HIGH)
							{
								epNew->hc_entry.type = USBH_list_entry_type_iTD;
							}
							else
							{
								// If this is directly connected to the root hub then make it iTD
								if (devNew->parent == NULL)
								{
									epNew->hc_entry.type = USBH_list_entry_type_iTD;
								}
								else
								{
									epNew->hc_entry.type = USBH_list_entry_type_siTD;
								}
							}
						}

						// Initialise links to this endpoint from the interface. Always add it LAST.
						epNext = ifaceNew->endpoints;
						if (epNext)
						{
							while (epNext->next)
							{
								epNext = epNext->next;
							}
							epNext->next = epNew;
						}
						else
						{
							ifaceNew->endpoints = epNew;
						}
#ifdef USBH_DEBUG_CONFIG_DESC
						tfp_printf("\r\n");
#endif // USBH_DEBUG_CONFIG_DESC
					}
				}
			}
			else
			{
#ifdef USBH_DEBUG_CONFIG_DESC
				tfp_printf("Descriptor: Unknown Descriptor type %d length %d\r\n",
						confDesc->bDescriptorType, confDesc->bLength);
#endif
			}

			confDescOffset += confDesc->bLength;
		}
	}

	return status;
}

static int8_t usbh_enumerate_config_desc_128(USBH_endpoint *epZero, uint8_t configurationIndex, size_t confDescLen, USBH_device *devNew)
{
	uint8_t buf[128];

	return usbh_enumerate_parse_config_desc(epZero, buf, configurationIndex, confDescLen, devNew);
}

static int8_t usbh_enumerate_config_desc_256(USBH_endpoint *epZero, uint8_t configurationIndex, size_t confDescLen, USBH_device *devNew)
{
	uint8_t buf[256];

	return usbh_enumerate_parse_config_desc(epZero, buf, configurationIndex, confDescLen, devNew);
}

static int8_t usbh_enumerate_config_desc_512(USBH_endpoint *epZero, uint8_t configurationIndex, size_t confDescLen, USBH_device *devNew)
{
	uint8_t buf[512];

	return usbh_enumerate_parse_config_desc(epZero, buf, configurationIndex, confDescLen, devNew);
}

static int8_t usbh_enumerate_config_desc_1024(USBH_endpoint *epZero, uint8_t configurationIndex, size_t confDescLen, USBH_device *devNew)
{
	uint8_t buf[1024];

	return usbh_enumerate_parse_config_desc(epZero, buf, configurationIndex, confDescLen, devNew);
}

static int8_t usbh_enumerate_config_desc_2048(USBH_endpoint *epZero, uint8_t configurationIndex, size_t confDescLen, USBH_device *devNew)
{
	uint8_t buf[2048];

	return usbh_enumerate_parse_config_desc(epZero, buf, configurationIndex, confDescLen, devNew);
}

static int8_t usbh_enumerate_config_desc_4096(USBH_endpoint *epZero, uint8_t configurationIndex, size_t confDescLen, USBH_device *devNew)
{
	uint8_t buf[4096];

	return usbh_enumerate_parse_config_desc(epZero, buf, configurationIndex, confDescLen, devNew);
}

static int8_t usbh_hub_port_enumerate_device(USBH_device *devNew, USBH_device *hubDev, uint8_t hubPort, uint8_t configurationNum)
{
	int8_t status = USBH_OK;

	// Storage for config descriptors. Max packet size of High Speed control endpoint.
	uint8_t buf0[64];
	// Index of configuration descriptor that matches configuration number.
	uint8_t configurationIndex = 0;
	// Temporary device pointer to find end of list of child devices of hub.
	USBH_device *devNext;
	// Configuration descriptor pointers (data stored in buf0).
	USB_configuration_descriptor *confDesc;
	// Hub descriptor
	USB_hub_descriptor hubDesc;
	// Pointer to current interface to attach endpoints to.
	USBH_interface *ifaceNew;
	// Pointer to current endpoint to attach queue headers to.
	USBH_endpoint *epNew;
	// Intermediate configuration descriptor length.
	size_t confDescLen;

	(void)hubPort;
	
	enumInProgress = 1;

#ifdef USBH_DEBUG_ENUM
	//usbh_dump_device_descriptor(&(devNew->descriptor));
#endif

	// Transaction 4: Get Configuration Descriptor Part
#ifdef USBH_DEBUG_ENUM
	tfp_printf("Enumeration: Get Config Descriptor first\r\n");
#endif

	for (configurationIndex = 0; configurationIndex < devNew->descriptor.bNumConfigurations; configurationIndex++)
	{
		status = usbh_req_get_descriptor(devNew->endpoint0, USB_DESCRIPTOR_TYPE_CONFIGURATION,
				configurationIndex, sizeof(USB_configuration_descriptor), (uint8_t *)buf0);
		if (status == USBH_OK)
		{
			confDesc = (USB_configuration_descriptor*)buf0;

			// Check for a valid config descriptor first.
			if (confDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_CONFIGURATION)
			{
				// Take a matching configuration value or the first one if the requested configuration is zero.
				if ((confDesc->bConfigurationValue == configurationNum) || (configurationNum == 0))
				{
					configurationNum = confDesc->bConfigurationValue;
					usbh_dev_disconnect_config(devNew, 1);
					break;
				}
			}
		}
		status = USBH_ERR_NOT_FOUND;
	}

	if (status == USBH_OK)
	{
#ifdef USBH_DEBUG_ENUM
		//usbh_dump_config_descriptor((USB_configuration_descriptor*)buf0, 0);
#endif

		// Check for a valid config descriptor first.
		if (confDesc->bDescriptorType == USB_DESCRIPTOR_TYPE_CONFIGURATION)
		{
			// Make a new device handle.
			// Add current hc_ep0_qh to this.

			// Only need to continue with enumeration if the config descriptor claims more interfaces.
			// Otherwise there is nothing else to do.
#ifdef USBH_DEBUG_ENUM
			tfp_printf("Enumeration: Parsing Configuration Descriptor for %d interfaces\r\n", confDesc->bNumInterfaces);
#endif

			if (confDesc->bNumInterfaces)
			{
				// Depending on the size of the config descriptor call one or
				// other functions to use stack memory to acheive this.
				confDescLen = confDesc->wTotalLength;

				if (confDescLen <= 64)
				{
					status = usbh_enumerate_parse_config_desc(devNew->endpoint0, buf0, configurationIndex, confDescLen, devNew);
				}
				else if (confDescLen <= 128)
				{
					status = usbh_enumerate_config_desc_128(devNew->endpoint0, configurationIndex, confDescLen, devNew);
				}
				else if (confDescLen <= 256)
				{
					status = usbh_enumerate_config_desc_256(devNew->endpoint0, configurationIndex, confDescLen, devNew);
				}
				else if (confDescLen <= 512)
				{
					status = usbh_enumerate_config_desc_512(devNew->endpoint0, configurationIndex, confDescLen, devNew);
				}
				else if (confDescLen <= 1024)
				{
					status = usbh_enumerate_config_desc_1024(devNew->endpoint0, configurationIndex, confDescLen, devNew);
				}
				else if (confDescLen <= 2048)
				{
					status = usbh_enumerate_config_desc_2048(devNew->endpoint0, configurationIndex, confDescLen, devNew);
				}
				else if (confDescLen <= 4096)
				{
					status = usbh_enumerate_config_desc_4096(devNew->endpoint0, configurationIndex, confDescLen, devNew);
				}

				if (status == USBH_ENUM_PARTIAL)
				{
					// By definition there are no enumeration resources
					// available now.
					enumResourcesFull = 1;
				}
				// Increment enumeration value
				uniqueEnumValue++;
			}
#ifdef USBH_DEBUG_ENUM
			tfp_printf("Enumeration: Get Config Descriptor status %d\r\n", status);
#endif
		}
	}

	if (status >= USBH_OK)
	{
		// Transaction 6: Set Configuration
		status = usbh_req_set_configuration(devNew->endpoint0, configurationNum);
	}

	if (status >= USBH_OK)
	{
		devNew->configuration = configurationNum;

		if (devNew->descriptor.bDeviceClass == USB_CLASS_HUB)
		{
			// Wait a short time for the hub to become responsive
			delayms(10);

			status = usbh_get_hub_descriptor(devNew, &hubDesc);
			devNew->portCount = hubDesc.bNbrPorts;

			// Once the hub is fully enumerated then we will start polling it
			// properly.
			// Calculate the polling interval of the hub Status Change Endpoint
			devNew->hubPollTimeout = -1;
			ifaceNew = devNew->interfaces;
			if (ifaceNew)
			{
				// Get Status Change Endpoint
				epNew = ifaceNew->endpoints;
				if (epNew)
				{
					// Set the polling interval for the hub accordingly.
					// The polling interval for hubs is in milliseconds so divide
					// the endpoint interval (in milli-frames) by 8.
					devNew->hubPollTimeout = (epNew->interval >> 3);
				}
			}

#ifdef USBH_DEBUG_ENUM
			tfp_printf("Enumeration: Downstream hub with %d ports\r\n", devNew->portCount);
			tfp_printf("Enumeration: polling interval approx %d ms\r\n", devNew->hubPollTimeout);
#endif
		}

		usbh_update_periodic_tree();

		if (hubDev == NULL)
		{
			// For root hub set the device chain head.
			usbh_firstDev = devNew;
		}
		else
		{
			// For downstream hub add the device to the parent hub child list.
			devNext = hubDev->children;
			if (devNext)
			{
				while (devNext->next)
				{
					devNext = devNext->next;
				}
				devNext->next = devNew;
			}
			else
			{
				hubDev->children = devNew;
			}
		}

		//CRITICAL_SECTION_END
		// UNLOCK
	}

	if (status < USBH_OK)
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: failed %d\r\n", status);
#endif // USBH_DEBUG_ENUM

	}
	enumInProgress = 0;

	return status;
}

static int8_t usbh_hub_port_enumerate(USBH_device *hubDev, uint8_t hubPort, uint8_t configurationNum, USBH_device **ppDevNew)
{
	int8_t status = USBH_OK;

	// Storage for device descriptors.
	uint8_t buf0[64];
	// Pointer to new device made during this enumeration
	USBH_device *devNew;
	// Pointer to device endpoint0 for new device.
	USBH_endpoint *epZero;
	// Pointer to control endpoint data structure
	USBH_qh *hc_ep0_qh;
	// Device descriptor
	USB_device_descriptor *devDesc;
	// Hub and Port statuses
	USB_hub_port_status portStatus;
	// Device speed
	uint8_t speed = USBH_SPEED_FULL;
	// Hub descriptor
	USB_hub_descriptor hubDesc;
	// Device address
	uint8_t addr;
	// Initialise the delay to TSIGATT which is a minimum of 100 ms.
	uint32_t delay = 100;

	enumInProgress = 1;

	if (hubDev)
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Port %d on Downstream Hub - Get Descriptor\r\n", hubPort);
#endif
		// Get downstream hub descriptor
		status = usbh_get_hub_descriptor(hubDev, &hubDesc);
		if (status == USBH_OK)
		{
			// Add in extra delay required by hub
			delay = (hubDesc.bPwrOn2PwrGood * 2);
		}
	}

	// TSIGATT 100 ms delay
	delayms(delay);

	// Get port status (root hub OR downstream hub)
	usbh_get_hub_status_worker(hubDev, hubPort, &portStatus);
	if (portStatus.currentConnectStatus == 0)
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Device attach error\r\n");
#endif
		portStatus.currentConnectStatusChange = 1;
		enumInProgress = 0;

		return USBH_ERR_NOT_FOUND;
	}

	usbh_clear_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_C_PORT_CONNECTION);

	// TATTDB 100 ms delay
	delayms(delay);

	// Get port status (root hub OR downstream hub)
	usbh_get_hub_status_worker(hubDev, hubPort, &portStatus);
	if ((portStatus.currentConnectStatusChange != 0) ||
			(portStatus.currentConnectStatus == 0))
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Device debounce error\r\n");
#endif
		enumInProgress = 0;

		return USBH_ERR_NOT_FOUND;
	}

	if (hubDev == NULL)
	{
		// Enumerate Root hub
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Root Hub %d - Start Hardware\r\n", hubPort);
#endif
		// Disable the top level device to stop its use.
		usbh_firstDev = NULL;
	}

	// Reinitialise USB (root) hub
	status = usbh_set_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_PORT_RESET);

	if (hubDev == NULL)
	{
		// TDRSTR Min 50 ms
		delayms(50);
	}
	else
	{
		// TDRST Min 10 ms, Max 20 ms delay
		delayms(10);
	}

	status += usbh_clear_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_PORT_RESET);
	if (status != USBH_OK)
	{
		// Hub port feature error. No port found on hub device?
		portStatus.currentConnectStatusChange = 1;
		enumInProgress = 0;

		return USBH_ERR_NOT_FOUND;
	}

	usbh_get_hub_status_worker(hubDev, hubPort, &portStatus);
	if (portStatus.currentConnectStatus == 0)
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Device disconnect TDRSTR %lx\r\n", *(uint32_t *)&portStatus);
#endif
		portStatus.currentConnectStatusChange = 1;
		enumInProgress = 0;

		return USBH_ERR_NOT_FOUND;
	}

	usbh_hw_clrsts();
	usbh_hw_run();

	usbh_clear_hub_port_feature(hubDev, hubPort, USB_HUB_CLASS_FEATURE_C_PORT_RESET);
	// TRSTRCY 10 ms delay
	delayms(10);

	usbh_get_hub_status_worker(hubDev, hubPort, &portStatus);
	if (portStatus.currentConnectStatus == 0)
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Device disconnect TRSTRCY %lx\r\n", *(uint32_t *)&portStatus);
#endif
		portStatus.currentConnectStatusChange = 1;
		enumInProgress = 0;

		return USBH_ERR_NOT_FOUND;
	}

	if (hubDev == NULL)
	{
		speed = usbh_hw_get_device_speed();
	}
	else
	{
		if (portStatus.portLowSpeed)
		{
			speed = USBH_SPEED_LOW;
		}
		else if (portStatus.portHighSpeed)
		{
			speed = USBH_SPEED_HIGH;
		}
		else
		{
			speed = USBH_SPEED_FULL;
		}
	}

#ifdef USBH_DEBUG_ENUM
	tfp_printf("Enumeration: Speed %d Hub\r\n", speed);
#endif

	// Create a new device to be connected to the parent hub (and port)
	devNew = usbh_alloc_device();
	if (devNew == NULL)
	{
		// By definition there are no enumeration resources
		// available now.
		enumResourcesFull = 1;
		enumInProgress = 0;

		return USBH_ENUM_PARTIAL;
	}

	// Initialise QH for control endpoint.
	hc_ep0_qh = (USBH_qh *)usbh_alloc_hc_2_blocks();
	if (hc_ep0_qh == NULL)
	{
		usbh_free_device(devNew);
		// By definition there are no enumeration resources
		// available now.
		enumResourcesFull = 1;
		enumInProgress = 0;

		return USBH_ENUM_PARTIAL;
	}

	// Initialise an endpoint to store the QH for control endpoint.
	epZero = usbh_alloc_endpoint();
	if (epZero == NULL)
	{
		usbh_free_device(devNew);
		usbh_free_hc_2_blocks((void *)hc_ep0_qh);

		// An error occurred so no more enumeration for this port.
		*ppDevNew = NULL;
		// By definition there are no enumeration resources
		// available now.
		enumResourcesFull = 1;
		enumInProgress = 0;

		return USBH_ENUM_PARTIAL;
	}

	// Set port number for device (0 is on Root Hub).
	devNew->parentPort = hubPort;

	// Construct an EP0 endpoint structure for control endpoint of device.
	epZero->hc_entry.qh = hc_ep0_qh;
	epZero->enumValue = uniqueEnumValue;
	epZero->max_packet_size = 64; // placeholder
	epZero->index = 0;
	epZero->type = USBH_EP_CTRL;
	// Control endpoints do not require direction set nor the interface set

	if (hubDev)
	{
		status = usbh_init_ctrlep(epZero, hc_ep0_qh, speed, 0, hubDev->address, hubPort);
	}
	else
	{
		status = usbh_init_ctrlep(epZero, hc_ep0_qh, speed, 0, 0, 0);
	}

	if (status == USBH_OK)
	{
		// Transaction 1: Get Device Descriptor at Addr 0
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Get Device Descriptor first\r\n");
#endif

		status = usbh_req_get_descriptor(epZero, USB_DESCRIPTOR_TYPE_DEVICE, 0,
				(speed == USBH_SPEED_HIGH)?64:8, (uint8_t *)buf0);

		if (status == USBH_OK)
		{
			devDesc = (USB_device_descriptor *)&buf0[0];
			// Fill in information from the device descriptor
			epZero->max_packet_size = devDesc->bMaxPacketSize0;
#ifdef USBH_DEBUG_ENUM
			tfp_printf("Enumeration: Get Device Descriptor status %d\r\n", status);
#endif
		}
	}

	if (status == USBH_OK)
	{
		addr = devAddr++;

		// Transaction 2: Set Address
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Set Address %d\r\n", addr);
#endif
		status = usbh_req_set_address(epZero, addr);

		// Set Address recovery time 2ms.
		delayms(3);

		if (status == USBH_OK)
		{
			if (hubDev)
			{
				usbh_update_ctrlep(hc_ep0_qh, speed, addr, hubDev->address, hubPort);
			}
			else
			{
				usbh_update_ctrlep(hc_ep0_qh, speed, addr, 0, 0);
			}

			// Update QH characteristics
			EHCI_MEM(hc_ep0_qh->ep_char_1) &= (~MASK_EHCI_QUEUE_HEAD_EP_CHAR_MAX_PACKET);
			EHCI_MEM(hc_ep0_qh->ep_char_1) |= (epZero->max_packet_size << BIT_EHCI_QUEUE_HEAD_EP_CHAR_MAX_PACKET);
		}

		// Construct USB device structure
		// Each enumeration will be noted by a (practially) unique token that can be
		// used to determine the validity or otherwise of a device handle.
		devNew->enumValue = uniqueEnumValue;

		// Simple enumeration without hubs, single device, no siblings or parents.
		devNew->next = NULL;
		devNew->parent = hubDev;
		devNew->children = NULL;
		// Initialise interface pointer
		devNew->interfaces = NULL;
		// Device is not configured presently
		devNew->configuration = 0;
		// Set speed for device
		devNew->speed = speed;
		// Device address
		devNew->address = addr;

		// Initialise API Structures
		devNew->endpoint0 = epZero;

		// Transaction 3: Get Device Descriptor with Address
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: Get Device Descriptor FULL\r\n");
#endif

		status = usbh_req_get_descriptor(epZero, USB_DESCRIPTOR_TYPE_DEVICE, 0,
				sizeof(USB_device_descriptor), (uint8_t *)&buf0[0]);

		if (status == USBH_OK)
		{
			//Note: struct copy operation
			memcpy(&devNew->descriptor, buf0, sizeof(USB_device_descriptor));
		}
	}

	status = usbh_hub_port_enumerate_device(devNew, hubDev, hubPort, configurationNum);
	if (status == USBH_OK)
	{
		// Return pointer to new device made during this enumeration pass.
		*ppDevNew = devNew;
	}
	else
	{
		usbh_free_endpoint(epZero);
		usbh_free_device(devNew);
		usbh_free_hc_2_blocks((void *)hc_ep0_qh);
		// top level device enumeration failed
		if (devNew == usbh_firstDev)
		{
			usbh_firstDev = NULL;
		}
	}
	return status;
}

static void usbh_dev_disconnect_config(USBH_device *devRemove, uint8_t keepDev)
{
	// Pointer to current interface to attach endpoints to.
	USBH_interface *ifaceRemove, *ifaceRemoveNext;
	// Pointer to current endpoint to attach queue headers to.
	USBH_endpoint *epRemove, *epRemoveNext;
	// Pointer to device endpoint0
	//USBH_endpoint *epZero;
	USBH_device *devChild;
	USBH_device *devChildNext;

	if (devRemove)
	{
		// Find all children devices and free them recursively
		devChild = devRemove->children;
		while (devChild)
		{
			devChildNext = devChild->next;
			usbh_dev_disconnect_config(devChild, 0);
			devChild = devChildNext;
		}
		devRemove->children = NULL;

		// Find all interfaces on this device and free
		ifaceRemove = devRemove->interfaces;
		while (ifaceRemove)
		{
			epRemove = ifaceRemove->endpoints;
			while (epRemove)
			{
				epRemoveNext = epRemove->next;
#ifdef USBH_DEBUG_LISTS
				tfp_printf("List disconnect: Remove EP %08x\r\n", epRemove);
#endif // USBH_DEBUG_LISTS
				usbh_free_endpoint(epRemove);
				epRemove = epRemoveNext;
			}
			ifaceRemove->endpoints = NULL;

			ifaceRemoveNext = ifaceRemove->next;
			usbh_free_interface(ifaceRemove);
			ifaceRemove = ifaceRemoveNext;
		}
		devRemove->interfaces = NULL;

#ifdef USBH_DEBUG_LISTS
		tfp_printf("List disconnect: Remove EP0 %08x\r\n", devRemove->endpoint0);
#endif // USBH_DEBUG_LISTS

		if (keepDev == 0)
		{
			usbh_free_endpoint(devRemove->endpoint0);
			usbh_free_device(devRemove);
		}

		// By definition there are free enumeration resources
		// available now.
		enumResourcesFull = 0;
	}
}

static void usbh_dev_disconnect(USBH_device *devRemove)
{
	usbh_dev_disconnect_config(devRemove, 0);
}

/* PREALLOCATED MEMORY BLOCK FUNCTIONS **************************************/

static void usbh_alloc_init(void)
{
	uint32_t i;

	// LOCK
	for (i = 0; i < sizeof(ehci_ram_alloc); i++)
	{
		// Mark unallocated as '0'
		ehci_ram_alloc[i] = 0;
	}
	// UNLOCK

	// Reserve the area allocated to the periodic schedule.
	usbh_alloc_hc_block(ehci_periodic_addr, ehci_periodic_size * sizeof(ft900_ehci_ram_t));


	// Clear all the memory allocated for API storage
	memset(usbh_device_alloc, 0, sizeof(usbh_device_alloc));
	memset(usbh_iface_alloc, 0, sizeof(usbh_iface_alloc));
	memset(usbh_endpoint_alloc, 0, sizeof(usbh_endpoint_alloc));
	memset(usbh_transfer_alloc, 0, sizeof(usbh_transfer_alloc));
}

static USBH_device *usbh_alloc_device(void)
{
	int i;
	USBH_device *devAlloc = 0;

	// LOCK
	for (i = 0; i < USBH_COUNT_DEVICES; i++)
	{
		if (usbh_device_alloc[i].enumValue == 0)
		{
			devAlloc = &usbh_device_alloc[i];
			memset(devAlloc, 0, sizeof(USBH_device));
			// Make sure it is not allocated multiple times.
			devAlloc->enumValue = -1;
			break;
		}
	}
	// UNLOCK

	return devAlloc;
}

static USBH_interface *usbh_alloc_interface(void)
{
	int i;
	USBH_interface *ifaceAlloc = 0;

	// LOCK
	for (i = 0; i < USBH_COUNT_INTERFACES; i++)
	{
		if (usbh_iface_alloc[i].enumValue == 0)
		{
			ifaceAlloc = &usbh_iface_alloc[i];
			memset(ifaceAlloc, 0, sizeof(USBH_interface));
			// Make sure it is not allocated multiple times.
			ifaceAlloc->enumValue = -1;
			break;
		}
	}
	// UNLOCK

	return ifaceAlloc;
}

static USBH_endpoint *usbh_alloc_endpoint(void)
{
	int i;
	USBH_endpoint *epAlloc = 0;

	// LOCK
	for (i = 0; i < USBH_COUNT_ENDPOINTS; i++)
	{
		if (usbh_endpoint_alloc[i].enumValue == 0)
		{
			epAlloc = &usbh_endpoint_alloc[i];
			memset(epAlloc, 0, sizeof(USBH_endpoint));
			// Make sure it is not allocated multiple times.
			epAlloc->enumValue = -1;
			break;
		}
	}
	// UNLOCK

	return epAlloc;
}

static USBH_xfer *usbh_alloc_transfer(void)
{
	int i;
	USBH_xfer *xferAlloc = 0;

	// LOCK
	for (i = 0; i < USBH_COUNT_TRANSFERS; i++)
	{
		if (usbh_transfer_alloc[i].endpoint == 0)
		{
			xferAlloc = &usbh_transfer_alloc[i];
			memset(xferAlloc, 0, sizeof(USBH_xfer));
			xferAlloc->endpoint = (USBH_endpoint *)-1;
			break;
		}
	}
	// UNLOCK

	return xferAlloc;
}

static void usbh_force_close_xfer(USBH_endpoint *ep, USBH_xfer *list)
{
	USBH_xfer *xferThis;
	uint32_t token;

	// Update all timeouts for non-Periodic transfers
	xferThis = list;

	// Decrement timeout on all current transactions
	while (xferThis)
	{
		if (ep == xferThis->endpoint)
		{
			token = EHCI_MEM(xferThis->hc_qtd->token);

			if (token & EHCI_QUEUE_TD_TOKEN_STATUS_ACTIVE)
			{
				// Deactivate qTD
				usbh_unlink_qtd(xferThis);
				//xferThis->token = token & MASK_EHCI_QUEUE_TD_TOKEN_STATUS;
				xferThis->status = USBH_ERR_REMOVED;
			}
		}
		xferThis = xferThis->next;
	};
}

static void usbh_free_endpoint(USBH_endpoint *epFree)
{
	// Remove endpoint from active lists and therefore remove the
	// qH from the Asynchronous or Periodic Queues.
	if ((epFree->type == USBH_EP_BULK) || (epFree->type == USBH_EP_CTRL))
	{
		// For BULK endpoints link in to list of asynch QHs
		usbh_ep_list_remove(epFree, usbh_async_ep_list);
		CRITICAL_SECTION_BEGIN
		// Remove any xfers for this endpoint from asynchronous list.
		usbh_force_close_xfer(epFree, usbh_xferList);
		CRITICAL_SECTION_END
	}
	else
	{
		// For Periodic endpoints link in to list of periodic QHs
		usbh_ep_list_remove(epFree, usbh_periodic_ep_list);
		CRITICAL_SECTION_BEGIN
		// Remove any xfers for this endpoint from periodic list.
		usbh_force_close_xfer(epFree, usbh_xferListPeriodic);
		CRITICAL_SECTION_END
	}

	// Free qH for endpoint.
	usbh_free_hc_2_blocks((void *)epFree->hc_entry.qh);
	if (epFree->hc_dummy_qtd)
	{
		usbh_free_hc_1_block(epFree->hc_dummy_qtd);
	}

	// Trash endpoint (allow memory to be reallocated)
	epFree->enumValue = 0;
}

static void usbh_free_device(USBH_device *devFree)
{
	// Trash device (allow memory to be reallocated)
	devFree->enumValue = 0;
}

static void usbh_free_interface(USBH_interface *ifaceFree)
{
	// Trash interface (allow memory to be reallocated)
	ifaceFree->enumValue = 0;
}

static void usbh_free_transfer(USBH_xfer *xferFree)
{
	// Trash interface (allow memory to be reallocated)
	xferFree->endpoint = 0;
}

/* HC SHARED MEMORY FUNCTIONS ***********************************************/

static void usbh_alloc_hc_block(uint32_t start, uint32_t size)
{
	uint32_t i;

	// LOCK
	for (i = 0; i < sizeof(ehci_ram_alloc); i++)
	{
		// Mark allocated as '1' if unavailable
		if (((i * EHCI_RAM_ALIGN) >= start) &&
				((i * EHCI_RAM_ALIGN) < start + size))
		{
			ehci_ram_alloc[i] = 1;
		}
	}
	// UNLOCK
}

// Allocate 16 DWORDs (64 bytes) in HC shared memory
static void *usbh_alloc_hc_2_blocks(void)
{
	uint32_t i;
	void *hc_alloc = 0;

	// LOCK
	for (i = 0; i < sizeof(ehci_ram_alloc); i++)
	{
		// Find 2 consecutive free blocks for a QH
		if ((ehci_ram_alloc[i] == 0) && (ehci_ram_alloc[i+1] == 0))
		{
			ehci_ram_alloc[i] = 1;
			ehci_ram_alloc[i+1] = 1;
			hc_alloc = (void *)&EHCI_RAM[EHCI_RAM_OFFSET(i * EHCI_RAM_ALIGN)];

#ifdef USBH_DEBUG_ALLOC
			tfp_printf("Alloc: resv 2 blocks %08x\r\n", hc_alloc);
#endif // USBH_DEBUG_ALLOC

			// Zero out queue header
			usbh_memset32_hc(hc_alloc, 0, 64);
			break;
		}
	}
	// UNLOCK

	return hc_alloc;
}

static void usbh_free_hc_2_blocks(void *hc_free)
{
	uint32_t i;

#ifdef USBH_DEBUG_ALLOC
	tfp_printf("Alloc: free 2 blocks %08x\r\n", hc_free);
#endif // USBH_DEBUG_ALLOC

	// LOCK
	i = EHCI_CPU_TO_HC(hc_free) / EHCI_RAM_ALIGN;
	ehci_ram_alloc[i] = 0;
	ehci_ram_alloc[i+1] = 0;
	// UNLOCK
}

// allocate 8 DWORDs (32 bytes) in HC shared memory
static void *usbh_alloc_hc_1_block(void)
{
	uint32_t i;
	void *hc_alloc = 0;

	// LOCK
	for (i = 0; i < sizeof(ehci_ram_alloc); i++)
	{
		// Find 2 consecutive free blocks for a QH
		if (ehci_ram_alloc[i] == 0)
		{
			ehci_ram_alloc[i] = 1;
			hc_alloc = (USBH_qtd *)&EHCI_RAM[EHCI_RAM_OFFSET(i * EHCI_RAM_ALIGN)];

#ifdef USBH_DEBUG_ALLOC
			tfp_printf("Alloc: resv 1 block %08x\r\n", hc_alloc);
#endif // USBH_DEBUG_ALLOC

			// Zero out allocated memory
			usbh_memset32_hc(hc_alloc, 0, 32);
			break;
		}
	}
	// UNLOCK
	return hc_alloc;
}

static void usbh_free_hc_1_block(void *hc_free)
{
	uint32_t i;

#ifdef USBH_DEBUG_ALLOC
	tfp_printf("Alloc: free 1 block %04x\r\n", hc_free);
#endif // USBH_DEBUG_ALLOC

	// LOCK
	i = EHCI_CPU_TO_HC(hc_free) / EHCI_RAM_ALIGN;
	ehci_ram_alloc[i] = 0;
	// UNLOCK
}

static void *usbh_alloc_hc_buffer(uint32_t size)
{
	uint32_t i;
	uint32_t count = 0;
	uint32_t blocks;
	void *hc_buf = 0;

	// Calculate number of blocks required.
	blocks = size / EHCI_RAM_ALIGN;
	// If there are bytes over alignment then add one block to cover.
	if ((blocks * EHCI_RAM_ALIGN) != size)
	{
		blocks++;
	}

	// LOCK
	// Start at top of EHCI RAM and work backwards
	for (i = sizeof(ehci_ram_alloc); i > 0; i--)
	{
		if (count == 0)
		{
			// Restart block detection
			count = blocks;
		}
		// Find n consecutive free blocks for a QH
		if (ehci_ram_alloc[i - 1] == 0)
		{
			count--;
			if (count == 0)
			{
				// Requisite number of free blocks found.
				hc_buf = (void*)&EHCI_RAM[EHCI_RAM_OFFSET((i - 1) * EHCI_RAM_ALIGN)];

				// Mark blocks as allocated.
				count = blocks;
				for (;count > 0; count--, i++)
				{
					ehci_ram_alloc[i - 1] = 1;
				}

#ifdef USBH_DEBUG_ALLOC
				tfp_printf("Alloc: resv buffer %d blocks %08x\r\n", blocks, hc_buf);
#endif // USBH_DEBUG_ALLOC

				break;
			}
		}
		else
		{
			// Allocated block, cannot fit into this gap
			count = 0;
		}
	}
	// UNLOCK
	return hc_buf;
}

static void usbh_free_hc_buffer(void *hc_buf, uint32_t size)
{
	uint32_t i;
	uint32_t blocks;

	// Calculate number of blocks required.
	blocks = size / EHCI_RAM_ALIGN;
	// If there are bytes over alignment then add one block to cover.
	if ((blocks * EHCI_RAM_ALIGN) != size)
	{
		blocks++;
	}

#ifdef USBH_DEBUG_ALLOC
	tfp_printf("Alloc: free buffer %d blocks %08x\r\n", blocks, hc_buf);
#endif // USBH_DEBUG_ALLOC

	// LOCK
	i = EHCI_CPU_TO_HC(hc_buf) / EHCI_RAM_ALIGN;
	while (blocks--)
	{
		// Find n consecutive free blocks for a QH
		if (ehci_ram_alloc[i] == 1)
		{
			ehci_ram_alloc[i] = 0;
		}
		// else trying to free an unallocated block?!
		i++;
	}
	// UNLOCK
}

// Clear 8KB HC shared memory
static void usbh_hc_memclear(void)
{
	uint32_t i;

	for (i = 0; i < 2 * 1024; i++)
	{
		EHCI_RAM[i] = 0;
	}

}

// Set pattern in HC shared memory
static void usbh_memset32_hc(void *dst0, uint32_t val, uint32_t len)
{
	volatile uint32_t *dst = (uint32_t *)dst0;
	uint32_t lenDw;

	// Clear an integral number of DWORDs starting on a DWORD boundary
	lenDw = len / (sizeof(uint32_t));
	while (lenDw--)
	{
		*dst++ = val;
	};
}

void usbh_init_qtd(USBH_qtd *hc_qtd)
{
	EHCI_MEM(hc_qtd->next) = MASK_EHCI_QUEUE_TD_TERMINATE;
	EHCI_MEM(hc_qtd->alt_next) = MASK_EHCI_QUEUE_TD_TERMINATE;
	EHCI_MEM(hc_qtd->token) = EHCI_QUEUE_TD_TOKEN_STATUS_HALTED;
}

/* HARDWARE ACCESS FUNCTIONS ************************************************/

/** @brief
 */
static void usbh_hw_init(void)
{
	uint32_t status;

	// Enable USB EHCI function -- clock configuration
	sys_enable(sys_device_usb_host);

	// BCD Host  -- enable
	//usbh_bcdh_init(0);

	// Perform software reset
	usbh_hw_reset();

	delayms(10);

	// Turn VBUS on
	usbh_set_hub_port_feature(0, 1, USB_HUB_CLASS_FEATURE_PORT_POWER);

	// Wait for VBUSB power to stabilise
	delayms(200);

	// Clear USB RAM
	usbh_hc_memclear();

	//2. periodic frame init
	usbh_periodic_init();

	//3. async schedule init
	usbh_asynch_init();

	//5. Set interrupt
	status = EHCI_REG(usbsts);
	EHCI_REG(usbintr) = 0;
	EHCI_REG(usbsts) |= status;

	usbh_hw_stop();
}

static void usbh_hw_enable_int(void)
{
	uint32_t reg;

	reg = EHCI_REG(usbcmd);
	reg &= (~MASK_EHCI_USBCMD_INT_THRC);
	reg |= (1 << BIT_EHCI_USBCMD_INT_THRC);
	EHCI_REG(usbcmd) = reg;

	EHCI_REG(usbintr) = (MASK_EHCI_USBINTR_USBERR_INT_EN
			| MASK_EHCI_USBINTR_USB_INT_EN
			| MASK_EHCI_USBINTR_PO_CHG_INT_EN );
}

static void usbh_hw_disable_int(void)
{
	EHCI_REG(usbintr) = 0;
}

//0-sdp, 1-dcp, 2-cpd1, 3-cpd2
//TODO: currently not used
#if 0
static void usbh_bcdh_init(uint8_t mode)
{
	// This register some bits are write only. read back is 0;
	SYS->MSC0CFG = MASK_SYS_MSC0CFG_BCDHOST_EN | (mode << BIT_SYS_MSC0CFG_BCDHOST_MODE);
}
#endif // 0

static void usbh_hw_pad_setup(void)
{
	// Pull down OC_N, VBUS_DISCHG, VBUS_DTC
	gpio_pull(0, pad_pull_pulldown);
	gpio_pull(1, pad_pull_pulldown);
	gpio_pull(3, pad_pull_pulldown);

	delayus(100);

	// Set PSW_N pin to low
	gpio_dir(2, pad_dir_output);
	gpio_write(2, 0);
}

static void usbh_hw_run(void)
{
	// Start EHCI Host
	EHCI_REG(usbcmd) |= MASK_EHCI_USBCMD_RS;
}

static void usbh_hw_stop(void)
{
	// Stop EHCI Host
	EHCI_REG(usbcmd) &= ~MASK_EHCI_USBCMD_RS;
}

static void usbh_hw_clrsts(void)
{
	uint32_t ehciStatus;

	ehciStatus = EHCI_REG(usbsts);
	EHCI_REG(usbsts) = ehciStatus; //clear
}

static void usbh_hw_reset (void)
{
	int i;

	SYS->MSC0CFG |= MASK_SYS_MSC0CFG_HOST_RESET_ATX; // reset host atx (bit 13)
	for (i = 0; i < 100; i++)
		;

	SYS->MSC0CFG |= MASK_SYS_MSC0CFG_HOST_RESET_EHCI; // reset host control (bit 14)
	for (i = 0; i < 100; i++)
		;

	SYS->MSC0CFG |= MASK_SYS_MSC0CFG_HOST_RESET_ALL; // reset host all (bit 15)

	while (SYS->MSC0CFG & MASK_SYS_MSC0CFG_HOST_RESET_ALL) //waiting reset clean
	{
		// Waiting software reset auto clear
	}
}

static uint8_t usbh_hw_root_connected(void)
{
	return (EHCI_REG(portsc) & MASK_EHCI_PORTSC_CONN_STS)?1:0;
}

static void usbh_hw_root_c_connect_change(void)
{
	uint32_t value = EHCI_REG(portsc);

	// Do not clear any WC bits.
	value &= (~MASK_EHCI_PORTSC_PO_EN_CHG);
	// Clear the connect change bit.
	value |= MASK_EHCI_PORTSC_CONN_CHG;

	EHCI_REG(portsc) = value;
}

static void usbh_hw_root_c_enable_change(void)
{
	uint32_t value = EHCI_REG(portsc);

	// Do not clear any WC bits.
	value &= (~MASK_EHCI_PORTSC_CONN_CHG);
	// Clear the connect change bit.
	value |= MASK_EHCI_PORTSC_PO_EN_CHG;

	EHCI_REG(portsc) = value;
}

//TODO: currently not used
#if 0
static void usbh_hw_wait_for_connect(void)
{
	//uint32_t ehciStatus;

	// Polling port status if no usb connected
	while (!(usbh_hw_root_connected()))
		; //waiting for usb device plug in
}
#endif // 0

static uint8_t usbh_hw_get_device_speed(void)
{
	uint32_t reg;
	//uint8_t hssp_typ;
	uint8_t speed;
	uint32_t bus_type;

	reg = EHCI_REG(busctrl);
	bus_type = reg & MASK_EHCI_BUSCTRL_HOST_SPD_TYP;
	//bus_typ = reg >> BIT_EHCI_BUSCTRL_HOST_SPD_TYP;
	if (bus_type == EHCI_BUSCTRL_HOST_SPD_TYP_HS)
	{
		//hssp_typ = (EHCI_QUEUE_HEAD_EP_CHAR_EPS_HS >> BIT_EHCI_QUEUE_HEAD_EP_CHAR_EPS);
		speed = USBH_SPEED_HIGH;
	}
	if (bus_type == EHCI_BUSCTRL_HOST_SPD_TYP_FS)
	{
		//hssp_typ = (EHCI_QUEUE_HEAD_EP_CHAR_EPS_FS >> BIT_EHCI_QUEUE_HEAD_EP_CHAR_EPS);
		speed = USBH_SPEED_FULL;
	}
	if (bus_type == EHCI_BUSCTRL_HOST_SPD_TYP_LS)
	{
		//hssp_typ = (EHCI_QUEUE_HEAD_EP_CHAR_EPS_LS >> BIT_EHCI_QUEUE_HEAD_EP_CHAR_EPS);
		speed = USBH_SPEED_LOW;
	}
	return speed;
}

static void usbh_hw_update_root_hub_status(void)
{
	// EHCI registers used to work out root hub status.
	uint32_t busctrl, busintstatus;
	// Value and Change masks for root hub status.
	uint32_t change, value;

	// Copy previous status mask to change variable.
	memcpy(&change, &currentHubStatus, sizeof(change));

	// Read EHCI registers.
	busctrl = EHCI_REG(busctrl);
	busintstatus = EHCI_REG(busintstatus);

	// Fill in current values from EHCI registers.
	currentHubStatus.localPowerSource = busctrl >> BIT_EHCI_BUSCTRL_VBUS_VLD;;
	currentHubStatus.overCurrent = busintstatus >> BIT_EHCI_BUSINTSTATUS_OVC;

	// Copy current mask to calculate change mask.
	memcpy(&value, &currentHubStatus, sizeof(value));

	// Update change bits using exclusive or.
	change = (change & 0xffff) ^ (value & 0xffff);
	// Move changed bits to high word.
	change <<= 16;
	// Or changed bits to high word of status.
	value |= change;

	// Update current status mask and change mask.
	memcpy(&currentHubStatus, &value, sizeof(currentHubStatus));
}

static void usbh_hw_update_root_hub_port_status(void)
{
	// EHCI registers used to work out port status.
	uint32_t busctrl, portsc, busintstatus;
	// Value and Change masks for root hub status.
	uint32_t change, value;

	//USB_hub_port_status lastPortStatus;

	// Copy previous status mask to change variable.
	memcpy(&change, &currentPortStatus, sizeof(change));

	busctrl = EHCI_REG(busctrl);
	portsc = EHCI_REG(portsc);
	busintstatus = EHCI_REG(busintstatus);

	// Port statuses with change bits in the EHCI registers.
	currentPortStatus.portEnabled = portsc >> BIT_EHCI_PORTSC_PO_EN;
	currentPortStatus.currentConnectStatus = portsc >> BIT_EHCI_PORTSC_CONN_STS;

#ifdef USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS
	// Not really needed in current implementation. Maybe usful for applications.
	currentPortStatus.portLowSpeed = busctrl >> BIT_EHCI_BUSCTRL_HOST_SPD_TYP;
	currentPortStatus.portHighSpeed = busctrl >> (BIT_EHCI_BUSCTRL_HOST_SPD_TYP + 1);
#endif // USBH_ENABLE_ROOT_HUB_ALL_PORT_STATUS

	currentPortStatus.portOverCurrent = busintstatus >> BIT_EHCI_BUSINTSTATUS_OVC;
	currentPortStatus.portPower = busctrl >> BIT_EHCI_BUSCTRL_VBUS_VLD;
	currentPortStatus.portReset = portsc >>BIT_EHCI_PORTSC_PO_RESET;
	currentPortStatus.portSuspend = portsc >> BIT_EHCI_PORTSC_PO_SUSP;

	// Copy current mask to calculate change mask.
	memcpy(&value, &currentPortStatus, sizeof(value));

	// Update change bits using exclusive or.
	change = (change & 0xffff) ^ (value & 0xffff);
	// Move changed bits to high word.
	change <<= 16;
	// Or changed bits to high word of status.
	value |= change;

	// Update current status mask and change mask.
	memcpy(&currentPortStatus, &value, sizeof(currentHubStatus));
}

void usbh_hw_wait_for_doorbell(void)
{
	uint32_t doorbell;

	// LOCK
	doorbell = EHCI_REG(usbcmd);
	// If controller is stopped then we can just unlink queue heads
	// without worrying about consistency. Otherwise we have to ring
	// the doorbell.
	if ((doorbell & MASK_EHCI_USBCMD_RS))
	{
#ifdef USBH_DEBUG_LISTS
		tfp_printf("List: Doorbell rung.\r\n");
#endif // USBH_DEBUG_LISTS

		// Turn off actual interrupt for doorbell (we will poll for it).
		EHCI_REG(usbintr) &= (~MASK_EHCI_USBINTR_INT_OAA_EN);

		// Turn on doorbell bit
		doorbell |= MASK_EHCI_USBCMD_INT_OAAD;
		// Clear doorbell interrupt.
		EHCI_REG(usbsts) = MASK_EHCI_USBSTS_INT_OAA;
		// Ring doorbell
		EHCI_REG(usbcmd) = doorbell;

		// Wait for response.
		// (MASK_EHCI_USBCMD_INT_OAAD will be cleared before MASK_EHCI_USBSTS_INT_OAA is set).
		do
		{
		} while ((EHCI_REG(usbsts) & MASK_EHCI_USBSTS_INT_OAA) == 0);

#ifdef USBH_DEBUG_LISTS
		tfp_printf("List: Doorbell answered.\r\n");
#endif // USBH_DEBUG_LISTS

		// Clear response
		EHCI_REG(usbsts) = MASK_EHCI_USBSTS_INT_OAA;
	}
}

static void usbh_hub_poll_start(USBH_device *hubDev)
{
	// Pointer to current interface to attach endpoints to.
	USBH_interface *ifaceHub;
	// Pointer to current endpoint to attach queue headers to.
	USBH_endpoint *epHub;

	if (hubDev->descriptor.bDeviceClass == USB_CLASS_HUB)
	{
		// Get first interface (for Status Change Endpoint)
		ifaceHub = hubDev->interfaces;
		if (ifaceHub)
		{
			if (ifaceHub->set)
			{
				// Get Status Change Endpoint
				epHub = ifaceHub->endpoints;

				if (epHub)
				{
					// Read hub status change endpoint.
					// This happens asynchronously, the transfer is started
					// here (at interrupt level) but completes later. The callback
					// function will therefore not be run at interrupt level.
					if (usbh_transfer_queued_worker(epHub,
							(uint8_t *)&hubDev->hubStatusChange, // Memory location for result.
							epHub->max_packet_size,
							0, // No timeout.
							(uint32_t)hubDev, // "ID" that callback function received.
							&usbh_hub_change) // Callback function.
							!= USBH_OK)
					{
#ifdef USBH_DEBUG_ERROR
						tfp_printf("Hub: could not submit read status change\r\n");
#endif // USBH_DEBUG_ERROR
					}
				}
			}
		}
	}
}


/* API FUNCTIONS ************************************************************/

/** @brief Millisecond timer handler for USB Host stack.
 *  Provides a timeout response to USB transfers that require this functionality.
 */
void USBH_timer(void)
{
	// NOTE: At interrupt level
	usbh_update_xfer_timeouts(usbh_xferList);
	usbh_update_xfer_timeouts(usbh_xferListPeriodic);
}

/** @brief Transaction processor function for USB Host stack.
 * Can be called periodically in application to handle USB transactions or called
 * from an interrupt handler when alerted to changes in state by the EHCI hardware.
 */
int8_t USBH_process(void)
{
	uint8_t intr_xfer, intr_port;

#ifndef USBH_USE_INTERRUPTS

	// Call interrupt handler to update variables.
	ISR_usbh();

#endif // USBH_USE_INTERRUPTS

	CRITICAL_SECTION_BEGIN

	intr_xfer = usbh_intr_xfer;
	usbh_intr_xfer = 0;

	intr_port = usbh_intr_port;
	usbh_intr_port = 0;

	CRITICAL_SECTION_END

	if (intr_xfer)
	{
		// Check for transaction completion.
		//CRITICAL_SECTION_BEGIN
		// delay needed for AOA initialization in some android devices
		delayus(usbh_aoa_compat_delay);
		usbh_update_transactions(&usbh_xferList);
		usbh_update_transactions(&usbh_xferListPeriodic);
		//CRITICAL_SECTION_END
	}

	if (intr_port)
	{
		// Check disconnects/connects.
		// This gets called on disconnect too. Make sure this is not
		// root hub port interrupts during enumeration. These must
		// be ignored to avoid unintended recursion.
		if (!enumInProgress)
		{
			// Ensure that a connect change is enacted.
			currentPortStatus.currentConnectStatusChange = 1;

			// Remove all devices/interfaces/endpoints from root hub.
			usbh_hub_port_remove(NULL, 1);

			return usbh_hub_enumerate(NULL, 0);
		}
	}

	return USBH_OK;
}

/** @brief Initialisation function for USB Host stack.
 * Performs memory configuration and hardware setup for USB Host driver.
 * Will setup interrupt handler if required.
 */
void USBH_initialise(USBH_ctx *ctx)
{
	// Note: context unused for now.
	//(void)ctx;

	usbh_firstDev = NULL;
	usbh_xferListPeriodic = NULL;
	usbh_xferList = NULL;

	usbh_alloc_init();
	usbh_hw_pad_setup();
	usbh_hw_init();

	if (ctx)
	{
		usbh_enum_change_callback = ctx->enumeration_change;
		usbh_aoa_compat_delay = ctx->aoa_compatibility_delay;
	}

	memset(&currentPortStatus, 0, sizeof(currentPortStatus));
	memset(&currentHubStatus, 0, sizeof(currentHubStatus));

	// Clear Status Register
	usbh_hw_clrsts();

#ifdef USBH_USE_INTERRUPTS
	interrupt_attach(interrupt_usb_host, (int8_t)interrupt_usb_host, ISR_usbh);
	usbh_hw_enable_int();
#endif // USBH_USE_INTERRUPTS

	CRITICAL_SECTION_BEGIN
	usbh_intr_port = 1;
	CRITICAL_SECTION_END
}

void USBH_finalise(void)
{
	// Remove all devices/interfaces/endpoints from root hub.
	usbh_hub_port_remove(NULL, 1);
	usbh_hw_disable_int();
	interrupt_detach(interrupt_usb_host);

	CRITICAL_SECTION_BEGIN
				   usbh_intr_xfer = 0;
				   usbh_intr_port = 0;
	CRITICAL_SECTION_END

	// Clear Status Register
	usbh_hw_clrsts();
	usbh_hw_stop();
	usbh_hw_reset();
	sys_disable(sys_device_usb_host);
}

int8_t USBH_endpoint_halt(USBH_endpoint_handle endpoint,
		const uint8_t request)
{
	int32_t status;
	USBH_endpoint *ep = (USBH_endpoint *)USBH_HANDLE_ADDR(endpoint);
	USB_device_request devReq;
	USBH_device *dev;
	USBH_interface *iface;
	uint16_t wIndex;

	if (ep)
	{
		if (ep->enumValue != USBH_HANDLE_ENUM_VAL(endpoint))
		{
			return USBH_ERR_REMOVED;
		}
	}

	iface = ep->interface;
	if (iface == NULL)
		return USBH_ERR_NOT_FOUND;
	dev = iface->device;
	if (dev == NULL)
		return USBH_ERR_NOT_FOUND;

	if (ep->direction == USBH_DIR_IN)
	{
		wIndex = ep->index | USB_ENDPOINT_DESCRIPTOR_EPADDR_IN;
	}
	else
	{
		wIndex = ep->index | USB_ENDPOINT_DESCRIPTOR_EPADDR_OUT;
	}

	if ((request == USB_REQUEST_CODE_CLEAR_FEATURE) ||
			(request == USB_REQUEST_CODE_SET_FEATURE))
	{
		devReq.bRequest = request;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
				USB_BMREQUESTTYPE_STANDARD | USB_BMREQUESTTYPE_RECIPIENT_ENDPOINT;
		devReq.wIndex = wIndex;
		devReq.wValue = USB_FEATURE_ENDPOINT_HALT;
		devReq.wLength = 0;

		status = USBH_device_setup_transfer(USBH_HANDLE(dev, dev->enumValue), &devReq, 0, 0);
		if (status < 0)
		{
			// Note: demotes int32_t to int8_t.
			return (int8_t)status;
		}
		return USBH_OK;
	}
	return USBH_ERR_PARAMETER;
}

int8_t USBH_device_remote_wakeup(USBH_device_handle device,
		const uint8_t request)
{
	int32_t status;
	USB_device_request devReq;

	if ((request == USB_REQUEST_CODE_CLEAR_FEATURE) ||
			(request == USB_REQUEST_CODE_SET_FEATURE))
	{
		devReq.bRequest = request;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
				USB_BMREQUESTTYPE_STANDARD | USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
		devReq.wIndex = 0;
		devReq.wValue = USB_FEATURE_DEVICE_REMOTE_WAKEUP;
		devReq.wLength = 0;

		status = USBH_device_setup_transfer(device, &devReq, 0, 0);
		if (status < 0)
		{
			// Note: demotes int32_t to int8_t.
			return (int8_t)status;
		}

		return status;
	}
	return USBH_ERR_PARAMETER;
}

int8_t USBH_interface_set_host_halt(USBH_endpoint_handle endpoint)
{
	USBH_endpoint *ep = (USBH_endpoint *)USBH_HANDLE_ADDR(endpoint);
	USBH_qh *this_qh;
	uint32_t token;

	if (ep)
	{
		if (ep->enumValue != USBH_HANDLE_ENUM_VAL(endpoint))
		{
			return USBH_ERR_REMOVED;
		}
	}
	else
	{
		return USBH_ERR_PARAMETER;
	}

	if (ep->hc_entry.type == USBH_list_entry_type_qH)
	{
		this_qh = ep->hc_entry.qh;
		if (this_qh == NULL)
			return USBH_ERR_NOT_FOUND;

		// Clear active bit and set halted bit in qTD overlay in qH
		CRITICAL_SECTION_BEGIN
		token = EHCI_MEM(this_qh->transfer_overlay.token);
		token |= (EHCI_QUEUE_TD_TOKEN_STATUS_HALTED);
		EHCI_MEM(this_qh->transfer_overlay.token) = token;
		CRITICAL_SECTION_END

		return USBH_OK;
	}

	return USBH_ERR_USBERR;
}

int8_t USBH_interface_clear_host_halt(USBH_endpoint_handle endpoint)
{
	USBH_endpoint *ep = (USBH_endpoint *)USBH_HANDLE_ADDR(endpoint);
	USBH_qh *this_qh;
	uint32_t token;

	if (ep)
	{
		if (ep->enumValue != USBH_HANDLE_ENUM_VAL(endpoint))
		{
			return USBH_ERR_REMOVED;
		}
	}
	else
	{
		return USBH_ERR_PARAMETER;
	}

	if (ep->hc_entry.type == USBH_list_entry_type_qH)
	{
		this_qh = ep->hc_entry.qh;
		if (this_qh == NULL)
			return USBH_ERR_NOT_FOUND;

		// Clear halt bit in qTD overlay in qH
		CRITICAL_SECTION_BEGIN
		token = EHCI_MEM(this_qh->transfer_overlay.token);
		if (token & EHCI_QUEUE_TD_TOKEN_STATUS_HALTED)
		{
			token &= (~EHCI_QUEUE_TD_TOKEN_STATUS_HALTED);
			EHCI_MEM(this_qh->transfer_overlay.token) = token;
		}
		CRITICAL_SECTION_END

		return USBH_OK;
	}

	return USBH_ERR_USBERR;
}

int8_t USBH_clear_endpoint_transfers(USBH_endpoint_handle endpoint)
{
	USBH_endpoint *epFree = (USBH_endpoint *)USBH_HANDLE_ADDR(endpoint);

	if (epFree)
	{
		if (epFree->enumValue != USBH_HANDLE_ENUM_VAL(endpoint))
		{
			return USBH_ERR_REMOVED;
		}
	}

	// Remove endpoint from active lists and therefore remove the
	// qH from the Asynchronous or Periodic Queues.
	if ((epFree->type == USBH_EP_BULK) || (epFree->type == USBH_EP_CTRL))
	{
		CRITICAL_SECTION_BEGIN
		// Remove any xfers for this endpoint from asynchronous list.
		usbh_force_close_xfer(epFree, usbh_xferList);
		CRITICAL_SECTION_END
	}
	else
	{
		CRITICAL_SECTION_BEGIN
		// Remove any xfers for this endpoint from periodic list.
		usbh_force_close_xfer(epFree, usbh_xferListPeriodic);
		CRITICAL_SECTION_END
	}

	return USBH_OK;
}

int32_t USBH_device_setup_transfer(USBH_device_handle device,
		USB_device_request *req,
		uint8_t       *buffer,
		int16_t       timeout)
{
	int32_t status = USBH_ERR_NOT_FOUND;
	size_t length = req->wLength;
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (dev)
	{
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(device))
		{
			return USBH_ERR_REMOVED;
		}

		status = usbh_setup_transfer(dev->endpoint0, req, buffer, length, timeout);
	}

	return status;
}

int32_t USBH_transfer(USBH_endpoint_handle endpoint,
		uint8_t       *buffer,
		const size_t length,
		uint16_t timeout)
{
	return USBH_transfer_async(endpoint, buffer, length, timeout, 0, NULL);
}

int32_t USBH_transfer_async(USBH_endpoint_handle endpoint,
		uint8_t       *buffer,
		const size_t length,
		uint16_t      timeout,
		uint32_t      id,
		USBH_callback cb
)
{
	// Obtain endpoint from Handle
	USBH_endpoint *ep = (USBH_endpoint *)USBH_HANDLE_ADDR(endpoint);

	if (ep)
	{
		if (ep->enumValue != USBH_HANDLE_ENUM_VAL(endpoint))
		{
			return USBH_ERR_REMOVED;
		}
	}
	else
	{
		return USBH_ERR_PARAMETER;
	}

	if (ep->type == USBH_EP_ISOC)
	{
		return usbh_transfer_iso_worker(ep, buffer, length, timeout, id, cb);
	}

	return usbh_transfer_queued_worker(ep, buffer, length, timeout, id, cb);
}

int8_t USBH_get_device_list(USBH_device_handle device, USBH_device_handle *child)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (child) *child = 0;

	if (dev)
	{
		if (dev->enumValue == USBH_HANDLE_ENUM_VAL(device))
		{
			if (dev->children)
			{
				if (child) *child = USBH_HANDLE(dev->children, dev->children->enumValue);
				return USBH_OK;
			}
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}
	else
	{
		// Return handle for first device.
		if (usbh_firstDev)
		{
			if (child) *child = USBH_HANDLE(usbh_firstDev, usbh_firstDev->enumValue);
			return USBH_OK;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_get_next_device(USBH_device_handle device, USBH_device_handle *next)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (next) *next = 0;

	if (dev)
	{
		if (dev->enumValue == USBH_HANDLE_ENUM_VAL(device))
		{
			if (dev->next)
			{
				if (next) *next = USBH_HANDLE(dev->next, dev->next->enumValue);
				return USBH_OK;
			}
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}
	else
	{
		// Return handle for first device.
		if (usbh_firstDev)
		{
			if (next) *next = USBH_HANDLE(usbh_firstDev, usbh_firstDev->enumValue);
			return USBH_OK;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_get_interface_count(USBH_device_handle device, uint8_t *count)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);
	USBH_interface *iface;

	if (dev)
	{
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(device))
		{
			return USBH_ERR_REMOVED;
		}
	}
	else
	{
		// Use handle for first device.
		if (usbh_firstDev)
		{
			dev = usbh_firstDev;
		}
	}

	if (dev)
	{
		iface = dev->interfaces;
		if (count)
		{
			*count = 0;
			while (iface)
			{
				(*count)++;
				iface = iface->next;
			}
		}
	}
	else
	{
		return USBH_ERR_NOT_FOUND;
	}

	return USBH_OK;
}

int8_t USBH_get_interface_list(USBH_device_handle device, USBH_interface_handle *interface)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);
	USBH_interface *devInterface;

	if (interface) *interface = 0;

	if (dev)
	{
		if (dev->enumValue == USBH_HANDLE_ENUM_VAL(device))
		{
			devInterface = dev->interfaces;
			if (devInterface)
			{
				if (interface) *interface = USBH_HANDLE(devInterface,
						devInterface->enumValue);
				return USBH_OK;
			}
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_get_next_interface(USBH_interface_handle interface, USBH_interface_handle *next)
{
	USBH_interface *iface = (USBH_interface *)USBH_HANDLE_ADDR(interface);

	if (next) *next = 0;

	if (iface)
	{
		if (iface->enumValue == USBH_HANDLE_ENUM_VAL(interface))
		{
			if (iface->next)
			{
				if (next) *next = USBH_HANDLE(iface->next, iface->next->enumValue);
				return USBH_OK;
			}
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_interface_get_info(USBH_interface_handle interface,
		USBH_interface_info *info)
{
	USBH_interface *iface = (USBH_interface *)USBH_HANDLE_ADDR(interface);

	if (iface)
	{
		if (iface->enumValue == USBH_HANDLE_ENUM_VAL(interface))
		{
			if (info)
			{
				info->dev = USBH_HANDLE(iface->device, iface->device->enumValue);
				info->interface_number = iface->number;
				info->alt = iface->alt;
			}
			return USBH_OK;
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_interface_get_class_info(USBH_interface_handle interface,
		uint8_t *usb_class, uint8_t *usb_subclass, uint8_t *usb_protocol)
{
	USBH_interface *iface = (USBH_interface *)USBH_HANDLE_ADDR(interface);

	if (iface)
	{
		if (iface->enumValue == USBH_HANDLE_ENUM_VAL(interface))
		{
			if (usb_class) *usb_class = iface->usb_class;
			if (usb_subclass) *usb_subclass = iface->usb_subclass;
			if (usb_protocol) *usb_protocol = iface->usb_protocol;
			return USBH_OK;
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_get_control_endpoint(USBH_device_handle device, USBH_endpoint_handle *endpoint)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (dev)
	{
		if (dev->enumValue == USBH_HANDLE_ENUM_VAL(device))
		{
			if (endpoint) *endpoint = USBH_HANDLE(dev->endpoint0, dev->endpoint0->enumValue);
			return USBH_OK;
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_device_get_vid_pid(USBH_device_handle device,
		uint16_t *vid,
		uint16_t *pid)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (dev)
	{
		if (dev->enumValue == USBH_HANDLE_ENUM_VAL(device))
		{
			if (vid) *vid = dev->descriptor.idVendor;
			if (pid) *pid = dev->descriptor.idProduct;
			return USBH_OK;
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_device_get_info(USBH_device_handle device,
		USBH_device_info *info)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (dev)
	{
		if (dev->enumValue == USBH_HANDLE_ENUM_VAL(device))
		{
			if (info)
			{
				info->addr = dev->address;
				info->configuration = dev->configuration;
				info->num_configurations = dev->descriptor.bNumConfigurations;
				if (dev->parent != 0)
				{
					info->dev = USBH_HANDLE(dev->parent, dev->parent->enumValue);
				}
				else
				{
					info->dev = 0;
				}
				info->port_number = dev->parentPort;
				info->speed = dev->speed;
			}
			return USBH_OK;
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_get_endpoint_count(USBH_interface_handle interface, uint8_t *count)
{
	USBH_interface *iface = (USBH_interface *)USBH_HANDLE_ADDR(interface);
	USBH_endpoint *endpoint;

	if (iface)
	{
		if (iface->enumValue == USBH_HANDLE_ENUM_VAL(interface))
		{
			if (count)
			{
				endpoint = iface->endpoints;
				*count = 0;
				while (endpoint)
				{
					(*count)++;
					endpoint = endpoint->next;
				}
			}
			return USBH_OK;
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	if (count) *count = 0;
	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_get_endpoint_list(USBH_interface_handle interface, USBH_endpoint_handle *endpoint)
{
	USBH_interface *iface = (USBH_interface *)USBH_HANDLE_ADDR(interface);
	USBH_endpoint *devEndpoint;

	if (endpoint) *endpoint = 0;

	if (iface)
	{
		if (iface->enumValue == USBH_HANDLE_ENUM_VAL(interface))
		{
			devEndpoint = iface->endpoints;
			if (endpoint) *endpoint = USBH_HANDLE(devEndpoint, devEndpoint->enumValue);
			return USBH_OK;
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_get_next_endpoint(USBH_endpoint_handle endpoint, USBH_endpoint_handle *next)
{
	USBH_endpoint *ep = (USBH_endpoint *)USBH_HANDLE_ADDR(endpoint);

	if (next) *next = 0;

	if (ep)
	{
		if (ep->enumValue == USBH_HANDLE_ENUM_VAL(endpoint))
		{
			if (ep->next)
			{
				if (next) *next = USBH_HANDLE(ep->next, ep->next->enumValue);
				return USBH_OK;
			}
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_endpoint_get_info(USBH_endpoint_handle endpoint,
		USBH_endpoint_info *info)
{
	USBH_endpoint *ep = (USBH_endpoint *)USBH_HANDLE_ADDR(endpoint);

	if (ep)
	{
		if (ep->enumValue == USBH_HANDLE_ENUM_VAL(endpoint))
		{
			if (info)
			{
				if (ep->interface)
				{
					info->iface = USBH_HANDLE(ep->interface, ep->interface->enumValue);
				}
				else
				{
					info->iface = 0;
				}
				info->direction = ep->direction;
				info->index = ep->index;
				info->max_packet_size = ep->max_packet_size;
				info->type = ep->type;
			}
			return USBH_OK;
		}
		else
		{
			return USBH_ERR_REMOVED;
		}
	}

	return USBH_ERR_NOT_FOUND;
}

int8_t USBH_enumerate(USBH_device_handle hub, uint8_t hubPort)
{
	int8_t status;
	USBH_device *hubDev = (USBH_device *)USBH_HANDLE_ADDR(hub);
	USBH_device *devNew = NULL;

	// Remove all devices/interfaces/endpoints below this point (where
	// the enumeration has been requested from).
	usbh_hub_port_remove(hubDev, hubPort);

	if (hubPort > 0)
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: start from downstream hub port %d\r\n", hubPort);
#endif // USBH_DEBUG_ENUM

		status = usbh_hub_port_enumerate(hubDev, hubPort, 0, &devNew);
		// Recurse through any hubs
		if (status == USBH_OK)
		{
			if (devNew)
			{
				if (devNew->descriptor.bDeviceClass == USB_CLASS_HUB)
				{
					status = usbh_hub_enumerate(devNew, 0);
					// Now poll downstream hub...
					usbh_hub_poll_start(devNew);
				}
			}
		}
	}
	else
	{
#ifdef USBH_DEBUG_ENUM
		tfp_printf("Enumeration: start from hub\r\n");
#endif // USBH_DEBUG_ENUM
		status = usbh_hub_enumerate(hubDev, 0);
	}

#ifdef USBH_DEBUG_ENUM
	tfp_printf("Enumeration: complete %d\r\n", status);
#endif // USBH_DEBUG_ENUM

	return status;
}

int8_t USBH_get_connect_state(USBH_device_handle hub,
		const uint8_t hubPort,
		USBH_STATE *state)
{
	int8_t status = USBH_OK;
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(hub);
	USB_hub_port_status portStatus;

	*state = USBH_STATE_NOTCONNECTED;

	if (dev)
	{
		status = USBH_get_hub_port_status(hub, hubPort, &portStatus);

		if (status == USBH_OK)
		{
			if (portStatus.currentConnectStatus)
			{
				*state = USBH_STATE_CONNECTED;

				if (dev->children != NULL)
				{
					// Does not report the enumeration status of
					// specific ports on a hub rather it reports
					// the presence of devices.
					if (enumResourcesFull)
					{
						*state = USBH_STATE_ENUMERATED_PARTIAL;
					}
					else
					{
						*state = USBH_STATE_ENUMERATED;
					}
				}
			}
		}
	}
	else
	{
		if (usbh_hw_root_connected())
		{
			*state = USBH_STATE_CONNECTED;

			if (usbh_firstDev)
			{
				if (enumResourcesFull)
				{
					*state = USBH_STATE_ENUMERATED_PARTIAL;
				}
				else
				{
					*state = USBH_STATE_ENUMERATED;
				}
			}
		}
	}

	return status;
}

int8_t USBH_get_controller_state(USBH_CONTROLLER_STATE *state)
{
	uint32_t portsc;

	portsc = EHCI_REG(portsc);
	portsc &= (MASK_EHCI_PORTSC_PO_EN | MASK_EHCI_PORTSC_PO_SUSP);

	if (portsc == (MASK_EHCI_PORTSC_PO_EN | MASK_EHCI_PORTSC_PO_SUSP))
	{
		*state = USBH_CONTROLLER_STATE_SUSPEND;
	}
	else if (portsc == (MASK_EHCI_PORTSC_PO_EN))
	{
		*state = USBH_CONTROLLER_STATE_OPERATIONAL;
	}
	else
	{
		*state = USBH_CONTROLLER_STATE_RESET;
	}

	return USBH_OK;
}

uint16_t USBH_get_frame_number(void)
{
	return (EHCI_REG(frindex) & MASK_EHCI_FRINDEX);
}

int8_t USBH_get_hub_port_count(USBH_device_handle hub,
		uint8_t *count)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(hub);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(hub))
		{
			return USBH_ERR_REMOVED;
		}

		if (dev->descriptor.bDeviceClass == USB_CLASS_HUB)
		{
			if (count) *count = dev->portCount;
		}
		else
		{
			// device is not a hub
			return USBH_ERR_NOT_FOUND;
		}
	}
	else
	{
		// Root hub - only has one port
		if (count) *count = 1;
	}

	return USBH_OK;
}

int8_t USBH_get_hub_status(USBH_device_handle hub,
		USB_hub_status *hubStatus)
{
	int32_t status;
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(hub);
	USB_device_request devReq;
	USBH_endpoint *ep0;

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(hub))
		{
			return USBH_ERR_REMOVED;
		}

		// Not a hub being asked...
		if (dev->descriptor.bDeviceClass != USB_CLASS_HUB)
		{
			return USBH_ERR_NOT_FOUND;
		}

		ep0 = dev->endpoint0;
		if (ep0 == NULL)
			return USBH_ERR_NOT_FOUND;

		devReq.bRequest = USB_REQUEST_CODE_GET_STATUS;
		devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
				USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_HUB;
		devReq.wIndex = 0;
		devReq.wValue = 0;
		devReq.wLength = 4;

		status = USBH_device_setup_transfer(USBH_HANDLE(dev, dev->enumValue), &devReq, (uint8_t *)hubStatus, 0);
		if (status < 0)
		{
			// Note: demotes in32_t to int8_t.
			return (int8_t)status;
		}
	}
	else
	{
		usbh_hw_update_root_hub_status();
		//Note: struct copy operation
		*hubStatus = currentHubStatus;
	}

	return USBH_OK;
}

int8_t USBH_get_hub_port_status(USBH_device_handle hub,
		const uint8_t hubPort,
		USB_hub_port_status *portStatus)
{
	int8_t status = USBH_OK;
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(hub);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(hub))
		{
			return USBH_ERR_REMOVED;
		}

		// Not a hub being asked...
		if (dev->descriptor.bDeviceClass != USB_CLASS_HUB)
		{
			return USBH_ERR_NOT_FOUND;
		}

		status = usbh_get_hub_status_worker(dev, hubPort, portStatus);
	}
	else
	{
		usbh_hw_update_root_hub_port_status();
		//Note: struct copy operation
		*portStatus = currentPortStatus;
	}

	return status;
}

int8_t USBH_hub_set_feature(USBH_device_handle hub,
		const uint16_t feature)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(hub);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(hub))
		{
			return USBH_ERR_REMOVED;
		}

		// Not a hub being asked...
		if (dev->descriptor.bDeviceClass != USB_CLASS_HUB)
		{
			return USBH_ERR_NOT_FOUND;
		}
	}

	return usbh_set_hub_port_feature(dev, 0, feature);
}

int8_t USBH_hub_clear_feature(USBH_device_handle hub,
		const uint16_t feature)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(hub);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(hub))
		{
			return USBH_ERR_REMOVED;
		}

		// Not a hub being asked...
		if (dev->descriptor.bDeviceClass != USB_CLASS_HUB)
		{
			return USBH_ERR_NOT_FOUND;
		}
	}

	return usbh_clear_hub_port_feature(dev, 0, feature);
}

int8_t USBH_hub_set_port_feature(USBH_device_handle hub,
		const uint8_t hubPort,
		const uint16_t feature)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(hub);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(hub))
		{
			return USBH_ERR_REMOVED;
		}

		// Not a hub being asked...
		if (dev->descriptor.bDeviceClass != USB_CLASS_HUB)
		{
			return USBH_ERR_NOT_FOUND;
		}
	}

	return usbh_set_hub_port_feature(dev, hubPort, feature);
}

int8_t USBH_hub_clear_port_feature(USBH_device_handle hub,
		const uint8_t hubPort,
		const uint16_t feature)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(hub);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(hub))
		{
			return USBH_ERR_REMOVED;
		}

		// Not a hub being asked...
		if (dev->descriptor.bDeviceClass != USB_CLASS_HUB)
		{
			return USBH_ERR_NOT_FOUND;
		}
	}

	return usbh_clear_hub_port_feature(dev, hubPort, feature);
}

int8_t USBH_get_device_count(USBH_device_handle device, uint8_t *count)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);
	USBH_device *child;

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(device))
		{
			return USBH_ERR_REMOVED;
		}

		child = dev->children;
		if (count)
		{
			*count = 0;
			while (child)
			{
				(*count)++;
				child = child->next;
			}
		}
	}
	else
	{
		if (count)
		{
			*count = 0;
			if (usbh_firstDev)
				*count = 1;
		}
	}

	return USBH_OK;
}

int8_t USBH_device_get_configuration(USBH_device_handle device,
		uint8_t *conf)
{
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(device))
		{
			return USBH_ERR_REMOVED;
		}
	}
	else
	{
		if (usbh_firstDev == NULL)
		{
			return USBH_ERR_NOT_FOUND;
		}
		dev = usbh_firstDev;
	}

	if (conf) *conf = dev->configuration;

	return USBH_OK;
}

int8_t USBH_device_set_configuration(USBH_device_handle device,
		const uint8_t conf)
{
	int8_t status;
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(device))
		{
			return USBH_ERR_REMOVED;
		}
	}
	else
	{
		if (usbh_firstDev == NULL)
		{
			return USBH_ERR_NOT_FOUND;
		}
		// Snapshot top-level device.
		dev = usbh_firstDev;
	}

	// Re-enumerate from this device downwards with new configuration.
	status = usbh_hub_port_enumerate_device(dev, dev->parent, dev->parentPort, conf);
	if (status == USBH_OK)
	{
		dev->configuration = conf;
	}

	return status;
}

int8_t USBH_device_get_descriptor(USBH_device_handle device,
		uint8_t type, uint8_t index, size_t len, uint8_t *buf)
{
	int8_t status = USBH_ERR_NOT_FOUND;
	USBH_device *dev = (USBH_device *)USBH_HANDLE_ADDR(device);

	if (dev)
	{
		// Non-root hub
		if (dev->enumValue != USBH_HANDLE_ENUM_VAL(device))
		{
			return USBH_ERR_REMOVED;
		}

		status = usbh_req_get_descriptor(dev->endpoint0, type, index, len, buf);
	}

	return status;
}

int8_t USBH_set_interface(USBH_interface_handle interface,
		const uint8_t alt)
{
	int8_t status = USBH_ERR_NOT_FOUND;
	USBH_interface *ifaceNew = (USBH_interface *)USBH_HANDLE_ADDR(interface);
	USBH_interface *ifaceOld;
	USBH_device *dev;
	USBH_endpoint *ep0;
	USBH_endpoint *epNew;
	USBH_endpoint *epOld;

	if (ifaceNew)
	{
		if (ifaceNew->enumValue != USBH_HANDLE_ENUM_VAL(interface))
		{
			return USBH_ERR_REMOVED;
		}

		dev = ifaceNew->device;
		ep0 = dev->endpoint0;

		// Find any previous active interface matching the newly enabled one
		// and disable it.
		ifaceOld = dev->interfaces;
		while (ifaceOld)
		{
			if (ifaceOld->set)
			{
				if (ifaceOld->number == ifaceNew->number)
				{
					ifaceOld->set = 0;
					// There can be only one active interface. Can stop now.
					break;
				}
			}
			ifaceOld = ifaceOld->next;
		}
		if (ifaceOld)
		{
			epOld = ifaceOld->endpoints;
			while (epOld)
			{
				if (epOld->type == USBH_EP_BULK)
				{
					// For BULK endpoints link in to list of asynch QHs
					usbh_ep_list_remove(epOld, usbh_async_ep_list);

					CRITICAL_SECTION_BEGIN
					// Remove any xfers for this endpoint from asynchronous list.
					usbh_force_close_xfer(epOld, usbh_xferList);
					CRITICAL_SECTION_END
				}
				else
				{
					// For Periodic endpoints link in to list of periodic QHs
					usbh_ep_list_remove(epOld, usbh_periodic_ep_list);
					CRITICAL_SECTION_BEGIN
					// Remove any xfers for this endpoint from periodic list.
					usbh_force_close_xfer(epOld, usbh_xferListPeriodic);
					CRITICAL_SECTION_END
				}
				// Free the old queue header if it exists. Iso endpoints will
				// have a null pointer for their iTD or siTDs.
				if (epOld->hc_entry.type == USBH_list_entry_type_qH)
				{
					if (epOld->hc_entry.qh != NULL)
					{
						usbh_free_hc_2_blocks(epOld->hc_entry.qh);
					}
				}
				epOld = epOld->next;
			}
		}
		ifaceNew->set = 1;

		epNew = ifaceNew->endpoints;
		while (epNew)
		{
			// Setup a new queue header for the new interface.
			// This can be qH only.
			if (epNew->hc_entry.type == USBH_list_entry_type_qH)
			{
				epNew->hc_entry.qh = (USBH_qh *)usbh_alloc_hc_2_blocks();
				usbh_init_ep_qh(&epNew->hc_entry, ep0->hc_entry.qh, epNew);
			}
			epNew = epNew->next;
		}

		// Update the queue headers in the periodic tree.
		usbh_update_periodic_tree();

		// Send the request to the interface to enable the alternate endpoints.
		status = usbh_req_set_interface(ep0, ifaceNew->number, alt);
		// set the data toggle to DATA0 for all affected endpoints.
		if ((epNew->type == USBH_EP_BULK) || (epNew->type == USBH_EP_INT))
		{
			uint32_t token;
			token = EHCI_MEM(epNew->hc_entry.qh->transfer_overlay.token);
			token &= (~MASK_EHCI_QUEUE_TD_TOKEN_TOGGLE);
			EHCI_MEM(epNew->hc_entry.qh->transfer_overlay.token) = token;
		}
		else
		{
			// for iso
		}
	}

	return status;
}

