/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 *@Note
 HSI��HSI/2��ΪPLL����ʱ�����̣�
 ��������ʾ��HSI��HSI/2��ΪPLL����ʱ�ӣ�ϵͳʱ����PLL��Ϊʱ��Դ��
 �ο�system_ch32v30x.c��system_ch32v30x.h��
 HSI��ΪPLL����ʱ�ӣ���ʾϵͳʱ��Ϊ 24MHz��48MHz��72MHz��
 HSI/2��ΪPLL����ʱ�ӣ���ʾϵͳʱ��Ϊ 24MHz��48MHz

*/
#include "debug.h"

#define HSI      0   //HSI
#define HSI_1_2  1   //HSI/2

#define PLL_Source   HSI
//#define PLL_Source   HSI_1_2

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n",SystemCoreClock);

    while(1);
}







