/**
    @file

    @brief
    Interrupt management registers

    
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

#ifndef FT900_INTERRUPT_REGISTERS_H_
#define FT900_INTERRUPT_REGISTERS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
#define BIT_IRQ_PRASSIGN     (0)
#define MASK_IRQ_PRASSIGN    (0x1F << BIT_IRQ_PRASSIGN)

#define BIT_IRQ_CTRL_NESTMAX        (0)
#define MASK_IRQ_CTRL_NESTMAX       (0xF << BIT_IRQ_CTRL_NESTMAX)
#define BIT_IRQ_CTRL_NESTEN         (7)
#define MASK_IRQ_CTRL_NESTEN        (1 << BIT_IRQ_CTRL_NESTEN)
#define BIT_IRQ_CTRL_MASKALL        (7)
#define MASK_IRQ_CTRL_MASKALL       (1 << BIT_IRQ_CTRL_MASKALL)

/* TYPES ***************************************************************************/
/** @brief Register mappings for Interrupt registers */
typedef struct
{
    volatile uint8_t IRQ[32];
    union{
        struct {
            volatile uint8_t nesting;
            volatile uint8_t rsvd0;
            volatile uint8_t rsvd1;
            volatile uint8_t global_mask;
        };
        volatile uint32_t IRQ_CTRL;
    };
} ft900_interrupt_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_INTERRUPT_REGISTERS_H_ */
