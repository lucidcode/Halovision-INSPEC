ENTRY(Reset_Handler)
MEMORY
{
  FLASH (rx): ORIGIN = 0x60000000, LENGTH = 0x00800000
  FLASH_FCB (RX): ORIGIN = 0x60040000 + 0x0000, LENGTH = 0x00001000
  FLASH_IVT (RX): ORIGIN = 0x60040000 + 0x1000, LENGTH = 0x00001000
  FLASH_ISR (RX): ORIGIN = 0x60040000 + 0x2000, LENGTH = 0x00001000
  FLASH_TEXT (RX): ORIGIN = 0x60043000, LENGTH = 0x00380000
  FLASH_VFS (RX): ORIGIN = 0x60400000, LENGTH = 0x00400000
  ITCM1 (RWX): ORIGIN = 0x00000000, LENGTH = 32K
  ITCM2 (RWX): ORIGIN = 0x00008000, LENGTH = 32K
  DTCM (RWX): ORIGIN = 0x20000000, LENGTH = 384K
  OCRM1 (RWX): ORIGIN = 0x20200000, LENGTH = 512K
  OCRM2 (RWX): ORIGIN = 0x20280000, LENGTH = 64K
  DRAM (RWX): ORIGIN = 0x80000000, LENGTH = 32M
}
_start = main;
__flash_start = ORIGIN(FLASH);
__sdram_start = ORIGIN(DRAM);
__vfs_start = ORIGIN(FLASH_VFS);
__vfs_end = ORIGIN(FLASH_VFS) + LENGTH(FLASH_VFS);
__iomux_gpr14_adr = 0x400AC038;
__iomux_gpr16_adr = 0x400AC040;
__iomux_gpr17_adr = 0x400AC044;
__iomux_gpr17_value = (0x5AAAAAAF);
SECTIONS
{
  .fcb :
  {
    . = ALIGN(4);
    __FLASH_BASE = .;
    KEEP(* (.boot_hdr.conf))
    . = ALIGN(4);
  } > FLASH_FCB
  .ivt :
  {
    . = ALIGN(4);
    KEEP(* (.boot_hdr.ivt))
    KEEP(* (.boot_hdr.boot_data))
    KEEP(* (.boot_hdr.dcd_data))
    . = ALIGN(4);
  } > FLASH_IVT
  .isr :
  {
    __VECTOR_TABLE = .;
    __Vectors = .;
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > FLASH_ISR
  .text :
  {
    . = ALIGN(4);
    *(EXCLUDE_FILE(*fsl_flexspi.o *gc.o *vm.o *parse*.o *runtime*.o *map.o *mpirq.o ) .text*)
    *(.rodata)
    *(.rodata*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
  } > FLASH_TEXT
  __etext = LOADADDR(.data);
  .data : ALIGN(4)
  {
    _ram_start = .;
    __data_start__ = .;
    *(m_usb_dma_init_data)
    . = ALIGN(4);
    *(.data)
    . = ALIGN(4);
    *(.data*)
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;
  } > DTCM AT> FLASH_TEXT
  .bss (NOLOAD) : ALIGN(4)
  {
    __bss_start__ = .;
    *(m_usb_dma_noninit_data)
    . = ALIGN(4);
    *(.bss)
    . = ALIGN(4);
    *(.bss*)
    . = ALIGN(4);
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = .;
  } > DTCM
  .ram_function : ALIGN(4)
  {
    __ram_function_start__ = .;
    *(.ram_functions*)
    *(.text*)
    . = ALIGN(4);
    __ram_function_end__ = .;
  } > ITCM2 AT> FLASH_TEXT
.gc.block0 (NOLOAD) : ALIGN(4)
{
  _heap_start = .;
  . = . + (26K);
  . = ALIGN(4);
  _heap_end = .;
} >OCRM2
.gc.block1 (NOLOAD) : { . = ALIGN(4); . += (288K); . = ALIGN(4); } >DTCM
.gc.block2 (NOLOAD) : { . = ALIGN(4); . += (8M); . = ALIGN(4); } >DRAM
.gc.blocks.table (READONLY) :
{
  . = ALIGN(4);
  _gc_blocks_table_start = .;
  LONG (ADDR(.gc.block1)) LONG (SIZEOF(.gc.block1))
  LONG (ADDR(.gc.block2)) LONG (SIZEOF(.gc.block2))
  _gc_blocks_table_end = .;
  . = ALIGN(4);
} > FLASH_TEXT
.stack (NOLOAD) : ALIGN(8)
{
  _sstack = .;
  __StackLimit = .;
  . = . + (32K);
  . = ALIGN(8);
  _estack = .;
  __StackTop = .;
} > ITCM1
PROVIDE(__stack = __StackTop);
.jpeg_memory (NOLOAD) :
{
  . = ALIGN(4);
  _jpeg_memory_start = .;
  . = . + (1M);
  . = ALIGN(4);
  _jpeg_memory_end = .;
} >DRAM
.vospi_memory (NOLOAD) :
{
  . = ALIGN(4);
  _vospi_buf = .;
  . = . + (38K);
  . = ALIGN(4);
} >OCRM2
.fb_memory (NOLOAD) :
{
  . = ALIGN(4);
  _fb_memory_start = .;
  . += (13M);
  . = ALIGN(4);
  _fb_memory_end = .;
  . += (10M);
  . = ALIGN(4);
  _fb_alloc_end = .;
  . = ALIGN(4);
} >DRAM
.fb_overlay_memory (NOLOAD) :
{
  . = ALIGN(4);
  _fballoc_overlay_start = .;
  . = . + (512K);
  _fballoc_overlay_end = .;
} >OCRM1
.dma.memory0 (NOLOAD) : ALIGN(32)
{
  . = ALIGN(16);
  _line_buf = .;
  . = . + (11 * 1024);
 . = ALIGN(16);
  *(.dma_buffer)
 . = ALIGN(16);
 *(NonCacheable)
  . = ALIGN(., 1 << LOG2CEIL(SIZEOF(.dma.memory0)));
} >DTCM
.dma.memory.table (READONLY) :
{
  . = ALIGN(4);
  _dma_memory_table_start = .;
  LONG (ADDR(.dma.memory0)) LONG (SIZEOF(.dma.memory0))
  _dma_memory_table_end = .;
  . = ALIGN(4);
} > FLASH_TEXT
_micropy_hw_romfs_part0_start = 0x60800000;
_micropy_hw_romfs_part0_size = 8M;
.ARM.attributes 0 : { *(.ARM.attributes) }
}
