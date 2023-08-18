/**
  @file
  @brief
  Free RTOS Example 4

  An example with FreeRTOS to demonstrate FreeRTOS kernel resource monitoring feature in Eclipse.
*/
/*
 * ============================================================================
 * History
 * =======
 * Aug 2017 : Created
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
#include <stdio.h>
#include "ft900.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "list.h"
#include "tinyprintf.h"



#ifdef __FT930__
#define MAX_TASK 5
#define MAX_QUEUES 6
#else
#define MAX_TASK 10
#define MAX_QUEUES 9
#endif
#define MAX_TIMER 6



static SemaphoreHandle_t xMutex = NULL;
static TimerHandle_t timer[MAX_TIMER] = {NULL};
static TaskHandle_t task[MAX_TASK] = {NULL};
static QueueHandle_t queue[MAX_QUEUES] = {NULL};
static int taskCount = 0;
static void frt_demo_setup(void);
static void prvPrintTask(void *pvParameters);
extern int memcmp(const void*, const void*, size_t);
extern size_t strlen(const char *);



/** tfp_printf putc
 *  @param p Parameters
 *  @param c The character to write */
void myputc(void* p, char c) {
	uart_write((ft900_uart_regs_t*) p, (uint8_t) c);
}

int main(void)
{
	/* Peripheral reset */
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

	/* Print out a welcome message... */
	uart_puts(UART0,
		"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
		"\x1B[H" /* ANSI/VT100 - Move Cursor to Home */
		"Copyright (C) Bridgetek Pte Ltd \r\n"
		"--------------------------------------------------------------------- \r\n"
		"Welcome to Free RTOS Test Example 4... \r\n"
		"\r\n"
		"Demonstrate FreeRTOS kernel-aware debugging feature\r\n"
		"--------------------------------------------------------------------- \r\n\r\n");
	frt_demo_setup();

	tfp_printf("Should never reach here!\n");
	for (;;);
}



/*
 ***************************************************
 * TASK LIST
 * tasks.c  - removed static access for ulTotalRunTime
 *  (value has been optimized out, works if application is compiled with -O0 instead of -Og)
 ***************************************************
 */
typedef struct tskTaskControlBlock
{
	volatile StackType_t	*pxTopOfStack;	/*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */

	#if ( portUSING_MPU_WRAPPERS == 1 )
		xMPU_SETTINGS	xMPUSettings;		/*< The MPU settings are defined as part of the port layer.  THIS MUST BE THE SECOND MEMBER OF THE TCB STRUCT. */
	#endif

	ListItem_t			xStateListItem;	/*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
	ListItem_t			xEventListItem;		/*< Used to reference a task from an event list. */
	UBaseType_t			uxPriority;			/*< The priority of the task.  0 is the lowest priority. */
	StackType_t			*pxStack;			/*< Points to the start of the stack. */
	char				pcTaskName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the task when created.  Facilitates debugging only. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

	#if ( portSTACK_GROWTH > 0 )
		StackType_t		*pxEndOfStack;		/*< Points to the end of the stack on architectures where the stack grows up from low memory. */
	#endif

	#if ( portCRITICAL_NESTING_IN_TCB == 1 )
		UBaseType_t		uxCriticalNesting;	/*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
	#endif

	#if ( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t		uxTCBNumber;		/*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
		UBaseType_t		uxTaskNumber;		/*< Stores a number specifically for use by third party trace code. */
	#endif

	#if ( configUSE_MUTEXES == 1 )
		UBaseType_t		uxBasePriority;		/*< The priority last assigned to the task - used by the priority inheritance mechanism. */
		UBaseType_t		uxMutexesHeld;
	#endif

	#if ( configUSE_APPLICATION_TASK_TAG == 1 )
		TaskHookFunction_t pxTaskTag;
	#endif

	#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
		void *pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
	#endif

	#if( configGENERATE_RUN_TIME_STATS == 1 )
		uint32_t		ulRunTimeCounter;	/*< Stores the amount of time the task has spent in the Running state. */
	#endif

	#if ( configUSE_NEWLIB_REENTRANT == 1 )
		/* Allocate a Newlib reent structure that is specific to this task.
		Note Newlib support has been included by popular demand, but is not
		used by the FreeRTOS maintainers themselves.  FreeRTOS is not
		responsible for resulting newlib operation.  User must be familiar with
		newlib and must provide system-wide implementations of the necessary
		stubs. Be warned that (at the time of writing) the current newlib design
		implements a system-wide malloc() that must be provided with locks. */
		struct	_reent xNewLib_reent;
	#endif

	#if( configUSE_TASK_NOTIFICATIONS == 1 )
		volatile uint32_t ulNotifiedValue;
		volatile uint8_t ucNotifyState;
	#endif

	/* See the comments above the definition of
	tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE. */
	#if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 )
		uint8_t	ucStaticallyAllocated; 		/*< Set to pdTRUE if the task is a statically allocated to ensure no attempt is made to free the memory. */
	#endif

	#if( INCLUDE_xTaskAbortDelay == 1 )
		uint8_t ucDelayAborted;
	#endif

} tskTCB;

static void DisplayTask(tskTCB* tcb, eTaskState state, uint32_t runTime)
{
	if (tcb == NULL) {
		return;
	}
#if ( configUSE_TRACE_FACILITY == 1 )
	int uxTCBNumber = tcb->uxTCBNumber;
#else
	int uxTCBNumber = 0;
#endif
#if ( configGENERATE_RUN_TIME_STATS == 1 )
	uint32_t ulRunTimeCounter = tcb->ulRunTimeCounter;
	double rate = 0;
	if (runTime != 0) {
		rate = (double)(ulRunTimeCounter * 100)/(double)runTime;
	}
#else
	int ulRunTimeCounter = 0;
	double rate = 0;
#endif

	char*   pcState = "UNKNOWN  ";
	switch (state) {
		case eRunning:
			pcState = "RUNNING  ";
			break;
		case eReady:
			pcState = "READY    ";
			break;
		case eBlocked:
			pcState = "BLOCKED  ";
			break;
		case eSuspended:
			pcState = "SUSPENDED";
			break;
		case eDeleted:
			pcState = "DELETED  ";
			break;
		default:
			break;
	}

	tfp_printf("%02d %7s 0x%08x %d %s 0x%08x 0x%08x %04d 0x%08x %d%%\r\n",
		uxTCBNumber,
		tcb->pcTaskName,
		(unsigned int)tcb,
		(int)tcb->uxPriority,
		pcState,
		(unsigned int)tcb->pxStack,
		(unsigned int)tcb->pxTopOfStack,
		(tcb->pxTopOfStack-tcb->pxStack)*4,
		tcb->xEventListItem.pvContainer == NULL ? 0 : (unsigned int)tcb->xEventListItem.pvContainer-36,
		(int)rate
		);
}

static void DisplayTaskInfo()
{
	tfp_printf("FreeRTOS Tasks\r\n");
	uint32_t runTime = 0;

	// get the total runtime counter
#if ( configUSE_TRACE_FACILITY == 1 )
	TaskStatus_t xTaskStatusArray[10] = {0};
	uxTaskGetSystemState((TaskStatus_t *)&xTaskStatusArray, sizeof(task)/sizeof(task[0]), &runTime);
#endif

	// display all the application created tasks
	int numTask = sizeof(task)/sizeof(task[0]);
	for (int i=0; i<numTask; i++) {
		if (task[i] != NULL) {
#if( ( INCLUDE_eTaskGetState == 1 ) || ( configUSE_TRACE_FACILITY == 1 ) )
			eTaskState state = eTaskGetState(task[i]);
#else
			eTaskState state = eInvalid;
#endif
			DisplayTask(task[i], state, runTime);
		}
	}

	// display the idle task
#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
	TaskHandle_t idleTask = xTaskGetIdleTaskHandle();
	if (idleTask != NULL) {
#if( ( INCLUDE_eTaskGetState == 1 ) || ( configUSE_TRACE_FACILITY == 1 ) )
		eTaskState state = eTaskGetState(idleTask);
#else
		eTaskState state = eInvalid;
#endif
		DisplayTask(idleTask, state, runTime);
	}
#endif

#if (configUSE_TIMERS == 1)
	// display the timer service task
	TaskHandle_t timerTask = xTimerGetTimerDaemonTaskHandle();
	if (timerTask != NULL) {
#if( ( INCLUDE_eTaskGetState == 1 ) || ( configUSE_TRACE_FACILITY == 1 ) )
		eTaskState state = eTaskGetState(timerTask);
#else
		eTaskState state = eBlocked;
#endif
		DisplayTask(timerTask, state, runTime);
	}
#endif

	tfp_printf("\r\n");
}



/*
 ***************************************************
 * QUEUE LIST
 ***************************************************
 */
static void DisplayQueueInfo()
{
	typedef struct QueueDefinition
	{
		int8_t *pcHead;					/*< Points to the beginning of the queue storage area. */
		int8_t *pcTail;					/*< Points to the byte at the end of the queue storage area.  Once more byte is allocated than necessary to store the queue items, this is used as a marker. */
		int8_t *pcWriteTo;				/*< Points to the free next place in the storage area. */

		union							/* Use of a union is an exception to the coding standard to ensure two mutually exclusive structure members don't appear simultaneously (wasting RAM). */
		{
			int8_t *pcReadFrom;			/*< Points to the last place that a queued item was read from when the structure is used as a queue. */
			UBaseType_t uxRecursiveCallCount;/*< Maintains a count of the number of times a recursive mutex has been recursively 'taken' when the structure is used as a mutex. */
		} u;

		List_t xTasksWaitingToSend;		/*< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
		List_t xTasksWaitingToReceive;	/*< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */

		volatile UBaseType_t uxMessagesWaiting;/*< The number of items currently in the queue. */
		UBaseType_t uxLength;			/*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
		UBaseType_t uxItemSize;			/*< The size of each items that the queue will hold. */

		volatile int8_t cRxLock;		/*< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
		volatile int8_t cTxLock;		/*< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */

		#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
			uint8_t ucStaticallyAllocated;	/*< Set to pdTRUE if the memory used by the queue was statically allocated to ensure no attempt is made to free the memory. */
		#endif

		#if ( configUSE_QUEUE_SETS == 1 )
			struct QueueDefinition *pxQueueSetContainer;
		#endif

		#if ( configUSE_TRACE_FACILITY == 1 )
			UBaseType_t uxQueueNumber;
			uint8_t ucQueueType;
		#endif

	} xQUEUE;

#if ( configQUEUE_REGISTRY_SIZE > 0 )

	/* The type stored within the queue registry array.  This allows a name
	to be assigned to each queue making kernel aware debugging a little
	more user friendly. */
	typedef struct QUEUE_REGISTRY_ITEM
	{
		const char *pcQueueName; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
		QueueHandle_t xHandle;
	} xQueueRegistryItem;

	/* The old xQueueRegistryItem name is maintained above then typedefed to the
	new xQueueRegistryItem name below to enable the use of older kernel aware
	debuggers. */
	typedef xQueueRegistryItem QueueRegistryItem_t;

	/* The queue registry is simply an array of QueueRegistryItem_t structures.
	The pcQueueName member of a structure being NULL is indicative of the
	array position being vacant. */
	extern QueueRegistryItem_t xQueueRegistry[ configQUEUE_REGISTRY_SIZE ];

#endif /* configQUEUE_REGISTRY_SIZE */

	tfp_printf("FreeRTOS Queues\r\n");
#if ( configQUEUE_REGISTRY_SIZE > 0 )
	for (int i=0; i<configQUEUE_REGISTRY_SIZE; i++) {
		xQUEUE* myQueue = (xQUEUE*)xQueueRegistry[i].xHandle;
		if (myQueue != NULL) {

#if ( configUSE_TRACE_FACILITY == 1 )
			char* ucQueueType = myQueue->ucQueueType == queueQUEUE_TYPE_BASE ? "Queue             " :
				myQueue->ucQueueType == queueQUEUE_TYPE_MUTEX ?                "Mutex             " :
				myQueue->ucQueueType == queueQUEUE_TYPE_COUNTING_SEMAPHORE ?   "Counting Semaphore" :
				myQueue->ucQueueType == queueQUEUE_TYPE_BINARY_SEMAPHORE ?     "Binary Semaphore  " :
				myQueue->ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX ?      "Recursive Mutex   " : "Unknown";
#else
			char* ucQueueType = "Unknown";
#endif
			tfp_printf("%s 0x%08x %d %d %d %d %d %s\r\n",
					xQueueRegistry[i].pcQueueName,
					(unsigned int)myQueue,
					(int)myQueue->xTasksWaitingToSend.uxNumberOfItems,
					(int)myQueue->xTasksWaitingToReceive.uxNumberOfItems,
					(int)myQueue->uxMessagesWaiting,
					(int)myQueue->uxLength,
					(int)myQueue->uxItemSize,
					ucQueueType
					);
		}
	}
#endif
	tfp_printf("\r\n");
}

void TimerCallbackOneShot( TimerHandle_t xTimer ) {
	tfp_printf("timer oneshot\r\n");
}

void TimerCallbackAutoReload( TimerHandle_t xTimer ) {
	tfp_printf("timer autoreload\r\n");
}



/*
 ***************************************************
 * TIMER LIST
 ***************************************************
 */
static void DisplayTimerInfo()
{
	/* The definition of the timers themselves. */
	typedef struct tmrTimerControl
	{
		const char				*pcTimerName;		/*<< Text name.  This is not used by the kernel, it is included simply to make debugging easier. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
		ListItem_t				xTimerListItem;		/*<< Standard linked list item as used by all kernel features for event management. */
		TickType_t				xTimerPeriodInTicks;/*<< How quickly and often the timer expires. */
		UBaseType_t				uxAutoReload;		/*<< Set to pdTRUE if the timer should be automatically restarted once expired.  Set to pdFALSE if the timer is, in effect, a one-shot timer. */
		void 					*pvTimerID;			/*<< An ID to identify the timer.  This allows the timer to be identified when the same callback is used for multiple timers. */
		TimerCallbackFunction_t	pxCallbackFunction;	/*<< The function that will be called when the timer expires. */
		#if( configUSE_TRACE_FACILITY == 1 )
			UBaseType_t			uxTimerNumber;		/*<< An ID assigned by trace tools such as FreeRTOS+Trace */
		#endif

		#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
			uint8_t 			ucStaticallyAllocated; /*<< Set to pdTRUE if the timer was created statically so no attempt is made to free the memory again if the timer is later deleted. */
		#endif
	} xTIMER;

	tfp_printf("FreeRTOS Timers\r\n");
	for (int i=0; i<10; i++) {
		xTIMER* myTimer = (xTIMER*)timer[i];
		if (myTimer != NULL) {
			tfp_printf("0x%08x %s 0x%08x %06d %s %s\r\n",
				(int)myTimer->pvTimerID,
				myTimer->pcTimerName,
				(unsigned int)myTimer,
				(int)myTimer->xTimerPeriodInTicks,
				myTimer->uxAutoReload == 0 ? "Off " : "Auto",
				myTimer->pxCallbackFunction == TimerCallbackOneShot ?
					"TimerCallbackOneShot" : "TimerCallbackAutoReload"
				);
		}
	}
	tfp_printf("\r\n");
}



/*
 ***************************************************
 * HEAP USAGE
 * Heap 1/2/4/5 : Uses pvPortGetHeapStart() and pvPortGetHeapEnd()
 * Heap 2       : ucHeap, xStart and xEnd should not be static
 ***************************************************
 */
void DisplayHeapInfo()
{
	typedef struct A_BLOCK_LINK
	{
		struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
		size_t xBlockSize;						/*<< The size of the free block. */
	} BlockLink_t;


	tfp_printf("FreeRTOS Heap Usage\r\n");

	//if (id != NULL) {
	//	tfp_printf(id);
	//}

	unsigned int totalBytes = 0;

#if (FT32_PORT_HEAP == 3)
	return;
#endif

	// Note: pvPortGetHeapStart and pvPortGetHeapEnd have been added in Heap 1/2/4/5
	extern void* pvPortGetHeapStart();
	extern void* pvPortGetHeapEnd();

#if (FT32_PORT_HEAP == 1)
	int heapCurrSize = pvPortGetHeapEnd() - pvPortGetHeapStart();
	tfp_printf("Heap 1 0x%08x 0x%08x %d %d\r\n",
		pvPortGetHeapStart(),
		pvPortGetHeapEnd(),
		heapCurrSize,
		0
	);
	totalBytes += heapCurrSize;

	tfp_printf("Heap 1 0x%08x 0x%08x %d %d\r\n",
		pvPortGetHeapEnd(),
		pvPortGetHeapStart() + configTOTAL_HEAP_SIZE,
		configTOTAL_HEAP_SIZE - heapCurrSize,
		configTOTAL_HEAP_SIZE - heapCurrSize
	);
	totalBytes += configTOTAL_HEAP_SIZE - heapCurrSize;
#elif (FT32_PORT_HEAP == 2)
	for (BlockLink_t* ptr = (BlockLink_t*)pvPortGetHeapStart();
		ptr != (BlockLink_t*)pvPortGetHeapEnd();
		ptr = ptr->pxNextFreeBlock)
	{
		unsigned int size = ptr->pxNextFreeBlock > 0 ?
			(int)ptr->pxNextFreeBlock - (int)ptr : 0;
		tfp_printf("Heap 2 0x%08x 0x%08x %d %d\r\n",
			ptr,
			ptr->pxNextFreeBlock,
			size,
			ptr->xBlockSize
		);
		totalBytes += size;
		if (!ptr->pxNextFreeBlock) {
			break;
		}
	}
#elif (FT32_PORT_HEAP == 4 || FT32_PORT_HEAP == 5)
#if (FT32_PORT_HEAP == 5)
	static int init = 0;
	if (!init) {
		HeapRegion_t xHeapRegions;
		xHeapRegions.pucStartAddress = 0x00001000;
		xHeapRegions.xSizeInBytes = configTOTAL_HEAP_SIZE;
		vPortDefineHeapRegions(&xHeapRegions);
		init = 1;
	}
#endif // (FT32_PORT_HEAP == 5)
	for (BlockLink_t* ptr = (BlockLink_t*)pvPortGetHeapStart();
		ptr!=(BlockLink_t*)pvPortGetHeapEnd();
		ptr = ptr->pxNextFreeBlock)
	{
		if (ptr->pxNextFreeBlock != NULL) {
			unsigned int size = (int)ptr->pxNextFreeBlock - (int)ptr;
			tfp_printf("Heap %d 0x%08x 0x%08x %05d %05d\r\n",
				FT32_PORT_HEAP,
				(unsigned int)ptr,
				(unsigned int)ptr->pxNextFreeBlock,
				size,
				ptr->xBlockSize
			);
			totalBytes += size;
		}
	}
#endif

	tfp_printf("TotalBytes %d\r\n", totalBytes);
	tfp_printf("\r\n");
}



static int deleteQueue(int index)
{
	if (index < sizeof(queue)/sizeof(queue[0])) {
		if (queue[index] != NULL) {
			//tfp_printf("Delete queue %d\r\n", index);
			vQueueDelete(queue[index]);
			queue[index] = NULL;
			return 1;
		}
	}
	return 0;
}

static int deleteTask(int index)
{
#if ( INCLUDE_vTaskDelete == 1 )
	if (index < sizeof(task)/sizeof(task[0])) {
		if (task[index] != NULL) {
			tfp_printf("Delete task %d\r\n", index);
			vTaskDelete(task[index]);
			task[index] = NULL;
			return 1;
		}
	}
#endif
	return 0;
}

static int createTask()
{
#ifdef __FT930__
	static int index = MAX_TASK - 1;
#else
	static int index = MAX_TASK - 3;
#endif
	if (index < MAX_TASK) {
		char* taskName = "Print ";
		taskName[strlen(taskName)-1] = '0'+ index++;
		tfp_printf("Create task %s\r\n", taskName);
		xTaskCreate(prvPrintTask, taskName, 1024,
				"PrintX pppppppppppppppppppppppppppppppppppppppppp\r\n", 1, &task[index]);
		return 1;
	}
	return 0;
}

static void prvNewPrintString(portCHAR *pcString)
{
	int delay = portMAX_DELAY;

	// Add a breakpoint on xSemaphoreTake or xSemaphoreGive
	xSemaphoreTake(xMutex, delay);

	tfp_printf("\r\n%s\r\n", pcString);
	DisplayTaskInfo();
	DisplayQueueInfo();
	DisplayTimerInfo();
	DisplayHeapInfo();

	// Add a breakpoint on xSemaphoreTake or xSemaphoreGive
	xSemaphoreGive(xMutex);
}

static void deleteResource(char *pcStringToPrint)
{
	// Delete tasks
	if (memcmp(pcStringToPrint, "Print0", strlen("Print0"))==0) {
		deleteTask(0);
	}
	else if (memcmp(pcStringToPrint, "Print1", strlen("Print1"))==0) {
		deleteTask(1);
	}
	// Create tasks
	else if (memcmp(pcStringToPrint, "Print2", strlen("Print2"))==0) {
		createTask();
	}
	else if (memcmp(pcStringToPrint, "Print3", strlen("Print3"))==0) {
		createTask();
	}
	// Delete queue
	else if (memcmp(pcStringToPrint, "Print4", strlen("Print4"))==0) {
		deleteQueue(0);
	}
#ifndef __FT930__
	else if (memcmp(pcStringToPrint, "Print5", strlen("Print5"))==0) {
		deleteQueue(4);
	}
	else if (memcmp(pcStringToPrint, "Print6", strlen("Print6"))==0) {
		deleteQueue(7);
	}
#endif // __FT930__
}

static void prvPrintTask(void *pvParameters)
{
	char *pcStringToPrint;
	int max = 0x1FF;
	int random = 0;

	pcStringToPrint = (char *)pvParameters;
	for(int i=0; ; i++)
	{
		random = rand() & max;
		prvNewPrintString(pcStringToPrint);
#if ( INCLUDE_vTaskDelay == 1 )
		vTaskDelay(random);
#endif
		deleteResource(pcStringToPrint);
	}
}



static void frt_demo_setup(void)
{
	srand(567);

	DisplayHeapInfo("xxx 1\r\n");

	// Create the tasks
#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	xMutex = xSemaphoreCreateMutex();
#endif
	if (xMutex != NULL)
	{
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
		uint16_t stackDepth = 1024; // equivalent to 1024*4

		xTaskCreate(prvPrintTask, "Print0", stackDepth,
		"Print0 oooooooooooooooooooooooooooooooooooooooooo\r\n", 1, &task[0]);
		taskCount++;

		xTaskCreate(prvPrintTask, "Print1", stackDepth,
		"Print1 ******************************************\r\n", 1, &task[1]);
		taskCount++;

		xTaskCreate(prvPrintTask, "Print2", stackDepth,
		"Print2 ------------------------------------------\r\n", 2, &task[2]);
		taskCount++;

		xTaskCreate(prvPrintTask, "Print3", stackDepth,
		"Print3 ##########################################\r\n", 3, &task[3]);
		taskCount++;

#ifndef __FT930__
		xTaskCreate(prvPrintTask, "Print4", stackDepth,
		"Print4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n", 4, &task[4]);
		taskCount++;

		xTaskCreate(prvPrintTask, "Print5", stackDepth,
		"Print5 ++++++++++++++++++++++++++++++++++++++++++\r\n", 5, &task[5]);
		taskCount++;

		xTaskCreate(prvPrintTask, "Print6", stackDepth,
		"Print6 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n", 6, &task[6]);
		taskCount++;
#endif // __FT930__
#endif
	}

	DisplayHeapInfo("xxx 2\r\n");

	// Create and register the queues
	{
#ifdef __FT930__
		// Create the queue sets
		queue[0] = xQueueCreate(1, 5);
		queue[1] = xQueueCreate(2, 7);

		// Create the counting semaphores
#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		queue[2] = xSemaphoreCreateCounting(3, 1);
		queue[3] = xSemaphoreCreateCounting(4, 0);
#endif

		// Create the mutexes
#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		queue[4] = xSemaphoreCreateMutex();
		queue[5] = xSemaphoreCreateMutex();
#endif

#if ( configQUEUE_REGISTRY_SIZE > 0 )
		vQueueAddToRegistry(queue[0], "QU1");
		vQueueAddToRegistry(queue[1], "QU2");
#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		vQueueAddToRegistry(queue[2], "CS1");
		vQueueAddToRegistry(queue[3], "CS2");
#endif
#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		vQueueAddToRegistry(queue[4], "MU1");
		vQueueAddToRegistry(queue[5], "MU2");
#endif
		vQueueAddToRegistry(xMutex, "MUX");
#endif


#else // __FT930__

		// Create the queue sets
		queue[0] = xQueueCreate(1, 5);
		queue[1] = xQueueCreate(2, 7);
		queue[2] = xQueueCreate(3, 9);

		// Create the counting semaphores
#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		queue[3] = xSemaphoreCreateCounting(4, 1);
		queue[4] = xSemaphoreCreateCounting(5, 0);
		queue[5] = xSemaphoreCreateCounting(6, 1);
#endif

		// Create the mutexes
#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		queue[6] = xSemaphoreCreateMutex();
		queue[7] = xSemaphoreCreateMutex();
		queue[8] = xSemaphoreCreateMutex();
#endif

		// Note: only registered queues can be viewed in the FreeRTOS Queue List view
#if ( configQUEUE_REGISTRY_SIZE > 0 )
		vQueueAddToRegistry(queue[0], "QU1");
		vQueueAddToRegistry(queue[1], "QU2");
		vQueueAddToRegistry(queue[2], "QU3");
#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		vQueueAddToRegistry(queue[3], "CS1");
		vQueueAddToRegistry(queue[4], "CS2");
		vQueueAddToRegistry(queue[5], "CS3");
#endif
#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		vQueueAddToRegistry(queue[6], "MU1");
		vQueueAddToRegistry(queue[7], "MU2");
		vQueueAddToRegistry(queue[8], "MU3");
#endif
		vQueueAddToRegistry(xMutex, "MUX");
#endif


#endif // __FT930__
	}

	DisplayHeapInfo("xxx 3\r\n");

	// Create the timers
#if( (configTIMER_QUEUE_LENGTH > 0) && ( configUSE_TIMERS == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	{
		timer[0] = xTimerCreate("OneShotTmr1", pdMS_TO_TICKS(1000),   pdFALSE, (void*)1, TimerCallbackOneShot);
		timer[1] = xTimerCreate("OneShotTmr2", pdMS_TO_TICKS(10000),  pdFALSE, (void*)2, TimerCallbackOneShot);
		timer[2] = xTimerCreate("OneShotTmr3", pdMS_TO_TICKS(30000),  pdFALSE, (void*)3, TimerCallbackOneShot);
		timer[3] = xTimerCreate("AutoReload1", pdMS_TO_TICKS(5000),   pdTRUE,  (void*)4, TimerCallbackAutoReload);
		timer[4] = xTimerCreate("AutoReload2", pdMS_TO_TICKS(100000), pdTRUE,  (void*)5, TimerCallbackAutoReload);
		timer[5] = xTimerCreate("AutoReload3", pdMS_TO_TICKS(300000), pdTRUE,  (void*)6, TimerCallbackAutoReload);
		for (int i=0; i<MAX_TIMER; i++) {
			xTimerStart(timer[i],  0);
		}
	}
#endif

	DisplayHeapInfo("xxx 4\r\n");

	// Start the scheduler so the created tasks start executing.
	vTaskStartScheduler();
}


