/**
  @file
  @brief
  Free RTOS Example 3

  An example with FreeRTOS illustrating the use of time-slicing, preemption and mutex based synchronization.
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
#include "ft900.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tinyprintf.h"

/* Three Demo Tasks are implemented here to illustrate the usage of FreeRTOS.
 *
 * DEMO 1 - Time-slicing example
 * ------------------------------
 * 3 Tasks with equal priority are created. FreeRTOS will try give all 3 of them equal execution time on average.
 *
 * DEMO 2 - Testing task priorities
 * ------------------------------
 * 3 Tasks of different priorities (1,2,3) are created. The task with priority 2 runs constantly, never yielding.
 * The task with priority 3 (highest priority) prints it's name and yields every 500mS. The net result is that Task_2
 * runs constantly, while being interrupted by Task_3 every 500mS. Task_1 never gets to run.
 *
 * DEMO 3 - Using a mutex to synchronize access
 * ------------------------------
 * 4 Tasks are created with different priorities. Each of the tasks print lines onto UART0. Since the tasks have
 * different priorities, preemption will occur and a mutex is used to synchronize access to UART0, keeping the printed
 * strings uninterrupted.
 *
 */

// Demos range from 1 - 3
#define FRT_DEMO 1

#define mainDELAY_LOOP_COUNT 100000


#if(FRT_DEMO == 1)
static void frt_demo1_setup(void);
#elif(FRT_DEMO == 2)
static void frt_demo2_setup(void);
#elif(FRT_DEMO == 3)
static void frt_demo3_setup(void);
#endif

/** tfp_printf putc
 *  @param p Parameters
 *  @param c The character to write */
void myputc(void* p, char c) {
	uart_write((ft900_uart_regs_t*) p, (uint8_t) c);
}



int main(void)
{

	/*
	 * Peripheral reset
	 */
	sys_reset_all();

	/* enable uart */
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
	/* Enable tfp_printf() functionality... */
	init_printf(UART0, myputc);


#if(FRT_DEMO == 1)
	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
					"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
					"Copyright (C) Bridgetek Pte Ltd \r\n"
					"--------------------------------------------------------------------- \r\n"
					"Welcome to Free RTOS Test Example 3... \r\n"
					"\r\n"
					"Demonstrate FreeRTOS Time-slicing\r\n"
					"--------------------------------------------------------------------- \r\n");
	frt_demo1_setup();
#elif(FRT_DEMO == 2)
	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
					"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
					"Copyright (C) Bridgetek Pte Ltd \r\n"
					"--------------------------------------------------------------------- \r\n"
					"Welcome to Free RTOS Test Example 3... \r\n"
					"\r\n"
					"Demonstrate FreeRTOS Task Priority handling\r\n"
					"--------------------------------------------------------------------- \r\n");
	frt_demo2_setup();
#elif((FRT_DEMO == 3))
	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
					"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
					"Copyright (C) Bridgetek Pte Ltd \r\n"
					"--------------------------------------------------------------------- \r\n"
					"Welcome to Free RTOS Test Example 3... \r\n"
					"\r\n"
					"Demonstrate FreeRTOS mutex based synchronization\r\n"
					"--------------------------------------------------------------------- \r\n");
	frt_demo3_setup();
#endif

	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	tfp_printf("Should never reach here!\n");

	for (;;)
		;
}

#if (FRT_DEMO == 1)

/*
 * Real test starts here
 *
 ***************************************************
 */

void vTask1(void *pvParameters)
{
	 char *pcTaskName = "Task1\n";
	volatile unsigned long ul;
	int	a, b;

	for(;;)
	{
		vTaskSuspendAll ();
		/* tfp_printf out the name of this task. */
		tfp_printf(pcTaskName);
		xTaskResumeAll ();

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			a = a * 10;
			b = b * 10;
		}
	}
}

void vTask2(void *pvParameters)
{
	 char *pcTaskName = "Task2\r\n";
	volatile unsigned long ul;
	int	a, b;

	for(;;)
	{
		vTaskSuspendAll ();
		/* tfp_printf out the name of this task. */
		tfp_printf(pcTaskName);
		xTaskResumeAll ();

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			a = a * 10;
			b = b * 10;
		}
	}
}

void vTask3(void *pvParameters)
{
	 char *pcTaskName = "Task3\r\n";
	volatile unsigned long ul;
	int	a, b;

	for(;;)
	{
		vTaskSuspendAll ();
		/* tfp_printf out the name of this task. */
		tfp_printf(pcTaskName);
		xTaskResumeAll ();

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			a = a * 10;
			b = b * 10;
		}
	}
}


static void frt_demo1_setup(void)
{


	xTaskCreate(vTask1, /* Pointer to the function that implements the task. */
		"Task 1",               /* Text name for the task */
		1000,			/* Stack depth  */
		NULL,			/* We are not using the task parameter. */
		1,			/* Priority */
		NULL);			 /* We are not going to use the task handle. */

	xTaskCreate(vTask2, "Task 2", 1000, NULL, 1, NULL);
	xTaskCreate(vTask3, "Task 3", 1000, NULL, 1, NULL);


}

#elif(FRT_DEMO == 2)

/*
 * Test starts here
 *
 ***************************************************
 */


/* This task is the highest priority and prints it's name every half second */

void vTask1(void *pvParameters)
{
	 char *pcTaskName = "*Task1*\r\n";

	for(;;)
	{
		/* tfp_printf out the name of this task. */
		tfp_printf(pcTaskName);

		vTaskDelay(500 * portTICK_PERIOD_MS);
	}
}

/* This task constantly runs */

void vTask2(void *pvParameters)
{
	 char *pcTaskName = "Task2\r\n";
	volatile unsigned long ul;
	int	a, b;

	for(;;)
	{
		/* tfp_printf out the name of this task. */
		tfp_printf(pcTaskName);

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			a = a * 10;
			b = b * 10;
		}
	}
}

/* This task should never run, too low a priority */

void vTask3(void *pvParameters)
{
    char *pcTaskName = "Task3\r\n";

	for(;;)
	{
		/* tfp_printf out the name of this task. */
		tfp_printf(pcTaskName);
	}
}

static void frt_demo2_setup(void)
{
	tfp_printf("Demo 2\n");


	xTaskCreate(vTask1, /* Pointer to the function that implements the task. */
		"Task 1",               /* Text name for the task */
		1000,			/* Stack depth  */
		NULL,			/* We are not using the task parameter. */
		3,			/* Priority */
		NULL);			 /* We are not going to use the task handle. */

	xTaskCreate(vTask2, "Task 2", 1000, NULL, 2, NULL);
	xTaskCreate(vTask3, "Task 3", 1000, NULL, 1, NULL);

}


#elif (FRT_DEMO == 3)

/*
 * Real test starts here
 *
 ***************************************************
 */

SemaphoreHandle_t xMutex;

static void prvNewPrintString( portCHAR *pcString)
{
	/* The mutex is created before the scheduler is started so already
	 * exists by the time this task first executes.
	 * Attempt to take the mutex, blocking indefinitely to wait for the mutex if
	 * it is not available straight away. The call to xSemaphoreTake() will only
	 * return when the mutex has been successfully obtained so there is no need to
	 * check the function return value. If any other delay period was used then
	 * the code must check that xSemaphoreTake() returns pdTRUE before accessing
	 * the shared resource (which in this case is standard out).
	 */

	xSemaphoreTake(xMutex, portMAX_DELAY);

	{
		/*
		 * The following line will only execute once the mutex has been
		 * successfully obtained. Standard out can be accessed freely now as
		 * only one task can have the mutex at any one time.
		 */

		tfp_printf(pcString);

		/* The mutex MUST be given back! */
	}

	xSemaphoreGive(xMutex);
}

static void prvPrintTask(void *pvParameters)
{
	char *pcStringToPrint;

	/*
	 * four instances of this task are created so the string the task will send
	 * to prvNewPrintString() is passed into the task using the task parameter.
	 * Cast this to the required type.
	 */

	pcStringToPrint = (char *)pvParameters;
	for(;;)
	{
		/* tfp_printf out the string using the newly defined function. */
		prvNewPrintString(pcStringToPrint);

		/*
		 * Wait a pseudo random time. Note that rand() is not necessarily
		 * reentrant, but in this case it does not really matter as the code does
		 * not care what value is returned. In a more secure application a version
		 * of rand() that is known to be reentrant should be used - or calls to
		 * rand() should be protected using a critical section.
		 */
		vTaskDelay((rand() & 0x1FF));
	}
}


static void frt_demo3_setup(void)
{
	tfp_printf("Demo 3\r\n");

	/* The tasks are going to use a pseudo random delay, seed the random number
	 * generator.
	*/

	srand(567);


	/* Before a semaphore is used it must be explicitly created. In this example
	 * a mutex type semaphore is created.
	 */

	xMutex = xSemaphoreCreateMutex();

	/* Check the semaphore was created successfully before creating the tasks. */

	if(xMutex != NULL)
	{
		/* Create two instances of the tasks that write to stdout. The string
		 * they write is passed in as the task parameter. The tasks are created
		 * at different priorities so some pre-emption will occur.
		 */

		xTaskCreate(prvPrintTask, "Print1", 512,
		"Task 1 ******************************************\r\n", 1, NULL);

		xTaskCreate(prvPrintTask, "Print2", 512,
		"Task 2 ------------------------------------------\r\n", 2, NULL);

		xTaskCreate(prvPrintTask, "Print3", 512,
		"Task 3 ##########################################\r\n", 3, NULL);

		xTaskCreate(prvPrintTask, "Print4", 512,
		"Task 4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n", 4, NULL);

		xTaskCreate(prvPrintTask, "Print5", 512,
		"Task 5 ++++++++++++++++++++++++++++++++++++++++++\r\n", 5, NULL);

		xTaskCreate(prvPrintTask, "Print6", 512,
		"Task 6 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n", 6, NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}

}


#endif

