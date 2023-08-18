/**
    @file

    @brief
    I2S registers

    
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

#ifndef FT900_I2S_REGISTERS_H_
#define FT900_I2S_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/

/* CR bits */
#define BIT_I2S_CR_TX_ENABLE				0
#define MASK_I2S_CR_TX_ENABLE				(0x01 << BIT_I2S_CR_TX_ENABLE)
#define BIT_I2S_CR_RX_ENABLE				1
#define MASK_I2S_CR_RX_ENABLE				(0x01 << BIT_I2S_CR_RX_ENABLE)
#define BIT_I2S_CR_LRCLK_IN_POL				2
#define MASK_I2S_CR_LRCLK_IN_POL			(0x01 << BIT_I2S_CR_LRCLK_IN_POL)
#define BIT_I2S_CR_LRCLK_OUT_POL			3
#define MASK_I2S_CR_LRCLK_OUT_POL			(0x01 << BIT_I2S_CR_LRCLK_OUT_POL)
#define BIT_I2S_CR_BCLK_POL					4
#define MASK_I2S_CR_BCLK_POL				(0x01 << BIT_I2S_CR_BCLK_POL)
#define BIT_I2S_CR_IS_MASTER_22				5
#define MASK_I2S_CR_IS_MASTER_22			(0x01 << BIT_I2S_CR_IS_MASTER_22)
#define BIT_I2S_CR_MASTER_MODE				6
#define MASK_I2S_CR_MASTER_MODE				(0x01 << BIT_I2S_CR_MASTER_MODE)
#define BIT_I2S_CR_IS_MASTER_64				7
#define MASK_I2S_CR_IS_MASTER_64			(0x01 << BIT_I2S_CR_IS_MASTER_64)
#define BIT_I2S_CR_BIT_LENGTH_0				8
#define MASK_I2S_CR_BIT_LENGTH_0			(0x01 << BIT_I2S_CR_BIT_LENGTH_0)
#define BIT_I2S_CR_BIT_LENGTH_1				9
#define MASK_I2S_CR_BIT_LENGTH_1			(0x01 << BIT_I2S_CR_BIT_LENGTH_1)
#define MASK_I2S_CR_BIT_LENGTH				(MASK_I2S_CR_BIT_LENGTH_1 | MASK_I2S_CR_BIT_LENGTH_0)
#define BIT_I2S_CR_FORMAT_0					10
#define MASK_I2S_CR_FORMAT_0				(0x01 << BIT_I2S_CR_FORMAT_0)
#define BIT_I2S_CR_FORMAT_1					11
#define MASK_I2S_CR_FORMAT_1				(0x01 << BIT_I2S_CR_FORMAT_1)
#define MASK_I2S_CR_FORMAT					(MASK_I2S_CR_FORMAT_1 | MASK_I2S_CR_FORMAT_0)
#define BIT_I2S_CR_PADDING_0				12
#define MASK_I2S_CR_PADDING_0				(0x01 << BIT_I2S_CR_PADDING_0)
#define BIT_I2S_CR_PADDING_1				13
#define MASK_I2S_CR_PADDING_1				(0x01 << BIT_I2S_CR_PADDING_1)
#define BIT_I2S_CR_PADDING_2				14
#define MASK_I2S_CR_PADDING_2				(0x01 << BIT_I2S_CR_PADDING_2)
#define MASK_I2S_CR_PADDING					(MASK_I2S_CR_PADDING_2 | MASK_I2S_CR_PADDING_1 | MASK_I2S_CR_PADDING_0)
#define BIT_I2S_CR_RESET					15
#define MASK_I2S_CR_RESET					(0x01 << BIT_I2S_CR_RESET)
#define MASK_I2S_CR							(0xFFFF)
	
/* CR2 bits */
#define BIT_I2S_CR2_BCLK_DIV_0				0
#define MASK_I2S_CR2_BCLK_DIV_0				(0x01 << BIT_I2S_CR2_BCLK_DIV_0)
#define BIT_I2S_CR2_BCLK_DIV_1				1
#define MASK_I2S_CR2_BCLK_DIV_1				(0x01 << BIT_I2S_CR2_BCLK_DIV_1)
#define BIT_I2S_CR2_BCLK_DIV_2				2
#define MASK_I2S_CR2_BCLK_DIV_2				(0x01 << BIT_I2S_CR2_BCLK_DIV_2)
#define BIT_I2S_CR2_BCLK_DIV_3				3
#define MASK_I2S_CR2_BCLK_DIV_3				(0x01 << BIT_I2S_CR2_BCLK_DIV_3)
#define BIT_I2S_CR2_MCLK_DIV_0				8
#define MASK_I2S_CR2_MCLK_DIV_0				(0x01 << BIT_I2S_CR2_MCLK_DIV_0)
#define BIT_I2S_CR2_MCLK_DIV_1				9
#define MASK_I2S_CR2_MCLK_DIV_1				(0x01 << BIT_I2S_CR2_MCLK_DIV_1)
#define BIT_I2S_CR2_MCLK_DIV_2				10
#define MASK_I2S_CR2_MCLK_DIV_2				(0x01 << BIT_I2S_CR2_MCLK_DIV_2)
#define BIT_I2S_CR2_MCLK_DIV_3				11
#define MASK_I2S_CR2_MCLK_DIV_3				(0x01 << BIT_I2S_CR2_MCLK_DIV_3)
#define MASK_I2S_CR2						(0x0F0F)
	
/* IE bits	*/
#define BIT_I2S_IE_FIFO_TX_UNDER			0
#define MASK_I2S_IE_FIFO_TX_UNDER			(0x01 << BIT_I2S_IE_FIFO_TX_UNDER)
#define BIT_I2S_IE_FIFO_TX_EMPTY			1
#define MASK_I2S_IE_FIFO_TX_EMPTY			(0x01 << BIT_I2S_IE_FIFO_TX_EMPTY)
#define BIT_I2S_IE_FIFO_TX_HALF_FULL		2
#define MASK_I2S_IE_FIFO_TX_HALF_FULL		(0x01 << BIT_I2S_IE_FIFO_TX_HALF_FULL)
#define BIT_I2S_IE_FIFO_TX_FULL				3
#define MASK_I2S_IE_FIFO_TX_FULL			(0x01 << BIT_I2S_IE_FIFO_TX_FULL)
#define BIT_I2S_IE_FIFO_TX_OVER				4
#define MASK_I2S_IE_FIFO_TX_OVER			(0x01 << BIT_I2S_IE_FIFO_TX_OVER)
#define BIT_I2S_IE_FIFO_RX_UNDER			8
#define MASK_I2S_IE_FIFO_RX_UNDER			(0x01 << BIT_I2S_IE_FIFO_RX_UNDER)
#define BIT_I2S_IE_FIFO_RX_EMPTY			9
#define MASK_I2S_IE_FIFO_RX_EMPTY			(0x01 << BIT_I2S_IE_FIFO_RX_EMPTY)
#define BIT_I2S_IE_FIFO_RX_HALF_FULL		10
#define MASK_I2S_IE_FIFO_RX_HALF_FULL		(0x01 << BIT_I2S_IE_FIFO_RX_HALF_FULL)
#define BIT_I2S_IE_FIFO_RX_FULL				11
#define MASK_I2S_IE_FIFO_RX_FULL			(0x01 << BIT_I2S_IE_FIFO_RX_FULL)
#define BIT_I2S_IE_FIFO_RX_OVER				12
#define MASK_I2S_IE_FIFO_RX_OVER			(0x01 << BIT_I2S_IE_FIFO_RX_OVER)
#define MASK_I2S_CR2						(0x0F0F)

/* PEND bits */
#define BIT_I2S_PEND_FIFO_TX_UNDER			0
#define MASK_I2S_PEND_FIFO_TX_UNDER			(0x01 << BIT_I2S_PEND_FIFO_TX_UNDER)
#define BIT_I2S_PEND_FIFO_TX_EMPTY			1
#define MASK_I2S_PEND_FIFO_TX_EMPTY			(0x01 << BIT_I2S_PEND_FIFO_TX_EMPTY)
#define BIT_I2S_PEND_FIFO_TX_HALF_FULL		2
#define MASK_I2S_PEND_FIFO_TX_HALF_FULL		(0x01 << BIT_I2S_PEND_FIFO_TX_HALF_FULL)
#define BIT_I2S_PEND_FIFO_TX_FULL			3
#define MASK_I2S_PEND_FIFO_TX_FULL			(0x01 << BIT_I2S_PEND_FIFO_TX_FULL)
#define BIT_I2S_PEND_FIFO_TX_OVER			4
#define MASK_I2S_PEND_FIFO_TX_OVER			(0x01 << BIT_I2S_PEND_FIFO_TX_OVER)
#define BIT_I2S_PEND_FIFO_RX_UNDER			8
#define MASK_I2S_PEND_FIFO_RX_UNDER			(0x01 << BIT_I2S_PEND_FIFO_RX_UNDER)
#define BIT_I2S_PEND_FIFO_RX_EMPTY			9
#define MASK_I2S_PEND_FIFO_RX_EMPTY			(0x01 << BIT_I2S_PEND_FIFO_RX_EMPTY)
#define BIT_I2S_PEND_FIFO_RX_HALF_FULL		10
#define MASK_I2S_PEND_FIFO_RX_HALF_FULL		(0x01 << BIT_I2S_PEND_FIFO_RX_HALF_FULL)
#define BIT_I2S_PEND_FIFO_RX_FULL			11
#define MASK_I2S_PEND_FIFO_RX_FULL			(0x01 << BIT_I2S_PEND_FIFO_RX_FULL)
#define BIT_I2S_PEND_FIFO_RX_OVER			12
#define MASK_I2S_PEND_FIFO_RX_OVER			(0x01 << BIT_I2S_PEND_FIFO_RX_OVER)

/* FIFO bits */
#define BIT_I2S_FIFO_0						0
#define MASK_I2S_FIFO_0						(0x01 << BIT_I2S_FIFO_0)
#define BIT_I2S_FIFO_1						1
#define MASK_I2S_FIFO_1						(0x01 << BIT_I2S_FIFO_1)
#define BIT_I2S_FIFO_2						2
#define MASK_I2S_FIFO_2						(0x01 << BIT_I2S_FIFO_2)
#define BIT_I2S_FIFO_3						3
#define MASK_I2S_FIFO_3						(0x01 << BIT_I2S_FIFO_3)
#define BIT_I2S_FIFO_4						4
#define MASK_I2S_FIFO_4						(0x01 << BIT_I2S_FIFO_4)
#define BIT_I2S_FIFO_5						5
#define MASK_I2S_FIFO_5						(0x01 << BIT_I2S_FIFO_5)
#define BIT_I2S_FIFO_6						6
#define MASK_I2S_FIFO_6						(0x01 << BIT_I2S_FIFO_6)
#define BIT_I2S_FIFO_7						7
#define MASK_I2S_FIFO_7						(0x01 << BIT_I2S_FIFO_7)
#define BIT_I2S_FIFO_8						8
#define MASK_I2S_FIFO_8						(0x01 << BIT_I2S_FIFO_8)
#define BIT_I2S_FIFO_9						9
#define MASK_I2S_FIFO_9						(0x01 << BIT_I2S_FIFO_9)
#define BIT_I2S_FIFO_10						10
#define MASK_I2S_FIFO_10					(0x01 << BIT_I2S_FIFO_10)
#define BIT_I2S_FIFO_11						11
#define MASK_I2S_FIFO_11					(0x01 << BIT_I2S_FIFO_11)
#define BIT_I2S_FIFO_12						12
#define MASK_I2S_FIFO_12					(0x01 << BIT_I2S_FIFO_12)
#define BIT_I2S_FIFO_13						13
#define MASK_I2S_FIFO_13					(0x01 << BIT_I2S_FIFO_13)
#define BIT_I2S_FIFO_14						14
#define MASK_I2S_FIFO_14					(0x01 << BIT_I2S_FIFO_14)
#define BIT_I2S_FIFO_15						15
#define MASK_I2S_FIFO_15					(0x01 << BIT_I2S_FIFO_15)
#define MASK_I2S_FIFO						(0xFFFF)

/* RX Count */
#define BIT_I2S_RX_COUNT_0					0
#define MASK_I2S_RX_COUNT_0					(0x01 << BIT_I2S_RX_COUNT_0)
#define BIT_I2S_RX_COUNT_1					1
#define MASK_I2S_RX_COUNT_1					(0x01 << BIT_I2S_RX_COUNT_1)
#define BIT_I2S_RX_COUNT_2					2
#define MASK_I2S_RX_COUNT_2					(0x01 << BIT_I2S_RX_COUNT_2)
#define BIT_I2S_RX_COUNT_3					3
#define MASK_I2S_RX_COUNT_3					(0x01 << BIT_I2S_RX_COUNT_3)
#define BIT_I2S_RX_COUNT_4					4
#define MASK_I2S_RX_COUNT_4					(0x01 << BIT_I2S_RX_COUNT_4)
#define BIT_I2S_RX_COUNT_5					5
#define MASK_I2S_RX_COUNT_5					(0x01 << BIT_I2S_RX_COUNT_5)
#define BIT_I2S_RX_COUNT_6					6
#define MASK_I2S_RX_COUNT_6					(0x01 << BIT_I2S_RX_COUNT_6)
#define BIT_I2S_RX_COUNT_7					7
#define MASK_I2S_RX_COUNT_7					(0x01 << BIT_I2S_RX_COUNT_7)
#define BIT_I2S_RX_COUNT_8					8
#define MASK_I2S_RX_COUNT_8					(0x01 << BIT_I2S_RX_COUNT_8)
#define BIT_I2S_RX_COUNT_9					9
#define MASK_I2S_RX_COUNT_9					(0x01 << BIT_I2S_RX_COUNT_9)
#define BIT_I2S_RX_COUNT_10					10
#define MASK_I2S_RX_COUNT_10				(0x01 << BIT_I2S_RX_COUNT_10)
#define BIT_I2S_RX_COUNT_11					11
#define MASK_I2S_RX_COUNT_11				(0x01 << BIT_I2S_RX_COUNT_11)
#define BIT_I2S_RX_COUNT_12					12
#define MASK_I2S_RX_COUNT_12				(0x01 << BIT_I2S_RX_COUNT_12)
#define BIT_I2S_RX_COUNT_13					13
#define MASK_I2S_RX_COUNT_13				(0x01 << BIT_I2S_RX_COUNT_13)
#define BIT_I2S_RX_COUNT_14					14
#define MASK_I2S_RX_COUNT_14				(0x01 << BIT_I2S_RX_COUNT_14)
#define BIT_I2S_RX_COUNT_15					15
#define MASK_I2S_RX_COUNT_15				(0x01 << BIT_I2S_RX_COUNT_15)
#define MASK_I2S_RX_COUNT					(0xFFFF)

/* TX Count */
#define BIT_I2S_TX_COUNT_0					0
#define MASK_I2S_TX_COUNT_0					(0x01 << BIT_I2S_TX_COUNT_0)
#define BIT_I2S_TX_COUNT_1					1
#define MASK_I2S_TX_COUNT_1					(0x01 << BIT_I2S_TX_COUNT_1)
#define BIT_I2S_TX_COUNT_2					2
#define MASK_I2S_TX_COUNT_2					(0x01 << BIT_I2S_TX_COUNT_2)
#define BIT_I2S_TX_COUNT_3					3
#define MASK_I2S_TX_COUNT_3					(0x01 << BIT_I2S_TX_COUNT_3)
#define BIT_I2S_TX_COUNT_4					4
#define MASK_I2S_TX_COUNT_4					(0x01 << BIT_I2S_TX_COUNT_4)
#define BIT_I2S_TX_COUNT_5					5
#define MASK_I2S_TX_COUNT_5					(0x01 << BIT_I2S_TX_COUNT_5)
#define BIT_I2S_TX_COUNT_6					6
#define MASK_I2S_TX_COUNT_6					(0x01 << BIT_I2S_TX_COUNT_6)
#define BIT_I2S_TX_COUNT_7					7
#define MASK_I2S_TX_COUNT_7					(0x01 << BIT_I2S_TX_COUNT_7)
#define BIT_I2S_TX_COUNT_8					8
#define MASK_I2S_TX_COUNT_8					(0x01 << BIT_I2S_TX_COUNT_8)
#define BIT_I2S_TX_COUNT_9					9
#define MASK_I2S_TX_COUNT_9					(0x01 << BIT_I2S_TX_COUNT_9)
#define BIT_I2S_TX_COUNT_10					10
#define MASK_I2S_TX_COUNT_10				(0x01 << BIT_I2S_TX_COUNT_10)
#define BIT_I2S_TX_COUNT_11					11
#define MASK_I2S_TX_COUNT_11				(0x01 << BIT_I2S_TX_COUNT_11)
#define BIT_I2S_TX_COUNT_12					12
#define MASK_I2S_TX_COUNT_12				(0x01 << BIT_I2S_TX_COUNT_12)
#define BIT_I2S_TX_COUNT_13					13
#define MASK_I2S_TX_COUNT_13				(0x01 << BIT_I2S_TX_COUNT_13)
#define BIT_I2S_TX_COUNT_14					14
#define MASK_I2S_TX_COUNT_14				(0x01 << BIT_I2S_TX_COUNT_14)
#define BIT_I2S_TX_COUNT_15					15
#define MASK_I2S_TX_COUNT_15				(0x01 << BIT_I2S_TX_COUNT_15)
#define MASK_I2S_TX_COUNT					(0xFFFF)

/* TYPES ***************************************************************************/
/** @brief Register mappings for I2S registers */
typedef struct
{
	volatile uint16_t I2S_CR;
	volatile uint16_t I2S_CR2;
	volatile uint16_t I2S_IRQ_EN;
	volatile uint16_t I2S_IRQ_PEND;
	volatile uint16_t I2S_RW_DATA;
	volatile uint16_t RESERVED10;
	volatile uint16_t I2S_RX_COUNT;
	volatile uint16_t I2S_TX_COUNT;
} ft900_i2s_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_I2S_REGISTERS_H_ */
