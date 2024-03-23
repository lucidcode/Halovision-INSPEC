/**
 * @file
 * lwIP Operating System abstraction
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
 * @defgroup sys_layer Porting (system abstraction layer)
 * @ingroup lwip
 * @verbinclude "sys_arch.txt"
 *
 * @defgroup sys_os OS abstraction layer
 * @ingroup sys_layer
 * No need to implement functions in this section in NO_SYS mode.
 *
 * @defgroup sys_sem Semaphores
 * @ingroup sys_os
 *
 * @defgroup sys_mutex Mutexes
 * @ingroup sys_os
 * Mutexes are recommended to correctly handle priority inversion,
 * especially if you use LWIP_CORE_LOCKING .
 *
 * @defgroup sys_mbox Mailboxes
 * @ingroup sys_os
 *
 * @defgroup sys_time Time
 * @ingroup sys_layer
 *
 * @defgroup sys_prot Critical sections
 * @ingroup sys_layer
 * Used to protect short regions of code against concurrent access.
 * - Your system is a bare-metal system (probably with an RTOS)
 *   and interrupts are under your control:
 *   Implement this as LockInterrupts() / UnlockInterrupts()
 * - Your system uses an RTOS with deferred interrupt handling from a
 *   worker thread: Implement as a global mutex or lock/unlock scheduler
 * - Your system uses a high-level OS with e.g. POSIX signals:
 *   Implement as a global mutex
 *
 * @defgroup sys_misc Misc
 * @ingroup sys_os
 */

/* Most of the functions defined in sys.h must be implemented in the
 * architecture-dependent file sys_arch.c */

#if !NO_SYS

#include "string.h"

//*****************************************************************************
//
// Include OS functionality.
//
//*****************************************************************************

/* ------------------------ lwIP includes --------------------------------- */
#include "lwip/opt.h"

#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#ifdef FT32_FREERTOS
#include "portmacro.h"  // uPortIsInISR()
#endif // FT32_FREERTOS

/* ------------------------ System architecture includes ----------------------------- */
#include "arch/sys_arch.h"

/* Configuration. */
#define USE_FREERTOS_SEMAPHORE_V822     1

/* Debug. */
#undef DEBUG_ERROR
#undef DEBUG_WARN
#undef DEBUG_INFO

#ifdef DEBUG_ERROR
#define DPRINTF_ERROR(fmt, ...)         DPrintf(fmt, ##__VA_ARGS__)
#else
#define DPRINTF_ERROR(fmt, ...)
#endif

#ifdef DEBUG_WARN
#define DPRINTF_WARN(fmt, ...)          DPrintf(fmt, ##__VA_ARGS__)
#else
#define DPRINTF_WARN(fmt, ...)
#endif

#ifdef DEBUG_INFO
#define DPRINTF_INFO(fmt, ...)          DPrintf(fmt, ##__VA_ARGS__)
#else
#define DPRINTF_INFO(fmt, ...)
#endif

// 13-Jul-16, FIXME, enable FIXME here will result in udiv instruction.
//        There is known issue with Ethernet 100Mbps operation.
#define FIXME_0                         0
#define FIXME_1                         0


#ifdef FT32_FREERTOS
/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_valid
 *---------------------------------------------------------------------------*
 * Description:
 *      Tests the validity of a mailbox
 * Inputs:
 *      sys_mbox_t              -- Handle to mailbox
 * Outputs:
 *      int                     -- Zero for not valid, non-zero for valid
 *---------------------------------------------------------------------------*/
int sys_mbox_valid(sys_mbox_t *mbox)
{
    return (*mbox != NULL)?1:0;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	*mbox = NULL;
}


/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Creates a new mailbox
 * Inputs:
 *      int size                -- Size of elements in the mailbox
 * Outputs:
 *      sys_mbox_t              -- Handle to new mailbox
 *---------------------------------------------------------------------------*/
err_t sys_mbox_new( sys_mbox_t *pxMailBox, int iSize )
{
	*pxMailBox = xQueueCreate( iSize, sizeof( void * ) );
	if( *pxMailBox == NULL )
	{
        DPRINTF_ERROR("sys_mbox_new() failed\n");
        return ERR_MEM;
	}
    DPRINTF_INFO("sys_mbox_new() %x %d\n", (int)pxMailBox, iSize);
	return ERR_OK;

    /* *pxMailBox = mem_malloc(sizeof(sys_mbox_t));
    if (*pxMailBox == NULL)
    	return ERR_MEM;

	*pxMailBox->valid = 0;
	*pxMailBox->queue = xQueueCreate( iSize, sizeof( void * ) );
	if( *pxMailBox->queue == NULL )
	{
        DPRINTF_ERROR("sys_mbox_new() failed\n");
        return ERR_MEM;
	}

	*pxMailBox->valid = 1;
	SYS_STATS_INC_USED( mbox );
	DPRINTF_INFO("sys_mbox_new() done\n");
	return xERR_OK;
	*/
}


/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_free
 *---------------------------------------------------------------------------*
 * Description:
 *      Deallocates a mailbox. If there are messages still present in the
 *      mailbox when the mailbox is deallocated, it is an indication of a
 *      programming error in lwIP and the developer should be notified.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 * Outputs:
 *      sys_mbox_t              -- Handle to new mailbox
 *---------------------------------------------------------------------------*/
void sys_mbox_free( sys_mbox_t *pxMailBox )
{
    DPRINTF_INFO("sys_mbox_free() %x\n", (int)*pxMailBox);

    unsigned long ulMessagesWaiting;

	ulMessagesWaiting = uxQueueMessagesWaiting( *pxMailBox );
	configASSERT( ( ulMessagesWaiting == 0 ) );

	#if SYS_STATS
	{
		if( ulMessagesWaiting != 0UL )
		{
			SYS_STATS_INC( mbox.err );
		}

		SYS_STATS_DEC( mbox.used );
	}
	#endif /* SYS_STATS */

	vQueueDelete( *pxMailBox );
    DPRINTF_INFO("sys_mbox_free()\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_post
 *---------------------------------------------------------------------------*
 * Description:
 *      Post the "msg" to the mailbox.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void *data              -- Pointer to data to post
 *---------------------------------------------------------------------------*/
void sys_mbox_post( sys_mbox_t *pxMailBox, void *pxMessageToPost )
{
    if(uPortIsInISR() == portENTER_ISR)
    {
        DPRINTF_WARN("sys_mbox_post from ISR\n");
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        if(pdPASS != xQueueSendToBackFromISR( *pxMailBox, &pxMessageToPost, &xHigherPriorityTaskWoken ))
        {
            DPRINTF_ERROR("sys_mbox_post failed xQueueSendToBackFromISR\n");
        }
    }
    else
    {
        xQueueSendToBack( *pxMailBox, &pxMessageToPost, portMAX_DELAY );
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_trypost
 *---------------------------------------------------------------------------*
 * Description:
 *      Try to post the "msg" to the mailbox.  Returns immediately with
 *      error if cannot.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void *msg               -- Pointer to data to post
 * Outputs:
 *      err_t                   -- ERR_OK if message posted, else ERR_MEM
 *                                  if not.
 *---------------------------------------------------------------------------*/
err_t sys_mbox_trypost( sys_mbox_t *pxMailBox, void *pxMessageToPost )
{
    err_t xReturn;

	if(uPortIsInISR() == portENTER_ISR)
	{
	    // 08-Mar-16, so far only this branch is hit, from tcpip_input().
	    DPRINTF_INFO("sys_mbox_trypost from ISR\n");
	    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		xReturn = xQueueSendToBackFromISR( *pxMailBox, &pxMessageToPost, &xHigherPriorityTaskWoken );
	}
	else
	{
		xReturn = xQueueSendToBack( *pxMailBox, &pxMessageToPost, 0 );
	}

	if( xReturn == pdPASS )
	{
		xReturn = ERR_OK;
	}
	else
	{
		/* The queue was already full. */
		xReturn = ERR_MEM;
		SYS_STATS_INC( mbox.err );
		DPRINTF_ERROR("sys_mbox_trypost failed in xQueueSendToBack%s\n", uPortIsInISR() == portENTER_ISR ? "FromISR" : "");
	}

	return xReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_mbox_fetch
 *---------------------------------------------------------------------------*
 * Description:
 *      Blocks the thread until a message arrives in the mailbox, but does
 *      not block the thread longer than "timeout" milliseconds (similar to
 *      the sys_arch_sem_wait() function). The "msg" argument is a result
 *      parameter that is set by the function (i.e., by doing "*msg =
 *      ptr"). The "msg" parameter maybe NULL to indicate that the message
 *      should be dropped.
 *
 *      The return values are the same as for the sys_arch_sem_wait() function:
 *      Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
 *      timeout.
 *
 *      Note that a function with a similar name, sys_mbox_fetch(), is
 *      implemented by lwIP.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void **msg              -- Pointer to pointer to msg received
 *      u32_t timeout           -- Number of milliseconds until timeout
 * Outputs:
 *      u32_t                   -- SYS_ARCH_TIMEOUT if timeout, else number
 *                                  of milliseconds until received.
 *---------------------------------------------------------------------------*/
// 14-Jan-16, this is not reentrant-safe (cannot be called in ISR).
u32_t sys_arch_mbox_fetch( sys_mbox_t *pxMailBox, void **ppvBuffer, u32_t ulTimeOut )
{
    void *pvDummy;
    TickType_t xStartTime, xEndTime, xElapsed;
    unsigned long ulReturn;

    // 14-Jan-16, FIXME, should not be happening in ISR, but no harm keeping it.
    //        14-Jan-16, TODO, clean up.
	if(uPortIsInISR() == portENTER_ISR)
	{
	    DPRINTF_WARN("sys_arch_mbox_fetch from ISR\n");
		return 0;
	}

    xStartTime = xTaskGetTickCount();

    if( NULL == ppvBuffer )
    {
        ppvBuffer = &pvDummy;
    }

    if( ulTimeOut != 0UL )
    {
#if FIXME_0 == 1
        if( pdTRUE == xQueueReceive( *pxMailBox, &( *ppvBuffer ), pdMS_TO_TICKS(ulTimeOut) ) )
#else
        if( pdTRUE == xQueueReceive( *pxMailBox, &( *ppvBuffer ), ulTimeOut/ portTICK_PERIOD_MS ) )
#endif
        {
            xEndTime = xTaskGetTickCount();
            xElapsed = ( xEndTime - xStartTime ) * portTICK_PERIOD_MS;

            ulReturn = xElapsed;
        }
        else
        {
            /* Timed out. */
            *ppvBuffer = NULL;
            ulReturn = SYS_ARCH_TIMEOUT;
        }
    }
    else
    {
        while( pdTRUE != xQueueReceive( *pxMailBox, &( *ppvBuffer ), portMAX_DELAY ) );
        xEndTime = xTaskGetTickCount();
        xElapsed = ( xEndTime - xStartTime ) * portTICK_PERIOD_MS;

        if( xElapsed == 0UL )
        {
            xElapsed = 1UL;
        }

        ulReturn = xElapsed;
    }

    return ulReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_mbox_tryfetch
 *---------------------------------------------------------------------------*
 * Description:
 *      Similar to sys_arch_mbox_fetch, but if message is not ready
 *      immediately, we'll return with SYS_MBOX_EMPTY.  On success, 0 is
 *      returned.
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      void **msg              -- Pointer to pointer to msg received
 * Outputs:
 *      u32_t                   -- SYS_MBOX_EMPTY if no messages.  Otherwise,
 *                                  return ERR_OK.
 *---------------------------------------------------------------------------*/
u32_t sys_arch_mbox_tryfetch( sys_mbox_t *pxMailBox, void **ppvBuffer )
{
    void *pvDummy;
    unsigned long ulReturn;
    long lResult;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    if( ppvBuffer== NULL )
    {
        ppvBuffer = &pvDummy;
    }

    // 14-Jan-16, FIXME, should not be happening in ISR, but no harm keeping it.
    //        14-Jan-16, TODO, clean up.
    if(uPortIsInISR() == portENTER_ISR)
    {
        DPRINTF_WARN("sys_arch_mbox_tryfetch from ISR\n");
        lResult = xQueueReceiveFromISR( *pxMailBox, &( *ppvBuffer ), &xHigherPriorityTaskWoken );
    }
    else
    {
        lResult = xQueueReceive( *pxMailBox, &( *ppvBuffer ), 0UL );
    }

    if( lResult == pdPASS )
    {
        ulReturn = ERR_OK;
    }
    else
    {
        DPRINTF_INFO("sys_arch_mbox_tryfetch failed\n");
        ulReturn = SYS_MBOX_EMPTY;
    }

    return ulReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_valid
 *---------------------------------------------------------------------------*
 * Description:
 *      Tests the validity of a semaphore
 * Inputs:
 *      sys_sem_t               -- Handle to semapohore
 * Outputs:
 *      int                     -- Zero for not valid, non-zero for valid
 *---------------------------------------------------------------------------*/
int sys_sem_valid(sys_sem_t *sem)
{
    return (sem != NULL)?1:0;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
	(void)sem;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Creates and returns a new semaphore. The "ucCount" argument specifies
 *      the initial state of the semaphore.
 *      NOTE: Currently this routine only creates counts of 1 or 0
 * Inputs:
 *      sys_mbox_t mbox         -- Handle of mailbox
 *      u8_t ucCount              -- Initial ucCount of semaphore (1 or 0)
 * Outputs:
 *      sys_sem_t               -- Created semaphore or 0 if could not create.
 *---------------------------------------------------------------------------*/
err_t sys_sem_new( sys_sem_t *pxSemaphore, u8_t ucCount )
{
    err_t xReturn = ERR_MEM;
    DPRINTF_INFO("sys_sem_new %x %d\n", (int)pxSemaphore, ucCount);

    /*
     * Note that binary semaphores created using
     * the vSemaphoreCreateBinary() macro are created in a state such that the
     * first call to 'take' the semaphore would pass, whereas binary semaphores
     * created using xSemaphoreCreateBinary() are created in a state such that the
     * the semaphore must first be 'given' before it can be 'taken'.
     */
#if USE_FREERTOS_SEMAPHORE_V822
    *pxSemaphore = xSemaphoreCreateBinary();
#else
    vSemaphoreCreateBinary( ( *pxSemaphore ) );
#endif

    if( *pxSemaphore != NULL )
    {
#if USE_FREERTOS_SEMAPHORE_V822
        if( ucCount == 1U )
        {
            xSemaphoreGive( *pxSemaphore );
        }
#else
        if( ucCount == 0U )
        {
            xSemaphoreTake( *pxSemaphore, 1UL );
        }
#endif

        xReturn = ERR_OK;
        SYS_STATS_INC_USED( sem );
    }
    else
    {
        DPRINTF_ERROR("sys_sem_new failed\n");
        SYS_STATS_INC( sem.err );
    }

    return xReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_sem_wait
 *---------------------------------------------------------------------------*
 * Description:
 *      Blocks the thread while waiting for the semaphore to be
 *      signaled. If the "timeout" argument is non-zero, the thread should
 *      only be blocked for the specified time (measured in
 *      milliseconds).
 *
 *      If the timeout argument is non-zero, the return value is the number of
 *      milliseconds spent waiting for the semaphore to be signaled. If the
 *      semaphore wasn't signaled within the specified time, the return value is
 *      SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
 *      (i.e., it was already signaled), the function may return zero.
 *
 *      Notice that lwIP implements a function with a similar name,
 *      sys_sem_wait(), that uses the sys_arch_sem_wait() function.
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to wait on
 *      u32_t timeout           -- Number of milliseconds until timeout
 * Outputs:
 *      u32_t                   -- Time elapsed or SYS_ARCH_TIMEOUT.
 *---------------------------------------------------------------------------*/
u32_t sys_arch_sem_wait( sys_sem_t *pxSemaphore, u32_t ulTimeout )
{
    // 14-Jan-16, FIXME, should not be happening in ISR, but no harm keeping it.
    //        14-Jan-16, TODO, clean up.
    if(uPortIsInISR() == portENTER_ISR)
    {
        DPRINTF_WARN("sys_arch_sem_wait from ISR\n");
		return 0;
    }

	TickType_t xStartTime, xEndTime, xElapsed;
	unsigned long ulReturn;

    xStartTime = xTaskGetTickCount();

    if( ulTimeout != 0UL )
    {
#if FIXME_1 == 1
        if( xSemaphoreTake( *pxSemaphore, pdMS_TO_TICKS(ulTimeout) ) == pdTRUE )
#else
        if( xSemaphoreTake( *pxSemaphore, ulTimeout / portTICK_PERIOD_MS ) == pdTRUE )
#endif
        {
            xEndTime = xTaskGetTickCount();
            xElapsed = (xEndTime - xStartTime) * portTICK_PERIOD_MS;
            ulReturn = xElapsed;
        }
        else
        {
            DPRINTF_WARN("sys_arch_sem_wait SYS_ARCH_TIMEOUT\n");
            ulReturn = SYS_ARCH_TIMEOUT;
        }
    }
    else
    {
        while( xSemaphoreTake( *pxSemaphore, portMAX_DELAY ) != pdTRUE );
        xEndTime = xTaskGetTickCount();
        xElapsed = ( xEndTime - xStartTime ) * portTICK_PERIOD_MS;

        if( xElapsed == 0UL )
        {
            xElapsed = 1UL;
        }

        ulReturn = xElapsed;
    }

    return ulReturn;
}

#if LWIP_TCPIP_CORE_LOCKING || LWIP_TCPIP_CORE_LOCKING_INPUT
/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new( sys_mutex_t *pxMutex )
{
    err_t xReturn = ERR_MEM;

    *pxMutex = xSemaphoreCreateMutex();

    if( *pxMutex != NULL )
    {
        xReturn = ERR_OK;
        SYS_STATS_INC_USED( mutex );
    }
    else
    {
        SYS_STATS_INC( mutex.err );
    }

    return xReturn;
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock( sys_mutex_t *pxMutex )
{
    if(uPortIsInISR() == portEXIT_ISR)
    {
        if( xSemaphoreTake( *pxMutex, portMAX_DELAY ) != pdPASS )
        {
            DPRINTF_ERROR("sys_mutex_lock failed\n");
        }
    }
    else
    {
        DPRINTF_WARN("sys_mutex_lock from ISR\n");
    }
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *pxMutex )
{
    if(uPortIsInISR() == portEXIT_ISR)
    {
        if(xSemaphoreGive( *pxMutex ) != pdPASS)
        {
            DPRINTF_ERROR("sys_mutex_unlock failed\n");
        }
    }
    else
    {
        DPRINTF_INFO("sys_mutex_unlock from ISR\n");
        BaseType_t xHigherPriorityTaskWoken;
        if( pdTRUE != xSemaphoreGiveFromISR( *pxMutex, &xHigherPriorityTaskWoken ))
        {
            DPRINTF_ERROR("sys_mutex_unlock from ISR failed\n");
        }
    }
}


/** Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free( sys_mutex_t *pxMutex )
{
    SYS_STATS_DEC( mutex.used );
    vQueueDelete( *pxMutex );
}
#endif  //LWIP_TCPIP_CORE_LOCKING || LWIP_TCPIP_CORE_LOCKING_INPUT

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_signal
 *---------------------------------------------------------------------------*
 * Description:
 *      Signals (releases) a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to signal
 *---------------------------------------------------------------------------*/
void sys_sem_signal( sys_sem_t *pxSemaphore )
{
    if(uPortIsInISR() == portEXIT_ISR)
    {
        if(xSemaphoreGive( *pxSemaphore ) != pdTRUE)
        {
            DPRINTF_ERROR("sys_sem_signal failed\n");
        }
    }
    else
    {
        DPRINTF_INFO("sys_sem_signal from ISR\n");
        BaseType_t xHigherPriorityTaskWoken;
        if( pdTRUE != xSemaphoreGiveFromISR( *pxSemaphore, &xHigherPriorityTaskWoken ))
        {
            DPRINTF_ERROR("sys_sem_signal from ISR failed\n");
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_free
 *---------------------------------------------------------------------------*
 * Description:
 *      Deallocates a semaphore
 * Inputs:
 *      sys_sem_t sem           -- Semaphore to free
 *---------------------------------------------------------------------------*/
void sys_sem_free( sys_sem_t *pxSemaphore )
{
    DPRINTF_INFO("sys_sem_free\n");
    SYS_STATS_DEC(sem.used);
    vQueueDelete( *pxSemaphore );
}
#endif // FT32_FREERTOS

/*---------------------------------------------------------------------------*
 * Routine:  sys_init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize sys arch
 *---------------------------------------------------------------------------*/
void sys_init(void)
{
}

#ifdef FT32_FREERTOS
u32_t sys_now(void)
{
    return xTaskGetTickCount();
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_thread_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Starts a new thread with priority "prio" that will begin its
 *      execution in the function "thread()". The "arg" argument will be
 *      passed as an argument to the thread() function. The id of the new
 *      thread is returned. Both the id and the priority are system
 *      dependent.
 * Inputs:
 *      char *name              -- Name of thread
 *      void (* thread)(void *arg) -- Pointer to function to run.
 *      void *arg               -- Argument passed into function
 *      int stacksize           -- Required stack amount in bytes
 *      int prio                -- Thread priority
 * Outputs:
 *      sys_thread_t            -- Pointer to per-thread timeouts.
 *---------------------------------------------------------------------------*/
sys_thread_t sys_thread_new( const char *pcName, void( *pxThread )( void *pvParameters ), void *pvArg, int iStackSize, int iPriority )
{
    TaskHandle_t xCreatedTask;
    portBASE_TYPE xResult;
    sys_thread_t xReturn;

    xResult = xTaskCreate( pxThread, pcName, iStackSize, pvArg, iPriority, &xCreatedTask );

    if( xResult == pdPASS )
    {
        xReturn = xCreatedTask;
    }
    else
    {
        xReturn = NULL;
    }

    return xReturn;
}
#endif // FT32_FREERTOS



/* 0: None, 1: None, 2: taskENTER_CRITICAL(), 3: portIRQ_SET_MASK_ALL(). */
#ifdef FT32_FREERTOS
#define USE_PROTECTION_SCHEME           3
#else
#define USE_PROTECTION_SCHEME           4
#endif

#if USE_PROTECTION_SCHEME == 2
    #define CRITICAL_SECTION_START      vPortEnterCriticalCheckISR
    #define CRITICAL_SECTION_FINISH        vPortExitCriticalCheckISR
#elif USE_PROTECTION_SCHEME == 3
    // Beware that using this pair does NOT support nested interrupt masking.
    //        If nested interrupt masking must be handled, use scheme [2].
    #define CRITICAL_SECTION_START      portIRQ_SET_MASK_ALL
    #define CRITICAL_SECTION_FINISH        portIRQ_RESTORE_MASK_All
#else
    #define CRITICAL_SECTION_START()
    #define CRITICAL_SECTION_FINISH()
#endif

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_protect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      operating system.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
sys_prot_t sys_arch_protect( void )
{
    CRITICAL_SECTION_START();
    return ( sys_prot_t ) 1;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_unprotect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an operating system.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
void sys_arch_unprotect( sys_prot_t xValue )
{
    (void) xValue;
    CRITICAL_SECTION_FINISH();
}

/*
 * Prints an assertion messages and aborts execution.
 */
void sys_assert( const char *pcMessage )
{
    (void) pcMessage;

    for (;;)
    {
    }
}

void *my_mem_calloc(mem_size_t count, mem_size_t size)
{
	int len = count*size;
	void *pmem = mem_clib_malloc(len);
	if (pmem)
	{
		memset(pmem, 0, len);
	}
	return pmem;
}

#endif /* !NO_SYS */

/*-------------------------------------------------------------------------*
 * End of File:  sys_arch.c
 *-------------------------------------------------------------------------*/
