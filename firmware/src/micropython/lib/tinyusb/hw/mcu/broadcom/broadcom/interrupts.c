#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "broadcom/cpu.h"
#include "broadcom/defines.h"
#include "broadcom/gen/interrupt_handlers.h"

extern void print(const char* str);

void Default_Handler(void) {
    while (true) {}
}

uint8_t CPU_Index(void) {
    #ifdef __aarch64__
    uint32_t mpidr = 0;
    __asm__("mrs     %[mpidr], mpidr_el1"
            : /* No outputs. */
            : [mpidr] "r" (mpidr));
    return mpidr & 0xff;
    #endif
    return 0;
}

#if BCM_VERSION != 2711
static IRQn_Type _current_interrupt;
#endif

// These are CPU local.
void BP_EnableIRQs(void) {
    // We only turn on the distributor. Turning it off later may break other CPUs.
    // GIC_DIST->GICD_CTLR_b.ENABLE_GROUP0 = true;
    #if defined(__ARM_ARCH) && (__ARM_ARCH >= 8)
    #if BCM_VERSION == 2711
    GIC_CPU->GICC_CTLR_b.ENABLE_GROUP_0 = true;
    #else
    _current_interrupt = INTERRUPT_COUNT;
    #endif
    __asm__("msr    daifclr, #2");
    __asm__("isb");
    #else
    uint32_t psr;
    __asm__ volatile ("mrs %[psr], CPSR": [psr] "=r" (psr));
    psr = psr & 0x1f;
    // Don't enable interrupts if we're not in user or system mode.
    if (psr != 0x1f && psr != 0x10) {
        return;
    }
    _current_interrupt = INTERRUPT_COUNT;
    __asm__ volatile ("cpsie i" : : : "memory");
    #endif
}

void BP_DisableIRQs(void) {
    #if defined(__ARM_ARCH) && (__ARM_ARCH >= 8)
    __asm__("msr    daifset, #2");
    __asm__("isb");
    #else
    __asm__ volatile ("cpsid i" : : : "memory");
    #endif
}

__attribute__((weak)) void handle_irq(void) {
    #if BCM_VERSION != 2711
    static const uint8_t basic_to_id[] = {7, 9, 10, 18, 19, 53, 54, 55, 56, 57, 62};
    COMPLETE_MEMORY_READS;
    while (LIC->BASIC_PENDING != 0) {
        uint32_t current_pending = LIC->BASIC_PENDING;
        // We don't allow for nested interrupts so look through the pending bits in
        // "priority" order.
        size_t interrupt_id = INTERRUPT_COUNT;

        // Check arm locals first. They start at interrupt id 64.
        for (size_t i = 0; i < 8; i++) {
            if ((current_pending & (1 << i)) != 0) {
                interrupt_id = 64 + i;
                break;
            }
        }

        // Next check the direct connected IRQs.
        if (interrupt_id == INTERRUPT_COUNT) {
            for (size_t i = 0; i < 11; i++) {
                if ((current_pending & (1 << (i + 10))) != 0) {
                    interrupt_id = basic_to_id[i];
                    break;
                }
            }
        }

        // Finally, check the aggregate bits.
        if (interrupt_id == INTERRUPT_COUNT) {
            for (size_t i = 0; i < 2; i++) {
                if ((current_pending & (1 << (i + 8))) != 0) {
                    uint32_t gpu_pending;
                    if (i == 0) {
                        gpu_pending = LIC->PENDING_1;
                    } else {
                        gpu_pending = LIC->PENDING_2;
                    }
                    for (size_t j = 0; j < 32; j++) {
                        if ((gpu_pending & (1 << j)) != 0) {
                            interrupt_id = 32 * i + j;
                            break;
                        }
                    }
                }
                if (interrupt_id != INTERRUPT_COUNT) {
                    break;
                }
            }
        }

        if (interrupt_id >= INTERRUPT_COUNT) {
            break;
        }

        void(* handler)(void) = interrupt_handlers[interrupt_id];
        if (handler == NULL) {
            // Unhandled interrupt. Read interrupt_id from GDB to find out the mistake.
            while(true) {}
        }
        _current_interrupt = interrupt_id;
        // Put aggressive data access barriers around the interrupt handler
        COMPLETE_MEMORY_READS;
        handler();
        COMPLETE_MEMORY_READS;
        _current_interrupt = INTERRUPT_COUNT;
    }
    #else

    COMPLETE_MEMORY_READS;
    while (GIC_CPU->GICC_HPPIR_b.INTERRUPT_ID < INTERRUPT_COUNT) {
        // This register changes state after being read so make sure to read it
        // all at once. We need the full value to pass back to EOIR later.
        uint32_t current_interrupt = GIC_CPU->GICC_IAR;

        uint32_t interrupt_id = current_interrupt & GIC_CPU_GICC_IAR_INTERRUPT_ID_Msk;
        if (interrupt_id >= INTERRUPT_COUNT) {
            break;
        }

        // Turn on interrupts to allow for preemption.
        BP_EnableIRQs();

        void(* handler)(void) = interrupt_handlers[interrupt_id];
        if (handler == NULL) {
            // Unhandled interrupt. Read interrupt_id from GDB to find out the mistake.
            while(true) {}
        }
        COMPLETE_MEMORY_READS;
        handler();
        COMPLETE_MEMORY_READS;

        // Turn off interrupts while we do housekeeping.
        BP_DisableIRQs();

        GIC_CPU->GICC_EOIR = current_interrupt;
    }
    COMPLETE_MEMORY_READS;
    #endif
}

// Minimum priority is the highest priority value. (0 is highest priority)
void BP_SetMinPriority(uint8_t priority) {
    #if BCM_VERSION == 2711
    GIC_CPU->GICC_PMR_b.PRIORITY = priority;
    #else
    (void) priority;
    #endif
}

// We mimic the NVIC used in Cortex M SoCs.
void BP_EnableIRQ(IRQn_Type IRQn) {
    COMPLETE_MEMORY_READS;
    #if BCM_VERSION != 2711
    if (IRQn < 32) {
        LIC->ENABLE_1 = 1 << IRQn;
    } else if (IRQn < 64) {
        LIC->ENABLE_2 = 1 << (IRQn - 32);
    } else {
        LIC->ENABLE_BASIC = 1 << (IRQn - 64);
    }
    #else
    volatile uint8_t* targets = (volatile uint8_t*) &GIC_DIST->GICD_ITARGETSR;
    targets[IRQn] |= 1 << CPU_Index();
    volatile uint32_t* enabled = (volatile uint32_t*) &GIC_DIST->GICD_ISENABLER;
    enabled[IRQn / 32] = 1 << (IRQn % 32);
    #endif
    COMPLETE_MEMORY_READS;
}

bool BP_GetEnableIRQ(IRQn_Type IRQn) {
    COMPLETE_MEMORY_READS;
    #if BCM_VERSION != 2711
    if (IRQn < 32) {
        return (LIC->ENABLE_1 & (1 << IRQn)) != 0;
    } else if (IRQn < 64) {
        return (LIC->ENABLE_2 & (1 << (IRQn - 32))) != 0;
    } else {
        return (LIC->ENABLE_BASIC & (1 << (IRQn - 64))) != 0;
    }
    #else
    volatile uint8_t* targets = (volatile uint8_t*) &GIC_DIST->GICD_ITARGETSR;
    return (targets[IRQn] & (1 << CPU_Index())) != 0;
    #endif
    COMPLETE_MEMORY_READS;
}

void BP_DisableIRQ(IRQn_Type IRQn) {
    COMPLETE_MEMORY_READS;
    #if BCM_VERSION != 2711
    if (IRQn < 32) {
        LIC->DISABLE_1 = 1 << IRQn;
    } else if (IRQn < 64) {
        LIC->DISABLE_2 = 1 << (IRQn - 32);
    } else {
        LIC->DISABLE_BASIC = 1 << (IRQn - 64);
    }
    #else
    volatile uint8_t* targets = (volatile uint8_t*) &GIC_DIST->GICD_ITARGETSR;
    targets[IRQn] &= ~(1 << CPU_Index());
    #endif
    COMPLETE_MEMORY_READS;
}

bool BP_GetPendingIRQ(IRQn_Type IRQn) {
    COMPLETE_MEMORY_READS;
    #if BCM_VERSION == 2711
    volatile uint32_t* pending = (volatile uint32_t*) &GIC_DIST->GICD_ISPENDR;
    return (pending[IRQn / 32] & (1 << (IRQn % 32))) != 0;
    #else
    if (IRQn < 32) {
        return (LIC->PENDING_1 & (1 << IRQn)) != 0;
    } else if (IRQn < 64) {
        return (LIC->PENDING_2 & (1 << (IRQn - 32))) != 0;
    } else {
        return (LIC->BASIC_PENDING & (1 << (IRQn - 64))) != 0;
    }
    #endif
    COMPLETE_MEMORY_READS;
}

void BP_SetPendingIRQ(IRQn_Type IRQn) {
    #if BCM_VERSION == 2711
    COMPLETE_MEMORY_READS;
    volatile uint32_t* set_pending = (volatile uint32_t*) &GIC_DIST->GICD_ISPENDR;
    set_pending[IRQn / 32] = 1 << (IRQn % 32);
    COMPLETE_MEMORY_READS;
    #else
    // The legacy interrupt controller cannot set interrupts pending. The interrupt
    // state is directly tied to the source peripheral.
    (void) IRQn;
    #endif
}

void BP_ClearPendingIRQ(IRQn_Type IRQn)  {
    #if BCM_VERSION == 2711
    COMPLETE_MEMORY_READS;
    volatile uint32_t* clear_pending = (volatile uint32_t*) &GIC_DIST->GICD_ICPENDR;
    clear_pending[IRQn / 32] = 1 << (IRQn % 32);
    COMPLETE_MEMORY_READS;
    #else
    // The legacy interrupt controller cannot clear interrupts. They must be done
    // on the peripheral.
    (void) IRQn;
    #endif
}

bool BP_GetActive(IRQn_Type IRQn) {
    #if BCM_VERSION == 2711
    COMPLETE_MEMORY_READS;
    volatile uint32_t* active = (volatile uint32_t*) &GIC_DIST->GICD_ISACTIVER;
    COMPLETE_MEMORY_READS;
    return (active[IRQn / 32] & (1 << (IRQn % 32))) != 0;
    #else
    return _current_interrupt == IRQn;
    #endif
}

void BP_SetPriority(IRQn_Type IRQn, uint8_t priority) {
    #if BCM_VERSION == 2711
    COMPLETE_MEMORY_READS;
    volatile uint8_t* irq_priority = (volatile uint8_t*) &GIC_DIST->GICD_IPRIORITYR;
    irq_priority[IRQn] = priority;
    COMPLETE_MEMORY_READS;
    #else
    (void) IRQn;
    (void) priority;
    #endif
}

uint8_t BP_GetPriority(IRQn_Type IRQn) {
    #if BCM_VERSION == 2711
    volatile uint8_t* irq_priority = (volatile uint8_t*) &GIC_DIST->GICD_IPRIORITYR;
    return irq_priority[IRQn];
    #else
    (void) IRQn;
    return 0;
    #endif
}
