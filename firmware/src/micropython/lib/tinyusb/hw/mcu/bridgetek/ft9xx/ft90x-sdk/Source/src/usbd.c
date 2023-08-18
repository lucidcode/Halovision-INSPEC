/**
    @file

    @brief
    API for USB device stack on FT900.

    API functions for USB Device stack. These functions provide all the functionality
    required to implement a USB Device application.
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

/* CONSTANTS ***********************************************************************/

/**
 @brief The maximum number of endpoints which can be used in
 the library. The default value is for a control endpoints
 and an additional 4 BULK, INTERRUPT or ISOCHRONOUS
 endpoints. The more endpoints defined, the larger the
 interrupt code in ISR_usbd() and the more memory required
 for the USB_ep structure.
 Always count the control endpoint. There are a maximum of 8
 endpoints available.
 */
//@{
#ifndef USBD_MAX_ENDPOINT_COUNT
#if defined(__FT930__)
#define USBD_MAX_ENDPOINT_COUNT 16
#else
#define USBD_MAX_ENDPOINT_COUNT 8
#endif
#endif
//@}

/**
 @brief Enable interrupts to asynchronously update status
 of the endpoints and common events.
 @details The ISR_usbd function will be called at interrupt
 level when enabled by a hardware interrupt. If this is
 disabled then the USBD_process function will call the ISR_usbd
 function after disabling all other interrupts.
 */
#define USBD_USE_INTERRUPTS

/**
 @brief Enable streaming commands to access endpoint FIFO
 registers.
 @details The streamin and streamout assembler commands will
 be used when accessing the FIFO registers for endpoints. This
 is faster than a program loop to perform reads and writes to
 the FIFOs.
 */
#define USBD_USE_STREAMS

/**
 @brief Enable checking of endpoint configurations.
 @details When an endpoint is accessed this will enable certain
  checks to make sure it is configured and the endpoint number is
  valid.
  When endpoints are created this will ensure that the total number
  of bytes available in the endpoint FIFO RAM is not exceeded and
  that the max packet size of the endpoint is allowable for the
  type of endpoint and device speed.
  In all cases the endpoint used must be within the valid range of
  endpoints by the definition USBD_MAX_ENDPOINT_COUNT.
 */
#define USBD_ENDPOINT_CHECKS

#if defined(__FT900__)
#undef USBD_SW_SPEED_DETECT
#endif
/**
    @name    USB Macros to enable/disable debugging output of the driver
    @details These macros will enable debug output to a linked printf function.
 */
//@{
// Debugging only
#undef USBD_LIBRARY_DEBUG_ENABLED
#ifdef USBD_LIBRARY_DEBUG_ENABLED
#undef USBD_DEBUG_CONFIGURE
#undef USBD_DEBUG_INTERRUPTS
#undef USBD_DEBUG_COMMON_INTERRUPTS
#undef USBD_DEBUG_ERRORS
#undef USBD_DEBUG_SETUP_REQ
#undef USBD_DEBUG_IN_PACKET
#undef USBD_DEBUG_OUT_PACKET
#undef USBD_DEBUG_TEST_MODE
#undef USBD_DEBUG_TRANSFER
#include <ft900_uart_simple.h>
//#include "../tinyprintf/tinyprintf.h"
#endif // USBD_LIBRARY_DEBUG_ENABLED
//@}
#if defined(__FT930__)
#define USBD_VBUS_DTC_PIN			39
#else
#define USBD_VBUS_DTC_PIN			3
#endif

/* MACRO for timeout on IN packets. On a timeout, the endpoint is flushed */
#define USBD_TRANSFER_TIMEOUT_CFG		10  //10*200us = 2ms

/* TYPES ***************************************************************************/

/**
    @struct USBD_endpoint
    @brief Structure that represents a USB endpoint.
    This structure is used in all calls to transfer data between the Host
    and Device.
 **/
typedef struct USBD_endpoint
{
	/** Indicates the endpoint is enabled or disabled (not used).
	 **/
	uint8_t enabled;
	/** IN or OUT endpoint. Not applicable for control endpoint.
    Section 3.4.1 FT900 USB Program Manual.
    From DC_EP_CONTROL register in Table 3.10.\n
        0: EP direction OUT.
        1: EP direction IN.
	 **/
	USBD_ENDPOINT_DIR direction;
	/** Endpoint max packet size.
    Control endpoints from from DC_EP0_CONTROL register in Section 3.3.1 FT900
    USB Program Manual, Table 3.6.
    Control endpoints are maximum of 64 bytes.\n
    Non-control endpoints from DC_EP_CONTROL register in Section 3.4.1 FT900
    USB Program Manual.Table 3.10.
    Low-speed and full-speed non-isochronous endpoints are maximum of 64 bytes.\n
        0: 8 bytes.
        1: 16 bytes.
        2: 32 bytes.
        3: 64 bytes.
        4: 128 bytes.
        5: 256 bytes.
        6: 512 bytes.
        7: 1024 bytes.
	 **/
	USBD_ENDPOINT_SIZE max_packet_size;
	/** BULK, ISO, INT (or CTRL) endpoint. Section 3.4.1 FT900 USB Program Manual.
    From DC_EP_CONTROL register in Table 3.10.\n
        0: disabled.
        1: BULK.
        2: INTERRUPT.
        3: ISOCHRONOUS.
        4: CONTROL.
	 **/
	USBD_ENDPOINT_TYPE type;
	/** Enable Double Buffering for this endpoint.
	 **/
	USBD_ENDPOINT_DB db;
	/** Flag to indicate that there are outstanding transactions for this endpoint.
            The interrupt handler will increment this value when action is required for
            the endpoint and the USB_process function will decrement it when complete.
	 */
	volatile uint8_t process;
	/** Last Transaction Status for endpoint. NOT CURRENTLY USED.
        Control endpoints: Section 3.3.2 FT900 USB Program Manual.
        Updated by interrupt handler from register DC_EP0_STATUS in Table 3.7.\n
        Bit 3: STALL sent in response to Setup token.
        Bit 2: Setup token received.
        Bit 1: In packet ready.
        Bit 0: Out packet ready.
        Non-control endpoints: Section 3.4.2 FT900 USB Program Manual.
        Updated by interrupt handler from register DC_EP_STATUS in Table 3.11.\n
        Bit 5: Data error.
        Bit 4: Endpoint stalled.
        Bit 3: Data underrun.
        Bit 2: Data overrun.
        Bit 1: In packet ready.
        Bit 0: Out packet ready.
	 **/
	uint8_t ep_status;
	/** Callback function for data being available to read for OUT endpoints
              or data read by host for IN endpoints.
	 **/
	USBD_ep_callback cb;
} USBD_endpoint;

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
 @name Current state of USB device
 @brief Powered, Default, Address, Configured or Suspended.
 */
static volatile USBD_STATE USBD_current_state;

/**
 @name Remote Wakeup Enable Request flag.
 @brief This flag is set when Remote Wakeup Enable request comes
         through DEVICE_REMOTE_WAKEUP feature selector. USB spec 9.4.5.
 */
static uint8_t USBD_remote_wakeup_enable = 0;

/**
 @name Test mode selector values from TEST MODE feature request.
 @brief This variable is set with the test mode selector values
        when a feature request for TEST MODE comes. USB spec 9.4.9.
 */
static uint8_t USBD_testmode_selector = 0;

#ifdef USBD_SW_SPEED_DETECT
/**
 @name speed detect flag
 @brief Variable used to intiate the USB bus speed detection
         It is reset after bus speed detection is complete. 
 */
static volatile uint8_t USBD_speed_detect = 0;
#endif

/**
 @name Link power management status changed flag.
 @brief Variable used to signal that a Link Power Management
 event has occurred.
 */
//UNUSED: static volatile uint8_t USBD_LPM_changed = 0;

/**
 @name Current USB configuration value.
 @brief Zero when the device is in the Address state but non-
 zero once configured. Must match a valid configuration in the
 configuration descriptor. Set by SET_CONFIGURATION request.
 */
static uint8_t USBD_configuration = 0;

/**
 @name Callbacks for USB device events.
 @brief Callback functions for events. Certain SETUP requests
 and state changes require handling by an application.
 Most standard requests can be handled by the library without
 additional code in an application. If the USBD_standard_req_cb
 callback is not defined then the library will handle standard
 requests but will call USBD_descriptor_req_cb to get descriptors.
 All class and vendor requests must by handled by the application.
 If a request is not supported then the callback must return a
 value other than USBD_OK. If this happens then the library code
 will STALL the request.
 The suspend and resume callbacks are activated when the host
 stops and restarts bus activity (SOFs) respectively.
 The reset callback is used when the host asserts a reset state
 on the USB or the device is removed from the USB.
 */
//{@
/** Standard request callback **/
static USBD_request_callback USBD_standard_req_cb = NULL;
/** Get descriptors for standard requests callback **/
static USBD_descriptor_callback USBD_descriptor_req_cb = NULL;
/** Set configuration standard requests callback **/
static USBD_set_configuration_callback USBD_set_configuration_req_cb = NULL;
/** Set interface standard requests callback **/
static USBD_set_interface_callback USBD_set_interface_req_cb = NULL;
/** Get interface standard requests callback **/
static USBD_get_interface_callback USBD_get_interface_req_cb = NULL;
/** Class request callback **/
static USBD_request_callback USBD_class_req_cb = NULL;
/** Vendor request callback **/
static USBD_request_callback USBD_vendor_req_cb = NULL;
/** Endpoint Feature request callback **/
static USBD_request_callback USBD_endpoint_feature_req_cb = NULL;
/** Device Feature request callback **/
static USBD_request_callback USBD_device_feature_req_cb = NULL;
/** Callback when the device resumes **/
static USBD_suspend_callback USBD_resume_cb = NULL;
/** Callback when the device suspends **/
static USBD_suspend_callback USBD_suspend_cb = NULL;
/** Callback when the host initiates a reset **/
static USBD_reset_callback USBD_reset_cb = NULL;
/** Callback when the host makes a LPM (Link Power Management) change **/
static USBD_suspend_callback USBD_lpm_cb = NULL;
/** Callback when the USB bus signals a SOF **/
static USBD_sof_callback USBD_sof_cb = NULL;
//@}

/**
 @brief Device supports High Speed.
 */
static USBD_DEVICE_SPEED USBD_speed = USBD_SPEED_HIGH;

/**
 @brief Array of pointers to device endpoint information.
 **/
static USBD_endpoint USBD_ep[USBD_MAX_ENDPOINT_COUNT];

/**
 @brief Decode of USB_ENDPOINT_SIZE definitions into actual endpoint
  Max Packet size values.
static const uint16_t USBD_ep_size_bytes[8] =
{ 8, 16, 32, 64, 128, 256, 512, 1024 };
 **/
#define USBD_ep_size_bytes(x)		(8 << (x))

/**
 @brief USBD_test_pattern bytes for high speed test mode;
        see USB 2.0 spec 7.1.20
 **/
static const uint8_t USBD_test_pattern_bytes[53] =
{ 		/* implicit SYNC then DATA0 to start */
		/* JKJKJKJK x9 */
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		/* JJKKJJKK x8 */
		0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
		/* JJJJKKKK x8 */
		0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
		 /* JJJJJJJKKKKKKK x8 */
		0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		/* JJJJJJJK x8 */
		0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd,
		/* JKKKKKKK x10, JK */
		0xfc, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e
		 /* implicit CRC16 then EOP to end */
};

/* MACROS **************************************************************************/

#ifdef USBD_ENDPOINT_CHECKS
#define CHECK_EP(A) \
		{\
	int8_t status = usbd_check_ep(A);\
	if (status != USBD_OK) \
	{\
		/*tfp_printf("EP CHECK FAILED\r\n");*/\
		return status;\
	}\
		}
#else
#define CHECK_EP(A)
#endif // USBD_ENDPOINT_CHECKS

/* LOCAL FUNCTIONS / INLINES *******************************************************/
static int32_t usbd_out_request(uint8_t ep_number,
		uint8_t *buffer, size_t length);
static int32_t usbd_in_request(uint8_t ep_number,
		const uint8_t *buffer, size_t length);
static void usbd_process_setup_packet(void);
static void usbd_restore_endpoints(void);

static inline int8_t usbd_vbus(void)
{
	return gpio_read(USBD_VBUS_DTC_PIN);
}

#ifdef USBD_ENDPOINT_CHECKS
#define USBD_ENDPOINT_DISABLE 0
#define USBD_ENDPOINT_ENABLE  1

static void usbd_set_endpoints(int enable)
{
	uint8_t i;

	// Enable control endpoint and all other endpoints.
	for (i = 0; i < USBD_MAX_ENDPOINT_COUNT; i++)
	{
		if (USBD_ep[i].type != USBD_EP_TYPE_DISABLED)
		{
			USBD_ep[i].enabled = enable;
		}
	}
}
#endif

static inline void usbd_phy_enable(bool en)
{
	if (en)
		SYS->PMCFG_L |= MASK_SYS_PMCFG_DEV_PHY_EN;
	else
		SYS->PMCFG_L &= ~MASK_SYS_PMCFG_DEV_PHY_EN;
}

__attribute__((weak)) void USBD_pipe_isr_start(void)
{
}

__attribute__((weak)) void USBD_pipe_isr_stop(void)
{
}

__attribute__((weak)) void USBD_pipe_isr(uint16_t pipe_bitfields)
{
	for (int i = USBD_EP_1; i < USBD_MAX_ENDPOINT_COUNT; i++)
	{
		if (pipe_bitfields & MASK_USBD_EPIF_IRQ(i))
		{
			USBD_ep[i].process++;
		}
	}
}

static inline void cmif_process(void)
{
	const uint8_t MASKS = MASK_USBD_CMIF_ALL;
	uint8_t cmif = USBD_REG(cmif);

#ifdef USBD_DEBUG_INTERRUPTS
	tfp_printf ("cmif %x ", cmif);
#endif // USBD_DEBUG_INTERRUPTS
	if (!(cmif & MASKS))
	{
		return;
	}
	USBD_REG(cmif) = MASKS;
#ifdef USBD_DEBUG_COMMON_INTERRUPTS
	if (cmif & MASK_USBD_CMIF_PHYIRQ)                               //Handle PHY interrupt
	{
		tfp_printf ("DUSB2 PHYIRQ!\r\n");
	}
#endif // USBD_DEBUG_COMMON_INTERRUPTS
#ifdef USBD_DEBUG_COMMON_INTERRUPTS
	if (cmif & MASK_USBD_CMIF_PIDIRQ)                               //Handle PIDIRQ interrupt
	{
		tfp_printf ("DUSB2 PIDIRQ!\r\n");
	}
#endif // USBD_DEBUG_COMMON_INTERRUPTS
#ifdef USBD_DEBUG_COMMON_INTERRUPTS
	if (cmif & MASK_USBD_CMIF_CRC16IRQ)                             //Handle CRC16IRQ interrupt
	{
		tfp_printf ("DUSB2 CRC16IRQ!\r\n");
	}
#endif // USBD_DEBUG_COMMON_INTERRUPTS
#ifdef USBD_DEBUG_COMMON_INTERRUPTS
	if (cmif & MASK_USBD_CMIF_CRC5IRQ)                              //Handle CRC5 interrupt
	{
		tfp_printf ("DUSB2 CRC5IRQ!\r\n");
	}
#endif // USBD_DEBUG_COMMON_INTERRUPTS
	if (cmif & MASK_USBD_CMIF_RSTIRQ)                               //Handle Reset interrupt
	{
#ifdef USBD_DEBUG_INTERRUPTS
		tfp_printf ("DUSB2 RSTIRQ!\r\n");
#endif // USBD_DEBUG_INTERRUPTS
		{
			if (USBD_current_state == USBD_STATE_NONE)
			{
				return;
			}
			USBD_REG(faddr) = 0;

#ifdef USBD_ENDPOINT_CHECKS
			// Disable all endpoints during callback to make sure that
			// there are no transactions carried out.
			// USBD hardware also disables endpoints and resets endpoint
			// configuration registers when a reset is received.
			usbd_set_endpoints(USBD_ENDPOINT_DISABLE);
#endif

			// Revert remote wakeup enable to default value of off
			USBD_clear_remote_wakeup();

			USBD_current_state = USBD_STATE_DEFAULT;
			if (USBD_reset_cb)
				USBD_reset_cb(0);

			usbd_restore_endpoints();
		}
	}
	if (cmif & MASK_USBD_CMIF_SUSIRQ)                               //Handle Suspend interrupt
	{
#ifdef USBD_DEBUG_INTERRUPTS
		tfp_printf ("DUSB2 SUSIRQ!\r\n");
#endif // USBD_DEBUG_INTERRUPTS
		if (usbd_vbus() && (USBD_current_state < USBD_STATE_SUSPENDED)
				&& (USBD_current_state >= USBD_STATE_DEFAULT))
		{
			USBD_suspend_device();
		}
	}

	if (cmif & MASK_USBD_CMIF_RESIRQ)                               //Handle Resume interrupt
	{
#ifdef USBD_DEBUG_INTERRUPTS
		tfp_printf ("DUSB2 RESIRQ!\r\n");
#endif // USBD_DEBUG_INTERRUPTS
		if (USBD_current_state >= USBD_STATE_SUSPENDED)
		{
			USBD_current_state >>= 4;
			if (USBD_resume_cb)
			{
				USBD_resume_cb(0);
			}
		}
	}

	if (cmif & MASK_USBD_CMIF_SOFIRQ)                               //Handle SOF interrupt
	{
#ifdef USBD_DEBUG_INTERRUPTS
		//tfp_printf ("DUSB2 SOFIRQ!\r\n");
#endif // USBD_DEBUG_INTERRUPTS
		if (USBD_sof_cb)
		{
			// Warning callback at IRQ level
			USBD_sof_cb(USBD_REG(frame));
		}
#ifdef USBD_SW_SPEED_DETECT
	    if (USBD_speed_detect)
	    {
			USBD_REG(cmif) = MASK_USBD_CMIF_SOFIRQ;
	    	USBD_speed_detect = 0;
			delayus(200); /* delay of 200us is used to cater for 125us high speed micro frames */
	    	if (USBD_REG(cmif) & MASK_USBD_CMIF_SOFIRQ)
	    	{
			    /* Another micro frame detected */
	    		USBD_speed = USBD_SPEED_HIGH;
	    	}
	    	else
	    	{
	    		USBD_speed = USBD_SPEED_FULL;
	    	}
#ifdef USBD_DEBUG_INTERRUPTS
	    	tfp_printf ("DUSB2 SOFIRQ: Speed detected: %d!\r\n", USBD_speed);
#endif
	    }
#endif
	}
}

static inline void epif_process(void)
{
#if defined(__FT930__)
	uint16_t epif = USBD_REG(epif);
#else
	uint16_t epif = USBD_REG(epif) & 0x00FF;
#endif

#ifdef USBD_DEBUG_INTERRUPTS
	tfp_printf ("epif %x\r\n", epif);
#endif // USBD_DEBUG_INTERRUPTS
	if (!epif)
	{
		return;
	}

	USBD_REG(epif) = epif;

	if (epif & MASK_USBD_EPIF_EP0IRQ)
	{
#ifdef USBD_DEBUG_INTERRUPTS
		tfp_printf ("EP0IRQ!\r\n");
#endif // USBD_DEBUG_INTERRUPTS
		if (USBD_EP_SR_REG(USBD_EP_0) & MASK_USBD_EP0SR_SETUP)
		{
			//if protocol STALL, End the STALL signalling
			if (USBD_EP_CR_REG(USBD_EP_0) & MASK_USBD_EP0CR_SDSTL)
			{
				// STALL end
				USBD_EP_CR_REG(USBD_EP_0) = USBD_EP_CR_REG(USBD_EP_0) &
						(~MASK_USBD_EP0CR_SDSTL);
				// Clear STALL send.
				USBD_EP_SR_REG(USBD_EP_0) = MASK_USBD_EP0SR_STALL;
			}
			// Host has sent a SETUP packet.
			usbd_process_setup_packet();
			USBD_ep[USBD_EP_0].ep_status = USBD_EP_SR_REG(USBD_EP_0);
		}
		if (epif == MASK_USBD_EPIF_EP0IRQ)
		{
			return;
		}
	}
	USBD_pipe_isr_start();
	USBD_pipe_isr(epif);
	USBD_pipe_isr_stop();
}

void ISR_usbd(void)
{
	cmif_process();
	epif_process();
}

/**
 @brief      USB standard request handler for GET_FEATURE and SET_FEATURE
 @details    Handle standard GET_FEATURE and SET_FEATURE requests
             from the host application.
             The bmRequestType value is parsed and the appropriate
             action or function is performed.
 @param[in]  req - USB_device_request structure containing the
             SETUP portion of the request from the host.
 @return        status - FT51_OK if successful or FT51_FAILED
             if there is an error or the bmRequestType is not
             supported.
 **/
static int8_t usbd_standard_req_set_clear_feature(USB_device_request *req)
{
	USBD_ENDPOINT_NUMBER ep_number = LSB(req->wIndex) & 0x0F;
	USBD_STATE state = USBD_get_state();
	int8_t status = USBD_ERR_NOT_SUPPORTED;

	if (req->bmRequestType == (USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_RECIPIENT_ENDPOINT))
	{
		// Only support the endpoint halt feature in this device
		if (req->wValue == USB_FEATURE_ENDPOINT_HALT)
		{
			// Only allow this if the device is configured or the endpoint
			// is zero if the device is not configured.
			if (((state < USBD_STATE_CONFIGURED) && (ep_number == 0))
					|| (state >= USBD_STATE_CONFIGURED))
			{
				status = USBD_OK;

				if (USBD_endpoint_feature_req_cb)
				{
					status = USBD_endpoint_feature_req_cb(req);
				}

				if (status == USBD_OK)
				{
					// Perform a stall or a clear
					if (req->bRequest == USB_REQUEST_CODE_CLEAR_FEATURE)
					{
						// Feature selector CLEAR
						USBD_clear_endpoint(ep_number);
					}
					else
					{
						// Feature selector SET
						USBD_stall_endpoint(ep_number);
					}
					// ACK packet
					USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
				}
			}
		}
	}
	else if (req->bmRequestType == (USB_BMREQUESTTYPE_DIR_HOST_TO_DEV |
			USB_BMREQUESTTYPE_RECIPIENT_DEVICE))
	{
		uint8_t lIndex = LSB(req->wIndex);
		uint8_t testSelector = MSB(req->wIndex);

		// Perform a set or a clear feature
		switch (req->wValue)
		{
		case USB_FEATURE_DEVICE_REMOTE_WAKEUP:
			status = USBD_OK;
			break;
		case USB_FEATURE_TEST_MODE:
			if (lIndex == 0)
			{
				if ((testSelector >= 1)  && (testSelector <= 4))
				{
					USBD_testmode_selector = testSelector;
					status = USBD_OK;
				}
			}
			break;

		default:
			break;
		}

		if ((status == USBD_OK) && (USBD_device_feature_req_cb))
		{
			status = USBD_device_feature_req_cb(req);
		}

		if (status == USBD_OK)
		{
			// ACK packet
			USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
		}
	}

	return status;
}

__attribute__((weak)) int8_t USBD_standard_req_get_descriptor(
		USB_device_request *req)
{
	(void)req;
	return USBD_ERR_INVALID_PARAMETER;
}

static int8_t usbd_standard_req_get_descriptor(USB_device_request *req)
{
	uint8_t *src = NULL;
	uint16_t length = 0;
	int8_t status = USBD_descriptor_req_cb(req, &src, &length);

	if (status == USBD_OK)
	{
		// One or more IN packets
#if defined USBD_DEBUG_ERRORS  && !defined USBD_USE_STREAMS
		uint16_t transferred =
#endif
				USBD_transfer_ep0(USBD_DIR_IN, src, length, req->wLength);

#if defined USBD_DEBUG_ERRORS  && !defined USBD_USE_STREAMS
		if (length != transferred)
		{
			//status = USBD_ERR_INCOMPLETE;
			tfp_printf("transferred %d of %d\r\n", transferred, length);
		}
#endif // USBD_DEBUG_ERRORS
		// ACK packet
		USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);
	}

	return status;
}

int8_t USBD_req_set_configuration(USB_device_request *req)
{
	int8_t status = USBD_OK;

	// Give application a change to reject this configuration.
	if (USBD_set_configuration_req_cb)
	{
		status = USBD_set_configuration_req_cb(req);
	}

	if (status == USBD_OK)
	{
		if (USBD_current_state >= USBD_STATE_ADDRESS)
		{
			// A value greater than zero means that the device is configured.
			if (req->wValue)
			{
				USBD_current_state = USBD_STATE_CONFIGURED;
#ifdef USBD_ENDPOINT_CHECKS
				usbd_set_endpoints(USBD_ENDPOINT_ENABLE);
#endif

			}
			else
				USBD_current_state = USBD_STATE_ADDRESS;

			// Application has made sure configuration selected is valid.
			USBD_configuration = req->wValue & 0xFF;

			// ACK packet
			USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);

			return USBD_OK;
		}
	}

	return USBD_ERR_NOT_SUPPORTED;
}

int8_t USBD_req_get_configuration(void)
{
	USBD_transfer_ep0(USBD_DIR_IN, &USBD_configuration, 1, 1);

	// ACK packet
	USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);

	return USBD_OK;
}

static int8_t usbd_standard_req_set_interface(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;

	if (USBD_set_interface_req_cb)
	{
		status = USBD_set_interface_req_cb(req);
	}

	if (status == USBD_OK)
	{
		// ACK packet
		USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);
	}
	return status;
}

static int8_t usbd_standard_req_get_interface(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;
	uint8_t val;

	if (USBD_get_interface_req_cb)
	{
		status = USBD_get_interface_req_cb(req, &val);
		if (status == USBD_OK)
		{
			USBD_transfer_ep0(USBD_DIR_IN, &val, 1, req->wLength);
		}
	}

	if (status == USBD_OK)
	{
		// ACK packet
		USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);
	}

	return status;
}

/**
 @brief      USB standard request handler for GET_STATUS
 @details    Handle standard GET_STATUS requests from the host
             application.
             The bmRequestType value is parsed and the appropriate
             action or function is performed.
 @param[in]         req - USB_device_request structure containing the
             SETUP portion of the request from the host.
 @return         status - FT51_OK if successful or FT51_FAILED
             if there is an error or the bmRequestType is not
             supported.
 **/
static int8_t usbd_standard_req_get_status(USB_device_request *req)
{
	USBD_STATE state = USBD_get_state();
	USBD_ENDPOINT_NUMBER ep_number = LSB(req->wIndex) & 0x0F;
	uint8_t buf[2];

	buf[0] = buf[1] = 0;

	// Get Status for endpoints only
	if (req->bmRequestType == (USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_RECIPIENT_ENDPOINT))
	{
		if (((state < USBD_STATE_CONFIGURED) && (ep_number == 0))
				|| (state >= USBD_STATE_CONFIGURED))
		{
			if (USBD_get_ep_stalled(ep_number))
			{
				buf[0] = USB_GET_STATUS_ENDPOINT_HALT;
			}
			USBD_transfer_ep0(USBD_DIR_IN, buf, 2, req->wLength);

			// ACK packet
			USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);

			return USBD_OK;
		}
	}
	else if (req->bmRequestType == (USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
			USB_BMREQUESTTYPE_RECIPIENT_DEVICE))
	{
		USB_device_request dev_req;
		USB_configuration_descriptor *config;
		uint16_t len;

		dev_req.wLength = 255;
		dev_req.wValue = USB_DESCRIPTOR_TYPE_CONFIGURATION << 8;
		dev_req.bRequest = USB_REQUEST_CODE_GET_DESCRIPTOR;
		dev_req.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
				USB_BMREQUESTTYPE_RECIPIENT_DEVICE;
		if (USBD_descriptor_req_cb)
		{
			USBD_descriptor_req_cb(&dev_req, (uint8_t **)&config, &len);
			if (config)
			{
				if ((config->bmAttributes & USB_CONFIG_BMATTRIBUTES_REMOTE_WAKEUP) && (USBD_remote_wakeup_enable))
				{
					buf[0] |= USB_GET_STATUS_DEVICE_REMOTE_WAKEUP;
				}
				if (config->bmAttributes & USB_CONFIG_BMATTRIBUTES_SELF_POWERED)
				{
					buf[0] |= USB_GET_STATUS_DEVICE_SELF_POWERED;
				}
			}
		}
		// This will match the configuration descriptor's bmAttributes
		USBD_transfer_ep0(USBD_DIR_IN, buf, 2, req->wLength);

		// ACK packet
		USBD_transfer_ep0(USBD_DIR_OUT, NULL, 0, 0);

		return USBD_OK;
	}
	return USBD_ERR_NOT_SUPPORTED;
}

/**
 @brief      USB standard request handler
 @details    Handle standard requests from the host application.
 The bRequest value is parsed and the appropriate
 action or function is performed. Additional values
 from the USB_device_request structure are decoded
 and provided to other handlers.
 @param[in]        req - USB_device_request structure containing the
 SETUP portion of the request from the host.
 @return                status - FT51_OK if successful or FT51_FAILED
 if there is an error or the bRequest is not
 supported.
 **/
static int8_t usbd_standard_req(USB_device_request *req)
{
	int8_t status = USBD_ERR_NOT_SUPPORTED;

	switch (req->bRequest)
	{
	case USB_REQUEST_CODE_GET_STATUS:
		status = usbd_standard_req_get_status(req);
		break;

	case USB_REQUEST_CODE_CLEAR_FEATURE:
	case USB_REQUEST_CODE_SET_FEATURE:
		status = usbd_standard_req_set_clear_feature(req);
		break;

	case USB_REQUEST_CODE_GET_DESCRIPTOR:
		if (USBD_descriptor_req_cb)
		{
			status = usbd_standard_req_get_descriptor(req);
		}
		else
		{
			status = USBD_standard_req_get_descriptor(req);
		}
		break;

	case USB_REQUEST_CODE_SET_CONFIGURATION:
		status = USBD_req_set_configuration(req);
		break;

	case USB_REQUEST_CODE_GET_CONFIGURATION:
		status = USBD_req_get_configuration();
		break;

	case USB_REQUEST_CODE_SET_ADDRESS:
		status = USBD_req_set_address(req);
		break;

	case USB_REQUEST_CODE_GET_INTERFACE:
		status = usbd_standard_req_get_interface(req);
		break;

	case USB_REQUEST_CODE_SET_INTERFACE:
		status = usbd_standard_req_set_interface(req);
		break;

	default:
		// Unknown or unsupported request.
		status = USBD_ERR_NOT_SUPPORTED;
		break;
	}

	return status;
}

/*static*/ int8_t usbd_check_ep(USBD_ENDPOINT_NUMBER ep_number)
{

	if (ep_number >= USBD_MAX_ENDPOINT_COUNT)
	{
#ifdef USBD_DEBUG_ERRORS
		tfp_printf("EP%d not legal\r\n", ep_number);
#endif // USBD_DEBUG_ERRORS
		return USBD_ERR_INVALID_PARAMETER;
	}

	if (USBD_ep[ep_number].enabled == 0)
	{
		// Endpoint is not configured.
#ifdef USBD_DEBUG_ERRORS
		tfp_printf("EP%d not configured\r\n", ep_number);
#endif // USBD_DEBUG_ERRORS
		return USBD_ERR_NOT_CONFIGURED;
	}

	return USBD_OK;
}

/* HARDWARE ACCESS FUNCTIONS ************************************************/


static void usbd_hw_enable(void)
{
	uint8_t reg;

	CRITICAL_SECTION_BEGIN
	{
		// Reset USB Device.
		SYS->MSC0CFG = SYS->MSC0CFG | MASK_SYS_MSC0CFG_DEV_RESET_ALL;
		// Disable device connect/disconnect/host reset detection.
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_DIS_DEV;
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_CONN_DEV;
		SYS->PMCFG_L = SYS->PMCFG_L & (~MASK_SYS_PMCFG_DEV_DETECT_EN);

		// Setup VBUS detect GPIO. If the device is connected then this
		// will set the MASK_SYS_PMCFG_DEV_DETECT_EN bit in PMCFG.
		gpio_interrupt_disable(USBD_VBUS_DTC_PIN);
		gpio_function(USBD_VBUS_DTC_PIN, pad_vbus_dtc);
		gpio_pull(USBD_VBUS_DTC_PIN, pad_pull_pulldown);
		gpio_dir(USBD_VBUS_DTC_PIN, pad_dir_input);

		//delayms(1);

		// Enable Chip USB device clock/PM configuration.
		sys_enable(sys_device_usb_device);

		// Wait a short time to get started.
		delayms(1);

		// Turn off the device enable bit. Set the full speed only bit if required.
		USBD_REG(fctrl) = (USBD_speed == USBD_SPEED_FULL?MASK_USBD_FCTRL_MODE_FS_ONLY:0);

		// Clear first reset and suspend interrupts.
		do
		{
			reg = USBD_REG(cmif);
			USBD_REG(cmif) = reg;
		} while (reg);

		// Hardware is now good to go.

		// Disable all interrupts from USB device control.
		USBD_REG(cmie) = 0;

	#ifdef USBD_USE_INTERRUPTS
		interrupt_attach(interrupt_usb_device, (int8_t)interrupt_usb_device, ISR_usbd);
		//interrupt_attach(interrupt_gpio, (int8_t)interrupt_gpio, ISR_usbd);
		// Enable interrupts from USB device control.
		USBD_REG(cmie) = MASK_USBD_CMIE_ALL/*(MASK_USBD_CMIE_RSTIE |
				MASK_USBD_CMIE_SUSIE |
				MASK_USBD_CMIE_RESIE |
				MASK_USBD_CMIE_CRC5IE |
				MASK_USBD_CMIE_CRC16IE |
				MASK_USBD_CMIE_PIDIE |
				MASK_USBD_CMIE_PHYIE |
				MASK_USBD_CMIE_SOFIE)*/;
	#endif // USBD_USE_INTERRUPTS

		// Enable endpoint interrupts for ep0, and ep1 to ep7.
	#ifdef USBD_USE_INTERRUPTS
		USBD_REG(epie) = (MASK_USBD_EPIE_EP0IE |
				((1 << (USBD_MAX_ENDPOINT_COUNT)) - 2));
	#else // USBD_USE_INTERRUPTS
		USBD_REG(epie) = 0;
	#endif // USBD_USE_INTERRUPTS

	#ifdef USBD_DEBUG_CONFIGURE
		tfp_printf ("Interrupt mask for %d endpoints %x\r\n", USBD_MAX_ENDPOINT_COUNT, USBD_REG(epie));
	#endif // USBD_DEBUG_CONFIGURE
	}
	CRITICAL_SECTION_END;
	// Enable device connect/disconnect/host reset detection.
	// Set device detect and remote wakeup enable interrupt enables.
	SYS->PMCFG_L = SYS->PMCFG_L | MASK_SYS_PMCFG_DEV_DETECT_EN;
#if defined(__FT930__)
	// Setup VBUS detect
	SYS->MSC0CFG = SYS->MSC0CFG | MASK_SYS_MSC0CFG_USB_VBUS_EN;
#endif
}

static void usbd_hw_disable(void)
{
	// Disable device connect/disconnect/host reset detection.
	SYS->PMCFG_L = SYS->PMCFG_L & (~MASK_SYS_PMCFG_DEV_DETECT_EN);

#if defined(__FT930__)
		// Disable VBUS detection.
		SYS->MSC0CFG = SYS->MSC0CFG & (~MASK_SYS_MSC0CFG_USB_VBUS_EN);
#endif
	CRITICAL_SECTION_BEGIN
	{
		USBD_REG(epie) = 0;
		USBD_REG(cmie) = 0;
	}
	CRITICAL_SECTION_END;

	/*
	 * // Turn off the device enable bit. Set the full speed only bit if required.
	 * USBD_REG(fctrl) = (USBD_speed == USBD_SPEED_FULL?MASK_USBD_FCTRL_MODE_FS_ONLY:0);
	 */
	// Disable the USB function.
	USBD_REG(fctrl) = 0;
	delayms(1);

	// Disable USB PHY
	usbd_phy_enable(false);
	delayms(1);

	// Disable Chip USB device clock/PM configuration.
	sys_disable(sys_device_usb_device);

	// Reset USB Device... Needed for Back voltage D+ to be <400mV
	SYS->MSC0CFG = SYS->MSC0CFG | MASK_SYS_MSC0CFG_DEV_RESET_ALL;
	
	delayms(1);
}

/**
 @brief \par USB IN Request
 @details Writes the data to the USB hardware.
 @param[in] ep Endpoint to send the IN request to.
 @param[in] buffer The command to write to FT122_CMD.
 @param[in] length Number of bytes to read.
 @return The actual number of bytes written.
 **/
static int32_t usbd_in_request(uint8_t ep_number, const uint8_t *buffer, size_t length)
{
	volatile uint8_t *data_reg;
	int32_t bytes_read = 0;

	CRITICAL_SECTION_BEGIN
	{

#ifdef USBD_DEBUG_IN_PACKET
		tfp_printf("IN %d: ", length);
#endif // USBD_DEBUG_IN_PACKET

#ifdef USBD_USE_STREAMS
		data_reg = (uint8_t *)&(USBD->ep[ep_number].epxfifo);
		if (length)
		{
			if (((uint32_t)buffer) % 4 == 0)
			{
				uint16_t aligned = length & (~3);
				uint16_t left = length & 3;

				if (aligned)
				{
					__asm__ volatile ("streamout.l %0,%1,%2" : :
							"r"(data_reg), "r"(buffer), "r"(aligned));
					buffer += aligned;
				}
				if (left)
				{
					__asm__ volatile ("streamout.b %0,%1,%2" : :
							"r"(data_reg), "r"(buffer), "r"(left));
				}
			}
			else
			{
				__asm__ volatile ("streamout.b %0,%1,%2" : :"r"
						(data_reg), "r"(buffer), "r"(length));
			}
			bytes_read = length;
		}
#else // USBD_USE_STREAMS

	    while (length--)
		{
#ifdef USBD_DEBUG_IN_PACKET
			USBD_EP_FIFO_REG(ep_number) = *buffer;
			tfp_printf("%x ", *buffer);
			buffer++;
#else // USBD_DEBUG_IN_PACKET
			USBD_EP_FIFO_REG(ep_number) = *buffer++;
#endif // USBD_DEBUG_IN_PACKET
			bytes_read++;
		};

#endif // USBD_USE_STREAMS

#ifdef USBD_DEBUG_IN_PACKET
		tfp_printf("\r\n");
#endif // USBD_DEBUG_IN_PACKET
	}
	CRITICAL_SECTION_END;

	return bytes_read;
}

/**
 @brief \par USB OUT Request
 @details Reads data from the USB hardware.
 @param[in] ep Endpoint to read data from.
 @param[in] buffer Suitably-sized buffer to hold the data.
 @return Returns the number of bytes read.
 **/
static int32_t usbd_out_request(uint8_t ep_number, uint8_t *buffer, size_t length)
{
	volatile uint8_t *data_reg;
	uint32_t buff_size = 0;
	uint32_t bytes_read;

	CRITICAL_SECTION_BEGIN
	{

		if (ep_number == USBD_EP_0)
		{
			buff_size = USBD_EP_CNT_REG(USBD_EP_0);
		}
		else
		{
			if (USBD_EP_SR_REG(ep_number) & (MASK_USBD_EPxSR_OPRDY))
			{
				buff_size = USBD_EP_CNT_REG(ep_number);
			}
		}

#ifdef USBD_DEBUG_OUT_PACKET
		tfp_printf("OUT %d/%d: ", length, buff_size);
#endif // USBD_DEBUG_OUT_PACKET

		// Only read as many bytes as we have space for.
		if (buff_size > length)
			buff_size = length;
		bytes_read = buff_size;

#ifdef USBD_USE_STREAMS
		data_reg = (uint8_t *)&(USBD->ep[ep_number].epxfifo);
		if (buff_size)
		{
			if ((uint32_t)buffer % 4 == 0)
			{
				uint16_t aligned = buff_size & (~3);
				uint16_t left = buff_size & 3;

				if (aligned)
				{
					__asm__ volatile ("streamin.l %0,%1,%2" : :
							"r"(buffer), "r"(data_reg), "r"(aligned));
					buffer += aligned;
				}
				if (left)
				{
					__asm__ volatile ("streamin.b %0,%1,%2" : :
							"r"(buffer), "r"(data_reg), "r"(left));
				}
			}
			else
			{
				__asm__ volatile ("streamin.b %0,%1,%2" : :
						"r"(buffer), "r"(data_reg), "r"(buff_size));
			}
		}
#else // USBD_USE_STREAMS

	    while (buff_size--)
		{
#ifdef USBD_DEBUG_OUT_PACKET
			*buffer = USBD_EP_FIFO_REG(ep_number);
			tfp_printf("%x ", *buffer);
			buffer++;
#else // USBD_DEBUG_OUT_PACKET
			*buffer++ = USBD_EP_FIFO_REG(ep_number);
#endif // USBD_DEBUG_OUT_PACKET
		}

#endif // USBD_USE_STREAMS

#ifdef USBD_DEBUG_OUT_PACKET
		tfp_printf("\r\n");
#endif // USBD_DEBUG_OUT_PACKET
	}
	CRITICAL_SECTION_END;

	return bytes_read;
}

/**
 @brief   \par Process USB SETUP packets
 @details Read in a SETUP request token from the USB device and enact any
 callbacks for the standard, class and vendor request types.
 **/
static void usbd_process_setup_packet(void)
{
	USBD_request_callback cb = NULL;
	int8_t status = -1;
	USB_device_request USBD_received_setup_packet;

	//Get Setup Packet from control endpoint
	usbd_out_request(USBD_EP_0, (uint8_t *)&USBD_received_setup_packet, sizeof(USB_device_request));

	// Clear SETUP bit to allow following IN/OUT packets to be received by the
	// control endpoint.
	USBD_EP_SR_REG(USBD_EP_0) = (MASK_USBD_EP0SR_SETUP);

#ifdef USBD_DEBUG_SETUP_REQ
	{
		uint8_t *buffer = (uint8_t *)&USBD_received_setup_packet;
		int i;
		tfp_printf("SETUP REQ: ");
		for (i = 0; i < sizeof(USB_device_request); i++)
			tfp_printf("%x ", *buffer++);
		tfp_printf("\r\n");
	}
#endif // USBD_DEBUG_SETUP_REQ

	// Invoke request-specific handler, if any; otherwise, stall.
	if ((USBD_received_setup_packet.bmRequestType & USB_BMREQUESTTYPE_TYPE_MASK)
			== USB_BMREQUESTTYPE_STANDARD)
	{
		if (USBD_standard_req_cb != NULL)
		{
			cb = USBD_standard_req_cb;
		}
		else
		{
			status = usbd_standard_req(&USBD_received_setup_packet);
		}
	}
	else if ((USBD_received_setup_packet.bmRequestType & USB_BMREQUESTTYPE_TYPE_MASK)
			== USB_BMREQUESTTYPE_CLASS)
	{
		if (USBD_class_req_cb != NULL)
		{
			cb = USBD_class_req_cb;
		}
	}
	else if ((USBD_received_setup_packet.bmRequestType & USB_BMREQUESTTYPE_TYPE_MASK)
			== USB_BMREQUESTTYPE_VENDOR)
	{
		if (USBD_vendor_req_cb != NULL)
		{
			cb = USBD_vendor_req_cb;
		}
	}

	if (cb != NULL)
	{
		status = cb(&USBD_received_setup_packet);

#ifdef USBD_DEBUG_SETUP_REQ
		tfp_printf("SETUP status %d\r\n", status);
#endif // USBD_DEBUG_SETUP_REQ
	}

	if (status != 0)
	{
#ifdef USBD_DEBUG_SETUP_REQ
		tfp_printf("SETUP stall\r\n");
#endif // USBD_DEBUG_SETUP_REQ
		// Request is unrecognised, or handler failed.
		USBD_stall_endpoint(0);
	}
}

static void usbd_restore_endpoints(void)
{
	uint8_t i;

	// Recreate control endpoint and all other endpoints.
	for (i = 0; i < USBD_MAX_ENDPOINT_COUNT; i++)
	{
		if (USBD_ep[i].type != USBD_EP_TYPE_DISABLED)
		{
			USBD_create_endpoint(i, USBD_ep[i].type,
					USBD_ep[i].direction,
					USBD_ep[i].max_packet_size,
					USBD_ep[i].db,
					USBD_ep[i].cb);
		}
	}
}


static void determine_usb_speed(void)
{
	uint8_t  fctrl_val;

	// If USB device function is already enabled then disable it.
	if (USBD_REG(fctrl) & MASK_USBD_FCTRL_USB_DEV_EN) {
		USBD_REG(fctrl) = (USBD_REG(fctrl) & (~MASK_USBD_FCTRL_USB_DEV_EN));
		delayus(200);
	}

	/* User force set to full speed */
	if (USBD_speed == USBD_SPEED_FULL)
	{
		fctrl_val =
			MASK_USBD_FCTRL_USB_DEV_EN | MASK_USBD_FCTRL_MODE_FS_ONLY;
#if defined(__FT900__)
		if (!sys_check_ft900_revB())//if 90x series is rev C
		{
			fctrl_val |= MASK_USBD_FCTRL_IMP_PERF;
		}
#endif
		USBD_REG(fctrl) = fctrl_val;
#ifdef USBD_DEBUG_CONFIGURE
		tfp_printf("Force set to full speed\r\n");
#endif // USBD_DEBUG_CONFIGURE
		return;
	}

	fctrl_val = MASK_USBD_FCTRL_USB_DEV_EN;
#if defined(__FT900__)
	if (!sys_check_ft900_revB())//if 90x series is rev C
	{
		fctrl_val |= MASK_USBD_FCTRL_IMP_PERF;
	}
#endif
	USBD_REG(fctrl) = fctrl_val;

#if defined(__FT930__)
	delayus(200);

	USBD_speed = (SYS->MSC0CFG & MASK_SYS_MSC0CFG_HIGH_SPED_MODE) ?
		USBD_SPEED_HIGH : USBD_SPEED_FULL;
#else /* __FT930__ */
	/* Detection by SOF */
	while (!(USBD_REG(cmif) & MASK_USBD_CMIF_SOFIRQ));
	USBD_REG(cmif) = MASK_USBD_CMIF_SOFIRQ;
	delayus(125 + 5);
	USBD_speed = (USBD_REG(cmif) & MASK_USBD_CMIF_SOFIRQ) ?
		USBD_SPEED_HIGH : USBD_SPEED_FULL;
#endif /* !__FT930__ */
#ifdef USBD_DEBUG_CONFIGURE
	tfp_printf("Detect as highspeed:%d\r\n", USBD_speed);
#endif // USBD_DEBUG_CONFIGURE
}

/**
 @brief \par wait for clearing of INPRDY bit for EPx
 @details Waits on a timeout for clearing of INPRDY bit for EPx.
 @param[in] ep Endpoint to check for ongoing IN transaction.
 @return Returns true is last IN transaction was success, returns false after a timeout.
 **/
static bool usbd_wait_epx_in_ready(USBD_ENDPOINT_NUMBER   ep_number)
{
	uint8_t USBD_transfer_timeout;

	USBD_transfer_timeout = USBD_TRANSFER_TIMEOUT_CFG;
	while (USBD_EP_SR_REG(ep_number) & MASK_USBD_EPxSR_INPRDY)
	{
		if (!USBD_transfer_timeout)
		{
			/** clear an in-process transaction */
			/* Writing a ‘1’ to this bit flushes the next packet to be transmitted from the Endpoint 1 IN FIFO.
			 * The FIFO pointer is reset and the INPRDY bit is cleared.
			 **/
			USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_FIFO_FLUSH);
			return false;
		}
		else
		{
			delayus(200);
			USBD_transfer_timeout--;
		}
	}
	return true;
}

/* API FUNCTIONS ************************************************************/

void USBD_timer(void)
{

}

/** @brief Transaction processor function for USB Device stack.
 * Can be called periodically in application to handle USB transactions or called
 * from an interrupt handler when alerted to changes in state by the EHCI hardware.
 */
__attribute__ ((deprecated)) int8_t USBD_process(void)
{
	if (USBD_current_state < USBD_STATE_DEFAULT)
		return USBD_ERR_NOT_CONFIGURED;

	return USBD_OK;
}

int8_t USBD_create_endpoint(USBD_ENDPOINT_NUMBER ep_num,
		USBD_ENDPOINT_TYPE ep_type,
		USBD_ENDPOINT_DIR ep_dir,
		USBD_ENDPOINT_SIZE ep_size,
		USBD_ENDPOINT_DB ep_db,
		USBD_ep_callback ep_cb)
{
	// Refer to section 6.2.5 for information on obtaining endpoints.
	uint8_t ep_data = 0x00;
	USBD_endpoint *new_ep;

#ifdef USBD_ENDPOINT_CHECKS
	{
		uint8_t i;
		uint16_t ramTot = 0;

		// EP number higher than maximium allowed.
		if (ep_num >= USBD_MAX_ENDPOINT_COUNT)
			return USBD_ERR_NOT_SUPPORTED;

		// EP Size too big
		if ((ep_type != USBD_EP_ISOC) && (ep_type != USBD_EP_INT)
				&& (ep_size > USBD_EP_SIZE_512))
			return USBD_ERR_INVALID_PARAMETER;

		// Check there is enough memory for the EP size requested...

		// Add the control endpoint requirements.
		if (sys_check_ft900_revB())
		{
		ramTot = USBD_ep_size_bytes(USBD_ep[USBD_EP_0].max_packet_size);
		/*RAM total size is including control endpoint size of 64bytes
		 * in case of 90x series rev B
		 */
		}

		// Add in all the other enabled endpoints.
		for (i = USBD_EP_1; i < USBD_MAX_ENDPOINT_COUNT; i++)
		{
			// Add up RAM usage of all active endpoints
			if (USBD_ep[i].enabled)
			{
				// If we are changing or re-enabling our own endpoint
				// then we don't need to count it's size as it will change.
				if (i != ep_num)
				{
					ramTot += USBD_ep_size_bytes(USBD_ep[i].max_packet_size);
					// Double the amount if double buffering is on.
					if (USBD_ep[i].db != USBD_DB_OFF)
					{
						ramTot += USBD_ep_size_bytes(USBD_ep[i].max_packet_size);
					}
				}
			}
		}

		// Add in the requirements for this new endpoint.
		ramTot += USBD_ep_size_bytes(ep_size);
		// Double the amount if double buffering is on.
		if (ep_db != USBD_DB_OFF)
		{
			ramTot += USBD_ep_size_bytes(ep_size);
		}
		if(ramTot > (USBD_RAMTOTAL_IN+USBD_RAMTOTAL_OUT))
		{
			/*The total buffer size is to be shared by all the endpoints */
			return USBD_ERR_RESOURCES;
		}

	}
#endif // USB_ENDPOINT_CHECKS

	new_ep = &USBD_ep[ep_num];

	if (ep_num > 0)
	{
		// Set EP cmd parameters...
		ep_data |= (ep_size << BIT_USBD_EP_MAX_SIZE);

		if (ep_type == USBD_EP_BULK)
			ep_data |= (USBD_EP_DIS_BULK << BIT_USBD_EP_CONTROL_DIS);
		else if (ep_type == USBD_EP_INT)
			ep_data |= (USBD_EP_DIS_INT << BIT_USBD_EP_CONTROL_DIS);
		else if (ep_type == USBD_EP_ISOC)
			ep_data |= (USBD_EP_DIS_ISO << BIT_USBD_EP_CONTROL_DIS);

		if (ep_dir == USBD_DIR_IN)
			ep_data |= MASK_USBD_EPxCR_DIR;

		if (ep_db != USBD_DB_OFF)
			ep_data |= MASK_USBD_EPxCR_DB;

		USBD_EP_CR_REG(ep_num) = ep_data;
	}
	else
	{
		USBD_EP_CR_REG(USBD_EP_0) = (ep_size << BIT_USBD_EP0_MAX_SIZE);
	}

	// All parameters are correct, go on to configure the endpoint...
	new_ep->direction = ep_dir;
	new_ep->max_packet_size = ep_size;
	new_ep->type = ep_type;
	new_ep->db = ep_db;
	new_ep->cb = ep_cb;
	new_ep->enabled = 1;

	return USBD_OK;
}

int8_t USBD_free_endpoint(USBD_ENDPOINT_NUMBER ep_number)
{
	CHECK_EP(ep_number);

	// Disable endpoint
	USBD_EP_CR_REG(ep_number) = 0;
	memset(&USBD_ep[ep_number], 0, sizeof(USBD_endpoint));

	return USBD_OK;
}

uint16_t USBD_ep_max_size(USBD_ENDPOINT_NUMBER ep_number)
{
	CHECK_EP(ep_number);

	return USBD_ep_size_bytes(USBD_ep[ep_number].max_packet_size);
}

void USBD_detach(void)
{
	// Initialise the state.
	USBD_current_state = USBD_STATE_NONE;
	USBD_configuration = 0;
	usbd_hw_disable();
	// Set device detect and remote wakeup enable interrupt enables.
	SYS->PMCFG_L = SYS->PMCFG_L | MASK_SYS_PMCFG_DEV_DETECT_EN;
#if defined(__FT930__)
	// Setup VBUS detect
	SYS->MSC0CFG = SYS->MSC0CFG | MASK_SYS_MSC0CFG_USB_VBUS_EN;
#endif
}

void USBD_attach(void)
{
	if (USBD_current_state != USBD_STATE_NONE)
	{
		return;
	}
	USBD_REG(faddr) = 0;
	// Move state to attached if the device is connected.
	USBD_current_state = USBD_STATE_ATTACHED;
	usbd_hw_enable();
	usbd_restore_endpoints();
}

void USBD_finalise(void)
{
	USBD_detach();

	// Initialise (zero) all the endpoint pointers.
	memset(USBD_ep, 0, sizeof(USBD_ep));
}

void USBD_initialise(USBD_ctx *c)
{
	// Initialise the state.
	USBD_current_state = USBD_STATE_NONE;
	USBD_configuration = 0;

	// Initialise (zero) all the endpoint pointers.
	memset(USBD_ep, 0, sizeof(USBD_ep));

	// Setup default environment.
	USBD_speed = USBD_SPEED_HIGH;

	// Setup software environment.
	if (c)
	{
		// Callbacks for responding to SETUP requests.
		USBD_standard_req_cb = c->standard_req_cb;
		// Callbacks for the built-in standard request handler.
		USBD_descriptor_req_cb = c->get_descriptor_cb;
		USBD_set_configuration_req_cb = c->set_configuration_cb;
		USBD_set_interface_req_cb = c->set_interface_cb;
		USBD_get_interface_req_cb = c->get_interface_cb;
		// Callbacks for class and vendor requests (always handled by application).
		USBD_class_req_cb = c->class_req_cb;
		USBD_vendor_req_cb = c->vendor_req_cb;
		USBD_endpoint_feature_req_cb = c->ep_feature_req_cb;
		USBD_device_feature_req_cb = c->feature_req_cb;
		// Callbacks for handling bus status changes.
		USBD_suspend_cb = c->suspend_cb;
		USBD_resume_cb = c->resume_cb;
		USBD_reset_cb = c->reset_cb;
		USBD_lpm_cb = c->lpm_cb;
		USBD_sof_cb = c->sof_cb;
		// Maximum USB negotiation speed.
		// Note that there is no feedback on whether high speed
		// was allowed.
		USBD_speed = (c->speed > USBD_SPEED_HIGH)?USBD_SPEED_HIGH:c->speed;
		/* Speed is later updated based on the speed detected on the bus
		 * when connected
		 */
	}

	// Each device has, as a minimum, an IN and OUT control endpoint.
	// Control Out
	if (c)
	{
		USBD_ENDPOINT_SIZE ep_size = (c->ep0_size > USBD_EP_SIZE_64)?USBD_EP_SIZE_64:c->ep0_size;
		USBD_create_endpoint(USBD_EP_0, USBD_EP_CTRL, 0, ep_size, 0, c->ep0_cb);
	}
	else
	{
		USBD_create_endpoint(USBD_EP_0, USBD_EP_CTRL, 0, USBD_EP_SIZE_64, 0, NULL);
	}
}

int8_t USBD_is_connected()
{
    return usbd_vbus();
}

int8_t USBD_connect(void)
{

	// Wait for a physical connection to the USB host.
	if (usbd_vbus() == 0)
	{
		return USBD_ERR_DISCONNECTED;
	}
	if (USBD_current_state >= USBD_STATE_POWERED)
	{
		return USBD_OK;
	}

	// Initialise the state.
	USBD_configuration = 0;
	usbd_restore_endpoints();

	// Set device address to zero.
	USBD_REG(faddr) = 0;
	usbd_phy_enable(true);

	CRITICAL_SECTION_BEGIN
	determine_usb_speed();
	// Initialise the current state to be powered.
	USBD_current_state = USBD_STATE_POWERED;
	CRITICAL_SECTION_END

	return USBD_OK;
}

USBD_STATE USBD_get_state(void)
{
	return USBD_current_state;
}

void USBD_set_state(USBD_STATE state)
{
	USBD_current_state = state;
}

void USBD_resume(void)
{

	// Enable USB PHY
	usbd_phy_enable(true);
}

void USBD_wakeup(void)
{
	if (USBD_remote_wakeup_enable)
	{

		SYS->MSC0CFG = SYS->MSC0CFG | MASK_SYS_MSC0CFG_DEV_RMWAKEUP;

		delayms(2);  /* Atleast 2 ms of delay needed for RESUME Data K state */

		SYS->MSC0CFG &= ~MASK_SYS_MSC0CFG_DEV_RMWAKEUP;

		CRITICAL_SECTION_BEGIN
		{
			// Enable USB PHY
			usbd_phy_enable(true);
		}
		CRITICAL_SECTION_END;
	}
}

uint8_t USBD_get_remote_wakeup(void)
{
	return USBD_remote_wakeup_enable;
}

void USBD_set_remote_wakeup(void)
{
	USBD_remote_wakeup_enable = 1;
}

void USBD_clear_remote_wakeup(void)
{
	USBD_remote_wakeup_enable = 0; //USB spec 9.4.5: Remote Wakeup is disabled(reset to zero) when the device is reset.
}

void USBD_set_test_mode(USBD_TESTMODE_SELECT test_selector)
{
	int clear_bit = 0;
	uint8_t mask = 0;
	uint8_t reg;

	switch (test_selector)
	{
		case USBD_TEST_J:
			#ifdef USBD_DEBUG_TEST_MODE
				tfp_printf ("USBD_TEST_J!\r\n");
			#endif // USBD_DEBUG_TEST_MODE
			mask = MASK_USBD_FCTRL_TST_MODE_SELECT0;
			break;
		case USBD_TEST_K:
			#ifdef USBD_DEBUG_TEST_MODE
				tfp_printf ("USBD_TEST_K!\r\n");
			#endif // USBD_DEBUG_TEST_MODE
			mask = MASK_USBD_FCTRL_TST_MODE_SELECT1;
			break;
		case USBD_TEST_SE0_NAK:
			#ifdef USBD_DEBUG_TEST_MODE
					tfp_printf ("USBD_TEST_SE0_NAK!\r\n");
			#endif // USBD_DEBUG_TEST_MODE
			mask = (MASK_USBD_FCTRL_TST_MODE_SELECT0 | MASK_USBD_FCTRL_TST_MODE_SELECT1);
			clear_bit = 1;
			break;
		case USBD_TEST_PACKET:
			#ifdef USBD_DEBUG_TEST_MODE
					tfp_printf ("USBD_TEST_PACKET!\r\n");
			#endif // USBD_DEBUG_TEST_MODE
			mask = (MASK_USBD_FCTRL_TST_MODE_SELECT0 | MASK_USBD_FCTRL_TST_MODE_SELECT1);
			//Fill the EP0 FIFO with Test pattern
			{
				// If there is data to transmit then wait until the IN buffer
				// for the endpoint is empty.
				do
				{
					reg = USBD_EP_SR_REG(USBD_EP_0);
				} while ((reg & MASK_USBD_EP0SR_INPRDY) &&
						(reg & (MASK_USBD_EP0SR_STALL)) == 0);

				// An IN direction SETUP can be interrupted by an OUT packet.
				// This will result in a STALL generated by the silicon.
				while (reg & MASK_USBD_EP0SR_STALL)
				{
					// Clear the STALL and finish the transaction.
					USBD_EP_SR_REG(USBD_EP_0) = (MASK_USBD_EP0SR_STALL | MASK_USBD_EP0SR_DATAEND);
				}

				#ifdef USBD_DEBUG_TEST_MODE
						tfp_printf ("USBD_TEST_PACKET pattern on IN!\r\n");
				#endif // USBD_DEBUG_TEST_MODE
				// Place the data to transmit into the endpoint buffer.
				usbd_in_request(USBD_EP_0, USBD_test_pattern_bytes, 53);
			}
			break;
		default:
			return;
			break;
	}
	CRITICAL_SECTION_BEGIN
	{
		(clear_bit)?(USBD_REG(fctrl) &= ~mask):(USBD_REG(fctrl) |= mask);
		USBD_REG(fctrl) |= MASK_USBD_FCTRL_TST_MODE_ENABLE;
	}
	CRITICAL_SECTION_END;
}

int8_t USBD_req_set_address(USB_device_request *req)
{
	uint8_t address = req->wValue;

	if (USBD_current_state < USBD_STATE_ADDRESS)
	{
		if (address > 127)
		{
			// Greater than the value allowed in the spec so stall!
			USBD_stall_endpoint(USBD_EP_0);
			return USBD_ERR_NOT_SUPPORTED;
		}
		else if (address > 0)
		{
			USBD_current_state = USBD_STATE_ADDRESS; // Move to the address state...
		}
		else
		{
			return USBD_OK;
		}

		address |= 0x80;  // Set the Enable bit
	}
	else if (USBD_current_state == USBD_STATE_ADDRESS)
	{
		if (address == 0)
		{
			USBD_current_state = USBD_STATE_DEFAULT; // Move to the default state...
		}
		else
		{
			return USBD_OK;
		}
	}

	// We do not require to keep the address.
	CRITICAL_SECTION_BEGIN
	{
		USBD_REG(faddr) = address;
	}
	CRITICAL_SECTION_END;

	// ACK packet
	USBD_transfer_ep0(USBD_DIR_IN, NULL, 0, 0);

	// After a SET_ADDRESS then configuration value is zeroed.
	USBD_configuration = 0;

	return USBD_OK;
}

int8_t USBD_ep_buffer_full(USBD_ENDPOINT_NUMBER ep_number)
{
	uint8_t reg;
	int8_t flag = 0;

	CHECK_EP(ep_number);

	reg = USBD_EP_SR_REG(ep_number);

	if (USBD_ep[ep_number].direction == USBD_DIR_IN)
	{
		// IN packet is sitting waiting to be transmitted.
		if (reg & MASK_USBD_EPxSR_INPRDY)
		{
			flag = 1;
		}
	}
	else
	{
		// OUT packet is sitting waiting to be read.
		if (reg & MASK_USBD_EPxSR_OPRDY)
		{
			flag = 1;
		}
	}

	return flag;
}

int32_t USBD_ep_data_rx_count(USBD_ENDPOINT_NUMBER ep_number)
{
	uint8_t reg;
	int32_t buff_size = 0;

	CHECK_EP(ep_number);

	reg = USBD_EP_SR_REG(ep_number);

	if (ep_number == USBD_EP_0)
	{
		buff_size = USBD_EP_CNT_REG(USBD_EP_0);
	}
	else
	{
		if (reg & (MASK_USBD_EPxSR_OPRDY))
		{
		   // size of the OUT packet that is sitting waiting to be read
			buff_size = USBD_EP_CNT_REG(ep_number);
		}
	}

	return buff_size;
}

int8_t USBD_get_ep_stalled(USBD_ENDPOINT_NUMBER ep_number)
{
	CHECK_EP(ep_number);

	if (ep_number > USBD_EP_0)
	{
		if (USBD_EP_CR_REG(ep_number) & MASK_USBD_EPxCR_SDSTL)
		{
			return 1;
		}
	}

	// Control endpoints are never stalled.
	return 0;
}

int8_t USBD_clear_endpoint(USBD_ENDPOINT_NUMBER ep_number)
{
	CHECK_EP(ep_number);

	if (ep_number > USBD_EP_0)
	{
		USBD_EP_SR_REG(ep_number) = MASK_USBD_EPxSR_CLR_TOGGLE;
		USBD_EP_CR_REG(ep_number) = USBD_EP_CR_REG(ep_number) &
				(~MASK_USBD_EPxCR_SDSTL);
	}

	return USBD_OK;
}

int8_t /*__attribute__((optimize("O0")))*/ USBD_stall_endpoint(USBD_ENDPOINT_NUMBER ep_number)
{
	CHECK_EP(ep_number);

	if (ep_number == USBD_EP_0)
	{
		// STALL start
		USBD_EP_CR_REG(USBD_EP_0) = USBD_EP_CR_REG(USBD_EP_0) |
				MASK_USBD_EP0CR_SDSTL;
		
		//Commented out to fix the CV9 test failure in FULL Speed mode,
		//where after a protocol STALL the subsequent host request fails.
		//Clearing the STALL handshake is done in USBD_ISR() when a next SETUP PID comes.
#if 0 
		// Wait for STALL to be sent.
		while((USBD_EP_SR_REG(USBD_EP_0) & MASK_USBD_EP0SR_STALL) == 0x0)
		{
		};

		// STALL end
		USBD_EP_CR_REG(USBD_EP_0) = USBD_EP_CR_REG(USBD_EP_0) &
				(~MASK_USBD_EP0CR_SDSTL);
		// Clear STALL send.
		USBD_EP_SR_REG(USBD_EP_0) = MASK_USBD_EP0SR_STALL;
#endif
	}
	else
	{
		// Cannot stall an isochronous endpoint
		if (USBD_ep[ep_number].type == USBD_EP_ISOC)
		{
			return USBD_ERR_NOT_SUPPORTED;
		}

		USBD_EP_CR_REG(ep_number) = USBD_EP_CR_REG(ep_number) |
				MASK_USBD_EPxCR_SDSTL;
		// Reset data toggle and flush data from the endpoint's buffers.
		USBD_EP_SR_REG(ep_number) = MASK_USBD_EPxSR_CLR_TOGGLE |
				MASK_USBD_EPxSR_FIFO_FLUSH;
	}

	CRITICAL_SECTION_BEGIN
	{
		USBD_ep[ep_number].process = 0;
	}
	CRITICAL_SECTION_END;
	USBD_ep[ep_number].ep_status = USBD_EP_SR_REG(ep_number);

	return USBD_OK;
}

int32_t USBD_transfer(USBD_ENDPOINT_NUMBER   ep_number,
		uint8_t *buffer,
		size_t length)
{
	return USBD_transfer_ex(ep_number, buffer, length, USBD_TRANSFER_EX_PART_NORMAL, 0);
}

int32_t /*__attribute__((optimize("O0")))*/ USBD_transfer_ex(USBD_ENDPOINT_NUMBER   ep_number,
		uint8_t *buffer,
		size_t length,
		uint8_t part,
		size_t offset)
{
	size_t packetLen;
	size_t transferedLen;
	size_t totalLen = length;
	uint8_t *pdata = buffer;
	size_t max_bytes;
	int32_t transferred = 0;
	USBD_ENDPOINT_DIR dir;
#ifdef USBD_DEBUG_TRANSFER
	int	loop_count = 0;
#endif //USBD_DEBUG_TRANSFER

	CHECK_EP(ep_number);

	max_bytes = USBD_ep_size_bytes(USBD_ep[ep_number].max_packet_size);
	dir = USBD_ep[ep_number].direction;

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

		if (dir == USBD_DIR_OUT)
		{
			if (USBD_ep[ep_number].type != USBD_EP_INT)
			{
				// Wait for buffer space to become available.
				while (!(USBD_EP_SR_REG(ep_number) & MASK_USBD_EPxSR_OPRDY));
			}

			// When len is not exactly a the maximum packet size then we
			// know that data-receiving is complete.
			transferedLen = usbd_out_request(ep_number, pdata, packetLen);
			transferred += transferedLen;

			if (transferedLen < packetLen)
			{
				USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_OPRDY);
				break;
			}

			// If this is the last packet.
			if (totalLen == 0)
			{
				if (part == USBD_TRANSFER_EX_PART_NORMAL)
				{
					USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_OPRDY);
				}
				break;
			}
			USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_OPRDY);
		}
		else
		{
			if (USBD_ep[ep_number].type != USBD_EP_INT)
			{
				// Wait for buffer space to become available.
				usbd_wait_epx_in_ready(ep_number);
			}
			transferred += usbd_in_request(ep_number, pdata, packetLen);

			if ((packetLen != 0) && ((packetLen + offset) % max_bytes == 0))
			{
				// When len is exact multiple of packet size, append zero-length
				// packet so that host knows the data-sending is complete.
				if (totalLen == 0)
				{
					// Must not send a zero length packet when we are an interrupt
					// endpoint. This is not required.
					if (USBD_ep[ep_number].type != USBD_EP_INT)
					{
						USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_INPRDY);
						if (part == USBD_TRANSFER_EX_PART_NORMAL)
						{
							// Wait for packet to be transmitted, before sending
							// ZLP.
							usbd_wait_epx_in_ready(ep_number);

#ifdef USBD_DEBUG_TRANSFER
							tfp_printf ("%d:zlp sr=0x%x\n", ep_number, USBD_EP_SR_REG(ep_number));
#endif //USBD_DEBUG_TRANSFER
							(void) usbd_in_request(ep_number, NULL, 0);
						}
					}
				}
			}
			// If this is the last packet.
			if (totalLen == 0)
			{
				if (part == USBD_TRANSFER_EX_PART_NORMAL)
				{
					// Acknowledge end of packet if flag is set.
					USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_INPRDY);
				}
				break;
			}

			// There are still more data to send so just set the INPRDY bit.
			USBD_EP_SR_REG(ep_number) = (MASK_USBD_EPxSR_INPRDY);
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

int32_t /*__attribute__((optimize("O0")))*/ USBD_transfer_ep0(USBD_ENDPOINT_DIR dir,
		uint8_t *buffer,
		size_t dataLength,
		size_t requestLength)
{
	size_t len;
	size_t totlen = dataLength;
	uint8_t *pdata = buffer;
	size_t max_bytes;
	int32_t transferred = 0;

	uint8_t reg;

	CHECK_EP(USBD_EP_0);
	max_bytes = USBD_ep_size_bytes(USBD_ep[USBD_EP_0].max_packet_size);

	do
	{
		// Calculate the packet size. This will be the max packet size
		// of the endpoint or less if there fewer data to send.
		len = totlen;
		if (len > max_bytes)
		{
			len = max_bytes;
		}

		// Decrement the total length remaining in the transfer.
		totlen = totlen - len;

		if (dir == USBD_DIR_OUT)
		{
			// If there is data to receive then wait until the OUT buffer
			// is not empty.
			if (len > 0)
			{
				do
				{
					reg = USBD_EP_SR_REG(USBD_EP_0);
				} while ((reg & (MASK_USBD_EP0SR_OPRDY)) == 0);

				// When len is not exactly a the maximum packet size then we
				// know that data-receiving is complete.
				len = usbd_out_request(USBD_EP_0, pdata, len);
			}

			// If this is the last packet to send then set the DATAEND bit.
			if (totlen == 0)
			{
				USBD_EP_SR_REG(USBD_EP_0) = (MASK_USBD_EP0SR_OPRDY | MASK_USBD_EP0SR_DATAEND);
				// Wait for out packet to be sent.
				transferred += len;
				do
				{
					reg = USBD_EP_SR_REG(USBD_EP_0);
				} while (reg & (MASK_USBD_EP0SR_OPRDY));
				break;
			}

			USBD_EP_SR_REG(USBD_EP_0) = MASK_USBD_EP0SR_OPRDY;

			transferred += len;
			if (len < max_bytes)
			{
				break;
			}
		}
		else
		{
			// If there is data to transmit then wait until the IN buffer
			// for the endpoint is empty.
			do
			{
				reg = USBD_EP_SR_REG(USBD_EP_0);
			} while ((reg & MASK_USBD_EP0SR_INPRDY) &&
					(reg & (MASK_USBD_EP0SR_STALL)) == 0);

			// An IN direction SETUP can be interrupted by an OUT packet.
			// This will result in a STALL generated by the silicon.
			if (reg & MASK_USBD_EP0SR_STALL)
			{
				// Clear the STALL and finish the transaction.
				USBD_EP_SR_REG(USBD_EP_0) = (MASK_USBD_EP0SR_STALL | MASK_USBD_EP0SR_DATAEND);
				break;
			}

			// Place the data to transmit into the endpoint buffer.
			transferred += usbd_in_request(USBD_EP_0, pdata, len);

			// The last packet has been transmitted to the host if:
			// - The exact number of bytes requested by the host have been sent.
			// - A packet of less than the max packet size has been sent.
			// - A zero length packet has been sent.
			if ((len != 0) && (len % max_bytes == 0))
			{
				// When the last packet to send is an exact multiple of the
				// max packet size, and we are sending fewer data than the
				// host requested then we need to send a zero length IN data
				// packet so that host knows the data-sending is complete.
				if ((totlen == 0) && (dataLength < requestLength))
				{
					USBD_EP_SR_REG(USBD_EP_0) = MASK_USBD_EP0SR_INPRDY;

					do
					{
						reg = USBD_EP_SR_REG(USBD_EP_0);
					} while ((reg & MASK_USBD_EP0SR_INPRDY) &&
							(reg & (MASK_USBD_EP0SR_STALL)) == 0);
				}
			}

			// If this is the last packet to send then set the DATAEND bit.
			if (totlen == 0)
			{
				USBD_EP_SR_REG(USBD_EP_0) = (MASK_USBD_EP0SR_INPRDY | MASK_USBD_EP0SR_DATAEND);
				break;
			}

			// There are still more data to send so just set the INPRDY bit.
			USBD_EP_SR_REG(USBD_EP_0) = MASK_USBD_EP0SR_INPRDY;
		}
		// Move pointer to next chunk of data to send.
		pdata += max_bytes;

		// Until all data has been transfered.
	} while (totlen > 0);

	return transferred;
}

USBD_DEVICE_SPEED USBD_get_bus_speed(void)
{
#ifdef	USBD_SW_SPEED_DETECT
	return USBD_speed;
#else
#if defined(__FT930__)
	return (SYS->MSC0CFG & MASK_SYS_MSC0CFG_HIGH_SPED_MODE);
#else
	return USBD_speed;
#endif
#endif
}

void USBD_suspend_device(void)
{
	// Disable USB PHY
	usbd_phy_enable(false);
	// Push current state to top nibble of variable
	USBD_current_state <<= 4;
	// Mark low nibble as suspended therefore
	USBD_current_state |= USBD_STATE_SUSPENDED;

	if (USBD_suspend_cb)
	{
		USBD_suspend_cb(0);
	}
}
