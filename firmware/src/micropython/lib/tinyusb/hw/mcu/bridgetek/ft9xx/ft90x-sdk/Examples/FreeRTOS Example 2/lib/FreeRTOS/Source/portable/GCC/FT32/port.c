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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the FT32 port.
 *----------------------------------------------------------*/

/* Standard includes. */
//#include "stdio.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Hardware includes. */
#include "ft900.h"
#include "tinyprintf.h"

#define WATCHDOG_ENABLE		0

#if 0
#define WDOG_TIMEOUT                            (wdt_counter_256M_clocks)   //2sec
#elif 0
#define WDOG_TIMEOUT                            (wdt_counter_512M_clocks)   //5sec
#else
#define WDOG_TIMEOUT                            (wdt_counter_2G_clocks)     //21sec
#endif

/* Debug. */
#define DPRINTF                                 DPrintf
#if 0
#define LOOP_ON_ERROR()                         while(1)
#else
#define LOOP_ON_ERROR()                         while(0)
#endif

/* Setup the TB to generate the tick interrupts. */
static void prvSetupTimerInterrupt(void);

void vPortStart(void);
#define portRESTORE_CONTEXT()                   vPortStart()

volatile uint32_t IsInISR;

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 */
portSTACK_TYPE *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
{
    int i;
    pxTopOfStack -= portCONTEXT_SIZE;

#if 0
    //kahho:: for reference; fully unrolled.
    //Bottom of stack (high address)
    pxTopOfStack[..] = (StackType_t)0xdeadbeef;     //Guard zone, if corrupted, it indicates very likely that OTHER task(s) has overflow its stack
    pxTopOfStack[32] = (StackType_t)pxCode;         //RETURN
    pxTopOfStack[31] = (StackType_t)0;              //R30
    pxTopOfStack[30] = (StackType_t)pvParameters;   //R0
    pxTopOfStack[29] = (StackType_t)0;              //R1
    pxTopOfStack[28] = (StackType_t)0;              //R2
    pxTopOfStack[27] = (StackType_t)0;              //R3
    pxTopOfStack[26] = (StackType_t)0;              //R4
    pxTopOfStack[25] = (StackType_t)0;              //R5
    pxTopOfStack[24] = (StackType_t)0;              //R6
    pxTopOfStack[23] = (StackType_t)0;              //R7
    pxTopOfStack[22] = (StackType_t)0;              //R8
    pxTopOfStack[21] = (StackType_t)0;              //R9
    pxTopOfStack[20] = (StackType_t)0;              //R10
    pxTopOfStack[19] = (StackType_t)0;              //R11
    pxTopOfStack[18] = (StackType_t)0;              //R12
    pxTopOfStack[17] = (StackType_t)0;              //R13
    pxTopOfStack[16] = (StackType_t)0;              //R14
    pxTopOfStack[15] = (StackType_t)0;              //R15
    pxTopOfStack[14] = (StackType_t)0;              //R16
    pxTopOfStack[13] = (StackType_t)0;              //R17
    pxTopOfStack[12] = (StackType_t)0;              //R18
    pxTopOfStack[11] = (StackType_t)0;              //R19
    pxTopOfStack[10] = (StackType_t)0;               //R20
    pxTopOfStack[9] = (StackType_t)0;               //R21
    pxTopOfStack[8] = (StackType_t)0;               //R22
    pxTopOfStack[7] = (StackType_t)0;               //R23
    pxTopOfStack[6] = (StackType_t)0;               //R24
    pxTopOfStack[5] = (StackType_t)0;               //R25
    pxTopOfStack[4] = (StackType_t)0;               //R26
    pxTopOfStack[3] = (StackType_t)0;               //R27
    pxTopOfStack[2] = (StackType_t)0;               //R28
    pxTopOfStack[1] = (StackType_t)0;               //R29
    pxTopOfStack[0] = (StackType_t)0;               //task interrupt state
    //Top of stack (low address)
#else
    for(i=0; i<=portCONTEXT_SIZE; i++)
    {
        pxTopOfStack[i] = (StackType_t)i;
    }

    pxTopOfStack[0] = (StackType_t)IRQ_UNMASK_ALL;      //Default Interrupt state.
    pxTopOfStack[1] = (StackType_t)0;                   //R29
    pxTopOfStack[2] = (StackType_t)0;                   //R28
    pxTopOfStack[31] = (StackType_t)0;                  //R30
#endif

    /* When the task starts is will expect to find the function parameter in R0. */
    pxTopOfStack[30] = (StackType_t)pvParameters;       //R0

    /* First on the stack is the return address - which in this case is the start of the task. */
    pxTopOfStack[32] = (StackType_t)pxCode;             //RETURN

    /* Guard zone. For debug. */
    for(i=33; i<portCONTEXT_SIZE; i++)
    {
        pxTopOfStack[i] = (StackType_t)0xdeadbeef;
    }
    //pxTopOfStack[portCONTEXT_SIZE];

    return pxTopOfStack;
}

portBASE_TYPE xPortStartScheduler(void)
{
    /* Setup the hardware to generate the tick. */
    prvSetupTimerInterrupt();

    timer_start(configTICK_TIMER_CHANNEL);
#if WATCHDOG_ENABLE
    wdt_init(WDOG_TIMEOUT);
#endif

    /* Restore the context of the first task that is going to run. This will also enable interrtups */
    portRESTORE_CONTEXT();

    /* Should never reach here. */
    DPRINTF("xPortStartScheduler(): bad state\r\n");
    return pdTRUE;
}

void vPortEndScheduler(void)
{
    /* Nothing to do here */
}

int call_isr(void (*isr)())
{
    extern void* pxCurrentTCB;
    void* prevTCB = pxCurrentTCB;

    __asm__("" ::: "memory");
    isr();
    __asm__("" ::: "memory");

    return(pxCurrentTCB != prevTCB);
}

/*
 * Timer interrupt handler.
 *
 * \note
 *  For any ISR that yields (switch context), it has to call
 *  portYIELD_FROM_ISR().  This can be done anywhere in the ISR.
 *
 * \note
 *  FT90X has 4 GPTs sharing the same IRQ.
 */
#define TIMER_IRQ_PRIORITY  (17)
#define TIMER_COUNT_MAX     (configCPU_CLOCK_HZ * portTICK_PERIOD_MS / 1000)

/* For profiler. */
static volatile uint32_t ulTimerWrapCount;

void prvPortTimerISR(void)
{
    if(timer_is_interrupted(configTICK_TIMER_CHANNEL) == 1)
    {
        wdt_kick();
        ulTimerWrapCount++;
        if(xTaskIncrementTick())
        {
            //portYIELD_FROM_ISR();
            //vPortDisableInterrupts();
            vTaskSwitchContext();
        }
    }
    else
    {
        /* Other timers handling if necessary. */
        /* Bad state for now. */
        DPRINTF("prvPortTimerISR(): bad state\r\n");
        while(1);
    }
    return;
}

static void prvSetupTimerInterrupt(void)
{
    sys_enable(sys_device_timer_wdt);

    /* FT900 Rev A and B have timers that share the prescaler */
    /* FT900 Rev C and FT93x timers have dedicated prescalers */
    timer_prescaler(configTICK_TIMER_CHANNEL, configTICK_TIMER_PRESCALER);

    /* Setup timer to be continuous up-counting. */
    timer_init(configTICK_TIMER_CHANNEL, TIMER_COUNT_MAX, timer_direction_up, timer_prescaler_select_on, timer_mode_continuous);
    ulTimerWrapCount = 0;

    /* Register IRQ handler.
     *
     * \note
     *  Order of timer_enable_interupt() and interrupt_attach()
     *  invocation must be in this ordering.
     */
    interrupt_disable_nesting();
    interrupt_attach(interrupt_timers, TIMER_IRQ_PRIORITY, prvPortTimerISR);
    timer_enable_interrupt(configTICK_TIMER_CHANNEL);
}

/*
 * Run time stats APIs.
 */
void vPortProfilerStart(void)
{
    /* Nothing specific needs to be done here. */
}

/*
 *  \note
 *  There is a possibility that Timer_ISR occurs in the middle of this
 *  function, and ulTimerWrapCount is not yet updated (depending on
 *  how the asm is generated).  In such case, the error is TIMER_COUNT_MAX.
 */
uint32_t ulPortProfilerCounterGet(void)
{
    uint16_t k;
    uint32_t current_count;

    timer_read(configTICK_TIMER_CHANNEL, &k);
    current_count = ulTimerWrapCount * TIMER_COUNT_MAX + k;

    return current_count;
}

/*
 * Get current systick count.
 *
 * \note
 * This gives same result as xTaskGetTickCount(),
 * but without mutex complication as modification is done by IRQ handler.
 */
inline uint32_t ulPortSysTickGet(void)
{
    return ulTimerWrapCount;
}


void vPortSetInterrupts(int interrupts)
{
    portIRQ_SET(interrupts);
}

int vPortGetInterrupts(void)
{
    return portIRQ_GET();
}

void vPortDisableInterrupts(void)
{
    //INTERRUPT->IRQ_CTRL |= MASK_IRQ_CTRL_MASKALL;
    portIRQ_SET_MASK_ALL();                          //Faster; less instructions.
}

void vPortEnableInterrupts(void)
{
    //INTERRUPT->IRQ_CTRL &= ~(MASK_IRQ_CTRL_MASKALL);
    portIRQ_SET_UNMASK_ALL();                        //Faster; less instructions.
}

uint32_t uPortIsInISR(void){
	return IsInISR;
}
