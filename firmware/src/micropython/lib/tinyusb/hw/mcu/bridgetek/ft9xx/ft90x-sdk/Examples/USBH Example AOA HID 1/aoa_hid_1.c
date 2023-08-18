/**
  @file aoa_hid_1.c
  @brief
  USB AOA HID Device Test

  This example program will send HID reports to an Android accessory.
*/
/*
 * ============================================================================
 * History
 * =======
 * GDM 21 Jan 2015 : Created
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
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include <ft900.h>
#include <ft900_timers.h>
#include <ft900_sys.h>
#include <ft900_interrupt.h>
#include <ft900_gpio.h>
#include <ft900_usb.h>
#include "ft900_usb_aoa.h"
#include <ft900_usbh.h>
#include "ft900_usbh_aoa.h"
// For startup DFU mode optional feature.
#include <ft900_startup_dfu.h>

#include <registers/ft900_registers.h>

/* UART support for printf output. */
#include <ft900_uart_simple.h>
#include "../tinyprintf/tinyprintf.h"

/* For MikroC const qualifier will place variables in Flash
 * not just make them constant.
 */
#if defined(__GNUC__)
#define DESCRIPTOR_QUALIFIER const
#elif defined(__MIKROC_PRO_FOR_FT90x__)
#define DESCRIPTOR_QUALIFIER data
#endif

/* CONSTANTS ***********************************************************************/

/**
 @name Keyboard Emulation Constants
 @brief States and timeouts for keyboard operation
 */
//@{
/// Key press state machine definitions.
#define KEY_ON      1
#define KEY_OFF     2
/// Time from connection to commencing sending of keypresses (in ms).
#define KEY_STARTUP_TIME 20 // 0.02 seconds
/// Length of time between key presses (in ms).
#define KEY_OFF_PERIOD 1
/// How long to press key for (in ms).
#define KEY_ON_PERIOD 1
//@}

/**
 @brief Change this value in order to modify the size of the Tx and Rx ring buffers
  used to implement UART buffering.
 */
#define RINGBUFFER_SIZE (128)

/**
    @name    USB Macros to enable/disable debugging output of the driver
    @details These macros will enable debug output to a linked printf function.
 */
//@{
#define USBH_AOA_UART_KEYS
// Debugging only
#undef USBH_AOA_KEYSCAN_CODES
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/**
 @name hid_report_descriptor_keyboard
 @brief HID Report descriptor for keyboard.

 See Device Class Definition for Human Interface Devices (HID) Version 1.11
 from USB Implementers’ Forum USB.org

 0x05, 0x01,             Usage Page: Generic Desktop Controls
 0x09, 0x06,             Usage: Keyboard
 0xA1, 0x01,             Collection: Application
 0x05, 0x07,               Usage Page: Keyboard
 0x19, 0xE0,               Usage Minimum: Keyboard LeftControl
 0x29, 0xE7,               Usage Maximum: Keyboard Right GUI
 0x15, 0x00,               Logical Minimum: 0
 0x25, 0x01,               Logical Maximum: 1
 0x75, 0x01,               Report Size: 1
 0x95, 0x08,               Report Count: 8
 0x81, 0x02,               Input: Data (2)
 0x95, 0x01,               Report Count: 1
 0x75, 0x08,               Report Size: 8
 0x81, 0x01,               Input: Constant (1)
 0x95, 0x03,               Report Count: 3
 0x75, 0x01,               Report Size: 1
 0x05, 0x08,               Usage Page: LEDs
 0x19, 0x01,               Usage Minimum: Num Lock
 0x29, 0x03,               Usage Maximum: Scroll Lock
 0x91, 0x02,               Output: Data (2)
 0x95, 0x05,               Report Count: 5
 0x75, 0x01,               Report Size: 1
 0x91, 0x01,               Output: Constant (1)
 0x95, 0x06,               Report Count: 6
 0x75, 0x08,               Report Size: 8
 0x15, 0x00,               Logical Minimum: 0
 0x26, 0xFF, 0x00,         Logical Maximum: 255
 0x05, 0x07,               Usage Page: Keyboard/Keypad
 0x19, 0x00,               Usage Minimum: 0
 0x2A, 0xFF, 0x00,         Usage Maximum: 255
 0x81, 0x00,               Input: Data (0)
 0xC0                    End collection
 **/
//@{
DESCRIPTOR_QUALIFIER uint8_t hid_report_descriptor_keyboard[65] =
{ 0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15,
		0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75,
		0x08, 0x81, 0x01, 0x95, 0x03, 0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29,
		0x03, 0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91, 0x01, 0x95, 0x06, 0x75,
		0x08, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x05, 0x07, 0x19, 0x00, 0x2A, 0xFF,
		0x00, 0x81, 0x00, 0xC0, };
//@}

/**
 @struct hid_report_structure
 @brief HID Report structure to match
 hid_report_descriptor_keyboard descriptor.
 **/
typedef struct PACK hid_report_structure_t
{
	union {
		struct {
			unsigned char kbdLeftControl :1;
			unsigned char kbdLeftShift :1;
			unsigned char kbdLeftAlt :1;
			unsigned char kbdLeftGUI :1;
			unsigned char kbdRightControl :1;
			unsigned char kbdRightShift :1;
			unsigned char kbdRightAlt :1;
			unsigned char kbdRightGUI :1;
		};
		unsigned char arrayMap;
	};

	unsigned char arrayNotUsed;  // [1]
	unsigned char arrayKeyboard; // [2]
	unsigned char arrayResv1;
	unsigned char arrayResv2;
	unsigned char arrayResv3;
	unsigned char arrayResv4;
	unsigned char arrayResv5;
} hid_report_structure_t;

/**
 @name report_buffer
 @brief Buffer used to report back to host the current state of keypresses.
 */
hid_report_structure_t report_buffer;

/**
 @name Keyboard Scan Codes Lookup table (for CSI escaped characters)
 @brief Lookup table for CSI escaped characters to scan codes.
 @details Columns are [0] Escaped string from UART terminal.
                      [1] keyscan code to send in HID report.
                      [2] non-zero to set shift key pressed.
                      [3] non-zero to set ctrl key pressed.
                      [4] non-zero to set alt key pressed.
	A tilde '~' or a capital letter signify the end	of an escape sequence.
 */
//@{
struct escape_mappings
{
	char msg[8];
	uint8_t scan;
	uint8_t shift;
	uint8_t ctrl;
	uint8_t alt;
};

const struct escape_mappings ESCAPED_scancode[] =
{
		{"\x1B\x0D", 116, 0, 0, 0 }, // Execute - Alt+Enter
		{"\x1B[D", 80, 0, 0, 0 }, // Left
		{"\x1B[C", 79, 0, 0, 0 }, // Right
		{"\x1B[B", 81, 0, 0, 0 }, // Down
		{"\x1B[A", 82, 0, 0, 0 }, // Up
		{"\x1B[Z", 43, 1, 0, 0 }, // Tab Left - Shift Tab
		{"\x1B[1~", 74, 0, 0, 0 }, // Home
		{"\x1B[4~", 77, 0, 0, 0 }, // End
		{"\x1B[5~", 75, 0, 0, 0 }, // Page Up
		{"\x1B[6~", 78, 0, 0, 0 }, // Page Down
		{"\x1B[2~", 73, 0, 0, 0 }, // Insert
		{"\x1B[3~", 76, 0, 0, 0 }, // Delete
		{"\x1B[11~", 102, 0, 0, 0 }, // Power button - F1
		{"\x1B[17~", 127, 0, 0, 0 }, // Mute button - F6
		{"\x1B[18~", 129, 0, 0, 0 }, // Volume Down button - F7
		{"\x1B[19~", 128, 0, 0, 0 }, // Volume Up button -F8
		{"\x1B[21~", 234, 0, 0, 0 }, // Prev button - F9
		{"\x1B[23~", 232, 0, 0, 0 }, // Play button - F10
		{"\x1B[24~", 235, 0, 0, 0}, // Next button - F11

		{"", 0, 0, 0, 0 }, // End marker.
};
//@}

/**
 @name Keyboard Scan Codes Lookup table (for ASCII keys)
 @brief Lookup table for converting ASCII to scan codes.
 @details Columns are [0] ASCII character from UART terminal
                      [1] keyscan code to send in HID report.
                      [2] non-zero to set shift key pressed.
                      [3] non-zero to set ctrl key pressed.
                      [4] non-zero to set alt key pressed.
 */
//@{
struct key_mappings
{
	char msg;
	uint8_t scan;
	uint8_t shift;
	uint8_t ctrl;
	uint8_t alt;
};

const struct key_mappings ASCII_scancode[] =
{
		{0x0d, 40, 0, 0, 0 }, // Enter key
		{'a', 4, 0, 0, 0 }, {'A', 4, 1, 0, 0 }, {0x01, 4, 0, 1, 0 },
		{'b', 5, 0, 0, 0 }, {'B', 5, 1, 0, 0 }, {0x02, 5, 0, 1, 0 },
		{'c', 6, 0, 0, 0 }, {'C', 6, 1, 0, 0 }, {0x03, 6, 0, 1, 0 },
		{'d', 7, 0, 0, 0 }, {'D', 7, 1, 0, 0 }, {0x04, 7, 0, 1, 0 },
		{'e', 8, 0, 0, 0 }, {'E', 8, 1, 0, 0 }, {0x05, 8, 0, 1, 0 },
		{'f', 9, 0, 0, 0 }, {'F', 9, 1, 0, 0 }, {0x06, 9, 0, 1, 0 },
		{'g', 10, 0, 0, 0 }, {'G', 10, 1, 0, 0 }, {0x07, 10, 0, 1, 0 },
		{'h', 11, 0, 0, 0 }, {'H', 11, 1, 0, 0 },
		{'i', 12, 0, 0, 0 }, {'I', 12, 1, 0, 0 },
		{'j', 13, 0, 0, 0 }, {'J', 13, 1, 0, 0 }, {0x0a, 13, 0, 1, 0 },
		{'k', 14, 0, 0, 0 }, {'K', 14, 1, 0, 0 }, {0x0b, 14, 0, 1, 0 },
		{'l', 15, 0, 0, 0 }, {'L', 15, 1, 0, 0 }, {0x0c, 15, 0, 1, 0 },
		{'m', 16, 0, 0, 0 }, {'M', 16, 1, 0, 0 },
		{'n', 17, 0, 0, 0 }, {'N', 17, 1, 0, 0 }, {0x0e, 17, 0, 1, 0 },
		{'o', 18, 0, 0, 0 }, {'O', 18, 1, 0, 0 }, {0x0f, 18, 0, 1, 0 },
		{'p', 19, 0, 0, 0 }, {'P', 19, 1, 0, 0 }, {0x10, 19, 0, 1, 0 },
		{'q', 20, 0, 0, 0 }, {'Q', 20, 1, 0, 0 }, {0x11, 20, 0, 1, 0 },
		{'r', 21, 0, 0, 0 }, {'R', 21, 1, 0, 0 }, {0x12, 21, 0, 1, 0 },
		{'s', 22, 0, 0, 0 }, {'S', 22, 1, 0, 0 }, {0x13, 22, 0, 1, 0 },
		{'t', 23, 0, 0, 0 }, {'T', 23, 1, 0, 0 }, {0x14, 23, 0, 1, 0 },
		{'u', 24, 0, 0, 0 }, {'U', 24, 1, 0, 0 }, {0x15, 24, 0, 1, 0 },
		{'v', 25, 0, 0, 0 }, {'V', 25, 1, 0, 0 }, {0x16, 25, 0, 1, 0 },
		{'w', 26, 0, 0, 0 }, {'W', 26, 1, 0, 0 }, {0x17, 26, 0, 1, 0 },
		{'x', 27, 0, 0, 0 }, {'X', 27, 1, 0, 0 }, {0x18, 27, 0, 1, 0 },
		{'y', 28, 0, 0, 0 }, {'Y', 28, 1, 0, 0 }, {0x19, 28, 0, 1, 0 },
		{'z', 29, 0, 0, 0 }, {'Z', 29, 1, 0, 0 }, {0x1a, 29, 0, 1, 0 },
		{'1', 30, 0, 0, 0 }, {'!', 30, 1, 0, 0 },
		{'2', 31, 0, 0, 0 }, {'"', 31, 1, 0, 0 },
		{'3', 32, 0, 0, 0 }, {'£', 32, 1, 0, 0 },
		{'4', 33, 0, 0, 0 }, {'$', 33, 1, 0, 0 },
		{'5', 34, 0, 0, 0 }, {'%', 34, 1, 0, 0 },
		{'6', 35, 0, 0, 0 }, {'^', 35, 1, 0, 0 },
		{'7', 36, 0, 0, 0 }, {'&', 36, 1, 0, 0 },
		{'8', 37, 0, 0, 0 }, {'*', 37, 1, 0, 0 },
		{'9', 38, 0, 0, 0 }, {'(', 38, 1, 0, 0 },
		{'0', 39, 0, 0, 0 }, {')', 39, 1, 0, 0 },
		{0x7f, 42, 0, 0, 0 },  // <BS> Backspace CTRL+?
		{0x08, 42, 0, 0, 0 },  // <BS> Backspace CTRL+H
		{0x09, 43, 0, 0, 0 },  // <TAB> Horizontal tab CTRL+I
		{' ', 44, 0, 0, 0 },
		{'-', 45, 0, 0, 0 }, {'_', 45, 1, 0, 0 },
		{'=', 46, 0, 0, 0 }, {'+', 46, 1, 0, 0 },
		{'[', 47, 0, 0, 0 }, {'{', 47, 1, 0, 0 },
		{']', 48, 0, 0, 0 }, {'}', 48, 1, 0, 0 },
		{'#', 50, 0, 0, 0 }, {'~', 50, 1, 0, 0 },
		{';', 51, 0, 0, 0 }, {':', 51, 1, 0, 0 },
		{'\'', 52, 0, 0, 0 }, {'@', 52, 1, 0, 0 },
		{',', 54, 0, 0, 0 }, {'<', 54, 1, 0, 0 },
		{'.', 55, 0, 0, 0 }, {'>', 55, 1, 0, 0 },
		{'/', 56, 0, 0, 0 }, {'?', 56, 1, 0, 0 },
		{'\\', 100, 0, 0, 0 }, {'|', 100, 1, 0, 0 },

		{ 0, 0, 0, 0 }, // end marker
};
//@}

/**
 @name keypress_counter
 @brief Count up ms timer to simulate keypress reports at a set interval.
 */
static volatile int16_t keypress_counter = 0;

/**
 @name UART ring buffer variables.
 @brief Variables required to implement an Rx ring buffer for the UART.
 */
//@{
static uint8_t uart0BufferIn_data[RINGBUFFER_SIZE];
static volatile uint16_t uart0BufferIn_wr_idx = 0;
static volatile uint16_t uart0BufferIn_rd_idx = 0;
static volatile uint16_t uart0BufferIn_avail = RINGBUFFER_SIZE;
//@}

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/** @name tfp_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
void tfp_putc(void* p, char c)
{
	uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}

/** @name my_getc
 *  @details Machine dependent getc function for.
 *  @param p Parameters (machine dependent)
 *  @param c The character to read.
 */
int8_t my_getc(char *c)
{
	int8_t copied = -1;

	CRITICAL_SECTION_BEGIN
	{
		if (uart0BufferIn_avail < RINGBUFFER_SIZE)
		{
			uart0BufferIn_avail++;

			*c = uart0BufferIn_data[uart0BufferIn_rd_idx];

			/* Increment the pointer and wrap around */
			uart0BufferIn_rd_idx++;
			if (uart0BufferIn_rd_idx == RINGBUFFER_SIZE) uart0BufferIn_rd_idx = 0;

			copied = 0;
		}
	}
	CRITICAL_SECTION_END


    /* Report back how many bytes have been copied into the buffer...*/
    return copied;
}

/**
 The Interrupt that allows echoing back over UART0
 */
/**
 The Interrupt which handles asynchronous transmission and reception
 of data into the ring buffer
 */
void uart0ISR()
{
    static uint8_t c;

    /* Receive interrupt... */
    if (uart_is_interrupted(UART0, uart_interrupt_rx))
    {
        /* Read a byte into the Ring Buffer... */
        uart_read(UART0, &c);

        // Do not overwrite data already in the buffer.
        if (uart0BufferIn_avail > 0)
        {
        	uart0BufferIn_avail--;

        	uart0BufferIn_data[uart0BufferIn_wr_idx] = c;

			/* Increment the pointer and wrap around */
			uart0BufferIn_wr_idx++;
			if (uart0BufferIn_wr_idx == RINGBUFFER_SIZE) uart0BufferIn_wr_idx = 0;
        }
    }
}

void ISR_timer(void)
{
	if (timer_is_interrupted(timer_select_a))
	{
		// Countdown for keypress simulation.
		if (keypress_counter > 0)
			keypress_counter--;

    	// Call USB timer handler to action transaction and hub timers.
    	USBH_timer();
	}
}

int8_t keyboard(USBH_AOA_context *ctx)
{
    int8_t status;
    uint8_t state = KEY_ON; // First action is a keypress.
    uint16_t delay = KEY_ON_PERIOD;
	int key;
    uint8_t keyscan;
	uint8_t shift;
	uint8_t ctrl;
	uint8_t alt;
	uint8_t escape;
	char scanmsg[8];
	char msg;

	escape = 0;

	// Endless loop.
	while (1)
	{
		// Timer ISR will count this value down to zero to make a
		// millisecond delay available to us to time keypress
		// simulations.
		if (keypress_counter == 0)
		{
			keypress_counter = delay;

			switch (state)
			{
			case KEY_ON:
				if (my_getc(&msg) != -1)
				{
					// Clear any previous key scan codes.
					keyscan = 0;
					shift = 0;
					ctrl = 0;
					alt = 0;

#ifdef USBH_AOA_UART_KEYS
					// For debugging display character sent.
					if (isgraph(msg))// || isspace(msg))
						tfp_printf("%c", msg);
					else
						tfp_printf("\\x%02x", msg);
#endif // USBH_AOA_UART_KEYS

					// 0x1b is an escape character.
					// This signifies the start of the escape sequence.
					if ((msg == '\x1b') || (escape))
					{
						// Timeout for escape sequence to be received
						// in it's entirety.
						delay = KEY_ON_PERIOD;

						scanmsg[escape++] = msg;
						scanmsg[escape] = '\0';

						// Decode escape code into key scan codes.
						for (key = 0; ESCAPED_scancode[key].scan; key++)
						{
							if (strcmp(ESCAPED_scancode[key].msg, scanmsg) == 0)
							{
								keyscan = ESCAPED_scancode[key].scan;
								shift = ESCAPED_scancode[key].shift;
								ctrl = ESCAPED_scancode[key].ctrl;
								alt = ESCAPED_scancode[key].alt;
								break;
							}
						}
					}
					else
					{
						// Decode normal non-escaped keypresses into
						// key scan codes.
						for (key = 0; ASCII_scancode[key].scan; key++)
						{
							if (ASCII_scancode[key].msg == msg)
							{
								keyscan = ASCII_scancode[key].scan;
								shift = ASCII_scancode[key].shift;
								ctrl = ASCII_scancode[key].ctrl;
								alt = ASCII_scancode[key].alt;
								break;
							}
						}
					}

					// If a valid key scan code has been detected then
					// send it to the Android device.
					if (keyscan)
					{
						delay = KEY_ON_PERIOD;
						state = KEY_OFF;

						report_buffer.arrayKeyboard = keyscan;
						report_buffer.kbdRightShift = shift;
						report_buffer.kbdLeftControl = ctrl;
						report_buffer.kbdLeftAlt = alt;

						status = USBH_AOA_send_hid_data(ctx, 0x0403, 8, (uint8_t *) &report_buffer);
#ifdef USBH_AOA_KEYSCAN_CODES
						tfp_printf("<%d/%x", keyscan, report_buffer.arrayMap);
#endif // USBH_AOA_KEYSCAN_CODES

						escape = 0;
					}
				}
				else
				{
					// If the delay expires without a complete escape
					// sequence being received then send an escape scancode
					// (0x41) if, and only if an escape sequence has not been
					// started. If it has then assume it is unsupported and
					// ignore it.
					if (escape == 1)
					{
						// Send an escape keypress on it's own.
						delay = KEY_ON_PERIOD;
						state = KEY_OFF;

						memset(&report_buffer, 0, 8);
						report_buffer.arrayKeyboard = 41;

						status = USBH_AOA_send_hid_data(ctx, 0x0403, 8, (uint8_t *) &report_buffer);
#ifdef USBH_AOA_KEYSCAN_CODES
						tfp_printf("<ESC");
#endif // USBH_AOA_KEYSCAN_CODES
					}
					escape = 0;
				}
				break;

			case KEY_OFF:
				// Send a zero report which indicates that all keys are not
				// pressed.
				delay = KEY_OFF_PERIOD;
				state = KEY_ON;

				report_buffer.arrayKeyboard = 0x00;
				report_buffer.arrayMap = 0;

				status = USBH_AOA_send_hid_data(ctx, 0x0403, 8, (uint8_t *) &report_buffer);
#ifdef USBH_AOA_KEYSCAN_CODES
				tfp_printf(">");
#endif // USBH_AOA_KEYSCAN_CODES
				break;

			default:
				break;
			}
		}
		else
		{
			delay = 1;
		}

		// Update USBH status and perform callbacks if required.
		status = USBH_process();

	} while (status == USBH_OK);

	tfp_printf("Done %d\r\n", status);

	// Disconnected from host.
	return status;
}

void aoa_testing(USBH_AOA_context *ctx, USBH_device_handle hAOAdev)
{
    int8_t status;
    int i;
    uint16_t usbVid, usbPid;
    USBH_device_info devInfo;
    char *speed[3] = {"low", "full", "high"};
    uint16_t protocol;

	memset(&report_buffer, 0, sizeof(report_buffer));

	// Wait an initial delay for OS to setup keyboard.
	keypress_counter = KEY_STARTUP_TIME;

    // Display some information on the Android accessory device.
	if (USBH_device_get_vid_pid(hAOAdev, &usbVid, &usbPid) == USBH_OK)
	{
		tfp_printf("VID: %04x PID: %04x\r\n", usbVid, usbPid);
	}
	if (USBH_device_get_info(hAOAdev, &devInfo) == USBH_OK)
	{
		tfp_printf("Speed: %d %s \r\n", devInfo.speed, speed[devInfo.speed]);
	    tfp_printf("Address: %d \r\n", devInfo.addr);
	}

	// Display accessory interfaces supported.
	i = USBH_AOA_has_accessory(ctx);
    tfp_printf("Accessory: %s \r\n", i==USBH_AOA_DETECTED?"yes":"no");
	i = USBH_AOA_has_audio(ctx);
    tfp_printf("Audio: %s \r\n", i==USBH_AOA_DETECTED?"yes":"no");
	i = USBH_AOA_has_adb(ctx);
    tfp_printf("Adb: %s \r\n", i==USBH_AOA_DETECTED?"yes":"no");

    // HID support is only available on protocol 2 and above.
    if (USBH_AOA_get_protocol(ctx, &protocol) == USBH_AOA_OK)
    {
    	if (protocol >= USB_AOA_PROTOCOL_2)
    	{
			// Register a HID device for a keyboard.
			status = USBH_AOA_register_hid(ctx, 0x0403, sizeof(hid_report_descriptor_keyboard));

			if (status == USBH_AOA_OK)
			{
				// Send the report descriptor in one packet.
				status = USBH_AOA_set_hid_report_descriptor(ctx, 0x0403, 0,
						sizeof(hid_report_descriptor_keyboard),
						(uint8_t *)hid_report_descriptor_keyboard);
				if (status == USBH_AOA_OK)
				{
				    tfp_printf("Keyboard:\r\n");
					keyboard(ctx);
				}
			}
		}
    }
}

// Process for connecting to AOA devices is:
// 1) Check AOA protocol is valid. This means that the special vendor SETUP
//    command works and the return value is non-zero and matches a protocol
//    version supported by the driver.
// 2) Send string descriptors to the AOA device using the vendor SETUP
//    commands. Then send a start accessory device vendor SETUP command.
// 3) The device re-enumerates by doing a device reset.
// 4) The host re-enumerates the device and then decodes support for the
//    various accessory modes from the USB PID.
// 5) It is now ready for use.

int8_t hub_scan_for_aoa(USBH_device_handle hDev, int level, int retry)
{
    uint8_t status;
    USBH_AOA_context ctx = {0};
    USBH_AOA_descriptors dsc_no_accessory =
    {
    		NULL, // Blank - no app associated with this accessory.
    		NULL, // Blank - no app associated with this accessory.
    		"FT900 HID Accessory Test",
    		"1.0",
    		"http://www.ftdichip.com",
    		"FT900Accessory1",
    };
    USBH_AOA_descriptors dsc_alternate =
    {
    		"FTDI", // Manufacturer
    		"Keyboard", // Dummy application
    		"FT900 HID Accessory Test (device does not support app-less configuration)",
    		"1.0",
    		"http://www.ftdichip.com",
    		"FT900Accessory1",
    };
    // Optionally enable audio mode if protocol supports it.
    uint16_t audio_on = USB_AOA_SET_AUDIO_MODE_16BIT_PCM_2CH_44100KHz;
    uint16_t audio_off = USB_AOA_SET_AUDIO_MODE_NONE;

    (void)audio_on; (void)audio_off;

	tfp_printf("!parent %d \n!port %d \n",
			ctx.parentPort,
			ctx.hDevParent);
    // Initialise the device. Try to change it into an accessory after
    // checking the protocol, setting the descriptors and starting the
    // accessory.
    if (!retry)
    {
        tfp_printf("Init AOA\r\n");

    	status = USBH_AOA_init(&ctx, hDev, &dsc_no_accessory, audio_off);
    }
    else
    {
        tfp_printf("Init AOA (alternate descriptors)\r\n");

    	status = USBH_AOA_init(&ctx, hDev, &dsc_alternate, audio_off);
    }

    if (status == USBH_AOA_STARTED)
    {
    	// The device is an Android accessory device which is in it's normal
    	// mode of operation. The USBH_AOA_init call has started the
    	// accessory and it will do a device reset and re-enumerate. We go
    	// back to the usbh_testing function to wait for enumeration to
    	// complete.

    	// A device in Android accessory mode is connected. We can now
    	// attach to the device before calling the testing function.
        tfp_printf("Attaching AOA\r\n");
        if (USBH_AOA_attach(&ctx) == USBH_AOA_OK)
        {
        	// AOA testing function(s).
            tfp_printf("Testing AOA\r\n");
        	aoa_testing(&ctx, hDev);
        }
        else
        {
            tfp_printf("Could not attach AOA\r\n");
        }
    }

    return status;
}

int8_t usbh_testing(void)
{
    int8_t status;
    int retry = 0;
    USBH_device_handle hRootDev;
    USBH_STATE connect;
	/* 100 us delay for compatibility issues during USBH_AOA_init in some Android devices*/
    USBH_ctx ctx = { NULL, 100};

    USBH_initialise(&ctx);

    while (1)
    {
    	// Check if a device is already connected to the USB Host.
		USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		if (connect == USBH_STATE_NOTCONNECTED)
		{
			tfp_printf("\r\nPlease plug in a USB Device\r\n");

	    	// Wait for a device (any device) to connect to the USB Host.
			do
			{
				// We must process USB events while waiting to connect.
				status = USBH_process();
				USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
			} while (connect == USBH_STATE_NOTCONNECTED);
		}
		// We now have a device connected - it may not be enumerated though.
		tfp_printf("\r\nUSB Device Detected\r\n");

		do
		{
			// Wait for enumeration to complete.
			// This may be when the device is enumerating as an Android accessory
			// or when it is in it's normal mode of operation.
			do{
				status = USBH_process();
				USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
			} while (connect < USBH_STATE_ENUMERATED);

			// Enumeration may not complete depending on available resources.
			// Report this but the accessory device may still work.
			if (connect == USBH_STATE_ENUMERATED)
			{
				tfp_printf("USB Devices Enumerated\r\n");
			}
			else
			{
				tfp_printf("USB Devices Partially Enumerated\r\n");
			}

			// Get the first device (device on root hub)
			status = USBH_get_device_list(USBH_ROOT_HUB_HANDLE, &hRootDev);
			if (status != USBH_OK)
			{
				// Report the error code.
				tfp_printf("Device list not found. Error %d\r\n", status);
			}
			else
			{
				// Scan the device for an accessory then begin actual testing.
				status = hub_scan_for_aoa(hRootDev, 1, retry);
				retry = 0;
				if (status == USBH_AOA_STARTED)
				{
					retry = 1;
				}
			}
		} while (status > USBH_OK);

		// The testing has finished. The device may have been removed or the test
		// exited normally. Ask the user to remove the device and wait for it to
		// be removed.
		tfp_printf("\r\nPlease remove the USB Device\r\n");
		// Detect usb device disconnect
		do
		{
			status = USBH_process();
			USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		} while (connect != USBH_STATE_NOTCONNECTED);

		// Go back and wait for a device to be connected.
    }

    return 0;
}

/* FUNCTIONS ***********************************************************************/

int main(void)
{
	/* Check for a USB device connection and initiate a DFU firmware download or
	   upload operation. This will timeout and return control here if no host PC
	   program contacts the device's DFU interace. USB device mode is disabled
	   before returning.
	*/
	STARTUP_DFU();

	/* Enable the UART Device... */
    sys_enable(sys_device_uart0);

	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
    uart_open(UART0,                    /* Device */
              1,                        /* Prescaler = 1 */
              UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */

    /* Print out a welcome message... */
    uart_puts(UART0,
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
    	);

    /* Enable tfp_printf() functionality... */
    init_printf(UART0, tfp_putc);

	sys_enable(sys_device_timer_wdt);

    interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, ISR_timer);
    interrupt_enable_globally();

    /* Timer A = 1ms */
	timer_prescaler(timer_select_a, 1000);
    timer_init(timer_select_a, 1000, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
    timer_enable_interrupt(timer_select_a);
    timer_start(timer_select_a);

    tfp_printf("Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to USBH AOA HID Tester Example 1... \r\n"
        "\r\n"
        "Bridges UART input to a HID keyboard on an Android device.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

	/* Attach the interrupt so it can be called... */
	interrupt_attach(interrupt_uart0, (uint8_t) interrupt_uart0, uart0ISR);
    uart_disable_interrupt(UART0, uart_interrupt_tx);
	/* Enable the UART to fire interrupts when receiving buffer... */
	uart_enable_interrupt(UART0, uart_interrupt_rx);
	/* Enable interrupts to be fired... */
	uart_enable_interrupts_globally(UART0);

    usbh_testing();

    interrupt_detach(interrupt_timers);
    interrupt_disable_globally();
    sys_disable(sys_device_timer_wdt);

    // Wait forever...
    for (;;);

}
