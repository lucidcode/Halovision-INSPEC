/**
    @file

    @brief
    Chip management registers

    
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

#ifndef FT930_SLAVE_CPU_REGISTERS_H_
#define FT930_SLAVE_CPU_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/

/* Bit masks for the PMCFG register */

/** Write “0” to de-assert this bit so that the Slave CPU will start. Recommend to de-assert this bit after enable D2XX_mode. */
#define BIT_SLAVE_CPU_CTRL_SLV_RESET  (24)
#define MASK_SLAVE_CPU_CTRL_SLV_RESET (0x1 << BIT_SLAVE_CPU_CTRL_SLV_RESET)

/** D2XX mode bit; Set this bit to “1” to enable D2XX mode (enable Slave sub-system).  */
#define BIT_SLAVE_CPU_CTRL_D2XX_MODE  (0)
#define MASK_SLAVE_CPU_CTRL_D2XX_MODE (0x1 << BIT_SLAVE_CPU_CTRL_D2XX_MODE)

/* TYPES ***************************************************************************/
/** @brief Register mappings for System level registers */
typedef volatile uint32_t ft930_slave_control_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT930_SLAVE_CPU_REGISTERS_H_ */



