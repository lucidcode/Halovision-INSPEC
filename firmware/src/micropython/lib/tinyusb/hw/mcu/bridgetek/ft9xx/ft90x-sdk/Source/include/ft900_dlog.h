#ifndef __FT900_DLOG_H__
#define __FT900_DLOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <ft900.h>

/**< 
 * @function	dlog_init
 * @brief	read one page of data 
 * @param	[in]	flashbuf	pointer to flash datalog partition
 * @param	[out]	pgsz		size of page on flash
 * @param	[out]	pages		number of pages in partition, pg=1..pages
 * @returns	0	on success
 * 		-1	if partition is invalid
 */
int dlog_init (__flash__ uint32_t *flashbuf, int *pgsz, int *pages);


/**< 
 * @function	dlog_erase
 * @brief	erases the datalog partition and writes the signature into page 0
 * @param	none
 * @returns	0 	datalog partition was erased
 * 		-1	datalog library has not been initialised
 */
int dlog_erase (void);


/**< 
 * @function	dlog_read
 * @brief	read one page of data 
 * @param	[in]	pg	page number, valid range 1..7
 * @param	[out]	data	32-bit pointer to buffer of page size length
 * @returns	0	on success
 * 		-1	if pg or data are invalid
 */
int dlog_read (int pg, uint32_t *data);


/**< 
 * @function	dlog_prog
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
