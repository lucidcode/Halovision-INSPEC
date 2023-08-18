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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/
#ifdef __TFP_PRINTF__
#include "tinyprintf.h"
#endif

#define DPrintf	tfp_printf
/* Tick timer configuration. */
#define MHz                                         (1000000)
#define SYSCLK                                      ((unsigned long)100*MHz)
#define configTICK_TIMER_CHANNEL                    (timer_select_a)
#define configTICK_TIMER_PRESCALER                  (50)
#define configCPU_CLOCK_HZ                          ((unsigned long)(SYSCLK / configTICK_TIMER_PRESCALER))

/* These are used in porting code (Source\portable). */
#define configTICK_RATE_HZ                          ((TickType_t)1000)
#if defined(__FT900__)
#define configTOTAL_HEAP_SIZE                       ((size_t)(50 * 1024))
#elif defined(__FT930__)
#define configTOTAL_HEAP_SIZE                       ((size_t)(26 * 1024))
#endif
#define configUSE_MALLOC_FAILED_HOOK                0

/* These are defined in FreeRTOS.h, mandatory. */
#define configMINIMAL_STACK_SIZE                    ((unsigned short)2*128)
#define configMAX_PRIORITIES                        10
#define configUSE_PREEMPTION                        1
#define configUSE_IDLE_HOOK                         0
#define configUSE_TICK_HOOK                         0
#define configUSE_16_BIT_TICKS                      0

/* These are defined in FreeRTOS.h, default 0 when !defined. */
#define configUSE_APPLICATION_TASK_TAG              0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS     0
#define configUSE_RECURSIVE_MUTEXES                 0
#define configUSE_MUTEXES                           1
#define configUSE_COUNTING_SEMAPHORES               1
#define configUSE_ALTERNATIVE_API                   0
#define configUSE_TIMERS                            1
#define configQUEUE_REGISTRY_SIZE                   11
#define configCHECK_FOR_STACK_OVERFLOW              2       //0, 1, or 2
#define configUSE_QUEUE_SETS                        0
#define configUSE_NEWLIB_REENTRANT                  0
#define portTICK_TYPE_IS_ATOMIC                     0
#define portCRITICAL_NESTING_IN_TCB					1

/* These are defined in FreeRTOS.h, default 0 when !defined. For debug use. */
#define configGENERATE_RUN_TIME_STATS               1
#define configUSE_STATS_FORMATTING_FUNCTIONS        0
#define configUSE_TRACE_FACILITY                    1

 #define configSUPPORT_STATIC_ALLOCATION            0
 #define configSUPPORT_DYNAMIC_ALLOCATION           1

/* These are defined in FreeRTOS.h, default non-zero when !defined. */
#define configMAX_TASK_NAME_LEN                     (16)    //Default 16
#define configIDLE_SHOULD_YIELD                     1       //Default 1
#define configUSE_TIME_SLICING                      1       //Default 1
#define configUSE_TASK_NOTIFICATIONS                1       //Default 1
#define configENABLE_BACKWARD_COMPATIBILITY         0       //Default 1

/* These must be defined when configUSE_TIMERS==1. */
#if 1 //(configUSE_TIMERS == 1)
#define configTIMER_TASK_PRIORITY                   (configMAX_PRIORITIES)
#define configTIMER_QUEUE_LENGTH                    (10)
#define configTIMER_TASK_STACK_DEPTH                (256)
#endif

#ifdef DEBUG
#define configASSERT(x)                                                 \
    {                                                                   \
        if((int)(x) == 0) {                                             \
            DPrintf("%s() #%x\r\n", __FUNCTION__, __LINE__);            \
        }                                                               \
    }
#else
#define configASSERT(x...) (void)(x)
#endif


/* Set the following definitions to 1 to include the API function, or zero to exclude the API function. */
/* Mandatory. */
#define INCLUDE_vTaskPrioritySet                    0
#define INCLUDE_uxTaskPriorityGet                   0
#define INCLUDE_vTaskDelete                         1
#define INCLUDE_vTaskSuspend                        1
#define INCLUDE_vTaskDelayUntil                     0
#define INCLUDE_vTaskDelay                          1

/* Optional, default to 0 when !defined. */
#define INCLUDE_xTaskGetIdleTaskHandle              1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle      1
#define INCLUDE_xQueueGetMutexHolder                0
#define INCLUDE_xSemaphoreGetMutexHolder            0
#define INCLUDE_pcTaskGetTaskName                   0
#define INCLUDE_uxTaskGetStackHighWaterMark         0
#define INCLUDE_eTaskGetState                       0
#define INCLUDE_xTaskResumeFromISR                  0
#define INCLUDE_xEventGroupSetBitFromISR            0
#define INCLUDE_xTimerPendFunctionCall              0
#define INCLUDE_xTaskGetSchedulerState              0
#define INCLUDE_xTaskGetCurrentTaskHandle           0
#define INCLUDE_vTaskCleanUpResources               1

/* Trace. */
//#define traceSTART()                                /* Available only for FreeRTOS+Trace? */
//#define traceEND()                                  /* Available only for FreeRTOS+Trace? */
//#define traceTASK_PRIORITY_INHERIT(pxTCBOfMutexHolder, uxInheritedPriority)     /* TODO */
//#define traceTASK_PRIORITY_DISINHERIT(pxTCBOfMutexHolder, uxOriginalPriority)   /* TODO */
//#define traceBLOCKING_ON_QUEUE_RECEIVE(pxQueue)     /* TODO */
//#define traceBLOCKING_ON_QUEUE_SEND(pxQueue)        /* TODO */

//#define traceTASK_SWITCHED_IN()                     portTASK_SWITCHED('I')
//#define traceTASK_SWITCHED_OUT()                    portTASK_SWITCHED('O')
#define traceTASK_CREATE                            portTASK_CREATE
//#define traceTASK_CREATE_FAILED                     portTASK_CREATE_FAILED
//#define traceTASK_DELAY                             portTASK_DELAY

#endif /* FREERTOS_CONFIG_H */
