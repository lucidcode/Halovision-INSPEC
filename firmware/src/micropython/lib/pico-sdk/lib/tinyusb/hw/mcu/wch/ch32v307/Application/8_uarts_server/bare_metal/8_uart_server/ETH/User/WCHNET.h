/********************************** (C) COPYRIGHT *******************************
* File Name          : WCHNET.H
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/13
* Description        : ��̫��Э��ջ��ͷ�ļ�
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/



#ifndef __WCHNET_H__
#define __WCHNET_H__
#include "ch32v30x_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef     TRUE
#define     TRUE    1
#define     FALSE   0
#endif
#ifndef     NULL
#define     NULL    0
#endif

#define WCHNET_LIB_VER                      0x10

#ifndef WCHNET_MAX_SOCKET_NUM
#define WCHNET_MAX_SOCKET_NUM               8                                /* Socket�ĸ������û��������ã�Ĭ��Ϊ4��Socket,���Ϊ32 */
#endif

#define   ETH_RXBUFNB                         10                             /* MAC�������������� */
#define   ETH_TXBUFNB                         10                             /* MAC�������������� */                                                                        /* #ifndef RX_QUEUE_ENTRIES */
                                                                                
#ifndef RX_BUF_SIZE
#define RX_BUF_SIZE                           1520                             /* MAC����ÿ�����������ȣ�Ϊ4�������� */
#endif

#ifndef WCHNET_PING_ENABLE
#define WCHNET_PING_ENABLE                  TRUE                              /* Ĭ��PING���� */
#endif                                                                          /* PINGʹ�� */   
                                                                                
#ifndef TCP_RETRY_COUNT                                                         
#define TCP_RETRY_COUNT                       20                               /* TCP���Դ�����λ��Ϊ5λ*/
#endif                                                                          
                                                                                
#ifndef TCP_RETRY_PERIOD                                                        
#define TCP_RETRY_PERIOD                      10                               /* TCP�������ڣ���λΪMS�� */
#endif                                                                          
                                                                                
#ifndef WCHNETTIMERPERIOD                                                      
#define WCHNETTIMERPERIOD                     10                                /* ��ʱ�����ڣ���λMs,���ô���500 */
#endif                                                                          

#ifndef SOCKET_SEND_RETRY                                                       
#define SOCKET_SEND_RETRY                     1                                 /* Ĭ�Ϸ������� */
#endif                    

#define LIB_CFG_VALUE                         ((SOCKET_SEND_RETRY << 25) |\
                                              (1 << 24) |\
                                              (TCP_RETRY_PERIOD << 19) |\
                                              (TCP_RETRY_COUNT << 14) |\
                                              (WCHNET_PING_ENABLE << 13) |\
                                              (ETH_TXBUFNB << 9) |\
                                              (ETH_RXBUFNB << 5) |\
                                              (WCHNET_MAX_SOCKET_NUM))
#ifndef MISCE_CFG0_TCP_SEND_COPY 
#define MISCE_CFG0_TCP_SEND_COPY              1                                 /* TCP���ͻ��������� */
#endif                                                                          

#ifndef CFG0_TCP_RECV_COPY                                                
#define CFG0_TCP_RECV_COPY              1                                 /* TCP���ո����Ż����ڲ�����ʹ�� */
#endif                                                                          

#ifndef MISCE_CFG0_TCP_OLD_DELETE                                               
#define MISCE_CFG0_TCP_OLD_DELETE             0                                 /* ɾ�������TCP���� */
#endif

/*�����ڴ���� */
#ifndef WCHNET_NUM_IPRAW
#define WCHNET_NUM_IPRAW                    4                                 /* IPRAW���ӵĸ��� */
#endif

#ifndef WCHNET_NUM_UDP
#define WCHNET_NUM_UDP                      8                                 /* UDP���ӵĸ��� */
#endif

#ifndef WCHNET_NUM_TCP
#define WCHNET_NUM_TCP                      8                                 /* TCP���ӵĸ��� */
#endif

#ifndef WCHNET_NUM_TCP_LISTEN
#define WCHNET_NUM_TCP_LISTEN               8                                 /* TCP�����ĸ��� */
#endif

#ifndef WCHNET_NUM_PBUF
#define WCHNET_NUM_PBUF                     5                                 /* PBUF�ṹ�ĸ��� */
#endif

#ifndef WCHNET_NUM_POOL_BUF
#define WCHNET_NUM_POOL_BUF                 6                                /* POOL BUF�ĸ��� */
#endif

#ifndef WCHNET_NUM_TCP_SEG
#define WCHNET_NUM_TCP_SEG                  10                                /* tcp�εĸ���*/
#endif

#ifndef WCHNET_NUM_IP_REASSDATA
#define WCHNET_NUM_IP_REASSDATA             5                                 /* IP�ֶεĳ��� */
#endif

#ifndef WCHNET_TCP_MSS
#define WCHNET_TCP_MSS                      800                               /* tcp MSS�Ĵ�С*/
#endif

#ifndef WCH_MEM_HEAP_SIZE
#define WCH_MEM_HEAP_SIZE                   4600                             /* �ڴ�Ѵ�С */
#endif

#ifndef WCHNET_NUM_ARP_TABLE
#define WCHNET_NUM_ARP_TABLE                10                                /* ARP�б���� */
#endif

#ifndef WCHNET_MEM_ALIGNMENT
#define WCHNET_MEM_ALIGNMENT                4                                 /* 4�ֽڶ��� */
#endif

#ifndef WCHNET_IP_REASS_PBUFS
#if (WCHNET_NUM_POOL_BUF < 32)
#define WCHNET_IP_REASS_PBUFS               (WCHNET_NUM_POOL_BUF - 1)       /* IP��Ƭ��PBUF���������Ϊ31 */
#else
#define WCHNET_IP_REASS_PBUFS               31
#endif
#endif

#define WCHNET_MISC_CONFIG0                 ((MISCE_CFG0_TCP_SEND_COPY << 0) |\
                                               (CFG0_TCP_RECV_COPY << 1) |\
                                               (MISCE_CFG0_TCP_OLD_DELETE << 2)|\
                                               (WCHNET_IP_REASS_PBUFS)<<3)
/* PHY ״̬ */
#define PHY_LINK_SUCCESS                              (1<<2)                            /*PHY��������*/
#define PHY_AUTO_SUCCESS                              (1<<5)                            /* PHY�Զ�Э�����*/
                                                                                
/* Socket ����ģʽ����,Э������ */                                                       
#define PROTO_TYPE_IP_RAW                     0                                 /* IP��ԭʼ���� */
#define PROTO_TYPE_UDP                        2                                 /* UDPЭ������ */
#define PROTO_TYPE_TCP                        3                                 /* TCPЭ������ */
                                                                                
/* �ж�״̬ */                                                                  
/* ����ΪGLOB_INT�������״̬ */                                                
#define GINT_STAT_UNREACH                     (1<<0)                            /* ���ɴ��ж�*/
#define GINT_STAT_IP_CONFLI                   (1<<1)                            /* IP��ͻ*/
#define GINT_STAT_PHY_CHANGE                  (1<<2)                            /* PHY״̬�ı� */
#define GINT_STAT_SOCKET                      (1<<4)                            /* scoket �����ж� */
                                                                                
/*����ΪSn_INT�������״̬*/                                                    
#define SINT_STAT_RECV                        (1<<2)                            /* socket�˿ڽ��յ����ݻ��߽��ջ�������Ϊ�� */
#define SINT_STAT_CONNECT                     (1<<3)                            /* ���ӳɹ�,TCPģʽ�²������ж� */
#define SINT_STAT_DISCONNECT                  (1<<4)                            /* ���ӶϿ�,TCPģʽ�²������ж� */
#define SINT_STAT_TIM_OUT                     (1<<6)                            /* ARP��TCPģʽ�»ᷢ�����ж� */

/* ������ */
#define WCHNET_ERR_SUCCESS                  0x00                              /* ��������ɹ� */
#define WCHNET_RET_ABORT                    0x5F                              /* �������ʧ�� */
#define WCHNET_ERR_BUSY                     0x10                              /* æ״̬����ʾ��ǰ����ִ������ */
#define WCHNET_ERR_MEM                      0x11                              /* �ڴ���� */
#define WCHNET_ERR_BUF                      0x12                              /* ���������� */
#define WCHNET_ERR_TIMEOUT                  0x13                              /* ��ʱ */
#define WCHNET_ERR_RTE                      0x14                              /* ·�ɴ���*/
#define WCHNET_ERR_ABRT                     0x15                              /* ����ֹͣ*/
#define WCHNET_ERR_RST                      0x16                              /* ���Ӹ�λ */
#define WCHNET_ERR_CLSD                     0x17                              /* ���ӹر�/socket �ڹر�״̬*/
#define WCHNET_ERR_CONN                     0x18                              /* ������ */
#define WCHNET_ERR_VAL                      0x19                              /* �����ֵ */
#define WCHNET_ERR_ARG                      0x1a                              /* ����Ĳ��� */
#define WCHNET_ERR_USE                      0x1b                              /* �Ѿ���ʹ�� */
#define WCHNET_ERR_IF                       0x1c                              /* MAC����  */
#define WCHNET_ERR_ISCONN                   0x1d                              /* ������ */
#define WCHNET_ERR_SOCKET_MEM               0X20                              /* Socket��Ϣ�б��������ߴ��� */
#define WCHNET_ERR_UNSUPPORT_PROTO          0X21                              /* ��֧�ֵ�Э������ */
#define WCHNET_ERR_UNKNOW                   0xFA                              /* δ֪���� */

/* ���ɴ���� */
#define UNREACH_CODE_HOST                     0                                 /* �������ɴ� */
#define UNREACH_CODE_NET                      1                                 /* ���粻�ɴ� */
#define UNREACH_CODE_PROTOCOL                 2                                 /* Э�鲻�ɴ� */
#define UNREACH_CODE_PROT                     3                                 /* �˿ڲ��ɴ� */
/*����ֵ��ο�RFC792�ĵ�*/                                                      

/* TCP�رղ��� */
#define TCP_CLOSE_NORMAL                      0                                 /* �����رգ�����4������ */
#define TCP_CLOSE_RST                         1                                 /* ��λ���ӣ����ر�  */
#define TCP_CLOSE_ABANDON                     2                                 /* �ڲ��������ӣ����ᷢ���κ���ֹ���� */

/* socket״̬ */
#define  SOCK_STAT_CLOSED                     0X00                              /* socket�ر� */
#define  SOCK_STAT_OPEN                       0X05                              /* socket�� */

/* TCP״̬ */
#define TCP_CLOSED                            0                                 /* TCP���� */
#define TCP_LISTEN                            1                                 /* TCP�ر� */
#define TCP_SYN_SENT                          2                                 /* SYN���ͣ��������� */
#define TCP_SYN_RCVD                          3                                 /* SYN���գ����յ��������� */
#define TCP_ESTABLISHED                       4                                 /* TCP���ӽ��� */
#define TCP_FIN_WAIT_1                        5                                 /* WAIT_1״̬ */
#define TCP_FIN_WAIT_2                        6                                 /* WAIT_2״̬ */
#define TCP_CLOSE_WAIT                        7                                 /* �ȴ��ر� */
#define TCP_CLOSING                           8                                 /* ���ڹر� */
#define TCP_LAST_ACK                          9                                 /* LAST_ACK*/
#define TCP_TIME_WAIT                         10                                /* 2MSL�ȴ� */

/* sokcet��Ϣ�� */

typedef struct _SCOK_INF
{
    u32 IntStatus;                                                           /* �ж�״̬ */
    u32 SockIndex;                                                           /* Socket����ֵ */
    u32 RecvStartPoint;                                                      /* ���ջ������Ŀ�ʼָ�� */
    u32 RecvBufLen;                                                          /* ���ջ��������� */
    u32 RecvCurPoint;                                                        /* ���ջ������ĵ�ǰָ�� */
    u32 RecvReadPoint;                                                       /* ���ջ������Ķ�ָ�� */
    u32 RecvRemLen;                                                          /* ���ջ�������ʣ�೤�� */
    u32 ProtoType;                                                           /* Э������ */
    u32 ScokStatus;                                                          /* ���ֽ�Socket״̬���ε��ֽ�ΪTCP״̬����TCPģʽ�������� */
    u32 DesPort;                                                             /* Ŀ�Ķ˿� */
    u32 SourPort;                                                            /* Դ�˿���IPRAWģʽ��ΪЭ������ */
    u8  IPAddr[4];                                                           /* SocketĿ��IP��ַ 32bit*/
    void *Resv1;                                                                /* �������ڲ�ʹ�ã����ڱ������PCB */
    void *Resv2;                                                                /* �������ڲ�ʹ�ã�TCP Serverʹ�� */
    void (*AppCallBack)(struct _SCOK_INF *,u32 ,u16 ,u8 *,u32 ); /* ���ջص�����*/

}SOCK_INF;


struct _WCH_CFG
{
    u32 RxBufSize;                                                     /* MAC���ջ�������С */
    u32 TCPMss;                                                        /* TCP MSS��С */
    u32 HeapSize;                                                      /* �ѷ����ڴ��С */
    u32 ARPTableNum;                                                   /* ARP�б���� */
    u32 MiscConfig0;                                                   /* �������� */
};


struct _NET_SYS
{
    u8  IPAddr[4];                                                           /* IP��ַ 32bit */
    u8  GWIPAddr[4];                                                         /* ���ص�ַ 32bit */
    u8  MASKAddr[4];                                                         /* �������� 32bit */
    u8  MacAddr[8];                                                          /* MAC��ַ 48bit */
    u8  UnreachIPAddr[4];                                                    /* ���ɵ���IP */
    u32 RetranCount;                                                         /* ���Դ��� Ĭ��Ϊ10�� */
    u32 RetranPeriod;                                                        /* ��������,��λMS,Ĭ��200MS */
    u32 PHYStat;                                                             /* PHY״̬�� 8bit */
    u32 NetStat;                                                           /* ��̫����״̬ �������Ƿ�򿪵� */
    u32 MackFilt;                                                            /*  MAC���ˣ�Ĭ��Ϊ���չ㲥�����ձ���MAC 8bit */
    u32 GlobIntStatus;                                                       /* ȫ���ж� */
    u32 UnreachCode;                                                         /* ���ɴ� */
    u32 UnreachProto;                                                        /* ���ɴ�Э�� */
    u32 UnreachPort;                                                         /* ���ɵ���˿� */
    u32 SendFlag;
    u32 Flags;
};


/* KEEP LIVE���ýṹ�� */
#ifndef ST_KEEP_CFG
#define ST_KEEP_CFG
struct _KEEP_CFG
{
   u32 KLIdle;                                                               /* KEEPLIVE����ʱ�� */
   u32 KLIntvl;                                                              /* KEEPLIVE���� */
   u32 KLCount;                                                              /* KEEPLIVE���� */
};
#endif

/* ����ֵΪ�̶�ֵ�����Ը��� */
#define WCHNET_MEM_ALIGN_SIZE(size)         (((size) + WCHNET_MEM_ALIGNMENT - 1) & ~(WCHNET_MEM_ALIGNMENT-1))
#define WCHNET_SIZE_IPRAW_PCB               0xFF                            /* IPRAW PCB��С */
#define WCHNET_SIZE_UDP_PCB                 0x30                              /* UDP PCB��С */
#define WCHNET_SIZE_TCP_PCB                 0xFF                              /* TCP PCB��С */
#define WCHNET_SIZE_TCP_PCB_LISTEN          0x40                              /* TCP LISTEN PCB��С */
#define WCHNET_SIZE_IP_REASSDATA            0x40                              /* IP��Ƭ����  */
#define WCHNET_SIZE_PBUF                    0x40                              /* Packet Buf */
#define WCHNET_SIZE_TCP_SEG                 0x60                              /* TCP SEG�ṹ */
#define WCHNET_SIZE_MEM                     0x0c                             /* sizeof(struct mem) */
#define WCHNET_SIZE_ARP_TABLE               0x20                             /* sizeof arp table */

#define WCHNET_SIZE_POOL_BUF                WCHNET_MEM_ALIGN_SIZE(WCHNET_TCP_MSS + 40 +14) /* pbuf�ش�С */
#define WCHNET_MEMP_SIZE                    ((WCHNET_MEM_ALIGNMENT - 1) + \
                                              (WCHNET_NUM_IPRAW * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IPRAW_PCB)) + \
                                              (WCHNET_NUM_UDP * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_UDP_PCB)) + \
                                              (WCHNET_NUM_TCP * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB)) + \
                                              (WCHNET_NUM_TCP_LISTEN * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB_LISTEN)) + \
                                              (WCHNET_NUM_TCP_SEG * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_SEG)) + \
                                              (WCHNET_NUM_IP_REASSDATA * WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IP_REASSDATA)) + \
                                              (WCHNET_NUM_PBUF * (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(0))) + \
                                              (WCHNET_NUM_POOL_BUF * (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_POOL_BUF))))


#define  HEAP_MEM_ALIGN_SIZE                  (WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_MEM))
#define  WCHNET_RAM_HEAP_SIZE               (WCH_MEM_HEAP_SIZE + (2 * HEAP_MEM_ALIGN_SIZE) + WCHNET_MEM_ALIGNMENT)
#define  WCHNET_RAM_ARP_TABLE_SIZE          (WCHNET_SIZE_ARP_TABLE * WCHNET_NUM_ARP_TABLE)
/* DNS�ṹ��ص� */
typedef void (*dns_callback)(const char *name, u8 *ipaddr, void *callback_arg);

/* ������� */
/* ���WCHNET_NUM_IPRAW�Ƿ�С��1 */
#if(WCHNET_NUM_IPRAW < 1)
#error "WCHNET_NUM_IPRAW Error,Please Config WCHNET_NUM_IPRAW >= 1"
#endif
/* ���WCHNET_SIZE_UDP_PCB�Ƿ�С��1 */
#if(WCHNET_SIZE_UDP_PCB < 1)
#error "WCHNET_SIZE_UDP_PCB Error,Please Config WCHNET_SIZE_UDP_PCB >= 1"
#endif
/* ���WCHNET_NUM_TCP�Ƿ�С��1 */
#if(WCHNET_NUM_TCP < 1)
#error "WCHNET_NUM_TCP Error,Please Config WCHNET_NUM_TCP >= 1"
#endif
/* ���WCHNET_NUM_TCP_LISTEN�Ƿ�С��1 */
#if(WCHNET_NUM_TCP_LISTEN < 1)
#error "WCHNET_NUM_TCP_LISTEN Error,Please Config WCHNET_NUM_TCP_LISTEN >= 1"
#endif
/* ����ֽڶ������Ϊ4�������� */
#if((WCHNET_MEM_ALIGNMENT % 4) || (WCHNET_MEM_ALIGNMENT == 0))
#error "WCHNET_MEM_ALIGNMENT Error,Please Config WCHNET_MEM_ALIGNMENT = 4 * N, N >=1"
#endif
/* TCP����Ķγ��� */
#if((WCHNET_TCP_MSS > 1460) || (WCHNET_TCP_MSS < 60))
#error "WCHNET_TCP_MSS Error,Please Config WCHNET_TCP_MSS >= 60 && WCHNET_TCP_MSS <= 1460"
#endif
/* ARP�������� */
#if((WCHNET_NUM_ARP_TABLE > 0X7F) || (WCHNET_NUM_ARP_TABLE < 1))
#error "WCHNET_NUM_ARP_TABLE Error,Please Config WCHNET_NUM_ARP_TABLE >= 1 && WCHNET_NUM_ARP_TABLE <= 0X7F"
#endif
/* ���POOL BUF���� */
#if(WCHNET_NUM_POOL_BUF < 1)
#error "WCHNET_NUM_POOL_BUF Error,Please Config WCHNET_NUM_POOL_BUF >= 1"
#endif
/* ���PBUF�ṹ���� */
#if(WCHNET_NUM_PBUF < 1)
#error "WCHNET_NUM_PBUF Error,Please Config WCHNET_NUM_PBUF >= 1"
#endif
/* ���IP�������� */
#if((WCHNET_NUM_IP_REASSDATA > 10)||(WCHNET_NUM_IP_REASSDATA < 1))
#error "WCHNET_NUM_IP_REASSDATA Error,Please Config WCHNET_NUM_IP_REASSDATA < 10 && WCHNET_NUM_IP_REASSDATA >= 1 "
#endif
/* ���IP��Ƭ��С */
#if(WCHNET_IP_REASS_PBUFS > WCHNET_NUM_POOL_BUF)
#error "WCHNET_IP_REASS_PBUFS Error,Please Config WCHNET_IP_REASS_PBUFS < WCHNET_NUM_POOL_BUF"
#endif


extern  ETH_DMADESCTypeDef /* ������������ */DMARxDscrTab[ETH_RXBUFNB];
extern  ETH_DMADESCTypeDef /* ������������ */DMATxDscrTab[ETH_TXBUFNB];
extern u8  MACRxBuf[ETH_RXBUFNB*ETH_MAX_PACKET_SIZE];
extern u8  MACTxBuf[ETH_TXBUFNB*ETH_MAX_PACKET_SIZE];



/* ���ڲ��������� */


u8 WCHNET_Init(const u8* ip,const u8* gwip,const u8* mask,const u8* macaddr); /* ���ʼ�� */

u8 WCHNET_GetVer(void);

u8 WCH_GetMac(u8 *macaddr);

u8 WCHNET_ConfigLIB(struct _WCH_CFG *cfg);                               /* ���ÿ�*/
                                                                                
void  WCHNET_MainTask(void);                                                  /* ��������������Ҫһֱ���ϵ��� */
                                                                                
void WCHNET_TimeIsr(u16 timperiod);                                        /* ʱ���жϷ�����������ǰ������ʱ������ */
                                                                                
void WCHNET_ETHIsr(void);                                                  /* ETH�жϷ����� */
                                                                                
u8 WCHNET_GetPHYStatus(void);                                              /* ��ȡPHY״̬ */
                                                                                
u8 WCHNET_QueryGlobalInt(void);                                             /* ��ѯȫ���ж� */
                                                                                
u8 WCHNET_GetGlobalInt (void);                                             /* ��ȫ���жϲ���ȫ���ж����� */
                                                                                
void WCHNET_OpenMac(void);                                                    /* ��MAC */
                                                                                
void WCHNET_CloseMac(void);                                                   /* �ر�MAC */
                                                                                
u8 WCHNET_SocketCreat(u8 *socketid,SOCK_INF *socinf);                   /* ����socket */
                                                                                
u8 WCHNET_SocketSend(u8 socketid,u8 *buf,u32 *len);               /* Socket�������� */
                                                                                
u8 WCHNET_SocketRecv(u8 socketid,u8 *buf,u32 *len);               /* Socket�������� */
                                                                                
u8 WCHNET_GetSocketInt(u8 sockedid);                                    /* ��ȡsocket�жϲ����� */
                                                                                
u32 WCHNET_SocketRecvLen(u8 socketid,u32 *bufaddr);                  /* ��ȡsocket���ճ��� */
                                                                                
u8 WCHNET_SocketConnect(u8 socketid);                                   /* TCP����*/
                                                                                
u8 WCHNET_SocketListen(u8 socindex);                                    /* TCP���� */
                                                                                
u8 WCHNET_SocketClose(u8 socindex,u8 flag);                          /* �ر����� */

void WCHNET_ModifyRecvBuf(u8 sockeid,u32 bufaddr,u32 bufsize);       /* �޸Ľ��ջ����� */

u8 WCHNET_SocketUdpSendTo(u8 socketid, u8 *buf, u32 *slen,u8 *sip,u16 port);/* ��ָ����Ŀ��IP���˿ڷ���UDP�� */

u8 WCHNET_Aton(const u8 *cp, u8 *addr);                              /* ASCII���ַת�����ַ */
                                                                                
u8 *WCHNET_Ntoa(u8 *ipaddr);                                            /* �����ַתASCII��ַ */
                                                                                
u8 WCHNET_SetSocketTTL(u8 socketid, u8 ttl);                         /* ����socket��TTL */
                                                                                
void WCHNET_RetrySendUnack(u8 socketid);                                   /* TCP�ش� */

u8 WCHNET_QueryUnack(SOCK_INF  *sockinf,u32 *addrlist,u16 lislen)    /* ��ѯδ���ͳɹ������ݰ� */;

u8 WCHNET_DHCPStart(u8(* usercall)(u8 status,void *));               /* DHCP���� */
                                                                                
u8 WCHNET_DHCPStop(void);                                                  /* DHCPֹͣ */

void WCHNET_InitDNS(u8 *dnsip,u16 port);                                /* DNS��ʼ�� */

u8 WCHNET_GetHostName(const char *hostname,u8 *addr,dns_callback found,void *arg);/* DNS��ȡ������ */

void WCHNET_ConfigKeepLive(struct _KEEP_CFG *cfg);                            /* ���ÿ�KEEP LIVE���� */

u8 WCHNET_SocketSetKeepLive(u8 socindex,u8 cfg);                     /* ����socket KEEP LIVE*/

void WCHNET_SetHostname(char *name);

#ifdef __cplusplus
}
#endif
#endif


