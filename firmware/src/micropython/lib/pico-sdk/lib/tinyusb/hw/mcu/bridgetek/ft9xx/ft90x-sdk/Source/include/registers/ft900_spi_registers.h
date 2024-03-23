/**
    @file ft900_spi_registers.h

    @brief
    SPI registers

    
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


#ifndef FT900_SPI_REGISTERS_H_
#define FT900_SPI_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
/* Register Bitfields */
#define BIT_SPIM_SPIM_CNTL_SP_R0     (0)
#define MASK_SPIM_SPIM_CNTL_SP_R0    (1 << BIT_SPIM_SPIM_CNTL_SP_R0)
#define BIT_SPIM_SPIM_CNTL_SP_R1     (1)
#define MASK_SPIM_SPIM_CNTL_SP_R1    (1 << BIT_SPIM_SPIM_CNTL_SP_R1)
#define BIT_SPIM_SPIM_CNTL_CLK_PHA   (2)
#define MASK_SPIM_SPIM_CNTL_CLK_PHA  (1 << BIT_SPIM_SPIM_CNTL_CLK_PHA)
#define BIT_SPIM_SPIM_CNTL_CLK_POL   (3)
#define MASK_SPIM_SPIM_CNTL_CLK_POL  (1 << BIT_SPIM_SPIM_CNTL_CLK_POL)
#define BIT_SPIM_SPIM_CNTL_MSTR      (4)
#define MASK_SPIM_SPIM_CNTL_MSTR     (1 << BIT_SPIM_SPIM_CNTL_MSTR)
#define BIT_SPIM_SPIM_CNTL_SP_R2     (5)
#define MASK_SPIM_SPIM_CNTL_SP_R2    (1 << BIT_SPIM_SPIM_CNTL_SP_R2)
#define BIT_SPIM_SPIM_CNTL_SP_E      (6)
#define MASK_SPIM_SPIM_CNTL_SP_E     (1 << BIT_SPIM_SPIM_CNTL_SP_E)
#define BIT_SPIM_SPIM_CNTL_SP_IE     (7)
#define MASK_SPIM_SPIM_CNTL_SP_IE    (1 << BIT_SPIM_SPIM_CNTL_SP_IE)

#define BIT_SPIM_SPIM_STATUS_SSC_EN  (0)
#define MASK_SPIM_SPIM_STATUS_SSC_EN (1 << BIT_SPIM_SPIM_STATUS_SSC_EN)
#define BIT_SPIM_SPIM_STATUS_RX_FIFOFULL     (1)
#define MASK_SPIM_SPIM_STATUS_RX_FIFOFULL    (1 << BIT_SPIM_SPIM_STATUS_RX_FIFOFULL)
#define BIT_SPIM_SPIM_STATUS_TX_EMPTY      (2)
#define MASK_SPIM_SPIM_STATUS_TX_EMPTY     (1 << BIT_SPIM_SPIM_STATUS_TX_EMPTY)
#define BIT_SPIM_SPIM_STATUS_THRE       (3)
#define MASK_SPIM_SPIM_STATUS_THRE      (1 << BIT_SPIM_SPIM_STATUS_THRE)
#define BIT_SPIM_SPIM_STATUS_MOD_FAULT      (4)
#define MASK_SPIM_SPIM_STATUS_MOD_FAULT     (1 << BIT_SPIM_SPIM_STATUS_MOD_FAULT)
#define BIT_SPIM_SPIM_STATUS_SPI_BIS     (5)
#define MASK_SPIM_SPIM_STATUS_SPI_BIS    (1 << BIT_SPIM_SPIM_STATUS_SPI_BIS)
#define BIT_SPIM_SPIM_STATUS_WR_COL       (6)
#define MASK_SPIM_SPIM_STATUS_WR_COL      (1 << BIT_SPIM_SPIM_STATUS_WR_COL)
#define BIT_SPIM_SPIM_STATUS_SPI_FLAG        (7)
#define MASK_SPIM_SPIM_STATUS_SPI_FLAG       (1 << BIT_SPIM_SPIM_STATUS_SPI_FLAG)

#define BIT_SPIM_SPIM_FIFO_CNTL_FIFO_EN     (0)
#define MASK_SPIM_SPIM_FIFO_CNTL_FIFO_EN    (1 << BIT_SPIM_SPIM_FIFO_CNTL_FIFO_EN)
#define BIT_SPIM_SPIM_FIFO_CNTL_RCVR_RST    (1)
#define MASK_SPIM_SPIM_FIFO_CNTL_RCVR_RST   (1 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_RST)
#define BIT_SPIM_SPIM_FIFO_CNTL_TX_RST    (2)
#define MASK_SPIM_SPIM_FIFO_CNTL_TX_RST   (1 << BIT_SPIM_SPIM_FIFO_CNTL_TX_RST)
#define BIT_SPIM_SPIM_FIFO_CNTL_TIMEOUT    (4)
#define MASK_SPIM_SPIM_FIFO_CNTL_TIMEOUT   (1 << BIT_SPIM_SPIM_FIFO_CNTL_TIMEOUT)
#define BIT_SPIM_SPIM_FIFO_CNTL_64_BYTE    (5)
#define MASK_SPIM_SPIM_FIFO_CNTL_64_BYTE   (1 << BIT_SPIM_SPIM_FIFO_CNTL_64_BYTE)
#define BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG    (6)
#define MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG   (0x3 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG)

#define BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI    (0)
#define MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI   (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI)
#define BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI    (1)
#define MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI   (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI)
#define BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_DIR     (2)
#define MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DIR    (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_DIR)
#define BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_TX_IEN   (3)
#define MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_TX_IEN  (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_TX_IEN)
/*****************************************************
 * !!! FAST_SPI available from FT900 REV C onwards!!!
 * */
#define BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_FAST_SPI   (4)
#define MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_FAST_SPI  (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_FAST_SPI)
/*****************************************************/
#define BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_MULTI_REC   (5)
#define MASK_SPIM_SPIM_TNSFR_FMRT_CNTL_MULTI_REC  (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_MULTI_REC)
#define BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_BISINT_EN   (6)
#define MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_BISINT_EN  (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_BISINT_EN)
#define BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_FIFO_EXT      (7)
#define MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_FIFO_EXT     (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_FIFO_EXT)

/*****************************************************
 * !!! Below macros for FT930 and FT900 REV C onwards!!!
 * */
#define BIT_SPIM_SPIM_CNTL_2_RX_EMPTY     	(0)
#define MASK_SPIM_SPIM_CNTL_2_RX_EMPTY    	(1 << BIT_SPIM_SPIM_CNTL_2_RX_EMPTY)
#define BIT_SPIM_SPIM_CNTL_2_CHG_SPR      	(1)
#define MASK_SPIM_SPIM_CNTL_2_CHG_SPR     	(1 << BIT_SPIM_SPIM_CNTL_2_CHG_SPR)
#define BIT_SPIM_SPIM_CNTL_2_LSBF   	  	(2)
#define MASK_SPIM_SPIM_CNTL_2_LSBF        	(1 << BIT_SPIM_SPIM_CNTL_2_LSBF)
#define BIT_SPIM_SPIM_CNTL_2_CLSBF   	  	(3)
#define MASK_SPIM_SPIM_CNTL_2_CLSBF       	(1 << BIT_SPIM_SPIM_CNTL_2_CLSBF)
#define BIT_SPIM_SPIM_CNTL_2_BAUD_REG_EN  	(7)
#define MASK_SPIM_SPIM_CNTL_2_BAUD_REG_EN   (1 << BIT_SPIM_SPIM_CNTL_2_BAUD_REG_EN)

#define BIT_SPIM_SPIM_STATUS_2_TXFIFO_EMPTY  		(0)
#define MASK_SPIM_SPIM_STATUS_2_TXFIFO_EMPTY 		(1 << BIT_SPIM_SPIM_STATUS_2_TXFIFO_EMPTY)
#define BIT_SPIM_SPIM_STATUS_2_TXFIFO_FULL    		(1)
#define MASK_SPIM_SPIM_STATUS_2_TXFIFO_FULL    		(1 << BIT_SPIM_SPIM_STATUS_2_TXFIFO_FULL)
#define BIT_SPIM_SPIM_STATUS_2_RX_EMPTY      		(2)
#define MASK_SPIM_SPIM_STATUS_2_RX_EMPTY     		(1 << BIT_SPIM_SPIM_STATUS_2_RX_EMPTY)
#define BIT_SPIM_SPIM_STATUS_2_RXFIFO_FULL       	(3)
#define MASK_SPIM_SPIM_STATUS_2_RXFIFO_FULL      	(1 << BIT_SPIM_SPIM_STATUS_2_RXFIFO_FULL)
#define BIT_SPIM_SPIM_STATUS_2_RXFULL_OVERRIDE      (4)
#define MASK_SPIM_SPIM_STATUS_2_RXFULL_OVERRIDE     (1 << BIT_SPIM_SPIM_STATUS_2_RXFULL_OVERRIDE)
#define BIT_SPIM_SPIM_STATUS_2_TXSPIF     			(5)
#define MASK_SPIM_SPIM_STATUS_2_TXSPIF    			(1 << BIT_SPIM_SPIM_STATUS_2_TXSPIF)
#define BIT_SPIM_SPIM_STATUS_2_TEMT      		 	(6)
#define MASK_SPIM_SPIM_STATUS_2_TEMT      			(1 << BIT_SPIM_SPIM_STATUS_2_TEMT)



#define BIT_SPIM_SPIM_FIFO_CNTL_2_RX_TRIG_EN     (0)
#define MASK_SPIM_SPIM_FIFO_CNTL_2_RX_TRIG_EN    (1 << BIT_SPIM_SPIM_FIFO_CNTL_2_RX_TRIG_EN)
#define BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_EN     (1)
#define MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_EN    (1 << BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_EN)
#define BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL    (2)
#define MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL   (0x3 << BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL)

/* TYPES ***************************************************************************/
#if defined(__FT900__)
/** @brief Register mappings for SPI registers - for FT900 Rev B  */
typedef struct
{
    volatile uint32_t SPI_CNTL;
    volatile uint32_t SPI_STATUS;
    volatile uint32_t SPI_DATA;
    volatile uint32_t SPI_SLV_SEL_CNTL;
    volatile uint32_t SPI_FIFO_CNTL;
    volatile uint32_t SPI_TNSFR_FRMT_CNTL;
    volatile uint32_t SPI_ALT_DATA;
    volatile uint32_t SPI_RCV_FIFO_COUNT;
} ft900_spi_regs_t;


/** @brief Register mappings for SPI registers - for FT900 Rev C */
typedef struct
{
    volatile uint32_t SPI_CNTL;
    volatile uint32_t SPI_STATUS;
    volatile uint32_t SPI_DATA;
    volatile uint32_t SPI_SLV_SEL_CNTL;
    volatile uint32_t SPI_FIFO_CNTL;
    volatile uint32_t SPI_TNSFR_FRMT_CNTL;
    volatile uint32_t SPI_ALT_DATA;
    volatile uint32_t SPI_RCV_FIFO_COUNT;

    volatile uint32_t reservedaccess[416];	//SPI_CNTL_2 starts at a different address base.
    /* Below are extension to the SPI register set */
    volatile uint32_t SPI_CNTL_2;
    volatile uint32_t reserved[3];
    volatile uint32_t SPI_STATUS_2;
    volatile uint32_t SPI_FIFO_CNTL_2;
    volatile uint32_t SPI_TX_FIFO_COUNT;
    volatile uint32_t SPI_BAUD;
} ft900_spi_regs_ex_t;
#else
/** @brief Register mappings for SPI registers - for FT930*/
typedef struct
{
    volatile uint32_t SPI_CNTL;
    volatile uint32_t SPI_STATUS;
    volatile uint32_t SPI_DATA;
    volatile uint32_t SPI_SLV_SEL_CNTL;
    volatile uint32_t SPI_FIFO_CNTL;
    volatile uint32_t SPI_TNSFR_FRMT_CNTL;
    volatile uint32_t SPI_ALT_DATA;
    volatile uint32_t SPI_RCV_FIFO_COUNT;

    /* Below are extension to the SPI register set */
    volatile uint32_t SPI_CNTL_2;
    volatile uint32_t reserved[3];
    volatile uint32_t SPI_STATUS_2;
    volatile uint32_t SPI_FIFO_CNTL_2;
    volatile uint32_t SPI_TX_FIFO_COUNT;
    volatile uint32_t SPI_BAUD;
} ft900_spi_regs_t;
#endif

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/


#endif /* FT900_SPI_REGISTERS_H_ */
