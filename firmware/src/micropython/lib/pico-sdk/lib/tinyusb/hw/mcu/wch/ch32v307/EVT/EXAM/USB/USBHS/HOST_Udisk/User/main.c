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
  USBHS�豸�ļ���ö�ٹ������̣�
  USBHS_DM(PB6)��USBHS_DM(PB7)
  ��������ʾ U���ļ�ϵͳ,���ֽ�Ϊ��λ��д�ļ��������ļ�������ɾ�����޸��ļ�
  ���ԣ��޸��ļ�����
  ע���ļ�ϵͳ֧�� FAT12/FAT16/FAT32��

*/

#include "debug.h"
#include "ch32v30x_usbhs_host.h"
#include "string.h"
#include "CHRV3UFI.h"

__attribute__ ((aligned(4))) UINT8   RxBuffer[ MAX_PACKET_SIZE  ];  // IN, must even address
__attribute__ ((aligned(4))) UINT8   TxBuffer[ MAX_PACKET_SIZE  ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   endpTxBuf[ MAX_PACKET_SIZE ];  // OUT, must even address
__attribute__ ((aligned(4))) UINT8   endpRxBuf[ MAX_PACKET_SIZE ];  // OUT, must even addres
__attribute__ ((aligned(4))) UINT8   buf[ 128 ];  // OUT, must even address


/* ������״̬,�����������ʾ������벢ͣ�� */
void mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* �����ɹ� */
    }
    printf( "Error:%02x\r\n", iError );  /* ��ʾ���� */
    /* ���������,Ӧ�÷����������Լ�CHRV3DiskStatus״̬,�������CHRV3DiskReady��ѯ��ǰU���Ƿ�����,���U���ѶϿ���ô�����µȴ�U�̲����ٲ���,
         ��������Ĵ�����:
         1������һ��CHRV3DiskReady,�ɹ����������,����Open,Read/Write��
         2�����CHRV3DiskReady���ɹ�,��ôǿ�н���ͷ��ʼ���� */
    while(1)
    {  }
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
    UINT8 ret;
    UINT8 i,t;
    UINT16 TotalCount = 0;
	Delay_Init();
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	pHOST_TX_RAM_Addr = TxBuffer;
	pHOST_RX_RAM_Addr = RxBuffer;
	USBHS_HostInit(ENABLE);
    ret = CHRV3LibInit();

	while(1)
	{
        if( USBHSH->INT_FG & USBHS_DETECT_FLAG )
        {
            USBHSH->INT_FG = USBHS_DETECT_FLAG;
            if( USBHSH->MIS_ST & USBHS_ATTCH )
            {
                ret = USBHS_HostEnum();
                if( ret == ERR_SUCCESS )
                {
                    printf("enumerate success\n");
                    Delay_Ms(100);
                    // U�̲������̣�USB���߸�λ��U�����ӡ���ȡ�豸������������USB��ַ����ѡ�Ļ�ȡ������������֮�󵽴�˴�����CH103�ӳ���������ɺ�������
                    CHRV3DiskStatus = DISK_USB_ADDR;
                    for ( i = 0; i != 10; i ++ )
                    {
                        printf( "Wait DiskReady\r\n" );
                        ret = CHRV3DiskReady( );                                 //�ȴ�U��׼����
                        if ( ret == ERR_SUCCESS )
                        {
//                            printf("CHRV3DiskStatus:%02x\n",CHRV3DiskStatus);
                            break;
                        }
                        else
                        {
                            printf("%02x\r\n",ret);
                            printf("CHRV3DiskStatus:%02x\n",CHRV3DiskStatus);
                        }
                        Delay_Ms( 50 );
                    }

                  if ( CHRV3DiskStatus >= DISK_MOUNTED )
                    {
                        /* ���ļ� */
                        strcpy( (PCHAR)mCmdParam.Open.mPathName, "/C51/NEWFILE.C" );     //���ý�Ҫ�������ļ�·�����ļ���/C51/NEWFILE.C
                        ret = CHRV3FileOpen( );                                       //���ļ�
                        if ( ret == ERR_MISS_DIR || ret == ERR_MISS_FILE )            //û���ҵ��ļ�
                        {
                            //�����ļ���ʾ
                            printf( "Find No File And Create\r\n" );
                            strcpy( (PCHAR)mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* ���ļ���,�ڸ�Ŀ¼��,�����ļ��� */
                            ret = CHRV3FileCreate( );                                        /* �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */
                            mStopIfError( ret );
                            printf( "ByteWrite\n" );
                            //ʵ��Ӧ���ж�д���ݳ��ȺͶ��建���������Ƿ������������ڻ�������������Ҫ���д��
                            i = sprintf( (PCHAR)buf,"Note: \xd\xa������������ֽ�Ϊ��λ����U���ļ���д,����ʾ���ܡ�\xd\xa");  /*��ʾ */
                            for(t=0; t<10; t++)
                            {
                                mCmdParam.ByteWrite.mByteCount = i;                          /* ָ������д����ֽ��� */
                                mCmdParam.ByteWrite.mByteBuffer = buf;                       /* ָ�򻺳��� */
                                ret = CHRV3ByteWrite( );                                       /* ���ֽ�Ϊ��λ���ļ�д������ */
                                mStopIfError( ret );
                                printf("�ɹ�д�� %02X��\r\n",(UINT16)t);
                            }
                            //��ʾ�޸��ļ�����
                            printf( "Modify\r\n" );
                            mCmdParam.Modify.mFileAttr = 0xff;   //�������: �µ��ļ�����,Ϊ0FFH���޸�
                            mCmdParam.Modify.mFileTime = 0xffff;   //�������: �µ��ļ�ʱ��,Ϊ0FFFFH���޸�,ʹ���½��ļ�������Ĭ��ʱ��
                            mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //�������: �µ��ļ�����: 2015.05.18
                            mCmdParam.Modify.mFileSize = 0xffffffff;  // �������: �µ��ļ�����,���ֽ�Ϊ��λд�ļ�Ӧ���ɳ����ر��ļ�ʱ�Զ����³���,���Դ˴����޸�
                            i = CHRV3FileModify( );   //�޸ĵ�ǰ�ļ�����Ϣ,�޸�����
                            mStopIfError( i );
                            printf( "Close\r\n" );
                            mCmdParam.Close.mUpdateLen = 1;     /* �Զ������ļ�����,���ֽ�Ϊ��λд�ļ�,�����ó����ر��ļ��Ա��Զ������ļ����� */
                            i = CHRV3FileClose( );
                            mStopIfError( i );

                            /* ɾ��ĳ�ļ� */
    //          printf( "Erase\n" );
    //          strcpy( (PCHAR)mCmdParam.Create.mPathName, "/OLD" );  //����ɾ�����ļ���,�ڸ�Ŀ¼��
    //          i = CHRV3FileErase( );  //ɾ���ļ����ر�
    //          if ( i != ERR_SUCCESS ) printf( "Error: %02X\n", (UINT16)i );  //��ʾ����
                        }
                        else
                        {
                            //���ϣ������������ӵ�ԭ�ļ���β��,�����ƶ��ļ�ָ��
                          ///////////һ��д���ļ�/////////////////////////////////////////
                            printf( "ByteWrite\r\n" );
                            mCmdParam.ByteLocate.mByteOffset = 0xffffffff;  //�Ƶ��ļ���β��
                            CHRV3ByteLocate( );
                          //ʵ��Ӧ���ж�д���ݳ��ȺͶ��建���������Ƿ������������ڻ�������������Ҫ���д��
                            i = sprintf( (PCHAR)buf,"Note: \xd\xa������������ֽ�Ϊ��λ����U���ļ���д,����ʾ���ܡ�\xd\xa");  /*��ʾ */
                            for(t=0; t<10; t++)
                            {
                                mCmdParam.ByteWrite.mByteCount = i;                          /* ָ������д����ֽ��� */
                                mCmdParam.ByteWrite.mByteBuffer = buf;                       /* ָ�򻺳��� */
                                ret = CHRV3ByteWrite( );                                       /* ���ֽ�Ϊ��λ���ļ�д������ */
                                mStopIfError( ret );
                                printf("�ɹ�д�� %02X��\r\n",(UINT16)t);
                            }

                            ///////////������ȡ�ļ�ǰN�ֽ�/////////////////////////////////////////
                            TotalCount = 100;                                      //����׼����ȡ�ܳ���100�ֽ�
                            printf( "������ǰ%d���ַ���:\r\n",TotalCount );
                            while ( TotalCount ) {                                 //����ļ��Ƚϴ�,һ�ζ�����,�����ٵ���CH103ByteRead������ȡ,�ļ�ָ���Զ�����ƶ�
                                if ( TotalCount > (MAX_PATH_LEN-1) ) t = MAX_PATH_LEN-1;/* ʣ�����ݽ϶�,���Ƶ��ζ�д�ĳ��Ȳ��ܳ��� sizeof( mCmdParam.Other.mBuffer ) */
                                else t = TotalCount;                                 /* ���ʣ����ֽ��� */
                                mCmdParam.ByteRead.mByteCount = t;                   /* ���������ʮ�ֽ����� */
                                mCmdParam.ByteRead.mByteBuffer= &buf[0];
                                ret = CHRV3ByteRead( );                                /* ���ֽ�Ϊ��λ��ȡ���ݿ�,���ζ�д�ĳ��Ȳ��ܳ���MAX_BYTE_IO,�ڶ��ε���ʱ���Ÿղŵ����� */
                                TotalCount -= mCmdParam.ByteRead.mByteCount;           /* ����,��ȥ��ǰʵ���Ѿ��������ַ��� */
                                for ( i=0; i!=mCmdParam.ByteRead.mByteCount; i++ ) printf( "%c", mCmdParam.ByteRead.mByteBuffer[i] );  /* ��ʾ�������ַ� */
                                if ( mCmdParam.ByteRead.mByteCount < t ) {             /* ʵ�ʶ������ַ�������Ҫ��������ַ���,˵���Ѿ����ļ��Ľ�β */
                                    printf( "\r\n" );
                                    printf( "�ļ��Ѿ�����\r\n" );
                                    break;
                                }
                            }

                          ///////////������ָ��λ�ö�ȡ�ļ�N�ֽ�/////////////////////////////////////////
                            printf( "Close\r\n" );
                            i = CHRV3FileClose( );                                    /* �ر��ļ� */
                            mStopIfError( i );
                        }

                    }
                }
                else
                {
                    printf("enum error��%02x\n",ret);
                }
            }
            else
            {
                USBHS_HostInit(DISABLE);
                USBHS_HostInit(ENABLE);
                CHRV3DiskStatus = DISK_DISCONNECT;
                printf("disconnect\n");
            }
        }
	}
}




