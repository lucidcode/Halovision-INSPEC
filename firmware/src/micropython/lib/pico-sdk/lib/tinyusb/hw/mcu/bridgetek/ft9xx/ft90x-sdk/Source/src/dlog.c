/**
    @file datalogger.c

    @brief API for a simple datalogger service

    @version 1.0.0

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
#include <stdlib.h>

/* INCLUDES ************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include <ft900_memctl.h>
#include "ft900_dlog.h"

/* TEST CONFIGURATION CONSTANTS ******************************************************************/
#undef DEBUG
/* TEST CONFIGURATION CONSTANTS ******************************************************************/


#ifdef DEBUG
#include "tinyprintf.h"
#define dbg(s,...)	tfp_printf ((s), __VA_ARGS__)
#else
#define dbg(s,...)
#endif

/* CONSTANTS ***********************************************************************/

/**
 @brief Exponents to describe flash geometry
 */
#define FLASH_SIZE_PAGE_EXP 	8	/**< 256 bytes per page */
#define FLASH_SIZE_SECTOR_EXP	4	/**< 16 pages per sector */
#define FLASH_SIZE_BLOCK_EXP	4	/**< 16 sectors per block */
#define FLASH_SIZE_EXP		2	/**< 4 blocks per flash */

/**
 @brief Page write size for Flash.
 @details The size of the page is defined for the EEPROM.
 	 A page is the smallest area of Flash that can be
 	 programmed. All programming operations must be aligned
 	 on a Flash page and be a multiple of the Flash page in
 	 length.
 */

#define FLASH_SIZE_PAGE 	(1UL << FLASH_SIZE_PAGE_EXP)

/**
 @brief Sector size for Flash.
 @details The size of the sector is defined by the EEPROM.
 	 This is the smallest area of Flash that can be erased.
 */
#define FLASH_SIZE_SECTOR 	(FLASH_SIZE_PAGE << FLASH_SIZE_SECTOR_EXP)

/**
    @brief There are 16 sectors in a block.
    @details A whole block can be erased.
 */
#define FLASH_SIZE_BLOCK	(FLASH_SIZE_SECTOR << FLASH_SIZE_BLOCK_EXP)


/**
    @brief There are 4 blocks in the flash.
 */
#define FLASH_SIZE		(FLASH_SIZE_BLOCK << FLASH_SIZE_EXP)

/**
 @brief Data memory alignment size required by Flash.
 */
#define MEMORY_ALIGN_SIZE 4

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

#ifndef BIT
#define BIT(x)			(1UL << (x))
#endif

#define FLASH_IMG_DLOG		BIT(10)
#define FLASH_IMG_D2XX		BIT(9)
#define FLASH_IMG_SDBL		BIT(8)

#define FLASH_IMG_CHIP_MASK	(BIT(1)|BIT(0))
#define FLASH_IMG_CHIP_FT900	0
#define FLASH_IMG_CHIP_FT930	1
#define FLASH_IMG_CHIP_FT980	2

#define SIG_IMG_DLOG	0xF7D1D106
#define SIG_IMG_D2XX	0xF7D1DCF6
#define SIG_IMG_SDBL	0xF7D15DB1
#define SIG_IMG_BDFU	0xF7D10403

#define MAX_DLOG_PAGE	((1 << FLASH_SIZE_SECTOR_EXP) - 2)

static __flash__ uint32_t	*dlog_addr;

/**< 
 * @function	dlog_init
 * @brief	read one page of data 
 * @param	[in]	flashbuf	pointer to flash datalog partition
 * @param	[out]	pgsz		size of page on flash
 * @param	[out]	pages		number of pages in partition, pg=1..pages
 * @returns	0	on success
 * 		-1	if partition is invalid
 */
int dlog_init (__flash__ uint32_t *flashbuf, int *pgsz, int *pages)
{
	__flash__	uint32_t *d = flashbuf;

	if (!(uint32_t)d || ((uint32_t)d & 0x03UL))
	{
		return -1;
	}

#if 1
	if (*d != SIG_IMG_DLOG)
	{
		dbg ("dlog_init: invalid signature: 0x%lx\n", *d);
		return -1;
	}
#endif

	dlog_addr = flashbuf;
	*pgsz = FLASH_SIZE_PAGE;
	*pages = (1 << FLASH_SIZE_SECTOR_EXP) - 2;

	return 0;
}


/**< 
 * @function	dlog_read
 * @brief	read one page of data 
 * @param	[in]	pg	page number, valid range 0..13
 * @param	[out]	data	32-bit pointer to buffer of page size length
 * @returns	0	on success
 * 		-1	if pg or data are invalid
 */

int dlog_read (int pg, uint32_t *data)
{
	if (pg < 0 || pg > MAX_DLOG_PAGE || !data || ((uint32_t) data & 0x03UL))
		return -1;

	CRITICAL_SECTION_BEGIN
	{
		memcpy_flash2dat (data, (uint32_t)dlog_addr+((pg+1) << FLASH_SIZE_PAGE_EXP), FLASH_SIZE_PAGE);
	}
	CRITICAL_SECTION_END;

	return 0;
}

/**< 
 * @function	dlog_prog
 * @brief	program one page of data 
 * @param	[in]	pg	page number, valid range 0..13
 * @param	[in]	data	32-bit pointer to buffer of page size length
 * @returns	0	on success
 * 		-1	if pg or data are invalid
 */
int dlog_prog (int pg, uint32_t *data)
{
	if (pg < 0 || pg > MAX_DLOG_PAGE || !data || ((uint32_t) data & 0x03UL))
		return -1;

	CRITICAL_SECTION_BEGIN
	{
		memcpy_dat2flash ((uint32_t)dlog_addr+((pg+1) << FLASH_SIZE_PAGE_EXP), data, FLASH_SIZE_PAGE);
	}
	CRITICAL_SECTION_END;

	return 0;
}

/**< 
 * @function	dlog_erase
 * @brief	erases the datalog partition and writes the signature into page 0
 * @param	none
 * @returns	0 	datalog partition was erased
 * 		-1	datalog library has not been initialised
 */
int dlog_erase (void)
{
	uint32_t	d[FLASH_SIZE_PAGE >> 2];
	uint32_t	i;

	if (!dlog_addr)
		return -1;

	CRITICAL_SECTION_BEGIN
	{
		flash_sector_erase ((uint32_t)dlog_addr >> (FLASH_SIZE_PAGE_EXP + FLASH_SIZE_SECTOR_EXP));
	}
	CRITICAL_SECTION_END;

	for (i = 0; i < (FLASH_SIZE_PAGE >> 2); i++)
		d[i] = 0xAA55AA55;

	d[0] = SIG_IMG_DLOG;

	CRITICAL_SECTION_BEGIN
	{
		memcpy_dat2flash ((uint32_t)dlog_addr, d, FLASH_SIZE_PAGE);
		d[0] = 0xAA55AA55;
		d[(FLASH_SIZE_PAGE >> 2)-1] = SIG_IMG_BDFU;
		memcpy_dat2flash ((uint32_t)dlog_addr+FLASH_SIZE_SECTOR-FLASH_SIZE_PAGE, d, FLASH_SIZE_PAGE);
	}
	CRITICAL_SECTION_END;

	return 0;
}

inline uint32_t dlog_get_page_size (void) {return FLASH_SIZE_PAGE;}

/* end */
