/**
  @file d2xx_uart_bridge.c
  @brief
  UART to USB D2XX BRIDGE.

  Any data on the first 4 of D2XX USB COM ports are bridged to the four 
  UART peripherals and viceversa. UART peripherals can be looped back to 
  one another or to themselves, to observe the characters sent on the D2XX USB
  COM ports being received on them.
 */
/*
 * ============================================================================
 * History
 * =======
 * UKB 20 Jan 2017 : Created
 *
 * Copyright (C) Bridgetek Pte Ltd
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

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "ftd2xx_api.h"
#include <ft900_startup_dfu.h>
#include <ft900.h>


/* UART support for printf output. */
#define DEBUG

#ifdef DEBUG
#include "tinyprintf.h"
#define dbg(s,...)	tfp_printf ((s), ##__VA_ARGS__)
#else
#define dbg(s,...)
#endif

/* MACROS ***********************************************************************/
#if defined(__FT930__)
/// FTDI Product ID for FT930 variants
#define USB_PID_FTDI_FT930_1                      0x6034
#define USB_PID_FTDI_FT930_2                      0x6035
#define USB_PID_FTDI_FT930_3                      0x6036
#define USB_PID_FTDI_FT930_4                      0x6037
#define USB_PID_FTDI_FT930_5                      0x6038
#define USB_PID_FTDI_FT930_6                      0x6039
#define USB_PID_FTDI_FT930_7                      0x603A
#else
/// FTDI Product ID for FT900 variants
#define USB_PID_FTDI_FT900_1                      0x6031
#define USB_PID_FTDI_FT900_2                      0x6032
#define USB_PID_FTDI_FT900_3                      0x6033
#endif

#define GPIO_REMOTE_WAKEUP

#ifdef GPIO_REMOTE_WAKEUP
#if defined(__FT930__)
//GPIO12 in FT930Q mini module board
#define GPIO_PIN  12
#else
#define GPIO_PIN  18
#endif
#endif
#define TEST_WAKEUP_SOURCE_RESET 				(1 << 0)
#define TEST_WAKEUP_SOURCE_RESUME 				(1 << 1)
#define TEST_WAKEUP_SOURCE_GPIO 				(1 << 2)
#define TEST_WAKEUP_SOURCE_READY				(1 << 3)
#define TEST_WAKEUP_SOURCE_CONNECT				(1 << 4)
#define TEST_WAKEUP_SOURCE_DISCONNECT			(1 << 5)
#define TEST_WAKEUP_SOURCE_OTHERS				(1 << 6)
#define TEST_WAKEUP_SOURCE_SLAVE				(1 << 7)

#define DEVICE_INACTIVITY				10000 //10 second timeout
/*Minimum 10 seconds needed or would interfere in hibernate/wakeup process in bus powered mode */

#if defined(__FT930__)
#define MAX_UARTS				4
#else
#define MAX_UARTS				2
#endif
/* GLOBAL VARIABLES ****************************************************************/

extern TD2XX_DeviceConfiguration __pD2XXDefaultConfiguration[]; // pointer to D2XX Config in flash
// The test config (in Flash and then PM) is copied to the RAM structure D2XXTEST_FlashConfig on startup
__attribute__ ((aligned (4))) TD2XX_DeviceConfiguration D2XXTEST_UserD2xxConfig = {0};

/* LOCAL VARIABLES *****************************************************************/
#ifdef DEBUG
static char *D2XXTest_EventStrings[D2XX_EVT_MAX_CODE] = {	"SUSPEND",    	/**< SUSPEND EVENT from USB Host */
																	"RESUME",	 	/**< RESUME EVENT from USB Host */
																	"BUS_RESET",	 		/**< USB Bus Reset */
																	"READY",	 		/**< D2XX enters Ready state where READ/WRITE requests are processed*/
																	"UNREADY",	 	/**< D2XX exits Ready state*/
																	"DETACH",		/**< DFU DETACH Class command from DFU application */
																	"TESTMODE",	 	/**< D2XX enters Test Mode. Exit is via power cycle*/
																	"INTF_RESET"	/**< Interface RESET Vendor Command from D2XX Application */
																};
#endif
uint8_t D2XXTEST__DfuDetach = 0;
uint8_t D2XXTEST__Ready = 0;
volatile uint8_t SetRemoteWakeup = 0;
#if defined(__FT930__)
extern volatile uint8_t RemoteWakeupEnable;  // used in d2xx_api_master.c
#else
volatile uint8_t RemoteWakeupEnable = 0;
#endif
volatile uint8_t D2XXTEST__Suspend = 0;
volatile uint8_t D2XXTEST__Sleep = 0;
volatile uint8_t D2XXTEST__Wakeup = 0;
volatile uint8_t wkupSource = 0;
volatile uint8_t D2XXTEST__Testmode = 0;

#if 0
volatile int device_inactive_counter = 0;
volatile uint8_t check_device_status = 0;
#endif
/**
 @brief Change this value in order to modify the size of the Tx and Rx ring buffers
  used to implement UART buffering.
 */
#define RINGBUFFER_SIZE  (3*512)

/**
 @name UART ring buffer variables.
 @brief Variables required to implement an Rx and Tx ring buffer for the UART.
 */
//@{
uint8_t uartBufferIn_data[MAX_UARTS][RINGBUFFER_SIZE];
volatile uint16_t uartBufferIn_wr_idx[MAX_UARTS] = {0};
volatile uint16_t uartBufferIn_rd_idx[MAX_UARTS] = {0};
volatile uint16_t uartBufferIn_avail[MAX_UARTS] = {RINGBUFFER_SIZE,RINGBUFFER_SIZE
#if defined(__FT930__)
		,RINGBUFFER_SIZE, RINGBUFFER_SIZE
#endif
		};
uint8_t uartBufferOut_data[MAX_UARTS][RINGBUFFER_SIZE];
volatile uint16_t uartBufferOut_wr_idx[MAX_UARTS] = {0};
volatile uint16_t uartBufferOut_rd_idx[MAX_UARTS] = {0};
volatile uint16_t uartBufferOut_avail[MAX_UARTS] = {RINGBUFFER_SIZE,RINGBUFFER_SIZE
#if defined(__FT930__)
		,RINGBUFFER_SIZE, RINGBUFFER_SIZE
#endif
		};
//@}

/* variable to store the number of uarts to be initialized for the bridging */
int no_of_uarts = MAX_UARTS;
/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);
void debug_uart_init(void);
void tfp_putc(void* p, char c);
void d2xx_callback(ED2XX_EventCode  eventID, void *ref, void* param1, void* param2);
void power_management_ISR(void);
#ifdef GPIO_REMOTE_WAKEUP
void gpio_ISR();
#endif
void timer_ISR();
#if 0
uint16_t xorChecksum(const char str[], uint16_t length);
#endif
size_t uartTx(int uart_idx, uint8_t *buffer, size_t len);
size_t uartRx(int uart_idx, uint8_t *buffer, size_t len);
void d2xx_uart_bridge(void);
void uart_ISR(void);

#undef BDFU  /* Undefine for C-DFU */

#ifdef BDFU
extern char __PMSIZE;
typedef void (*__dfumain_t)(unsigned int, unsigned int) __attribute__((noreturn));
#define GO_BDFU(VID, PID, REL) ((__dfumain_t)(&__PMSIZE - 12))( (PID) << 16 | (VID), REL);
#endif


/**
 @struct uart_config_structure_t
 @brief various configurations for
 initializing the UARTs for bridging.
 **/
typedef struct uart_config_structure_t
{
	ft900_uart_regs_t *uart_id;
	uint8_t tx_pin_num;
	pad_func_t tx_pin_func;
	uint8_t rx_pin_num;
	pad_func_t rx_pin_func;
	uint32_t baudrate;
	interrupt_t interrupt_num;
	sys_device_t sys_uart_id;
	/*void (*uart_ISR)(void);*/
} uart_config_structure_t;

uart_config_structure_t uart_configuration[MAX_UARTS] = {
#if defined(__FT930__)
		{UART0, 23, pad_uart0_txd, 22, pad_uart0_rxd, UART_DIVIDER_19200_BAUD, interrupt_uart0, sys_device_uart0/*, uart0ISR*/}
		,{UART1, 27, pad_uart1_txd, 26, pad_uart1_rxd, UART_DIVIDER_19200_BAUD, interrupt_uart1, sys_device_uart1/*, uart1ISR*/}
#else
		{UART0, 48, pad_uart0_txd, 49, pad_uart0_rxd, UART_DIVIDER_19200_BAUD, interrupt_uart0, sys_device_uart0/*, uart0ISR*/}
		,{UART1, 52, pad_uart1_txd, 53, pad_uart1_rxd, UART_DIVIDER_19200_BAUD, interrupt_uart1, sys_device_uart1/*, uart1ISR*/}
#endif
#if defined(__FT930__)
		,{UART2, 15, pad_uart2_txd, 14, pad_uart2_rxd, UART_DIVIDER_19200_BAUD, interrupt_uart2, sys_device_uart2/*, uart2ISR*/}
		,{UART3, 19, pad_uart3_txd, 18, pad_uart3_rxd, UART_DIVIDER_19200_BAUD, interrupt_uart3, sys_device_uart3/*, uart3ISR*/}
#endif
};

/* FUNCTIONS ***********************************************************************/
int main(void)
{

	/* Setup UART and D2XX */
    setup();

    for(;;){

#if defined(__FT900__)
    	D2XX_Process();
#endif

    	//Test mode
		if (D2XXTEST__Testmode)
		{
			//interrupt_disable_globally();
			for (;;);
			// wait for power cycle after testmode
		}

		// UART to D2XX Bridge application
		if (D2XXTEST__Ready)
		{
			d2xx_uart_bridge();
		}

		//DFU mode
		if (D2XXTEST__DfuDetach)
		{
			interrupt_disable_globally();
			/* DFU detach request had come. Exit D2XX */
			D2XX_Exit();

#if defined(__FT930__)
		    *(SLAVECPU) &= ~(MASK_SLAVE_CPU_CTRL_D2XX_MODE);    // turn-off D2XX_mode
		    *(SLAVECPU) |= (MASK_SLAVE_CPU_CTRL_SLV_RESET);  // assert bit to keep slave CPU in reset
#endif

			/* Take the system to DFU Mode. Provide timeout in ms to
			 * wait for re-enumeration with the host
			 * */
#ifndef BDFU
			STARTUP_DFU(0);
#else
			tfp_printf("Switching to DFU");
			if (USBD_DFU_is_runtime()) /* to ward off linkage issue */
			{
				GO_BDFU(0x0403, 0x0FDE, 0x2300); // use default VID, PID and REL
			}
#endif
			break;
		}

		/* System power down handling */
		if (D2XXTEST__Suspend)
		{

			wkupSource = 0;
			dbg("Enter sleep...\r\n");
#ifdef GPIO_REMOTE_WAKEUP
			//Configuring GPIO pin to wakeup
			SetRemoteWakeup = 0;
			if (RemoteWakeupEnable)
			{
				/* Set up the pin */
				gpio_dir(GPIO_PIN, pad_dir_input);
				gpio_pull(GPIO_PIN, pad_pull_pullup);

				/* Attach an interrupt */
				interrupt_attach(interrupt_gpio, (uint8_t)interrupt_gpio, gpio_ISR);
				gpio_interrupt_enable(GPIO_PIN, gpio_int_edge_falling);
			}
#endif

			//Enable d2xx hw engine wakeup events
#if defined(__FT930__)
			SYS->PMCFG_L &= ~(MASK_SYS_PMCFG_SLAVE_PERI_IRQ_EN);
			interrupt_attach(interrupt_0, (int8_t)interrupt_0, power_management_ISR);
			SYS->PMCFG_L |= MASK_SYS_PMCFG_SLAVE_PERI_IRQ_EN;
#endif

			// Close UART
#if 0
			uart_close(UART0);
			sys_disable(sys_device_uart0);
#endif

			CRITICAL_SECTION_BEGIN
			{
				/* Power down Mode */
				D2XXTEST__Sleep = 1;
				D2XXTEST__Wakeup = 0;
			}
			CRITICAL_SECTION_END
			SYS->PMCFG_L |= MASK_SYS_PMCFG_PM_PWRDN_MODE;
			SYS->PMCFG_L |= MASK_SYS_PMCFG_PM_PWRDN;

			asm volatile ("nop");
			asm volatile ("nop");
			asm volatile ("nop");
			asm volatile ("nop");
			asm volatile ("nop");

			//wait for wakeup
			while(!D2XXTEST__Wakeup);

			//Woken up!!! Disable all wakeup interrupt sources
			interrupt_disable_globally();
#if defined(__FT930__)
			interrupt_detach(interrupt_0);
			SYS->PMCFG_L &= ~(MASK_SYS_PMCFG_SLAVE_PERI_IRQ_EN);
#endif

			//Remove powerdown mode
			SYS->PMCFG_L &= ~MASK_SYS_PMCFG_PM_PWRDN_MODE;
			SYS->PMCFG_L &= ~MASK_SYS_PMCFG_PM_PWRDN;

#ifdef GPIO_REMOTE_WAKEUP
			gpio_interrupt_disable(GPIO_PIN);
			interrupt_detach(interrupt_gpio);
			gpio_is_interrupted(GPIO_PIN);
#endif

			D2XXTEST__Sleep = 0;
			D2XXTEST__Suspend = 0;
			D2XXTEST__Wakeup = 0;

			//Enable back the UART
#if 0
			debug_uart_init();
#endif
			interrupt_enable_globally();
			dbg("Exit sleep:%02X... \r\n", wkupSource);
			if (SetRemoteWakeup)
			{
				D2XX_IOCTL(0, D2XX_IOCTL_SYS_REMOTE_WAKEUP, &SetRemoteWakeup, NULL);
				SetRemoteWakeup = 0;
				dbg("Sending remote wakeup to host.. \r\n");
			}
		}
    }
    return 0;
}

/**
 Transmit a buffer of data over UART[idx] asynchronously.

 @return The number of bytes written to the buffer
 */
size_t uartTx(int idx, uint8_t *buffer, size_t len)
{
	signed int start;
	size_t copied = 0;

	// Determine if we need to start a transmission or there is one
	// already in action.
	CRITICAL_SECTION_BEGIN
	start = uartBufferOut_wr_idx[idx] - uartBufferOut_rd_idx[idx];
	CRITICAL_SECTION_END

	CRITICAL_SECTION_BEGIN
	// Copy in as much data from the producer as there is space for
	// in the out buffer.
	while (len--)
	{
		// Check there is space there to receive the data from the producer.
		if (uartBufferOut_avail[idx] > 0)
		{
			// Reduce the number of available bytes.
			uartBufferOut_avail[idx]--;

			// Add byte of data to the buffer.
			uartBufferOut_data[idx][uartBufferOut_wr_idx[idx]] = *buffer++;

			// Increment the pointer and wrap around.
			uartBufferOut_wr_idx[idx]++;
			if (uartBufferOut_wr_idx[idx] == RINGBUFFER_SIZE) uartBufferOut_wr_idx[idx] = 0;

			// Tally up number of bytes actually transmitted.
			copied++;
		}
	}
	CRITICAL_SECTION_END

	// Start new a transmission if nothing is being transmitted.
	if (start == 0)
	{
		uint8_t c;

		CRITICAL_SECTION_BEGIN

		// Get data from the out buffer.
		c = uartBufferOut_data[idx][uartBufferOut_rd_idx[idx]];

		// Increment the consumer pointer and wrap around.
		uartBufferOut_rd_idx[idx]++;
		if (uartBufferOut_rd_idx[idx] == RINGBUFFER_SIZE) uartBufferOut_rd_idx[idx] = 0;

		// Byte has been sent, update available bytes value.
		uartBufferOut_avail[idx]++;

		CRITICAL_SECTION_END
		//dbg("wr[%d]:%c\n",idx,c);
		// Send data to the UART. This will result in a TX interrupt when
		// it completes.
		uart_write(uart_configuration[idx].uart_id, c);
	}

	return copied;
}

/**
 Receive a number of bytes from UART[idx]

 @return The number of bytes read from UART[idx]
 */
size_t uartRx(int idx, uint8_t *buffer, size_t len)
{
	size_t copied = 0;

	// Copy in as much data as we can ...
	//   This can be either the maximum size of the buffer being given
	//   or the maximum number of bytes available in the Serial Port
	//   buffer.
	while(len--)
	{
		CRITICAL_SECTION_BEGIN
		if (uartBufferIn_avail[idx] < RINGBUFFER_SIZE)
		{
			uartBufferIn_avail[idx]++;

			*buffer = uartBufferIn_data[idx][uartBufferIn_rd_idx[idx]];

			//dbg("rd[%d]:%c\n",idx,*buffer);

			buffer++;

			// Increment the pointer and wrap around.
			uartBufferIn_rd_idx[idx]++;
			if (uartBufferIn_rd_idx[idx] == RINGBUFFER_SIZE) uartBufferIn_rd_idx[idx] = 0;

			copied++;
		}
		CRITICAL_SECTION_END
	}

	// Report back how many bytes have been copied into the buffer...
	return copied;
}

/**
 The Interrupt which handles asynchronous transmission and reception
 of data into the ring buffer
 */
void uart_ISR(void)
{
	static uint8_t c;
	//int i = 0;
	for (int i = 0; i < no_of_uarts; i++)
	{
		// Transmit interrupt...
		if (uart_is_interrupted(uart_configuration[i].uart_id, uart_interrupt_tx))
		{
			// Check to see how much data we have to transmit...
			//avail = uartBufferOut_rd_idx - uartBufferOut_wr_idx;

			if (uartBufferOut_avail[i] < RINGBUFFER_SIZE)
			{
				uartBufferOut_avail[i]++;

				// Copy out the byte to be transmitted so that the uart_write is
				// the last thing we do...
				c = uartBufferOut_data[i][uartBufferOut_rd_idx[i]];

				// Increment the pointer and wrap around.
				uartBufferOut_rd_idx[i]++;
				if (uartBufferOut_rd_idx[i] == RINGBUFFER_SIZE) uartBufferOut_rd_idx[i] = 0;

				// Write out a new byte, the following Transmit interrupt should handle
				// the remaning bytes...
				uart_write(uart_configuration[i].uart_id, c);
			}
		}

		// Receive interrupt...
		if (uart_is_interrupted(uart_configuration[i].uart_id, uart_interrupt_rx))
		{
			// Read a byte into the Ring Buffer...
			uart_read(uart_configuration[i].uart_id, &c);

			// Do not overwrite data already in the buffer.
			if (uartBufferIn_avail[i] > 0)
			{
				uartBufferIn_avail[i]--;

				uartBufferIn_data[i][uartBufferIn_wr_idx[i]] = c;

				// Increment the pointer and wrap around.
				uartBufferIn_wr_idx[i]++;
				if (uartBufferIn_wr_idx[i] == RINGBUFFER_SIZE) uartBufferIn_wr_idx[i] = 0;
			}
		}
	}
}
/**
 The initialization function for an UART.
 */
void uart_init(int i)
{

	/* Enable the UART Device... */
	sys_enable(uart_configuration[i].sys_uart_id);
	gpio_function(uart_configuration[i].tx_pin_num, uart_configuration[i].tx_pin_func); /* UART0 TXD */
	gpio_function(uart_configuration[i].rx_pin_num, uart_configuration[i].rx_pin_func); /* UART0 RXD */

	// Open the UART using the coding required.
	uart_open(uart_configuration[i].uart_id,                    /* Device */
			1,                        /* Prescaler = 1 */
			uart_configuration[i].baudrate,  /* Divider = 1302 */
			uart_data_bits_8,         /* No. buffer Bits */
			uart_parity_none,         /* Parity */
			uart_stop_bits_1);        /* No. Stop Bits */


	CRITICAL_SECTION_BEGIN
	{
		/* Attach the interrupt so it can be called... */
		interrupt_attach(uart_configuration[i].interrupt_num, (uint8_t) uart_configuration[i].interrupt_num, uart_ISR/*uart_configuration[i].uart_ISR*/);
		/* Enable the UART to fire interrupts when receiving data... */
		uart_enable_interrupt(uart_configuration[i].uart_id, uart_interrupt_rx);
		/* Enable the UART to fire interrupts when transmitting data... */
		uart_enable_interrupt(uart_configuration[i].uart_id, uart_interrupt_tx);
		/* Enable interrupts to be fired... */
		uart_enable_interrupts_globally(uart_configuration[i].uart_id);

	}
	CRITICAL_SECTION_END
}

uint8_t bufUart2Usb[MAX_UARTS][RINGBUFFER_SIZE] = {0};
uint8_t bufUsb2Uart[MAX_UARTS][RINGBUFFER_SIZE] = {0};
/* Test function that does the D2XX to UART and viceversa bridging functionality */
void d2xx_uart_bridge (void)
{
	size_t read_bytes;
	int bytesWritten;

	for (int i = 0; i < no_of_uarts; i++)
	{
			/*read the bytes out... */
			read_bytes = D2XX_Read(i+1, &bufUsb2Uart[i][0],RINGBUFFER_SIZE);

			if (read_bytes)
			{
				// Write it out to the UART.
				//dbg("dr[%d]: %d\n",i,read_bytes);
				uartTx(i, &bufUsb2Uart[i][0], read_bytes);
			}
#if 1 // FIXME: Workaround to get DFU detach to work
			delayms(100);
			if (D2XXTEST__DfuDetach)
			{
				return;
			}
#endif

			// Read in a packet of data from the UART.
			// (up to the size of the ring buffer)
			read_bytes = uartRx(i, &bufUart2Usb[i][0], RINGBUFFER_SIZE);

			/* Write the byte out... */
			if (read_bytes > 0)
			{
					int	ptr = 0;
					do
					{
						bytesWritten = D2XX_Write(i+1, &bufUart2Usb[i][ptr], read_bytes);
						if (bytesWritten > 0)
						{
							//dbg("dw[%d]: %d\n",i,bytesWritten);
							read_bytes -= bytesWritten;
							ptr += bytesWritten;
						};
					}while (read_bytes);
			}
	}
}

void setup(void)
{
	ED2XX_ErrorCode retVal;

	D2XXTEST__Suspend = 0;
	D2XXTEST__Sleep = 0;
	D2XXTEST__Wakeup = 0;

#ifdef BDFU
	timer_prescaler(1000); /* to ward off linkage issue */
#endif
#if defined(__FT900__)
	interrupt_attach(interrupt_0, (int8_t)interrupt_0, power_management_ISR);
#endif

#if defined(__FT930__)
	retVal =
			D2XX_Init(&D2XXTEST_UserD2xxConfig, d2xx_callback, NULL);


    /*slave sub-system control register setup*/
    *(SLAVECPU) |= (MASK_SLAVE_CPU_CTRL_SLV_RESET);  // assert bit to keep slave CPU in reset
    *(SLAVECPU) |= (MASK_SLAVE_CPU_CTRL_D2XX_MODE);    // turn-on D2XX_mode
    *(SLAVECPU) &= ~(MASK_SLAVE_CPU_CTRL_SLV_RESET); // de-assert bit to allow slave CPU to start
#else

	retVal =

			D2XX_Init(__pD2XXDefaultConfiguration, d2xx_callback, NULL);
	memcpy_pm2dat(&D2XXTEST_UserD2xxConfig.ConfigDesc, (uint32_t)&__pD2XXDefaultConfiguration->ConfigDesc, sizeof(TConfigDescriptors));
#endif

	sys_enable(sys_device_timer_wdt);
	/* Register the interrupt... */
	interrupt_attach(interrupt_timers, 17, timer_ISR);
	timer_prescaler(timer_select_a, 1000);
	/* Enable Timers... */
	timer_init(timer_select_a, 100, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
	timer_enable_interrupt(timer_select_a);
	timer_start(timer_select_a);
	interrupt_enable_globally(); //needed for interrupts
#if 0
	if (retVal == D2XX_ERR_NONE)
	{
		check_device_status = 1;
	}
#endif

	no_of_uarts = (D2XXTEST_UserD2xxConfig.ConfigDesc.NumOfD2XXInterfaces > MAX_UARTS)?MAX_UARTS:D2XXTEST_UserD2xxConfig.ConfigDesc.NumOfD2XXInterfaces;
	for (int i = 0; i < no_of_uarts; i++)
	{
		uart_init(i);
	}

#ifdef DEBUG
	/* Enable tfp_printf() functionality... */
	init_printf(UART0, tfp_putc);
#endif

    /* Print out a welcome message... */
	dbg("(C) Copyright, Bridgetek Pte. Ltd. \r\n"
			"--------------------------------------------------------------------- \r\n"
			"Welcome to D2XX UART Bridge Example... \r\n"
			"\r\n"
			"Enter any text on the D2XX[1,2..4] port, the same is echoed on \r\n"
			"the UART[0,1,..3]... \r\n"
			"--------------------------------------------------------------------- \r\n"
	);
	dbg("D2XX_Init() called, Result: %d Interfaces: %d\r\n", retVal, D2XXTEST_UserD2xxConfig.ConfigDesc.NumOfD2XXInterfaces);

}

/** @name tfp_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
void tfp_putc(void* p, char c)
{
	uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}

/** @name d2xx_callback
 *  @details Callback function to the D2XX library.
 *  @param
 *  @param
 */
void d2xx_callback(ED2XX_EventCode  eventID, void *ref, void* param1, void* param2)
{
	/* 0 => Device, 1-3 => Interface*/
	/* or */
	/* 0 => Remote Wakeup disabled, 1 => Remote wakeup enabled */
	uint8_t param = 0;

	if (param1)
	{
		param = (*(uint8_t *)param1);
	}
	//dbg("~%d",eventID);
	//dbg("%s\n", D2XXTest_EventStrings[eventID]);
	switch(eventID)
	{
	case D2XX_EVT_SUSPEND:
		if (!D2XXTEST__Sleep)
		{
			D2XXTEST__Suspend = 1;
			D2XXTEST__Sleep = 0;
			RemoteWakeupEnable = param;
			//dbg("RemoteWakeup Enabled :%d \r\n", RemoteWakeupEnable);
		}
		break;
	case D2XX_EVT_RESUME:
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_RESUME;
			D2XXTEST__Wakeup = 1;
		}
		else
		{
			D2XXTEST__Suspend = 0;
		}
		break;
	case D2XX_EVT_BUS_RESET:
#if 0
		check_device_status = 0;
#endif
		break;
	case D2XX_EVT_READY: /*Connected */
		D2XXTEST__Ready = 1;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_READY;
			D2XXTEST__Wakeup = 1;
		}
		else
		{
			D2XXTEST__Suspend = 0;
		}
		break;
	case D2XX_EVT_UNREADY: /*Detached */
		D2XXTEST__Ready = 0;
		D2XXTEST__Suspend = 1;
		break;
	case D2XX_EVT_DFU_DETACH: /* DFU Detach from DFU application */
		D2XXTEST__DfuDetach = 1;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_OTHERS;
			D2XXTEST__Wakeup = 1;
		}
		else
		{
			D2XXTEST__Suspend = 0;
		}
		break;
	case D2XX_EVT_TESTMODE: /* Testmode during electrical tests */
		D2XXTEST__Testmode = 1;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_OTHERS;
			D2XXTEST__Wakeup = 1;
		}
		else
		{
			D2XXTEST__Suspend = 0;
		}
		break;
	default:
		break;
	}
}

/* Initializes the UART for the testing */
void debug_uart_init(void)
{

	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
#if defined(__FT930__)
    /* Make GPIO23 function as UART0_TXD and GPIO22 function as UART0_RXD... */
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#else
	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#endif

	// Open the UART using the coding required.
	uart_open(UART0,                    /* Device */
			1,                        /* Prescaler = 1 */
			UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
			uart_data_bits_8,         /* No. buffer Bits */
			uart_parity_none,         /* Parity */
			uart_stop_bits_1);        /* No. Stop Bits */

	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
	);

#ifdef DEBUG
	/* Enable tfp_printf() functionality... */
	init_printf(UART0, tfp_putc);
#endif

}
#if 0
/* function to calculate the expected XOR checksum for the d2xx configuration */
uint16_t xorChecksum(const char str[], uint16_t length) {
    uint16_t sum = 0;
    uint16_t *s = (uint16_t *)str;

    length >>= 1;
    while (length) {
        sum ^= *s++;
        length--;
    }
    return sum;
}
#endif

/* Power management ISR */
void power_management_ISR(void)
{

#if defined(__FT930__)
	if (SYS->PMCFG_H & MASK_SYS_PMCFG_SLAVE_PERI_IRQ_PEND)
	{
		// Clear d2xx hw engine wakeup
		SYS->PMCFG_H = MASK_SYS_PMCFG_SLAVE_PERI_IRQ_PEND;

		//D2XXTEST__Suspend = 0;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_SLAVE;
			D2XXTEST__Wakeup = 1;
		}
	}
#endif
	if (SYS->PMCFG_H & MASK_SYS_PMCFG_PM_GPIO_IRQ_PEND)
	{
		// Clear GPIO wakeup pending
		SYS->PMCFG_H = MASK_SYS_PMCFG_PM_GPIO_IRQ_PEND;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_GPIO;
			D2XXTEST__Wakeup = 1;
		}
	}
#if defined(__FT900__)
	if (SYS->PMCFG_H & MASK_SYS_PMCFG_DEV_CONN_DEV)
	{
		// Clear connection interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_CONN_DEV;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_CONNECT;
			D2XXTEST__Wakeup = 1;
		}

	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_DEV_DIS_DEV)
	{
		// Clear disconnection interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_DIS_DEV;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_DISCONNECT;
			D2XXTEST__Wakeup = 1;
		}
	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_HOST_RST_DEV)
	{
		// Clear Host Reset interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_HOST_RST_DEV;
		USBD_resume();
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_RESET;
			D2XXTEST__Wakeup = 1;
		}
	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_HOST_RESUME_DEV)
	{
		// Clear Host Resume interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_HOST_RESUME_DEV;
		if(! (SYS->MSC0CFG & MASK_SYS_MSC0CFG_DEV_RMWAKEUP))
		{
			// If we are driving K-state on Device USB port;
			// We must maintain the 1ms requirement before resuming the phy
			USBD_resume();
		}
	}
#endif
}


#ifdef GPIO_REMOTE_WAKEUP
void gpio_ISR()
{
    if (gpio_is_interrupted(GPIO_PIN))
    {
		if (D2XXTEST__Sleep)
		{
			//Need to perform remote wakeup procedure
			D2XXTEST__Wakeup = 1;
			SetRemoteWakeup = 1;
		}
    }
}
#endif

void timer_ISR(void)
{
	if (timer_is_interrupted(timer_select_a))
	{
		D2XX_Timer();
#if 0
		if (check_device_status)
		{
			device_inactive_counter++;

			if (device_inactive_counter >= DEVICE_INACTIVITY)
			{
				dbg("Device inactive !!!\n");
				USBD_suspend_device();
				device_inactive_counter = 0;
				check_device_status = 0;
			}
		}
#endif

	}
}
