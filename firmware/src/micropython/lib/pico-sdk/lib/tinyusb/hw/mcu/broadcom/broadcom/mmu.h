#pragma once


// From: https://github.com/s-matyukevich/raspberry-pi-os/blob/master/docs/lesson06/rpi-os.md
#if defined(__ARM_ARCH) && (__ARM_ARCH >= 8)
/*
 * Memory region attributes:
 *
 *   n = AttrIndx[2:0]
 *            n    MAIR
 *   DEVICE_nGnRnE    000    00000000
 *   NORMAL_NC        001    01000100
 */
#define MT_DEVICE_nGnRnE         0x0
#define MT_NORMAL_NC            0x1
#define MT_READONLY             0x2
#define MT_DEVICE_nGnRnE_FLAGS      0x00
#define MT_NORMAL_NC_FLAGS          0x44
#define MT_READONLY_FLAGS           0xff
#define MAIR_VALUE            (MT_DEVICE_nGnRnE_FLAGS << (8 * MT_DEVICE_nGnRnE)) | \
                              (MT_NORMAL_NC_FLAGS << (8 * MT_NORMAL_NC)) | \
                              (MT_READONLY_FLAGS) << (8 * MT_READONLY)

#define TCR_T0SZ            (64 - 36) 
#define TCR_PS              (0x01 << 16) // 36-bit physical address
#define TCR_TG0_4K          (0 << 14)
#define TCR_IPS_64G         (1ull << 32)
#define TCR_SH0_OUTER_SHAREABLE (0x2 << 12)
#define TCR_SH0_INNER_SHAREABLE (0x3 << 12)
#define OUTER_CACHEABLE     (0x1 << 10)
#define INNER_CACHEABLE     (0x1 << 8)
#define TCR_VALUE           (TCR_IPS_64G | OUTER_CACHEABLE | INNER_CACHEABLE | TCR_T0SZ | TCR_PS | TCR_TG0_4K | TCR_SH0_INNER_SHAREABLE)

#define ENTRY_TYPE_TABLE_DESCRIPTOR 0x11
#define ENTRY_TYPE_BLOCK_ENTRY 0x01
#define ENTRY_TYPE_TABLE_ENTRY 0x11
#define ENTRY_TYPE_INVALID 0x00

#define MM_DESCRIPTOR_VALID (0x1)

#define MM_DESCRIPTOR_BLOCK (0x0 << 1)
#define MM_DESCRIPTOR_TABLE (0x1 << 1)

// Block attributes
#define MM_DESCRIPTOR_EXECUTE_NEVER (0x1ull << 54)
#define MM_DESCRIPTOR_CONTIGUOUS    (0x1ull << 52)
#define MM_DESCRIPTOR_ACCESS_FLAG   (0x1ull << 10)

#define MM_DESCRIPTOR_NON_SHAREABLE (0x00ull << 8)
#define MM_DESCRIPTOR_OUTER_SHAREABLE (0x10ull << 8)
#define MM_DESCRIPTOR_INNER_SHAREABLE (0x11ull << 8)

#define MM_DESCRIPTOR_MAIR_INDEX(index) (index << 2)

#define MM_TTBR_CNP (0x1)


#endif  // defined(__ARM_ARCH) && (__ARM_ARCH >= 8)
#if defined(__ARM_ARCH) && (__ARM_ARCH == 6)

#define MM_DESCRIPTOR_TYPE_FAULT      (0x0 << 0)
#define MM_DESCRIPTOR_TYPE_PAGE_TABLE (0x1 << 0)
#define MM_DESCRIPTOR_TYPE_SECTION    (0x2 << 0)
#define MM_DESCRIPTOR_TYPE_RESERVED   (0x3 << 0)

#define MM_DESCRIPTOR_EXECUTE_NEVER   (0x1 << 4)

#define MM_DESCRIPTOR_SUPERSECTION    (0x1 << 18)

#define MM_DESCRIPTOR_GLOBAL           (0x0 << 17)
#define MM_DESCRIPTOR_PROCESS_SPECIFIC (0x1 << 17)

#define MM_DESCRIPTOR_NON_SHARED      (0x0 << 16)
#define MM_DESCRIPTOR_SHARED          (0x1 << 16)

#define MM_DESCRIPTOR_CACHEABLE        (0x1 << 3)

#define MM_DESCRIPTOR_BUFFERABLE       (0x1 << 2)

// For both privileged and user
#define MM_DESCRIPTOR_ACCESS_READ_ONLY (1 << 15 | 0x2 << 10)
#define MM_DESCRIPTOR_ACCESS_READ_WRITE (0 << 15 | 0x3 << 10)

#define MM_DESCRIPTOR_STRONGLY_ORDERED          (0x0 << 12)
#define MM_DESCRIPTOR_SHARED_DEVICE             (0x0 << 12 | MM_DESCRIPTOR_BUFFERABLE)
#define MM_DESCRIPTOR_WRITE_THROUGH_NO_ALLOC    (0x0 << 12 | MM_DESCRIPTOR_CACHEABLE)
#define MM_DESCRIPTOR_WRITE_BACK_NO_ALLOC       (0x0 << 12 | MM_DESCRIPTOR_BUFFERABLE | MM_DESCRIPTOR_CACHEABLE)
#define MM_DESCRIPTOR_NONCACHEABLE              (0x1 << 12)
#define MM_DESCRIPTOR_WRITE_BACK_ALLOC          (0x1 << 12 | MM_DESCRIPTOR_BUFFERABLE | MM_DESCRIPTOR_CACHEABLE)
#define MM_DESCRIPTOR_NON_SHARED_DEVICE         (0x2 << 12)
#define MM_DESCRIPTOR_CACHED_WRITE_BACK_ALLOC       (0x5 << 12 | MM_DESCRIPTOR_BUFFERABLE)
#define MM_DESCRIPTOR_CACHED_WRITE_THROUGH_NO_ALLOC (0x6 << 12 | MM_DESCRIPTOR_CACHEABLE)
#define MM_DESCRIPTOR_CACHED_WRITE_BACK_NO_ALLOC    (0x7 << 12 | MM_DESCRIPTOR_CACHEABLE | MM_DESCRIPTOR_BUFFERABLE)

#define MM_TTBR_INNER_CACHEABLE      (0x1 << 0)
#define MM_TTBR_RGN_WRITE_BACK_ALLOC (0x1 << 3)

#define MM_TTBCR_PD1 (0x1 << 5)
#define MM_TTBCR_N(v) (v << 0)

#endif

void setup_mmu_flat_map(void);
