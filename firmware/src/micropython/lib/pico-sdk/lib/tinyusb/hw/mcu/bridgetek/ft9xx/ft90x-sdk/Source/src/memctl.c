/**
    @file

    @brief
    driver for FT900 debug/flash/program/data memory transfers


 **/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <string.h>

#include <ft900.h>

/* CONSTANTS ***********************************************************************/

/**
 @name Debugger/programmer interface commands.
 @details The FT900 has a Flash device similar to a MX25L2006E. There are
    extensions to the command interface to provide direct memory and
    program data accesses to and from the Flash.
 */
//@{
/**
 @brief Initiates data transfer from Program Memory to Flash Memory.
 @details The start address of the Program Memory destination will be
	dictated by RSADDRx which must be 32-bit aligned.
	The start address of the Flash Memory destination will be
	dictated by FSADDRx which must be 256-byte aligned.
	The number of bytes to transfer will be dictated by BLENGTHx
	and must be multiples of 256 bytes
 */
#define	DBG_CMDP2F1		0xf0
/**
 @brief Initiates data transfer from Flash Memory to Program Memory.
 @details The start address of the Program Memory destination will be
	dictated by RSADDRx which must be 32-bit aligned.
	The start address of the Flash Memory destination will be
	dictated by FSADDRx which must be 256-byte aligned.
	The number of bytes to transfer will be dictated by BLENGTHx.
 */
#define	DBG_CMDF2P1		0xf4
/**
 @brief Initiates data transfer from Data Memory to Flash Memory.
 @details The start address of the Data Memory destination will be
	dictated by RSADDRx which must be 32-bit aligned.
	The start address of the Flash Memory destination will be
	dictated by FSADDRx which must be 256-byte aligned.
	The number of bytes to transfer will be dictated by BLENGTHx
	and must be multiples of 256 bytes.
 */
#define	DBG_CMDD2F1		0xf8
/**
 @brief Initiates data transfer from Data Memory to Flash Memory.
 @details Similar to CMDD2F1 except at the end of the transfer, a system
	reboot will be performed.
 */
#define	DBG_CMDD2F3		0xfa
/**
 @brief Initiates data transfer from Flash Memory to Data Memory.
 @details The start address of the Data Memory destination will be
	dictated by RSADDRx which must be 32-bit aligned.
	The start address of the Flash Memory destination will be
	dictated by FSADDRx which must be 256-byte aligned.
	The number of bytes to transfer will be dictated by BLENGTHx.
 */
#define	DBG_CMDF2D1		0xfc
/**
 @brief Initiates data transfer from Flash Memory to Data Memory.
 @details Similar to DBG_CMDF2D1 except at the end of the transfer, a system
	reboot will be performed.
 */
#define	DBG_CMDF2D3		0xfe

/**
 @brief Sets the Write Enable Latch
 @details The Write Enable (WREN) instruction is for setting
	the Write Enable Latch (WEL) bit. Those
	instructions such as PP, SE,
	BE, CE, and WRSR, which are intended to change
	the device content, should be set every time after
	the WREN instruction sets the WEL bit.
 */
#define DBG_CMDWREN		0x06
/**
 @brief Sector erase
 @details Sector address can be set at FSADDRx.
 */
#define DBG_CMDSE		0x20
/**
 @brief Block erase
 @details Block address can be set at FSADDRx.
 */
#define DBG_CMDBE1		0x52
/**
 @brief Chip erase
 */
#define DBG_CMDCE1		0x60
//@}

/** @name Debugger/programmer status register bits.
 */
//@{
/**
 @brief Flash interface controller busy.
 */
#define DBG_CONTROL_BUSY	0x40
//@}

/* MACROS **************************************************************************/

#define PMCONTROL_UNLOCK ((volatile uint32_t *)0x1fc80)
#define PMCONTROL_ADDR	((volatile uint32_t *)0x1fc84)
#define PMCONTROL_DATA	((volatile uint32_t *)0x1fc88)

#define DBG_RSADDR0     ((volatile uint8_t *)0x10800)
#define DBG_RSADDR1     ((volatile uint8_t *)0x10801)
#define DBG_RSADDR2     ((volatile uint8_t *)0x10802)
#define DBG_FSADDR0     ((volatile uint8_t *)0x10803)
#define DBG_FSADDR1     ((volatile uint8_t *)0x10804)
#define DBG_FSADDR2     ((volatile uint8_t *)0x10805)
#define DBG_BLENGTH0    ((volatile uint8_t *)0x10806)
#define DBG_BLENGTH1    ((volatile uint8_t *)0x10807)
#define DBG_BLENGTH2    ((volatile uint8_t *)0x10808)
#define DBG_COMMAND     ((volatile uint8_t *)0x10809)
#define DBG_SEMAPHORE   ((volatile uint8_t *)0x1080b)
#define DBG_CONFIG      ((volatile uint8_t *)0x1080c)
#define DBG_STATUS      ((volatile uint8_t *)0x1080d)
#define DBG_DRWDATA     ((volatile uint8_t *)0x10880)

/*Sector size (in bytes)*/
#define SECTOR_SIZE	(4096)
/*Sector block (in bytes)*/
#define BLK_SIZE	(SECTOR_SIZE * 16)
/*Flash size (in bytes)*/
#if defined(__FT900__)
#define MAX_FLASH_SIZE	0X40000
#elif defined(__FT930__)
#define MAX_FLASH_SIZE	0X20000
#endif
/*Number of sectors in flash*/
#define MAX_SECTOR_NUM 	(MAX_FLASH_SIZE / SECTOR_SIZE)

/*Number of blocks in flash*/
#define MAX_BLK_NUM 	(MAX_FLASH_SIZE / BLK_SIZE)
/* LOCAL FUNCTIONS / INLINES *******************************************************/

static int get_debugger(void)
{
	int     count;

	count = 100000;

	while(count--)
	{
		__asm__(""::: "memory");
		if(*DBG_SEMAPHORE == 0)
			return 1;
	}

	return 0;
}

static void release_debugger(void)
{
	__asm__(""::: "memory");
	*DBG_SEMAPHORE = 1;
}

/* FUNCTIONS ***********************************************************************/

/** @brief use the boot/debug/memcopy interface to transfer a memory block.
 *
 * The RAM address is the address in 32 bit words (i.e. byte address >> 2) and must be aligned
 * to a 32 bit word boundary.
 * The flash  address must always be aligned to a 256-byte boundary for the write case,
 * and may be any value for the read case.
 * When writing to flash the length must be a multiple of 256 byes.
 *
 *  @param raddr RAM start address (program or data)
 *  @param faddr flash address 
 *  @param length number of bytes to transfer
 *  @returns nothing
 */

#if defined(__GNUC__)
#pragma GCC optimize 0  /* don't let the optimizer loose on this */
#endif // defined(__GNUC__)

void dbg_memory_copy (uint8_t command, uint32_t raddr, uint32_t faddr, uint32_t length)
{
	union {
		uint8_t  bytes[4];
		uint32_t word;
	} reg;

	volatile uint8_t	st;

	get_debugger();

	/* Check if debugger is ready (Status register : control busy is 0 when ready) */
	while ((st = *DBG_STATUS) & DBG_CONTROL_BUSY) {
	}

	/* Setup ram start address */
	reg.word = raddr;
	*DBG_RSADDR0 = reg.bytes[0];
	*DBG_RSADDR1 = reg.bytes[1];
	*DBG_RSADDR2 = reg.bytes[2];

	/* Setup flash start address */
	reg.word = faddr;
	*DBG_FSADDR0 = reg.bytes[0];
	*DBG_FSADDR1 = reg.bytes[1];
	*DBG_FSADDR2 = reg.bytes[2];

	/* Setup length to be transferred */
	reg.word = length;
	*DBG_BLENGTH0 = reg.bytes[0];
	*DBG_BLENGTH1 = reg.bytes[1];
	*DBG_BLENGTH2 = reg.bytes[2];

	/* Enable Flash to be written, command send to Flash memory, */
	*DBG_COMMAND = DBG_CMDWREN;

	/* Check if debugger is done (Status register : control busy is 0 when ready) */
	while ((st = *DBG_STATUS) & DBG_CONTROL_BUSY) {
	}

	/* Initiate transfer */
	*DBG_COMMAND = command;

	/* Check if debugger is done (Status register : control busy is 0 when ready) */
	while ((st = *DBG_STATUS) & DBG_CONTROL_BUSY) {
	}

	release_debugger();
}

#if defined(__GNUC__)
#pragma GCC reset_options
#endif // defined(__GNUC__)

/** @brief copy a memory block from data memory to program memory
 *  @param dst address in program memory
 *  @param src the start address in data memory
 *  @param len the number of bytes to transfer
 */

void memcpy_dat2pm(uint32_t dst, const void *src, size_t s)
{
	*PMCONTROL_UNLOCK = 0x1337f7d1;

	*PMCONTROL_ADDR = dst;

	__asm__("streamout.l %0,%1,%2" : :"r"(PMCONTROL_DATA), "r"(src), "r"(s): "memory");

	*PMCONTROL_UNLOCK = 0x00000000;
}

/******************************************************************************
 *
 * Function Name:  manualCopy
 *
 * Description: Manually copies data from memory to memory.  This is used by
 * sysFastMemCopy to copy a few lingering bytes at the beginning and end.
 *
 *****************************************************************************/
 static inline void __attribute__((optimize("O0")))manualCopy( uint8_t *pDest, const __flash__ uint8_t *pSrc, uint32_t len )
{
	 uint32_t i;

    // Manually copy the data
    for ( i = 0; i < len; i++ )
    {
        // Copy data from source to destination
        *pDest++ = *pSrc++;
    }
}

/******************************************************************************
  *
  * Function Name:  memcpy_pm2dat
  *
  * @brief copy a memory block from program memory to data memory
  *  @param dst start address in data memory
  *  @param src start address in program memory
  *  @param len the number of bytes to transfer
  *  @returns nothing
  *
  * Description: Assuming that your processor can do 32-bit memory accesses
  * and contains a barrel shifter and that you are using an efficient
  * compiler, then this memory-to-memory copy procedure will probably be more
  * efficient than just using the traditional memcpy procedure if the number
  * of bytes to copy is greater than about 20.  It works by doing 32-bit
  * reads and writes instead of using 8-bit memory accesses.
  *
  * NOTE that this procedure assumes a Little Endian processor!  The shift
  * operators ">>" and "<<" should all be reversed for Big Endian.
  *
  * NEVER use this when the number of bytes to be copied is less than about
  * 10, since it may not work for a small number of bytes.  Also, do not use
  * this when the source and destination regions overlap.
  *
  * NOTE that this may NOT be faster than memcpy if your processor supports a
  * really fast cache memory!
  *
  * Timing for this sysFastMemCopy varies some according to which shifts need
  * to be done.  The following results are from one attempt to measure timing
  * on a Cortex M4 processor running at 48 MHz.
  *
  *                           MEMCPY        FAST
  *                  BYTES  bytes/usec   bytes/usec
  *                  -----  ----------  ------------
  *                    50       4.2      6.3 to  6.3
  *                   100       4.5      8.3 to 10.0
  *                   150       4.8     10.0 to 11.5
  *                   200       4.9     10.5 to 12.5
  *                   250       5.1     11.4 to 13.2
  *                   300       5.1     11.5 to 13.6
  *                   350       5.1     12.1 to 14.6
  *                   400       5.1     12.1 to 14.8
  *                   450       5.2     12.2 to 15.5
  *                   500       5.2     12.5 to 15.2
  *
  * The following macro can be used instead of memcpy to automatically select
  * whether to use memcpy or sysFastMemCopy:
  *
  *   #define MEMCOPY(pDst,pSrc,len) \
  *     (len) < 16 ? memcpy(pDst,pSrc,len) : sysFastMemCopy(pDst,pSrc,len);
  *
  *****************************************************************************/
void __attribute__((optimize("O0"))) memcpy_pm2dat( void *dest, const __flash__ void *src, size_t len )
{
    uint32_t srcCnt;
    uint32_t destCnt;
    uint32_t newLen;
    uint32_t endLen;
    uint32_t longLen;
    __flash__ uint32_t *pLongSrc;
    uint32_t *pLongDest;
    uint32_t longWord1;
    uint32_t longWord2;
    uint32_t methodSelect;
    uint8_t *pDest = (uint8_t*)dest;
    const __flash__ uint8_t *pSrc = (const __flash__ uint8_t *)src;

    // If the size is smaller or equal to 4 bytes, just to normal copy and done
    if (4 >= len) {
    	manualCopy( pDest, pSrc, len );
    	return;
    }
    // Determine the number of bytes in the first word of src and dest
    srcCnt = 4 - ( (uint32_t) pSrc & 0x03 );
    destCnt = 4 - ( (uint32_t) pDest & 0x03 );

    // Copy the initial bytes to the destination
    manualCopy( pDest, pSrc, destCnt );

    // Determine the number of bytes remaining
    newLen = len - destCnt;

    // Determine how many full long words to copy to the destination
    longLen = newLen / 4;

    // Determine number of lingering bytes to copy at the end
    endLen = newLen & 0x03;

    // Pick the initial long destination word to copy to
    pLongDest = (uint32_t*) __builtin_assume_aligned(( pDest + destCnt ), sizeof(uint32_t));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
    // Pick the initial source word to start our algorithm at
    if ( srcCnt <= destCnt )
    {
        // Advance to pSrc at the start of the next full word
        pLongSrc = (__flash__ uint32_t*) ( pSrc + srcCnt );
    }
    else // There are still source bytes remaining in the first word
    {
        // Set pSrc to the start of the first full word
        pLongSrc = (__flash__ uint32_t*) ( pSrc + srcCnt - 4 );
    }
#pragma GCC diagnostic pop

    // There are 4 different longWord copy methods
    methodSelect = ( srcCnt - destCnt ) & 0x03;

    // Just copy one-to-one
    if ( methodSelect == 0 )
    {
        // Just copy the specified number of long words
        while ( longLen-- > 0 )
        {
            *pLongDest++ = *pLongSrc++;
        }
    }
    else if ( methodSelect == 1 )
    {
        // Get the first long word
        longWord1 = *pLongSrc++;

        // Copy words created by combining 2 adjacent long words
        while ( longLen-- > 0 )
        {
            // Get the next 32-bit word
            longWord2 = *pLongSrc++;

            // Write to the destination
            *pLongDest++ = ( longWord1 >> 24 ) | ( longWord2 << 8 );

            // Re-use the word just retrieved
            longWord1 = longWord2;
        }
    }
    else if ( methodSelect == 2 )
    {
        // Get the first long word
        longWord1 = *pLongSrc++;

        // Copy words created by combining 2 adjacent long words
        while ( longLen-- > 0 )
        {
            // Get the next 32-bit word
            longWord2 = *pLongSrc++;

            // Write to the destination
            *pLongDest++ = ( longWord1 >> 16 ) | ( longWord2 << 16 );

            // Re-use the word just retrieved
            longWord1 = longWord2;
        }
    }
    else // ( methodSelect == 3 )
    {
        // Get the first long word
        longWord1 = *pLongSrc++;

        // Copy words created by combining 2 adjacent long words
        while ( longLen-- > 0 )
        {
            // Get the next 32-bit word
            longWord2 = *pLongSrc++;

            // Write to the destination
            *pLongDest++ = ( longWord1 >> 8 ) | ( longWord2 << 24 );

            // Re-use the word just retrieved
            longWord1 = longWord2;
        }
    }

    // Copy any remaining bytes
    if ( endLen != 0 )
    {
        // The trailing bytes will be copied next
        pDest = (uint8_t*) pLongDest;

        // Determine where the trailing source bytes are located
        pSrc += len - endLen;

        // Copy the remaining bytes
        manualCopy( pDest, pSrc, endLen );
    }
}

/** @brief copy a memory block from flash to data memory 
 *  @param dst start address of data memory
 *  @param src start address of flash memory
 *  @param s number of bytes to transfer
 *  @returns nothing
 */

void memcpy_flash2dat(void *dst, uint32_t src, size_t s)
{
	if(s == 0) return; /* Invalid */

	dbg_memory_copy(DBG_CMDF2D1, (uint32_t)dst >> 2, src, --s);
}

/** @brief copy a memory block from data memory to flash memory 
 *  @param dst start address of flash memory
 *  @param src start address of data memory
 *  @param s number of bytes to transfer
 *  @returns nothing
 */

void memcpy_dat2flash(uint32_t dst, void *src, size_t s)
{
	if(s == 0) return; /* Invalid */

	dbg_memory_copy(DBG_CMDD2F1, (uint32_t)src >> 2, dst, --s);
}

/** @brief copy a memory block from data memory to flash memory and reboot processor
 *  @param dst start address of flash memory
 *  @param src start address of data memory
 *  @param s number of bytes to transfer
 *  @returns nothing
 *
 */

void memcpy_dat2flash_reboot(uint32_t dst, void *src, size_t s)
{
	if(s == 0) return; /* Invalid */

	dbg_memory_copy(DBG_CMDD2F3, (uint32_t)src >> 2, dst, --s);
}

/** @brief copy a memory block from program memory to flash.
 *  @param dst start address of flash memory
 *  @param src start address of program memory
 *  @param s number of bytes to transfer
 *  @returns nothing
 *
 */

void memcpy_pm2flash(uint32_t dst, const void *src, size_t s)
{
	if(s == 0) return; /* Invalid */

	dbg_memory_copy(DBG_CMDP2F1, (uint32_t)src >> 2, dst, --s);
}


/* flash sector, block and chip erase */

/* @brief Erase a 64K block of flash memory
 *
 * The flash is arranged in 4 blocks of 65536 bytes
 *
 * block 0  corresponds to address 0x00000 to 0x0ffff
 * block 1  corresponds to address 0x00000 to 0x1ffff
 * block 2  corresponds to address 0x00000 to 0x2ffff
 * block 3  corresponds to address 0x00000 to 0x3ffff
 *
 * @param none
 * @returns nothing
 */

void flash_block_erase(int block)
{
	if(block > (MAX_BLK_NUM - 1))
		return;

	dbg_memory_copy(DBG_CMDBE1, 0, block << 16, 0);
}

/** @brief Erase entire flash memory
 *  @param none
 *  @returns nothing
 */

void flash_chip_erase(void)
{
#if defined(__FT900__)
	dbg_memory_copy(DBG_CMDCE1, 0, 0, 0);
#elif defined(__FT930__)
	int i;
	for(i = 0; i < MAX_BLK_NUM; i++)
	{
		flash_block_erase(i);
	}
#endif
}

/* @brief Erase a 4K block of flash
 *
 * the flash is arranged in 63 sectors of 4096 bytes.
 * sector 0  corresponds to address 0x00000 to 0x00fff
 * sector 1  corresponds to address 0x00000 to 0x01fff
 * sector 2  corresponds to address 0x00000 to 0x02fff
 * sector 62 corresponds to address 0x00000 to 0x3efff
 * sector .. corresponds to address ....... to .......
 * sector 63 corresponds to address 0x00000 to 0x3ffff
 *
 * @param none
 * @returns nothing
 */

void flash_sector_erase(int sector)
{
	if(sector > MAX_SECTOR_NUM - 1)
		return;

	dbg_memory_copy(DBG_CMDSE, 0, sector << 12, 0);
}

void chip_reboot(void)
{
	dbg_memory_copy(DBG_CMDF2D3, 0, 0, 255);
}

void flash_revert(void)
{
    /* do it sector by sector */
    int    i;

    for(i = 0; i < MAX_SECTOR_NUM; i++)
    {
        flash_sector_erase(i);
        memcpy_pm2flash(i * SECTOR_SIZE, (void *)(i * SECTOR_SIZE), SECTOR_SIZE);
    }
}
