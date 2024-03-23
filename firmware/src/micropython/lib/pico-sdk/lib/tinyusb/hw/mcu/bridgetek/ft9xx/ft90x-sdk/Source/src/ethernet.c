/**
    @file

    @brief
    Driver for the FT900 ethernet peripheral

    
**/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek") subject to the licence terms set out
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


/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ft900.h>
#include <ft900_sys.h>
#include <ft900_interrupt.h>
#include <ft900_gpio.h>
#include "ft900_eth.h"
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/

/**
 *  @brief Allowed modes for auto-negotiate.
 *	@details This is sent in the advertise phase with the link partner.
 *		Set to zero if auto-negotiation is not to be used and instead a fixed
 *		mode is to be selected with ETHERNET_MODE below.
 */
#define ETHERNET_AUTO_NEG_ALLOW (MII_ACR_10HALF | MII_ACR_10FULL | MII_ACR_100HALF | MII_ACR_100FULL)

/**
 *  @brief Preset modes operation.
 *	@details If auto-negotiation is not to be used then this is the mode which is
 *		to be used instead.
 */
#define ETHERNET_MODE (MII_BMCR_SPEED100 | MII_BMCR_FULLDUPLEX)

/**
 *  @brief PHY address hardwired into device.
 */
#define ETHERNET_PHY_ADDRESS    0x17

/**
 *  @brief Use a threshold to start a packet transmit.
 *  @details This offers a slight speed advantage as long as data is streamed to
 *  the transmit buffer sufficiently fast.
 */
/* 1 = use threshold to start packet transmit */
#define USE_THR      0        

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

/** @brief ethernet_hook_interrupt install interrupt service routine for the ethernet.
 *             The default priority is 3
 *
 *  @param none
 *  @returns nothing
 */

void ethernet_enable_interrupt(uint8_t mask)
{
    ETH->ETH_INT_STATUS = mask;
    ETH->ETH_INT_ENABLE = mask;
}


/** @brief read the content of an MII register
 * 
 *  @param reg register to read
 *  @returnns the content of requested register 
 */

uint16_t ethernet_mii_read(uint8_t reg)
{
	uint16_t res;

	CRITICAL_SECTION_BEGIN
    /* wait for any previous transmit to complete */
    while (ETH->ETH_TR_REQ) {};

    ETH->ETH_MNG_CNTL = (reg << BIT_ETH_MCR_RGAD) | MASK_ETH_MCR_START;

    while((ETH->ETH_INT_STATUS & MASK_ETH_IACK_MD_INT) == 0) {};

    ETH->ETH_INT_STATUS = MASK_ETH_IACK_MD_INT;
    res = ETH->ETH_MNG_RX0;
    ETH->ETH_MNG_CNTL = 0;
    CRITICAL_SECTION_END

	return res;
}

/** @brief write a value to an MII register
 *  @param reg register to write to
 *  @param v value to write to requested register
 *  @returns 0 on success, -1 on error
 */

int ethernet_mii_write(uint8_t reg, uint16_t v)
{
    if ((ETH->ETH_INT_STATUS & MASK_ETH_IACK_MD_INT) != 0)
	{
        return -1;
	}

	CRITICAL_SECTION_BEGIN

	ETH->ETH_MNG_TX0 = v;
    ETH->ETH_MNG_CNTL = (reg << BIT_ETH_MCR_RGAD) | MASK_ETH_MCR_START | MASK_ETH_MCR_WRITE;

    while((ETH->ETH_INT_STATUS & MASK_ETH_IACK_MD_INT) == 0) {};

    ETH->ETH_INT_STATUS = MASK_ETH_IACK_MD_INT;
    ETH->ETH_MNG_CNTL = 0;
    CRITICAL_SECTION_END

    return 0;
}

/** @brief set the ethernet peripheral in promiscuous mode 
 *  @param flag 0 - disable promiscuous mode,  1 - set promiscuous mode
 *  @returns nothing
 */

void ethernet_set_promiscuous(int flag)
{
	uint8_t mode;

	mode = (ETH->ETH_RX_CNTL) & (~MASK_ETH_RCR_RX_MEM_SIZE);

	if (flag)
	{
		mode |= MASK_ETH_RCR_PRMS_MODE;
	}
    else
    {
        mode &= (~MASK_ETH_RCR_PRMS_MODE);
    }

	ETH->ETH_RX_CNTL = mode;
}

/** @brief set the ethernet peripheral to accept multicast
 *  @param flag 0 - disable accept multicast mode,  1 - set accept multicast mode
 *  @returns nothing
 */

void ethernet_accept_multicast(int flag)
{
	uint8_t mode;

	mode = (ETH->ETH_RX_CNTL) & (~MASK_ETH_RCR_RX_MEM_SIZE);

	if (flag)
	{
		mode |= MASK_ETH_RCR_ACC_MULTI;
	}
    else
    {
        mode &= (~MASK_ETH_RCR_ACC_MULTI);
    }

	ETH->ETH_RX_CNTL = mode;
}

/** @brief set the ethernet peripheral to transmit in full duplex mode
 *  @param flag 0 - disable duplex mode,  1 - set enable duplex mode
 *  @returns nothing
 */

void ethernet_enable_full_duplex(int flag)
{
	uint8_t mode;

	mode = (ETH->ETH_TX_CNTL) & (~MASK_ETH_TCR_TX_MEM_SIZE);

	if (flag)
	{
		mode |= MASK_ETH_TCR_DUPLEX_MODE;
	}
    else
    {
        mode &= (~MASK_ETH_TCR_DUPLEX_MODE);
    }

	ETH->ETH_TX_CNTL = mode;
}

/** @brief set the ethernet peripheral to handle CRCs
 *  @param flag 0 - disable crc mode,  1 - set enable crc mode
 *  @returns nothing
 */

void ethernet_enable_crc(int flag)
{
	uint8_t mode;

	mode = (ETH->ETH_TX_CNTL) & (~MASK_ETH_TCR_TX_MEM_SIZE);

	if (flag)
	{
		mode |= MASK_ETH_TCR_CRC_ENABLE;
	}
    else
    {
        mode &= (~MASK_ETH_TCR_CRC_ENABLE);
    }

	ETH->ETH_TX_CNTL = mode;
}

/** @brief set the ethernet peripheral to handle padding
 *  @param flag 0 - disable pad mode,  1 - set enable pad mode
 *  @returns nothing
 */

void ethernet_enable_pad(int flag)
{
	uint8_t mode;

	mode = (ETH->ETH_TX_CNTL) & (~MASK_ETH_TCR_TX_MEM_SIZE);

	if (flag)
	{
		mode |= MASK_ETH_TCR_PAD_ENABLE;
	}
    else
    {
        mode &= (~MASK_ETH_TCR_PAD_ENABLE);
    }

	ETH->ETH_TX_CNTL = mode;
}

/** @brief enable or disable the ethernet transmitter
 *  @param flag 0 - disable transmitter, 1 - enable transmitter
 *  @returns nothing
 */

void ethernet_tx_enable(int flag)
{
	uint8_t mode = 0;

	CRITICAL_SECTION_BEGIN

	mode = (ETH->ETH_TX_CNTL) & (~MASK_ETH_TCR_TX_MEM_SIZE);

	if (flag)
	{
        mode |=	MASK_ETH_TCR_TX_ENABLE;
	}
	else
	{
		// Clear any outstanding transmit interrupts.
		ETH->ETH_INT_STATUS = (MASK_ETH_IACK_TX_ERR | MASK_ETH_IACK_TX_EMPTY);
       	mode &= (~MASK_ETH_TCR_TX_ENABLE);
	}

    ETH->ETH_TX_CNTL = mode;

    CRITICAL_SECTION_END
}

/** @brief enable or disable the ethernet receiver
 *  @param flag 0 - disable receiver, 1 - enable receiver
 *  @returns nothing
 */

void ethernet_rx_enable(int flag)
{
	uint8_t mode;

	CRITICAL_SECTION_BEGIN

	mode = (ETH->ETH_RX_CNTL) & (~MASK_ETH_RCR_RX_MEM_SIZE);

	if (flag)
	{
		mode |= MASK_ETH_RCR_RX_ENABLE;
	}
    else
    {
        ETH->ETH_INT_STATUS = (MASK_ETH_IACK_RX_INT | MASK_ETH_IACK_FIFO_OV |
        		MASK_ETH_IACK_RX_ERR);    /* reset rxer,fov and rxint */
        mode &= (~MASK_ETH_RCR_RX_ENABLE);
    }

	ETH->ETH_RX_CNTL = mode;

	CRITICAL_SECTION_END
}

/** @brief Set the ethernet MAC
 *  @param mac pointer to a six byte array containing MAC
 *  @returns nothing
 */

void ethernet_set_mac(const uint8_t *mac)
{
    int    i;

    for(i = 0; i < 6; i++)
        ETH->ETH_ADDR[i] = mac[i];
}

/** @brief Return the ethernet link status 
 *  @params none
 *  @returns non-zero - link up, 0 - link is not up.
 */
int ethernet_is_link_up(void)
{
	int link_stat = 0;

	if (sys_check_ft900_revB()) //90x series rev B
	{
		uint16_t reg;
		/* MII register 1. bit  0x04 = valid link established bit */
		reg = ethernet_mii_read(MII_BMSR);

		// Return positive if link and auto-negotiate complete set.
		link_stat = ((reg & MII_BMSR_LINK) == MII_BMSR_LINK)?1:0;
	}
	else
	{
		link_stat = (ETHPHY->MISC & MASK_ETHPHY_MISC_LINK_STATUS)?1:0;
	}
	return link_stat;
}

/** @brief Return the ethernet link speed
 *  @params none
 *  @returns 10 - 10 Mb/sec, 100 - 100 Mb/sec.
 */
int ethernet_link_speed(void)
{
	int speed = 10;

	if (sys_check_ft900_revB()) //90x series rev B
	{
		/* MII register 1. bit  0x04 = valid link established bit */
		if (ethernet_mii_read(MII_BMCR) & MII_BMCR_SPEED100)
			speed = 100;
	}
	else
	{
		if (!(ETHPHY->MISC & MASK_ETHPHY_MISC_LINK_SPEED_LED))
			speed = 100;
	}

    return speed;
}

/** @brief Return the ethernet duplex mode
 *  @params none
 *  @returns 0 - half duplex, 1 - full duplex.
 */
int ethernet_link_duplex(void)
{
	int duplex = 0;
	if (sys_check_ft900_revB()) //90x series rev B
	{
		/* MII register 1. bit  0x04 = valid link established bit */
		if (ethernet_mii_read(MII_BMCR) & MII_BMCR_FULLDUPLEX)
			duplex = 1;
	}
	else
	{
		if (!(ETHPHY->MISC & MASK_ETHPHY_MISC_FULL_DUPLEX_LED))
			duplex = 1;
	}


    return duplex;
}

/** @brief initialise the ethernet hardware.
 *  @param mac pointer to a six byte array containing MAC
 */
void ethernet_init(const uint8_t *mac)
{
	uint16_t bmcr;
	/* Reset the PHY */
    SYS->MSC0CFG |= MASK_SYS_MSC0CFG_MAC_RESET_PHY;
    while(SYS->MSC0CFG & MASK_SYS_MSC0CFG_MAC_RESET_PHY) {};

    /* Setup the PHY address */
	ETHPHY->MISC = (ETHPHY->MISC & ~MASK_ETHPHY_MISC_PHYAD) |
                   (ETHERNET_PHY_ADDRESS << BIT_ETHPHY_MISC_PHYAD);
                   
    /* Point the MII to the PHY */
    ETH->ETH_MNG_ADDR = ETHERNET_PHY_ADDRESS;

    /* Set the MII clock divider */
	/* The minimum value of this must ensure a 400 ns minimum period.
	 *  Fmdc = Fclk/(2*(ETH_MNG_DIV + 1))
	 * So, for an Fmdc of 2.5 MHz given a 100 MHz Fclk, the minimum
	 * value to write to this register is 19dec.
	 * There is no maximum value and the default is 128.
	 */
    ETH->ETH_MNG_DIV = 19;

    /* Disable both receiver and transmitter */
    ETH->ETH_RX_CNTL = MASK_ETH_RCR_RESET_FIFO |
    		MASK_ETH_RCR_BAD_CRC;
    ETH->ETH_TX_CNTL = 0;

    /* Reset the PHY */
    ethernet_mii_write(MII_BMCR, MII_BMCR_RESET);
    while (ethernet_mii_read(MII_BMCR) & MII_BMCR_RESET)
    {
    	delayms(50);
    };

    /* Setup the PHY. */
#if ETHERNET_AUTO_NEG_ALLOW != 0

	/* Setup auto-negotiation advertisement register. */
	ethernet_mii_write(MII_ADVERTISE,
			MII_ACR_CSMA
			| ETHERNET_AUTO_NEG_ALLOW
			| MII_ACR_LPACK);

    /* Restart the auto-negotiation. */
	bmcr = ethernet_mii_read(MII_BMCR);
	bmcr &= (~MII_BMCR_ISOLATE);
	ethernet_mii_write(MII_BMCR, bmcr | MII_BMCR_RESET_AUTONEG | MII_BMCR_AUTO_NEG_EN);

#else
	ethernet_mii_write(MII_BMCR,
		ETHERNET_MODE // Connection mode.
		);
#endif
	delayms(1);

	/* Setup defaults for handling packets */
    ethernet_enable_full_duplex(1);
    ethernet_enable_crc(1);
    ethernet_enable_pad(1);
    ethernet_set_promiscuous(0);
    ethernet_accept_multicast(0);

    /* Set MAC address */
    ethernet_set_mac(mac);
}

/** @brief Set the mode of the led 
 *  @param led The number of the led (0 or 1)
 *  @param mode The mode which the led will be in
 */
void ethernet_led_mode(uint8_t led, ethernet_led_mode_t mode)
{
    if (led == 0)
    {
        ETHPHY->MISC = (ETHPHY->MISC & (~MASK_ETHPHY_MISC_LED0_SEL)) |
        		((uint32_t)(mode) << BIT_ETHPHY_MISC_LED0_SEL);
    }
    else if (led == 1)
    {
        ETHPHY->MISC = (ETHPHY->MISC & (~MASK_ETHPHY_MISC_LED1_SEL)) |
        		((uint32_t)(mode) << BIT_ETHPHY_MISC_LED1_SEL);
    }
}

/** @brief Poll the ethernet peripheral for the receptition of a single packet
 *  @param buf pointer to buffer to store the received packet. This MUST be
 *  	32 bit aligned.
 *  @param blen size of reception buffer
 *  @returns 1 packet received, 0 no packet received
 */
int ethernet_read(size_t *blen, uint8_t *buf)
{
    //uint8_t           *pb;
    uint16_t          len;
    uint32_t          w0;
    uint32_t          *dst;
    volatile uint32_t *data_reg;

    /* check the link is up! */
    if ((ETH->ETH_RX_CNTL & MASK_ETH_RCR_RX_ENABLE) == 0)
	{
        /* no, it's down! */
        return -1;
    }

    /* check to see if there are any packets in the fifo */
    if(ETH->ETH_NUM_PKT == 0)
    {
        /* no, no packets in fifo */
        return 0;
    }

    dst = __builtin_assume_aligned(buf, sizeof(uint32_t));

    /* Store the current interrupt mask and then disable all interrupts.
     * Note: This is safe to call from within an ISR.
     */
	CRITICAL_SECTION_BEGIN

    /* read 32 bits from the ethernet module */
    w0 = ETH->ETH_DATA;

	/* length from data register is complete frame from destination
	 * address to CRC32 inclusive PLUS 2 bytes of length itself.
	 */
    len = (w0 & 0xffff);

    /* remove CRC32 */
    *blen = len - 4;

    /* add on amount to align last streamin.l to a 32 bit boundary */
    len -= 1;
    len &= (~3);

    w0 = (w0 & (~0xffff)) |(len & 0xffff);

    /* length and first 2 bytes of packet */
    *dst = w0;

    /* start at aligned location */
    dst++;

    /* transfer the remainder of the packet */
    data_reg = &ETH->ETH_DATA;
    __asm__("streamin.l %0,%1,%2" : :"r"(dst), "r"(data_reg), "r"(len): "memory");

    /* Restore previous interrupt mask. */
	CRITICAL_SECTION_END

	/* return 1 - we received a packet */
    return 1;
}

/** @brief outputs a packet on the ethernet interface.
 *  @param buf Pointer to packet to send. This MUST be 32 bit aligned.
 *  	The payload length is the length of the full packet with the
 *  	destination MAC, source MAC and payload length size subtracted.
 *  	ETHERNET_WRITE_HEADER is defined for this purpose.
 *  @param blen Length of packet to send (in bytes). This must
 *  	include the size of the payload length, destination and
 *  	source MAC headers, packet type and payload.
 * 
 *  The buffer must be in the following format:
 *  To send (n) bytes
 *  buf[0]    lsb of payload length (n & 0xff)
 *  buf[1]    msb of payload length (n >> 16)
 *  buf[2]    destination MAC[0]
 *  buf[3]    destination MAC[1]
 *  buf[4]    destination MAC[2]
 *  buf[5]    destination MAC[3]
 *  buf[6]    destination MAC[4]
 *  buf[7]    destination MAC[5]
 *  buf[8]    source MAC[0]
 *  buf[9]    source MAC[1]
 *  buf[10]    source MAC[2]
 *  buf[11]    source MAC[3]
 *  buf[12]    source MAC[4]
 *  buf[13]    source MAC[5]
 *  buf[14]    packet type
 *  buf[15]    packet type
 *  buf[16]    payload
 *  buf[.]    ...
 *  buf[n+16]    end of payload (n bytes).
 */
int ethernet_write(uint8_t *buf, size_t blen)
{
    register uint32_t *src;
    register volatile uint32_t *data_reg;
    register uint32_t size = (uint32_t)blen;

    /* check to see if the link is up */
    if ((ETH->ETH_TX_CNTL & MASK_ETH_TCR_TX_ENABLE) == 0)
	{
        /* no, it's down! */
        return -1;
    }

    /* Wait for any previous transmit to complete and free up
     * the TX RAM. */
    while (ETH->ETH_TR_REQ) {};

    /* add on amount to align last streamout.l to a 32 bit boundary */
    size = blen;
    size += 3;
    size &= (~3L);

    src = __builtin_assume_aligned(buf, sizeof(uint32_t));
    data_reg = &ETH->ETH_DATA;

    CRITICAL_SECTION_BEGIN

    __asm__("streamout.l %0,%1,%2" : :"r"(data_reg), "r"(src), "r"(size): "memory");

    /* send the packet! */
    ETH->ETH_TR_REQ = 1;

    CRITICAL_SECTION_END

    return 1;
}
