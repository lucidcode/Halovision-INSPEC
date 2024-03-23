/**
    @file ft900_usbh_ft.h

    @brief
    FT devices on USB host stack API.

	@details
    API functions for USB Host stack. These functions provide additional
    functionality useful to implement a USB Host application.
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
 * ("Bridgetek") subject to the licence terms set out
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
 * have additional licence terms that apply to those amendments. However, FTDI
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#ifndef FT900_USBH_FT232_H_
#define FT900_USBH_FT232_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <ft900_usbh.h>

/* CONSTANTS ***********************************************************************/

/**
    @name    Definitions for CDC return values
	@details Set of return values from USB Host API calls.
 */
//@{
/** @brief Success for CDC function.
 */
#define USBH_FT232_OK 0
/** @brief Parameter error in call to CDC function.
 */
#define USBH_FT232_ERR_PARAMETER -1
/** @brief Device class not supported.
 */
#define USBH_FT232_ERR_CLASS_NOT_SUPPORTED -2
/** @brief Class request not supported.
 */
#define USBH_FT232_ERR_CLASS -3
/** @brief Notification Endpoint not found or polling failed.
 */
#define USBH_FT232_ERR_NOTIFICATION_ENDPOINT -4
/** @brief Data Endpoints not found or polling failed.
 */
#define USBH_FT232_ERR_DATA_ENDPOINT -5
/** @brief Function descriptor not found.
 */
#define USBH_FT232_ERR_FUNCTIONAL_DESCRIPTOR -6
/** @brief Unexpected USB error occurred.
 */
#define USBH_FT232_ERR_USB -7
//@}

/**
 * @name CDC Data IN endpoint maximum packet size
 */
//@{
#ifndef USBH_FT232_IN_MAX_PACKET
#define USBH_FT232_IN_MAX_PACKET 512
#endif // USBH_FT232_IN_MAX_PACKET
//@}

/**
 * @name CDC receive buffer size
 */
//@{
#ifndef USBH_FT232_IN_BUFFER
#define USBH_FT232_IN_BUFFER 512
#endif // USBH_FT232_IN_BUFFER
//@}

/**
 * @name FT232 Data Format Modes
 */
//@{
#define USB_FT232_SETDATA_7_BIT  7
#define USB_FT232_SETDATA_8_BIT  8


#define USB_FT232_SETDATA_NOPAR  0
#define USB_FT232_SETDATA_ODDPAR  1
#define USB_FT232_SETDATA_EVENPAR  2
#define USB_FT232_SETDATA_MARKPAR  3
#define USB_FT232_SETDATA_SPACEPAR  4

#define USB_FT232_SETDATA_1_STOP  0
#define USB_FT232_SETDATA_2_STOP  2
//@}

/**
 * @name FT232 Flow Control Modes
 */
//@{
#define USB_FT232_SETFLOWCTRL_NONE    0x0000
#define USB_FT232_SETFLOWCTRL_RTS_CTS 0x0100
#define USB_FT232_SETFLOWCTRL_DTR_DSR 0x0200
#define USB_FT232_SETFLOWCTRL_XON_XOFF 0x0400
//@}

/* TYPES ***************************************************************************/

// FT232 Context
//
// Holds a context structure required by each instance of the FT232 driver
typedef struct USBH_FT232_context {
	/// Handle to the FT232 device. There may be multiple FT232 interfaces
	/// on the same devices.
    USBH_device_handle hDevice;
    /// Interface handles for FT232 DATA interface.
    USBH_interface_handle hDataInterface;
    // Endpoint handles for the FT232 DATA interfaces.
    USBH_endpoint_handle hDataEpIn, hDataEpOut;
    /// bcdDevice from the Device Descriptor. Used to work out the type of
    /// FT232 device we are connected to.
    uint16_t bcdDev;
    /// Interface number for data interface.
    /// These are used in SETUP requests to identify the correct interface.
    uint8_t dataInterfaceNumber;
    /// Last status of FT232 DATA IN endpoint poll from USB Host driver.
    int8_t recvStatus;
    /// Buffer to receive data from USB Host driver. This is exactly one MaxPacket
    /// size buffer. It must be of type uint32_t to follow alignment requirements
    /// of data buffers in USB Host driver.
    /// Note: can be of type uint8_t if it is qualified with:
    /// __attribute__ ((aligned (4)))
    uint32_t recvPacket[USBH_FT232_IN_MAX_PACKET/sizeof(uint32_t)];
    /// Circular buffer used to group packet data from USB Host. This does not
    /// have any alignment issues.
    uint8_t recvBuffer[USBH_FT232_IN_BUFFER];
    /// Read, write and available pointers for  circular buffer.
    volatile uint16_t recvBufferWrite;
    volatile uint16_t recvBufferRead;
    volatile uint16_t recvBufferAvail;
    volatile uint16_t lastModemStatus;
} USBH_FT232_context;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/**
    @brief      Initialise the FT232 driver.
    @details	Setup a context for the FT232 driver to use the interface and settings
    			provided in the call.
    @param		hInterface - handle to the FT232 interface to use.
    @param		flags - None.
    @param		ctx - structure instantiated in the application to hold the context
    			information for this instance of the driver.
    @returns    USBH_FT232_OK if successful
 **/
int8_t USBH_FT232_init(USBH_interface_handle hInterface,
		uint8_t flags, USBH_FT232_context *ctx);

/**
    @brief      Read data from the FT232 device.
    @details	Read a block of data from the FT232 device DATA interface.
    			The data is buffered internally in the driver as it is produced by the FT232
    			device and polled by the USB host.
    			The buffer is designed to discard incoming data if the internal buffer fills.
    			Care must therefore be taken to ensure an adequate consumption rate of data
    			from the FT232 device.
    @param		ctx - context information for this instance of the driver.
    @param                buffer - receiving buffer.
    @param		len - maximum length of data to read.
    @returns    Number of bytes transferred to the receiving buffer. This may be less than
     	 	 	the amount requested if insufficient data has been received from the CDC
     	 	 	device.
 **/
int32_t USBH_FT232_read(USBH_FT232_context *ctx, uint8_t *buffer, size_t len);

/**
    @brief      Write data to the FT232 device.
    @details	Write a block of data to the CDC device DATA interface.
				Data is written immediately to the device without buffering.
    @param		ctx - context information for this instance of the driver.
    @param                buffer - transmission buffer.
    @param		len - maximum length of data to write.
    @returns    Number of bytes transferred from the transmission buffer to the device.
 **/
int32_t USBH_FT232_write(USBH_FT232_context *ctx, uint8_t *buffer, size_t len);

/**
    @brief      Set FT232 Baud Rate.
    @details	The baud rate is passed as a uint32_t and the routine works out
    			the divisor and sub-integer prescalar required. Refer to:
     	 	 	http://www.ftdichip.com/Support/Documents/AppNotes/AN232B-05_BaudRates.pdf
     	 	 	It doesn't check if the baud rate can be calculated within the
     	 	 	+/- 3% required to ensure a stable link.
    @param		ctx - context information for this instance of the driver.
    @param		baud - requested baud rate.
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_set_baud_rate(USBH_FT232_context *ctx, uint32_t baud);

/**
    @brief      Set FT232 Flow Control.
    @details	Flow control can be set as CTS/RTS, DTR/DSR or None.
    @param		ctx - context information for this instance of the driver.
    @param		mode - flow control mode required. Can be zero for none or
					USB_FT232_SETFLOWCTRL_RTS_CTS or USB_FT232_SETFLOWCTRL_DTR_DSR.
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_set_flow_control(USBH_FT232_context *ctx, uint16_t mode);

/**
    @brief      Set FT232 Modem Control.
    @details	Enable RTS, DTR signals for use with flow control and set their
    			current state.
    @param		ctx - context information for this instance of the driver.
    @param		mode - flow control mode USB_FT232_SETFLOWCTRL_RTS_CTS or USB_FT232_SETFLOWCTRL_DTR_DSR
	@param		assert - To set or clear RTS or DTR control signals according to the flow control selected.
					Value 1 to Set RTS or DTR and Value 0 to clear RTS or DTR.
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_set_modem_control(USBH_FT232_context *ctx, uint16_t mode, uint8_t assert);

/**
    @brief      Set FT232 Data Format.
    @details	Data format sets the number of data bits, stop bits and
    			parity mode used.
    @param		ctx - context information for this instance of the driver.
    @param		bits - Can be either USB_FT232_SETDATA_7_BIT or
    				USB_FT232_SETDATA_8_BIT
    @param		parity - Can be one of USB_FT232_SETDATA_NOPAR,
					USB_FT232_SETDATA_ODDPAR, USB_FT232_SETDATA_EVENPAR,
					USB_FT232_SETDATA_MARKPAR, USB_FT232_SETDATA_SPACEPAR.
    @param		stop - Number of stop bits. Can be one of
    				USB_FT232_SETDATA_1_STOP or USB_FT232_SETDATA_2_STOP.
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_set_data(USBH_FT232_context *ctx, uint16_t bits, uint16_t parity, uint16_t stop);

/**
    @brief      Set FT232 Latency Timer
    @details	Latency timer can be set from 2 upwards..
    @param		ctx - context information for this instance of the driver.
    @param		latency - Number of frames between reporting by FT232 device.
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_set_latency(USBH_FT232_context *ctx, uint16_t latency);

/**
    @brief      Returns the last USB Host statuses for endpoint polling.
    @details	Each time an endpoint is polled the status is stored.
    			The data IN endpoint values are stored and can be queried
    			by this function.
    @param		ctx - context information for this instance of the driver.
    @param		data_status - pointer to receive status of data endpoint
    				polling.
 **/
void USBH_FT232_get_poll_status(USBH_FT232_context *ctx, int8_t *data_status);

/**
    @brief      Get FT232 Latency Timer
    @details	Latency timer can be got from 2ms upwards..
    @param		ctx - context information for this instance of the driver.
    @param		latency - Number of frames between reporting by FT232 device.
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_get_latency(USBH_FT232_context *ctx, uint16_t *latency);

/**
    @brief      Gets the modem status and line status from the device.
    @details	The least significant byte of the modemstat parameter holds the modem 
				status. The line status in most significant byte. 
				The modem status is bit-mapped as follows: 
				Clear To Send (CTS) = 0x10, Data Set Ready (DSR) = 0x20, 
				Ring Indicator (RI) = 0x40, Data Carrier Detect (DCD) = 0x80.
				The line status is bit-mapped as follows: 
				Overrun Error (OE) = 0x02, Parity Error (PE) = 0x04, 
				Framing Error (FE) = 0x08, Break Interrupt (BI) = 0x10.
    @param		ctx - context information for this instance of the driver.
    @param		modemstat - Pointer to a variable which receives the modem status 
				and line status from the device.
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_get_modemstat(USBH_FT232_context *ctx, uint16_t *modemstat);

/**
    @brief      Read a value from an EEPROM location.
    @details	Read a value from an EEPROM location.
				EEPROMs for FTDI devices are organised by WORD, so each value returned 
				is 16-bits wide.
    @param		ctx - context information for this instance of the driver.
    @param		e2address - EEPROM location to read from.
    @param		e2data - Pointer to the WORD value read from the EEPROM.			
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_eeprom_read(USBH_FT232_context *ctx, uint16_t e2address, uint16_t *e2data);

/**
    @brief      Set FT232 Bit mode
    @details	Enables different chip mode
    @param		ctx - context information for this instance of the driver.
    @param		mask - Required value for bit mode mask. This sets up which bits are input and outputs
                       A bit value of 0 sets the corresponding pin to an input, a bit value of 1 for an output.
                       In the case of CBUS Bit Bag, the upper nibble of this value controls which pins are
                       inputs and outputs, while the lower nibble controls which of the outputs are high and
                       low.
    @param		mode - Can be one of the following:
    					0x0 - Reset
    					0x1 - Asynchronous Bit Bang
    					0x4 - Synchronous Bit Bang (FT232R, FT245R, FT2232, FT2232H, FT4232H and FT232H devices only)
    					0x20 - CBUS Bit Bang Mode (FT232R and FT232H devices only)
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_set_bitmode(USBH_FT232_context *ctx, uint8_t mask, uint8_t mode);

/**
    @brief      Gets the instantaneous value of the data bus
    @details	Gets the instantaneous value of the data bus
    @param		ctx - context information for this instance of the driver.
    @param		bitmode - pointer to a variable to store the instantaneous data bus value.
    @returns    USBH_FT232_OK - if the interface supports the COMM Feature requests.
    			USBH_FT232_ERR_CLASS - if it does not support it.
 **/
int8_t USBH_FT232_get_bitmode(USBH_FT232_context *ctx, uint8_t *bitmode);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBH_FT232_H_ */
