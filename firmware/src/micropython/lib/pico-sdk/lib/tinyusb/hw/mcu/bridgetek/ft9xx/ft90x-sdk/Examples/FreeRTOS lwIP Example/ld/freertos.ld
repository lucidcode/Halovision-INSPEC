/* Default linker script, for normal executables */
OUTPUT_FORMAT("elf32-ft32")
OUTPUT_ARCH(ft32)
SEARCH_DIR("/opt/ft32/ft32-elf/lib");
/* Allow the command line to override the memory region sizes.  */
__PMSIZE = DEFINED(__PMSIZE)  ? __PMSIZE : 256K;
__RAMSIZE = DEFINED(__RAMSIZE) ? __RAMSIZE : 64K;
MEMORY
{
  flash     (rx)   : ORIGIN = 0,        LENGTH = __PMSIZE
  ram       (rw!x) : ORIGIN = 0x800000, LENGTH = __RAMSIZE
}

SECTIONS
{
  .text :
  {
    *(.crt0*)
    *(.text*)
    *(.strings)
    *(._pm*)
    *(.init)
    *(.fini)
    . = ALIGN(4);
     _etext = . ;
  }  > flash
  
  .tors :
  {
    ___ctors = . ;
    *(.ctors)
    ___ctors_end = . ;
    ___dtors = . ;
    *(.dtors)
    . = ALIGN(4);
    ___dtors_end = . ;
  } > ram
  .data	  :
  {
  	. = ALIGN(4);
    __data = .;
    *(.data)
	*(.data*)
	*(.rodata)
    *(.rodata*)
  . = ALIGN(4);
     _edata = . ;
  }  > ram AT>flash
  .bss  SIZEOF(.data) + ADDR(.data) (NOLOAD) :
  {
     _bss_start = . ;
    *(.bss)
    *(COMMON)
     _end = . ;
  }  > ram
   __data_load_start = LOADADDR(.data);
   __data_load_end = __data_load_start + SIZEOF(.data);
   
   
  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }
  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }
}
