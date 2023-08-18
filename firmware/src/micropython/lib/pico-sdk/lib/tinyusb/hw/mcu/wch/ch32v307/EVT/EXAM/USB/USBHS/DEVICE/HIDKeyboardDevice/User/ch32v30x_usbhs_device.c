/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbhs_device.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : USB���ٲ�������ļ�
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_usbhs_device.h"

/******************************************************************************/
/* ������������ */

/* �������� */
void USBHS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/* USB���������� */
__attribute__ ((aligned(4))) UINT8 EP0_Databuf[ USBHS_UEP0_SIZE ]; /* �˵�0�����շ������� */
__attribute__ ((aligned(4))) UINT8 EP1_Rx_Databuf[ USBHS_MAX_PACK_SIZE ]; /* �˵�1���ݽ��ջ����� */
__attribute__ ((aligned(4))) UINT8 EP1_Tx_Databuf[ USBHS_MAX_PACK_SIZE ]; /* �˵�1���ݷ��ͻ����� */
__attribute__ ((aligned(4))) UINT8 EP2_Rx_Databuf[ USBHS_MAX_PACK_SIZE ]; /* �˵�2���ݽ��ջ����� */
__attribute__ ((aligned(4))) UINT8 EP2_Tx_Databuf[ USBHS_MAX_PACK_SIZE ]; /* �˵�2���ݷ��ͻ����� */

#define pMySetupReqPak        ((PUSB_SETUP_REQ)EP0_Databuf)
#define RepDescSize           62
const UINT8 *pDescr;
volatile UINT8  USBHS_Dev_SetupReqCode = 0xFF;                                  /* USB2.0�����豸Setup�������� */
volatile UINT16 USBHS_Dev_SetupReqLen = 0x00;                                   /* USB2.0�����豸Setup������ */
volatile UINT8  USBHS_Dev_SetupReqValueH = 0x00;                                /* USB2.0�����豸Setup��Value���ֽ� */
volatile UINT8  USBHS_Dev_Config = 0x00;                                        /* USB2.0�����豸����ֵ */
volatile UINT8  USBHS_Dev_Address = 0x00;                                       /* USB2.0�����豸��ֵַ */
volatile UINT8  USBHS_Dev_SleepStatus = 0x00;                                   /* USB2.0�����豸˯��״̬ */
volatile UINT8  USBHS_Dev_EnumStatus = 0x00;                                    /* USB2.0�����豸ö��״̬ */
volatile UINT8  USBHS_Dev_Endp0_Tog = 0x01;                                     /* USB2.0�����豸�˵�0ͬ����־ */
volatile UINT8  USBHS_Dev_Speed = 0x01;                                         /* USB2.0�����豸�ٶ� */
volatile UINT8  USBHS_Int_Flag  = 0x00;                                         /* USB2.0�����豸�жϱ�־ */

volatile UINT16 USBHS_Endp1_Up_Flag = 0x00;                                     /* USB2.0�����豸�˵�1�����ϴ�״̬: 0:����; 1:�����ϴ�; */
volatile UINT8  USBHS_Endp1_Down_Flag = 0x00;                                   /* USB2.0�����豸�˵�1�´��ɹ���־ */
volatile UINT8  USBHS_Endp1_Down_Len = 0x00;                                    /* USB2.0�����豸�˵�1�´����� */
volatile BOOL   USBHS_Endp1_T_Tog = 0;                                          /* USB2.0�����豸�˵�1����togλ��ת */
volatile BOOL   USBHS_Endp1_R_Tog = 0;

volatile UINT16 USBHS_Endp2_Up_Flag = 0x00;                                     /* USB2.0�����豸�˵�2�����ϴ�״̬: 0:����; 1:�����ϴ�; */
volatile UINT16 USBHS_Endp2_Up_LoadPtr = 0x00;                                  /* USB2.0�����豸�˵�2�����ϴ�װ��ƫ�� */
volatile UINT8  USBHS_Endp2_Down_Flag = 0x00;                                   /* USB2.0�����豸�˵�2�´��ɹ���־ */

/******************************************************************************/
/* USB�豸������ */
UINT8  MyDevDescr[18] =
{
    0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40,
    0x86, 0x1A, 0xE1, 0xE6,
    DEF_IC_PRG_VER2, DEF_IC_PRG_VER, 0x01, 0x02, 0x00, 0x01
};

/* USB����������(ȫ��) */
const UINT8  MyCfgDescr_FS[] =
{
    0x09, 0x02, 0x22, 0x00, 0x01, 0x01, 0x00, 0xA0, 0x32,/* Config Desc */
    0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,/* Interface Desc */
    0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x22, RepDescSize , 0x00,/* HID Desc */
    0x07, 0x05, 0x81, 0x03, (UINT8)DEF_USB_FS_EP_SIZE, (UINT8)( DEF_USB_FS_EP_SIZE >> 8 ), 0x01,
};

/* USB����������(����) */
const UINT8  MyCfgDescr_HS[ ] =
{
    0x09, 0x02, 0x22, 0x00, 0x01, 0x01, 0x00, 0xA0, 0x32,/* Config Desc */
    0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,/* Interface Desc */
    0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x22, RepDescSize, 0x00,/* HID Desc */
    0x07, 0x05, 0x81, 0x03, (UINT8)DEF_USB_HS_EP_SIZE, (UINT8)( DEF_USB_HS_EP_SIZE >> 8 ), 0x01,
};
/* USB���������� */
const UINT8  MyReportDesc[] =
{
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00, 0x25, 0x01,
    0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x03, 0x75, 0x01,
    0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91, 0x01, 0x95, 0x06,
    0x75, 0x08, 0x26, 0xFF, 0x00, 0x05, 0x07, 0x19, 0x00, 0x29, 0x91, 0x81, 0x00, 0xC0,
};
/* USB�����ַ��������� */
const UINT8  MyLangDescr[] =
{
    0x04,0x03,0x09,0x04
};

/* USB�����ַ��������� */
const UINT8  MyManuInfo[] =
{
    /* wch.cn */
    14,03,119,0,99,0,104,0,46,0,99,0,110,0
};

/* USB��Ʒ�ַ��������� */
const UINT8  MyProdInfo[] =
{
    /* WCH USB2.0 DEVICE */
    38,03,87,0,67,0,72,0,32,0,85,0,83,0,66,0,50,0,46,0,48,0,32,0,68,0,69,0,86,0,73,0,67,0,69,0,32,0
};

/* USB���к��ַ��������� */
const UINT8  MySerNumInfo[] =
{
    /* 0123456789 */
    22,03,48,0,49,0,50,0,51,0,52,0,53,0,54,0,55,0,56,0,57,0
};

/* USB�豸�޶������� */
const UINT8 MyUSBQUADesc[] =
{
    0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40, 0x01, 0x00,
};

/* BOS������ */
const UINT8 MyBOSDesc[] =
{
    0x05,0x0f,0x16,0x00,0x02,
    0x07,0x10,0x02,0x02,0x00,0x00,0x00,
    0x0a,0x10,0x03,0x00,0x0e,0x00,0x01,0x0a,0xff,0x07,
};
/* USBȫ��ģʽ,�����ٶ����������� */
UINT8 TAB_USB_FS_OSC_DESC[sizeof(MyCfgDescr_HS)] =
{
    0x09, 0x07,                                                                 /* ��������ͨ�������� */
};

/* USB����ģʽ,�����ٶ����������� */
UINT8 TAB_USB_HS_OSC_DESC[sizeof(MyCfgDescr_FS)] =
{
    0x09, 0x07,                                                                 /* ��������ͨ�������� */
};

/*********************************************************************
 * @fn      USBHS_RCC_Init
 *
 * @brief   Initializes the clock for USB2.0 High speed device.
 *
 * @return  none
 */
void USBHS_RCC_Init( void )
{
#if 0
    RCC->CFGR2 = USBHS_PLL_SRC_HSE | USBHS_PLL_SRC_PRE_DIV2 | USBHS_PLL_CKREF_4M; /* PLL REF = HSE/2 = 4MHz */
    RCC->CFGR2 |= USB_48M_CLK_SRC_PHY | USBHS_PLL_ALIVE;
    RCC->AHBPCENR |= ( (uint32_t)( 1 << 11 ) );
#else
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE );
    RCC_USBHSConfig( RCC_USBPLL_Div2 );
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M );
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY );
    RCC_USBHSPHYPLLALIVEcmd( ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE );
#endif
    Delay_Us( 200 );
}

/*********************************************************************
 * @fn      USBHS_Device_Endp_Init
 *
 * @brief   USB2.0�����豸�˵��ʼ��
 *
 * @return  none
 */
void USBHS_Device_Endp_Init ( void )
{
    /* ʹ�ܶ˵�1���˵�2���ͺͽ���  */
    USBHSD->ENDP_CONFIG = USBHS_EP0_T_EN | USBHS_EP0_R_EN | USBHS_EP1_T_EN | USBHS_EP2_T_EN | USBHS_EP1_R_EN | USBHS_EP2_R_EN;

    /* �˵��ͬ���˵� */
    USBHSD->ENDP_TYPE = 0x00;

    /* �˵㻺����ģʽ����˫��������ISO����BUFģʽ��Ҫָ��0  */
    USBHSD->BUF_MODE = 0x00;

    /* �˵���󳤶Ȱ����� */
    USBHSD->UEP0_MAX_LEN = 64;
    USBHSD->UEP1_MAX_LEN = 512;
    USBHSD->UEP2_MAX_LEN = 512;
    USBHSD->UEP3_MAX_LEN = 512;
    USBHSD->UEP4_MAX_LEN = 512;
    USBHSD->UEP5_MAX_LEN = 512;
    USBHSD->UEP6_MAX_LEN = 512;
    USBHSD->UEP7_MAX_LEN = 512;
    USBHSD->UEP8_MAX_LEN = 512;
    USBHSD->UEP9_MAX_LEN = 512;
    USBHSD->UEP10_MAX_LEN = 512;
    USBHSD->UEP11_MAX_LEN = 512;
    USBHSD->UEP12_MAX_LEN = 512;
    USBHSD->UEP13_MAX_LEN = 512;
    USBHSD->UEP14_MAX_LEN = 512;
    USBHSD->UEP15_MAX_LEN = 512;

    /* �˵�DMA��ַ���� */
    USBHSD->UEP0_DMA    = (UINT32)(UINT8 *)EP0_Databuf;
    USBHSD->UEP1_TX_DMA = (UINT32)(UINT8 *)EP1_Tx_Databuf;
    USBHSD->UEP1_RX_DMA = (UINT32)(UINT8 *)EP1_Rx_Databuf;
    USBHSD->UEP2_TX_DMA = (UINT32)(UINT8 *)EP2_Tx_Databuf;
    USBHSD->UEP2_RX_DMA = (UINT32)(UINT8 *)EP2_Rx_Databuf;

    /* �˵���ƼĴ������� */
    USBHSD->UEP0_TX_LEN  = 0;
    USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_NAK;
    USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;

    USBHSD->UEP1_TX_LEN  = 0;
    USBHSD->UEP1_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP1_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP2_TX_LEN  = 0;
    USBHSD->UEP2_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP2_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP3_TX_LEN  = 0;
    USBHSD->UEP3_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP3_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP4_TX_LEN  = 0;
    USBHSD->UEP4_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP4_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP5_TX_LEN  = 0;
    USBHSD->UEP5_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP5_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP6_TX_LEN  = 0;
    USBHSD->UEP6_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP6_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP7_TX_LEN  = 0;
    USBHSD->UEP7_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP7_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP8_TX_LEN  = 0;
    USBHSD->UEP8_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP8_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP9_TX_LEN  = 0;
    USBHSD->UEP9_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP9_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP10_TX_LEN  = 0;
    USBHSD->UEP10_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP10_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP11_TX_LEN  = 0;
    USBHSD->UEP11_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP11_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP12_TX_LEN  = 0;
    USBHSD->UEP12_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP12_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP13_TX_LEN  = 0;
    USBHSD->UEP13_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP13_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP14_TX_LEN  = 0;
    USBHSD->UEP14_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP14_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP15_TX_LEN  = 0;
    USBHSD->UEP15_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP15_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;
}

/*********************************************************************
 * @fn      USBHS_Device_Init
 *
 * @brief   USB2.0�����豸��ʼ��
 *
 * @return  none
 */
void USBHS_Device_Init ( FunctionalState sta )
{
    if( sta )
    {
        /* ����DMA���ٶȡ��˵�ʹ�ܵ� */
        USBHSD->HOST_CTRL = 0x00;
        USBHSD->HOST_CTRL = USBHS_SUSPENDM;

        USBHSD->CONTROL = 0;
#if IF_FULL_SPEED == 0
        USBHSD->CONTROL   = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_HIGH_SPEED;
#else
        USBHSD->CONTROL   = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_FULL_SPEED;
#endif

        USBHSD->INT_EN    = 0;
        USBHSD->INT_EN    = USBHS_SETUP_ACT_EN | USBHS_TRANSFER_EN | USBHS_DETECT_EN | USBHS_SUSPEND_EN;

        /* ALL endpoint enable */
        USBHSD->ENDP_CONFIG = 0xffffffff;

        /* USB2.0�����豸�˵��ʼ�� */
        USBHS_Device_Endp_Init( );
        Delay_Us(10);

        /* ʹ��USB���� */
        USBHSD->CONTROL |= USBHS_DEV_PU_EN;
    }
    else
    {
        USBHSD->CONTROL &= ~USBHS_DEV_PU_EN;
        USBHSD->CONTROL |= USBHS_ALL_CLR | USBHS_FORCE_RST;
    }
}

/*********************************************************************
 * @fn      Ep1_Tx
 *
 * @brief   USBHS �˵�1�ϴ�
 *
 * @return  none
 */
void Ep1_Tx( PUINT8 data, UINT8 len )
{
    memcpy( EP1_Tx_Databuf, data, len );
    USBHS_Endp1_Up_Flag = 1;
    USBHSD->UEP1_TX_LEN  = len;
    USBHSD->UEP1_TX_CTRL = ( USBHSD->UEP1_TX_CTRL & ~(USBHS_EP_T_RES_MASK|USBHS_EP_T_LEN_MASK|USBHS_EP_T_TOG_MASK) ) | USBHS_EP_T_RES_ACK;
    USBHSD->UEP1_TX_CTRL |= ( USBHS_Endp1_T_Tog ? USBHS_EP_T_TOG_1 : USBHS_EP_T_TOG_0 );
    printf( "tg%02x\n", USBHSD->UEP1_TX_CTRL&USBHS_EP_T_TOG_MASK );
    USBHS_Endp1_T_Tog ^= 1;
}

/*********************************************************************
 * @fn      USBHS_Device_SetAddress
 *
 * @brief   USB2.0�����豸�����豸��ַ
 *
 * @return  none
 */
void USBHS_Device_SetAddress( UINT32 address )
{
    USBHSD->DEV_AD = 0;
    USBHSD->DEV_AD = address & 0xff;
}

/*********************************************************************
 * @fn      USBHS_IRQHandler
 *
 * @brief   This function handles USBHS exception.
 *
 * @return  none
 */
void USBHS_IRQHandler( void )
{
    UINT32 end_num;
    UINT32 rx_token;
    UINT16 len = 0x00;
    UINT16 rx_len = 0;
    UINT16 i;
    UINT8  errflag = 0x00;
    UINT8  chtype;

    USBHS_Int_Flag = USBHSD->INT_FG;


    if( USBHS_Int_Flag & USBHS_TRANSFER_FLAG )
    {
        /* �˵㴫�䴦�� */
        end_num  = (USBHSD->INT_ST) & MASK_UIS_ENDP;
        rx_token = ( ( (USBHSD->INT_ST) & MASK_UIS_TOKEN ) >> 4 ) & 0x03;

#if 0
        if( !( USBHS->STATUS & TOG_MATCH ) )
        {
            DUG_PRINTF(" TOG MATCH FAIL : ENDP %x token %x \n", end_num, rx_token);
        }
#endif
        if( end_num == 0 )
        {
            /* �˵�0���� */
            if( rx_token == PID_IN )
            {
                /* �˵�0�ϴ��ɹ��ж� */
                switch( USBHS_Dev_SetupReqCode )
                {
                    case USB_GET_DESCRIPTOR:
                        len = USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE ? USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                        memcpy( EP0_Databuf, pDescr, len );
                        USBHS_Dev_SetupReqLen -= len;
                        pDescr += len;
                        USBHS_Dev_Endp0_Tog ^= 1;
                        USBHSD->UEP0_TX_LEN  = len;
                        USBHSD->UEP0_TX_CTRL =  USBHS_EP_T_RES_ACK | ( USBHS_Dev_Endp0_Tog ? USBHS_EP_T_TOG_0 : USBHS_EP_T_TOG_1 );
                        break;

                    case USB_SET_ADDRESS:
                        USBHS_Device_SetAddress( USBHS_Dev_Address );
                        USBHSD->UEP0_TX_LEN = 0;
                        USBHSD->UEP0_TX_CTRL = 0;
                        USBHSD->UEP0_RX_CTRL = 0;
                        break;

                    default:
                        /* ״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ��� */
                        USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK | USBHS_EP_R_TOG_1;
                        pDescr = NULL;
                        break;
                }
            }
            else if( rx_token == PID_OUT )
            {
                USBHSD->UEP0_TX_LEN  = 0;
                USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_ACK | USBHS_EP_T_TOG_1;
            }
        }
        else if( end_num == 1 )
        {
            if( rx_token == PID_IN )
            {
                USBHS_Endp1_Up_Flag = 0x00;
                /* Ĭ�ϻ�NAK */
                USBHSD->UEP1_TX_CTRL = ( USBHSD->UEP1_TX_CTRL & ~(USBHS_EP_T_RES_MASK|USBHS_EP_T_TOG_MASK) ) | USBHS_EP_T_RES_NAK | USBHS_EP_T_TOG_0;
            }
            else if( rx_token == PID_OUT )
            {

            }
        }
        else if( end_num == 2 )
        {
            if( rx_token == PID_IN )
            {

            }
            else if( rx_token == PID_OUT )
            {
                rx_len = USBHSD->RX_LEN;
                for( i=0; i<rx_len; i++)
                {
                    EP2_Tx_Databuf[i] = ~EP2_Rx_Databuf[i];
                }
                USBHSD->UEP1_TX_LEN  = rx_len;
                USBHSD->UEP1_TX_CTRL &= ~USBHS_EP_T_RES_MASK;
                USBHSD->UEP1_TX_CTRL |= USBHS_EP_T_RES_ACK;
                USBHSD->UEP1_RX_CTRL &= ~ USBHS_EP_R_RES_MASK;
                USBHSD->UEP1_RX_CTRL |= USBHS_EP_R_RES_ACK;
            }
        }
        USBHSD->INT_FG = USBHS_TRANSFER_FLAG;
    }
    else if( USBHS_Int_Flag & USBHS_SETUP_FLAG )
    {
     /* SETUP������ */
     USBHS_Dev_SetupReqLen = pMySetupReqPak->wLength;
     USBHS_Dev_SetupReqCode = pMySetupReqPak->bRequest;
     chtype = pMySetupReqPak->bRequestType;
     len = 0x00;
     errflag = 0x00;
    #if 1
     /* ��ӡ��ǰUsbsetup����  */
     printf( "%02X ", EP0_Databuf[0] );
     printf( "%02X ", EP0_Databuf[1] );
     printf( "%02X ", EP0_Databuf[2] );
     printf( "%02X ", EP0_Databuf[3] );
     printf( "%02X ", EP0_Databuf[4] );
     printf( "%02X ", EP0_Databuf[5] );
     printf( "%d\n",  (UINT16)EP0_Databuf[6]+(UINT16)(EP0_Databuf[7]<<8) );
    #endif
     /* �жϵ�ǰ�Ǳ�׼�������������� */
     if( ( pMySetupReqPak->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
     {
         /* ��������,��������,��������� */
         if( pMySetupReqPak->bRequestType & 0x40 )
         {
             /* �������� */
             switch( USBHS_Dev_SetupReqCode )
             {
                 default:
                     errflag = 0xFF;/* ����ʧ�� */
                     break;
             }
         }
         else if( pMySetupReqPak->bRequestType & 0x20 )
         {
             /* HID������ */
             switch( USBHS_Dev_SetupReqCode )
             {
             case 0x01: //GetReport
                 break;
             case 0x02: //GetIdle
                 break;
             case 0x03: //GetProtocol
                 break;
             case 0x09: //SetReport
                 break;
             case 0x0A: //SetIdle
                 break;
             case 0x0B: //SetProtocol
                 break;
             default:
                 errflag = 0xFF;
                 break;
             }
         }

         /* �ж��Ƿ������������ */
         if( errflag != 0xFF )
         {
             if( USBHS_Dev_SetupReqLen > len )
             {
                 USBHS_Dev_SetupReqLen = len;
             }
             len = ( USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE ) ? USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
             memcpy( EP0_Databuf, pDescr, len );
             pDescr += len;
         }
     }
     else
     {
         /* �����׼USB����� */
         switch( USBHS_Dev_SetupReqCode )
         {
             case USB_GET_DESCRIPTOR:
             {
                 switch( ( ( pMySetupReqPak->wValue ) >> 8 ) )
                 {
                     case USB_DESCR_TYP_DEVICE:
                         /* ��ȡ�豸������ */
                         pDescr = MyDevDescr;
                         len = MyDevDescr[ 0 ];
                         break;

                     case USB_DESCR_TYP_CONFIG:
                         /* ��ȡ���������� */
                         pDescr = MyCfgDescr_HS;
                         len = MyCfgDescr_HS[ 2 ] | ( (UINT16)MyCfgDescr_HS[ 3 ] << 8 );
                         break;

                     case USB_DESCR_TYP_STRING:
                         /* ��ȡ�ַ��������� */
                         switch( ( pMySetupReqPak->wValue ) & 0xff )
                         {
                             case 0:
                                 /* �����ַ��������� */
                                 pDescr = MyLangDescr;
                                 len = MyLangDescr[ 0 ];
                                 break;

                             case 1:
                                 /* USB�����ַ��������� */
                                 pDescr = MyManuInfo;
                                 len = sizeof( MyManuInfo );
                                 break;

                             case 2:
                                 /* USB��Ʒ�ַ��������� */
                                 pDescr = MyProdInfo;
                                 len = sizeof( MyProdInfo );
                                 break;

                             case 3:
                                 /* USB���к��ַ��������� */
                                 pDescr = MySerNumInfo;
                                 len = sizeof( MySerNumInfo );
                                 break;

                             default:
                                 errflag = 0xFF;
                                 break;
                         }
                         break;

                     case USB_DESCR_TYP_REPORT:
                         /* USB�豸���������� */
                         pDescr = MyReportDesc;
                         len = sizeof( MyReportDesc );
                         break;

                     case USB_DESCR_TYP_QUALIF:
                         /* �豸�޶������� */
                         pDescr = ( PUINT8 )&MyUSBQUADesc[ 0 ];
                         len = sizeof( MyUSBQUADesc );
                         break;

                     case USB_DESCR_TYP_SPEED:
                         /* �����ٶ����������� */
                         if( USBHS_Dev_Speed == 0x01 )
                         {
                             /* ����ģʽ */
                             memcpy( &TAB_USB_HS_OSC_DESC[ 2 ], &MyCfgDescr_FS[ 2 ], sizeof( MyCfgDescr_FS ) - 2 );
                             pDescr = ( PUINT8 )&TAB_USB_HS_OSC_DESC[ 0 ];
                             len = sizeof( TAB_USB_HS_OSC_DESC );
                         }
                         else if( USBHS_Dev_Speed == 0x00 )
                         {
                             /* ȫ��ģʽ */
                             memcpy( &TAB_USB_FS_OSC_DESC[ 2 ], &MyCfgDescr_HS[ 2 ], sizeof( MyCfgDescr_HS ) - 2 );
                             pDescr = ( PUINT8 )&TAB_USB_FS_OSC_DESC[ 0 ];
                             len = sizeof( TAB_USB_FS_OSC_DESC );
                         }
                         else
                         {
                             errflag = 0xFF;
                         }
                         break;

                     case USB_DESCR_TYP_BOS:
                         /* BOS������ */
                         /* USB2.0�豸��֧��BOS������ */
                         errflag = 0xFF;
                         break;
                     default :
                         errflag = 0xFF;
                         break;
                 }

                 /* �ж��Ƿ������������ */
                 if( errflag != 0xFF )
                 {
                     if( USBHS_Dev_SetupReqLen > len )
                     {
                         USBHS_Dev_SetupReqLen = len;
                     }
                     len = ( USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE ) ? USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                     memcpy( EP0_Databuf, pDescr, len );
                     pDescr += len;
                 }
             }
             break;

             case USB_SET_ADDRESS:
                 /* ���õ�ַ */
                 USBHS_Dev_Address = ( pMySetupReqPak->wValue )& 0xff;
                 break;

             case USB_GET_CONFIGURATION:
                 /* ��ȡ����ֵ */
                 EP0_Databuf[ 0 ] = USBHS_Dev_Config;
                 if( USBHS_Dev_SetupReqLen > 1 )
                 {
                     USBHS_Dev_SetupReqLen = 1;
                 }
                 break;

             case USB_SET_CONFIGURATION:
                 /* ��������ֵ */
                 USBHS_Dev_Config = ( pMySetupReqPak->wValue ) & 0xff;
                 USBHS_Dev_EnumStatus = 0x01;
                 break;

             case USB_CLEAR_FEATURE:
                 /* ������� */
                 if( ( pMySetupReqPak->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                 {
                     /* ����˵� */
                     switch( ( pMySetupReqPak->wIndex ) & 0xff )/* wIndexL */
                     {
                         case 0x82:
                             /* SET Endp2 Tx to USBHS_EP_T_RES_NAK;USBHS_EP_T_TOG_0;len = 0 */
                             USBHSD->UEP2_TX_LEN = 0;
                             USBHSD->UEP2_TX_CTRL = USBHS_EP_T_RES_NAK | USBHS_EP_T_TOG_0;
                             break;

                         case 0x02:
                             /* SET Endp2 Rx to USBHS_EP_R_RES_ACK;USBHS_EP_R_TOG_0 */
                             USBHSD->UEP2_TX_CTRL = USBHS_EP_R_RES_ACK | USBHS_EP_R_TOG_0;
                             break;

                         case 0x81:
                             /* SET Endp1 Tx to USBHS_EP_T_RES_NAK;USBHS_EP_T_TOG_0;len = 0 */
                             USBHSD->UEP1_TX_LEN = 0;
                             USBHSD->UEP1_TX_CTRL = USBHS_EP_T_RES_NAK | USBHS_EP_T_TOG_0;
                             break;

                         case 0x01:
                             /* SET Endp1 Rx to USBHS_EP_R_RES_NAK;USBHS_EP_R_TOG_0 */
                             USBHSD->UEP1_RX_CTRL = USBHS_EP_R_RES_ACK | USBHS_EP_R_TOG_0;
                             break;

                         default:
                             errflag = 0xFF;
                             break;
                     }
                 }
                 else if( ( pMySetupReqPak->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                 {
                     if( ( ( pMySetupReqPak->wValue ) & 0xff ) == 1 )/* wIndexL */
                     {
                         USBHS_Dev_SleepStatus &= ~0x01;
                     }
                 }
                 else
                 {
                     errflag = 0xFF;
                 }
                 break;

             case USB_SET_FEATURE:
                 /* �������� */
                 if( ( pMySetupReqPak->bRequestType & 0x1F ) == 0x00 )
                 {
                     /* �����豸 */
                     if( pMySetupReqPak->wValue == 0x01 )
                     {
                         if( MyCfgDescr_HS[ 7 ] & 0x20 )
                         {
                             /* ���û���ʹ�ܱ�־ */
                             USBHS_Dev_SleepStatus = 0x01;
                         }
                         else
                         {
                             errflag = 0xFF;
                         }
                     }
                     else
                     {
                         errflag = 0xFF;
                     }
                 }
                 else if( ( pMySetupReqPak->bRequestType & 0x1F ) == 0x02 )
                 {
                     /* ���ö˵� */
                     if( pMySetupReqPak->wValue == 0x00 )
                     {
                         /* ����ָ���˵�STALL */
                         switch( ( pMySetupReqPak->wIndex ) & 0xff )
                         {
                             case 0x82:
                                 /* ���ö˵�2 IN STALL */
                                 USBHSD->UEP2_TX_CTRL = ( USBHSD->UEP2_TX_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                 break;

                             case 0x02:
                                 /* ���ö˵�2 OUT Stall */
                                 USBHSD->UEP2_RX_CTRL = ( USBHSD->UEP2_RX_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
                                 break;

                             case 0x81:
                                 /* ���ö˵�1 IN STALL */
                                 USBHSD->UEP1_TX_CTRL = ( USBHSD->UEP1_TX_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                 break;

                             case 0x01:
                                 /* ���ö˵�1 OUT STALL */
                                 USBHSD->UEP1_RX_CTRL = ( USBHSD->UEP1_RX_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
                                 break;

                             default:
                                 errflag = 0xFF;
                                 break;
                         }
                     }
                     else
                     {
                         errflag = 0xFF;
                     }
                 }
                 else
                 {
                     errflag = 0xFF;
                 }
                 break;

             case USB_GET_INTERFACE:
                 EP0_Databuf[ 0 ] = 0x00;
                 if( USBHS_Dev_SetupReqLen > 1 )
                 {
                     USBHS_Dev_SetupReqLen = 1;
                 }
                 break;

             case USB_SET_INTERFACE:
                 EP0_Databuf[ 0 ] = 0x00;
                 if( USBHS_Dev_SetupReqLen > 1 )
                 {
                     USBHS_Dev_SetupReqLen = 1;
                 }
                 break;

             case USB_GET_STATUS:
                 /* ���ݵ�ǰ�˵�ʵ��״̬����Ӧ�� */
                 EP0_Databuf[ 0 ] = 0x00;
                 EP0_Databuf[ 1 ] = 0x00;
                 if( pMySetupReqPak->wIndex == 0x81 )
                 {
                     if( ( USBHSD->UEP1_TX_CTRL & USBHS_EP_T_RES_MASK ) == USBHS_EP_T_RES_STALL )
                     {
                         EP0_Databuf[ 0 ] = 0x01;
                     }
                 }
                 else if( pMySetupReqPak->wIndex == 0x01 )
                 {
                     if( ( USBHSD->UEP1_RX_CTRL & USBHS_EP_R_RES_MASK ) == USBHS_EP_R_RES_STALL )
                     {
                         EP0_Databuf[ 0 ] = 0x01;
                     }
                 }
                 else if( pMySetupReqPak->wIndex == 0x82 )
                 {
                     if( ( USBHSD->UEP2_TX_CTRL & USBHS_EP_T_RES_MASK ) == USBHS_EP_T_RES_STALL )
                     {
                         EP0_Databuf[ 0 ] = 0x01;
                     }
                 }
                 else if( pMySetupReqPak->wIndex == 0x02 )
                 {
                     if( ( USBHSD->UEP2_RX_CTRL & USBHS_EP_R_RES_MASK ) == USBHS_EP_R_RES_STALL )
                     {
                         EP0_Databuf[ 0 ] = 0x01;
                     }
                 }
                 if( USBHS_Dev_SetupReqLen > 2 )
                 {
                     USBHS_Dev_SetupReqLen = 2;
                 }
                 break;

             default:
                 errflag = 0xff;
                 break;
         }
     }

     /* �˵�0���� */
     if( errflag == 0xFF )
     {
         /* IN - STALL / OUT - DATA - STALL */
         USBHS_Dev_SetupReqCode = 0xFF;
         USBHSD->UEP0_TX_LEN  = 0;
         USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_STALL;
         USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_STALL;
     }
     else
     {
         /* DATA stage (IN -DATA1-ACK) */
         if( chtype & 0x80 )
         {
             len = ( USBHS_Dev_SetupReqLen> USBHS_UEP0_SIZE ) ? USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
             USBHS_Dev_SetupReqLen -= len;
         }
         else
         {
             len = 0;
         }
         USBHSD->UEP0_TX_LEN  = len;
         USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_ACK | USBHS_EP_T_TOG_1;
     }
     USBHSD->INT_FG = USBHS_SETUP_FLAG;
    }
    else if( USBHS_Int_Flag & USBHS_DETECT_FLAG )
    {
        /* USB���߸�λ�ж� */
#if 0
        printf("Rs\n");

#endif
        USBHS_Dev_Address = 0x00;
        USBHS_Device_Endp_Init( );                                              /* USB2.0�����豸�˵��ʼ�� */
        USBHS_Device_SetAddress( USBHS_Dev_Address );                           /* USB2.0�����豸�����豸��ַ */
        USBHSD->INT_FG = USBHS_DETECT_FLAG;
    }
    else if( USBHS_Int_Flag & USBHS_SUSPEND_FLAG )
    {
        /* ���� */
#if 0
        printf("USB SUSPEND!!!\n");
#endif
        USBHS_Dev_SleepStatus &= ~0x02;
        USBHS_Dev_EnumStatus = 0x01;
        USBHSD->INT_FG = USBHS_SUSPEND_FLAG;
    }
}

