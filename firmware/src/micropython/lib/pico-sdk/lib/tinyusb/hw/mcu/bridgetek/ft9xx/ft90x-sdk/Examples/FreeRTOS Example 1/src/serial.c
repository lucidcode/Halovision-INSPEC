/*
    FreeRTOS V8.2.1 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR USART0.

	This file contains all the serial port components that can be compiled to
	either ARM or THUMB mode.  Components that must be compiled to ARM mode are
	contained in serialISR.c.
*/

/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

/* Demo application includes. */
#include "serial.h"
#include "ft900.h"

/*-----------------------------------------------------------*/

#define ENABLE_SEM_TEST

#if defined(__FT900__)
#define PIN_UART_TXD	53
#define PIN_UART_RXD	52
#elif defined(__FT930__)
#define PIN_UART_TXD	27
#define PIN_UART_RXD	26
#endif

/* Constants to setup and access the UART. */
#define portUSART0_AIC_CHANNEL	( ( unsigned long ) 2 )

#define serINVALID_QUEUE		( ( QueueHandle_t ) 0 )
#define serHANDLE				( ( xComPortHandle ) 1 )
#define serNO_BLOCK				( ( TickType_t ) 0 )

/*-----------------------------------------------------------*/

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;


#ifdef ENABLE_SEM_TEST
/* This semaphore does nothing useful except test a feature of the
scheduler. */
static SemaphoreHandle_t xTestSem;
#endif

void vSerialISR( void );


/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void xSerialHWInit(void){

	portENTER_CRITICAL();
	{
		/* Enable the UART Device... */
		sys_enable(sys_device_uart1);
		/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
		gpio_function(PIN_UART_TXD, pad_uart1_txd); /* UART1 TXD */
		gpio_function(PIN_UART_RXD, pad_uart1_rxd); /* UART1 RXD */
		uart_open(UART1, 1,
				UART_DIVIDER_921600_BAUD, uart_data_bits_8, uart_parity_none,
				uart_stop_bits_1);
#if defined(__FT930__)
		uint8_t cChar;
		uart_read(UART1, &cChar); // Clear register
#endif
		interrupt_attach(interrupt_uart1, (uint8_t) interrupt_uart1, vSerialISR);
		// UART has already been initialized in main(). Here we just setup the ISR
		/* Enable the UART to fire interrupts when receiving data... */
		uart_enable_interrupt(UART1, uart_interrupt_rx);
		//uart_enable_interrupt(UART1, uart_interrupt_tx);
	    uart_enable_interrupts_globally(UART1);

		tfp_printf("Setup of UART1 Complete !\n");

	}
	portEXIT_CRITICAL();
}

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{

xComPortHandle xReturn = serHANDLE;


/* Create the queues used to hold Rx/Tx characters. */
xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

	if(
		( xRxedChars != serINVALID_QUEUE ) &&
		( xCharsForTx != serINVALID_QUEUE )
	  )
	{

	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{
	/* The port handle is not required as this driver only supports UART1. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
signed char *pxNext;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART1. */
	( void ) pxPort;
	( void ) usStringLength;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
	( void ) pxPort;
	char cChar;
	/* Place the character in the queue of characters to be transmitted. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		uart_puts(UART0, "Q Send Fail!\n");
		return pdFAIL;
	}

	/* Turn on the Tx interrupt so the ISR will remove the character from the
	queue and send it.   This does not need to be in a critical section as
	if the interrupt has already removed the character the next interrupt
	will simply turn off the Tx interrupt again. */

	if( xQueueReceive( xCharsForTx, &cChar, 0 ) == pdTRUE )
	{
		/* Send the next character queued for Tx. */
		//uart_enable_interrupt(UART1, uart_interrupt_tx);
		uart_write(UART1, cChar);
	}


	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
	( void ) xPort;
}

/*-----------------------------------------------------------*/
#ifdef ENABLE_SEM_TEST

portBASE_TYPE xSerialWaitForSemaphore( xComPortHandle xPort )
{
const TickType_t xBlockTime = ( TickType_t ) 0xffff;

	/* This function does nothing interesting, but test the
	semaphore from ISR mechanism. */
	return xSemaphoreTake(xTestSem, xBlockTime );
}
/*-----------------------------------------------------------*/
#endif

/*-----------------------------------------------------------*/


void vSerialISR( void )
{
uint8_t cChar;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

// Note uart_is_interrupted() clears the appropriate interrupt flag
if (uart_is_interrupted(UART1, uart_interrupt_rx)) // Receive interrupt
{
	/* Get the character and post it on the queue of Rxed characters.
	If the post causes a task to wake force a context switch as the woken task
	may have a higher priority than the task we have interrupted. */

	uart_read(UART1, &cChar);
	//uart_write(UART0, cChar);

	xQueueSendFromISR( xRxedChars, ( const void * ) &cChar, &xHigherPriorityTaskWoken );

#ifdef ENABLE_SEM_TEST
	/* Also release the semaphore - this does nothing interesting and is just a test.
	We first attempt to unsuspend the task to check the scheduler correctly detects
	this as an invalid call, then give the semaphore for real. */
	vComTestUnsuspendTask();
	xSemaphoreGiveFromISR(xTestSem, &xHigherPriorityTaskWoken );
#endif

}

/* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
 should be performed to ensure the interrupt returns directly to the highest
 priority task.  The macro used for this purpose is dependent on the port in
 use and may be called portEND_SWITCHING_ISR(). */
if(xHigherPriorityTaskWoken){
	portYIELD_FROM_ISR();
}

}
/*-----------------------------------------------------------*/

