/**
    @file ft930_ipc_registers.h

    @brief
    IPC registers. 


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
 * ("Bridgetek") subject to the licence terms set out
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
