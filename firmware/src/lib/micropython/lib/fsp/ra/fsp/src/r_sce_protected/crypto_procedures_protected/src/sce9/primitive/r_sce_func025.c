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

void R_SCE_func025_r1(uint32_t ARG1)
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
    SCE->REG_ECH = 0x018473b6U;
    SCE->REG_ECH = 0x00030040U;
    SCE->REG_ECH = 0x0000b7c0U;
    SCE->REG_ECH = 0x01ab9622U;
    SCE->REG_ECH = 0x00050040U;
    SCE->REG_ECH = 0x0000b7c0U;
    SCE->REG_ECH = 0x0138e816U;
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
    SCE->REG_100H = S_FLASH2[ARG1+120 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+120 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+120 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+120 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+124 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+124 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+124 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+124 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000090U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    SCE->REG_34H = 0x00000104U;
    SCE->REG_24H = 0x800011c0U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_24H_b.B21)
    {
        /* waiting */
    }
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+128 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+128 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+128 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+128 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+132 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+132 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+132 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+132 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000018U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    SCE->REG_34H = 0x00000108U;
    SCE->REG_24H = 0x800012c0U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_24H_b.B21)
    {
        /* waiting */
    }
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+136 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+136 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+136 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+136 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+140 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+140 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+140 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+140 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000091U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    SCE->REG_34H = 0x00000304U;
    SCE->REG_24H = 0x800015c0U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_24H_b.B21)
    {
        /* waiting */
    }
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+144 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+144 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+144 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+144 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+148 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+148 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+148 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+148 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000019U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_00H_b.B25)
    {
        /* waiting */
    }
    SCE->REG_1CH = 0x00001800U;
    SCE->REG_34H = 0x00000308U;
    SCE->REG_24H = 0x800016c0U;
    /* WAIT_LOOP */
    while (0U != SCE->REG_24H_b.B21)
    {
        /* waiting */
    }
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+152 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+152 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+152 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+152 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+156 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+156 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+156 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+156 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000092U;
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
    SCE->REG_100H = S_FLASH2[ARG1+160 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+160 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+160 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+160 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+164 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+164 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+164 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+164 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x00000093U;
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
    SCE->REG_100H = S_FLASH2[ARG1+168 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+168 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+168 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+168 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+172 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+172 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+172 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+172 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x0000009aU;
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
    SCE->REG_100H = S_FLASH2[ARG1+176 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+176 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+176 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+176 + 3];
    /* WAIT_LOOP */
    while (1U != SCE->REG_104H_b.B31)
    {
        /* waiting */
    }
    SCE->REG_100H = S_FLASH2[ARG1+180 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+180 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+180 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+180 + 3];
    SCE->REG_00H = 0x00003223U;
    SCE->REG_2CH = 0x0000009bU;
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
    SCE->REG_100H = S_FLASH2[ARG1+184 + 0];
    SCE->REG_100H = S_FLASH2[ARG1+184 + 1];
    SCE->REG_100H = S_FLASH2[ARG1+184 + 2];
    SCE->REG_100H = S_FLASH2[ARG1+184 + 3];
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
End of function ./input_dir/S6C1/Cryptographic/R_SCE_func025_r1.prc
***********************************************************************************************************************/
