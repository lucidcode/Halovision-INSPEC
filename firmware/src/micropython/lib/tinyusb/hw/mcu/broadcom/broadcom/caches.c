
#include <stdbool.h>
#include <stdint.h>

#include "broadcom/caches.h"

#include "broadcom/cpu.h"

static uint64_t bytes_per_data_line;

void init_caches(void) {
    bytes_per_data_line = 0;
    #ifdef __aarch64__
    uint64_t ctr;
    uint64_t sctlr = 0;
    __asm__ volatile (
        // The ISB forces these changes to be seen before the MMU is enabled.
        "ISB\n\t"
        // Load the smallest cache line size from the cache type register.
        "MRS %[ctr], CTR_EL0\n\t"
        // Read System Control Register configuration data
        "MRS %[sctlr], SCTLR_EL1\n\t"
        // Turn on the instruction cache
        "ORR %[sctlr], %[sctlr], #0x1000\n\t"
        // Turn on the data cache
        "ORR %[sctlr], %[sctlr], #0x4\n\t"
        "MSR SCTLR_EL1, %[sctlr]\n\t"
        // The ISB forces these changes to be seen by the next instruction
        "ISB\n\t"
        : [ctr] "=r" (ctr)
        : [sctlr] "r" (sctlr)
        : "memory"
    );
    // Convert the power of two cache line size in words to bytes.
    bytes_per_data_line = (1 << ((ctr >> 16) & 0xf)) * sizeof(uint32_t);
    #endif
    #if BCM_VERSION == 2835
    uint64_t sctlr = 0;
    __asm__ volatile (
        // Flush the prefetch buffer.
        "MCR p15, 0, %[zero], c7, c5,  4\n\t"
        // Read System Control Register configuration data
        "MRC p15, 0, %[sctlr], c1, c0, 0\n\t"
        // Turn on the instruction cache
        "ORR %[sctlr], %[sctlr], #0x1000\n\t"
        // Turn on branch prediction
        "ORR %[sctlr], %[sctlr], #0x800\n\t"
        // Turn on the data cache
        "ORR %[sctlr], %[sctlr], #0x4\n\t"
        "MCR p15, 0, %[sctlr], c1, c0, 0\n\t"
        // Flush the prefetch buffer.
        "MCR p15, 0, %[zero], c7, c5,  4\n\t"
        :
        : [sctlr] "r" (sctlr), [zero] "r" (0)
        : "memory"
    );
    bytes_per_data_line = 32;
    #endif
}

static bool _data_cache_on(void) {
    size_t sctlr = 0;
    #ifdef __aarch64__
    __asm__ volatile (
        // Read System Control Register configuration data
        "MRS %[sctlr], SCTLR_EL1\n\t"
        : [sctlr] "=r" (sctlr)
    );
    #else
    __asm__ volatile (
        // Read System Control Register configuration data
        "MRC p15, 0, %[sctlr], c1, c0, 0\n\t"
        : [sctlr] "=r" (sctlr)
    );
    #endif
    return (sctlr & 0x4) != 0;
}

// Writes values from the cache back into memory but keep a copy in the cache.
STRICT_ALIGN void data_clean(volatile void* starting_address, size_t size) {
    if (!_data_cache_on()) {
        return;
    }
    #ifdef __aarch64__
    for (size_t offset = 0; offset < size + bytes_per_data_line; offset += bytes_per_data_line) {
        volatile void* ptr = starting_address + offset;
        __asm__ volatile("dc cvac, %[ptr]" : : [ptr] "r" (ptr));
    }
    __asm__ volatile("isb");
    __asm__ volatile("dsb sy");
    #else
    uint32_t mask = ~(bytes_per_data_line - 1);
    size_t start = ((size_t) starting_address) & mask;
    size_t end = (((size_t) starting_address) + size + bytes_per_data_line) & mask;
    __asm__ volatile("mcrr p15,0,%[end],%[start],c12" : : [end] "r" (end), [start] "r" (start) : "memory");
    #endif
}

// Writes values from the cache back into memory and remove it from the cache.
STRICT_ALIGN void data_clean_and_invalidate(volatile void* starting_address, size_t size) {
    if (!_data_cache_on()) {
        return;
    }
    #ifdef __aarch64__
    for (size_t offset = 0; offset < size; offset += bytes_per_data_line) {
        __asm__ volatile("dc civac, %[ptr]" : : [ptr] "r" (starting_address + offset));
    }
    __asm__ volatile("isb");
    __asm__ volatile("dsb sy");
    #else
    uint32_t mask = ~(bytes_per_data_line - 1);
    size_t start = ((size_t) starting_address) & mask;
    size_t end = (((size_t) starting_address) + size + bytes_per_data_line) & mask;
    __asm__ volatile("mcrr p15,0,%[end],%[start],c14" : : [end] "r" (end), [start] "r" (start) : "memory");
    #endif
}

// Remove values from the cache because the value in memory may have changed.
STRICT_ALIGN void data_invalidate(volatile void* starting_address, size_t size) {
    if (!_data_cache_on()) {
        return;
    }
    #ifdef __aarch64__
    for (size_t offset = 0; offset < size + bytes_per_data_line; offset += bytes_per_data_line) {
        __asm__ volatile("dc ivac, %[ptr]" : : [ptr] "r" (starting_address + offset): "memory");
    }
    __asm__ volatile("isb");
    __asm__ volatile("dsb sy" : : : "memory");
    #else
    uint32_t mask = ~(bytes_per_data_line - 1);
    size_t start = ((size_t) starting_address) & mask;
    size_t end = (((size_t) starting_address) + size + bytes_per_data_line) & mask;
    __asm__ volatile("mcrr p15,0,%[end],%[start],c6" : : [end] "r" (end), [start] "r" (start) : "memory");
    #endif
}
