/**
    @file

    @brief
    Timer and Watchdog Registers

    
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

#ifndef FT900_TIMER_WDT_REGISTERS_H_
#define FT900_TIMER_WDT_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
#define BIT_TIMER_CONTROL_0_SOFT_RESET      (0)
#define MASK_TIMER_CONTROL_0_SOFT_RESET     (1 << BIT_TIMER_CONTROL_0_SOFT_RESET)
#define BIT_TIMER_CONTROL_0_BLOCK_EN        (1)
#define MASK_TIMER_CONTROL_0_BLOCK_EN       (1 << BIT_TIMER_CONTROL_0_BLOCK_EN)

#define BIT_TIMER_CONTROL_1_START(__id__)   (__id__)
#define MASK_TIMER_CONTROL_1_START(__id__)  (1 << BIT_TIMER_CONTROL_1_START(__id__))
#define BIT_TIMER_CONTROL_1_STOP(__id__)    ((__id__) + 4)
#define MASK_TIMER_CONTROL_1_STOP(__id__)   (1 << BIT_TIMER_CONTROL_1_STOP(__id__))

#define BIT_TIMER_CONTROL_2_START_WDT       (0)
#define MASK_TIMER_CONTROL_2_START_WDT      (1 << BIT_TIMER_CONTROL_2_START_WDT)
#define BIT_TIMER_CONTROL_2_CLEAR_WDT       (1)
#define MASK_TIMER_CONTROL_2_CLEAR_WDT      (1 << BIT_TIMER_CONTROL_2_CLEAR_WDT)
#define BIT_TIMER_CONTROL_2_WDG_INT         (2)
#define MASK_TIMER_CONTROL_2_WDG_INT        (1 << BIT_TIMER_CONTROL_2_WDG_INT)
#define BIT_TIMER_CONTROL_2_WDG_INT_EN      (3)
#define MASK_TIMER_CONTROL_2_WDG_INT_EN     (1 << BIT_TIMER_CONTROL_2_WDG_INT_EN)

#define BIT_TIMER_CONTROL_2_PRESCALER_EN(__id__)  ((__id__) + 4)
#define MASK_TIMER_CONTROL_2_PRESCALER_EN(__id__) (1 << BIT_TIMER_CONTROL_2_PRESCALER_EN(__id__))

#define BIT_TIMER_CONTROL_3_MODE(__id__)          (__id__)
#define MASK_TIMER_CONTROL_3_MODE(__id__)         (1 << BIT_TIMER_CONTROL_3_MODE(__id__))
#define BIT_TIMER_CONTROL_3_DIRECTION(__id__)     ((__id__) + 4)
#define MASK_TIMER_CONTROL_3_DIRECTION(__id__)    (1 << BIT_TIMER_CONTROL_3_DIRECTION(__id__))

#define BIT_TIMER_CONTROL_4_CLEAR_A         (0)
#define MASK_TIMER_CONTROL_4_CLEAR_A        (1 << BIT_TIMER_CONTROL_4_CLEAR_A)
#define BIT_TIMER_CONTROL_4_CLEAR_B         (1)
#define MASK_TIMER_CONTROL_4_CLEAR_B        (1 << BIT_TIMER_CONTROL_4_CLEAR_B)
#define BIT_TIMER_CONTROL_4_CLEAR_C         (2)
#define MASK_TIMER_CONTROL_4_CLEAR_C        (1 << BIT_TIMER_CONTROL_4_CLEAR_C)
#define BIT_TIMER_CONTROL_4_CLEAR_D         (3)
#define MASK_TIMER_CONTROL_4_CLEAR_D        (1 << BIT_TIMER_CONTROL_4_CLEAR_D)
//used in FT930 and FT90X Rev C onwards

#define BIT_TIMER_CONTROL_4_PRESC_CLEAR_A     (4)
#define MASK_TIMER_CONTROL_4_PRESC_CLEAR_A    (1 << BIT_TIMER_CONTROL_4_PRESC_CLEAR_A)
#define BIT_TIMER_CONTROL_4_PRESC_CLEAR_B     (5)
#define MASK_TIMER_CONTROL_4_PRESC_CLEAR_B    (1 << BIT_TIMER_CONTROL_4_PRESC_CLEAR_B)
#define BIT_TIMER_CONTROL_4_PRESC_CLEAR_C     (6)
#define MASK_TIMER_CONTROL_4_PRESC_CLEAR_C    (1 << BIT_TIMER_CONTROL_4_PRESC_CLEAR_C)
#define BIT_TIMER_CONTROL_4_PRESC_CLEAR_D     (7)
#define MASK_TIMER_CONTROL_4_PRESC_CLEAR_D    (1 << BIT_TIMER_CONTROL_4_PRESC_CLEAR_D)

//Macros used in FT90X Rev B only

#define BIT_TIMER_CONTROL_4_PRESC_CLEAR     (4)
#define MASK_TIMER_CONTROL_4_PRESC_CLEAR    (1 << BIT_TIMER_CONTROL_4_PRESC_CLEAR)

// end of FT90X Rev B ONLY macros

#define BIT_TIMER_INT_TIMER_INT(__id__)           ((__id__ ) * 2)
#define MASK_TIMER_INT_TIMER_INT(__id__)          (1 << BIT_TIMER_INT_TIMER_INT(__id__))
#define BIT_TIMER_INT_TIMER_INT_EN(__id__)        (((__id__ ) * 2) + 1)
#define MASK_TIMER_INT_TIMER_INT_EN(__id__)       (1 << BIT_TIMER_INT_TIMER_INT_EN(__id__))

#define BIT_TIMER_SELECT_TIMER_WRITE_SEL    (0)
#define MASK_TIMER_SELECT_TIMER_WRITE_SEL   (0x3 << BIT_TIMER_SELECT_TIMER_WRITE_SEL)
#define BIT_TIMER_SELECT_TIMER_READ_SEL     (2)
#define MASK_TIMER_SELECT_TIMER_READ_SEL    (0x3 << BIT_TIMER_SELECT_TIMER_READ_SEL)

#define BIT_TIMER_WDG_WRITE                 (0)
#define MASK_TIMER_WDG_WRITE                (0x1F << BIT_TIMER_WDG_WRITE)

//Macros used in FT90X Rev C

/* This bit is set if there was a second watchdog roll-over that causes
 * a POR-like system reset. It’s automatically cleared after being read.
 */
#define BIT_TIMER_WDG_STATUS                 (0)
#define MASK_TIMER_WDG_STATUS                (1 << BIT_TIMER_WDG_STATUS)

/* TYPES ***************************************************************************/
/** @brief Register mappings for Timer registers */
typedef struct
{
    volatile uint8_t TIMER_CONTROL_0;
    volatile uint8_t TIMER_CONTROL_1;
    volatile uint8_t TIMER_CONTROL_2;
    volatile uint8_t TIMER_CONTROL_3;
    volatile uint8_t TIMER_CONTROL_4;
    volatile uint8_t TIMER_INT;
    volatile uint8_t TIMER_SELECT;
    volatile uint8_t TIMER_WDG;
    volatile uint8_t TIMER_WRITE_LS;
    volatile uint8_t TIMER_WRITE_MS;
    volatile uint8_t TIMER_PRESC_LS;
    volatile uint8_t TIMER_PRESC_MS;
    volatile uint8_t TIMER_READ_LS;
    volatile uint8_t TIMER_READ_MS;
    volatile uint8_t TIMER_RESERVED; /*unused*/
    volatile uint8_t TIMER_WDG_STATUS; /*!!! Available in FT90x Rev C onwards!!! */
} ft900_timer_wdt_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_TIMER_WDT_REGISTERS_H_ */

