/**
  @file sys_arch.h
 */
/*
 * ============================================================================
 * History
 * =======
 * 2017-03-15 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
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

#ifndef SYS_ARCH_H_
#define SYS_ARCH_H_

#if defined(NO_SYS) && (NO_SYS==0)

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "lwip/mem.h"

#define SYS_MBOX_NULL (NULL)
#define SYS_SEM_NULL (NULL)

typedef SemaphoreHandle_t * sys_sem_t;
/*typedef struct
{
	int8_t valid;
	QueueHandle_t queue;
} sys_mbox_t;*/
typedef QueueHandle_t * sys_mbox_t;
typedef SemaphoreHandle_t * sys_mutex_t;
typedef TaskHandle_t * sys_thread_t;
typedef char sys_prot_t;

void *my_mem_calloc(mem_size_t count, mem_size_t size);

#else

#define SYS_MBOX_NULL (NULL)
#define SYS_SEM_NULL (NULL)

//typedef void * sys_sem_t;
//typedef void * sys_mbox_t;
typedef void * sys_thread_t;
typedef void * sys_prot_t;

#endif

#endif /* SYS_ARCH_H_ */
