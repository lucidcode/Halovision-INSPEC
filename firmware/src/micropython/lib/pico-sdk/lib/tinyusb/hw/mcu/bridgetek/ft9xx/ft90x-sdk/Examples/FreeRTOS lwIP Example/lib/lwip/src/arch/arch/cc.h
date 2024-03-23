/**
  @file cc.h
 */
/*
 * ============================================================================
 * History
 * =======
 * 2017-03-15 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
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

#ifndef CC_H_
#define CC_H_

#include <stdint.h>
#include <stdlib.h>

#if defined(__FT32__)
#include <ft900.h>
#include <ft900_asm.h>
#endif // defined(__FT32__)

//#define CC_DEBUG
#ifdef CC_DEBUG
#undef TINYPRINTF_OVERRIDE_LIBC
#define TINYPRINTF_OVERRIDE_LIBC 0
#include "tinyprintf.h"
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

typedef uint8_t     u8_t;
typedef int8_t      s8_t;
typedef uint16_t    u16_t;
typedef int16_t     s16_t;
typedef uint32_t    u32_t;
typedef int32_t     s32_t;

typedef uintptr_t   mem_ptr_t;

#define LWIP_ERR_T  int

#if defined(__FT32__)

/*#define LWIP_PLATFORM_BYTESWAP 1*/
/*#define LWIP_PLATFORM_HTONS(x) ( (((u16_t)(x))>>8) | (((x)&0xFF)<<8) )*/
/*#define LWIP_PLATFORM_HTONL(x) ( (((u32_t)(x))>>24) | (((x)&0xFF0000)>>8) \
                               | (((x)&0xFF00)<<8) | (((x)&0xFF)<<24) ) */

// load byte by byte, construct 16 bits word and add: not efficient for most platforms
//#define LWIP_CHKSUM_ALGORITHM 1
// load first byte if odd address, loop processing 16 bits words, add last byte.
//#define LWIP_CHKSUM_ALGORITHM 2
// load first byte and word if not 4 byte aligned, loop processing 32 bits words, add last word/byte.
#define LWIP_CHKSUM_ALGORITHM 3

// Use this to use a platform version of the checksum calculation
//#define LWIP_CHKSUM my_chksum

#define PACK_STRUCT_FIELD(x)  __attribute__ ((aligned (1))) x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_USE_INCLUDES
// see bpstruct.h and epstruct.h
//#define PACK_STRUCT_BEGIN
//#define PACK_STRUCT_END

// Diagnostic output
#ifdef CC_DEBUG
#define LWIP_PLATFORM_DIAG(x) tfp_printf x
#define LWIP_PLATFORM_ASSERT(x) do { tfp_printf("Assertion Failed %s:%d:%s\n",__FILE__,__LINE__,x); for(;;); } while(0)
#endif

#define LWIP_RAND() ((u32_t)rand())

#else
#define PACK_STRUCT_FIELD(x)  x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_USE_INCLUDES
// Diagnostic output
#define LWIP_PLATFORM_DIAG(x) 
#define LWIP_PLATFORM_ASSERT(x) 
#endif // defined(__FT32__)

#define U8_F    "u"
#define X8_F    "02x"
#define U16_F   "u"
#define S16_F   "d"
#define X16_F   "x"
#define U32_F   "lu"
#define S32_F   "ld"
#define X32_F   "lx"
#define SZT_F   "uz"

#define LWIP_TIMEVAL_PRIVATE 0

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//u16_t my_chksum(void *dataptr, u16_t len);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CC_H_ */
