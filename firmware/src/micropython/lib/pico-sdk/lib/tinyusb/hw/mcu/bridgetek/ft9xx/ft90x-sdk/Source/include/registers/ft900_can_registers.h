/**
    @file

    @brief
    CANBus registers

    
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

#ifndef FT900_CAN_REGISTERS_H_
#define FT900_CAN_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/

/* Bit masks for the CAN_MODE register */
#define BIT_CAN_MODE_ACC_FLTR   (0)
#define MASK_CAN_MODE_ACC_FLTR  (1 << BIT_CAN_MODE_ACC_FLTR)
#define BIT_CAN_MODE_LSTN_ONLY  (1)
#define MASK_CAN_MODE_LSTN_ONLY (1 << BIT_CAN_MODE_LSTN_ONLY)
#define BIT_CAN_MODE_RST        (2)
#define MASK_CAN_MODE_RST       (1 << BIT_CAN_MODE_RST)

/* Bit masks for the CAN_CMD register */
#define BIT_CAN_CMD_ABORT_TX    (1)
#define MASK_CAN_CMD_ABORT_TX   (1 << BIT_CAN_CMD_ABORT_TX)
#define BIT_CAN_CMD_TX_REQ      (2)
#define MASK_CAN_CMD_TX_REQ     (1 << BIT_CAN_CMD_TX_REQ)

/* Bit masks for the CAN_STATUS register */
#define BIT_CAN_STATUS_BUS_OFF_STS  (0)
#define MARK_CAN_STATUS_BUS_OFF_STS (1 << BIT_CAN_STATUS_BUS_OFF_STS)
#define BIT_CAN_STATUS_ERR_STS  (1)
#define MASK_CAN_STATUS_ERR_STS (1 << BIT_CAN_STATUS_ERR_STS)
#define BIT_CAN_STATUS_TX_STS   (2)
#define MASK_CAN_STATUS_TX_STS  (1 << BIT_CAN_STATUS_TX_STS)
#define BIT_CAN_STATUS_RX_STS   (3)
#define MASK_CAN_STATUS_RX_STS  (1 << BIT_CAN_STATUS_RX_STS)
#define BIT_CAN_STATUS_TX_BUF_STS  (5)
#define MASK_CAN_STATUS_TX_BUF_STS (1 << BIT_CAN_STATUS_TX_BUF_STS)
#define BIT_CAN_STATUS_OVRN_STS    (6)
#define MASK_CAN_STATUS_OVRN_STS   (1 << BIT_CAN_STATUS_OVRN_STS)
#define BIT_CAN_STATUS_RX_BUF_STS  (7)
#define MASK_CAN_STATUS_RX_BUF_STS (1 << BIT_CAN_STATUS_RX_BUF_STS)
/************* Available from FT900 Rev C Onwards ***********************/
#define BIT_CAN_STATUS_DOVLD_STS   (4)  /* 1 -> the overload condition occurs due to the
										  internal conditions of the receiver,
										  which requires a delay of the next frame*/
#define MASK_CAN_STATUS_DOVLD_STS  (1 << BIT_CAN_STATUS_DOVLD_STS)
/***********************************************************************/

/* Bit masks for the CAN_INT_STATUS and CAN_INT_ENABLE registers */
#define BIT_CAN_INT_STATUS_DATA_OVRN     (0)
#define MASK_CAN_INT_STATUS_DATA_OVRN    (1 << BIT_CAN_INT_STATUS_DATA_OVRN)
#define BIT_CAN_INT_STATUS_BUS_ERR       (1)
#define MASK_CAN_INT_STATUS_BUS_ERR      (1 << BIT_CAN_INT_STATUS_BUS_ERR)
#define BIT_CAN_INT_STATUS_TX          (2)
#define MASK_CAN_INT_STATUS_TX         (1 << BIT_CAN_INT_STATUS_TX)
#define BIT_CAN_INT_STATUS_RX          (3)
#define MASK_CAN_INT_STATUS_RX         (1 << BIT_CAN_INT_STATUS_RX)
#define BIT_CAN_INT_STATUS_ERR_PSV     (4)
#define MASK_CAN_INT_STATUS_ERR_PSV    (1 << BIT_CAN_INT_STATUS_ERR_PSV)
#define BIT_CAN_INT_STATUS_ERR_WRNG     (5)
#define MASK_CAN_INT_STATUS_ERR_WRN    (1 << BIT_CAN_INT_STATUS_ERR_WRNG)
#define BIT_CAN_INT_STATUS_ARB_LOST     (6)
#define MASK_CAN_INT_STATUS_ARB_LOST    (1 << BIT_CAN_INT_STATUS_ARB_LOST)
/************* Available from FT900 Rev C Onwards ***********************/
#define BIT_CAN_INT_STATUS_DOVLD	     (7)  /* Set when the overload condition occurs.
											   * Write a 1 to clear this interrupt
											   */
#define MASK_CAN_INT_STATUS_DOVLD        (1 << BIT_CAN_INT_STATUS_DOVLD)
/***********************************************************************/

#define BIT_CAN_INT_ENABLE_DATA_OVRN_EN     (0)
#define MASK_CAN_INT_ENABLE_DATA_OVRN_EN    (1 << BIT_CAN_INT_ENABLE_DATA_OVRN_EN)
#define BIT_CAN_INT_ENABLE_BUS_ERR_EN       (1)
#define MASK_CAN_INT_ENABLE_BUS_ERR_EN      (1 << BIT_CAN_INT_ENABLE_BUS_ERR_EN)
#define BIT_CAN_INT_ENABLE_TX_EN          (2)
#define MASK_CAN_INT_ENABLE_TX_EN         (1 << BIT_CAN_INT_ENABLE_TX_EN)
#define BIT_CAN_INT_ENABLE_RX_EN          (3)
#define MASK_CAN_INT_ENABLE_RX_EN         (1 << BIT_CAN_INT_ENABLE_RX_EN)
#define BIT_CAN_INT_ENABLE_ERR_PSV_EN     (4)
#define MASK_CAN_INT_ENABLE_ERR_PSV_EN    (1 << BIT_CAN_INT_ENABLE_ERR_PSV_EN)
#define BIT_CAN_INT_ENABLE_ERR_WRNG_EN     (5)
#define MASK_CAN_INT_ENABLE_ERR_WRNG_EN    (1 << BIT_CAN_INT_ENABLE_ERR_WRNG_EN)
#define BIT_CAN_INT_ENABLE_ARB_LOST_EN     (6)
#define MASK_CAN_INT_ENABLE_ARB_LOST_EN    (1 << BIT_CAN_INT_ENABLE_ARB_LOST_EN)
/************* Available from FT900 Rev C Onwards ***********************/
#define BIT_CAN_INT_ENABLE_DOVLD_EN     (7) /*Mask for Data Overload interrupt */
#define MASK_CAN_INT_ENABLE_DOVLD_EN    (1 << BIT_CAN_INT_ENABLE_DOVLD_EN)
/***********************************************************************/

/* Bit masks for the CAN_BUS_TIM_0 register */
#define BIT_CAN_BUS_TIM_0_BAUD_PSCL        (0)
#define MASK_CAN_BUS_TIM_0_BAUD_PSCL       (0x3F << BIT_CAN_BUS_TIM_0_BAUD_PSCL)
#define BIT_CAN_BUS_TIM_0_SYNC_JMP_WDT        (6)
#define MASK_CAN_BUS_TIM_0_SYNC_JMP_WDT       (0x3 << BIT_CAN_BUS_TIM_0_SYNC_JMP_WDT)

/* Bit masks for the CAN_BUS_TIM_1 register */
#define BIT_CAN_BUS_TIM_1_TIM_SEG1     (0)
#define MASK_CAN_BUS_TIM_1_TIM_SEG1    (0xF << BIT_CAN_BUS_TIM_1_TIM_SEG1)
#define BIT_CAN_BUS_TIM_1_TIM_SEG2     (4)
#define MASK_CAN_BUS_TIM_1_TIM_SEG2    (0x7 << BIT_CAN_BUS_TIM_1_TIM_SEG2)
#define BIT_CAN_BUS_TIM_1_NUM_SAM     (7)
#define MASK_CAN_BUS_TIM_1_NUM_SAM    (1 << BIT_CAN_BUS_TIM_1_NUM_SAM)

/* Bit masks for the CAN_ERR_CODE (Error Code) register */
#define BIT_CAN_ERR_CODE_BIT_ERR       (0)
#define MASK_CAN_ERR_CODE_BIT_ERR      (1 << BIT_CAN_ERR_CODE_BIT_ERR)
#define BIT_CAN_ERR_CODE_STF_ERR     (1)
#define MASK_CAN_ERR_CODE_STF_ERR    (1 << BIT_CAN_ERR_CODE_STF_ERR)
#define BIT_CAN_ERR_CODE_CRC_ERR       (2)
#define MASK_CAN_ERR_CODE_CRC_ERR      (1 << BIT_CAN_ERR_CODE_CRC_ERR)
#define BIT_CAN_ERR_CODE_FRM_ERR      (3)
#define MASK_CAN_ERR_CODE_FRM_ERR     (1 << BIT_CAN_ERR_CODE_FRM_ERR)
#define BIT_CAN_ERR_CODE_ACK_ERR       (4)
#define MASK_CAN_ERR_CODE_ACK_ERR      (1 << BIT_CAN_ERR_CODE_ACK_ERR)
#define BIT_CAN_ERR_CODE_ERR_DIR       (5)
#define MASK_CAN_ERR_CODE_ERR_DIR      (1 << BIT_CAN_ERR_CODE_ERR_DIR)
#define BIT_CAN_ERR_CODE_TX_WRN       (6)
#define MASK_CAN_ERR_CODE_TX_WRN      (1 << BIT_CAN_ERR_CODE_TX_WRN)
#define BIT_CAN_ERR_CODE_RX_WRN       (7)
#define MASK_CAN_ERR_CODE_RX_WRN      (1 << BIT_CAN_ERR_CODE_RX_WRN)

/* TYPES ***************************************************************************/

/** @brief Register mappings for CAN registers */
typedef struct
{
	volatile uint8_t CAN_MODE;			
	volatile uint8_t CAN_CMD;			
	volatile uint8_t CAN_STATUS;		
	volatile uint8_t CAN_INT_STATUS;	
	volatile uint8_t CAN_INT_ENABLE;	
	volatile uint8_t CAN_RX_MSG;		
	volatile uint8_t CAN_BUS_TIM_0;		
	volatile uint8_t CAN_BUS_TIM_1;		
	volatile uint32_t CAN_TX_BUF;		
	volatile uint32_t CAN_RX_BUF;		
	volatile uint8_t CAN_ACC_CODE_0;	
	volatile uint8_t CAN_ACC_CODE_1;	
	volatile uint8_t CAN_ACC_CODE_2;	
	volatile uint8_t CAN_ACC_CODE_3;	
	volatile uint8_t CAN_ACC_ENABLE_0;	
	volatile uint8_t CAN_ACC_ENABLE_1;	
	volatile uint8_t CAN_ACC_ENABLE_2;	
	volatile uint8_t CAN_ACC_ENABLE_3;	
	volatile uint8_t CAN_ERR_CODE;		
	volatile uint8_t CAN_RX_ERR_CNTR;	
	volatile uint8_t CAN_TX_ERR_CNTR;	
	volatile uint8_t CAN_ARB_LOST_CODE;	
} ft900_can_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_CAN_REGISTERS_H_ */
