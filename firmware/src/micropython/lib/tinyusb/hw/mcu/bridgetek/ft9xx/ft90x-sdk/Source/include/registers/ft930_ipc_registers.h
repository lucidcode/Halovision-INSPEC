#ifndef __FT930_IPC_REGS_H__
#define __FT930_IPC_REGS_H__

/** \brief ft930_ipc_t describes the source and destination registers in the master and slave. 
 *
 * Triggers from the master source registers are received at the slave destination registers and 
 * vice versa.
 */
typedef volatile struct _ft930_ipc_t
{
	uint32_t	reqtrg;		/**< source request trigger */
	uint32_t	rspien;		/**< source rsp interrupt enable */
	uint32_t	rspisr;		/**< source rsp interrupt status */
	uint32_t	srcrst;		/**< source regs reset */

	uint32_t	reqien;		/**< destinaton request interrupt enable */
	uint32_t	reqisr;		/**< destination request interrupt status */
	uint32_t	rspten;		/**< destination response interrupt trigger enable */
	uint32_t	dstrst;		/**< destination regs reset */
} ft930_ipc_t;

#define FT930_IPC_WRMBX			0x13000		/**< mailbox address for m2s, write only */
#define FT930_IPC_RDMBX			0x13000		/**< mailbox address for s2m, read only */

#define FT930_IPC_REG_BASE		0x108D0		/**< base address of IPC registers in FT930 */

#define FT930_IPC_REG(x)		(*(uint32_t *)(FT930_IPC_REG_BASE+(x)))
#define FT930_IPC_REG_REQTRG		FT930_IPC_REG(0)
#define FT930_IPC_REG_RSPIEN		FT930_IPC_REG(0x04)
#define FT930_IPC_REG_RSPISR		FT930_IPC_REG(0x08)
#define FT930_IPC_REG_SRCRST		FT930_IPC_REG(0x0C)
#define FT930_IPC_REG_REQIEN		FT930_IPC_REG(0x10)
#define FT930_IPC_REG_REQISR		FT930_IPC_REG(0x14)
#define FT930_IPC_REG_RSPTEN		FT930_IPC_REG(0x18)
#define FT930_IPC_REG_DSTRST		FT930_IPC_REG(0x1C)

#endif // __FT930_IPC_REGS_H__
