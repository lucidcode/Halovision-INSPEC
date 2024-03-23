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
 �͹��ģ�����ģʽ���̣�
 WKUP(PA0)
 ��������ʾ WFI �������ģʽ��WKUP(PA0)���������أ��˳�����ģʽ��
 ���Ѻ����λ��
 ע��Ϊ�����ܵĽ��͹��ģ����齫���õ�GPIO���ó�������������ģ����������ó����ģʽ��

*/

#include "debug.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    /* Ϊ���͹��ģ��轫���õ�GPIO���ó��������������� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                               RCC_APB2Periph_GPIOC,
                           ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    Delay_Ms(1000);
    Delay_Ms(1000);

    printf("Standby Mode Test\r\n");

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
    {
        printf("\r\n Standby wake up reset \r\n");
    }
    else
    {
        printf("\r\n Power on reset \r\n");
        PWR_WakeUpPinCmd(ENABLE);
        PWR_EnterSTANDBYMode();
    }

    printf("\r\n ########## \r\n");
    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}
