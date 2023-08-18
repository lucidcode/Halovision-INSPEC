#pragma once

// To ensure we loaded the correct data, we must have the CPU wait for pending
// reads before moving onto reads from other peripherals. This is done with a
// memory barrier instruction.
#if defined(__ARM_ARCH) && (__ARM_ARCH >= 8)
#define COMPLETE_MEMORY_READS __asm__ volatile ("dsb sy")
#define STRICT_ALIGN __attribute__((target("strict-align")))
#else
#define COMPLETE_MEMORY_READS __asm__ volatile ("mcr p15, 0, %[zero], c7, c10, 5" : : [zero] "r" (0) : "memory")
#define STRICT_ALIGN
#endif
