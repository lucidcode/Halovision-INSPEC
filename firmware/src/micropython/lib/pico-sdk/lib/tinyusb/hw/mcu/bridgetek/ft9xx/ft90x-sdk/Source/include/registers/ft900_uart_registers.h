/**
    @file

    @brief
    UART registers


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

#ifndef FT900_UART_REGSITERS_H_
#define FT900_UART_REGSITERS_H_


/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
//LCR bits
#define BIT_UART_LCR_WLS                (0)
#define MASK_UART_LCR_WLS               (0x3 << BIT_UART_LCR_WLS)
#define BIT_UART_LCR_STB                (2)
#define MASK_UART_LCR_STB               (1 << BIT_UART_LCR_STB)
#define BIT_UART_LCR_PEN                (3)
#define MASK_UART_LCR_PEN               (1 << BIT_UART_LCR_PEN)
#define BIT_UART_LCR_EPS                (4)
#define MASK_UART_LCR_EPS               (1 << BIT_UART_LCR_EPS)
#define BIT_UART_LCR_STKPAR             (5)
#define MASK_UART_LCR_STKPAR            (1 << BIT_UART_LCR_STKPAR)
#define BIT_UART_LCR_BRK                (6)
#define MASK_UART_LCR_BRK               (1 << BIT_UART_LCR_BRK)
#define BIT_UART_LCR_DLAB               (7)
#define MASK_UART_LCR_DLAB              (1 << BIT_UART_LCR_DLAB)

//LSR bits
#define BIT_UART_LSR_DR                 (0)
#define MASK_UART_LSR_DR                (1 << BIT_UART_LSR_DR)
#define BIT_UART_LSR_OE                 (1)
#define MASK_UART_LSR_OE                (1 << BIT_UART_LSR_OE)
#define BIT_UART_LSR_PE_RXDATA          (2)
#define MASK_UART_LSR_PE_RXDATA         (1 << BIT_UART_LSR_PE_RXDATA)
#define BIT_UART_LSR_FE                 (3)
#define MASK_UART_LSR_FE                (1 << BIT_UART_LSR_FE)
#define BIT_UART_LSR_BI                 (4)
#define MASK_UART_LSR_BI                (1 << BIT_UART_LSR_BI)
#define BIT_UART_LSR_THRE               (5)
#define MASK_UART_LSR_THRE              (1 << BIT_UART_LSR_THRE)
#define BIT_UART_LSR_TEMT               (6)
#define MASK_UART_LSR_TEMT              (1 << BIT_UART_LSR_TEMT)
#define BIT_UART_LSR_RHRERR             (7)
#define MASK_UART_LSR_RHRERR            (1 << BIT_UART_LSR_RHRERR)

//IER bits
#define BIT_UART_IER_ERBFI              (0)
#define MASK_UART_IER_ERBFI             (1 << BIT_UART_IER_ERBFI)
#define BIT_UART_IER_ETBEI              (1)
#define MASK_UART_IER_ETBEI             (1 << BIT_UART_IER_ETBEI)
#define BIT_UART_IER_ELSI               (2)
#define MASK_UART_IER_ELSI              (1 << BIT_UART_IER_ELSI)
#define BIT_UART_IER_EDSSI              (3)
#define MASK_UART_IER_EDSSI             (1 << BIT_UART_IER_EDSSI)
#define BIT_UART_IER_ESM                (4)
#define MASK_UART_IER_ESM               (1 << BIT_UART_IER_ESM)
#define BIT_UART_IER_ESCH               (5)
#define MASK_UART_IER_ESCH              (1 << BIT_UART_IER_ESCH)
#define BIT_UART_IER_RTSIMASK           (6)
#define MASK_UART_IER_RTSIMASK          (1 << BIT_UART_IER_RTSIMASK)
#define BIT_UART_IER_CTSIMASK           (7)
#define MASK_UART_IER_CTSIMASK          (1 << BIT_UART_IER_CTSIMASK)

//MCR bits
#define BIT_UART_MCR_DTR                (0)
#define MASK_UART_MCR_DTR               (1 << BIT_UART_MCR_DTR)
#define BIT_UART_MCR_RTS                (1)
#define MASK_UART_MCR_RTS               (1 << BIT_UART_MCR_RTS)
#define BIT_UART_MCR_OUT1               (2)
#define MASK_UART_MCR_OUT1              (1 << BIT_UART_MCR_OUT1)
#define BIT_UART_MCR_OUT2               (3)
#define MASK_UART_MCR_OUT2              (1 << BIT_UART_MCR_OUT2)
#define BIT_UART_MCR_LOOP               (4)
#define MASK_UART_MCR_LOOP              (1 << BIT_UART_MCR_LOOP)
#define BIT_UART_MCR_XON                (5)
#define MASK_UART_MCR_XON               (1 << BIT_UART_MCR_XON)
#define BIT_UART_MCR_IRDA               (6)
#define MASK_UART_MCR_IRDA              (1 << BIT_UART_MCR_IRDA)
#define BIT_UART_MCR_PRESCALAR          (7)
#define MASK_UART_MCR_PRESCALAR         (1 << BIT_UART_MCR_PRESCALAR)

//MSR bits
#define BIT_UART_MSR_DCTS               (0)
#define MASK_UART_MSR_DCTS              (1 << BIT_UART_MSR_DCTS)
#define BIT_UART_MSR_DDSR               (1)
#define MASK_UART_MSR_DDSR              (1 << BIT_UART_MSR_DDSR)
#define BIT_UART_MSR_TERI               (2)
#define MASK_UART_MSR_TERI              (1 << BIT_UART_MSR_TERI)
#define BIT_UART_MSR_DDCD               (3)
#define MASK_UART_MSR_DDCD              (1 << BIT_UART_MSR_DDCD)
#define BIT_UART_MSR_CTS                (4)
#define MASK_UART_MSR_CTS               (1 << BIT_UART_MSR_CTS)
#define BIT_UART_MSR_DSR                (5)
#define MASK_UART_MSR_DSR               (1 << BIT_UART_MSR_DSR)
#define BIT_UART_MSR_RI                 (6)
#define MASK_UART_MSR_RI                (1 << BIT_UART_MSR_RI)
#define BIT_UART_MSR_DCD                (7)
#define MASK_UART_MSR_DCD               (1 << BIT_UART_MSR_DCD)

// FCR bits
#define BIT_UART_FCR_EN                 (0)
#define MASK_UART_FCR_EN                (1 << BIT_UART_FCR_EN)
#define BIT_UART_FCR_RCVR_RST           (1)
#define MASK_UART_FCR_RCVR_RST          (1 << BIT_UART_FCR_RCVR_RST)
#define BIT_UART_FCR_XMIT_RST           (2)
#define MASK_UART_FCR_XMIT_RST          (1 << BIT_UART_FCR_XMIT_RST)
#define BIT_UART_FCR_DMA                (3)
#define MASK_UART_FCR_DMA               (1 << BIT_UART_FCR_DMA)
#define BIT_UART_FCR_THR_TRG0           (4)
#define MASK_UART_FCR_THR_TRG0          (1 << BIT_UART_FCR_THR_TRG0)
#define BIT_UART_FCR_THR_TRG1           (5)
#define MASK_UART_FCR_THR_TRG1          (1 << BIT_UART_FCR_THR_TRG1)
#define MASK_UART_FCR_THR_TRG           (MASK_UART_FCR_THR_TRG0 | MASK_UART_FCR_THR_TRG1)
#define BIT_UART_FCR_FIFO_SIZE          (5)
#define MASK_UART_FCR_FIFO_SIZE         (1 << BIT_UART_FCR_FIFO_SIZE)
#define BIT_UART_FCR_RCRV_TRG0          (6)
#define MASK_UART_FCR_RCRV_TRG0         (1 << BIT_UART_FCR_RCRV_TRG0)
#define BIT_UART_FCR_RCRV_TRG1          (7)
#define MASK_UART_FCR_RCRV_TRG1         (1 << BIT_UART_FCR_RCRV_TRG1)
#define MASK_UART_FCR_RCRV_TRG          (MASK_UART_FCR_RCRV_TRG0 | MASK_UART_FCR_RCRV_TRG1)

// EFR bits
#define BIT_UART_EFR_IBRFCM0            (0)
#define MASK_UART_EFR_IBRFCM            (3 << BIT_UART_EFR_IBRFCM0)
#define MASK_UART_EFR_IBRFCM0           (1 << BIT_UART_EFR_IBRFCM0)
#define BIT_UART_EFR_IBRFCM1            (1)
#define MASK_UART_EFR_IBRFCM1           (1 << BIT_UART_EFR_IBRFCM1)
#define BIT_UART_EFR_IBTFCM0            (2)
#define MASK_UART_EFR_IBTFCM0           (1 << BIT_UART_EFR_IBTFCM0)
#define BIT_UART_EFR_IBTFCM1            (3)
#define MASK_UART_EFR_IBTFCM1           (1 << BIT_UART_EFR_IBTFCM1)
#define MASK_UART_EFR_IBTFCM            (MASK_UART_EFR_IBTFCM0 | MASK_UART_EFR_IBTFCM1)
#define BIT_UART_EFR_EM                 (4)
#define MASK_UART_EFR_EM                (1 << BIT_UART_EFR_EM)
#define BIT_UART_EFR_SPL_CHAR           (5)
#define MASK_UART_EFR_SPL_CHAR          (1 << BIT_UART_EFR_SPL_CHAR)
#define BIT_UART_EFR_RTSFC              (6)
#define MASK_UART_EFR_RTSFC             (1 << BIT_UART_EFR_RTSFC)
#define BIT_UART_EFR_CTSFC              (7)
#define MASK_UART_EFR_CTSFC             (1 << BIT_UART_EFR_CTSFC)

// ASR bits
#define BIT_UART_ASR_TX_DIS             (0)
#define MASK_UART_ASR_TX_DIS            (1 << BIT_UART_ASR_TX_DIS)
#define BIT_UART_ASR_REM_TX_DIS         (1)
#define MASK_UART_ASR_REM_TX_DIS        (1 << BIT_UART_ASR_REM_TX_DIS)
#define BIT_UART_ASR_RTS                (2)
#define MASK_UART_ASR_RTS               (1 << BIT_UART_ASR_RTS)
#define BIT_UART_ASR_DTR                (3)
#define MASK_UART_ASR_DTR               (1 << BIT_UART_ASR_DTR)
#define BIT_UART_ASR_SP_CHAR_DET        (4)
#define MASK_UART_ASR_SP_CHAR_DET       (1 << BIT_UART_ASR_SP_CHAR_DET)
#define BIT_UART_ASR_FIFO_SEL           (5)
#define MASK_UART_ASR_FIFO_SEL          (1 << BIT_UART_ASR_FIFO_SEL)
#define BIT_UART_ASR_FIFO_SIZE          (6)
#define MASK_UART_ASR_FIFO_SIZE         (1 << BIT_UART_ASR_FIFO_SIZE)
#define BIT_UART_ASR_TX_IDLE            (7)
#define MASK_UART_ASR_TX_IDLE           (1 << BIT_UART_ASR_TX_IDLE)

/* SPECIAL REGISTERS */

#define OFFSET_UART_SPR_ACR             (0U)

#define OFFSET_UART_SPR_CPR             (1U)

#define OFFSET_UART_SPR_TCR             (2U)

#define OFFSET_UART_SPR_CKS             (3U)

#define OFFSET_UART_SPR_TTL             (4U)

#define OFFSET_UART_SPR_RTL             (5U)

#define OFFSET_UART_SPR_FCL             (6U)

#define OFFSET_UART_SPR_FCH             (7U)

#define OFFSET_UART_SPR_ID1             (8U)

#define OFFSET_UART_SPR_ID2             (9U)

#define OFFSET_UART_SPR_ID3             (10U)

#define OFFSET_UART_SPR_REV             (11U)

#define OFFSET_UART_SPR_CSR             (12U)

#define OFFSET_UART_SPR_NMR             (13U)
#define BIT_UART_SPR_NMR_9EN            (0)
#define MASK_UART_SPR_NMR_9EN           (1 << BIT_UART_SPR_NMR_9EN)

#define OFFSET_UART_SPR_MDM             (14U)

#define OFFSET_UART_SPR_RFC             (15U)

#define OFFSET_UART_SPR_GDS             (16U)


/* SPR bits*/
#define BIT_UART_SPR_TX_9BIT            (0)
#define MASK_UART_SPR_TX_9BIT           (1 << BIT_UART_SPR_TX_9BIT)

/* ACR bits */
#define BIT_UART_ACR_RX                 (0)
#define MASK_UART_ACR_RX                (1 << BIT_UART_ACR_RX)
#define BIT_UART_ACR_TX                 (1)
#define MASK_UART_ACR_TX                (1 << BIT_UART_ACR_TX)
#define BIT_UART_ACR_DSR                (2)
#define MASK_UART_ACR_DSR               (1 << BIT_UART_ACR_DSR)
#define BIT_UART_ACR_DTR0               (3)
#define MASK_UART_ACR_DTR0              (1 << BIT_UART_ACR_DTR0)
#define BIT_UART_ACR_DTR1               (4)
#define MASK_UART_ACR_DTR1              (1 << BIT_UART_ACR_DTR1)
#define MASK_UART_ACR_DTR               (MASK_UART_ACR_DTR0 | MASK_UART_ACR_DTR1)
#define BIT_UART_ACR_950TLE             (5)
#define MASK_UART_ACR_950TLE            (1 << BIT_UART_ACR_950TLE)
#define BIT_UART_ACR_ICR                (6)
#define MASK_UART_ACR_ICR               (1 << BIT_UART_ACR_ICR)
#define BIT_UART_ACR_ASE                (7)
#define MASK_UART_ACR_ASE               (1 << BIT_UART_ACR_ASE)

// CKS bits
#define BIT_UART_CKS_RCSS0              (0)
#define MASK_UART_CKS_RCSS0             (1 << BIT_UART_CKS_RCSS0)
#define BIT_UART_CKS_RCSS1              (1)
#define MASK_UART_CKS_RCSS1             (1 << BIT_UART_CKS_RCSS1)
#define MASK_UART_CKS_RCSS              (MASK_UART_CKS_RCSS0 | MASK_UART_CKS_RCSS1)
#define BIT_UART_CKS_BDOUT              (2)
#define MASK_UART_CKS_BDOUT             (1 << BIT_UART_CKS_BDOUT)
#define BIT_UART_CKS_RCMS               (3)
#define MASK_UART_CKS_RCMS              (1 << BIT_UART_CKS_RCMS)
#define BIT_UART_CKS_TCRG0              (4)
#define MASK_UART_CKS_TCRG0             (1 << BIT_UART_CKS_TCRG0)
#define BIT_UART_CKS_TCRG1              (5)
#define MASK_UART_CKS_TCRG1             (1 << BIT_UART_CKS_TCRG1)
#define MASK_UART_CKS_TCRG              (MASK_UART_CKS_TCRG0 | MASK_UART_CKS_TCRG1)
#define BIT_UART_CKS_TCSS               (6)
#define MASK_UART_CKS_TCSS              (1 << BIT_UART_CKS_TCSS)
#define BIT_UART_CKS_TCMS               (7)
#define MASK_UART_CKS_TCMS              (1 << BIT_UART_CKS_TCMS)

// NMR bits
#define BIT_UART_NMR_9_EN               (0)
#define MASK_UART_NMR_9_EN              (1 << BIT_UART_NMR_9_EN)
#define BIT_UART_NMR_9_INT_EN           (1)
#define MASK_UART_NMR_9_INT_EN          (1 << BIT_UART_NMR_9_INT_EN)
#define BIT_UART_NMR_9_SC1              (2)
#define MASK_UART_NMR_9_SC1             (1 << BIT_UART_NMR_9_SC1)
#define BIT_UART_NMR_9_SC2              (3)
#define MASK_UART_NMR_9_SC2             (1 << BIT_UART_NMR_9_SC2)
#define BIT_UART_NMR_9_SC3              (4)
#define MASK_UART_NMR_9_SC3             (1 << BIT_UART_NMR_9_SC3)
#define BIT_UART_NMR_9_SC4              (5)
#define MASK_UART_NMR_9_SC4             (1 << BIT_UART_NMR_9_SC4)

/* TYPES ***************************************************************************/
/** @brief Register mappings for UART registers */
typedef struct
{
    volatile uint8_t RHR_THR_DLL;
    volatile uint8_t IER_DLH_ASR;
    volatile uint8_t ISR_FCR_EFR;
    volatile uint8_t LCR_RFL;
    volatile uint8_t MCR_XON1_TFL;
    volatile uint8_t LSR_ICR_XON2;
    volatile uint8_t MSR_XOFF1;
    volatile uint8_t SPR_XOFF2;
} ft900_uart_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_UART_REGSITERS_H_ */

