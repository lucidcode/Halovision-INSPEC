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
  ģ���Զ���USB�豸��CH372�豸�����̣�
 ���յ�������ȡ���ϴ���ʹ��PC�ϵ��������У��
 
 
*/

#include "ch32v30x_usbhs_device.h"
#include "debug.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

    /* USB20 device init */
    USBHS_RCC_Init( );                                                         /* USB2.0�����豸RCC��ʼ�� */
    USBHS_Device_Init( ENABLE );
    NVIC_EnableIRQ( USBHS_IRQn );

	while(1)
	{ }
}
