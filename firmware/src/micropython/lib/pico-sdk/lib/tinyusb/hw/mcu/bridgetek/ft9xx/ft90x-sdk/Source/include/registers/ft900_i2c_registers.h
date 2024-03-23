/**
    @file

    @brief
    I2C registers

    
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


#ifndef FT900_I2C_REGISTERS_H_
#define FT900_I2C_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
/* I2CM_SLV_ADDR bits */
#define BIT_I2CM_SLV_ADDR_RX_OP						0
#define MASK_I2CM_SLV_ADDR_RX_OP					(0x01 << BIT_I2CM_SLV_ADDR_RX_OP)
#define BIT_I2CM_SLV_ADDR_0							1
#define MASK_I2CM_SLV_ADDR_0						(0x01 << BIT_I2CM_SLV_ADDR_0)
#define BIT_I2CM_SLV_ADDR_1							2
#define MASK_I2CM_SLV_ADDR_1						(0x01 << BIT_I2CM_SLV_ADDR_1)
#define BIT_I2CM_SLV_ADDR_2							3
#define MASK_I2CM_SLV_ADDR_2						(0x01 << BIT_I2CM_SLV_ADDR_2)
#define BIT_I2CM_SLV_ADDR_3							4
#define MASK_I2CM_SLV_ADDR_3						(0x01 << BIT_I2CM_SLV_ADDR_3)
#define BIT_I2CM_SLV_ADDR_4							5
#define MASK_I2CM_SLV_ADDR_4						(0x01 << BIT_I2CM_SLV_ADDR_4)
#define BIT_I2CM_SLV_ADDR_5							6
#define MASK_I2CM_SLV_ADDR_5						(0x01 << BIT_I2CM_SLV_ADDR_5)
#define BIT_I2CM_SLV_ADDR_6							7
#define MASK_I2CM_SLV_ADDR_6						(0x01 << BIT_I2CM_SLV_ADDR_6)
#define MASK_I2CM_SLV_ADDR							(0x7F << BIT_I2CM_SLV_ADDR_0)

/* I2CM_CNTL bits */
#define BIT_I2CM_CNTL_RUN							0
#define MASK_I2CM_CNTL_RUN							(0x01 << BIT_I2CM_CNTL_RUN)
#define BIT_I2CM_CNTL_START							1
#define MASK_I2CM_CNTL_START						(0x01 << BIT_I2CM_CNTL_START)
#define BIT_I2CM_CNTL_STOP							2
#define MASK_I2CM_CNTL_STOP							(0x01 << BIT_I2CM_CNTL_STOP)
#define BIT_I2CM_CNTL_ACK 							3
#define MASK_I2CM_CNTL_ACK							(0x01 << BIT_I2CM_CNTL_ACK)
#define BIT_I2CM_CNTL_HS							4
#define MASK_I2CM_CNTL_HS							(0x01 << BIT_I2CM_CNTL_HS)
#define BIT_I2CM_CNTL_ADDR							5
#define MASK_I2CM_CNTL_ADDR							(0x01 << BIT_I2CM_CNTL_ADDR)
#define BIT_I2CM_CNTL_SLV_RST						6
#define MASK_I2CM_CNTL_SLV_RST						(0x01 << BIT_I2CM_CNTL_SLV_RST)
#define BIT_I2CM_CNTL_I2C_RST						7
#define MASK_I2CM_CNTL_I2C_RST						(0x01 << BIT_I2CM_CNTL_I2C_RST)

/* I2CM_STATUS bits	*/
#define BIT_I2CM_STATUS_I2C_BUSY					0
#define MASK_I2CM_STATUS_I2C_BUSY					(0x01 << BIT_I2CM_STATUS_I2C_BUSY)
#define BIT_I2CM_STATUS_I2C_ERR						1
#define MASK_I2CM_STATUS_I2C_ERR					(0x01 << BIT_I2CM_STATUS_I2C_ERR)
#define BIT_I2CM_STATUS_ADDR_ACK					2
#define MASK_I2CM_STATUS_ADDR_ACK					(0x01 << BIT_I2CM_STATUS_ADDR_ACK)
#define BIT_I2CM_STATUS_DATA_ACK					3
#define MASK_I2CM_STATUS_DATA_ACK					(0x01 << BIT_I2CM_STATUS_DATA_ACK)
#define BIT_I2CM_STATUS_ARB_LOST					4
#define MASK_I2CM_STATUS_ARB_LOST					(0x01 << BIT_I2CM_STATUS_ARB_LOST)
#define BIT_I2CM_STATUS_I2C_IDLE					5
#define MASK_I2CM_STATUS_I2C_IDLE					(0x01 << BIT_I2CM_STATUS_I2C_IDLE)
#define BIT_I2CM_STATUS_BUS_BUSY					6
#define MASK_I2CM_STATUS_BUS_BUSY					(0x01 << BIT_I2CM_STATUS_BUS_BUSY)

/* I2CM_TIME_PERIOD bits */
#define BIT_I2CM_TIME_PERIOD_STD_FAST_FPLUS			0
#define MASK_I2CM_TIME_PERIOD_STD_FAST_FPLUS		(0x7F << BIT_I2CM_TIME_PERIOD_STD_FAST_FPLUS)
#define BIT_I2CM_TIME_PERIOD_HIGHSPEED				0
#define MASK_I2CM_TIME_PERIOD_HIGHSPEED				(0x1F << BIT_I2CM_TIME_PERIOD_HIGHSPEED)
#define BIT_I2CM_TIME_PERIOD_FS_SELECT				6
#define MASK_I2CM_TIME_PERIOD_FS_SELECT			    (0x01 << BIT_I2CM_TIME_PERIOD_FS_SELECT)
#define BIT_I2CM_TIME_PERIOD_HS_SELECT				7
#define MASK_I2CM_TIME_PERIOD_HS_SELECT				(0x01 << BIT_I2CM_TIME_PERIOD_HS_SELECT)

/* I2CM_FIFO_LEN bits */
#define BIT_I2CM_FIFO_BL_0							0
#define MASK_I2CM_FIFO_BL_0							(0x01 << BIT_I2CM_FIFO_BL_0)
#define BIT_I2CM_FIFO_BL_1							1
#define MASK_I2CM_FIFO_BL_1							(0x01 << BIT_I2CM_FIFO_BL_1)
#define BIT_I2CM_FIFO_BL_2							2
#define MASK_I2CM_FIFO_BL_2							(0x01 << BIT_I2CM_FIFO_BL_2)
#define BIT_I2CM_FIFO_BL_3							3
#define MASK_I2CM_FIFO_BL_3							(0x01 << BIT_I2CM_FIFO_BL_3)
#define BIT_I2CM_FIFO_BL_4							4
#define MASK_I2CM_FIFO_BL_4							(0x01 << BIT_I2CM_FIFO_BL_4)
#define BIT_I2CM_FIFO_BL_5							5
#define MASK_I2CM_FIFO_BL_5							(0x01 << BIT_I2CM_FIFO_BL_5)
#define BIT_I2CM_FIFO_BL_6							6
#define MASK_I2CM_FIFO_BL_6							(0x01 << BIT_I2CM_FIFO_BL_6)
#define BIT_I2CM_FIFO_BL_7							7
#define MASK_I2CM_FIFO_BL_7							(0x01 << BIT_I2CM_FIFO_BL_7)
#define MASK_I2CM_FIFO_LEN							(0xFF << BIT_I2CM_FIFO_BL_0)

/* I2CM_FIFO_INT_ENABLE bits */
#define BIT_I2CM_FIFO_INT_ENABLE_TX_EMPTY			0
#define MASK_I2CM_FIFO_INT_ENABLE_TX_EMPTY			(0x01 << BIT_I2CM_FIFO_INT_ENABLE_TX_EMPTY)
#define BIT_I2CM_FIFO_INT_ENABLE_TX_HALF			1
#define MASK_I2CM_FIFO_INT_ENABLE_TX_HALF			(0x01 << BIT_I2CM_FIFO_INT_ENABLE_TX_HALF)
#define BIT_I2CM_FIFO_INT_ENABLE_TX_FULL			2
#define MASK_I2CM_FIFO_INT_ENABLE_TX_FULL			(0x01 << BIT_I2CM_FIFO_INT_ENABLE_TX_FULL)
#define BIT_I2CM_FIFO_INT_ENABLE_RX_EMPTY			3
#define MASK_I2CM_FIFO_INT_ENABLE_RX_EMPTY			(0x01 << BIT_I2CM_FIFO_INT_ENABLE_RX_EMPTY)
#define BIT_I2CM_FIFO_INT_ENABLE_RX_HALF			4
#define MASK_I2CM_FIFO_INT_ENABLE_RX_HALF			(0x01 << BIT_I2CM_FIFO_INT_ENABLE_RX_HALF)
#define BIT_I2CM_FIFO_INT_ENABLE_RX_FULL			5
#define MASK_I2CM_FIFO_INT_ENABLE_RX_FULL			(0x01 << BIT_I2CM_FIFO_INT_ENABLE_RX_FULL)
#define BIT_I2CM_FIFO_INT_ENABLE_I2C_INT				6
#define MASK_I2CM_FIFO_INT_ENABLE_I2C_INT			(0x01 << BIT_I2CM_FIFO_INT_ENABLE_I2C_INT)
#define BIT_I2CM_FIFO_INT_ENABLE_DONE				7
#define MASK_I2CM_FIFO_INT_ENABLE_DONE				(0x01 << BIT_I2CM_FIFO_INT_ENABLE_DONE)

/* I2CM_FIFO_INT_PEND bits */
#define BIT_I2CM_FIFO_INT_PEND_TX_EMPTY				0
#define MASK_I2CM_FIFO_INT_PEND_TX_EMPTY			(0x01 << BIT_I2CM_FIFO_INT_PEND_TX_EMPTY)
#define BIT_I2CM_FIFO_INT_PEND_TX_HALF				1
#define MASK_I2CM_FIFO_INT_PEND_TX_HALF				(0x01 << BIT_I2CM_FIFO_INT_PEND_TX_HALF)
#define BIT_I2CM_FIFO_INT_PEND_TX_FULL				2
#define MASK_I2CM_FIFO_INT_PEND_TX_FULL				(0x01 << BIT_I2CM_FIFO_INT_PEND_TX_FULL)
#define BIT_I2CM_FIFO_INT_PEND_RX_EMPTY				3
#define MASK_I2CM_FIFO_INT_PEND_RX_EMPTY			(0x01 << BIT_I2CM_FIFO_INT_PEND_RX_EMPTY)
#define BIT_I2CM_FIFO_INT_PEND_RX_HALF				4
#define MASK_I2CM_FIFO_INT_PEND_RX_HALF				(0x01 << BIT_I2CM_FIFO_INT_PEND_RX_HALF)
#define BIT_I2CM_FIFO_INT_PEND_RX_FULL				5
#define MASK_I2CM_FIFO_INT_PEND_RX_FULL				(0x01 << BIT_I2CM_FIFO_INT_PEND_RX_FULL)
#define BIT_I2CM_FIFO_INT_PEND_I2C_INT				6
#define MASK_I2CM_FIFO_INT_PEND_I2C_INT				(0x01 << BIT_I2CM_FIFO_INT_PEND_I2C_INT)
#define BIT_I2CM_FIFO_INT_PEND_DONE					7
#define MASK_I2CM_FIFO_INT_PEND_DONE				(0x01 << BIT_I2CM_FIFO_INT_PEND_DONE)

/* I2CM_FIFO_DATA bits */
#define BIT_I2CM_FIFO_DATA_0						0
#define MASK_I2CM_FIFO_DATA_0						(0x01 << BIT_I2CM_FIFO_DATA_0)
#define BIT_I2CM_FIFO_DATA_1						1
#define MASK_I2CM_FIFO_DATA_1						(0x01 << BIT_I2CM_FIFO_DATA_1)
#define BIT_I2CM_FIFO_DATA_2						2
#define MASK_I2CM_FIFO_DATA_2						(0x01 << BIT_I2CM_FIFO_DATA_2)
#define BIT_I2CM_FIFO_DATA_3						3
#define MASK_I2CM_FIFO_DATA_3						(0x01 << BIT_I2CM_FIFO_DATA_3)
#define BIT_I2CM_FIFO_DATA_4						4
#define MASK_I2CM_FIFO_DATA_4						(0x01 << BIT_I2CM_FIFO_DATA_4)
#define BIT_I2CM_FIFO_DATA_5						5
#define MASK_I2CM_FIFO_DATA_5						(0x01 << BIT_I2CM_FIFO_DATA_5)
#define BIT_I2CM_FIFO_DATA_6						6
#define MASK_I2CM_FIFO_DATA_6						(0x01 << BIT_I2CM_FIFO_DATA_6)
#define BIT_I2CM_FIFO_DATA_7						7
#define MASK_I2CM_FIFO_DATA_7						(0x01 << BIT_I2CM_FIFO_DATA_7)
#define MASK_I2CM_FIFO_DATA							(0xFF << BIT_I2CM_FIFO_DATA_0)

/* I2CM_TRIG bits */
#define BIT_I2CM_TRIG_RX_OP							7
#define MASK_I2CM_TRIG_RX_OP						(0x01 << BIT_I2CM_TRIG_RX_OP)

/* I2CS_OWN_ADDR bits */
#define BIT_I2CS_OWN_ADDR_0							0
#define MASK_I2CS_OWN_ADDR_0						(0x01 << BIT_I2CS_OWN_ADDR_0)
#define BIT_I2CS_OWN_ADDR_1							1
#define MASK_I2CS_OWN_ADDR_1						(0x01 << BIT_I2CS_OWN_ADDR_1)
#define BIT_I2CS_OWN_ADDR_2							2
#define MASK_I2CS_OWN_ADDR_2						(0x01 << BIT_I2CS_OWN_ADDR_2)
#define BIT_I2CS_OWN_ADDR_3							3
#define MASK_I2CS_OWN_ADDR_3						(0x01 << BIT_I2CS_OWN_ADDR_3)
#define BIT_I2CS_OWN_ADDR_4							4
#define MASK_I2CS_OWN_ADDR_4						(0x01 << BIT_I2CS_OWN_ADDR_4)
#define BIT_I2CS_OWN_ADDR_5							5
#define MASK_I2CS_OWN_ADDR_5						(0x01 << BIT_I2CS_OWN_ADDR_5)
#define BIT_I2CS_OWN_ADDR_6							6
#define MASK_I2CS_OWN_ADDR_6						(0x01 << BIT_I2CS_OWN_ADDR_6)
#define MASK_I2CS_OWN_ADDR							(0x7F << BIT_I2CS_OWN_ADDR_0)
	
/* I2CS_CNTL bits */
#define BIT_I2CS_CNTL_SEND_FIN_CLR					2
#define MASK_I2CS_CNTL_SEND_FIN_CLR					(0x01 << BIT_I2CS_CNTL_SEND_FIN_CLR)
#define BIT_I2CS_CNTL_REC_FIN_CLR					3
#define MASK_I2CS_CNTL_REC_FIN_CLR					(0x01 << BIT_I2CS_CNTL_REC_FIN_CLR)
#define BIT_I2CS_CNTL_DEV_ACTV						6
#define MASK_I2CS_CNTL_DEV_ACTV						(0x01 << BIT_I2CS_CNTL_DEV_ACTV)
#define BIT_I2CS_CNTL_I2C_RST						7
#define MASK_I2CS_CNTL_I2C_RST						(0x01 << BIT_I2CS_CNTL_I2C_RST)
	
/* I2CS_STATUS bits	*/
#define BIT_I2CS_STATUS_RX_REQ						0
#define MASK_I2CS_STATUS_RX_REQ						(0x01 << BIT_I2CS_STATUS_RX_REQ)
#define BIT_I2CS_STATUS_TX_REQ						1
#define MASK_I2CS_STATUS_TX_REQ						(0x01 << BIT_I2CS_STATUS_TX_REQ)
#define BIT_I2CS_STATUS_SEND_FIN					2
#define MASK_I2CS_STATUS_SEND_FIN					(0x01 << BIT_I2CS_STATUS_SEND_FIN)
#define BIT_I2CS_STATUS_REC_FIN						3
#define MASK_I2CS_STATUS_REC_FIN					(0x01 << BIT_I2CS_STATUS_REC_FIN)
#define BIT_I2CS_STATUS_BUS_ACTV					4
#define MASK_I2CS_STATUS_BUS_ACTV					(0x01 << BIT_I2CS_STATUS_BUS_ACTV)
#define BIT_I2CS_STATUS_DEV_ACTV					6
#define MASK_I2CS_STATUS_DEV_ACTV					(0x01 << BIT_I2CS_STATUS_DEV_ACTV)

/* I2CS_FIFO_INT_ENABLE bits */
#define BIT_I2CS_FIFO_INT_ENABLE_TX_EMPTY			0
#define MASK_I2CS_FIFO_INT_ENABLE_TX_EMPTY			(0x01 << BIT_I2CS_FIFO_INT_ENABLE_TX_EMPTY)
#define BIT_I2CS_FIFO_INT_ENABLE_TX_HALF			1
#define MASK_I2CS_FIFO_INT_ENABLE_TX_HALF			(0x01 << BIT_I2CS_FIFO_INT_ENABLE_TX_HALF)
#define BIT_I2CS_FIFO_INT_ENABLE_TX_FULL			2
#define MASK_I2CS_FIFO_INT_ENABLE_TX_FULL			(0x01 << BIT_I2CS_FIFO_INT_ENABLE_TX_FULL)
#define BIT_I2CS_FIFO_INT_ENABLE_RX_EMPTY			3
#define MASK_I2CS_FIFO_INT_ENABLE_RX_EMPTY			(0x01 << BIT_I2CS_FIFO_INT_ENABLE_RX_EMPTY)
#define BIT_I2CS_FIFO_INT_ENABLE_RX_HALF			4
#define MASK_I2CS_FIFO_INT_ENABLE_RX_HALF			(0x01 << BIT_I2CS_FIFO_INT_ENABLE_RX_HALF)
#define BIT_I2CS_FIFO_INT_ENABLE_RX_FULL			5
#define MASK_I2CS_FIFO_INT_ENABLE_RX_FULL			(0x01 << BIT_I2CS_FIFO_INT_ENABLE_RX_FULL)
#define BIT_I2CS_FIFO_INT_ENABLE_I2C_INT			6
#define MASK_I2CS_FIFO_INT_ENABLE_I2C_INT			(0x01 << BIT_I2CS_FIFO_INT_ENABLE_I2C_INT)
#define BIT_I2CS_FIFO_INT_ENABLE_DONE				7
#define MASK_I2CS_FIFO_INT_ENABLE_DONE				(0x01 << BIT_I2CS_FIFO_INT_ENABLE_DONE)

/* I2CS_FIFO_INT_PEND bits */
#define BIT_I2CS_FIFO_INT_PEND_TX_EMPTY				0
#define MASK_I2CS_FIFO_INT_PEND_TX_EMPTY			(0x01 << BIT_I2CS_FIFO_INT_PEND_TX_EMPTY)
#define BIT_I2CS_FIFO_INT_PEND_TX_HALF				1
#define MASK_I2CS_FIFO_INT_PEND_TX_HALF				(0x01 << BIT_I2CS_FIFO_INT_PEND_TX_HALF)
#define BIT_I2CS_FIFO_INT_PEND_TX_FULL				2
#define MASK_I2CS_FIFO_INT_PEND_TX_FULL				(0x01 << BIT_I2CS_FIFO_INT_PEND_TX_FULL)
#define BIT_I2CS_FIFO_INT_PEND_RX_EMPTY				3
#define MASK_I2CS_FIFO_INT_PEND_RX_EMPTY			(0x01 << BIT_I2CS_FIFO_INT_PEND_RX_EMPTY)
#define BIT_I2CS_FIFO_INT_PEND_RX_HALF				4
#define MASK_I2CS_FIFO_INT_PEND_RX_HALF				(0x01 << BIT_I2CS_FIFO_INT_PEND_RX_HALF)
#define BIT_I2CS_FIFO_INT_PEND_RX_FULL				5
#define MASK_I2CS_FIFO_INT_PEND_RX_FULL				(0x01 << BIT_I2CS_FIFO_INT_PEND_RX_FULL)
#define BIT_I2CS_FIFO_INT_PEND_I2C_INT				6
#define MASK_I2CS_FIFO_INT_PEND_I2C_INT				(0x01 << BIT_I2CS_FIFO_INT_PEND_I2C_INT)
#define BIT_I2CS_FIFO_INT_PEND_DONE					7
#define MASK_I2CS_FIFO_INT_PEND_DONE				(0x01 << BIT_I2CS_FIFO_INT_PEND_DONE)

/* I2CS_TRIG bits */
#define BIT_I2CS_TRIG_RX_OP							7
#define MASK_I2CS_TRIG_RX_OP						(0x01 << BIT_I2CS_TRIG_RX_OP)

/* TYPES ***************************************************************************/
/** @brief Register mappings for I2C Master registers */
typedef struct
{
	volatile uint8_t I2CM_SLV_ADDR;			/* Slave address register */
	volatile uint8_t I2CM_CNTL_STATUS;		/* Control and Status register */
	volatile uint8_t I2CM_DATA;				/* Receive / transmit data register */
	volatile uint8_t I2CM_TIME_PERIOD;		/* Timer period register */
	volatile uint8_t I2CM_FIFO_LEN;			/* FIFO mode byte length register */
	volatile uint8_t I2CM_FIFO_INT_ENABLE;	/* FIFO mode interrupt enable register */
	volatile uint8_t I2CM_FIFO_INT_PEND;	/* FIFO mode interrupt pending register */
	volatile uint8_t I2CM_FIFO_DATA;		/* FIFO data register */
	volatile uint8_t I2CM_TRIG;				/* FIFO transfer trigger register */
} ft900_i2cm_regs_t;

/** @brief Register mappings for I2C Slave registers */
typedef struct
{
	volatile uint8_t I2CS_OWN_ADDR;			/* Own address register */
	volatile uint8_t I2CS_CNTL_STATUS;      /* Control and Status register	volatile uint8_t DATA; */
    volatile uint8_t I2CS_DATA;             /* Receive / transmit data register */
    volatile uint8_t I2CS_UNUSED3;
	volatile uint8_t I2CS_FIFO_LEN;			/* FIFO mode byte length register */
	volatile uint8_t I2CS_FIFO_INT_ENABLE;	/* FIFO mode interrupt enable register */
	volatile uint8_t I2CS_FIFO_INT_PEND;	/* FIFO mode interrupt pending register */
	volatile uint8_t I2CS_FIFO_DATA;		/* FIFO data register */
	volatile uint8_t I2CS_TRIG;				/* FIFO transfer trigger register */
} ft900_i2cs_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_I2C_REGISTERS_H_ */
