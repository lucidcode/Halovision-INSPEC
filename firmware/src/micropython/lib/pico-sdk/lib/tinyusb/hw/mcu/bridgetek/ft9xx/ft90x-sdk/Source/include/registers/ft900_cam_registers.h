/**
    @file

    @brief
    Camera/Parallel interface registers

    
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


#ifndef FT900_CAM_REGISTERS_H_
#define FT900_CAM_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
#define BIT_CAM_REG1_THRESHOLD      (0)
#define MASK_CAM_REG1_THRESHOLD     (0xFFF << BIT_CAM_REG1_THRESHOLD)
#define BIT_CAM_REG1_TRIGPAT0       (12)
#define MASK_CAM_REG1_TRIGPAT0      (1 << BIT_CAM_REG1_TRIGPAT0)
#define BIT_CAM_REG1_TRIGPAT1       (13)
#define MASK_CAM_REG1_TRIGPAT1      (1 << BIT_CAM_REG1_TRIGPAT1)
#define BIT_CAM_REG1_TRIGPAT2       (14)
#define MASK_CAM_REG1_TRIGPAT2      (1 << BIT_CAM_REG1_TRIGPAT2)
#define BIT_CAM_REG1_TRIGPAT3       (15)
#define MASK_CAM_REG1_TRIGPAT3      (1 << BIT_CAM_REG1_TRIGPAT3)
#define BIT_CAM_REG1_COUNT          (16)
#define MASK_CAM_REG1_COUNT         (0xFFFF << BIT_CAM_REG1_COUNT)

#define BIT_CAM_REG2_FULLNESS       (0)
#define MASK_CAM_REG2_FULLNESS      (0xFFF << BIT_CAM_REG2_FULLNESS)

#define BIT_CAM_REG4_INTEN          (0)
#define MASK_CAM_REG4_INTEN         (1 << BIT_CAM_REG4_INTEN)
#define BIT_CAM_REG4_CLKSENSE       (1)
#define MASK_CAM_REG4_CLKSENSE      (1 << BIT_CAM_REG4_CLKSENSE)
#define BIT_CAM_REG4_HASDATA        (2)
#define MASK_CAM_REG4_HASDATA       (1 << BIT_CAM_REG4_HASDATA)

/* TYPES ***************************************************************************/
/** @brief Register mappings for CAM registers */
typedef struct
{
    volatile uint32_t CAM_REG1;
    volatile uint32_t CAM_REG2;
    volatile uint32_t CAM_REG3;
    volatile uint32_t CAM_REG4;
} ft900_cam_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_CAM_REGISTERS_H_ */
