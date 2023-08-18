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
     _etext = . ;
    . = ALIGN(4);
    	
  }  > flash
  
  .tors :
  {
    ___ctors = . ;
    *(.ctors)
    ___ctors_end = . ;
    ___dtors = . ;
    *(.dtors)
    ___dtors_end = . ;
    . = ALIGN(4);
  } > ram
  
  .data	  : AT (ADDR (.text) + SIZEOF (.text))
  {
  	_sdata = .;
    *(.data)
    *(.rodata)
    *(.rodata*)
     _edata = . ;
    . = ALIGN(4);
  }  > ram
  


    .configs LOADADDR(.text) + SIZEOF(.text) + SIZEOF (.data) : AT (ADDR (.text) + SIZEOF (.text) + SIZEOF(.data))
  {
  
  	FILL(0xFF); /* Pad unused regions with 0xFF */
  	
  	/* If there's no SDBL/D2XX/LOGGER we don't update the location counter */
	. += (__sdbl_partition_sizeof ?  (__D2XX_partition_sizeof ? (__dlog_partition_sizeof ? LENGTH(flash)-0x5000 : LENGTH(flash)-0x4000) : (__dlog_partition_sizeof ? LENGTH(flash)-0x4000 : LENGTH(flash)-0x3000)): 
				(__D2XX_partition_sizeof ? (__dlog_partition_sizeof ? LENGTH(flash)-0x3000 : LENGTH(flash)-0x2000) : (__dlog_partition_sizeof ? LENGTH(flash)-0x2000 : SIZEOF(.text) + SIZEOF (.data)))) 
					- SIZEOF(.text) - SIZEOF (.data);
							
	*(._flash_dlog_partition)
	*(._flash_d2xx_config)
	*(._flash_sdbl_partition)
    . = ALIGN(4);
	
  }  > flash
  
  
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
