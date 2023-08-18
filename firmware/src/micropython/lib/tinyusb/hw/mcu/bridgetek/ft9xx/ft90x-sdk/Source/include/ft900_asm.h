/**
    @file

    @brief
    FT900 Assembler Macros

    
**/
/*
 * ============================================================================
 * History
 * =======
 *
 * 21 JAN 15: Fixed the memset.x commands since they were actually calling
 *            memcpy.x
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

#ifndef FT900_ASM_H_
#define FT900_ASM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

#if defined( __GNUC__)

/** @brief A No Operation Instruction */
#define asm_noop() __asm__ volatile("move.l $r0,$r0")

/** @brief 8-bitwise memory copy
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to copy */
#define asm_memcpy8(src, dst, size) __asm__ volatile("memcpy.b %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief 16-bitwise memory copy
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to copy */
#define asm_memcpy16(src, dst, size) __asm__ volatile("memcpy.s %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief 32-bitwise memory copy
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to copy */
#define asm_memcpy32(src, dst, size) __asm__ volatile("memcpy.l %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief 8-bitwise memory set
 *  @param val The value to set the memory to
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to set */
#define asm_memset8(val, dst, size) __asm__ volatile("memset.b %1, %0, %2" :: "r"(val), "r"(dst), "r"(size))

/** @brief 16-bitwise memory set
 *  @param val The value to set the memory to
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to set */
#define asm_memset16(val, dst, size) __asm__ volatile("memset.s %1, %0, %2" :: "r"(val), "r"(dst), "r"(size))

/** @brief 32-bitwise memory set
 *  @param val The value to set the memory to
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to set */
#define asm_memset32(val, dst, size) __asm__ volatile("memset.l %1, %0, %2" :: "r"(val), "r"(dst), "r"(size))

/** @brief String copy
 *  @param src A pointer to the source string
 *  @param dst A pointer to the destination string */
static inline void asm_strcpy(const char * src, char * dst ){
    __attribute__ ((__unused__)) int dummy;
    __asm__ volatile ("stpcpy.b %2, %1" :"=r" (dummy) : "r"(src),"0"(dst));
}

/** @brief 8-bitwise Memory stream from FIFO to Memory
 *  @param src A pointer to the source register
 *  @param dst A pointer to the destination data
 *  @param size The size of data to copy */
#define asm_streamin8(src, dst, size) __asm__ volatile("streamin.b %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief 16-bitwise Memory stream from FIFO to Memory
 *  @param src A pointer to the source register
 *  @param dst A pointer to the destination data
 *  @param size The size of data to copy */
#define asm_streamin16(src, dst, size) __asm__ volatile("streamin.s %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief 32-bitewise Memory stream from FIFO to Memory
 *  @param src A pointer to the source register
 *  @param dst A pointer to the destination data
 *  @param size The size of data to copy */
#define asm_streamin32(src, dst, size) __asm__ volatile("streamin.l %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief 8-bitwise Memory stream from Memory to FIFO
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination register
 *  @param size The size of data to copy */
#define asm_streamout8(src, dst, size) __asm__ volatile("streamout.b %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief 16-bitwise Memory stream from Memory to FIFO
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination register
 *  @param size The size of data to copy */
#define asm_streamout16(src, dst, size) __asm__ volatile("streamout.s %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief 32-bitwise Memory stream from Memory to FIFO
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination register
 *  @param size The size of data to copy */
#define asm_streamout32(src, dst, size) __asm__ volatile("streamout.l %1, %0, %2" :: "r"(src), "r"(dst), "r"(size))

/** @brief Set a bit in a 32 bit value
 *  @param val The value to use
 *  @param bit The bit position to set */
#define asm_setbit(val, bit) __asm__ volatile("bins.l %0, %0, %1", :: "r"(val), "r"(0x200 | 1 << 5 | bit));

/** @brief Clear a bit in a 32 bit value
 *  @param val The value to use
 *  @param bit The bit position to clear */
#define asm_clrbit(val, bit) __asm__ volatile("bins.l %0, %0, %1", :: "r"(val), "r"(0x000 | 1 << 5 | bit));

/** @brief Flip bit regions
 *
 *  For val:
 *  - If bit 0 is set, then every alternate bit is exchanged.
 *  - If bit 1 is set, then every alternate 2-bit group is exchanged.
 *  - If bit 2 is set, then every alternate 4-bit group is exchanged.
 *  - If bit 3 is set, then every alternate 8-bit group is exchanged.
 *  - If bit 4 is set, then the two 16-bit groups are exchanged.
 *
 *  For more information please refer to the Instruction Set Architecture documentation.
 *
 *  @param src The source data to use
 *  @param dst The destination for src
 *  @param val The selection of regions to flip */
#define asm_flip32(src, dst, val) __asm__ volatile("flip.l %1,%0,%2" :: "r"(src), "r"(dst), "r"(val))

/** @brief Reverse the endianness of a value
 *  @param val The value to use */
#define asm_reverse_endianness(val) asm_flip32(val, val, 24)

/** @brief Reverse the bits of a value
 *  @param val The value to use */
#define asm_reverse_bits(val) asm_flip32(val, val, 31)

/** @brief Rotate bits left or right
 *  @param val The value to rotate
 *  @param num The number and direction to rotate in (negative numbers rotate left) */
#define asm_rotate32(val, num) __asm__ volatile ("ror.l %0,%0,%1" :: "r"(val), "r"(num))

/** @brief Compare two strings
 *  @param str1 The first string to compare
 *  @param str2 The secong string to compare
 *  @return The difference between the two strings */
static inline int32_t asm_strcmp(const char* str1, const char* str2)
{
    register int32_t ret = 0;
    __asm__ volatile ("strcmp.b %0, %1, %2" : "=r"(ret) : "r"(str1), "r"(str2));
    return ret;
}

/** @brief Get the length of a string
 *  @param str The string to get the length of
 *  @return The length of the string */
static inline uint32_t asm_strlen(const char* str)
{
    register uint32_t ret = 0;
    __asm__ volatile ("strlen.b %0, %1" : "=r"(ret) : "r"(str));
    return ret;
}

#elif defined(__MIKROC_PRO_FOR_FT90x__)

/** @brief A No Operation Instruction */
#define asm_noop() asm {MOVE.L        R0, R0)

/** @brief 8-bitwise memory copy
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to copy */
#define asm_memcpy8(src, dst, size) MEMCPY_B(src, dst, size)

/** @brief 16-bitwise memory copy
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to copy */
#define asm_memcpy16(src, dst, size) MEMCPY_S(src, dst, size)

/** @brief 32-bitwise memory copy
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to copy */
#define asm_memcpy32(src, dst, size) MEMCPY_L(src, dst, size)

/** @brief 8-bitwise memory set
 *  @param val The value to set the memory to
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to set */
#define asm_memset8(val, dst, size) MEMSET_B(val, dst, size)

/** @brief 16-bitwise memory set
 *  @param val The value to set the memory to
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to set */
#define asm_memset16(val, dst, size) MEMSET_S(val, dst, size)

/** @brief 32-bitwise memory set
 *  @param val The value to set the memory to
 *  @param dst A pointer to the destination data
 *  @param size The size of the data to set */
#define asm_memset32(val, dst, size) MEMSET_L(val, dst, size)

/** @brief String copy
 *  @param src A pointer to the source string
 *  @param dst A pointer to the destination string */
#define asm_strcpy(src, dst) STRCPY_B(src, dst)

/** @brief 8-bitwise Memory stream from FIFO to Memory
 *  @param src A pointer to the source register
 *  @param dst A pointer to the destination data
 *  @param size The size of data to copy */
#define asm_streamin8(src, dst, size) STREAMIN_B(src, dst, size)

/** @brief 16-bitwise Memory stream from FIFO to Memory
 *  @param src A pointer to the source register
 *  @param dst A pointer to the destination data
 *  @param size The size of data to copy */
#define asm_streamin16(src, dst, size) STREAMIN_S(src, dst, size)

/** @brief 32-bitewise Memory stream from FIFO to Memory
 *  @param src A pointer to the source register
 *  @param dst A pointer to the destination data
 *  @param size The size of data to copy */
#define asm_streamin32(src, dst, size) STREAMIN_L(src, dst, size)

/** @brief 8-bitwise Memory stream from Memory to FIFO
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination register
 *  @param size The size of data to copy */
#define asm_streamout8(src, dst, size) STREAMOUT_B(src, dst, size)

/** @brief 16-bitwise Memory stream from Memory to FIFO
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination register
 *  @param size The size of data to copy */
#define asm_streamout16(src, dst, size) STREAMOUT_S(src, dst, size)

/** @brief 32-bitwise Memory stream from Memory to FIFO
 *  @param src A pointer to the source data
 *  @param dst A pointer to the destination register
 *  @param size The size of data to copy */
#define asm_streamout32(src, dst, size) STREAMOUT_L(src, dst, size)

/*inline void BINS_L(uint32_t val, uint32_t usebit)
{
    usebit |= (0x220);
    asm {
        BINS.L FARG_BINS_L_val, FARG_BINS_L_val, FARG_BINS_L_usebit;
    }
}
inline void FLIP_L(uint32_t src, uint32_t dst, uint32_t val)
{
    asm {
        FLIP.L FARG_FLIP_L_src, FARG_FLIP_L_dst, FARG_FLIP_L_val;
    }
}
*/
/** @brief Set a bit in a 32 bit value
 *  @param val The value to use
 *  @param bit The bit position to set */
#define asm_setbit(val, bit) BINS_L(val, bit)

/** @brief Clear a bit in a 32 bit value
 *  @param val The value to use
 *  @param bit The bit position to clear */
#define asm_clrbit(val, bit) BINS_L(val, bit)

/** @brief Flip bit regions
 *
 *  For val:
 *  - If bit 0 is set, then every alternate bit is exchanged.
 *  - If bit 1 is set, then every alternate 2-bit group is exchanged.
 *  - If bit 2 is set, then every alternate 4-bit group is exchanged.
 *  - If bit 3 is set, then every alternate 8-bit group is exchanged.
 *  - If bit 4 is set, then the two 16-bit groups are exchanged.
 *
 *  For more information please refer to the Instruction Set Architecture documentation.
 *
 *  @param src The source data to use
 *  @param dst The destination for src
 *  @param val The selection of regions to flip */
#define asm_flip32(src, dst, val) ASM{}

/** @brief Reverse the endianness of a value
 *  @param val The value to use */
#define asm_reverse_endianness(val) asm_flip32(val, val, 24)

/** @brief Reverse the bits of a value
 *  @param val The value to use */
#define asm_reverse_bits(val) asm_flip32(val, val, 31)

/** @brief Rotate bits left or right
 *  @param val The value to rotate
 *  @param num The number and direction to rotate in (negative numbers rotate left) */
#define asm_rotate32(val, num) ASM{}

/** @brief Compare two strings
 *  @param str1 The first string to compare
 *  @param str2 The secong string to compare
 *  @return The difference between the two strings */
#define asm_strcmp(src, dst) STRCMP_B(src, dst)

/** @brief Get the length of a string
 *  @param str The string to get the length of
 *  @return The length of the string */
#define asm_strlen(str) STRLEN_B(str)

#endif // __MIKROC_PRO_FOR_FT90x__

/* FUNCTION PROTOTYPES *************************************************************/




#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_ASM_H_ */