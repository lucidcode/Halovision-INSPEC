/**
    @file

    @brief
    General Purpose IO and Pad control registers

    
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

#ifndef FT900_GPIO_REGISTERS_H_
#define FT900_GPIO_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
#define BIT_PADXX_CFG_SLEW         (0)
#define MASK_PADXX_CFG_SLEW        (1 << BIT_PADXX_CFG_SLEW)
#define BIT_PADXX_CFG_SCHMITT      (1)
#define MASK_PADXX_CFG_SCHMITT     (1 << BIT_PADXX_CFG_SCHMITT)
#define BIT_PADXX_CFG_PULL0        (2)
#define MASK_PADXX_CFG_PULL0       (1 << BIT_PADXX_CFG_PULL0)
#define BIT_PADXX_CFG_PULL1        (3)
#define MASK_PADXX_CFG_PULL1       (1 << BIT_PADXX_CFG_PULL1)
#define BIT_PADXX_CFG_DRV0         (4)
#define MASK_PADXX_CFG_DRV0        (1 << BIT_PADXX_CFG_DRV0)
#define BIT_PADXX_CFG_DRV1         (5)
#define MASK_PADXX_CFG_DRV1        (1 << BIT_PADXX_CFG_DRV1)
#define BIT_PADXX_CFG_FUNC0        (6)
#define MASK_PADXX_CFG_FUNC0       (1 << BIT_PADXX_CFG_FUNC0)
#define BIT_PADXX_CFG_FUNC1        (7)
#define MASK_PADXX_CFG_FUNC1       (1 << BIT_PADXX_CFG_FUNC1)

#define BIT_GPIOXX_XX_CFG_INTEDGE      (0)
#define MASK_GPIOXX_XX_CFG_INTEDGE     (1 << BIT_GPIOXX_XX_CFG_INTEDGE)
#define BIT_GPIOXX_XX_CFG_INTEN        (1)
#define MASK_GPIOXX_XX_CFG_INTEN       (1 << BIT_GPIOXX_XX_CFG_INTEN)
#define BIT_GPIOXX_XX_CFG_DIR0         (2)
#define MASK_GPIOXX_XX_CFG_DIR0        (1 << BIT_GPIOXX_XX_CFG_DIR0)
#define BIT_GPIOXX_XX_CFG_DIR1         (3)
#define MASK_GPIOXX_XX_CFG_DIR1        (1 << BIT_GPIOXX_XX_CFG_DIR1)

/* TYPES ***************************************************************************/
/** @brief Registers mappings for GPIO registers */
typedef struct
{
    volatile uint8_t PAD00_CFG;
    volatile uint8_t PAD01_CFG;
    volatile uint8_t PAD02_CFG;
    volatile uint8_t PAD03_CFG;
    volatile uint8_t PAD04_CFG;
    volatile uint8_t PAD05_CFG;
    volatile uint8_t PAD06_CFG;
    volatile uint8_t PAD07_CFG;
    volatile uint8_t PAD08_CFG;
    volatile uint8_t PAD09_CFG;
    volatile uint8_t PAD10_CFG;
    volatile uint8_t PAD11_CFG;
    volatile uint8_t PAD12_CFG;
    volatile uint8_t PAD13_CFG;
    volatile uint8_t PAD14_CFG;
    volatile uint8_t PAD15_CFG;
    volatile uint8_t PAD16_CFG;
    volatile uint8_t PAD17_CFG;
    volatile uint8_t PAD18_CFG;
    volatile uint8_t PAD19_CFG;
    volatile uint8_t PAD20_CFG;
    volatile uint8_t PAD21_CFG;
    volatile uint8_t PAD22_CFG;
    volatile uint8_t PAD23_CFG;
    volatile uint8_t PAD24_CFG;
    volatile uint8_t PAD25_CFG;
    volatile uint8_t PAD26_CFG;
    volatile uint8_t PAD27_CFG;
    volatile uint8_t PAD28_CFG;
    volatile uint8_t PAD29_CFG;
    volatile uint8_t PAD30_CFG;
    volatile uint8_t PAD31_CFG;
    volatile uint8_t PAD32_CFG;
    volatile uint8_t PAD33_CFG;
    volatile uint8_t PAD34_CFG;
    volatile uint8_t PAD35_CFG;
    volatile uint8_t PAD36_CFG;
    volatile uint8_t PAD37_CFG;
    volatile uint8_t PAD38_CFG;
    volatile uint8_t PAD39_CFG;
#if defined(__FT930__)
    volatile uint32_t pad_reserved1;
    volatile uint32_t pad_reserved2;
    volatile uint32_t pad_reserved3;
    volatile uint32_t pad_reserved4;
    volatile uint32_t pad_reserved5;
    volatile uint32_t pad_reserved6;
    volatile uint32_t pad_reserved7;
#else
    volatile uint8_t PAD40_CFG;
    volatile uint8_t PAD41_CFG;
    volatile uint8_t PAD42_CFG;
    volatile uint8_t PAD43_CFG;
    volatile uint8_t PAD44_CFG;
    volatile uint8_t PAD45_CFG;
    volatile uint8_t PAD46_CFG;
    volatile uint8_t PAD47_CFG;
    volatile uint8_t PAD48_CFG;
    volatile uint8_t PAD49_CFG;
    volatile uint8_t PAD50_CFG;
    volatile uint8_t PAD51_CFG;
    volatile uint8_t PAD52_CFG;
    volatile uint8_t PAD53_CFG;
    volatile uint8_t PAD54_CFG;
    volatile uint8_t PAD55_CFG;
    volatile uint8_t PAD56_CFG;
    volatile uint8_t PAD57_CFG;
    volatile uint8_t PAD58_CFG;
    volatile uint8_t PAD59_CFG;
    volatile uint8_t PAD60_CFG;
    volatile uint8_t PAD61_CFG;
    volatile uint8_t PAD62_CFG;
    volatile uint8_t PAD63_CFG;
    volatile uint8_t PAD64_CFG;
    volatile uint8_t PAD65_CFG;
    volatile uint8_t PAD66_CFG;
    volatile uint8_t RESERVED67;
#endif
    volatile uint32_t GPIO00_07_CFG;
    volatile uint32_t GPIO08_15_CFG;
    volatile uint32_t GPIO16_23_CFG;
    volatile uint32_t GPIO24_31_CFG;
    volatile uint32_t GPIO32_39_CFG;
#if defined(__FT930__)
    volatile uint32_t cfg_reserved1;
    volatile uint32_t cfg_reserved2;
    volatile uint32_t cfg_reserved3;
    volatile uint32_t cfg_reserved4;
    volatile uint32_t GPIO00_31_VAL;
    volatile uint32_t GPIO32_39_VAL;
    volatile uint32_t val_reserved;
    volatile uint32_t GPIO00_31_INT_EN;
    volatile uint32_t GPIO32_39_INT_EN;
    volatile uint32_t int_reserved;
    volatile uint32_t GPIO00_31_INT_PEND;
    volatile uint32_t GPIO32_39_INT_PEND;
    volatile uint32_t int_pend_reserved;
#else
    volatile uint32_t GPIO40_47_CFG;
    volatile uint32_t GPIO48_55_CFG;
    volatile uint32_t GPIO56_63_CFG;
    volatile uint32_t GPIO64_71_CFG;
    volatile uint32_t GPIO00_31_VAL;
    volatile uint32_t GPIO32_63_VAL;
    volatile uint32_t GPIO64_66_VAL;
    volatile uint32_t GPIO00_31_INT_EN;
    volatile uint32_t GPIO32_63_INT_EN;
    volatile uint32_t GPIO64_66_INT_EN;
    volatile uint32_t GPIO00_31_INT_PEND;
    volatile uint32_t GPIO32_63_INT_PEND;
    volatile uint32_t GPIO64_66_INT_PEND;
#endif
} ft900_gpio_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_GPIO_REGISTERS_H_ */
