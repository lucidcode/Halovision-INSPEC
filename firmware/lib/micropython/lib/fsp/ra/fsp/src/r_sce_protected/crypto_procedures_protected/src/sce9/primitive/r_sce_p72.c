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

fsp_err_t R_SCE_Sha224256GenerateMessageDigestSub(const uint32_t *InData_InitVal, const uint32_t *InData_PaddedMsg, const uint32_t MAX_CNT, uint32_t *OutData_MsgDigest)
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
    if (0x0U != (SCE->REG_1BCH & 0x1fU))
    {
        return FSP_ERR_CRYPTO_SCE_RESOURCE_CONFLICT;
    }
    SCE->REG_84H = 0x00007201U;
    SCE->REG_108H = 0x00000000U;
    R_SCE_func100(0xfb2b56d5U, 0xe26d4898U, 0x26103057U, 0x276b6c33U);
    SCE->REG_7CH = 0x00000011U;
    SCE->REG_104H = 0x00000764U;
    SCE->REG_74H = 0x00000004U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = InData_InitVal[0];
    SCE->REG_100H = InData_InitVal[1];
    SCE->REG_100H = InData_InitVal[2];
    SCE->REG_100H = InData_InitVal[3];
    SCE->REG_100H = InData_InitVal[4];
    SCE->REG_100H = InData_InitVal[5];
    SCE->REG_100H = InData_InitVal[6];
    SCE->REG_100H = InData_InitVal[7];
    SCE->REG_104H = 0x000000b4U;
    SCE->REG_74H = 0x08000002U;
    for (iLoop = 0; iLoop < MAX_CNT ; iLoop = iLoop + 16)
    {
        /* WAIT_LOOP */
        while (1U != SCE->REG_104H_b.B31)
        {
            /* waiting */
        }
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 0];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 1];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 2];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 3];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 4];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 5];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 6];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 7];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 8];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 9];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 10];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 11];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 12];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 13];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 14];
        SCE->REG_100H = InData_PaddedMsg[0+iLoop + 15];
    }
    /* WAIT_LOOP */
    while (0U != SCE->REG_74H_b.B18)
    {
        /* waiting */
    }
    SCE->REG_74H = 0x00000000U;
    SCE->REG_104H = 0x00000000U;
    SCE->REG_1CH = 0x00001600U;
    SCE->REG_74H = 0x00000008U;
    SCE->REG_04H = 0x00000523U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_04H_b.B30)
    {
        /* waiting */
    }
    OutData_MsgDigest[0] = SCE->REG_100H;
    OutData_MsgDigest[1] = SCE->REG_100H;
    OutData_MsgDigest[2] = SCE->REG_100H;
    OutData_MsgDigest[3] = SCE->REG_100H;
    OutData_MsgDigest[4] = SCE->REG_100H;
    OutData_MsgDigest[5] = SCE->REG_100H;
    OutData_MsgDigest[6] = SCE->REG_100H;
    OutData_MsgDigest[7] = SCE->REG_100H;
    R_SCE_func102(0x6a815b4bU, 0xce37585cU, 0x5c537d01U, 0x83023271U);
    SCE->REG_1BCH = 0x00000040U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_18H_b.B12)
    {
        /* waiting */
    }
    return FSP_SUCCESS;
}

/***********************************************************************************************************************
End of function ./input_dir/S6C1/Cryptographic/R_SCE_p72.prc
***********************************************************************************************************************/
