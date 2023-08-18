/**
  @file
  @brief    FreeRTOS port for iHOME.
  @version  1.0
  @date     02-Sep-15
*/
/*
 * =============================================================================
 * Copyright (C) Bridgetek Pte Ltd
 * =============================================================================
 *
 * This source code ("the Software") is provided by Future Technology Devices
 * International Limited ("Bridgetek") subject to the licence terms set out
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
 * =============================================================================
 * Modification History
 * v1.0 @ 02-Sep-15
 * 1. Creation.
 */
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

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* Hardware includes. */
#include <ft900.h>
#include <tinyprintf.h>

#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/*==============================================================================
 * Type definitions.
 *==============================================================================
 */
#define portCHAR                                char
#define portFLOAT                               float
#define portDOUBLE                              double
#define portLONG                                long
#define portSHORT                               short
#define portSTACK_TYPE                          unsigned portLONG
#define portBASE_TYPE                           portLONG

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
    typedef uint16_t TickType_t;
    #define portMAX_DELAY                       ( TickType_t )0xffff
#else
    typedef uint32_t TickType_t;
    #define portMAX_DELAY                       ( TickType_t )0xffffffffUL
#endif

/*==============================================================================
 * Architecture specifics.
 *==============================================================================
 */
#define portSTACK_GROWTH                        ( -1 )
#define portTICK_PERIOD_MS                      ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT                      4
#define portNOP()                               asm volatile ( "move.l $r0, $r0" )


/*==============================================================================
 * Critical section handling.
 *==============================================================================
 */
void vPortDisableInterrupts(void);
void vPortEnableInterrupts(void);
#define portDISABLE_INTERRUPTS                  vPortDisableInterrupts
#define portENABLE_INTERRUPTS                   vPortEnableInterrupts

//kahho:: DO NOT use these for application; used by porting layer.
#define IRQ_MASK_ALL                            (0x80)
#define IRQ_UNMASK_ALL                          (0x0)
#if defined(__FT900__)
#define portIRQ_SET(x)                          (*((volatile uint8_t *)0x100e3) = (x))
#elif defined(__FT930__)
#define portIRQ_SET(x)                          (*((volatile uint8_t *)0x10123) = (x))
#endif
#define portIRQ_SET_MASK_ALL()                  portIRQ_SET(IRQ_MASK_ALL)
#define portIRQ_SET_UNMASK_ALL()                portIRQ_SET(IRQ_UNMASK_ALL)
#if defined(__FT900__)
#define portIRQ_GET()                           (*((volatile uint8_t *)0x100e3))
#elif defined(__FT930__)
#define portIRQ_GET()                           (*((volatile uint8_t *)0x10123))
#endif
#define portIRQ_IS(x)                           (portIRQ_GET() == (x))
#define portIRQ_IS_NOT(x)                       (portIRQ_GET() != (x))

#if 0
/* See vTaskEnterCritical() for more info. */
void vPortAssert_vTaskEnterCriticalInISR(void);
#define portASSERT_IF_IN_ISR                    vPortAssert_vTaskEnterCriticalInISR
#endif

#if 1
#define portENTER_CRITICAL                   portDISABLE_INTERRUPTS
#define portEXIT_CRITICAL                     portENABLE_INTERRUPTS
#endif


/*==============================================================================
 * Kernel utilities.
 *==============================================================================
 */
#define portCONTEXT_SIZE                        (35)

void vPortYield(void);
#define portYIELD()                             vPortYield()

#define portYIELD_FROM_ISR()                    vTaskSwitchContext()
#define taskYIELD_FROM_ISR                      portYIELD_FROM_ISR

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void * pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void * pvParameters )

/* Run time stats macros as described in http://www.freertos.org/rtos-run-time-stats.html */
void vPortProfilerStart(void);
uint32_t ulPortProfilerCounterGet(void);
uint32_t ulPortSysTickGet(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS  vPortProfilerStart
#define portGET_RUN_TIME_COUNTER_VALUE          ulPortProfilerCounterGet


/*==============================================================================
 * Debug utilities.
 *==============================================================================
 */

#define portENTER_ISR                           (0x1122)
#define portEXIT_ISR                            (0x3344)

#define portIRQ_RESTORE_MASK_All()                                  \
    do {                                                            \
            /* Undo the mask all since the state is unmask all. */  \
            portIRQ_SET_UNMASK_ALL();                               \
            portNOP();												\
            portNOP();												\
    } while(0)

uint32_t uPortIsInISR(void);

/* Tracing APIs. */
#if 0
void vPortTaskSwitched(char dir);
#define portTASK_SWITCHED(dir)                  vPortTaskSwitched(dir)
#elif 0
//kahho:: DEBUG_UNEXPECTED_IRQ, enable this to reproduce unexpected IRQ easily.
void vPortTaskSwitched_Short(char dir);
#define portTASK_SWITCHED(dir)                  vPortTaskSwitched_Short(dir)
#else
void vPortTaskSwitched_AssertIrqIssue(void);
#define portTASK_SWITCHED(dir)                  vPortTaskSwitched_AssertIrqIssue()
#endif

void vPortTaskCreate(void *pxNewTCB);
#define portTASK_CREATE                         vPortTaskCreate

void vPortTaskCreateFailed(void);
#define portTASK_CREATE_FAILED                  vPortTaskCreateFailed

void vPortTaskDelay(void);
#define portTASK_DELAY                          vPortTaskDelay

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
