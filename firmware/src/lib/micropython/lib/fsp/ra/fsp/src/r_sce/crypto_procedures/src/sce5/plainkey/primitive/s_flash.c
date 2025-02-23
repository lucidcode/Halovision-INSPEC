/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : s_flash.c
 * Version      : 1.09
 * Description  : Key information file.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 31.03.2020 1.09     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_sce_if.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
uint32_t const S_FLASH[] =
{
#ifndef __ARMEB__   /* Little endian */
    0x92f41cea, 0x2309b379, 0xb7d5e7b4, 0x0e53336d,
    0xe8cf625a, 0x19e79b10, 0x9dc85a10, 0x4d197093,
    0xbb94e956, 0xe38beca2, 0x9ed529ec, 0xb40e4b25,
    0x7fff4a80, 0x37c2251a, 0x0819a70c, 0x03666eba,
    0x74c55cd1, 0x9a0bc71d, 0x9cdafc90, 0xdef69ba5,
    0xb5431194, 0x006473e9, 0xb1acfd4a, 0xfd7dc888,
    0xb9cdc199, 0x32bd21b0, 0x6dd06b62, 0x88a15abb,
    0x5f9e4d6d, 0xa680b579, 0x702a5be7, 0x7f05e917,
    0x67902610, 0x5ddd6b35, 0xba3443c1, 0x5daa8d40,
    0xbecae6a6, 0x62584c47, 0x89a99ac7, 0x4d76f91a,
    0x313af776, 0x91e9d697, 0x57af6a0a, 0xd4c7e3e7,
    0xd09c715f, 0x22e3f7f0, 0x7cac0867, 0xdf8bd4d9,
    0x04e246b3, 0x1d50adaa, 0xd7404f94, 0x60fd642f,
    0xc8f3b8a4, 0x0cc8d4ef, 0x8c81684b, 0x4480724a,
    0x5569ded3, 0x322b8e85, 0x6187bfc5, 0x47166c34,
    0x1b9cfd57, 0xa092fbf0, 0x7b152f0e, 0x187731c0,
    0x819460ae, 0xb195c4d5, 0xc89dce93, 0xb1f082c2,
    0xebe23482, 0xf7ba9c09, 0x0ed87573, 0x629450be,
    0x2679d43f, 0xeffad241, 0xb2a2f72a, 0x79f04a49,
    0x2ec41794, 0xaa2de611, 0x56019ccb, 0x9d05fefe,
    0x626a52aa, 0x65051b73, 0x3ff7b5b8, 0x4258f3b1,
    0xb2a6abac, 0xc766eedb, 0x685deafc, 0xe32e8e7f,
    0xe4cab55d, 0x266ada78, 0x596f5930, 0x06c09e9e,
    0x81a822b1, 0xe93f12b9, 0x7b0f6ad9, 0x26cea2f4,
    0x23b058e7, 0x8117b8d9, 0xa64088d0, 0xe07fd1fb,
    0x21024258, 0xb59ba1a2, 0xf6c40124, 0x8c74e284,
    0x2309cf83, 0xe2a0cb8b, 0xaf024e64, 0x796e29ed,
    0xe48c48a9, 0x67e9630b, 0x47cf8506, 0x54879af5,
    0xebb7e35b, 0xd16f2922, 0x810768ec, 0x96c94bf3,
    0xf82acd5e, 0x3e0e9a66, 0xdd76d024, 0x6b931381,
    0xe8f1075e, 0x602a4438, 0x37328b5b, 0x45d2343b,
    0x5ccab479, 0x9940f6c0, 0xe4b10d54, 0x8c508659,
#else               /* Big endian */
    0xea1cf492, 0x79b30923, 0xb4e7d5b7, 0x6d33530e,
    0x5a62cfe8, 0x109be719, 0x105ac89d, 0x9370194d,
    0x56e994bb, 0xa2ec8be3, 0xec29d59e, 0x254b0eb4,
    0x804aff7f, 0x1a25c237, 0x0ca71908, 0xba6e6603,
    0xd15cc574, 0x1dc70b9a, 0x90fcda9c, 0xa59bf6de,
    0x941143b5, 0xe9736400, 0x4afdacb1, 0x88c87dfd,
    0x99c1cdb9, 0xb021bd32, 0x626bd06d, 0xbb5aa188,
    0x6d4d9e5f, 0x79b580a6, 0xe75b2a70, 0x17e9057f,
    0x10269067, 0x356bdd5d, 0xc14334ba, 0x408daa5d,
    0xa6e6cabe, 0x474c5862, 0xc79aa989, 0x1af9764d,
    0x76f73a31, 0x97d6e991, 0x0a6aaf57, 0xe7e3c7d4,
    0x5f719cd0, 0xf0f7e322, 0x6708ac7c, 0xd9d48bdf,
    0xb346e204, 0xaaad501d, 0x944f40d7, 0x2f64fd60,
    0xa4b8f3c8, 0xefd4c80c, 0x4b68818c, 0x4a728044,
    0xd3de6955, 0x858e2b32, 0xc5bf8761, 0x346c1647,
    0x57fd9c1b, 0xf0fb92a0, 0x0e2f157b, 0xc0317718,
    0xae609481, 0xd5c495b1, 0x93ce9dc8, 0xc282f0b1,
    0x8234e2eb, 0x099cbaf7, 0x7375d80e, 0xbe509462,
    0x3fd47926, 0x41d2faef, 0x2af7a2b2, 0x494af079,
    0x9417c42e, 0x11e62daa, 0xcb9c0156, 0xfefe059d,
    0xaa526a62, 0x731b0565, 0xb8b5f73f, 0xb1f35842,
    0xacaba6b2, 0xdbee66c7, 0xfcea5d68, 0x7f8e2ee3,
    0x5db5cae4, 0x78da6a26, 0x30596f59, 0x9e9ec006,
    0xb122a881, 0xb9123fe9, 0xd96a0f7b, 0xf4a2ce26,
    0xe758b023, 0xd9b81781, 0xd08840a6, 0xfbd17fe0,
    0x58420221, 0xa2a19bb5, 0x2401c4f6, 0x84e2748c,
    0x83cf0923, 0x8bcba0e2, 0x644e02af, 0xed296e79,
    0xa9488ce4, 0x0b63e967, 0x0685cf47, 0xf59a8754,
    0x5be3b7eb, 0x22296fd1, 0xec680781, 0xf34bc996,
    0x5ecd2af8, 0x669a0e3e, 0x24d076dd, 0x8113936b,
    0x5e07f1e8, 0x38442a60, 0x5b8b3237, 0x3b34d245,
    0x79b4ca5c, 0xc0f64099, 0x540db1e4, 0x5986508c,
#endif  /* defined __ARMEB__ */
};

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

