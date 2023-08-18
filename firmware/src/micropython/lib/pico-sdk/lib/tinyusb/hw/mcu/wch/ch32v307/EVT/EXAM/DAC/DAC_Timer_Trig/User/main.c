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
 ��ʱ������DACת�����̣�
 DACͨ��0(PA4)���
 ͨ��TIM8_TRGO�¼�����1��DACת����PA4�����Ӧ�ĵ�ѹ��

*/

#include "debug.h"

/* Global define */
#define Num 7

/* Global Variable */ 
u16 DAC_Value[Num]={64,128,256,512,1024,2048,4095};   


/*********************************************************************
 * @fn      Dac_Init
 *
 * @brief   Initializes DAC collection.
 *
 * @return  none
 */
void Dac_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    DAC_InitTypeDef DAC_InitType={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_4);

    DAC_InitType.DAC_Trigger=DAC_Trigger_T8_TRGO;
    DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;
    DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;
    DAC_Init(DAC_Channel_1,&DAC_InitType);

    DAC_Cmd(DAC_Channel_1, ENABLE);

}

/*********************************************************************
 * @fn      TIM8_Init
 *
 * @brief   Initializes TIM3 collection.
 *
 * @param   arr - TIM_Period
 *          psc - TIM_Prescaler
 *
 * @return  none
 */
void TIM8_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM8, ENABLE );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;
    TIM_TimeBaseInit( TIM8, &TIM_TimeBaseInitStructure);

    TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
    TIM_Cmd(TIM8, ENABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    u8 i=0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);

    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("Timer Trig\r\n");

    TIM8_Init(0x7,48000-1);
    Dac_Init();

    while(1)
    {
        DAC->R12BDHR1=DAC_Value[i];
        i++;

        if(i>Num){
          i=0;
        }

        Delay_Ms(1000);
        printf("run\r\n");
        printf("DAC->R12BDHR1:0x%04x\r\n",DAC->R12BDHR1);
        printf("DAC->DOR1:0x%04x\r\n",DAC->DOR1);
        printf("TIM3->CNT:%d\r\n",TIM3->CNT);

    }
}






