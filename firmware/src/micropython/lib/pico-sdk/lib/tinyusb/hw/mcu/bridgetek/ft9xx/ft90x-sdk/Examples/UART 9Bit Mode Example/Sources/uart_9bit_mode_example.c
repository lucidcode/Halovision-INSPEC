/**
 @file
 @brief
 UART 9 bit mode example

 This example program implements 9-bit mode of UART functionality
 with 950 FIFO mode.
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
#include <string.h>

/* MACROS ***********************************************************************/


#define NONE				0x00
/* 9-bit configure address*/
#define ADDR_1				0x1F1
#define ADDR_2 				0x1F2
#define ADDR_3 				0x1F3
#define ADDR_4 				0x1F4

/*Other address*/
#define OTHER_ADDR_1		0x1DE
#define OTHER_ADDR_2		0x1AD
#define OTHER_ADDR_3		0x1BC

/* LOCAL VARIABLES *****************************************************************/

static const char* device_1_text =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam dictum nunc id ullamcorper consectetur."
		"Vestibulum tristique egestas ligula a rutrum. In eu blandit elit, eu ultricies risus. Vivamus vitae dui"
		"ut purus vestibulum blandit. Orci varius natoque penatibus et magnis dis parturient montes, nascetur "
		"ridiculus mus. Pellentesque quis aliquam metus. Suspendisse sit amet bibendum felis, at iaculis elit. "
		"Suspendisse quis enim mauris. Morbi a accumsan dolor. Aliquam dignissim, lectus quis aliquet consequat,"
		"velit tortor volutpat tortor, a vehicula risus sem non ipsum. Aenean et iaculis magna, quis placerat elit.\r\n\n";


static const char* device_2_text =
		"Vivamus laoreet tempor lorem sit amet lobortis. Proin in mauris rutrum, sagittis erat ac, sodales enim."
		"In scelerisque scelerisque enim id cursus. Morbi aliquam leo eget ornare semper. Aliquam vitae diam ut "
		"dolor interdum tincidunt non eget velit. Ut eros enim, pellentesque vitae est a, feugiat congue massa. Nulla facilisi.\r\n\n";

static const char* device_3_text =
		"Suspendisse interdum ligula in convallis posuere. Pellentesque quis nisl turpis. In congue, enim at "
		"ultricies luctus, ex purus suscipit risus, quis tristique elit massa et nisl. Cras vehicula neque nec "
		"lacus tincidunt malesuada. Mauris aliquam, lectus a dictum hendrerit, nunc dui interdum metus, a "
		"finibus massa ipsum in sapien. Nam sit amet sem faucibus est eleifend vehicula. Nulla sapien justo, "
		"aliquam sed ullamcorper ut, facilisis non leo. Suspendisse elementum augue nunc, sit amet vulputate "
		"turpis consequat sit amet. Aenean quis lacinia sem.\r\n\n";

static const char* device_4_text =
		"In et libero sodales, condimentum lorem et, varius ipsum. Nulla nec cursus nibh, et suscipit enim. "
		"Nunc imperdiet, ligula id aliquam eleifend, turpis est gravida velit, et elementum mi metus vel est."
		"Sed cursus eu elit ut lobortis. Suspendisse potenti. Fusce nulla ante, aliquet nec consectetur id, "
		"aliquet ut leo. Nullam at dui consectetur, rutrum dolor feugiat, maximus purus. Morbi sapien lacus, "
		"aliquam convallis nisi ac, posuere malesuada enim. Ut pretium viverra dui, id vulputate mi blandit "
		"vitae. Nunc dignissim, dui ut faucibus tempus, turpis ipsum pharetra metus, ac porttitor risus orci "
		"nec nulla. Suspendisse est mauris, finibus sed fringilla sed, pharetra nec odio. Aliquam erat volutpat. "
		"Fusce luctus quis augue ut egestas. Aliquam non sem facilisis urna mattis feugiat vitae nec augue.\r\n\n";

static const char* other_device_text =
		"Ut vulputate bibendum arcu, id lobortis erat laoreet eu. Ut tristique in nunc eu molestie. Etiam gravida "
		"nisl id egestas pharetra. Praesent ac imperdiet quam. Mauris in placerat arcu. Nulla eget augue eu magna "
		"dapibus commodo sit amet vitae justo. Suspendisse sed tempor ex. Aliquam at magna vel neque volutpat "
		"tincidunt vitae in elit. In pharetra lectus vitae consectetur egestas. Mauris aliquet odio ligula, "
		"non condimentum ex fermentum convallis. Integer maximus ligula a velit mattis, eget congue lacus "
		"fermentum. Aenean hendrerit scelerisque lectus. Praesent magna lacus, consectetur at viverra tempus, "
		"consequat ut sapien. Proin ac arcu in odio consectetur scelerisque. Integer lobortis diam mollis, "
		"tristique massa ut, fermentum enim.\r\n\n";

/* LOCAL FUNCTIONS / INLINES *******************************************************/
void setup(void);
void uart0ISR(void);

/** @brief Send out data from UART
 *  @details Transmit specified data from UART.
 *  @param dev The device to use
 *  @param data data buffer to transmit.
 *  @param length length of the buffer to transmit
 */
static void send_data(ft900_uart_regs_t *dev, const char *data, uint32_t length)
{
	uart_writen(dev, (uint8_t*)data, length);
}

/** @brief print the string on UART0
 *  @details send out the specified string on UART0.
 *  @param string string to send
 */
void print(char *string)
{
	uart_puts(UART0,string);
}

void uart0ISR()
{

	static volatile uint8_t valid;

	/*Interrupt for 9th bit address is set*/
	if(uart_is_interrupted(UART1, uart_interrupt_9th_bit))
	{
		/* !!! read the 9-bit address, only then data could be read !!! */
		uint8_t tmp = UART1->RHR_THR_DLL;
		valid = 0;
	}

	 /*configured 9 bit address is detected*/
	if(uart_is_interrupted(UART1, uart_interrupt_xon_xoff))
	{
		valid = 1;
	}

	 /*Interrupt for data reception*/
	if((uart_is_interrupted(UART1, uart_interrupt_rx)) || (uart_is_interrupted(UART1, uart_interrupt_rx_time_out)))
	{
		 if(valid) // for configured address
		 {
			uint8_t rxbytes = uart_get_rx_fifo_level(UART1);
			while(rxbytes--)
			{
				UART0->RHR_THR_DLL = UART1->RHR_THR_DLL;
			}
		 }
		 else
		 {
			/*Flush FIFO for other addresses*/
			uart_flush_rx_fifo(UART1);
		 }
	 }
}

void setup()
{
	sys_reset_all();
	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
	sys_enable(sys_device_uart1);

	/* Set UART0 GPIO functions to UART0_TXD and UART0_RXD... */
	#if defined(__FT900__)
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
	gpio_function(53, pad_uart1_rxd); /* UART1 RXD */
	gpio_function(52, pad_uart1_txd); /* UART1 TXD */
#elif defined(__FT930__)
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
	gpio_function(26, pad_uart1_rxd); /* UART1 RXD */
	gpio_function(27, pad_uart1_txd); /* UART1 TXD */
#endif

	/*Reset UART registers to default value*/
	uart_soft_reset(UART0);
	uart_soft_reset(UART1);

	/*Enable 950 mode*/
	uart_mode(UART0, uart_mode_16950);
	uart_mode(UART1, uart_mode_16950);

	uart_open(UART0, /* Device */
			1, /* Prescaler = 1 */
			UART_DIVIDER_19200_BAUD, /* Divider = 1302 */
			uart_data_bits_8, /* No. Data Bits */
			uart_parity_none, /* Parity */
			uart_stop_bits_1); /* No. Stop Bits */

	uart_open(UART1, /* Device */
			1, /* Prescaler = 1 */
			UART_DIVIDER_19200_BAUD, /* Divider = 1302 */
			uart_data_bits_9, /* 9-bit address mode */
			uart_parity_none, /* Parity */
			uart_stop_bits_1); /* No. Stop Bits */

	/*configure 9-bit address for address detection*/
	uart_configure_9bit_address(UART1, ADDR_1, ADDR_2, ADDR_3, ADDR_4); /* the 5th function param is not applicable for FT900 Rev B */
	//uart_configure_9bit_address(UART1, ADDR_1, 0 , 0 ,0);
	//uart_configure_9bit_address(UART1, ADDR_1, 0 , ADDR_3 ,0);
	//uart_configure_9bit_address(UART1, 0, ADDR_2 , 0 ,0);
	//uart_configure_9bit_address(UART1, 0, 0 , ADDR_3 ,0);
	//uart_configure_9bit_address(UART1, 0, 0 , 0,ADDR_4);  /* the 5th function param is applicable only in FT900 Rev C */

	/* Print out a welcome message... */
	uart_puts(UART0,"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
			"Copyright (C) Bridgetek Pte Ltd \r\n"
			"--------------------------------------------------------------------------- \r\n"
			"Welcome to UART 9-bit mode Example... \r\n"
			"\r\n"
			" UART1 works in 9-bit mode. When Rx and Tx pin of UART1 are externally \r\n"
			" connected to make a loop. UART1 receives the data from its transmitter \r\n"
            " and the received data is displayed in the debug UART.                      \r\n"
			"-------------------------------------------------------------------------- \r\n\n");

	/* Attach the interrupt so it can be called... */
	interrupt_attach(interrupt_uart1, (uint8_t) interrupt_uart1, uart0ISR);
	/* Enable the UART to fire interrupts when receiving data... */
	uart_enable_interrupt(UART1, uart_interrupt_rx);

	/*Enable interrupt for 9th bit address detection*/
	uart_enable_interrupt(UART1, uart_interrupt_9th_bit);

	/*Enable interrupt for address detection for the configured address
	  xon_xoff is multiplexed with 9-bit special character detection*/
	uart_enable_interrupt(UART1, uart_interrupt_xon_xoff);

	/* Enable interrupts to be fired... */
	uart_enable_interrupts_globally(UART1);
	interrupt_enable_globally();
}

/* FUNCTIONS ***********************************************************************/

int main(void)
{
	setup();

	/*send configured address 1 and data*/
	uart_send_9bit_address(UART1, ADDR_1);
	send_data(UART1, device_1_text, strlen(device_1_text));

	/*send configured address 2 and data*/
	uart_send_9bit_address(UART1, ADDR_2);
	send_data(UART1,device_2_text,strlen(device_2_text));

	/*send address which isn't configured. UART1 should discard this data*/
	uart_send_9bit_address(UART1, OTHER_ADDR_1);
	send_data(UART1, other_device_text, strlen(other_device_text));

	/*send configured address 3 and data*/
	uart_send_9bit_address(UART1, ADDR_3);
	send_data(UART1, device_3_text, strlen(device_3_text));

	/*send address which isn't configured. UART1 should discard this data*/
	uart_send_9bit_address(UART1, OTHER_ADDR_2);
	send_data(UART1, other_device_text, strlen(other_device_text));

	/*send configured address 4 and data*/
	uart_send_9bit_address(UART1, ADDR_4);
	send_data(UART1, device_4_text, strlen(device_4_text));

	/*send address which isn't configured. UART1 should discard this data*/
	uart_send_9bit_address(UART1, OTHER_ADDR_3);
	send_data(UART1, other_device_text, strlen(other_device_text));

	// Wait forever...
	for (;;);

	return 0;
}
