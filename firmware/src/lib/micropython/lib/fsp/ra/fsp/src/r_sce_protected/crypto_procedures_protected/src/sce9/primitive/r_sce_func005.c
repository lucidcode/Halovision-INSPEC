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

void R_SCE_func005_r1(uint32_t ARG1)
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
    SCE->REG_ECH = 0x30003380U;
    SCE->REG_ECH = 0x00070020U;
    SCE->REG_ECH = 0x0000b7c0U;
    SCE->REG_ECH = 0x017c67d8U;
    SCE->REG_ECH = 0x00030040U;
    SCE->REG_ECH = 0x0000b7c0U;
    SCE->REG_ECH = 0x0126ddb5U;
    SCE->REG_ECH = 0x00050040U;
    SCE->REG_ECH = 0x0000b7c0U;
    SCE->REG_ECH = 0x01bcfa36U;
    SCE->REG_ECH = 0x00000080U;
    SCE->REG_28H = 0x00870001U;
    SCE->REG_C4H = 0x00443a0cU;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = change_endian_long(0x00000000U);
    SCE->REG_C4H = 0x000c3e1cU;
    SCE->REG_E0H = 0x810103c0U;
    SCE->REG_00H = 0x00002807U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    SCE->REG_104H = 0x00003f62U;
    SCE->REG_D0H = 0x00000f00U;
    SCE->REG_C4H = 0x02f087bfU;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+0 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+0 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+0 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+0 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+4 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+4 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+4 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+4 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x0000001aU;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+8 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+8 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+8 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+8 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+12 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+12 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+12 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+12 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x0000001bU;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+16 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+16 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+16 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+16 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+20 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+20 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+20 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+20 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000098U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+24 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+24 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+24 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+24 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+28 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+28 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+28 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+28 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000099U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+32 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+32 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+32 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+32 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+36 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+36 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+36 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+36 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000094U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+40 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+40 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+40 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+40 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+44 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+44 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+44 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+44 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000095U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+48 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+48 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+48 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+48 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+52 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+52 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+52 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+52 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x0000009cU;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+56 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+56 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+56 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+56 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+60 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+60 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+60 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+60 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x0000009dU;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    SCE->REG_104H = 0x00000362U;
    SCE->REG_C4H = 0x000007bdU;
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+64 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+64 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+64 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+64 + 3];
    SCE->REG_C4H = 0x00800c45U;
    SCE->REG_00H = 0x00002213U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
}

/***********************************************************************************************************************
End of function ./input_dir/S6C1/Cryptographic/R_SCE_func005_r1.prc
***********************************************************************************************************************/
