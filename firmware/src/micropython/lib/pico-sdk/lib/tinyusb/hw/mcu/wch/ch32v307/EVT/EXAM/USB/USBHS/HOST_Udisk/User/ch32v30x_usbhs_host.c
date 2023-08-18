/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbhs_host.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the USB firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#include "ch32v30x_usbhs_host.h"
#include "CHRV3UFI.h"


UINT8  FoundNewDev = 0;
UINT8  Endp0MaxSize = 0;
UINT8  UsbDevEndp0Size = 0;
UINT16 EndpnMaxSize = 0;
USBDEV_INFO  thisUsbDev;

PUINT8  pHOST_RX_RAM_Addr;
PUINT8  pHOST_TX_RAM_Addr;
/******************************** HOST DEVICE **********************************/
__attribute__ ((aligned(4))) const UINT8  GetDevDescrptor[]={USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, 0x12, 0x00};
__attribute__ ((aligned(4))) const UINT8  GetConfigDescrptor[]= {USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00};
__attribute__ ((aligned(4))) const UINT8  SetAddress[]={USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00};
__attribute__ ((aligned(4))) const UINT8  SetConfig[]={USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
__attribute__ ((aligned(4))) const UINT8  Clear_EndpStall[]={USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
__attribute__ ((aligned(4))) const UINT8  SetupSetUsbInterface[] = { USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
__attribute__ ((aligned(4))) const UINT8  SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*********************************************************************
 * @fn      USB20_RCC_Init
 *
 * @brief   USB RCC initialized
 *
 * @return  none
 */
void USB20_RCC_Init( void )
{
    RCC->CFGR2 = USBHS_PLL_SRC_HSE | USBHS_PLL_SRC_PRE_DIV2 | USBHS_PLL_CKREF_4M;//PLL REF = HSE/2 = 4MHz
    RCC->CFGR2 |= USBHS_CLK_SRC_PHY|USBHS_PLLALIVE;
    RCC->AHBPCENR |= RCC_AHBPeriph_USBHS;                  //USB clock enable
    Delay_Us(200);
    USBHSH->HOST_CTRL |= PHY_SUSPENDM;
    Delay_Us(5);
}

/*********************************************************************
 * @fn      USBHS_HostInit
 *
 * @brief   USB host mode initialized.
 *
 * @param   sta - ENABLE or DISABLE
 *
 * @return  none
 */
void USBHS_HostInit (FunctionalState sta)  // USBHS host initial
{
    if(sta==ENABLE)
    {
        USB20_RCC_Init();

        USBHSH->CONTROL =  INT_BUSY_EN | DMA_EN | HIGH_SPEED | HOST_MODE;
        USBHSH->HOST_CTRL = PHY_SUSPENDM | SEND_SOF_EN;
        USBHSH->HOST_EP_CONFIG = HOST_TX_EN | HOST_RX_EN ;                // send enable, receive enable
        USBHSH->HOST_RX_MAX_LEN = 512;                                    // receive max length
        USBHSH->HOST_RX_DMA = (UINT32)pHOST_RX_RAM_Addr;
        USBHSH->HOST_TX_DMA = (UINT32)pHOST_TX_RAM_Addr;
    }
    else
    {
        USBHSH->CONTROL = USB_FORCE_RST | USB_ALL_CLR;
    }
}

/*********************************************************************
 * @fn      SetBusReset
 *
 * @brief   Reset USB bus
 *
 * @return  none
 */
void  SetBusReset(void)
{
    USBHSH->HOST_CTRL |= SEND_BUS_RESET;                              //bus reset
    Delay_Ms(15);
    USBHSH->HOST_CTRL &= ~SEND_BUS_RESET;
    while( !(USBHSH->HOST_CTRL & UH_SOFT_FREE) );                     //wait bus idle;
    USBHSH->HOST_CTRL |= SEND_SOF_EN;                                 //sof enable
    if( (USBHSH->SPEED_TYPE & USBSPEED_MASK ) == 1 )   thisUsbDev.DeviceSpeed = USB_HIGH_SPEED;

}

/*********************************************************************
 * @fn      CopySetupReqPkg
 *
 * @brief   copy the contents of the buffer to send buffer.
 *
 * @param   pReqPkt - target buffer address
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
 * @fn      USBHostTransact
 *
 * @brief   General host transaction
 *
 * @param   endp_pid - PID of the transaction  and the number of endpoint
 *          toggle - sync  trigger bit
 *          timeout - value of timeout
 *
 * @return  Error state
 */
UINT8 USBHost_GeneralTransact(UINT8 endp_pid,UINT8 toggle,UINT32 timeout)  //general
{
    UINT8   TransRetry,r;
    UINT32  i;

    USBHSH->HOST_TX_CTRL = USBHSH->HOST_RX_CTRL = toggle;
    TransRetry = 0;
    do
    {
        USBHSH->HOST_EP_PID = endp_pid ;                                      //�����������Ͱ�������
        USBHSH->INT_FG = USBHS_ACT_FLAG;                                      //�巢������ж�
        for ( i = WAIT_USB_TOUT_200US; i != 0 && ((USBHSH->INT_FG) & USBHS_ACT_FLAG) == 0 ; i -- )//
        {
            Delay_Us( 5 );                                                      //�ȴ��������
        }
        USBHSH->HOST_EP_PID = 0x00;
        if ( (USBHSH->INT_FG & USBHS_ACT_FLAG) == 0 )  return( ERR_USB_UNKNOWN );

        if( USBHSH->INT_FG  & USBHS_DETECT_FLAG )                                             //��ǰ�豸���γ�
        {
            USBHSH->INT_FG = USBHS_DETECT_FLAG;
            Delay_Us(200);
            if( USBHSH->MIS_ST & USBHS_ATTCH )
            {
                if(USBHSH->HOST_CTRL & SEND_SOF_EN)    return ( ERR_USB_CONNECT );
            }
            else    return ( ERR_USB_DISCON );
        }
        if ( USBHSH->INT_FG  & USBHS_ACT_FLAG )                                                        //���ݰ�����ɹ�
        {
            r = USBHSH->INT_ST & USBHS_HOST_RES;
            if((endp_pid >> 4) == USB_PID_IN )
            {
                if ( (USBHSH->INT_ST & USBHS_TOGGLE_OK) )         return( ERR_SUCCESS );             //���ݰ�����ƥ��
            }
            else
            {
                if ( (r == USB_PID_ACK)||(r == USB_PID_NYET) )            return( ERR_SUCCESS );                //setup/out���ȴ��豸��ӦACK
            }
            if ( r == USB_PID_STALL )                    return( r | ERR_USB_TRANSFER );       //�豸��ӦSTALL

            if ( r == USB_PID_NAK )                              //�豸��ӦNAK ��ʱ
            {
                if ( timeout == 0 )                      return( r | ERR_USB_TRANSFER );
                if ( timeout < 0xFFFFFFFF ) timeout --;
                -- TransRetry;
            }
            else switch ( endp_pid >> 4  )
            {
                case USB_PID_SETUP:

                case USB_PID_OUT:
                    if ( r ) return( r | ERR_USB_TRANSFER );           //
                    break;
                case USB_PID_IN:                                        //2b
                    if ( (r == USB_PID_DATA0) || (r == USB_PID_DATA1) ){
                    }
                    else if ( r ) return( r | ERR_USB_TRANSFER );
                    break;
                default:
                    return( ERR_USB_UNKNOWN );
            }
        }
        else {
            USBHSH->INT_FG = 0x3F;
        }
        Delay_Us( 15 );
    } while ( ++ TransRetry < 3 );

    return( ERR_USB_TRANSFER );
}

/*********************************************************************
 * @fn      USBHostTransact
 *
 * @brief   Host transaction  adapted to CHRV3(file system)
 *
 * @param   endp_pid - PID of the transaction  and the number of endpoint
 *          toggle - sync  trigger bit
 *          timeout - value of timeout
 *
 * @return  Error state
 */
UINT8 USBHostTransact(UINT8 endp_pid,UINT8 toggle,UINT32 timeout)
{
    UINT8   TransRetry,r;
    UINT8   s;
    UINT32  i;

    USBHSH->HOST_RX_CTRL =  USBHSH->HOST_TX_CTRL = 0;
    if( toggle & 0x80 ){
        USBHSH->HOST_RX_CTRL = 1<<3;
    }
    if( toggle & 0x40 ){
        USBHSH->HOST_TX_CTRL = 1<<3;
    }
    TransRetry = 0;
    do
    {
        USBHSH->HOST_EP_PID = endp_pid ;                                      //�����������Ͱ�������
        USBHSH->INT_FG = USBHS_ACT_FLAG;                                      //�巢������ж�
        for ( i = WAIT_USB_TOUT_200US; i != 0 && ((USBHSH->INT_FG) & USBHS_ACT_FLAG) == 0 ; i -- )//
        {
            Delay_Us( 5 );                                                      //�ȴ��������
        }
        USBHSH->HOST_EP_PID = 0x00;
        if ( (USBHSH->INT_FG & USBHS_ACT_FLAG) == 0 )  return( ERR_USB_UNKNOWN );

        s  = USBHSH->INT_FG ;
        if( s & USBHS_DETECT_FLAG )                                             //��ǰ�豸���γ�
        {
            USBHSH->INT_FG = USBHS_DETECT_FLAG;
            Delay_Us(200);
            if( USBHSH->MIS_ST & USBHS_ATTCH )
            {
                    return ( ERR_USB_CONNECT );
            }
            else    return ( ERR_USB_DISCON );
        }
        else if ( s & USBHS_ACT_FLAG )                                                        //���ݰ�����ɹ�
        {
            r = USBHSH->INT_ST & USBHS_HOST_RES;

            if((endp_pid >> 4) == USB_PID_IN )
            {
                if ( (USBHSH->INT_ST & USBHS_TOGGLE_OK) )  return( ERR_SUCCESS );   //���ݰ�����ƥ��
            }
            else
            {
                if ( (r == USB_PID_ACK)||(r == USB_PID_NYET) )   return( ERR_SUCCESS );                    //setup/out���ȴ��豸��ӦACK
            }

            if ( r == USB_PID_STALL )                    return( r | ERR_USB_TRANSFER );       //�豸��ӦSTALL

            if ( r == USB_PID_NAK )                              //�豸��ӦNAK ��ʱ
            {
                if ( timeout == 0 )                      return( r | ERR_USB_TRANSFER );
                if ( timeout < 0xFFFF ) timeout --;
                -- TransRetry;
            }
            else switch ( endp_pid >> 4  )
            {
                case USB_PID_SETUP:

                case USB_PID_OUT:
                    if ( r ) return( r | ERR_USB_TRANSFER );           //
                    break;
                case USB_PID_IN:                                        //2b
                    if ( (r == USB_PID_DATA0) || (r == USB_PID_DATA1) ){
                    }
                    else if ( r ) return( r | ERR_USB_TRANSFER );
                    break;
                default:
                    return( ERR_USB_UNKNOWN );
            }
        }
        else {
            USBHSH->INT_FG = 0x3F;
        }
        Delay_Us( 15 );
    } while ( ++ TransRetry < 3 );

    return( ERR_USB_TRANSFER );
}

/*********************************************************************
 * @fn      HostCtrlTransfer
 *
 * @brief   Host control transfer.
 *
 * @param   databuf - Receiving or send data buffer.
 *          RetLen - Data length.
 *
 * @return  Error state
 */
UINT8 HostCtrlTransfer(PUINT8 databuf,PUINT8 len)
{
    UINT8   ret;
    UINT8   rxcnt,txcnt;
    UINT8   tog = 1;
    PUINT8  pBuf;
    UINT16  relen;
    PUINT8  pLen;
    UINT32  rxlen;

    pBuf = databuf;
    pLen = len;

    if( pLen )  *pLen = 0;
    Delay_Us( 200 );
    USBHSH->HOST_TX_LEN = 8;
    ret = USBHost_GeneralTransact( (USB_PID_SETUP<<4)|DEF_ENDP_0, 0, 2000000 );
    if(ret != ERR_SUCCESS)      return ( ret );                     //error

    USBHSH->HOST_RX_CTRL = USBHSH->HOST_TX_CTRL = UH_R_TOG_1  | UH_T_TOG_1 ;
    relen = pSetupReq->wLength;

    if(relen && pBuf)                                           //data stage
    {
       if( (pSetupReq->bRequestType) & USB_REQ_TYP_IN )            //device to host
       {
           while(relen)
           {
               if( thisUsbDev.DeviceSpeed != USB_HIGH_SPEED )
                   Delay_Us( 200 );
               ret = USBHost_GeneralTransact( (USB_PID_IN<<4)| DEF_ENDP_0, tog<<3, 2000000 );
               if(ret != ERR_SUCCESS)                return ( ret );
               tog ^=1;
               rxlen = (USBHSH->RX_LEN < relen) ? USBHSH->RX_LEN : relen;
               relen -= rxlen;
               if(pLen)  *pLen += rxlen;
               for ( rxcnt = 0; rxcnt != rxlen; rxcnt ++ )
               {
                  *pBuf = pHOST_RX_RAM_Addr[ rxcnt ];
                  pBuf ++;
               }
               if( ( USBHSH->RX_LEN == 0 ) || (USBHSH->RX_LEN & ( UsbDevEndp0Size - 1 )))  break;
            }
           USBHSH->HOST_TX_LEN = 0;
           tog = 1;
         }
       else
       {                                                           // host to device
          while(relen)
          {
              if( thisUsbDev.DeviceSpeed != USB_HIGH_SPEED )
                  Delay_Us( 200 );
               USBHSH->HOST_TX_LEN = (relen >= UsbDevEndp0Size)? UsbDevEndp0Size : relen;
               for ( txcnt = 0; txcnt != USBHSH->HOST_TX_LEN; txcnt ++ )
               {
                   pHOST_TX_RAM_Addr[ txcnt ] = *pBuf;
                   pBuf ++;
               }
               ret = USBHost_GeneralTransact((USB_PID_OUT<<4)|DEF_ENDP_0,  tog<<3,  2000000);
               if(ret != ERR_SUCCESS)               return  ( ret );
               tog ^=1;
               relen -= USBHSH->HOST_TX_LEN;
               if( pLen )  *pLen += USBHSH->HOST_TX_LEN;
          }
          tog = 1;
        }
    }
    if( thisUsbDev.DeviceSpeed != USB_HIGH_SPEED )
        Delay_Us( 200 );
    ret = USBHost_GeneralTransact( ( USBHSH->HOST_TX_LEN ? USB_PID_IN<<4|DEF_ENDP_0 : USB_PID_OUT<<4|DEF_ENDP_0),
            tog <<3 , 20000 );

    if(ret != ERR_SUCCESS)            return( ret );

    if ( USBHSH->HOST_TX_LEN == 0 )   return( ERR_SUCCESS );    //status stage is out, send a zero-length packet.

    if ( USBHSH->RX_LEN == 0 )        return( ERR_SUCCESS );    //status stage is in, a zero-length packet is returned indicating success.

    return ERR_USB_BUF_OVER;
}

/*********************************************************************
 * @fn      CtrlGetDevDescr
 *
 * @brief   Get device descrptor
 *
 * @return  Error state
 */
UINT8 CtrlGetDevDescr( UINT8 *Databuf )
{
    UINT8 ret;
    UINT8 len;
    UsbDevEndp0Size = 8;
    CopySetupReqPkg( GetDevDescrptor );
    ret = HostCtrlTransfer( Databuf,&len );
    if( ret != ERR_SUCCESS )                     return  ( ret );
    if( len < ((PUSB_SETUP_REQ)GetDevDescrptor)->wLength )           return  ERR_USB_BUF_OVER;

    UsbDevEndp0Size = ((PUSB_DEV_DESCR)Databuf)->bMaxPacketSize0;
    return  ERR_SUCCESS;
}

/*********************************************************************
 * @fn      CtrlGetConfigDescr
 *
 * @brief   Get configuration descriptor.
 *
 * @return  Error state
 */
UINT8 CtrlGetConfigDescr( UINT8 *Databuf )
{
    UINT8  ret;
    UINT8  len;
    UINT16 reallen;

    CopySetupReqPkg( GetConfigDescrptor );
    ret = HostCtrlTransfer( Databuf, &len );
    if(ret != ERR_SUCCESS)             return  ( ret );
    if(len < ( pSetupReq->wLength ) )  return  ERR_USB_BUF_OVER;

    reallen = ((PUSB_CFG_DESCR)Databuf)-> wTotalLength;             //����ȫ�������������ĳ���

    CopySetupReqPkg( GetConfigDescrptor );
    pSetupReq->wLength = reallen;
    ret = HostCtrlTransfer( (UINT8 *)Databuf, &len );              //��ȡȫ������������
    if( ret != ERR_SUCCESS )           return  ( ret );

    thisUsbDev.DeviceCongValue = ( (PUSB_CFG_DESCR)Databuf )-> bConfigurationValue;
    CHRV3vPacketSize = ((PUSB_CFG_DESCR_LONG)Databuf)->endp_descr[0].wMaxPacketSize;

    return  ERR_SUCCESS;
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
UINT8 CtrlSetAddress(UINT8 addr)
{
    UINT8 ret;

    CopySetupReqPkg( SetAddress );
    pSetupReq->wValue = addr;
    ret = HostCtrlTransfer( NULL, NULL );
    if(ret != ERR_SUCCESS)  return  ( ret );
    USBHS_CurrentAddr( addr );
    mDelaymS(5);
    return  ERR_SUCCESS;
}

/*********************************************************************
 * @fn      CtrlSetUsbConfig
 *
 * @brief   Set usb configration.
 *
 * @param   cfg_val - device configuration value
 *
 * @return  Error state
 */
UINT8 CtrlSetUsbConfig( UINT8 cfg_val)
{
    CopySetupReqPkg( SetConfig );
    pSetupReq->wValue = cfg_val;
    return( HostCtrlTransfer( NULL, NULL ));
}

/*********************************************************************
 * @fn      CtrlClearEndpStall
 *
 * @brief   clear endpoint stall status.
 *
 * @param   endp - number of endpoint
 *
 * @return  Error state
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
 * @brief   Set USB Interface configuration.
 *
 * @param   cfg - configuration value
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
 * @fn      HubGetPortStatus
 *
 * @brief   Check the port of hub,and return the port's status
 *
 * @param   HubPortIndex - index of the hub port index
 *
 * @return  Error state
 */
UINT8   HubGetPortStatus( UINT8 HubPortIndex )
{
    UINT8   s;
    UINT8  len;

    pSetupReq -> bRequestType = HUB_GET_PORT_STATUS;
    pSetupReq -> bRequest = HUB_GET_STATUS;
    pSetupReq -> wValue = 0x0000;
    pSetupReq -> wIndex = 0x0000|HubPortIndex;
    pSetupReq -> wLength = 0x0004;
    s = HostCtrlTransfer( RxBuffer, &len );         // ִ�п��ƴ���
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < 4 )
    {
        return( ERR_USB_BUF_OVER );                  // ���������ȴ���
    }

    return( ERR_SUCCESS );
}

/*********************************************************************
 * @fn      HubSetPortFeature
 *
 * @brief   set the port feature of hub
 *
 * @param   HubPortIndex - index of the hub port index
 *          FeatureSelt - feature selector
 *
 * @return  Error state
 */
UINT8   HubSetPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt )
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
 * @brief   clear the port feature of hub
 *
 * @param   HubPortIndex - index of the hub port index
 *          FeatureSelt - feature selector
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
 * @fn      USBOTG_HostEnum
 *
 * @brief   Host enumerated device.
 *
 * @return  Error state
 */
UINT8 USBHS_HostEnum( void )
{
  UINT8 ret;
  UINT8 dv_cls, if_cls;

  SetBusReset();
  Delay_Ms(5);
  ret = CtrlGetDevDescr( endpRxBuf );
  if( ret == ERR_SUCCESS )
  {
      dv_cls = ( (PUSB_DEV_DESCR)endpRxBuf ) -> bDeviceClass;
      ret = CtrlSetAddress( ((PUSB_SETUP_REQ)SetAddress)->wValue );
      if( ret == ERR_SUCCESS )
      {
          ret = CtrlGetConfigDescr( endpRxBuf );
          if_cls = ( (PUSB_CFG_DESCR_LONG)endpRxBuf )->itf_descr.bInterfaceClass;
          if ( (dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE))
          {
              ret = CtrlSetUsbConfig( thisUsbDev.DeviceCongValue );
              if( ret == ERR_SUCCESS ){
                  thisUsbDev.DeviceType = USB_DEV_CLASS_STORAGE;
                  return( ERR_SUCCESS );
              }
              else if ( (dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_PRINTER) &&
                         ((PUSB_CFG_DESCR_LONG)endpRxBuf)->itf_descr.bInterfaceSubClass == 0x01 )
              {
                  ret = CtrlSetUsbConfig( thisUsbDev.DeviceCongValue );
                  if ( ret == ERR_SUCCESS ) {
                      thisUsbDev.DeviceType = USB_DEV_CLASS_PRINTER;
                      return( ERR_SUCCESS );
                  }
              }
              else if ( (dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) &&
                          ((PUSB_CFG_DESCR_LONG)endpRxBuf)->itf_descr.bInterfaceSubClass <= 0x01 )
              {
                   if_cls = ( (PUSB_CFG_DESCR_LONG)endpRxBuf ) -> itf_descr.bInterfaceProtocol;
                   ret = CtrlSetUsbConfig( thisUsbDev.DeviceCongValue );
                   if ( ret == ERR_SUCCESS )
                   {
                      if ( if_cls == 1 ) {
                         thisUsbDev.DeviceType = DEV_TYPE_KEYBOARD;
                         return( ERR_SUCCESS );
                      }
                      else if ( if_cls == 2 ) {
                         thisUsbDev.DeviceType = DEV_TYPE_MOUSE;
                         return( ERR_SUCCESS );
                      }
                      ret = ERR_USB_UNSUPPORT;
                   }
             }
             else
             {
                 ret = CtrlSetUsbConfig( thisUsbDev.DeviceCongValue );
                 if ( ret == ERR_SUCCESS ){
                     thisUsbDev.DeviceType = DEV_TYPE_UNKNOW;
                     return( ERR_SUCCESS );
               }
             }
          }
      }
  }
  return( ret );
}

/*********************************************************************
 * @fn      USBHS_CurrentAddr
 *
 * @brief   Current device address.
 *
 * @param   address - Endpoint address.
 *
 * @return  none
 */
void USBHS_CurrentAddr( UINT8 address )
{
    USBHSH->DEV_AD = address;                  // SET ADDRESS
    thisUsbDev.DeviceAddress = address ;
}

/*********************************************************************
 * @fn      Anaylisys_Descr
 *
 * @brief   Descriptor analysis.
 *
 * @param   pusbdev - device information variable.
 *          pdesc - descriptor buffer to analyze
 *          l - length
 *
 * @return  none
 */
void Analysis_Descr(pUSBDEV_INFO pusbdev,PUINT8 pdesc, UINT16 l)
{
    UINT16 i;
    for( i=0; i<l; i++ )                                                //����������
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
                 EndpnMaxSize = ((UINT16)pdesc[i+5]<<8)|pdesc[i+4];     //ȡ�˵��С
                 pusbdev->DevEndp.InEndpMaxSize = EndpnMaxSize;
                 printf("In_endpmaxsize:%02x \n",EndpnMaxSize);
            }
            else
            {
                printf("endpOUT:%02x \n",pdesc[i+2]&0x0f);              //ȡout�˵��
                pusbdev->DevEndp.OutEndpNum = pdesc[i+2]&0x0f;
                pusbdev->DevEndp.OutEndpCount++;
                EndpnMaxSize =((UINT16)pdesc[i+5]<<8)|pdesc[i+4];        //ȡ�˵��С
                pusbdev->DevEndp.OutEndpMaxSize = EndpnMaxSize;
                printf("Out_endpmaxsize:%02x \n",EndpnMaxSize);
            }
        }
  }
}


