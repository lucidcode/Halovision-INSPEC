/**
    @file

    @brief
    High Bandwidth ISO configuration registers

    
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


#ifndef FT900_USBD_HBW_REGISTERS_H_
#define FT900_USBD_HBW_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
/** Bit masks for the USBD HBW ISO IN control registers */

//@{
#define BIT_USBD_HBW_CTRL0_HBW_INI 			7
#define MASK_USBD_HBW_CTRL0_HBW_INI 		(1 << BIT_USBD_HBW_CTRL0_HBW_INI)
#define BIT_USBD_HBW_CTRL0_OVERRUN 			6
#define MASK_USBD_HBW_CTRL0_OVERRUN 		(1 << BIT_USBD_HBW_CTRL0_OVERRUN)
#define BIT_USBD_HBW_CTRL0_PTSON			4
#define MASK_USBD_HBW_CTRL0_PTSON 			(1 << BIT_USBD_HBW_CTRL0_PTSON)
#define BIT_USBD_HBW_CTRL0_SCRON 			3
#define MASK_USBD_HBW_CTRL0_SCRON 			(1 << BIT_USBD_HBW_CTRL0_SCRON)
#define BIT_USBD_HBW_CTRL0_SET_STILL		2
#define MASK_USBD_HBW_CTRL0_SET_STILL 		(1 << BIT_USBD_HBW_CTRL0_SET_STILL)
#define BIT_USBD_HBW_CTRL0_SET_ERROR 		1
#define MASK_USBD_HBW_CTRL0_SET_ERROR 		(1 << BIT_USBD_HBW_CTRL0_SET_ERROR)
#define BIT_USBD_HBW_CTRL0_AUTO_HEADER	 	0
#define MASK_USBD_HBW_CTRL0_AUTO_HEADER     (1 << BIT_USBD_HBW_CTRL0_AUTO_HEADER)
//@}

//@{
#define BIT_USBD_HBW_CTRL1_FIFO_SIZE 		0
#define MASK_USBD_HBW_CTRL1_FIFO_SIZE 		(0x7F << BIT_USBD_HBW_CTRL1_FIFO_SIZE)
//@}

//@{
#define BIT_USBD_HBW_CTRL2_MAXPKTSIZE 		4
#define MASK_USBD_HBW_CTRL2_MAXPKTSIZE 		(0xF << BIT_USBD_HBW_CTRL2_MAXPKTSIZE)
#define BIT_USBD_HBW_CTRL2_HBWMODE 			2
#define MASK_USBD_HBW_CTRL2_HBWMODE 		(0x3 << BIT_USBD_HBW_CTRL2_HBWMODE)
#define BIT_USBD_HBW_CTRL2_THRESH_SEL 		0
#define MASK_USBD_HBW_CTRL2_THRESH_SEL 		(0x3 << BIT_USBD_HBW_CTRL2_THRESH_SEL)
//@}

//@{
#define BIT_USBD_HBW_CTRL3_HAVESPACE 		7
#define MASK_USBD_HBW_CTRL3_HAVESPACE 		(1 << BIT_USBD_HBW_CTRL3_HAVESPACE)
#define BIT_USBD_HBW_CTRL3_FULL 			6
#define MASK_USBD_HBW_CTRL3_FULL 			(1 << BIT_USBD_HBW_CTRL3_FULL)
#define BIT_USBD_HBW_CTRL3_INPRDY 			5
#define MASK_USBD_HBW_CTRL3_INPRDY 			(1 << BIT_USBD_HBW_CTRL3_INPRDY)
#define BIT_USBD_HBW_CTRL3_SEQDATA_END 		4
#define MASK_USBD_HBW_CTRL3_SEQDATA_END		(1 << BIT_USBD_HBW_CTRL3_SEQDATA_END)
#define BIT_USBD_HBW_CTRL3_ENDP_NUM 		0
#define MASK_USBD_HBW_CTRL3_ENDP_NUM		(0xF << BIT_USBD_HBW_CTRL3_ENDP_NUM)
//@}

//@{
#define BIT_USBD_HBW_INTERRUPT_EN 			0
#define MASK_USBD_HBW_INTERRUPT_EN 			(0x7F << BIT_USBD_HBW_INTERRUPT_EN)
//@}

//@{
#define BIT_USBD_HBW_INTERRUPT_STATUS 		0
#define MASK_USBD_HBW_INTERRUPT_STATUS 		(1 << BIT_USBD_HBW_INTERRUPT_STATUS)
//@}

/* TYPES ***************************************************************************/
/** @brief Register mappings for High bandwidth ISO IN endpoint configuration registers */
typedef struct
{
	volatile uint8_t auto_header;
	volatile uint8_t set_error;
	volatile uint8_t set_still;
	volatile uint8_t scron;
	volatile uint8_t ptson;
	volatile uint8_t reserved;
	volatile uint8_t overrun;
	volatile uint8_t hbw_ini;

}ft900_usbd_hbw_header_ctrl_regs_t;

typedef struct
{
	volatile uint32_t ctrl0;
	volatile uint32_t ctrl1;
	volatile uint32_t ctrl2;
	volatile uint32_t ctrl3;
	volatile uint32_t interrupt_en;
	volatile uint32_t interrupt_status;

}ft900_usbd_hbw_ctrl_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_USBD_HBW_REGISTERS_H_ */
