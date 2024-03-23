/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbotg_host.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : USBOTG full speed host operation function.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include <ch32vf30x_usbfs_host.h>


__attribute__ ((aligned(4))) const UINT8  SetupGetDevDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00 };

__attribute__ ((aligned(4))) const UINT8  SetupGetCfgDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00 };

__attribute__ ((aligned(4))) const UINT8  SetupSetUsbAddr[] = { USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00 };

__attribute__ ((aligned(4))) const UINT8  SetupSetUsbConfig[] = { USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

__attribute__ ((aligned(4))) const UINT8  SetupSetUsbInterface[] = { USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

__attribute__ ((aligned(4))) const UINT8  SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
__attribute__ ((aligned(16))) UINT8 endpRXbuff[1024];   //�����˵������շ�������
__attribute__ ((aligned(16))) UINT8 endpTXbuff[1024];   //�����˵������շ�������

#define pSetupReq    ((PUSB_SETUP_REQ)endpTXbuff)

UINT8 UsbDevEndp0Size = 64;
USBFS_HOST ThisUsbDev;
UINT8 FoundNewDev=0;

/*********************************************************************
 * @fn      USBOTG_RCC_Init
 *
 * @brief   Initializes the usbotg clock configuration.
 *
 * @return  none
 */
void OTG_RCC_Init(void)
{
#ifdef CH32V30x_D8C
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY );
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE );
    RCC_USBHSConfig( RCC_USBPLL_Div2 );
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M );
    RCC_USBHSPHYPLLALIVEcmd( ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE );

#else
    RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLCLK_Div3);;

#endif

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_OTG_FS, ENABLE );
}

/*********************************************************************
 * @fn      USBOTG_HostInit
 *
 * @brief   USB OTG host mode  initialized.
 *
 * @param   stus - ENABLE or DISABLE
 *
 * @return  none
 */
void USBOTG_HostInit(FunctionalState stus)
{
    if(stus==ENABLE)
    {
        OTG_RCC_Init();
        USBOTG_H_FS->BASE_CTRL = USBHD_UC_HOST_MODE;                  //host mode
        USBOTG_H_FS->HOST_CTRL = 0;
        USBOTG_H_FS->DEV_ADDR=0;
        USBOTG_H_FS->HOST_EP_MOD = USBHD_UH_EP_TX_EN | USBHD_UH_EP_RX_EN;      //host tx&rx enable

        USBOTG_H_FS->HOST_RX_DMA = (UINT32)&endpRXbuff[0];                  //host rx DMA address
        USBOTG_H_FS->HOST_TX_DMA = (UINT32)&endpTXbuff[0];                 //host tx DMA address

        USBOTG_H_FS->HOST_RX_CTRL=0;
        USBOTG_H_FS->HOST_TX_CTRL=0;
        USBOTG_H_FS->BASE_CTRL = USBHD_UC_HOST_MODE | USBHD_UC_INT_BUSY |USBHD_UC_DMA_EN;

        USBOTG_H_FS->INT_FG=0xff;

        DisableRootHubPort();
        USBOTG_H_FS->INT_EN = USBHD_UIE_TRANSFER | USBHD_UIE_DETECT;
        FoundNewDev = 0;
    }

}

/*********************************************************************
 * @fn      DisableRootHubPort
 *
 * @brief   disable root hub.
 *
 * @return  none
 */
void DisableRootHubPort(void)
{
#ifdef  FOR_ROOT_UDISK_ONLY
    CH103DiskStatus = DISK_DISCONNECT;

#endif

#ifndef DISK_BASE_BUF_LEN
    ThisUsbDev.DeviceStatus = ROOT_DEV_DISCONNECT;
    ThisUsbDev.DeviceAddress = 0x00;

#endif
}

/*********************************************************************
 * @fn      AnalyzeRootHub
 *
 * @brief   Analyze root hub state.
 *
 * @return  s - Error state
 */
UINT8 AnalyzeRootHub(void)
{
    UINT8   s;

    s = ERR_SUCCESS;

    if ( USBOTG_H_FS->MIS_ST & USBHD_UMS_DEV_ATTACH ) {

#ifdef DISK_BASE_BUF_LEN
        if ( CH103DiskStatus == DISK_DISCONNECT

#else
        if (ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT

#endif
                || ( USBOTG_H_FS->HOST_CTRL & USBHD_UH_PORT_EN ) == 0x00 ) {
                DisableRootHubPort( );

#ifdef DISK_BASE_BUF_LEN
                CH103DiskStatus = DISK_CONNECT;

#else
        ThisUsbDev.DeviceSpeed = USBOTG_H_FS->MIS_ST & USBHD_UMS_DM_LEVEL ? 0 : 1;
        ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;

#endif
        s = ERR_USB_CONNECT;
     }
  }

#ifdef DISK_BASE_BUF_LEN
  else if ( CH103DiskStatus >= DISK_CONNECT ) {

#else
  else if (ThisUsbDev.DeviceStatus >= ROOT_DEV_CONNECTED ) {

#endif
        DisableRootHubPort( );
        if ( s == ERR_SUCCESS ) s = ERR_USB_DISCON;
  }

    return( s );
}

/*********************************************************************
 * @fn      SetHostUsbAddr
 *
 * @brief   Set USB host address.
 *
 * @param   addr - set device address
 *
 * @return  none
 */
void SetHostUsbAddr( UINT8 addr )
{
    USBOTG_H_FS->DEV_ADDR = (USBOTG_H_FS->DEV_ADDR & USBHD_UDA_GP_BIT) | (addr & USBHD_USB_ADDR_MASK);
    ThisUsbDev.DeviceAddress = addr;
}

#ifndef FOR_ROOT_UDISK_ONLY
/*********************************************************************
 * @fn      SetUsbSpeed
 *
 * @brief   Set USB speed.
 *
 * @param   FullSpeed - USB speed.
 *
 * @return  none
 */
void SetUsbSpeed( UINT8 FullSpeed )
{
    if ( FullSpeed )
    {
        USBOTG_H_FS->BASE_CTRL &= ~ USBHD_UC_LOW_SPEED;
        USBOTG_H_FS->HOST_SETUP &= ~ USBHD_UH_PRE_PID_EN;
    }
    else
    {
        USBOTG_H_FS->BASE_CTRL |= USBHD_UC_LOW_SPEED;
    }
}
#endif

/*********************************************************************
 * @fn      ResetRootHubPort
 *
 * @brief   Reset root hub.
 *
 * @return  none
 */
void ResetRootHubPort( void )
{
    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
    SetHostUsbAddr( 0x00 );
    USBOTG_H_FS->HOST_CTRL &= ~USBHD_UH_PORT_EN;
    SetUsbSpeed( 1 );
    USBOTG_H_FS->HOST_CTRL = (USBOTG_H_FS->HOST_CTRL & ~USBHD_UH_LOW_SPEED) | USBHD_UH_BUS_RESET;
    Delay_Ms( 50 );
    USBOTG_H_FS->HOST_CTRL = USBOTG_H_FS->HOST_CTRL & ~ USBHD_UH_BUS_RESET;
    Delay_Us( 250 );
    USBOTG_H_FS->INT_FG = USBHD_UIF_DETECT;
}

/*********************************************************************
 * @fn      EnableRootHubPort
 *
 * @brief   Enable root hub.
 *
 * @return  Error state
 */
UINT8 EnableRootHubPort(void)
{
#ifdef DISK_BASE_BUF_LEN
  if ( CH103DiskStatus < DISK_CONNECT ) CH103DiskStatus = DISK_CONNECT;

#else
  if ( ThisUsbDev.DeviceStatus < ROOT_DEV_CONNECTED ) ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;

#endif
  if ( USBOTG_H_FS->MIS_ST & USBHD_UMS_DEV_ATTACH ) {
#ifndef DISK_BASE_BUF_LEN
    if ( ( USBOTG_H_FS->HOST_CTRL & USBHD_UH_PORT_EN ) == 0x00 ) {
        ThisUsbDev.DeviceSpeed = (USBOTG_H_FS->MIS_ST & USBHD_UMS_DM_LEVEL) ? 0 : 1;
            if ( ThisUsbDev.DeviceSpeed == 0 ) USBOTG_H_FS->HOST_CTRL |= USBHD_UH_LOW_SPEED;
    }
#endif
    USBOTG_H_FS->HOST_CTRL  |= USBHD_UH_PORT_EN;
    USBOTG_H_FS->HOST_SETUP |= USBHD_UH_SOF_EN;
    return( ERR_SUCCESS );
  }

  return( ERR_USB_DISCON );
}

/*********************************************************************
 * @fn      WaitUSB_Interrupt
 *
 * @brief   Wait USB Interrput.
 *
 * @return  Error state
 */
UINT8 WaitUSB_Interrupt( void )
{
    UINT16  i;

    for ( i = 30000; i != 0 && ((USBOTG_H_FS->INT_FG) & USBHD_UIF_TRANSFER) == 0; i -- ){;}
    return( ((USBOTG_H_FS->INT_FG) & USBHD_UIF_TRANSFER)  ? ERR_SUCCESS : ERR_USB_UNKNOWN );
}

/*********************************************************************
 * @fn      USBHostTransact
 *
 * @brief   USB host transport transaction.
 *
 * @param   endp_pid - endpoint(bit3:bit0) and PID(bit7:bit4).
 *          tog - bit3-data auto toggle. bit2-data toggle. bit0-synchronous transmission.
 *          timeout - timeout times.
 *
 * @return  Error state
 */
UINT8 USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT32 timeout )
{
    UINT8   TransRetry;
    UINT8   s, r;
    UINT32  i;

    USBOTG_H_FS->HOST_TX_CTRL = USBOTG_H_FS->HOST_RX_CTRL = tog;
    TransRetry = 0;

    do {
        USBOTG_H_FS->HOST_EP_PID = endp_pid;                       //�����������Ͱ�������
        USBOTG_H_FS->INT_FG = USBHD_UIF_TRANSFER;                  //�巢������ж�
        for ( i = WAIT_USB_TOUT_200US; i != 0 && ((USBOTG_H_FS->INT_FG) & USBHD_UIF_TRANSFER) == 0 ; i -- )//
        {
            Delay_Us( 1 );                                                      //�ȴ��������
        }

        USBOTG_H_FS->HOST_EP_PID = 0x00;
        if ( (USBOTG_H_FS->INT_FG & USBHD_UIF_TRANSFER) == 0 )
        {
            return( ERR_USB_UNKNOWN );
        }

        if( USBOTG_H_FS->INT_FG & USBHD_UIF_DETECT ) {                     //��ǰ�豸���γ�
            USBOTG_H_FS->INT_FG = USBHD_UIF_DETECT;
            s = AnalyzeRootHub( );

            if ( s == ERR_USB_CONNECT )             FoundNewDev = 1;

#ifdef DISK_BASE_BUF_LEN

            if ( CH103DiskStatus == DISK_DISCONNECT ) return( ERR_USB_DISCON );
            if ( CH103DiskStatus == DISK_CONNECT )    return( ERR_USB_CONNECT );

#else
            if ( ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT ) return( ERR_USB_DISCON );
            if ( ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED )  return( ERR_USB_CONNECT );
#endif
            Delay_Us( 200 );
        }

        if ( USBOTG_H_FS->INT_FG & USBHD_UIF_TRANSFER )                           //���ݰ�����ɹ�
        {
            if ( USBOTG_H_FS->INT_ST & USBHD_UIS_TOG_OK ) return( ERR_SUCCESS );  //���ݰ�����ƥ��
            r = USBOTG_H_FS->INT_ST & MASK_UIS_H_RES;

            if ( r == USB_PID_STALL ) return( r | ERR_USB_TRANSFER );              //�豸��ӦSTALL

            if ( r == USB_PID_NAK )
            {
                if ( timeout == 0 ) return( r | ERR_USB_TRANSFER );                 //�豸��ӦNAK ��ʱ
                if ( timeout < 0xFFFF ) timeout --;
                -- TransRetry;
            }
            else switch ( endp_pid >> 4 ) {
                case USB_PID_SETUP:

                case USB_PID_OUT:
                    if ( r ) return( r | ERR_USB_TRANSFER );          //
                    break;

                case USB_PID_IN:                                       //2b
                    if ( (r == USB_PID_DATA0) && (r == USB_PID_DATA1) )
                    {

                    }
                    else if ( r ) return( r | ERR_USB_TRANSFER );
                    break;
                default:
                    return( ERR_USB_UNKNOWN );
            }
        }
        else {
            USBOTG_H_FS->INT_FG = 0xFF;
        }
        Delay_Us( 15 );
    } while ( ++ TransRetry < 3 );                               //���Գ�������δ�ɹ�

    return( ERR_USB_TRANSFER );
}

/*********************************************************************
 * @fn      HostCtrlTransfer
 *
 * @brief   Host control transport.
 *
 * @param   DataBuf - Receive or send data buffer.
 *          RetLen - Data length.
 *
 * @return  Error state
 */
UINT8 HostCtrlTransfer( PUINT8 DataBuf, PUINT8 RetLen )
{
    UINT16  RemLen  = 0;
    UINT8   s, RxLen;
    PUINT8  pBuf;
    PUINT8   pLen;

    pBuf = DataBuf;
    pLen = RetLen;
    Delay_Us( 200 );
    if ( pLen ) *pLen = 0;

    USBOTG_H_FS->HOST_TX_LEN = sizeof( USB_SETUP_REQ );                    //setup�׶�
    s = USBHostTransact( USB_PID_SETUP << 4 | 0x00, 0x00, 200000 );
    if ( s != ERR_SUCCESS )
    {
         return( s );
    }
    USBOTG_H_FS->HOST_RX_CTRL = USBHD_UH_R_TOG ;
    USBOTG_H_FS->HOST_TX_CTRL = USBHD_UH_T_TOG ;                           //���÷��ͻ����Ԥ�ڵ����ư� DATA0��DATA1
    USBOTG_H_FS->HOST_TX_LEN = 0x01;
    RemLen = pSetupReq -> wLength;

    if ( RemLen && pBuf )                                                  //�ж��Ƿ������ݽ׶�
    {
        if ( pSetupReq -> bRequestType & USB_REQ_TYP_IN )                  //���ݽ׶�IN����
        {
            while ( RemLen )
            {
                Delay_Us( 200 );
                USBOTG_H_FS->HOST_RX_DMA = (UINT32)pBuf+*pLen;    //���ý���DMA��ַ
                s = USBHostTransact( USB_PID_IN << 4 | 0x00, USBOTG_H_FS->HOST_RX_CTRL, 200000 );
                if ( s != ERR_SUCCESS )     return( s );

                RxLen = USBOTG_H_FS->RX_LEN < RemLen ? USBOTG_H_FS->RX_LEN : RemLen;
                RemLen -= RxLen;
                if ( pLen ) *pLen += RxLen;
                USBOTG_H_FS->HOST_RX_CTRL ^= USBHD_UH_R_TOG;
                if ( (USBOTG_H_FS->RX_LEN == 0) || ( USBOTG_H_FS->RX_LEN & ( UsbDevEndp0Size - 1 ) ) ) break;  //�յ�0���Ȱ����߶̰�˵�����ݽ������
            }
            USBOTG_H_FS->HOST_TX_LEN = 0x00;
        }
        else
        {                                                              //���ݽ׶�OUT����
            while ( RemLen )
            {
                Delay_Us( 200 );
                USBOTG_H_FS->HOST_TX_DMA = (UINT32)pBuf + *pLen;                                     //���÷���DMA��ַ
                USBOTG_H_FS->HOST_TX_LEN = RemLen >= UsbDevEndp0Size ? UsbDevEndp0Size : RemLen;     //���÷��ͳ���
                s = USBHostTransact( USB_PID_OUT << 4 | 0x00, USBOTG_H_FS->HOST_TX_CTRL, 200000/20 ); //��ʼ����

                if ( s != ERR_SUCCESS )                return( s );
                RemLen -= USBOTG_H_FS->HOST_TX_LEN;
                USBOTG_H_FS->HOST_TX_CTRL ^= USBHD_UH_T_TOG;
                if ( pLen )       *pLen += USBOTG_H_FS->HOST_TX_LEN;
            }
        }
    }

    Delay_Us( 200 );
    s = USBHostTransact(  (USBOTG_H_FS->HOST_TX_LEN) ? (USB_PID_IN << 4 | 0x00): (USB_PID_OUT << 4 | 0x00) ,
            USBHD_UH_T_TOG | USBHD_UH_R_TOG, 200000/20 );   //״̬�׶�
    if ( s != ERR_SUCCESS )                      return( s );
    if ( USBOTG_H_FS->HOST_TX_LEN == 0 )         return( ERR_SUCCESS );
    if ( USBOTG_H_FS->RX_LEN == 0 )              return( ERR_SUCCESS );

    return( ERR_USB_BUF_OVER );
}

/*********************************************************************
 * @fn      CopySetupReqPkg
 *
 * @brief   Copy setup request package.
 *
 * @param   pReqPkt - setup request package address.
 *
 * @return  none
 */
void CopySetupReqPkg( const UINT8 *pReqPkt )
{
    UINT8 i;
    for ( i = 0; i != sizeof( USB_SETUP_REQ ); i ++ )
    {
        ((PUINT8)pSetupReq)[ i ] = *pReqPkt;
        pReqPkt++;
    }
}

/*********************************************************************
 * @fn      CtrlGetDeviceDescr
 *
 * @brief   Get device descrptor
 *
 * @param   DataBuf - Data buffer.
 *
 * @return  Error state
 */
UINT8 CtrlGetDeviceDescr( PUINT8 DataBuf )
{
    UINT8   s;
    UINT8  len;

    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
    CopySetupReqPkg( SetupGetDevDescr );
    pSetupReq->wLength = UsbDevEndp0Size;

    s = HostCtrlTransfer( DataBuf, &len );
    if ( s != ERR_SUCCESS )               return( s );
    if ( len < pSetupReq->wLength )       return( ERR_USB_BUF_OVER );
    UsbDevEndp0Size = ( (PUSB_DEV_DESCR)DataBuf ) -> bMaxPacketSize0;     //��¼���ƶ˵�������С

    return( ERR_SUCCESS );
}

/*********************************************************************
 * @fn      CtrlGetConfigDescr
 *
 * @brief   Get configration descrptor.
 *
 * @param   DataBuf - Data buffer.
 *
 * @return  Error state
 */
UINT8 CtrlGetConfigDescr( PUINT8 DataBuf )
{
    UINT8   s;
    UINT8  len;

    CopySetupReqPkg( SetupGetCfgDescr );
    s = HostCtrlTransfer( DataBuf, &len );
    if ( s != ERR_SUCCESS )              return( s );
    if ( len <  pSetupReq ->wLength )    return( ERR_USB_BUF_OVER );  //��ȡ����������ǰ4���ֽ�

    len = ( (PUSB_CFG_DESCR)DataBuf ) -> wTotalLength;
    CopySetupReqPkg( SetupGetCfgDescr );
    pSetupReq ->wLength = len;
    s = HostCtrlTransfer( DataBuf, &len );                                        //��ȡȫ������������
    if ( s != ERR_SUCCESS )                return( s );
    if( len < pSetupReq->wLength )         return( ERR_USB_BUF_OVER );

    Analysis_Descr(&ThisUsbDev, DataBuf, pSetupReq ->wLength);
    ThisUsbDev.DeviceCongValue = ((PUSB_CFG_DESCR)DataBuf)->bConfigurationValue;   //��¼�豸����ֵ
    return( ERR_SUCCESS );
}

/*********************************************************************
 * @fn      CtrlSetUsbAddress
 *
 * @brief   Set USB device address.
 *
 * @param   addr - Device address.
 *
 * @return  Error state
 */
UINT8 CtrlSetUsbAddress( UINT8 addr )
{
    UINT8   s;

    CopySetupReqPkg( SetupSetUsbAddr );
    pSetupReq -> wValue = addr;
    s = HostCtrlTransfer( NULL, NULL );
    if ( s != ERR_SUCCESS ) return( s );
    SetHostUsbAddr( addr );             //��������������ַ
    Delay_Ms( 10 );

    return( ERR_SUCCESS );
}

/*********************************************************************
 * @fn      CtrlSetUsbConfig
 *
 * @brief   Set usb configration.
 *
 * @param   cfg - Configration Value.
 *
 * @return  none
 */
UINT8 CtrlSetUsbConfig( UINT8 cfg )
{
    CopySetupReqPkg( SetupSetUsbConfig );
    pSetupReq -> wValue = cfg;
    return( HostCtrlTransfer( NULL, NULL ) );
}

/*********************************************************************
 * @fn      CtrlClearEndpStall
 *
 * @brief   Clear endpoint STALL.
 *
 * @param   endp - Endpoint address.
 *
 * @return  none
 */
UINT8 CtrlClearEndpStall( UINT8 endp )
{
    CopySetupReqPkg( SetupClrEndpStall );
    pSetupReq -> wIndex = endp;
    return( HostCtrlTransfer( NULL, NULL ) );
}

/*********************************************************************
 * @fn      CtrlSetUsbIntercace
 *
 * @brief   Set USB Interface configration.
 *
 * @param   cfg - Configration value.
 *
 * @return  Error state
 */
UINT8 CtrlSetUsbIntercace( UINT8 cfg )
{
    CopySetupReqPkg( SetupSetUsbInterface );
    pSetupReq -> wValue = cfg;
    return( HostCtrlTransfer( NULL, NULL ) );
}

/*********************************************************************
 * @fn      USBOTG_HostEnum
 *
 * @brief   host enumerated device.
 *
 * @return  Error state
 */
UINT8 USBOTG_HostEnum(UINT8 *buf)
{
    UINT8  i, s;

    ResetRootHubPort( );

    for ( i = 0, s = 0; i < 100; i ++ )
    {
        Delay_Ms( 1 );
        if ( EnableRootHubPort( ) == ERR_SUCCESS ) {
                i = 0;
                s ++;
                if ( s > 100 ) break;
        }
    }

    if ( i )
    {
        DisableRootHubPort( );
        return( ERR_USB_DISCON );
    }
    SetUsbSpeed( ThisUsbDev.DeviceSpeed );  //���õ�ǰ�����ٶ����豸һ��

    s = CtrlGetDeviceDescr( buf ); //��ȡ�豸������
    if(s !=ERR_SUCCESS)
    {
       printf("get device descriptor error\n");
    }

    s = CtrlSetUsbAddress( ((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValue );  //�����豸��ַ
    if(s !=ERR_SUCCESS)
    {
       printf("set address error\n");
    }

    s = CtrlGetConfigDescr( buf );             //��ȡ����������
    if(s !=ERR_SUCCESS)
    {
       printf("get configuration descriptor error\n");
    }

    s = CtrlSetUsbConfig( ThisUsbDev.DeviceCongValue );  //��������
    if(s !=ERR_SUCCESS)
    {
       printf("set configuration error\n");
    }

    return( s );
}

/*********************************************************************
 * @fn      HubSetPortFeature
 *
 * @brief   ����HUB�˿�����
 *
 * @param   HubPortIndex - HUB�˿�
 *          FeatureSelt - HUB�˿�����
 *
 * @return  Error state
 */
UINT8 HubSetPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt )
{
    pSetupReq -> bRequestType = HUB_SET_PORT_FEATURE;
    pSetupReq -> bRequest = HUB_SET_FEATURE;
    pSetupReq -> wValue = 0x0000|FeatureSelt;
    pSetupReq -> wIndex = 0x0000|HubPortIndex;
    pSetupReq -> wLength = 0x0000;
    return( HostCtrlTransfer( NULL, NULL ) );     // ִ�п��ƴ���
}

/*********************************************************************
 * @fn      HubClearPortFeature
 *
 * @brief   ���HUB�˿�����
 *
 * @param   HubPortIndex - HUB�˿�
 *          FeatureSelt - HUB�˿�����
 *
 * @return  Error state
 */
UINT8   HubClearPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt )
{
    pSetupReq -> bRequestType = HUB_CLEAR_PORT_FEATURE;
    pSetupReq -> bRequest = HUB_CLEAR_FEATURE;
    pSetupReq -> wValue = 0x0000|FeatureSelt;
    pSetupReq -> wIndex = 0x0000|HubPortIndex;
    pSetupReq -> wLength = 0x0000;
    return( HostCtrlTransfer( NULL, NULL ) );     // ִ�п��ƴ���
}

/*********************************************************************
 * @fn      Anaylisys_Descr
 *
 * @brief   Descriptor analysis.
 *
 * @param   pusbdev - host status variable.
 *          pdesc - descriptor buffer to analyze
 *          l - length
 *
 * @return  none
 */
void Analysis_Descr(PUSBFS_HOST pusbdev, PUINT8 pdesc, UINT16 l)
{
    UINT16 i,EndPMaxSize;
    for(i=0;i<l;i++)                                                //����������
    {
        if((pdesc[i]==0x09)&&(pdesc[i+1]==0x02))
        {
            printf("bNumInterfaces:%02x \n",pdesc[i+4]);            //������������Ľӿ���-��5���ֽ�
        }

        if((pdesc[i]==0x07)&&(pdesc[i+1]==0x05))
        {
            if((pdesc[i+2])&0x80)
            {
                 printf("endpIN:%02x \n",pdesc[i+2]&0x0f);              //ȡin�˵��
                 pusbdev->DevEndp.InEndpNum = pdesc[i+2]&0x0f;
                 pusbdev->DevEndp.InEndpCount++;
                 EndPMaxSize = ((UINT16)pdesc[i+5]<<8)|pdesc[i+4];     //ȡ�˵��С
                 pusbdev->DevEndp.InEndpMaxSize = EndPMaxSize;
                 printf("In_endpmaxsize:%02x \n",EndPMaxSize);
            }
            else
            {
                printf("endpOUT:%02x \n",pdesc[i+2]&0x0f);              //ȡout�˵��
                pusbdev->DevEndp.OutEndpNum = pdesc[i+2]&0x0f;
                pusbdev->DevEndp.OutEndpCount++;
                EndPMaxSize =((UINT16)pdesc[i+5]<<8)|pdesc[i+4];        //ȡ�˵��С
                pusbdev->DevEndp.OutEndpMaxSize = EndPMaxSize;
                printf("Out_endpmaxsize:%02x \n",EndPMaxSize);
            }
        }
    }
}
