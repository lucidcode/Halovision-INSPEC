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

fsp_err_t R_SCE_Aes128CcmDecryptFinalSub(uint32_t *InData_Text, uint32_t *InData_TextLen, uint32_t *InData_MAC, uint32_t *InData_MACLength, uint32_t *OutData_Text)
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
    R_SCE_func100(0xa7d161b2U, 0x08916af5U, 0x4b10382bU, 0xfe8c37fbU);
    SCE->REG_1CH = 0x00400000U;
    SCE->REG_1D0H = 0x00000000U;
    if (1U == (SCE->REG_1CH_b.B22))
    {
        R_SCE_func102(0x13ff4c69U, 0x5080d3beU, 0xedf46dc6U, 0x20aa2bdeU);
        SCE->REG_1BCH = 0x00000040U;
        /* WAIT_LOOP */
        while (0U != SCE->REG_18H_b.B12)
        {
            /* waiting */
        }
        return FSP_ERR_CRYPTO_SCE_FAIL;
    }
    else
    {
        SCE->REG_104H = 0x00000068U;
        SCE->REG_E0H = 0x80010140U;
        /* WAIT_LOOP */
        while (1U != SCE->REG_104H_b.B31)
        {
            /* waiting */
        }
        SCE->REG_100H = InData_MACLength[0];
        SCE->REG_ECH = 0x38008940U;
        SCE->REG_ECH = 0x00000000U;
        SCE->REG_E0H = 0x00000080U;
        SCE->REG_1CH = 0x00260000U;
        SCE->REG_ECH = 0x0000b7e0U;
        SCE->REG_ECH = 0x00000010U;
        SCE->REG_ECH = 0x34202beaU;
        SCE->REG_E0H = 0x00000080U;
        SCE->REG_1CH = 0x00260000U;
        R_SCE_func100(0xc6fe6dc6U, 0xbb00a8f8U, 0x77d3af08U, 0x4e638313U);
        SCE->REG_1CH = 0x00400000U;
        SCE->REG_1D0H = 0x00000000U;
        if (1U == (SCE->REG_1CH_b.B22))
        {
            R_SCE_func102(0xcbe276a6U, 0xd20e41a2U, 0x9cb6afaaU, 0x394111e0U);
            SCE->REG_1BCH = 0x00000040U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_18H_b.B12)
            {
                /* waiting */
            }
            return FSP_ERR_CRYPTO_SCE_FAIL;
        }
        else
        {
            SCE->REG_104H = 0x00000068U;
            SCE->REG_E0H = 0x80010000U;
            /* WAIT_LOOP */
            while (1U != SCE->REG_104H_b.B31)
            {
                /* waiting */
            }
            SCE->REG_100H = InData_TextLen[0];
            SCE->REG_ECH = 0x00008c00U;
            SCE->REG_ECH = 0x0000000fU;
            SCE->REG_ECH = 0x38008800U;
            SCE->REG_ECH = 0x00000000U;
            SCE->REG_E0H = 0x00000080U;
            SCE->REG_1CH = 0x00A60000U;
            R_SCE_func100(0xff4080f7U, 0xa56c4ac4U, 0xca60dc12U, 0x2c29646cU);
            SCE->REG_1CH = 0x00400000U;
            SCE->REG_1D0H = 0x00000000U;
            if (1U == (SCE->REG_1CH_b.B22))
            {
                SCE->REG_104H = 0x00000361U;
                SCE->REG_A4H = 0x000007b5U;
                /* WAIT_LOOP */
                while (1U != SCE->REG_104H_b.B31)
                {
                    /* waiting */
                }
                SCE->REG_100H = InData_Text[0];
                SCE->REG_100H = InData_Text[1];
                SCE->REG_100H = InData_Text[2];
                SCE->REG_100H = InData_Text[3];
                SCE->REG_ECH = 0x0000b420U;
                SCE->REG_ECH = 0x00000080U;
                SCE->REG_E0H = 0x80840001U;
                SCE->REG_00H = 0x00008113U;
                /* WAIT_LOOP */
                while (0U != SCE->REG_00H_b.B25)
                {
                    /* waiting */
                }
                SCE->REG_1CH = 0x00001800U;
                SCE->REG_ECH = 0x00000821U;
                SCE->REG_ECH = 0x0000a400U;
                SCE->REG_ECH = 0x00000080U;
                SCE->REG_ECH = 0x0000b7e0U;
                SCE->REG_ECH = 0x00000080U;
                for (iLoop = 0; iLoop < 16; iLoop = iLoop+1)
                {
                    SCE->REG_ECH = 0x3c002be0U;
                    SCE->REG_ECH = 0x12003c3fU;
                    SCE->REG_ECH = 0x00002fe0U;
                }
                R_SCE_func100(0x2ea80017U, 0xf617ca64U, 0xf57f4b87U, 0x28d56d13U);
                SCE->REG_ECH = 0x0000b420U;
                SCE->REG_ECH = 0x00000080U;
                SCE->REG_E0H = 0x81840001U;
                SCE->REG_04H = 0x00000613U;
                /* WAIT_LOOP */
                while (1U != SCE->REG_04H_b.B30)
                {
                    /* waiting */
                }
                OutData_Text[0] = SCE->REG_100H;
                /* WAIT_LOOP */
                while (1U != SCE->REG_04H_b.B30)
                {
                    /* waiting */
                }
                OutData_Text[1] = SCE->REG_100H;
                /* WAIT_LOOP */
                while (1U != SCE->REG_04H_b.B30)
                {
                    /* waiting */
                }
                OutData_Text[2] = SCE->REG_100H;
                /* WAIT_LOOP */
                while (1U != SCE->REG_04H_b.B30)
                {
                    /* waiting */
                }
                OutData_Text[3] = SCE->REG_100H;
                SCE->REG_A4H = 0x00000e55U;
                SCE->REG_E0H = 0x81840001U;
                SCE->REG_00H = 0x00001813U;
                /* WAIT_LOOP */
                while (0U != SCE->REG_00H_b.B25)
                {
                    /* waiting */
                }
                SCE->REG_1CH = 0x00001800U;
                R_SCE_func101(0xd06a9940U, 0x3b2200f7U, 0x9b79925cU, 0xf866ebf7U);
            }
            else
            {
                R_SCE_func101(0x00ea433eU, 0x76012293U, 0x8bc71133U, 0x86d1a7f6U);
            }
            SCE->REG_104H = 0x00000051U;
            SCE->REG_A4H = 0x00000cc4U;
            /* WAIT_LOOP */
            while (1U != SCE->REG_104H_b.B31)
            {
                /* waiting */
            }
            SCE->REG_100H = change_endian_long(0x00000000U);
            SCE->REG_A4H = 0x010007b5U;
            SCE->REG_00H = 0x00001113U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_00H_b.B25)
            {
                /* waiting */
            }
            SCE->REG_1CH = 0x00001800U;
            SCE->REG_ECH = 0x0000b420U;
            SCE->REG_ECH = 0x00000080U;
            SCE->REG_E0H = 0x80840001U;
            SCE->REG_00H = 0x00008113U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_00H_b.B25)
            {
                /* waiting */
            }
            SCE->REG_1CH = 0x00001800U;
            SCE->REG_ECH = 0x0000a540U;
            SCE->REG_ECH = 0x00000080U;
            SCE->REG_ECH = 0x0000b7e0U;
            SCE->REG_ECH = 0x00000080U;
            SCE->REG_ECH = 0x00000821U;
            for (iLoop = 0; iLoop < 16; iLoop = iLoop+1)
            {
                SCE->REG_ECH = 0x3c002beaU;
                SCE->REG_ECH = 0x12003c3fU;
                SCE->REG_ECH = 0x00002fe0U;
            }
            SCE->REG_104H = 0x00000361U;
            SCE->REG_A4H = 0x00050805U;
            /* WAIT_LOOP */
            while (1U != SCE->REG_104H_b.B31)
            {
                /* waiting */
            }
            SCE->REG_100H = InData_MAC[0];
            SCE->REG_100H = InData_MAC[1];
            SCE->REG_100H = InData_MAC[2];
            SCE->REG_100H = InData_MAC[3];
            SCE->REG_A4H = 0x00900c45U;
            SCE->REG_ECH = 0x0000b420U;
            SCE->REG_ECH = 0x00000080U;
            SCE->REG_E0H = 0x81840001U;
            SCE->REG_00H = 0x00001813U;
            /* WAIT_LOOP */
            while (0U != SCE->REG_00H_b.B25)
            {
                /* waiting */
            }
            SCE->REG_1CH = 0x00001800U;
            SCE->REG_A4H = 0x00000000U;
            R_SCE_func100(0xce751d84U, 0x1832c7c0U, 0xc2978b79U, 0x825e4eaaU);
            SCE->REG_1CH = 0x00400000U;
            SCE->REG_1D0H = 0x00000000U;
            if (1U == (SCE->REG_1CH_b.B22))
            {
                R_SCE_func102(0x524ed7a8U, 0x0e41a8c4U, 0x4b410ad9U, 0x4b5eee94U);
                SCE->REG_1BCH = 0x00000040U;
                /* WAIT_LOOP */
                while (0U != SCE->REG_18H_b.B12)
                {
                    /* waiting */
                }
                return FSP_ERR_CRYPTO_SCE_FAIL;
            }
            else
            {
                R_SCE_func102(0x52c1de68U, 0xbbcba771U, 0x480e3e89U, 0x90ef5432U);
                SCE->REG_1BCH = 0x00000040U;
                /* WAIT_LOOP */
                while (0U != SCE->REG_18H_b.B12)
                {
                    /* waiting */
                }
                return FSP_SUCCESS;
            }
        }
    }
}

/***********************************************************************************************************************
End of function ./input_dir/S6C1/Cryptographic/R_SCE_p98f_r1.prc
***********************************************************************************************************************/
