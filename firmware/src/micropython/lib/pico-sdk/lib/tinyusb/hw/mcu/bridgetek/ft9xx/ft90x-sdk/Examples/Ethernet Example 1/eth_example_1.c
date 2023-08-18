/**
  @file
  @brief
  Ethernet Example 1

  Allow the user to ping the device.

  To ping the device...
  $ ping 192.168.1.55

  Compile with VERBOSE set to 0 for minimal output
  Compile with VERBOSE set to 1 for verbose output (effects performance)
*/
/*
 * ============================================================================
 * History
 * =======
 * 29 Oct 2014 : Created
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd 
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
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

#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "ft900.h"
#include "tinyprintf.h"

/* CONFIGURATION OPTIONS ......................... */
/** Defines if byte swapping should occur on this device.
 *  For the FT900, leave this set to 1 */
#define BYTESWAP 1
/** Defines if debugging and verbose output should be shown.
 *  Change this to 1 to generate Verbose output */
#define VERBOSE 0
/** The number of receive buffers */
#define N_RX_BUFFERS 4
/** Undefine to use a locally administered MAC address.
 * Define to use a random valid MAC address. */
#undef USE_LOCAL_ADMIN_MAC

/* TYPES ......................................... */
/** The structure used to store packets coming into the device */
struct rx_pkt
{
    size_t len;
    uint8_t data[1500];
};

#pragma pack(push,1)

/** Hardware Header */
struct hw_hdr
{
    uint16_t len;               /**!< Transfer length */
};

/** Ethernet Header
 *  As defined in IEEE802.3 */
struct eth_hdr
{
    uint8_t dest_mac[6];        /**!< Destination MAC Address */
    uint8_t src_mac[6];         /**!< Source MAC Address */
    uint16_t ethertype_len;     /**!< Ethertype or Len */
};

/** ARP Header
 *  As defined in RCF826 */
struct arp_hdr
{
    uint16_t htype;             /**!< Hardware Type */
    uint16_t ptype;             /**!< Protocol Type */
    uint8_t hlen;               /**!< Hardware Address Length */
    uint8_t plen;               /**!< Protocol Address Length */
    uint16_t oper;              /**!< ARP Operation */
    uint8_t src_mac[6];         /**!< Source Hardware Address */
    uint8_t src_ip[4];          /**!< Source Protocol Address */
    uint8_t dest_mac[6];        /**!< Destination Hardware Address */
    uint8_t dest_ip[4];         /**!< Destination Protocol Address */
};

/** IPv4 Header
 *  As defined in RFC791 */
struct ip4_hdr
{
    uint8_t ver_ihl;            /**!< Version & Internet Header Length */
    uint8_t tos;                /**!< Type of Service */
    uint16_t len;               /**!< Datagram Length */
    uint16_t id;                /**!< Identification */
    uint16_t flags_fragoffset;  /**!< Flags & Fragmentation Offset */
    uint8_t ttl;                /**!< Time to Live */
    uint8_t proto;              /**!< Protocol */
    uint16_t hdr_chksum;        /**!< Header Checksum */
    uint8_t src_ip[4];          /**!< Source IP Address */
    uint8_t dest_ip[4];         /**!< Destination IP Address */
};

/** ICMP Header
 *  Ad defined RFC729 */
struct icmp_hdr
{
    uint8_t type;               /**!< ICMP Type */
    uint8_t code;               /**!< ICMP Code */
    uint16_t chksum;            /**!< Datagram Checksum */
    uint16_t id;                /**!< ICMP Identification */
    uint16_t seq;               /**!< ICMP Sequence Number */
};

#pragma pack(pop)

/* CONSTANTS & MACROS ............................ */

#define MIN(a,b) ((a<b)?a:b)

/** Ethernet Ethertype for IP (ICMP,etc) */
#define ETHERTYPE_IP  (0x0800)
/** Ethernet Ethertype for ARP */
#define ETHERTYPE_ARP (0x0806)

/** ARP Operation for a Request */
#define ARP_OPER_ARP_REQUEST (1)
/** ARP Operation for a Reply */
#define ARP_OPER_ARP_REPLY (2)

/** IP Protocol number for ICMP */
#define IP_PROTO_ICMP (1)

/** ICMP Type for an echo ("ping") */
#define ICMP_TYPE_ECHO (8)
/** ICMP Type for an echo reply ("pong") */
#define ICMP_TYPE_ECHOREPLY (0)

/** Casts a variable to a pointer type
 *  @param var The variable to use
 *  @param to The type to cast a pointer of
 *  @return A new pointer of type 'to'
 *
 *  e.g. uint8_t ptr = PTR_AT(a,uint8_t); expands to
 *       uint8_t ptr = (uint8_t *)((void*)(a));
 */
#define PTR_AT(var,to) (to *)((void*)(var))
/** Cast a variable to a pointer type, using the size
 *  of another type to offset it
 *  @param var The variable to use
 *  @param from The type cast from (used to find the end of that type)
 *  @param to The type to cast to
 *
 *  e.g. uint8_t ptr = PTR_AFTER(a,uint32_t,uint8_t); expands to
 *       uint8_t ptr = (uint8_t *)((void*)(a) + sizeof(uint32_t));
 */
#define PTR_AFTER(var,from,to) (to *)((void*)(var) + sizeof(from))

/* Host and Network endianess conversion */
/** @def HTONS
 *  Convert shorts to network endianness */
/** @def NTOHS
 *  Convert shorts to host endianness */
/** @def HTONL
 *  Convert longs to network endianness */
/** @def NTOHS
 *  Convert longs to host endianness */
#if BYTESWAP == 1
#   define HTONS(n) ((n & 0xff) << 8) | ((n & 0xff00) >> 8)
#   define NTOHS(n) HTONS(n)
#   define HTONL(n) ((n & 0xff) << 24) |       \
                    ((n & 0xff00) << 8) |      \
                    ((n & 0xff0000UL) >> 8) |  \
                    ((n & 0xff000000UL) >> 24)
#   define NTOHL(n) HTONL(n)
#else
#   define HTONS(n) (n)
#   define NTOHS(n) (n)
#   define HTONL(n) (n)
#   define NTOHL(n) (n)
#endif /* BYTESWAP */

/** Convert shorts to network endianness
 *  @param n The number to convert
 *  @return The converted data */
static inline uint16_t htons(uint16_t n) { return HTONS(n); }
/** Convert shorts to host endianness
 *  @param n The number to convert
 *  @return The converted data */
static inline uint16_t ntohs(uint16_t n) { return NTOHS(n); }
/** Convert longs to network endianness
 *  @param n The number to convert
 *  @return The converted data */
static inline uint32_t htonl(uint32_t n) { return HTONL(n); }
/** Convert longs to host endianness
 *  @param n The number to convert
 *  @return The converted data */
static inline uint32_t ntohl(uint32_t n) { return NTOHL(n); }

#ifndef USE_LOCAL_ADMIN_MAC
/** The MAC Address used by the device.
 *  This is a network local administered address */
static const uint8_t my_mac[6] = {0x00,0xF7,0xD1,0x00,0x00,0x01};
#else // USE_LOCAL_ADMIN_MAC
/** Another MAC Address which could be used by the device.
 *  This is a random address - not used unless you have trouble with
 *  local administered addresses. */
static const uint8_t my_mac[6] = {0x11,0x1A,0xC6,0x03,0x31,0x57};
#endif // USE_LOCAL_ADMIN_MAC
/** The IPv4 Address used by the device
 *  Change this to fit your network set up */
static const uint8_t my_ip[4] = {192,168,1,55};
/** The MAC Address used to identify broadcast addresses
 *  DO NOT CHANGE. */
static const uint8_t bcast_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

/* GLOBAL DATA ................................... */
static struct rx_pkt rxbuff[N_RX_BUFFERS] = {0};


/* FUNCTION DEFINITIONS .......................... */
void setup(void);
void loop(void);
/* Hardware Specific */
void eth_ISR(void);
void myputc(void* p, char c);
/* Debugging */
void hexdump(uint8_t *data, uint16_t len);
/* Ethernet Functions */
void eth_hdr_set(struct eth_hdr *hdr,
                 const uint8_t *src_mac, const uint8_t *dest_mac,
                 const uint16_t ethertype);
void eth_hdr_dump(struct eth_hdr *hdr);
/* ARP Functions */
void arp_process(struct arp_hdr *hdr);
void arp_gratuitous(void);
void arp_hdr_set(struct arp_hdr *hdr,
                 const uint16_t op,
                 const uint8_t *src_hw, const uint8_t *src_ip,
                 const uint8_t *dest_hw, const uint8_t *dest_ip);
void arp_hdr_dump(struct arp_hdr *hdr);
/* IPv4 Functions */
void ip4_hdr_set(struct ip4_hdr *hdr,
                 uint8_t tos, uint16_t total_len, uint16_t id, uint8_t flags,
                 uint16_t frag_offset, uint8_t ttl, uint8_t proto,
                 uint8_t *src_ip, uint8_t *dest_ip);
void ip4_hdr_dump(struct ip4_hdr *hdr);
/* ICMP Functions */
void icmp_process(struct eth_hdr *eth_hdr, struct ip4_hdr *ip4_hdr,
                  struct icmp_hdr *icmp_hdr);
void icmp_hdr_set(struct icmp_hdr *hdr, uint8_t type, uint8_t code, uint16_t id,
                  uint16_t seq, uint16_t payload_len);
void icmp_hdr_dump(struct icmp_hdr *hdr);
/* Checksum functions */
uint16_t inet_checksum16(uint16_t *data, uint16_t len);

#if VERBOSE > 0
#   define raw_dump(d,l) hexdump(d,l)
#   define verbose_printf(...) tfp_printf(__VA_ARGS__)
#else
#   define raw_dump(d,l)
#   define verbose_printf(...)
#endif



/* CODE .......................................... */

/** Program Entry */
int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

/** Program Setup/Initialisation */
void setup(void)
{
    /* Enable the UART Device... */
    sys_enable(sys_device_uart0);
    /* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
    uart_open(UART0,                    /* Device */
              1,                        /* Prescaler = 1 */
              UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */

    /* Print out a welcome message... */
    uart_puts(UART0,
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to Ethernet Example 1... \r\n"
        "\r\n"
        "Allow the user to \"Ping\" (ICMP Echo) to the device.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(UART0,myputc);

    /* Enable Ethernet... */
    tfp_printf("MAC address = %02X:%02X:%02X:%02X:%02X:%02X\r\n"
               "IP address = %d.%d.%d.%d\r\n",
                my_mac[0], my_mac[1], my_mac[2], my_mac[3], my_mac[4], my_mac[5],
                my_ip[0], my_ip[1], my_ip[2], my_ip[3]);

    sys_enable(sys_device_ethernet);
    ethernet_init(my_mac);
    ethernet_rx_enable(1);
    ethernet_tx_enable(1);

    /* Setup LED 0 to be the TX LED */
    gpio_function(4, pad_enet_led0);
    ethernet_led_mode(0, ethernet_led_mode_tx);

    /* Setup LED 1 to be the RX LED */
    gpio_function(5, pad_enet_led1);
    ethernet_led_mode(1, ethernet_led_mode_rx);

    /* Attach an interrupt to received packets */
    interrupt_attach(interrupt_ethernet, (uint8_t)interrupt_ethernet, eth_ISR);
    ethernet_enable_interrupt(MASK_ETH_IMR_RX_INT_MASK);

    /* Start handling Interrupts */
    interrupt_enable_globally();

    /* Wait until the ethernet link comes up */
    tfp_printf("Please plug in your Ethernet cable\r\n");
    while (!ethernet_is_link_up());
    tfp_printf("Ethernet Link Up\r\n");

    /* Send out a gratuitous ARP so people know where we are... */
    arp_gratuitous();
}

/** Program Loop */
void loop(void)
{
    static uint8_t i = 0;
    static struct eth_hdr *eth_hdr;
    size_t len;

    /* Loop through every packet */
    ++i;
    if (i == N_RX_BUFFERS) i = 0; /* Loop around */

    /* If the current index has a packet to be processed... */
    CRITICAL_SECTION_BEGIN
    	len = rxbuff[i].len;
    CRITICAL_SECTION_END

    if (len)
    {
        uint16_t ethertype;

        eth_hdr = PTR_AFTER(rxbuff[i].data, struct hw_hdr, struct eth_hdr);
        ethertype = ntohs(eth_hdr->ethertype_len);

        eth_hdr_dump(eth_hdr);

        /* If we have an ARP Packet ... */
        if (ethertype == ETHERTYPE_ARP)
        {
            struct arp_hdr *arp_hdr = PTR_AFTER(eth_hdr, struct eth_hdr, struct arp_hdr);

            tfp_printf("Got an ARP Packet\r\n");

            arp_process(arp_hdr);
        }

        /* If we have an IP Packet ... */
        else if (ethertype == ETHERTYPE_IP)
        {
            struct ip4_hdr *ip4_hdr = PTR_AFTER(eth_hdr, struct eth_hdr, struct ip4_hdr);

            ip4_hdr_dump(ip4_hdr);

            /* If we have an ICMP Packet ... */
            if (ip4_hdr->proto == IP_PROTO_ICMP)
            {
                struct icmp_hdr *icmp_hdr = PTR_AFTER(ip4_hdr, struct ip4_hdr, struct icmp_hdr);

                tfp_printf("Got an ICMP Packet\r\n");

                icmp_process(eth_hdr, ip4_hdr, icmp_hdr);
            }
        }

        /* Anything else ... */
        else
        {
            /* Do Nothing */
        }

        /* Mark the buffer as empty so another packet can be
           loaded into it */
        CRITICAL_SECTION_BEGIN
        	rxbuff[i].len = 0;
        CRITICAL_SECTION_END
    }

}

/* CODE - Hardware Specific ...................... */

/** Function used to handle received packets */
void eth_ISR(void)
{
    uint8_t isr;
    uint8_t i;

    isr = ETH->ETH_INT_STATUS;

    if (isr & MASK_ETH_IACK_RX_INT)
    {
        /* While there are packets to be received */
        while (ETH->ETH_NUM_PKT > 0)
        {
            /* Find a packet to store data into */
            for (i = 0; i < N_RX_BUFFERS; ++i)
            {
                /* This one is empty... store data in it */
                if (rxbuff[i].len == 0)
                {
                    verbose_printf("Putting packet into buffer %d\r\n",i);
                    ethernet_read(&(rxbuff[i].len), rxbuff[i].data);

                    break; /* Were finished with this packet */
                }
            }
            /* No space left */
            if (i == N_RX_BUFFERS)
            {
            	break;
            }
        } /* while(ETH->ETH_NUM_PKT > 0) */
    }
}

/** Function used to facilitate printf */
void myputc(void* p, char c)
{
    uart_write(p, (uint8_t)c);
}

/* CODE - Debugging .............................. */

/** Dump out a block of data in a human readable format
 *  @param data The pointer to the start of data
 *  @param len The number of bytes to display */
void hexdump(uint8_t *data, uint16_t len)
{
    uint16_t i = 0, j = 0;

    do
    {
        for (i = 0; i < 16; i++)
        {
            if (i+j < len) { tfp_printf("%02X ", data[i+j]); }
            else           { tfp_printf("   "); }
        }

        tfp_printf(" | ");

        for (i = 0; i < 16; i++)
        {
            if (i+j < len)
            {
                if (isalpha(data[i+j])) { tfp_printf("%c", data[i+j]); }
                else                    { tfp_printf("."); }
            }
        }
        tfp_printf("\r\n");

        j += 16;
    }
    while(j < len);
}

/* CODE - Ethernet Functions ..................... */

/** Setup an Ethernet header
 *  @param hdr A pointer to the header
 *  @param src_mac A pointer to the Source MAC Address
 *  @param dest_mac A pointer to the Destination MAC Address
 *  @param ethertype The Ethertype to use */
void eth_hdr_set(struct eth_hdr *hdr,
                 const uint8_t *src_mac, const uint8_t *dest_mac,
                 const uint16_t ethertype)
{
    hdr->dest_mac[0] = dest_mac[0];
    hdr->dest_mac[1] = dest_mac[1];
    hdr->dest_mac[2] = dest_mac[2];
    hdr->dest_mac[3] = dest_mac[3];
    hdr->dest_mac[4] = dest_mac[4];
    hdr->dest_mac[5] = dest_mac[5];
    hdr->src_mac[0] = src_mac[0];
    hdr->src_mac[1] = src_mac[1];
    hdr->src_mac[2] = src_mac[2];
    hdr->src_mac[3] = src_mac[3];
    hdr->src_mac[4] = src_mac[4];
    hdr->src_mac[5] = src_mac[5];
    hdr->ethertype_len = htons(ethertype);
}

/** Display the Ethernet header in a human readable format
 *  @param hdr A pointer to the header */
void eth_hdr_dump(struct eth_hdr *hdr)
{
#if VERBOSE > 0
    tfp_printf("+- ETHERNET --------------------+\r\n"
               "|  %02X : %02X : %02X : %02X : %02X : %02X  | (Source MAC)\r\n"
               "+-------------------------------+\r\n"
               "|  %02X : %02X : %02X : %02X : %02X : %02X  | (Destination MAC)\r\n"
               "+-------------------------------+\r\n"
               "| %5d (0x%04X) |              | (Ethertype)\r\n"
               "+-------------------------------+\r\n",
               hdr->src_mac[0], hdr->src_mac[1], hdr->src_mac[2],
               hdr->src_mac[3], hdr->src_mac[4], hdr->src_mac[5],
               hdr->dest_mac[0], hdr->dest_mac[1], hdr->dest_mac[2],
               hdr->dest_mac[3], hdr->dest_mac[4], hdr->dest_mac[5],
               ntohs(hdr->ethertype_len), ntohs(hdr->ethertype_len));
#endif
}

/* CODE - ARP Functions .......................... */

/** Process an ARP packet
 *  @param hdr A pointer to the header */
void arp_process(struct arp_hdr *hdr)
{
    uint8_t pkt[sizeof(struct hw_hdr) + sizeof(struct eth_hdr) + sizeof(struct arp_hdr)];
    struct hw_hdr  *hw_txhdr  = PTR_AT(pkt,struct hw_hdr);
    struct eth_hdr *eth_txhdr = PTR_AFTER(hw_txhdr,struct hw_hdr,struct eth_hdr);
    struct arp_hdr *arp_txhdr = PTR_AFTER(eth_txhdr,struct eth_hdr,struct arp_hdr);

    arp_hdr_dump(hdr);

    if (ntohs(hdr->oper) == ARP_OPER_ARP_REQUEST)
    {
        hw_txhdr->len = sizeof(struct hw_hdr) /*+ sizeof(struct eth_hdr)*/ + sizeof(struct arp_hdr);
        eth_hdr_set(eth_txhdr, my_mac, hdr->src_mac, ETHERTYPE_ARP);
        arp_hdr_set(arp_txhdr, ARP_OPER_ARP_REPLY, my_mac, my_ip, hdr->src_mac, hdr->src_ip);

        tfp_printf("Sending Reply ARP\r\n");
        eth_hdr_dump(eth_txhdr);
        arp_hdr_dump(arp_txhdr);

        ethernet_write(pkt, sizeof(struct hw_hdr) + sizeof(struct eth_hdr) + sizeof(struct arp_hdr));
    }

}

/** Send a gratuitous ARP to the network
 *
 *  Announce to the network the devices MAC address and IP combination */
void arp_gratuitous(void)
{
    uint8_t pkt[sizeof(struct hw_hdr) + sizeof(struct eth_hdr) + sizeof(struct arp_hdr)];
    struct hw_hdr  *hw_txhdr  = PTR_AT(pkt,struct hw_hdr);
    struct eth_hdr *eth_hdr = PTR_AFTER(hw_txhdr,struct hw_hdr,struct eth_hdr);
    struct arp_hdr *arp_hdr = PTR_AFTER(eth_hdr,struct eth_hdr,struct arp_hdr);

    hw_txhdr->len = sizeof(struct hw_hdr) /*+ sizeof(struct eth_hdr)*/ + sizeof(struct arp_hdr);
    eth_hdr_set(eth_hdr, my_mac, bcast_mac, ETHERTYPE_ARP);
    arp_hdr_set(arp_hdr, ARP_OPER_ARP_REPLY, my_mac, my_ip, my_mac, my_ip);

    tfp_printf("Sending gratuitous ARP\r\n");

    ethernet_write(pkt, sizeof(struct hw_hdr) + sizeof(struct eth_hdr) + sizeof(struct arp_hdr));
}

/** Setup an ARP header
 *  @param hdr A pointer to the header
 *  @param op The operation to use
 *  @param src_hw A pointer to the Source MAC Address
 *                or NULL to set all zero
 *  @param src_ip A pointer to the Source IP Address
 *                or NULL to set all zero
 *  @param dest_hw A pointer to the Destination MAC Address
 *                 or NULL to set all zero
 *  @param dest_ip A pointer to the Destination IP Address
 *                 or NULL to set all zero */
void arp_hdr_set(struct arp_hdr *hdr,
                 const uint16_t op,
                 const uint8_t *src_hw, const uint8_t *src_ip,
                 const uint8_t *dest_hw, const uint8_t *dest_ip)
{
    hdr->htype       = HTONS(1);
    hdr->ptype       = HTONS(ETHERTYPE_IP);
    hdr->hlen        = 6;
    hdr->plen        = 4;
    hdr->oper        = htons(op);

    if (src_hw)
    {
        hdr->src_mac[0] = src_hw[0];
        hdr->src_mac[1] = src_hw[1];
        hdr->src_mac[2] = src_hw[2];
        hdr->src_mac[3] = src_hw[3];
        hdr->src_mac[4] = src_hw[4];
        hdr->src_mac[5] = src_hw[5];
    }
    else
    {
        hdr->src_mac[0] = 0;
        hdr->src_mac[1] = 0;
        hdr->src_mac[2] = 0;
        hdr->src_mac[3] = 0;
        hdr->src_mac[4] = 0;
        hdr->src_mac[5] = 0;
    }

    if (src_ip)
    {
        hdr->src_ip[0] = src_ip[0];
        hdr->src_ip[1] = src_ip[1];
        hdr->src_ip[2] = src_ip[2];
        hdr->src_ip[3] = src_ip[3];
    }
    else
    {
        hdr->src_ip[0] = 0;
        hdr->src_ip[1] = 0;
        hdr->src_ip[2] = 0;
        hdr->src_ip[3] = 0;
    }

    if (dest_hw)
    {
        hdr->dest_mac[0] = dest_hw[0];
        hdr->dest_mac[1] = dest_hw[1];
        hdr->dest_mac[2] = dest_hw[2];
        hdr->dest_mac[3] = dest_hw[3];
        hdr->dest_mac[4] = dest_hw[4];
        hdr->dest_mac[5] = dest_hw[5];
    }
    else
    {
        hdr->dest_mac[0] = 0;
        hdr->dest_mac[1] = 0;
        hdr->dest_mac[2] = 0;
        hdr->dest_mac[3] = 0;
        hdr->dest_mac[4] = 0;
        hdr->dest_mac[5] = 0;
    }

    if (dest_ip)
    {
        hdr->dest_ip[0] = dest_ip[0];
        hdr->dest_ip[1] = dest_ip[1];
        hdr->dest_ip[2] = dest_ip[2];
        hdr->dest_ip[3] = dest_ip[3];
    }
    else
    {
        hdr->dest_ip[0] = 0;
        hdr->dest_ip[1] = 0;
        hdr->dest_ip[2] = 0;
        hdr->dest_ip[3] = 0;
    }
}

/** Display the ARP header in a human readable format
 *  @param hdr A pointer to the header */
void arp_hdr_dump(struct arp_hdr *hdr)
{
#if VERBOSE > 0
    tfp_printf("+- ARP ---------+---------------+ \r\n"
               "|         %5d (0x%04X)        | (Hardware type)\r\n"
               "+---------------+---------------+ \r\n"
               "|         %5d (0x%04X)        | (Protocol type)\r\n"
               "+---------------+---------------+ \r\n"
               "|     %3d       |      %3d      | (Hardware len, Protocol len)\r\n"
               "+---------------+---------------+ \r\n"
               "|         %5d (0x%04X)        | (Operation)\r\n"
               "+---------------+---------------+ \r\n"
               "|      %02X              %02X       | (Source MAC)\r\n"
               "|      %02X              %02X       | \r\n"
               "|      %02X              %02X       | \r\n"
               "+---------------+---------------+ \r\n"
               "|      %3d             %3d      | (Source IP)\r\n"
               "|      %3d             %3d      | \r\n"
               "+---------------+---------------+ \r\n"
               "|      %02X              %02X       | (Destination MAC)\r\n"
               "|      %02X              %02X       | \r\n"
               "|      %02X              %02X       | \r\n"
               "+---------------+---------------+ \r\n"
               "|      %3d             %3d      | (Destination IP)\r\n"
               "|      %3d             %3d      | \r\n"
               "+---------------+---------------+ \r\n",
               ntohs(hdr->htype), ntohs(hdr->htype),
               ntohs(hdr->ptype), ntohs(hdr->ptype),
               hdr->hlen,
               hdr->plen,
               ntohs(hdr->oper), ntohs(hdr->oper),
               hdr->src_mac[0], hdr->src_mac[1], hdr->src_mac[2],
                    hdr->src_mac[3], hdr->src_mac[4], hdr->src_mac[5],
               hdr->src_ip[0], hdr->src_ip[1], hdr->src_ip[2], hdr->src_ip[3],
               hdr->dest_mac[0], hdr->dest_mac[1], hdr->dest_mac[2],
                    hdr->dest_mac[3], hdr->dest_mac[4], hdr->dest_mac[5],
               hdr->dest_ip[0], hdr->dest_ip[1], hdr->dest_ip[2], hdr->dest_ip[3]);
#endif
}

/* CODE - IPv4 Functions ......................... */

/** Setup an IPv4 header
 *  @param hdr A pointer to the header
 *  @param tos The Type of Service
 *  @param total_len The overall length of the Datagram
 *  @param id The packet identifiation
 *  @param flags The flags to use
 *               bit 0: Must be 0
 *               bit 1: Don't Fragment (DF)
 *               bit 2: More Fragments (MF)
 *  @param frag_offset The fragmentation offset
 *  @param ttl The Time to live
 *  @param proto The IPv4 Protocol number
 *  @param src_ip A pointer to the Source IP Address
 *  @param dest_ip A pointer to the Destination IP Address */
void ip4_hdr_set(struct ip4_hdr *hdr,
                 uint8_t tos, uint16_t total_len, uint16_t id, uint8_t flags,
                 uint16_t frag_offset, uint8_t ttl, uint8_t proto,
                 uint8_t *src_ip, uint8_t *dest_ip)
{
    hdr->ver_ihl = (4 << 4) | (sizeof(struct ip4_hdr) >> 2);
    hdr->tos = tos;
    hdr->len = htons(total_len);
    hdr->id = htons(id);
    hdr->flags_fragoffset = htons((flags << 13) | (frag_offset * 0x1FFF));
    hdr->ttl = ttl;
    hdr->proto = proto;
    hdr->hdr_chksum = 0;
    hdr->src_ip[0] = src_ip[0];
    hdr->src_ip[1] = src_ip[1];
    hdr->src_ip[2] = src_ip[2];
    hdr->src_ip[3] = src_ip[3];
    hdr->dest_ip[0] = dest_ip[0];
    hdr->dest_ip[1] = dest_ip[1];
    hdr->dest_ip[2] = dest_ip[2];
    hdr->dest_ip[3] = dest_ip[3];

    hdr->hdr_chksum = inet_checksum16((uint16_t*)hdr, sizeof(struct ip4_hdr));
}

/** Display the IPv4 header in a human readable format
 *  @param hdr A pointer to the header */
void ip4_hdr_dump(struct ip4_hdr *hdr)
{
#if VERBOSE > 0
    tfp_printf("+- IPv4 ----+-----------+-----------+-----------+ \r\n"
               "| %2d  | %2d  |   0x%02X    |         %5d         | (Ver, Hdr len, ToS, Total len)\r\n"
               "+-----------+-----------+-----------+-----------+ \r\n"
               "|     %5d (0x%04X)    | %1d %1d %1d |     %5d     | (ID, Flags, Frag Offset)\r\n"
               "+-----------+-----------+-----------+-----------+ \r\n"
               "|    %3d    |    %3d    |         0x%04X        | (TTL, Proto, Header Checksum)\r\n"
               "+-----------+-----------+-----------+-----------+ \r\n"
               "|    %3d         %3d         %3d         %3d    | (Source IP) \r\n"
               "+-----------+-----------+-----------+-----------+ \r\n"
               "|     %3d        %3d         %3d         %3d    | (Destination IP) \r\n"
               "+-----------+-----------+-----------+-----------+ \r\n",
               hdr->ver_ihl >> 4, hdr->ver_ihl & 0xF, hdr->tos, ntohs(hdr->len),
               ntohs(hdr->id), ntohs(hdr->id),
                    ntohs(hdr->flags_fragoffset) >> 15 & 1,
                    ntohs(hdr->flags_fragoffset) >> 14 & 1,
                    ntohs(hdr->flags_fragoffset) >> 13 & 1,
                    ntohs(hdr->flags_fragoffset) & 0x1FFF,
               hdr->ttl, hdr->proto, ntohs(hdr->hdr_chksum),
               hdr->src_ip[0], hdr->src_ip[1], hdr->src_ip[2], hdr->src_ip[3],
               hdr->dest_ip[0], hdr->dest_ip[1], hdr->dest_ip[2], hdr->dest_ip[3]);
#endif
}

/* CODE - ICMP Functions ......................... */

/** Process an ICMP packet
 *  @param eth_hdr A pointer to the Ethernet header of the packet
 *  @param ip4_hdr A pointer to the IPv4 header of the packet
 *  @param icmp_hdr A pointer to the ICMP header of the packet */
void icmp_process(struct eth_hdr *eth_hdr, struct ip4_hdr *ip4_hdr,
                  struct icmp_hdr *icmp_hdr)
{
    uint8_t __attribute__((aligned(4))) pkt[sizeof(struct hw_hdr) +
                sizeof(struct eth_hdr) +
                sizeof(struct ip4_hdr) +
                sizeof(struct icmp_hdr) +
                256];
    struct hw_hdr  *hw_txhdr  = PTR_AT(pkt,struct hw_hdr);
    struct eth_hdr *eth_txhdr = PTR_AFTER(hw_txhdr,struct hw_hdr,struct eth_hdr);
    struct ip4_hdr *ip4_txhdr = PTR_AFTER(eth_txhdr,struct eth_hdr,struct ip4_hdr);
    struct icmp_hdr *icmp_txhdr = PTR_AFTER(ip4_txhdr,struct ip4_hdr,struct icmp_hdr);
    void * icmp_payload = PTR_AFTER(icmp_hdr,struct icmp_hdr,void);
    void * icmp_txpayload = PTR_AFTER(icmp_txhdr,struct icmp_hdr,void);
    uint16_t icmp_payload_len = ntohs(ip4_hdr->len) - sizeof(struct ip4_hdr) - sizeof(struct icmp_hdr);

    icmp_hdr_dump(icmp_hdr);

    verbose_printf("ICMP Payload (%d bytes):\r\n", icmp_payload_len);
    raw_dump(icmp_payload, icmp_payload_len);

    if (icmp_hdr->type == ICMP_TYPE_ECHO)
    {
        /* Reply to the Echo/Ping... */
        verbose_printf("Got ICMP Echo\r\n");

        hw_txhdr->len = /*sizeof(struct hw_hdr) + sizeof(struct eth_hdr) + */
                        sizeof(struct ip4_hdr) + sizeof(struct icmp_hdr) +
                        icmp_payload_len;
        eth_hdr_set(eth_txhdr, eth_hdr->dest_mac, eth_hdr->src_mac, ETHERTYPE_IP);
        ip4_hdr_set(ip4_txhdr,
                    0,
                    ntohs(ip4_hdr->len),
                    ntohs(ip4_hdr->id),
                    0x2,
                    0,
                    ip4_hdr->ttl,
                    ip4_hdr->proto,
                    (uint8_t *)my_ip,
                    ip4_hdr->src_ip);

        /* Copy over the payload from the source packet.
         * This can contain timing information to work out round trip times */
        memcpy(icmp_txpayload, icmp_payload, icmp_payload_len);

        icmp_hdr_set(icmp_txhdr, ICMP_TYPE_ECHOREPLY, 0, ntohs(icmp_hdr->id),
                     ntohs(icmp_hdr->seq), icmp_payload_len);

        tfp_printf("Sending ICMP Echo Reply\r\n");
        eth_hdr_dump(eth_txhdr);
        ip4_hdr_dump(ip4_txhdr);
        icmp_hdr_dump(icmp_txhdr);
        verbose_printf("ICMP Payload:\r\n");
        raw_dump(PTR_AFTER(icmp_txhdr,struct icmp_hdr,uint8_t), icmp_payload_len);

        ethernet_write(pkt,sizeof(struct hw_hdr) +
                sizeof(struct eth_hdr) +
                sizeof(struct ip4_hdr) +
                sizeof(struct icmp_hdr) + icmp_payload_len);
    }
}

/** Setup an IPv4 header
 *  @param hdr A pointer to the header
 *  @param type The ICMP type to use
 *  @param code The ICMP code to use
 *  @param id The ICMP Identification to use
 *  @param seq The ICMP Sequence Number to use
 *  @param payload_len The length of the data in the ICMP packet in bytes
 *  @warn Make sure that the payload is copied into the resulting packet
 *        BEFORE calling this function */
void icmp_hdr_set(struct icmp_hdr *hdr, uint8_t type, uint8_t code, uint16_t id,
                  uint16_t seq, uint16_t payload_len)
{
    hdr->type = type;
    hdr->code = code;
    hdr->chksum = 0;
    hdr->id = htons(id);
    hdr->seq = htons(seq);

    hdr->chksum = inet_checksum16((uint16_t*)hdr, sizeof(struct icmp_hdr)+payload_len);
}

/** Display the ICMP header in a human readable format
 *  @param hdr A pointer to the header */
void icmp_hdr_dump(struct icmp_hdr *hdr)
{
#if VERBOSE > 0
    tfp_printf("+- ICMP ----+-----------+-----------+-----------+ \r\n"
               "|    %3d    |    %3d    |         0x%04X        | (Type, Code, Checksum)\r\n"
               "+-----------+-----------+-----------+-----------+ \r\n"
               "|         %5d         |         %5d         | (ID, Sequence)\r\n"
               "+-----------+-----------+-----------+-----------+ \r\n",
               hdr->type, hdr->code, ntohs(hdr->chksum),
               htons(hdr->id), htons(hdr->seq));
#endif
}

/* CODE - Checksum Functions ..................... */

/** Generate a one's complemented one's complement sum checksum
 *  @param data A pointer to the data to checksum
 *  @param len The size of the data to be checksumed in bytes
 *  @returns A 16 bit checksum */
uint16_t inet_checksum16(uint16_t *data, uint16_t len)
{
    uint32_t sum = 0;
    uint16_t i = 0, carry, xsum;

    for (i = 0; i < len/sizeof(uint16_t); ++i)
        sum += data[i];
    xsum = sum & 0xFFFF;
    carry = (sum >> 16) & 0xFFFF;
    xsum += carry;

    return ~xsum;
}
