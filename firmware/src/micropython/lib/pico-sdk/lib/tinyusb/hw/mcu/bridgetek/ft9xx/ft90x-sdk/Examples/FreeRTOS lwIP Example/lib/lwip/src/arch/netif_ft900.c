/**
  @file netif_ft900.c
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

#include "netif_arch.h"

// Use TCP_MSS instead of hardcoded 1500
// Note that TCP_MSS can be lowered for RAM purposes
// All peers will understand this and only send small packets
#define USE_TCP_MSS_VALUE 1

// rlen < FT900_MAX_PACKET will always be true on 2nd try
// so the loopstart is useless
#define FIX_USELESS_LOOP 1

// Use task notification instead of polling
#define FIX_POLLING_BEHAVIOR 1



#define MIN(a,b) ((a<b)?a:b)
#define ARCH_HW_HLEN ETH_PAD_SIZE

#if USE_TCP_MSS_VALUE
#include "lwipopts.h"
#define FT900_MAX_PACKET ((TCP_MSS+40) + ETHERNET_WRITE_HEADER + ARCH_HW_HLEN + sizeof(uint32_t))
#else // USE_TCP_MSS_VALUE
#define FT900_MAX_PACKET (1500 + ETHERNET_WRITE_HEADER + ARCH_HW_HLEN + sizeof(uint32_t))
#endif // USE_TCP_MSS_VALUE

struct ifstats {
	u32_t rx_cnt;
	u32_t rx_err;
	u32_t tx_cnt;
	u32_t tx_err;
	u32_t overflow_cnt;
};

extern u32_t millis(void);

static struct ifstats  arch_ft900_stats = {0};

/* Reserve space to receive up to 2 packets at a time from the Ethernet interface.
 */
static uint8_t netif_rx_data[2 * FT900_MAX_PACKET];

static void arch_ft900_ethernet_ISR(void);

#if 0
void arch_ft900_stats_display()
{
	LWIP_PLATFORM_DIAG(("\nFT900\n\t"));
	LWIP_PLATFORM_DIAG(("rx:     %"U32_F"\n\t", (u32_t)arch_ft900_stats.rx_cnt));
	LWIP_PLATFORM_DIAG(("rx_err: %"U32_F"\n\t", (u32_t)arch_ft900_stats.rx_err));
	LWIP_PLATFORM_DIAG(("tx:     %"U32_F"\n\t", (u32_t)arch_ft900_stats.tx_cnt));
	LWIP_PLATFORM_DIAG(("tx_err: %"U32_F"\n\t", (u32_t)arch_ft900_stats.tx_err));
	LWIP_PLATFORM_DIAG(("rx_ov:  %"U32_F"\n\t", (u32_t)arch_ft900_stats.overflow_cnt));
}
#endif

#if LWIP_IGMP
err_t arch_ft900_igmp_mac_filter(struct netif *netif,
		const ip4_addr_t *group, enum netif_mac_filter_action action)
{
	(void)netif;
	(void)group;

	if (action == NETIF_DEL_MAC_FILTER)
	{
		ethernet_accept_multicast(0);
	}
	else if (action == NETIF_ADD_MAC_FILTER)
	{
		ethernet_accept_multicast(1);
	}

	return ERR_OK;
}
#endif // LWIP_IGMP

/* INITIALISATION */
/*
 * This is the initialization function that should be passed to netif_add where it will be run.
 * Ethernet must be enabled by sys_enable(sys_device_ethernet)
 * before initialising lwIP.
 */
err_t arch_ft900_init(struct netif *netif)
{
	LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_init: FT900 Driver init called\r\n"));

	// Setup lwIP arch interface.
	netif->output = etharp_output;
	netif->linkoutput = arch_ft900_link_output;
#if USE_TCP_MSS_VALUE
	netif->mtu = TCP_MSS + 40;
#else // USE_TCP_MSS_VALUE
	netif->mtu = 1500;
#endif // USE_TCP_MSS_VALUE
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
	netif->input = ethernet_input;
#if LWIP_IGMP
	netif->igmp_mac_filter = arch_ft900_igmp_mac_filter;
#endif // LWIP_IGMP

	/* Call Ethenet hardware driver initialisation routine. */
	ethernet_init(netif->hwaddr);

	// Enable options for the ethernet interface.
	ethernet_rx_enable(1);
	ethernet_tx_enable(1);
	ethernet_enable_crc(1);
	ethernet_enable_pad(1);
	ethernet_enable_full_duplex(1);

	/* Setup LED 0 to be the TX LED */
	gpio_function(4, pad_enet_led0);
	ethernet_led_mode(0, ethernet_led_mode_tx);
	//ethernet_led_mode(0, ethernet_led_mode_link);
	/* Setup LED 1 to be the RX LED */
	gpio_function(5, pad_enet_led1);
	ethernet_led_mode(1, ethernet_led_mode_rx);
	//ethernet_led_mode(1, ethernet_led_mode_spd);

	/* Configure hardware interrupts. */
	interrupt_attach(interrupt_ethernet, (uint8_t)interrupt_ethernet, arch_ft900_ethernet_ISR);
	/* Enable interrupts including receive interrupts. */
	arch_ft900_recv_on();

	LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_init: FT900 Driver init finished\r\n"));

	return ERR_OK;
}

inline void arch_ft900_recv_on()
{
	ETH->ETH_INT_ENABLE = MASK_ETH_IMR_RX_ERR_MASK |
			MASK_ETH_IACK_FIFO_OV |
			MASK_ETH_IACK_TX_EMPTY |
			MASK_ETH_IACK_TX_ERR |
			MASK_ETH_IACK_RX_INT;
}

inline void arch_ft900_recv_off()
{
	ETH->ETH_INT_ENABLE = MASK_ETH_IMR_RX_ERR_MASK |
			MASK_ETH_IACK_FIFO_OV |
			MASK_ETH_IACK_TX_EMPTY |
			MASK_ETH_IACK_TX_ERR;
}

/* OUTPUT */
/*
 * Called when a raw link packet is ready to be transmitted. This function should not add any more
 * headers. You must set netif->linkoutput to the address of this function.
 */

err_t arch_ft900_link_output(struct netif *netif, struct pbuf *pfirst)
{
	/* Source location in pbuf payload for streamout.l/memcpy.b */
	register uint32_t *src;
	/* Destination location in pbuf payload for memcpy.b */
	register uint8_t *dest;
	/* Destination register for Tx FIFO for streamout.l */
	register volatile uint32_t *data_reg = &ETH->ETH_DATA;
	/* Register used to pass the size of the payload to streamout.l/memcpy.b */
	register uint32_t size;
	/* Current pbuf when combining pbufs. */
	struct pbuf *pcur;
	/* New pbuf as destination of combined pbufs. */
	struct pbuf *pnew;
	/* Total length of new pbuf payload. */
	uint32_t tot_len;

	/* Check to see if transmission is enabled. */
	if ((ETH->ETH_TX_CNTL & MASK_ETH_TCR_TX_ENABLE) == 0)
	{
		/* Tranmit is disabled. */
		return -1;
	}

	/* To support pbuf chains when sending the loop is used to add
	 * multiple pbuf payloads into the transmit buffer. When the packet has
	 * been formed then it can be transmitted. The tot_len and len members
	 * of the pbuf structure determine whether a pbuf is a partial packet.
	 */
	LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: start\n"));
	do
	{
		/* Wait for any previous transmits or MII transactions to complete. */
		while (ETH->ETH_TR_REQ & MASK_ETH_TRR_NEXTX);

		LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: tot_len: %"U16_F"\n", pfirst->tot_len));

		/* Each packet has 2 free bytes at the start of the first
		 * pbuf in the chain. This is primarily to align the payload
		 * to a 32 bit boundary. It works well for us as we can have
		 * an aligned buffer to stream into the ethernet transmit
		 * buffer directly.
		 * The payload of the first pbuf has a word free (ARCH_HW_HLEN)
		 * where the packet length word is written.
		 */

		/* Calculate Ethernet frame length. This is the total size of the
		 * packet minus the length of the ethernet headers and the
		 * packet length word. Aligned to a longword boundary for
		 * Ethernet hardware. */
		size = pfirst->tot_len - ETHERNET_WRITE_HEADER - ARCH_HW_HLEN;
		size = ((size + 3) & (~3));
		*((uint16_t *)pfirst->payload) = size;

		/* Stream out the payloads from each pbuf in the chain.
		 * Start at the first. */
		pcur = pfirst;

		/* Holder for an aligned pbuf. If the length of the payload of a
		 * pbuf is not a multiple of 4 bytes then it cannot be streamed into
		 * the transmit buffer as the buffer requires 32-bit accesses.
		 * Therefore anything other than aligned writes will result in gaps
		 * in the data between pbuf payloads! */
		pnew = NULL;

		/* Iterate trough the pbuf chain. */
		while (1)
		{
			/* Setup registers for stream instruction.
			 * Source data is the start of the pbuf payload. */
			src = (uint32_t*)pcur->payload;
			/* Size is the length of this pbuf'f payload. */
			size = pcur->len;

			LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: pbuf chain size %"U32_F" %p\n", size, src));

			/* If the length of pbuf is not a multiple of 4 it cannot be streamed.
			 * The ETH_DATA register will only receive 32-bit writes correctly.
			 */
			if (size & 3)
			{
				/* If, however, this is the last pbuf in a packet then we can
				 * send pad bytes after to ensure a 32-bit write at the end. The
				 * packet size in the frame header ensures the hardware knows
				 * when to stop transmitting.
				 */
				if (pcur->next != NULL)
				{
					LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: Alignment... copying remaining chain\n"));

					/* Combine this and remaining pbufs into one buffer.
					 * Do not use the pbuf_coalesce function as this will
					 * free the original pbufs and cause issues.
					 * The total_len for the packet is used to create a new
					 * pbuf large enough for all these other pbufs.
					 */
					pnew = pbuf_alloc(PBUF_RAW, pcur->tot_len, PBUF_RAM);
					if (pnew == NULL)
					{
						/* Out of memory for creating a new pbuf. */
						/* The original packet will still be sent.
						 * It will be picked up by a network analyser and
						 * reported as malformed.
						 */
						LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: copy failed\n"));
						LINK_STATS_INC(link.memerr);
					}
					else
					{
						/* Copy this packet and all subsequent packets into
						 * the new pbuf. Technically we could stop if any of
						 * these pbufs ends on a 32-bit boundary but this
						 * would require additional complexity without a
						 * massive increase in speed.
						 */
						dest = pnew->payload;
						tot_len = 0;

						do
						{
							src = (uint32_t*)pcur->payload;
							size = pcur->len;

							/* Use a memcpy.b op code to copy bytewise from the
							 * original pbuf to the new pbuf.
							 */
							__asm__("memcpy.b %0,%1,%2" : :"r"(dest), "r"(src), "r"(size));

							/* Adjust the new pbuf with the sizes of the original
							 * pbufs. The original pbufs are not modified or freed
							 * as the caller will take care of that.
							 */
							tot_len += size;
							dest += size;
							LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: copied size %"U32_F" tot_len %"U32_F"\n", size, tot_len));
							/* Move to the next pbuf in the chain of original pbufs. */
							if (pcur->next == NULL) break;
							pcur = pcur->next;
						} while (pcur);

						/* Finalise the important bits of the new pbuf. */
						src = (uint32_t*)pnew->payload;
						size = tot_len;

						LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: new pbuf size now %"U32_F"\n", size));
					}
				}
				size = ((size + 3) & (~3));
			}

			/* Stream the aligned buffer to the ethernet transmit buffer.
			 * The settings for lwIP in lwipopts.h prevent us overflowing
			 * the 2 kB buffer.
			 */
			LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: streaming %"U32_F"\n", size));
			__asm__("streamout.l %0,%1,%2" : :"r"(data_reg), "r"(src), "r"(size));

			/* Send the packet! */
			ETH->ETH_TR_REQ = MASK_ETH_TRR_NEXTX;

			if (size > 0)
			{
				LINK_STATS_INC(link.xmit);
			}

			/* If we have created a new aligned buffer then free it now.
			 * Remember we are leaving the original pbuf chain intact.
			 */
			if (pnew != NULL)
			{
				pbuf_free(pnew);
				break;
			}

			/* Hard end of chain detected - catch this case. */
			if (pcur->next == NULL)
			{
				break;
			}

			/* Move to next pbuf in chain. */
			pcur = pcur->next;
			LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: next\n"));
		}

		// Move to next packet in chain. There probably isn't one.
		pfirst = pcur->next;

	} while (pfirst != NULL);

	LWIP_DEBUGF(NETIF_DEBUG, ("arch_ft900_link_output: sent\n"));

	return 0;
}

static void arch_ft900_ethernet_ISR(void)
{
	uint8_t isr;

	/* Get current interrupt active bitmap. */
	isr = ETH->ETH_INT_STATUS;
	/* Mask out disabled interrupts. */
	isr &= ETH->ETH_INT_ENABLE;

	if (isr & MASK_ETH_IACK_RX_ERR)
	{
		arch_ft900_stats.rx_err++;
		ETH->ETH_INT_STATUS = MASK_ETH_IACK_RX_ERR;
	}

	if (isr & MASK_ETH_IACK_TX_ERR)
	{
		arch_ft900_stats.tx_err++;
		ETH->ETH_INT_STATUS = MASK_ETH_IACK_TX_ERR;
	}

	if (isr & MASK_ETH_IACK_FIFO_OV)
	{
		arch_ft900_stats.overflow_cnt++;
		ETH->ETH_INT_STATUS = MASK_ETH_IACK_FIFO_OV;
	}

	if (isr & MASK_ETH_IACK_RX_INT)
	{
		/* Maximum length of data which can be put into
		 * the receive buffer. */
		uint32_t rlen = sizeof(netif_rx_data);
		/* Pointer to the start of the receive buffer. */
		register uint32_t *dst = (uint32_t *)netif_rx_data;
		/* Rx FIFO read. */
		register volatile uint32_t *data_reg = &ETH->ETH_DATA;
		/* Variable to receive first longword of packet which
		 * contains the packet length. This is a register.
		 */
		register uint32_t w0;
		/* Number of packets waiting in hardware Rx FIFO. */
		uint32_t c0;

		/* Collect any packets in Rx FIFO into RAM. */

#if FIX_USELESS_LOOP
#else // FIX_USELESS_LOOP
		/* This utilises labels and gotos to improve speed. */
		loopstart:

		/* We require enough room to store a maximum sized packet
		 * in the receive buffer. This is calculated as 1500 bytes
		 * of payload, an ethernet header, the size of the packet
		 * length and an end of packets marker.
		 */
		if (rlen < FT900_MAX_PACKET)
		{
			//tfp_printf("rlen(%d) < FT900_MAX_PACKET(%d)\r\n", rlen, FT900_MAX_PACKET);

			/* No room left for a full packet.
			 * It will get read in the next time.
			 */
			goto loopstop;
		}
#endif // FIX_USELESS_LOOP

		/* How many packets have been received and are
		 * waiting in the Rx FIFO.
		 */
		c0 = ETH->ETH_NUM_PKT;

		/* No packets waiting. */
		if (c0 == 0) goto loopstop;

		/* Read first dword of packet. This contains the length
		 * of the first packet.
		 */
		w0 = *data_reg;
		/* Copy first dword to destination buffer. */
		*dst = w0;
		/* Move pointer to next dword in destination buffer. */
		dst++;

		/* Calculate length of packet (excluding first dword). */
		w0 &= 0x07ff;
		w0 -= 1;
		w0 &= (~3);

		/* Get the remaining packet data in using the streamin.l opcode. */
		__asm__("streamin.l %0,%1,%2" : :"r"(dst), "r"(data_reg), "r"(w0));

		/* Update destination pointer to point to the end
		 * of the loaded packet. The typecasts are used to
		 * reduce the outputted code size by avoiding a shift.
		 */
		dst = (uint32_t *)(((uint8_t *)dst) + w0);

		//tfp_printf("w0=%d\r\n", w0);

		/* Reduce available space for new packets. */
		rlen -= w0;
		/* Read any more packets available. */

#if FIX_USELESS_LOOP
		// Note loopstartif condition will always fail on the 2nd try (rlen < FT900_MAX_PACKET)
		// Just exit the loop my goodness gracious
#else // FIX_USELESS_LOOP
		goto loopstart;
#endif // FIX_USELESS_LOOP

		loopstop:

		/* Write end of packets marker in destination buffer. */
		*dst = 0xFFFFFFFF;

		/* Turn off receive interrupts. It will be turned on again
		 * when the packets are processed.
		 */
		arch_ft900_recv_off();

#if FIX_POLLING_BEHAVIOR
		net_packet_available();
#endif //FIX_POLLING_BEHAVIOR
	}

	if (isr & MASK_ETH_IACK_TX_EMPTY)
	{
		arch_ft900_stats.tx_cnt++;
		ETH->ETH_INT_STATUS = MASK_ETH_IACK_TX_EMPTY;
	}
}

err_t arch_ft900_tick(struct netif *netif)
{
	err_t err = ERR_OK;
	struct pbuf *pnew;
	struct pbuf *pfirst = NULL;
	struct pbuf *plast = NULL;

	/* Variable to read first longword of packet
	 * to calculate the length of the packet.
	 */
	uint32_t w0;

	/* Aligned length for memcpy.l */
	register uint32_t len32;
	/* Destination pointer for memcpy.l */
	register uint32_t *dst;
	/* Source location in receive buffer for memcpy.l */
	register uint32_t *src;


	/* Split all packets received in the receive buffer
	 * into a pbuf queue which we can process with lwIP.
	 * Only when receive interrupts are turned off can we
	 * process the receive buffer.
	 */
	if ((ETH->ETH_INT_ENABLE & MASK_ETH_IACK_RX_INT) == 0)
	{
		src = (uint32_t *)netif_rx_data;

		do
		{
			arch_ft900_stats.rx_cnt++;

			/* Get first longword from buffered packets.*/
			w0 = *((uint32_t *)src);

			if (w0 == 0xFFFFFFFF)
			{
				break;
			}

			/* Length from data register is complete frame from destination
			 * address to CRC32 inclusive PLUS 2 bytes of length itself.
			 */
			len32 = (w0 & 0x0fff);
			len32 = (len32 + 3) & (~3);

			if (len32 == 0)
			{
				break;
			}

			/* Obtain a buffer to receive the packet and checksum. */
			pnew = pbuf_alloc(PBUF_RAW, len32, PBUF_RAM);
			if (pnew != NULL)
			{
				/* Get pointer to start of payload. */
				dst = (uint32_t*)pnew->payload;

				/* Copy from receive buffer to aligned pbuf payload. */
				__asm__("memcpy.l %0,%1,%2" : :"r"(dst), "r"(src), "r"(len32));

				/* Add the new pbuf to the end of a queue.
				 * The queue is broken into separate pbufs before processing.
				 */
				if (plast != NULL)
				{
					plast->next = pnew;
				}

				if (pfirst == NULL)
				{
					pfirst = pnew;
				}
				plast = pnew;

				LINK_STATS_INC(link.recv);
			}
			else
			{
				/* No memory to malloc a new pbuf of the required size. */
				LWIP_DEBUGF(NETIF_DEBUG, ("arch_tick: Out of memory to receive\n"));
				LINK_STATS_INC(link.memerr);
				break;
			}

			src = (uint32_t *)(((uint8_t *)src) + len32);

		} while (1);

		/* Turn on interrupts as we have processed all packets waiting. */
		arch_ft900_recv_on();
	}

	/* Now process the packets we have received.*/
	while (pfirst)
	{
		pnew = pfirst->next;
		/* Break pbuf queue after this entry. */
		pfirst->next = NULL;
		/* Pass it into LWIP */
		err = netif->input(pfirst, netif);
		/* Move to next pbuf. */
		pfirst = pnew;
	}

	return err;
}

u32_t sys_jiffies(void)
{
	static u32_t jiffies = 0;
	jiffies += 1 + millis() / 10;
	return jiffies;
}

#if defined(NO_SYS) && (NO_SYS==0)
#else
u32_t sys_now(void)
{
	return millis();
}
#endif
