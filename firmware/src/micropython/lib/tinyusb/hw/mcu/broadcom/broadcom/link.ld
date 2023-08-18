
MEMORY
{
    /* Kernel load address for ARM11 is 0x8000 */
    READONLY (rx): ORIGIN = 0x8000, LENGTH = 2016K
    RAM (rw): ORIGIN = 0x200000, LENGTH = 254M 
}

SECTIONS
{
    . = 0x8000;     
    .text : {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t*)
    } > READONLY
    .rodata : {
        . = ALIGN(4096);
        *(.rodata .rodata.* .gnu.linkonce.r*)
    } > READONLY

    .data : {
        . = ALIGN(4096);
        . += 4K;
        _fiq_stack = .;
        . += 4K;
        _irq_stack = .;
        . += 4K;
        _abort_stack = .;
        . += 4K;
        _undef_stack = .;
        _data = .;
        *(.data .data.* .gnu.linkonce.d*)
    } > RAM
    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
        _end = .;
        end = .;
    } > RAM

    _ld_ram_end = ORIGIN(RAM) + LENGTH(RAM);

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;
