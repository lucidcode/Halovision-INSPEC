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
