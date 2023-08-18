/**
 @file
 @brief
 UART Example 4

 This example program will implement transmit and receive functions that will
 make the UART completely interrupt driven with FIFO mode and flow control enabled.
 */
/*
 * ============================================================================
 * History
 * =======
 * 2018-04-09 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek ") subject to the licence terms set out
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

#include <stdint.h>
#include <ft900.h>

/* MACROS ***********************************************************************/

//#define UART_MODE_550
//#define UART_MODE_650
//#define UART_MODE_750
#define UART_MODE_950

/**
 * FIFO size depends on uart mode
 * -----------------------
 * |mode	 | FIFO SIZE |
 * |---------|-----------|
 * |450		 |  disabled |
 * |550      |   16      |
 * |550_ext  |   128     |
 * |650		 |   128     |
 * |750		 |   128     |
 * |950		 |   128     |
 * -----------------------
 */
#if defined(UART_MODE_550)
#define FIFO_SIZE 16
#else
#define FIFO_SIZE 128
#endif
/**
 @brief Change this value in order to modify the size of the UART data buffer
 */
#define BUF_SIZE (FIFO_SIZE * 2)

#if defined(UART_MODE_550)
#define UART_MODE uart_mode_16550
#elif defined(UART_MODE_650)
#define UART_MODE uart_mode_16650
#elif defined(UART_MODE_750)
#define UART_MODE uart_mode_16750
#elif defined(UART_MODE_950)
#define UART_MODE uart_mode_16950
#else
#error "Please enable one of the UART mode"
#endif


#if defined(UART_MODE_550)
#define RX_LEVEL	uart_fifo_trigger_level_14
#define TX_LEVEL	uart_fifo_trigger_level_1
#elif defined(UART_MODE_650)
/**In 650 mode RX_TRIGGER_LEVEL is combination of lower trigger for flow control(112)
 *  and rx interrupt trigger and upper trigger for flow control(120)*/
#define RX_LEVEL	uart_fifo_trigger_level_112_120
/*Transmit interrupt trigger level*/
#define TX_LEVEL	uart_fifo_trigger_level_112
#elif defined(UART_MODE_750)
#define RX_LEVEL	uart_fifo_trigger_level_112
#define TX_LEVEL	uart_fifo_trigger_level_1
#elif defined(UART_MODE_950)
/*Rx and Tx interrupt trigger levels for 950 mode can be any value between 0-127*/
#define RX_LEVEL	(127)
#define TX_LEVEL	(0)
#endif

/*Receiver interrupt trigger level*/
#define RX_TRIGGER_LEVEL	RX_LEVEL
/*Transmitter interrupt trigger level*/
#define TX_TRIGGER_LEVEL	TX_LEVEL

/**
 @name Flow control levels.
 @brief Flow control high and flow control low levels.
 */
//@{
#if defined(UART_MODE_950)
#define FCH	112
#define FCL	64
#else
#define FCH	0
#define FCL	0
#endif
//@}

/*XON and XOFF bytes for flow control*/
#define XON		0x11
#define XOFF	0x13

/* LOCAL VARIABLES *****************************************************************/
/**
 @name UART data buffer variable
 */
uint8_t BUF[BUF_SIZE];

/**
 @name UART data buffer index
 */
uint32_t buf_index = 0;

/* LOCAL FUNCTIONS / INLINES *******************************************************/
void setup(void);
void uart0ISR(void);

int main(void) {
	setup();
	while(1);
	return 0;
}

void setup() {
	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
	/* Set UART0 GPIO functions to UART0_TXD and UART0_RXD... */
#if defined(__FT900__)
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
	gpio_function(50, pad_uart0_rts); /* UART0 RTS */
	gpio_function(51, pad_uart0_cts); /* UART0 CTS */
#elif defined(__FT930__)
	gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
	gpio_function(23, pad_uart0_txd); /* UART0 TXD */
	gpio_function(24, pad_uart0_rts); /* UART0 RTS */
	gpio_function(25, pad_uart0_cts); /* UART0 CTS */
#endif

	/*Reset UART registers to default value*/
	uart_soft_reset(UART0);

	uart_open(UART0, /* Device */
			1, /* Prescaler = 1 */
			UART_DIVIDER_19200_BAUD, /* Divider = 1302 */
			uart_data_bits_8, /* No. Data Bits */
			uart_parity_none, /* Parity */
			uart_stop_bits_1); /* No. Stop Bits */

	/*Enable mode*/
	uart_mode(UART0, UART_MODE);
	/*Set trigger levels and flow controls*/
	uart_set_trigger_level(UART0, UART_MODE, RX_TRIGGER_LEVEL, TX_TRIGGER_LEVEL, FCH, FCL);
	/*Enable flow control*/
	uart_set_flow_control(UART0, uart_flow_rts_cts, XON, XOFF);

	/* Print out a welcome message... */
	uart_puts(UART0,"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
			"Copyright (C) Bridgetek Pte Ltd \r\n"
			"--------------------------------------------------------------------------- \r\n"
			"Welcome to UART Example 4... \r\n"
			"\r\n"
			"Any character typed here will be echoed back on the same serial port  \r\n"
			"via an interrupt. This example uses UART FIFO mode with auto flow control.\r\n"
			"-------------------------------------------------------------------------- \r\n");

	/* Attach the interrupt so it can be called... */
	interrupt_attach(interrupt_uart0, (uint8_t) interrupt_uart0, uart0ISR);
	/* Enable the UART to fire interrupts when receiving data... */
	uart_enable_interrupt(UART0, uart_interrupt_rx);
	/* Enable the UART to fire interrupts when transmitting data... */
	uart_enable_interrupt(UART0, uart_interrupt_tx);
	/* Enable interrupts to be fired... */
	uart_enable_interrupts_globally(UART0);
	interrupt_enable_globally();
}

void uart0ISR() {
	static volatile uint32_t ptr = 0;
	volatile uint32_t rxbytes, txbytes, space_available;

	/* Transmit interrupt... */
	if (uart_is_interrupted(UART0, uart_interrupt_tx)) {
		/*Get available Tx FIFO space to fill in */
		space_available = FIFO_SIZE - uart_get_tx_fifo_level(UART0);
		txbytes = space_available < (buf_index - ptr) ?	space_available : (buf_index - ptr);

		__asm__("streamout.b %0,%1,%2":: "r"(&(UART0->RHR_THR_DLL)), "r"(&BUF[ptr]), "r"(txbytes));

		ptr += txbytes;
		if (ptr == buf_index) {
			buf_index = 0;
			ptr = 0;
			uart_disable_interrupt(UART0, uart_interrupt_tx);
		}
	}

	/* Receive interrupt... */
	if (uart_is_interrupted(UART0, uart_interrupt_rx) || uart_is_interrupted(UART0, uart_interrupt_rx_time_out)) {
		rxbytes = uart_get_rx_fifo_level(UART0);
		space_available = BUF_SIZE - buf_index;
		rxbytes = rxbytes < space_available ? rxbytes : space_available;

		__asm__("streamin.b %0,%1,%2"::"r"(&BUF[buf_index]), "r"(&(UART0->RHR_THR_DLL)), "r"(rxbytes));

		buf_index += rxbytes;
		if (rxbytes) {
			uart_enable_interrupt(UART0, uart_interrupt_tx);
			rxbytes = 0;
		}
	}
}
