/**
    @file ft930_epfifo_registers.h

    @brief
    FT930 Endpoint FIFO registers

    
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

#ifndef __FT930_EPFIFO_REGS_H__
#define __FT930_EPFIFO_REGS_H__

#define BIT(x)			(1UL << (x))

#define FT930_BASEADDR_EPFIFO	0x10920
#define FT930_IRQ_EPFIFO	23

typedef struct _epfifo_len_t 
{
	uint32_t	rd;		/**< length in bytes of read fifo */
	uint32_t	wr;		/**< length in bytes of write fifo */
} epfifo_len_t;

typedef volatile struct _ft930_epfifo_t
{
	uint32_t	ctl;		/**< contains fifo reset bits */
	uint32_t	data[7];	/**< read accesses read FIFO and write accesses write FIFO, B */
	epfifo_len_t	len[7];		/**< struct containing alternating read and write lengths of FIFOs */
	uint32_t	grpisr;		/**< group level interrupt pending status, RO */
	uint32_t	isr[4];		/**< read and write FIFO interrupt status, RW1C */
	uint32_t	ien[4];		/**< read and write FIFO interrupt enables, RW */
	uint32_t	rng[4];		/**< slave only, 12-bit range registers for FIFO sizing, applies for each read & write pair */
	uint32_t	rsvd[(0x100-0x8C) >> 2];
} ft930_epfifo_t;

#define EPFIFO_CTL_RST_ALL	BIT(31)	/**< resets all FIFOs, master side control */
#define EPFIFO_CTL_EN		BIT(31)	/**< enables the FIFO block, slave side control */
#define EPFIFO_CTL_RST_FIFO(t,n)	do { (t)->ctl |= 1UL << ((n)&0x3F); for (int i = 0; i < 1000; i++); (t)->ctl &= ~(1UL << ((n)&0x3F)); } while (0)

#define EPFIFO_ISR_OV		BIT(5)
#define EPFIFO_ISR_F		BIT(4)
#define EPFIFO_ISR_HF		BIT(3)
#define EPFIFO_ISR_HE		BIT(2)
#define EPFIFO_ISR_E		BIT(1)
#define EPFIFO_ISR_UN		BIT(0)
#define EPFIFO_IEN_ENABLE_ALL	0x3FUL

/* bit-wise & is lower than <<, >> shift operations, ternary is lower than bitwise */
#define epfifo_get_rd_isr(t,i)		(((t)->isr[(i) >> 1] >> (((i) & 0x1) ? 16 : 0)) & 0xFFUL)
#define epfifo_get_wr_isr(t,i)		(((t)->isr[(i) >> 1] >> (((i) & 0x1) ? 24 : 8)) & 0xFFUL)
#define epfifo_clr_rd_isr(t,i,b)	((t)->isr[((i) >> 1)] = ((b)&0xFFUL) << (((i)&0x1) ? 16 : 0))
#define epfifo_clr_wr_isr(t,i,b)	((t)->isr[((i) >> 1)] = ((b)&0xFFUL) << (((i)&0x1) ? 24 : 8))

#define epfifo_get_rd_ien(t,i)		(((t)->ien[(i) >> 1] >> (((i) & 0x1) ? 16 : 0)) & 0xFFUL)
#define epfifo_get_wr_ien(t,i)		(((t)->ien[(i) >> 1] >> (((i) & 0x1) ? 24 : 8)) & 0xFFUL)
#define epfifo_set_rd_ien(t,i,b)	((t)->ien[(i) >> 1] |= ((b)&0xFFUL) << (((i)&0x1) ? 16 : 0))
#define epfifo_set_wr_ien(t,i,b)	((t)->ien[(i) >> 1] |= ((b)&0xFFUL) << (((i)&0x1) ? 24 : 8))

#define epfifo_get_rng(t,i)		(((t)->rng[(i) >> 1] >> (((i) & 0x1) ? 16 : 0)) & 0xFFFUL)
#define epfifo_set_rng(t,i,r)		do { \
						(t)->rng[(i) >> 1] &= ~(0xFFFUL << (((i)&0x1) ? 16 : 0));\
						(t)->rng[(i) >> 1] |= (((r) & 0xFFFUL) << (((i)&0x1) ? 16 : 0));} while (0)

#endif // __FT930_EPFIFO_REGS_H__
