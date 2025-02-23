/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED  AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 05.10.2020 1.00        First Release.
 *         : 02.12.2020 1.01        Added new functions such as the Brainpool curve.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_sce.h"
#include "r_sce_private.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

void R_SCE_func000(uint32_t *InData_PaddedMsg, int32_t MAX_CNT)
{
    uint32_t iLoop    = 0U;
    uint32_t iLoop1   = 0U;
    uint32_t iLoop2   = 0U;
    int32_t  jLoop    = 0U;
    uint32_t kLoop    = 0U;
    uint32_t oLoop    = 0U;
    uint32_t oLoop1   = 0U;
    uint32_t oLoop2   = 0U;
    uint32_t dummy    = 0U;
    uint32_t KEY_ADR  = 0U;
    uint32_t OFS_ADR  = 0U;
    uint32_t MAX_CNT2 = 0U;
    (void)iLoop;
    (void)iLoop1;
    (void)iLoop2;
    (void)jLoop;
    (void)kLoop;
    (void)oLoop;
    (void)oLoop1;
    (void)oLoop2;
    (void)dummy;
    (void)KEY_ADR;
    (void)OFS_ADR;
    (void)MAX_CNT2;
    SCE->REG_104H = 0x000000b4U;
    for (iLoop = 0; iLoop < (uint32_t)MAX_CNT; iLoop = iLoop + 16)
    {
        /* WAIT_LOOP */
        while (1U != SCE->REG_104H_b.B31)
        {
            /* waiting */
        }
        SCE->REG_100H = InData_PaddedMsg[iLoop + 0];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 1];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 2];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 3];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 4];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 5];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 6];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 7];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 8];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 9];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 10];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 11];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 12];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 13];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 14];
        SCE->REG_100H = InData_PaddedMsg[iLoop + 15];
    }
    /* WAIT_LOOP */
    while (0U != SCE->REG_74H_b.B18)
    {
        /* waiting */
    }
    SCE->REG_104H = 0x00000000U;
    SCE->REG_1CH = 0x00001600U;
}

/***********************************************************************************************************************
End of function ./input_dir/S6C1/Cryptographic/R_SCE_func000.prc
***********************************************************************************************************************/
