/**
  @file
  @brief
  Free RTOS Example 1

  An example with FreeRTOS illustrating the use of Interrupts with context switching,
  and data exchange with Queues. A Transmission Task sends over UART1 (user should connect the UART1_RX and UART1_TX
  lines together) at random intervals. The UART ISR receives the data and pushes it into a Receive Queue,
  which is then consumed by a Receive Task. The example is based on the FreeRTOS demo examples.
*/
/*
 * ============================================================================
 * History
 * =======
 * 29 Dec 2015 : Created
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
#include <string.h>
#include <stdlib.h>
#include <ft900.h>
#include "assert.h"
#include "tinyprintf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "integer.h"
#include "flop.h"
#include "comtest.h"


/* Constants for the ComTest tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )
#define mainCOM_TEST_LED		( 5 )
/* Priorities for the demo application tasks. */
#define mainCOM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 4 )

/* The rate at which the on board LED will toggle when there is/is not an
error. */
#define mainNO_ERROR_FLASH_PERIOD	( ( TickType_t ) 3000 / portTICK_PERIOD_MS  )
#define mainERROR_FLASH_PERIOD		( ( TickType_t ) 500 / portTICK_PERIOD_MS  )
#define mainON_BOARD_LED_BIT		( ( unsigned long ) 7 )

/* Constants used by the vMemCheckTask() task. */
#define mainCOUNT_INITIAL_VALUE		( ( unsigned long ) 0 )
#define mainNO_TASK					( 0 )

/* The size of the memory blocks allocated by the vMemCheckTask() task. */
#define mainMEM_CHECK_SIZE_1		( ( size_t ) 51 )
#define mainMEM_CHECK_SIZE_2		( ( size_t ) 52 )
#define mainMEM_CHECK_SIZE_3		( ( size_t ) 151 )

/*-----------------------------------------------------------*/

/*
 * Checks that all the demo application tasks are still executing without error
 * - as described at the top of the file.
 */
static long prvCheckOtherTasksAreStillRunning( unsigned long ulMemCheckTaskCount );

/*
 * The task that executes at the highest priority and calls
 * prvCheckOtherTasksAreStillRunning().  See the description at the top
 * of the file.
 */
static void vErrorChecks( void *pvParameters );

/*
 * Dynamically created and deleted during each cycle of the vErrorChecks()
 * task.  This is done to check the operation of the memory allocator.
 * See the top of vErrorChecks for more details.
 */
static void vMemCheckTask( void *pvParameters );


/*-----------------------------------------------------------*/

void setup(void);
void myputc(void* p, char c);



static void vErrorChecks( void *pvParameters )
{
TickType_t xDelayPeriod = mainNO_ERROR_FLASH_PERIOD;
unsigned long ulMemCheckTaskRunningCount;
TaskHandle_t xCreatedTask;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	/* Cycle for ever, delaying then checking all the other tasks are still
	operating without error.  If an error is detected then the delay period
	is decreased from mainNO_ERROR_FLASH_PERIOD to mainERROR_FLASH_PERIOD so
	the on board LED flash rate will increase.

	In addition to the standard tests the memory allocator is tested through
	the dynamic creation and deletion of a task each cycle.  Each time the
	task is created memory must be allocated for its stack.  When the task is
	deleted this memory is returned to the heap.  If the task cannot be created
	then it is likely that the memory allocation failed. */

	for( ;; )
	{
		/* Reset xCreatedTask.  This is modified by the task about to be
		created so we can tell if it is executing correctly or not. */
		xCreatedTask = mainNO_TASK;

		/* Dynamically create a task - passing ulMemCheckTaskRunningCount as a
		parameter. */
		ulMemCheckTaskRunningCount = mainCOUNT_INITIAL_VALUE;
		if( xTaskCreate( vMemCheckTask, "MEM_CHECK", configMINIMAL_STACK_SIZE, ( void * ) &ulMemCheckTaskRunningCount, tskIDLE_PRIORITY, &xCreatedTask ) != pdPASS )
		{
			tfp_printf("%s", "Error creating MEM Check Task!\n");
			/* Could not create the task - we have probably run out of heap. */
			xDelayPeriod = mainERROR_FLASH_PERIOD;
		}

		/* Delay until it is time to execute again. */
		vTaskDelay( xDelayPeriod );

		/* Delete the dynamically created task. */
		if( xCreatedTask != mainNO_TASK )
		{
			vTaskDelete( xCreatedTask );
		}

		/* Check all the standard demo application tasks are executing without
		error.  ulMemCheckTaskRunningCount is checked to ensure it was
		modified by the task just deleted. */
		if( prvCheckOtherTasksAreStillRunning( ulMemCheckTaskRunningCount ) != pdPASS )
		{
			/* An error has been detected in one of the tasks - flash faster. */
			xDelayPeriod = mainERROR_FLASH_PERIOD;
			tfp_printf("%s", "*******ERROR!******* Tests Failed!\r\n");
			//while(1);
		}

	}
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static long prvCheckOtherTasksAreStillRunning( unsigned long ulMemCheckTaskCount )
{
long lReturn = ( long ) pdPASS;

	/* Check all the demo tasks (other than the flash tasks) to ensure
	that they are all still running, and that none of them have detected
	an error. */
#if defined(__FT900__)
if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
{
	lReturn = ( long ) pdFAIL;
	tfp_printf("%s", "Int Math Task Tests Failed!\n");
}

if( xAreMathsTaskStillRunning() != pdTRUE )
{
	lReturn = ( long ) pdFAIL;
	tfp_printf("%s", "Math Task Tests Failed!\n");
}
#endif
	if( xAreComTestTasksStillRunning() != pdTRUE )
	{
		lReturn = ( long ) pdFAIL;
		tfp_printf("%s", "Comm Task Tests Failed - Please check that UART1 RX and TX lines are looped back!\n");
	}

	if( ulMemCheckTaskCount == mainCOUNT_INITIAL_VALUE )
	{
		/* The vMemCheckTask did not increment the counter - it must
		have failed. */
		lReturn = ( long ) pdFAIL;
	}

	return lReturn;
}
/*-----------------------------------------------------------*/

static void vMemCheckTask( void *pvParameters )
{
unsigned long *pulMemCheckTaskRunningCounter;
void *pvMem1, *pvMem2, *pvMem3;
long lErrorOccurred = pdFALSE;

	/* This task is dynamically created then deleted during each cycle of the
	vErrorChecks task to check the operation of the memory allocator.  Each time
	the task is created memory is allocated for the stack and TCB.  Each time
	the task is deleted this memory is returned to the heap.  This task itself
	exercises the allocator by allocating and freeing blocks.

	The task executes at the idle priority so does not require a delay.

	pulMemCheckTaskRunningCounter is incremented each cycle to indicate to the
	vErrorChecks() task that this task is still executing without error. */

	pulMemCheckTaskRunningCounter = ( unsigned long * ) pvParameters;

	for( ;; )
	{
		if( lErrorOccurred == pdFALSE )
		{
			/* We have never seen an error so increment the counter. */
			( *pulMemCheckTaskRunningCounter )++;
			//tfp_printf( "mem check cnt = %d\n",*pulMemCheckTaskRunningCounter );
		}
		else
		{
			/* There has been an error so reset the counter so the check task
			can tell that an error occurred. */
			*pulMemCheckTaskRunningCounter = mainCOUNT_INITIAL_VALUE;
			tfp_printf("%s", "*******ERROR!******* memCheck Error!!!\n");
		}

		/* Allocate some memory - just to give the allocator some extra
		exercise.  This has to be in a critical section to ensure the
		task does not get deleted while it has memory allocated. */
		vTaskSuspendAll();
		{
			pvMem1 = pvPortMalloc( mainMEM_CHECK_SIZE_1 );
			if( pvMem1 == NULL )
			{
				lErrorOccurred = pdTRUE;
				tfp_printf("%s", "Siz 1 Error!!!\n");
			}
			else
			{
				memset( pvMem1, 0xaa, mainMEM_CHECK_SIZE_1 );
				vPortFree( pvMem1 );
			}
		}
		xTaskResumeAll();

		/* Again - with a different size block. */
		vTaskSuspendAll();
		{
			pvMem2 = pvPortMalloc( mainMEM_CHECK_SIZE_2 );
			if( pvMem2 == NULL )
			{
				lErrorOccurred = pdTRUE;
				tfp_printf("%s", "Siz 2 Error!!!\n");
			}
			else
			{
				memset( pvMem2, 0xaa, mainMEM_CHECK_SIZE_2 );
				vPortFree( pvMem2 );
			}
		}
		xTaskResumeAll();

		/* Again - with a different size block. */
		vTaskSuspendAll();
		{
			pvMem3 = pvPortMalloc( mainMEM_CHECK_SIZE_3 );
			if( pvMem3 == NULL )
			{
				tfp_printf("%s", "Siz 3 Error!!!\n");
				lErrorOccurred = pdTRUE;
			}
			else
			{
				memset( pvMem3, 0xaa, mainMEM_CHECK_SIZE_3 );
				vPortFree( pvMem3 );
			}
		}
		xTaskResumeAll();
	}
}



void frt_start(void){

#if defined(__FT900__)
	/* Start the demo/test application tasks. */
	vStartIntegerMathTasks( tskIDLE_PRIORITY );
	vStartMathTasks( tskIDLE_PRIORITY );
#endif
	vStartComTestTasks( mainCOM_TEST_PRIORITY );

	/* Start the check task - which is defined in this file. */
	if(xTaskCreate( vErrorChecks, "Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL ) != 1){
		uart_puts(UART0, "Error creating mem check task\n");
	}

	/* Now all the tasks have been started - start the scheduler. */
	vTaskStartScheduler();

}


int main(void) {

	setup();

	for(;;);

	return 0;
}

/** Setup */
void setup(void) {
	sys_reset_all();

	/* Enable the UART Device... */
	sys_enable(sys_device_uart0);
    /* Set UART0 GPIO functions to UART0_TXD and UART0_RXD... */
#if defined(__FT900__)
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#elif defined(__FT930__)
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#endif
	uart_open(UART0, 1,
			UART_DIVIDER_19200_BAUD, uart_data_bits_8, uart_parity_none,
			uart_stop_bits_1);

	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
					"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
					"Copyright (C) Bridgetek Pte Ltd \r\n"
					"--------------------------------------------------------------------- \r\n"
					"Welcome to Free RTOS Test Example 1... \r\n"
					"\r\n"
					"Communication tasks which will send and receive data over UART1 using FreeRTOS Queues\r\n"
					"Please ensure that UART1 RX and TX lines are connected to each other.\r\n"
					"--------------------------------------------------------------------- \r\n");

	/* Enable tfp_printf() functionality... */
	init_printf(UART0, myputc);

	frt_start();

	// Control never comes here
	while(1);
}






/** Printf putc
 *  @param p Parameters
 *  @param c The character to write */
void myputc(void* p, char c) {
	uart_write((ft900_uart_regs_t*) p, (uint8_t) c);
}



/*
 * watch dog -- print message and hang about
 */

void watchdog_handler(void) {
	tfp_printf("*** Watch Dog Expired\n");

	while (1)
		;
}
