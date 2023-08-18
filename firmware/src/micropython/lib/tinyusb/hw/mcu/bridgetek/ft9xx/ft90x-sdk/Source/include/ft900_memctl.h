/**
    @file

    @brief
    FT900 memory controller driver header file

    
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
 
#ifndef __FT900_MEMCTL_H_
#define __FT900_MEMCTL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Use the boot/debug/memcopy interface to transfer a memory block.
 *
 *  The RAM address is the address in 32 bit words (i.e. byte address >> 2) and must be aligned
 *  to a 32 bit word boundary.
 *  The flash  address must always be aligned to a 256-byte boundary for the write case,
 *  and may be any value for the read case.
 *  When writing to flash the length must be a multiple of 256 byes.
 *
 *  @param command
 *  @param raddr RAM start address (program or data)
 *  @param faddr flash address
 *  @param length number of bytes to transfer
 */

void dbg_memory_copy (uint8_t command, uint32_t raddr, uint32_t faddr, uint32_t length);

/** @brief copy a memory block from program memory to flash.
 *  @param dst start address of flash memory
 *  @param src start address of program memory
 *  @param s number of bytes to transfer
 *
 */
 
void memcpy_pm2flash(uint32_t dst, const void *src, size_t s);

/** @brief copy a memory block from data memory to program memory
 *  @param dst address in program memory
 *  @param src the start address in data memory
 *  @param s the number of bytes to transfer
 */

void memcpy_dat2pm(uint32_t dst, const void *src, size_t s);

/** @brief copy a memory block from program memory to data memory
 *  @param dst start address in data memory
 *  @param src start address in program memory
 *  @param s the number of bytes to transfer
 */

void memcpy_pm2dat(void *dst, const __flash__ void* src, size_t s);

/** @brief copy a memory block from flash to data memory
 *  @param dst start address of data memory
 *  @param src start address of flash memory
 *  @param s number of bytes to transfer
 */

void memcpy_flash2dat(void *dst, uint32_t src, size_t s);

/** @brief copy a memory block from data memory to flash memory
 *  @param dst start address of flash memory
 *  @param src start address of data memory
 *  @param s number of bytes to transfer
 */
void memcpy_dat2flash(uint32_t dst, void *src, size_t s);

/** @brief copy a memory block from data memory to flash memory and reboot processor
 *  @param dst start address of flash memory
 *  @param src start address of data memory
 *  @param s number of bytes to transfer
 */
void memcpy_dat2flash_reboot(uint32_t dst, void *src, size_t s);

/** @brief Erase entire flash memory
 */
void flash_chip_erase(void);

/** @brief Erase a 64K block of flash memory
 *
 * The flash is arranged in 4 blocks of 65536 bytes \n
 * \n
 * block 0  corresponds to address 0x00000 to 0x0ffff \n
 * block 1  corresponds to address 0x00000 to 0x1ffff \n
 * block 2  corresponds to address 0x00000 to 0x2ffff \n
 * block 3  corresponds to address 0x00000 to 0x3ffff 
 *
 * @param block
 */

void flash_block_erase(int block);

/** @brief Erase a 4K block of flash
 *
 * the flash is arranged in 63 sectors of 4096 bytes. \n
 * sector 0  corresponds to address 0x00000 to 0x00fff \n
 * sector 1  corresponds to address 0x00000 to 0x01fff \n
 * sector 2  corresponds to address 0x00000 to 0x02fff \n
 * sector 62 corresponds to address 0x00000 to 0x3efff \n
 * sector .. corresponds to address ....... to ....... \n
 * sector 63 corresponds to address 0x00000 to 0x3ffff \n
 *
 * @param sector
 */

void flash_sector_erase(int sector);

/** @brief Restore Flash from Shadow RAM.
    @details Undo an incomplete firmware update.
 */
void flash_revert(void);

/** @brief Reboot entire device.
 */
void chip_reboot(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __FT900_MEMCTL_H_ */
