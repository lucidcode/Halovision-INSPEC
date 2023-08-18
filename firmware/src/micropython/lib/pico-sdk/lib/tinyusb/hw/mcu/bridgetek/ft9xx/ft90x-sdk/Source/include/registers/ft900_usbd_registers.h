/**
    @file ft900_usbd_registers.h

    @brief
    USBD (USB Device) registers

    
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

#ifndef FT900_USBD_REGISTERS_H_
#define FT900_USBD_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
#if defined(__FT930__)
#define USBD_MAX_ENDPOINT_COUNT 16
#else
#define USBD_MAX_ENDPOINT_COUNT 8
#endif

/** Bit masks for the USBD Common Interrupt register */
//@{
#define MASK_USBD_CMIF_ALL   0xFF
#define BIT_USBD_CMIF_PHYIRQ 7
#define MASK_USBD_CMIF_PHYIRQ (1 << BIT_USBD_CMIF_PHYIRQ)
#define BIT_USBD_CMIF_PIDIRQ 6
#define MASK_USBD_CMIF_PIDIRQ (1 << BIT_USBD_CMIF_PIDIRQ)
#define BIT_USBD_CMIF_CRC16IRQ 5
#define MASK_USBD_CMIF_CRC16IRQ (1 << BIT_USBD_CMIF_CRC16IRQ)
#define BIT_USBD_CMIF_CRC5IRQ 4
#define MASK_USBD_CMIF_CRC5IRQ (1 << BIT_USBD_CMIF_CRC5IRQ)
#define BIT_USBD_CMIF_RESIRQ 3
#define MASK_USBD_CMIF_RESIRQ (1 << BIT_USBD_CMIF_RESIRQ)
#define BIT_USBD_CMIF_SUSIRQ 2
#define MASK_USBD_CMIF_SUSIRQ (1 << BIT_USBD_CMIF_SUSIRQ)
#define BIT_USBD_CMIF_RSTIRQ 1
#define MASK_USBD_CMIF_RSTIRQ (1 << BIT_USBD_CMIF_RSTIRQ)
#define BIT_USBD_CMIF_SOFIRQ 0
#define MASK_USBD_CMIF_SOFIRQ (1 << BIT_USBD_CMIF_SOFIRQ)
//@}

/** Bit masks for the USBD Common Interrupt Enable register */
//@{
#define MASK_USBD_CMIE_ALL   0xFF
#define BIT_USBD_CMIE_PHYIE 7
#define MASK_USBD_CMIE_PHYIE (1 << BIT_USBD_CMIE_PHYIE)
#define BIT_USBD_CMIE_PIDIE 6
#define MASK_USBD_CMIE_PIDIE (1 << BIT_USBD_CMIE_PIDIE)
#define BIT_USBD_CMIE_CRC16IE 5
#define MASK_USBD_CMIE_CRC16IE (1 << BIT_USBD_CMIE_CRC16IE)
#define BIT_USBD_CMIE_CRC5IE 4
#define MASK_USBD_CMIE_CRC5IE (1 << BIT_USBD_CMIE_CRC5IE)
#define BIT_USBD_CMIE_RESIE 3
#define MASK_USBD_CMIE_RESIE (1 << BIT_USBD_CMIE_RESIE)
#define BIT_USBD_CMIE_SUSIE 2
#define MASK_USBD_CMIE_SUSIE (1 << BIT_USBD_CMIE_SUSIE)
#define BIT_USBD_CMIE_RSTIE 1
#define MASK_USBD_CMIE_RSTIE (1 << BIT_USBD_CMIE_RSTIE)
#define BIT_USBD_CMIE_SOFIE 0
#define MASK_USBD_CMIE_SOFIE (1 << BIT_USBD_CMIE_SOFIE)
//@}

/** Bit masks for the USBD EPIF Endpoint Interrupt Status register */
//@{
#define MASK_USBD_EPIF_IRQ(x)		(1 << (x))
#if defined(__FT930__)
#define BIT_USBD_EPIF_EP15IRQ 15
#define MASK_USBD_EPIF_EP15IRQ (1 << BIT_USBD_EPIF_EP15IRQ)
#define BIT_USBD_EPIF_EP14IRQ 14
#define MASK_USBD_EPIF_EP14IRQ (1 << BIT_USBD_EPIF_EP14IRQ)
#define BIT_USBD_EPIF_EP13IRQ 13
#define MASK_USBD_EPIF_EP13IRQ (1 << BIT_USBD_EPIF_EP13IRQ)
#define BIT_USBD_EPIF_EP12IRQ 12
#define MASK_USBD_EPIF_EP12IRQ (1 << BIT_USBD_EPIF_EP12IRQ)
#define BIT_USBD_EPIF_EP11IRQ 11
#define MASK_USBD_EPIF_EP11IRQ (1 << BIT_USBD_EPIF_EP11IRQ)
#define BIT_USBD_EPIF_EP10IRQ 10
#define MASK_USBD_EPIF_EP10IRQ (1 << BIT_USBD_EPIF_EP10IRQ)
#define BIT_USBD_EPIF_EP9IRQ 9
#define MASK_USBD_EPIF_EP9IRQ (1 << BIT_USBD_EPIF_EP9IRQ)
#define BIT_USBD_EPIF_EP8IRQ 8
#define MASK_USBD_EPIF_EP8IRQ (1 << BIT_USBD_EPIF_EP8IRQ)
#endif
#define BIT_USBD_EPIF_EP7IRQ 7
#define MASK_USBD_EPIF_EP7IRQ (1 << BIT_USBD_EPIF_EP7IRQ)
#define BIT_USBD_EPIF_EP6IRQ 6
#define MASK_USBD_EPIF_EP6IRQ (1 << BIT_USBD_EPIF_EP6IRQ)
#define BIT_USBD_EPIF_EP5IRQ 5
#define MASK_USBD_EPIF_EP5IRQ (1 << BIT_USBD_EPIF_EP5IRQ)
#define BIT_USBD_EPIF_EP4IRQ 4
#define MASK_USBD_EPIF_EP4IRQ (1 << BIT_USBD_EPIF_EP4IRQ)
#define BIT_USBD_EPIF_EP3IRQ 3
#define MASK_USBD_EPIF_EP3IRQ (1 << BIT_USBD_EPIF_EP3IRQ)
#define BIT_USBD_EPIF_EP2IRQ 2
#define MASK_USBD_EPIF_EP2IRQ (1 << BIT_USBD_EPIF_EP2IRQ)
#define BIT_USBD_EPIF_EP1IRQ 1
#define MASK_USBD_EPIF_EP1IRQ (1 << BIT_USBD_EPIF_EP1IRQ)
#define BIT_USBD_EPIF_EP0IRQ 0
#define MASK_USBD_EPIF_EP0IRQ (1 << BIT_USBD_EPIF_EP0IRQ)
//@}

/** Bit masks for the USBD EPIE Endpoint Interrupt Enable register */
//@{
#define BIT_USBD_EPIE_EP0IE 0
#define MASK_USBD_EPIE_EP0IE (1 << BIT_USBD_EPIE_EP0IE)
#define BIT_USBD_EPIE_EP1IE 1
#define MASK_USBD_EPIE_EP1IE (1 << BIT_USBD_EPIE_EP1IE)
#define BIT_USBD_EPIE_EP2IE 2
#define MASK_USBD_EPIE_EP2IE (1 << BIT_USBD_EPIE_EP2IE)
#define BIT_USBD_EPIE_EP3IE 3
#define MASK_USBD_EPIE_EP3IE (1 << BIT_USBD_EPIE_EP3IE)
#define BIT_USBD_EPIE_EP4IE 4
#define MASK_USBD_EPIE_EP4IE (1 << BIT_USBD_EPIE_EP4IE)
#define BIT_USBD_EPIE_EP5IE 5
#define MASK_USBD_EPIE_EP5IE (1 << BIT_USBD_EPIE_EP5IE)
#define BIT_USBD_EPIE_EP6IE 6
#define MASK_USBD_EPIE_EP6IE (1 << BIT_USBD_EPIE_EP6IE)
#define BIT_USBD_EPIE_EP7IE 7
#define MASK_USBD_EPIE_EP7IE (1 << BIT_USBD_EPIE_EP7IE)
//@}

/** Bit masks for the USBD FCTRL Device Mode register */
//@{
#define BIT_USBD_FCTRL_TST_MODE_ENABLE 7
#define MASK_USBD_FCTRL_TST_MODE_ENABLE (1 << BIT_USBD_FCTRL_TST_MODE_ENABLE)
#define BIT_USBD_FCTRL_TST_MODE_SELECT1 6
#define MASK_USBD_FCTRL_TST_MODE_SELECT1 (1 << BIT_USBD_FCTRL_TST_MODE_SELECT1)
#define BIT_USBD_FCTRL_TST_MODE_SELECT0 5
#define MASK_USBD_FCTRL_TST_MODE_SELECT0 (1 << BIT_USBD_FCTRL_TST_MODE_SELECT0)
#define BIT_USBD_FCTRL_MODE_FS_ONLY 1
#define MASK_USBD_FCTRL_MODE_FS_ONLY (1 << BIT_USBD_FCTRL_MODE_FS_ONLY)
#define BIT_USBD_FCTRL_USB_DEV_EN 0
#define MASK_USBD_FCTRL_USB_DEV_EN (1 << BIT_USBD_FCTRL_USB_DEV_EN)
/* Bit 2 and Bit 3 in FCTRL register for improvement in the USB hardware in case
 * of FT900 C only
 * */
#define BIT_USBD_FCTRL_NYET                2
#define BIT_USBD_FCTRL_CONT_RW             3
#define MASK_USBD_FCTRL_IMP_PERF 		((1 << BIT_USBD_FCTRL_CONT_RW)|(1 << BIT_USBD_FCTRL_NYET))
//@}

/** Bit masks for the USBD EP0_STATUS Endpoint 0 Status register */
//@{
#define BIT_USBD_EP0SR_DATAEND 4
#define MASK_USBD_EP0SR_DATAEND (1 << BIT_USBD_EP0SR_DATAEND)
#define BIT_USBD_EP0SR_STALL 3
#define MASK_USBD_EP0SR_STALL (1 << BIT_USBD_EP0SR_STALL)
#define BIT_USBD_EP0SR_SETUP 2
#define MASK_USBD_EP0SR_SETUP (1 << BIT_USBD_EP0SR_SETUP)
#define BIT_USBD_EP0SR_INPRDY 1
#define MASK_USBD_EP0SR_INPRDY (1 << BIT_USBD_EP0SR_INPRDY)
#define BIT_USBD_EP0SR_OPRDY 0
#define MASK_USBD_EP0SR_OPRDY (1 << BIT_USBD_EP0SR_OPRDY)
//@}

/** Bit masks for the USBD EP0_CONTROL Endpoint 0 Control register */
//@{
#define BIT_USBD_EP0CR_SDSTL 0
#define MASK_USBD_EP0CR_SDSTL (1 << BIT_USBD_EP0CR_SDSTL)
#define BIT_USBD_EP0CR_SIZE0 1
#define MASK_USBD_EP0CR_SIZE0 (1 << BIT_USBD_EP0CR_SIZE0)
#define BIT_USBD_EP0CR_SIZE1 2
#define MASK_USBD_EP0CR_SIZE1 (1 << BIT_USBD_EP0CR_SIZE1)
/** EP0 Max Packet Sizes */
//@{
#define BIT_USBD_EP0_MAX_SIZE BIT_USBD_EP0CR_SIZE0
#define MASK_USBD_EP0_MAX_SIZE (3 << BIT_USBD_EP0_MAX_SIZE)
#define USBD_EP0_MAX_SIZE_8    0
#define USBD_EP0_MAX_SIZE_16   1
#define USBD_EP0_MAX_SIZE_32   2
#define USBD_EP0_MAX_SIZE_64   3
//@}
//@}

/** Bit masks for the USBD EPx_STATUS Endpoints 1-7 Status registers */
//@{
#define BIT_USBD_EPxSR_CLR_TOGGLE 7
#define MASK_USBD_EPxSR_CLR_TOGGLE (1 << BIT_USBD_EPxSR_CLR_TOGGLE)
#define BIT_USBD_EPxSR_FIFO_FLUSH 6
#define MASK_USBD_EPxSR_FIFO_FLUSH (1 << BIT_USBD_EPxSR_FIFO_FLUSH)
#define BIT_USBD_EPxSR_DATA_ERR 5
#define MASK_USBD_EPxSR_DATA_ERR (1 << BIT_USBD_EPxSR_DATA_ERR)
#define BIT_USBD_EPxSR_SEND_STALL 4
#define MASK_USBD_EPxSR_SEND_STALL (1 << BIT_USBD_EPxSR_SEND_STALL)
#define BIT_USBD_EPxSR_UNDER_RUN 3
#define MASK_USBD_EPxSR_UNDER_RUN (1 << BIT_USBD_EPxSR_UNDER_RUN)
#define BIT_USBD_EPxSR_OVER_RUN 2
#define MASK_USBD_EPxSR_OVER_RUN (1 << BIT_USBD_EPxSR_OVER_RUN)
#define BIT_USBD_EPxSR_INPRDY 1
#define MASK_USBD_EPxSR_INPRDY (1 << BIT_USBD_EPxSR_INPRDY)
#define BIT_USBD_EPxSR_OPRDY 0
#define MASK_USBD_EPxSR_OPRDY (1 << BIT_USBD_EPxSR_OPRDY)
//@}

/** Bit masks for the USBD EPx_CONTROL Endpoints 1-7 Control registers */
//@{
#define BIT_USBD_EPxCR_DB 7
#define MASK_USBD_EPxCR_DB (1 << BIT_USBD_EPxCR_DB)
#define BIT_USBD_EPxCR_SIZE2 6
#define MASK_USBD_EPxCR_SIZE2 (1 << BIT_USBD_EPxCR_SIZE2)
#define BIT_USBD_EPxCR_SIZE1 5
#define MASK_USBD_EPxCR_SIZE1 (1 << BIT_USBD_EPxCR_SIZE1)
#define BIT_USBD_EPxCR_SIZE0 4
#define MASK_USBD_EPxCR_SIZE0 (1 << BIT_USBD_EPxCR_SIZE0)
#define BIT_USBD_EPxCR_SDSTL 3
#define MASK_USBD_EPxCR_SDSTL (1 << BIT_USBD_EPxCR_SDSTL)
#define BIT_USBD_EPxCR_MODE1 2
#define MASK_USBD_EPxCR_MODE1 (1 << BIT_USBD_EPxCR_MODE1)
#define BIT_USBD_EPxCR_MODE0 1
#define MASK_USBD_EPxCR_MODE0 (1 << BIT_USBD_EPxCR_MODE0)
#define BIT_USBD_EPxCR_DIR 0
#define MASK_USBD_EPxCR_DIR (1 << BIT_USBD_EPxCR_DIR)
/** EPx Max Packet Sizes */
//@{
#define BIT_USBD_EP_MAX_SIZE BIT_USBD_EPxCR_SIZE0
#define MASK_USBD_EP_MAX_SIZE (7 << BIT_USBD_EP_MAX_SIZE)

#define USBD_EP_MAX_SIZE_8    0
#define USBD_EP_MAX_SIZE_16   1
#define USBD_EP_MAX_SIZE_32   2
#define USBD_EP_MAX_SIZE_64   3
#define USBD_EP_MAX_SIZE_128  4
#define USBD_EP_MAX_SIZE_256  5
#define USBD_EP_MAX_SIZE_512  6
#define USBD_EP_MAX_SIZE_1024 7
//@}
/** EPx Endpoint Types */
//@{
#define BIT_USBD_EP_CONTROL_DIS BIT_USBD_EPxCR_MODE0
#define MASK_USBD_DIS (3 << BIT_USBD_EP_CONTROL_DIS)
#define USBD_EP_DIS_BULK 1
#define USBD_EP_DIS_INT  2
#define USBD_EP_DIS_ISO  3
//@}
//@}

/* TYPES ***************************************************************************/

/** \brief USBD device register definition **/
#define USBD_REG_ALIGN32 __attribute__ ((aligned (4)))
typedef volatile struct
{
	uint8_t  cmif USBD_REG_ALIGN32;/// 0x00 -
	uint16_t epif USBD_REG_ALIGN32;/// 0x04 - word
	uint8_t  cmie USBD_REG_ALIGN32;	/// 0x08 -
	uint16_t epie USBD_REG_ALIGN32;/// 0x0c - word
	uint8_t  fctrl USBD_REG_ALIGN32; 	/// 0x10 -
	uint16_t frame USBD_REG_ALIGN32;	/// 0x14 - word
	uint8_t  faddr USBD_REG_ALIGN32;	/// 0x18 -
	struct {
		uint8_t  epxcr USBD_REG_ALIGN32;	/// 0x1c -
		uint8_t  epxsr USBD_REG_ALIGN32;	/// 0x20 -
		uint16_t  epxcnt USBD_REG_ALIGN32;	/// 0x24 - word
		uint8_t  epxfifo USBD_REG_ALIGN32;	/// 0x28 -
	} ep[USBD_MAX_ENDPOINT_COUNT];
} ft900_usbd_regs_t;
#undef USBD_REG_ALIGN32

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/** \brief USBD Register Access
    \details Macros to address registers in the USBD space. Register names from the
    	ft900_USBD_regs_t structure are used.
    	For example: status = *((uint8_t *)&USBD->cmif); will be written as
    	status = USBD_REG(cmif);
**/
//@{
#define USBD_REG(A) (USBD->A)
#define USBD_EP_CR_REG(B) (USBD->ep[B].epxcr)
#define USBD_EP_SR_REG(B) (USBD->ep[B].epxsr)
#define USBD_EP_CNT_REG(B) (USBD->ep[B].epxcnt)
#define USBD_EP_FIFO_REG(B) (USBD->ep[B].epxfifo)
//@}

/**
    @name    Definitions for USBD RAM sizes.
    @details Definitions for maximum amount of RAM available for endpoints.
			 There is a total of 2 kB of RAM for IN endpoints and another 2 kB for
			 OUT endpoints (excluding the RAM allocated to endpoint 0).
			 Therefore the total max packet for all IN endpoints and OUT endpoints
			 must be less than this figure.
			 If double buffering is employed for an endpoint then it will use
			 twice the amount of RAM.
 **/
//@{
#if defined(__FT930__)
#define USBD_RAMTOTAL_IN    (4096)
#define USBD_RAMTOTAL_OUT   (4096)
#else
#define USBD_RAMTOTAL_IN    (sys_check_ft900_revB()?2048:3072)
#define USBD_RAMTOTAL_OUT   (sys_check_ft900_revB()?2048:3072)
#endif
//@}


/* FUNCTION PROTOTYPES *************************************************************/


#endif /* FT900_USBD_REGISTERS_H_ */
