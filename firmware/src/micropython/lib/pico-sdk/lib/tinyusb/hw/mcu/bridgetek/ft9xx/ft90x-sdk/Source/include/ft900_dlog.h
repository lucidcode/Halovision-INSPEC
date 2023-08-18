/**
    @file ft900_dlog.h

    @brief
    Data logger.

    
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
#ifndef __FT900_DLOG_H__
#define __FT900_DLOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <ft900.h>

/** 
 * @brief	read one page of data 
 * @param	[in]	flashbuf	pointer to flash datalog partition
 * @param	[out]	pgsz		size of page on flash
 * @param	[out]	pages		number of pages in partition, pg=1..pages
 * @returns	0	on success
 * 		-1	if partition is invalid
 */
int dlog_init (__flash__ uint32_t *flashbuf, int *pgsz, int *pages);


/** 
 * @brief	erases the datalog partition and writes the signature into page 0
 * @returns	0 	datalog partition was erased
 * 		-1	datalog library has not been initialised
 */
int dlog_erase (void);


/** 
 * @brief	read one page of data 
 * @param	[in]	pg	page number, valid range 1..7
 * @param	[out]	data	32-bit pointer to buffer of page size length
 * @returns	0	on success
 * 		-1	if pg or data are invalid
 */
int dlog_read (int pg, uint32_t *data);


/** 
 * @brief	program one page of data 
 * @param	[in]	pg	page number, valid range 1..7
 * @param	[in]	data	32-bit pointer to buffer of page size length
 * @returns	0	on success
 * 		-1	if pg or data are invalid
 */
int dlog_prog (int pg, uint32_t *data);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif	// __FT900_DLOG_H__

/* end */
