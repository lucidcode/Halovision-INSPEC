/**
  @file lwipopts.h
 */
/*
 * ============================================================================
 * History
 * =======
 * 2017-03-15 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#ifndef LOCAL_LWIPOPTS_H_
#define LOCAL_LWIPOPTS_H_

#include "ft900opts.h"

/* NO_SYS
 * Do not use Operating System (RTOS) style primitives and implement stack
 * as a single threaded application.
 */
#ifdef FT32_FREERTOS
#define NO_SYS                     0
#else
#define NO_SYS                     1
#endif

#define USE_FREERTOS_MALLOC        (NO_SYS==0)

#if (USE_FREERTOS_MALLOC != 0)
/**
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwip internal allocator. Can save code size if you
 * already use it.
 */
#define MEM_LIBC_MALLOC                 1
#define mem_clib_free vPortFree
#define mem_clib_malloc pvPortMalloc
#define mem_clib_calloc my_mem_calloc
#else
#define MEM_LIBC_MALLOC                 1
#endif

/**
* MEMP_MEM_MALLOC==1: Use mem_malloc/mem_free instead of the lwip pool allocator.
* Especially useful with MEM_LIBC_MALLOC but handle with care regarding execution
* speed and usage from interrupts!
*/
#define MEMP_MEM_MALLOC             1

/* NO_SYS_NO_TIMERS
 * Still use timers provided by interrupt sources in the application.
 */
#define NO_SYS_NO_TIMERS           0

/* SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task protection
 * for certain critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT      (NO_SYS==0)

/* LWIP_SOCKET/LWIP_NETCONN
 * Disable socket interface and network sequential API as NO_SYS is defined.
 */
#define LWIP_SOCKET               (NO_SYS==0)
#define LWIP_NETCONN              (NO_SYS==0)

/* LWIP_IGMP
 * Enable Internet Group Management Protocol
 * IP multicast support is used in this application.
 */
#define LWIP_IGMP                  1

/* LWIP_ICMP
 * Enable Internet Control Message Protocol
 * Allow support for ICMP which includes ping.
 */
#define LWIP_ICMP                  1

/* LWIP_SNMP
 * Disable Simple Network Management Protocol
 * There's no need for SNMP support in this application.
 */
#define LWIP_SNMP                  0

/* LWIP_DNS
 * Disable lwIP DNS module,
 */
#define LWIP_DNS                   1
#if LWIP_DNS
#define DNS_TABLE_SIZE             1   // number of table entries, default 4
#define DNS_MAX_NAME_LENGTH        64  // max. name length, default 256
#define DNS_MAX_SERVERS            1   // number of DNS servers, default 2
#define DNS_DOES_NAME_CHECK        1   // compare received name with given,def 0
#define DNS_MSG_SIZE               256
#endif

/* LWIP_RAW
 * Disable raw access so application has to go through UDP or TCP.
 */
#define LWIP_RAW                   0

/* LWIP_HAVE_LOOPIF
 * Disable loopback interface.
 */
#define LWIP_HAVE_LOOPIF           0
#define LWIP_NETIF_LOOPBACK        0
#define LWIP_LOOPBACK_MAX_PBUFS    5

#define TCP_LISTEN_BACKLOG         (NO_SYS)

#define LWIP_COMPAT_SOCKETS        (NO_SYS==0)
#define LWIP_PROVIDE_ERRNO         1
#define LWIP_SO_SNDTIMEO           1
#define LWIP_SO_RCVTIMEO           1
#define LWIP_SO_RCVBUF             0

#define LWIP_TCPIP_CORE_LOCKING    0

/* LWIP_NETIF_LINK_CALLBACK
 * Do provide a callback for link status change.
 */
#define LWIP_NETIF_LINK_CALLBACK   1

/* LWIP_NETIF_STATUS_CALLBACK
 * Do provide a callback for connection status change.
 */
#define LWIP_NETIF_STATUS_CALLBACK 1

#define LWIP_DEBUG
#define LWIP_NOASSERT

#ifdef LWIP_DEBUG
//#define LWIP_DBG_TRACE

#define LWIP_DBG_MIN_LEVEL         0
#define SYS_DEBUG                  LWIP_DBG_OFF
#define TIMERS_DEBUG               LWIP_DBG_OFF
#define PPP_DEBUG                  LWIP_DBG_OFF
#define MEM_DEBUG                  LWIP_DBG_OFF
#define MEMP_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                 LWIP_DBG_OFF
#define API_LIB_DEBUG              LWIP_DBG_OFF
#define API_MSG_DEBUG              LWIP_DBG_OFF
#define TCPIP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                LWIP_DBG_OFF
#define SOCKETS_DEBUG              LWIP_DBG_OFF
#define ETHARP_DEBUG               LWIP_DBG_OFF
#define DNS_DEBUG                  LWIP_DBG_OFF
#define AUTOIP_DEBUG               LWIP_DBG_OFF
#define DHCP_DEBUG                 LWIP_DBG_OFF
#define RAW_DEBUG                  LWIP_DBG_OFF
#define IP_DEBUG                   LWIP_DBG_OFF
#define IP_REASS_DEBUG             LWIP_DBG_OFF
#define ICMP_DEBUG                 LWIP_DBG_OFF
#define IGMP_DEBUG                 LWIP_DBG_OFF
#define UDP_DEBUG                  LWIP_DBG_OFF
#define TCP_DEBUG                  LWIP_DBG_OFF
#define TCP_INPUT_DEBUG            LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG           LWIP_DBG_OFF
#define TCP_RTO_DEBUG              LWIP_DBG_OFF
#define TCP_CWND_DEBUG             LWIP_DBG_OFF
#define TCP_WND_DEBUG              LWIP_DBG_OFF
#define TCP_FR_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG             LWIP_DBG_OFF
#define TCP_RST_DEBUG              LWIP_DBG_OFF
#define RTP_DEBUG                  LWIP_DBG_OFF

#endif /* LWIP_DEBUG */

#define LWIP_DBG_TYPES_ON         (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)


/* ---------- Memory options ---------- */

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                   4

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 * Ignored when MEM_LIBC_MALLOC==1
 */
#define MEM_SIZE                        (10 * 1024)

/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 *
 * Default 16.
 */
#define MEMP_NUM_PBUF                   4

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 *
 * Default 4.
 */
#define MEMP_NUM_RAW_PCB                0

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 *
 * Default 4.
 */
#define MEMP_NUM_UDP_PCB                4

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 *
 * Default 5.
 */
#define MEMP_NUM_TCP_PCB                8

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 *
 * Default 8.
 */
#define MEMP_NUM_TCP_PCB_LISTEN         4

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 *
 * Default 16.
 */
#define MEMP_NUM_TCP_SEG                4

/**
 * MEMP_NUM_FRAG_PBUF: the number of IP fragments simultaneously sent
 * (fragments, not whole packets!).
 * This is only used with IP_FRAG_USES_STATIC_BUF==0 and
 * LWIP_NETIF_TX_SINGLE_PBUF==0 and only has to be > 1 with DMA-enabled MACs
 * where the packet is not yet sent when netif->output returns.
 *
 * Default 15.
 */
#define MEMP_NUM_FRAG_PBUF              4

/**
 * MEMP_NUM_ARP_QUEUE: the number of simulateously queued outgoing
 * packets (pbufs) that are waiting for an ARP request (to resolve
 * their destination address) to finish.
 * (requires the ARP_QUEUEING option)
 *
 * Default 30.
 */
#define MEMP_NUM_ARP_QUEUE              4

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 *
 * Default 2.
 */
#define MEMP_NUM_NETBUF                 2

/**
 * MEMP_NUM_IGMP_GROUP: The number of multicast groups whose network interfaces
 * can be members et the same time (one per netif - allsystems group -, plus one
 * per netif membership).
 * (requires the LWIP_IGMP option)
 *
 * Default 8.
 */
#define MEMP_NUM_IGMP_GROUP             8

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 *
 * Default 4.
 */
#define MEMP_NUM_NETCONN                4

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 *
 * Default 8.
 */
#define MEMP_NUM_TCPIP_MSG_API          8

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 *
 * Default 8.
 */
#define MEMP_NUM_TCPIP_MSG_INPKT        8

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 *
 * Default 16.
 */
#define PBUF_POOL_SIZE                  8

/**
 * ETHARP_TRUST_IP_MAC==1: Incoming IP packets cause the ARP table to be
 * updated with the source MAC and IP addresses supplied in the packet.
 * You may want to disable this if you do not trust LAN peers to have the
 * correct addresses, or as a limited approach to attempt to handle
 * spoofing. If disabled, lwIP will need to make a new ARP request if
 * the peer is not already in the ARP table, adding a little latency.
 * The peer *is* in the ARP table if it requested our address before.
 * Also notice that this slows down input processing of every IP packet!
 */
#define ETHARP_TRUST_IP_MAC             1

#define MEMP_NUM_SYS_TIMEOUT            4

//#endif

/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. */
#define PBUF_LINK_HLEN          20

/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define TCP_TTL                 255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         0

/* TCP Maximum segment size. */
#define TCP_MSS                 1460

/* TCP receive window. */
#define TCP_WND                 (2 * TCP_MSS)
#define TCP_SND_BUF             (4 * TCP_MSS)

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           4

#define LWIP_TCP_KEEPALIVE 1

/* ---------- ARP options ---------- */
/**
 * LWIP_ARP==1: Enable ARP functionality.
 */
#define LWIP_ARP                        1

/**
 * ARP_TABLE_SIZE: Number of active MAC-IP address pairs cached.
 *
 * Default 10.
 */
#define ARP_TABLE_SIZE                  10

/**
 * ARP_QUEUEING==1: Multiple outgoing packets are queued during hardware address
 * resolution. By default, only the most recent packet is queued per IP address.
 * This is sufficient for most protocols and mainly reduces TCP connection
 * startup time. Set this to 1 if you know your application sends more than one
 * packet in a row to an IP address that is not in the ARP cache.
 *
 * Default 0.
 */
#define ARP_QUEUEING                    0

/* LWIP_BROADCAST_PING==1: respond to broadcast pings (default is unicast only) */
#define LWIP_BROADCAST_PING     1

/* LWIP_MULTICAST_PING==1: respond to multicast pings (default is unicast only) */
#define LWIP_MULTICAST_PING     0

/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD              0

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#define IP_REASSEMBLY           1
#define IP_REASS_MAX_PBUFS      10
#define IP_FRAG                 1

/**
 * MEMP_NUM_REASSDATA: the number of IP packets simultaneously queued for
 * reassembly (whole packets, not fragments!)
 *
 * Default 5.
 */
#define MEMP_NUM_REASSDATA              5

/* ---------- ICMP options ---------- */
#define ICMP_TTL                255

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP               1

/* 1 if you want to want DHCP to wait for link to be up
   before starting. */
#define LWIP_DHCP_CHECK_LINK_UP 1
/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK		1
/* 1 if you want to send a hostname to the DHCP server.
   (recommended). */
#define LWIP_NETIF_HOSTNAME     1

/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP             0
#define LWIP_DHCP_AUTOIP_COOP  (LWIP_DHCP && LWIP_AUTOIP)

/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define LWIP_UDPLITE            0
#define UDP_TTL                 64

/* ---------- Statistics options ---------- */

#define LWIP_STATS              0

#if LWIP_STATS
#define LWIP_STATS_DISPLAY      1
#define LINK_STATS              0
#define IP_STATS                0
#define ICMP_STATS              0
#define IGMP_STATS              0
#define IPFRAG_STATS            0
#define UDP_STATS               0
#define TCP_STATS               0
#define MEM_STATS               1
#define MEMP_STATS              0
#define PBUF_STATS              0
#define SYS_STATS               0
#endif /* LWIP_STATS */


/* ---------- PPP options ---------- */
#define PPP_SUPPORT             0      /* Set > 0 for PPP */

/* ---------- Checksum options ---------- */
/* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets. */
#define CHECKSUM_GEN_IP         1
/* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets. */
#define CHECKSUM_GEN_UDP        1
/* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets. */
#define CHECKSUM_GEN_TCP        1
/* CHECKSUM_GEN_ICMP==1: Generate checksums in software for outgoing ICMP packets. */
#define CHECKSUM_GEN_ICMP       1
/* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets. */
#define CHECKSUM_CHECK_IP       1
/* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets. */
#define CHECKSUM_CHECK_UDP      1
/* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets. */
#define CHECKSUM_CHECK_TCP      1
/* LWIP_CHECKSUM_ON_COPY==1: Calculate checksum when copying data from
 * application buffers to pbufs. */
#define LWIP_CHECKSUM_ON_COPY   0

/* ---------- mDNS options ---------- */
#define LWIP_NUM_NETIF_CLIENT_DATA  1

#if (NO_SYS==0)
/**
 * TCPIP_THREAD_NAME: The name assigned to the main tcpip thread.
 */
#define TCPIP_THREAD_NAME              "TCP/IP"

/**
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_STACKSIZE          300

/**
 * TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_PRIO               1

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#define TCPIP_MBOX_SIZE                 32      //Queue Size (4byte/slot)

/**
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_RAW_RECVMBOX_SIZE       0       //Queue Size (4byte/slot)

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_UDP_RECVMBOX_SIZE       32       //Queue Size (4byte/slot)

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_TCP_RECVMBOX_SIZE       32      //Queue Size (4byte/slot)

/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE         32      //Queue Size (4byte/slot)

#endif /* NO_SYS */

#endif /* LOCAL_LWIPOPTS_H_ */
