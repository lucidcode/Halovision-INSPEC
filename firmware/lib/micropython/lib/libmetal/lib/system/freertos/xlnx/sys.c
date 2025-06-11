/*
 * Copyright (C) 2023, Advanced Micro Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	freertos/xlnx/sys.c
 * @brief	machine specific system primitives implementation.
 */

#include <metal/compiler.h>
#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <stdint.h>
#include "xil_cache.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xil_mmu.h"

#if (defined(__aarch64__) || defined(ARMA53_32)) && !defined(SDT)

#ifdef VERSAL_NET
#include "xcpu_cortexa78.h"
#elif defined(versal)
#include "xcpu_cortexa72.h"
#else
#include "xreg_cortexa53.h"
#endif /* defined(versal) */

#elif defined(ARMR5)

#include "xil_mpu.h"
#include "xreg_cortexr5.h"

#endif /* (defined(__aarch64__) || defined(ARMA53_32)) && !defined(SDT) */

void sys_irq_restore_enable(unsigned int flags)
{
	Xil_ExceptionEnableMask(~flags);
}

unsigned int sys_irq_save_disable(void)
{
	unsigned int state = mfcpsr() & XIL_EXCEPTION_ALL;

	if (state != XIL_EXCEPTION_ALL) {
		Xil_ExceptionDisableMask(XIL_EXCEPTION_ALL);
	}
	return state;
}

void metal_machine_cache_flush(void *addr, unsigned int len)
{
	if (!addr && !len)
		Xil_DCacheFlush();
	else
		Xil_DCacheFlushRange((intptr_t)addr, len);
}

void metal_machine_cache_invalidate(void *addr, unsigned int len)
{
	if (!addr && !len)
		Xil_DCacheInvalidate();
	else
		Xil_DCacheInvalidateRange((intptr_t)addr, len);
}

/**
 * @brief poll function until some event happens
 */
void metal_weak metal_generic_default_poll(void)
{
	metal_asm volatile("wfi");
}

void *metal_machine_io_mem_map(void *va, metal_phys_addr_t pa,
			       size_t size, unsigned int flags)
{
	void *__attribute__((unused)) physaddr;

	physaddr = Xil_MemMap(pa, size, flags);
	metal_assert(physaddr == (void *)pa);
	return va;
}
