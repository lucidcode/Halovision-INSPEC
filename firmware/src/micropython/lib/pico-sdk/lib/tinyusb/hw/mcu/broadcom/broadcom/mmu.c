#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "broadcom/mmu.h"

#include "broadcom/cpu.h"

#ifdef __aarch64__
// Each entry is a gig.
uint64_t level_1_table[512] __attribute__((aligned(4096)));

// First gig is stack, code, rodata then data, bss, ram and then GPU.
uint64_t level_2_0x0_0000_0000_to_0x0_4000_0000[512] __attribute__((aligned(4096)));

#if BCM_VERSION == 2711
// Third gig has peripherals
uint64_t level_2_0x0_c000_0000_to_0x1_0000_0000[512] __attribute__((aligned(4096)));
#endif
#else
// Each entry is a megabyte
uint32_t level_1_table[1024] __attribute__((aligned(4096)));
#endif

STRICT_ALIGN void setup_mmu_flat_map(void) {
    #if BCM_VERSION == 2835
    // Each entry is 1MB or 0x10_0000
    // First two MB 0x0000_0000 to 0x0020_0000
    uint32_t code = MM_DESCRIPTOR_TYPE_SECTION |
                    MM_DESCRIPTOR_GLOBAL |
                    MM_DESCRIPTOR_NON_SHARED |
                    MM_DESCRIPTOR_CACHED_WRITE_BACK_ALLOC |
                    MM_DESCRIPTOR_ACCESS_READ_ONLY;
    uint32_t data = MM_DESCRIPTOR_TYPE_SECTION |
                    MM_DESCRIPTOR_GLOBAL |
                    MM_DESCRIPTOR_NON_SHARED |
                    MM_DESCRIPTOR_CACHED_WRITE_BACK_ALLOC |
                    MM_DESCRIPTOR_ACCESS_READ_WRITE |
                    MM_DESCRIPTOR_EXECUTE_NEVER;
    uint32_t device = MM_DESCRIPTOR_TYPE_SECTION |
                      MM_DESCRIPTOR_GLOBAL |
                      MM_DESCRIPTOR_NON_SHARED |
                      MM_DESCRIPTOR_SHARED_DEVICE |
                      MM_DESCRIPTOR_ACCESS_READ_WRITE |
                      MM_DESCRIPTOR_EXECUTE_NEVER;
    level_1_table[0] = 0x00000000 | 
                       code;
    level_1_table[1] = (1 << 20) | 
                       code;
    // TODO: Adjust for 256MB vs 512MB ram.
    for (size_t i = 2; i < 16; i++) {
        level_1_table[i] = (i << 20) |
                            data;
    }
    // Supersections take up 16 entries in the table but in the cached TLB they
    // only take one entry.
    for (size_t i = 16 / 16; i < 512 / 16; i++) {
        for (size_t j = 0; j < 16; j++) {
            level_1_table[i * 16 + j] = (i << 24) |
                                        data |
                                        MM_DESCRIPTOR_SUPERSECTION;
        }
    }

    // Set peripherals to register access using a supersection. 0x2000_0000 to 0x2100_0000
    for (uint64_t i = 512; i < 512 + 16; i++) {
        level_1_table[i] = (512 << 20) |
                            device |
                            MM_DESCRIPTOR_SUPERSECTION;
    }
    #endif
    #if BCM_VERSION == 2837
    // Each entry is 2MB or 0x20_0000
    // First two MB 0x0000_0000 to 0x0020_0000
    level_2_0x0_0000_0000_to_0x0_4000_0000[0] = 0x0000000000000000 | 
                                                MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                MM_DESCRIPTOR_INNER_SHAREABLE |
                                                MM_DESCRIPTOR_ACCESS_FLAG |
                                                MM_DESCRIPTOR_BLOCK |
                                                MM_DESCRIPTOR_VALID; 
    for (uint64_t i = 1; i < 256 - 8; i++) {
        level_2_0x0_0000_0000_to_0x0_4000_0000[i] = (0x0000000000000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                    MM_DESCRIPTOR_INNER_SHAREABLE |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }
    // Last 16 MB of 512MB RAM are shared with the GPU.
    for (uint64_t i = 256 - 8; i < 256; i++) {
        level_2_0x0_0000_0000_to_0x0_4000_0000[i] = (0x0000000000000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                    MM_DESCRIPTOR_INNER_SHAREABLE |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }

    // Set peripherals to register access. 0x3f00_0000 to 0x4000_0000
    for (uint64_t i = 504; i < 512; i++) {
        level_2_0x0_0000_0000_to_0x0_4000_0000[i] = (0x000000000000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_DEVICE_nGnRnE) |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }
    level_1_table[0] = ((uint64_t) level_2_0x0_0000_0000_to_0x0_4000_0000) |
                       MM_DESCRIPTOR_TABLE |
                       MM_DESCRIPTOR_VALID;
    #endif
    #if BCM_VERSION == 2711
    // Each entry is 2MB or 0x20_0000
    // First two MB
    level_2_0x0_0000_0000_to_0x0_4000_0000[0] = 0x0000000000000000 | 
                                                MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                MM_DESCRIPTOR_INNER_SHAREABLE |
                                                MM_DESCRIPTOR_ACCESS_FLAG |
                                                MM_DESCRIPTOR_BLOCK |
                                                MM_DESCRIPTOR_VALID; 
    for (uint64_t i = 1; i < 512 - 8; i++) {
        level_2_0x0_0000_0000_to_0x0_4000_0000[i] = (0x0000000000000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                    MM_DESCRIPTOR_INNER_SHAREABLE |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }
    // Last 16 MB are shared with the GPU.
    for (uint64_t i = 512 - 8; i < 512; i++) {
        level_2_0x0_0000_0000_to_0x0_4000_0000[i] = (0x0000000000000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_READONLY) |
                                                    MM_DESCRIPTOR_INNER_SHAREABLE |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }
    level_1_table[0] = ((uint64_t) level_2_0x0_0000_0000_to_0x0_4000_0000) |
                       MM_DESCRIPTOR_TABLE |
                       MM_DESCRIPTOR_VALID;

    // Set peripherals to register access.
    for (uint64_t i = 480; i < 512; i++) {
        level_2_0x0_c000_0000_to_0x1_0000_0000[i] = (0x00000000c0000000 + (i << 21)) |
                                                    MM_DESCRIPTOR_EXECUTE_NEVER |
                                                    MM_DESCRIPTOR_MAIR_INDEX(MT_DEVICE_nGnRnE) |
                                                    MM_DESCRIPTOR_ACCESS_FLAG |
                                                    MM_DESCRIPTOR_BLOCK |
                                                    MM_DESCRIPTOR_VALID;
    }
    level_1_table[3] = ((uint64_t) level_2_0x0_c000_0000_to_0x1_0000_0000) |
                       MM_DESCRIPTOR_TABLE |
                       MM_DESCRIPTOR_VALID;
    #endif
    #ifdef __aarch64__
    uint64_t mair = MAIR_VALUE;
    uint64_t tcr = TCR_VALUE;
    uint64_t ttbr0 = ((uint64_t) level_1_table) | MM_TTBR_CNP;
    uint64_t sctlr = 0;
    __asm__ volatile (
        // The ISB forces these changes to be seen before any other registers are changed
        "ISB\n\t"
        // Clear the TLB
        "TLBI VMALLE1\n\t"
        // Set MAIR
        "MSR MAIR_EL1, %[mair]\n\t"
        // Set TTBR0
        "MSR TTBR0_EL1, %[ttbr0]\n\t"
        // Set TCR
        "MSR TCR_EL1, %[tcr]\n\t"
        // The ISB forces these changes to be seen before the MMU is enabled.
        "ISB\n\t"
        // Read System Control Register configuration data
        "MRS %[sctlr], SCTLR_EL1\n\t"
        // Set [M] bit and enable the MMU.
        "ORR %[sctlr], %[sctlr], #0x1\n\t"
        "dsb ish\n\t"
        "isb\n\t"
        "MSR SCTLR_EL1, %[sctlr]\n\t"
        // The ISB forces these changes to be seen by the next instruction
        "ISB\n\t"
        : /* No outputs. */
        : [mair] "r" (mair),
          [tcr] "r" (tcr),
          [ttbr0] "r" (ttbr0),
          [sctlr] "r" (sctlr)
    );
    #else
    size_t ttbcr = MM_TTBCR_PD1 |
                   MM_TTBCR_N(0x2);
    size_t ttbr0 = ((size_t) level_1_table) |
                   MM_TTBR_INNER_CACHEABLE |
                   MM_TTBR_RGN_WRITE_BACK_ALLOC;
    size_t dacr = 0x1; // Client mode
    size_t sctlr = 0;
    __asm__ volatile (
        // Clear the TLB
        "MCR p15, 0, %[zero], c8, c7, 0\n\t"
        // Set TTBR0
        "MCR p15, 0, %[ttbr0], c2, c0, 0\n\t"
        // Set TTBCR
        "MCR p15, 0, %[ttbcr], c2, c0, 2\n\t"
        // Set Domain Access Control Register
        "MCR p15, 0, %[dacr], c3, c0, 0\n\t"
        // Read System Control Register configuration data
        "MRC p15, 0, %[sctlr], c1, c0, 0\n\t"
        // Set [XP] bit to enable the ARMv6 format page tables.
        "ORR %[sctlr], %[sctlr], #0x800000\n\t"
        // Set [U] bit to allow unaligned accesses
        "ORR %[sctlr], %[sctlr], #0x400000\n\t"
        // Set [M] bit to enable the MMU
        "ORR %[sctlr], %[sctlr], #0x1\n\t"
        // Enable the MMU
        "MCR p15, 0, %[sctlr], c1, c0, 0\n\t"
        // Flush the prefetch buffer.
        "MCR p15, 0, %[zero], c7, c5,  4\n\t"
        : /* No outputs. */
        : [zero] "r" (0),
          [ttbcr] "r" (ttbcr),
          [ttbr0] "r" (ttbr0),
          [dacr]  "r" (dacr),
          [sctlr] "r" (sctlr)
    );
    #endif
}
