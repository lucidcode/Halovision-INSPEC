/*
    FreeRTOS V8.2.2 - Copyright (C) 2015 Real Time Engineers Ltd.
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

#if (defined(VFW_PORT) && defined(FT32_PORT_HEAP) && (FT32_PORT_HEAP == 1))
#warning HEAP METHOD 1 used
/*
 * The simplest possible implementation of pvPortMalloc().  Note that this
 * implementation does NOT allow allocated memory to be freed again.
 *
 * See heap_2.c, heap_3.c and heap_4.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */
#include <ft900.h>
#include "kernel.h"
#include <stdlib.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* A few bytes might be lost to byte aligning the heap start address. */
#define configADJUSTED_HEAP_SIZE	( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )

/* Allocate the memory for the heap. */
static uint8_t KernelHeap[ configTOTAL_HEAP_SIZE ];
static volatile uint8_t* ucHeap = KernelHeap;
static size_t xNextFreeByte[MAX_HEAP_TYPES];
static uint8_t *pucAlignedHeap[MAX_HEAP_TYPES];

#define MAX_USER_HEAP_SIZE		(20 * 1024UL)
static bool UserHeapInitialized;
static uint8_t CurHeapIdx;


/*-----------------------------------------------------------*/

void *pvPortMalloc( size_t xWantedSize )
{
void *pvReturn = NULL;


	/* Ensure that blocks are always aligned to the required number of bytes. */
	#if portBYTE_ALIGNMENT != 1
		if( xWantedSize & portBYTE_ALIGNMENT_MASK )
		{
			/* Byte alignment required. */
			xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
		}
	#endif


	vTaskSuspendAll();
	kernel_do_on_malloc_entry();

    printf("----------------\n");
    printf("xWantedSize: %u\n", xWantedSize);
    printf("xNextFreeByte: %04X\n", xNextFreeByte);
    printf("pucAlignedHeap =%08X\n", ((uint32_t)pucAlignedHeap[CurHeapIdx]));
    printf("ucHeap =%08X\n", ((uint32_t)&ucHeap[ portBYTE_ALIGNMENT ] ));

	{
		if( pucAlignedHeap[CurHeapIdx] == NULL )
		{
			/* Ensure the heap starts on a correctly aligned boundary. */
			pucAlignedHeap[CurHeapIdx] = ( uint8_t * ) ( ( ( portPOINTER_SIZE_TYPE ) &ucHeap[ portBYTE_ALIGNMENT ] ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
		}

		/* Check there is enough room left for the allocation. */
		if( ( ( xNextFreeByte[CurHeapIdx] + xWantedSize ) < configADJUSTED_HEAP_SIZE ) &&
			( ( xNextFreeByte[CurHeapIdx] + xWantedSize ) > xNextFreeByte[CurHeapIdx] )	)/* Check for overflow. */
		{
			/* Return the next free byte then increment the index past this
			block. */
			pvReturn = pucAlignedHeap[CurHeapIdx] + xNextFreeByte[CurHeapIdx];
			xNextFreeByte[CurHeapIdx] += xWantedSize;
            
            
            printf("xNextFreeByte:  %04X\n", xNextFreeByte[CurHeapIdx]);
            printf("pvReturn =%08X\n", ((uint32_t)pvReturn));

		}

		traceMALLOC( pvReturn, xWantedSize );
	}
            printf("++++++++++++++\n");

	( void ) xTaskResumeAll();

	#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if( pvReturn == NULL )
		{
			extern void vApplicationMallocFailedHook( void );
			vApplicationMallocFailedHook();
		}
	}
	#endif

	return pvReturn;
}
/*-----------------------------------------------------------*/

void vPortFree( void *pv )
{
	/* Memory cannot be freed using this scheme.  See heap_2.c, heap_3.c and
	heap_4.c for alternative implementations, and the memory management pages of
	http://www.FreeRTOS.org for more information. */
	( void ) pv;

	/* Force an assert as it is invalid to call this function. */
	configASSERT( pv == NULL );
}
/*-----------------------------------------------------------*/

void vPortInitialiseBlocks( void )
{
	/* Only required when static memory is not cleared. */
	xNextFreeByte[Kernel_Heap] = ( size_t ) 0;
	xPortResetUserHeap();
}
/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize( void )
{
	return ( configADJUSTED_HEAP_SIZE - xNextFreeByte[Kernel_Heap]  );
}

bool xPortSwitchHeap(e_HeapTypes heap, uint32_t heapStartAdr){
	if(heap == User_Heap){
		if(!UserHeapInitialized){
			// Initialise the user heap
			xNextFreeByte[User_Heap] = 0;
			pucAlignedHeap[User_Heap] = NULL;
		}
		UserHeapInitialized = true;
		CurHeapIdx = (uint8_t)User_Heap;
		ucHeap = (uint8_t*)heapStartAdr;
		printf("switched to user heap!\n");
	}
	else if( heap == Kernel_Heap){
		CurHeapIdx = (uint8_t)Kernel_Heap;
		ucHeap = KernelHeap;
		printf("switched to kernel heap!\n");
	}
}

void xPortResetUserHeap(void)
{
	UserHeapInitialized = false;
}

#endif
