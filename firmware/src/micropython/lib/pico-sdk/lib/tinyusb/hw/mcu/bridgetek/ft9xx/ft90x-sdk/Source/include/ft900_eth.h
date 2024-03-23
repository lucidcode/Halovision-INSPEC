/**
    @file ft900_eth.h

    @brief
    Ethernet driver

    
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

#ifndef ETHERNET_H
#define ETHERNET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* CONSTANTS ***********************************************************************/

/** @name MII (Media Independent Interface) constants.
 *  @brief Used for interrogating or configuring the PHY.
 *  See Realtek RTL 8201 datasheet for register values.
 */
//@{

/** @name MII (Media Independent Interface) registers.
 */
//@{
#define MII_BMCR 0x00 /// MII Basic mode control register.
#define MII_BMSR 0x01 /// MII Basic mode status register.
#define MII_PHYID_HIGH 0x2 // PHYS ID 1.
#define MII_PHYID_LOW 0x3 // PHYS ID 2.
#define MII_ADVERTISE 0x4 // Advertisement control register.
#define MII_LPA 0x5 // Link partner ability registers.
#define MII_EXPANSION 0x6 // Expansion register.
#define MII_NEXT_PAGE 0x7
#define MII_LP_NEXT_PAGE 0x8
#define MII_1000B_CTRL 0x9
#define MII_1000B_ST 0xA
#define MII_EXT_ST 0xF // Extended status register.
#define MII_PHY_SPEC_CTRL 0x10
#define MII_PHY_SPEC_ST 0x11
#define MII_INT_EN 0x12
#define MII_INT_ST 0x13
#define MII_EXT_SP_CTRL 0x14
#define MII_REC_ERR_CNT 0x15
#define MII_EXT_ADDR0 0x16
#define MII_GLOBAL_ST 0x17
#define MII_LED_CTRL 0x18
#define MII_MAN_OV_LED 0x19
#define MII_EXT_PHY_SP_CTRL2 0x1A
#define MII_EXT_PHY_SP_ST 0x1B
#define MII_TESTER 0x1C
#define MII_EXT_ADDR1 0x1D
#define MII_CALIBR 0x1E
//@}

/** @name MII BMCR register definitions.
 */
//@{
#define MII_BMCR_RESET 0x8000 // Reset PHY.
#define MII_BMCR_LOOPBACK 0x4000 // Enable TX loopback.
#define MII_BMCR_SPEED100 0x2000 // Select 100 Mbit/sec. Not used if auto-negotiation enabled.
#define MII_BMCR_AUTO_NEG_EN 0x1000 // Enable auto-negotiation.
#define MII_BMCR_ENABLE_LOW_POWER 0x0800 // Enable low power state.
#define MII_BMCR_ISOLATE 0x0400 // Isolate from MII.
#define MII_BMCR_RESET_AUTONEG  0x0200 // Reset auto-negociation.
#define MII_BMCR_FULLDUPLEX 0x0100 // Enable full duplex. Not used if auto-negotiation enabled.
#define MII_BMCR_COLLISION 0x0080 // Collision test.
#define MII_BMCR_SPEED1000 0x0040 // Select 1000 Mbit/sec. Not used if auto-negotiation enabled.
//@}

/** @name MII BMSR register definitions. Read only.
 */
#define MII_BMSR_100BASE4 0x8000 // Able to do 100 Full Duplex, 4 kB packets.
#define MII_BMSR_100FULL 0x4000 // Able to do 100 Full Duplex.
#define MII_BMSR_100HALF 0x2000 // Able to do 100 Half Duplex.
#define MII_BMSR_10FULL 0x1000 // Able to do 10 Full Duplex.
#define MII_BMSR_10HALF 0x0800 // Able to do 10 Half Duplex.
#define MII_BMSR_100FULL2 0x0400 // Able to do 100BASE-T2 Full Duplex.
#define MII_BMSR_100HALF2 0x0200 // Able to do 100BASE-T2 Half Duplex.
#define MII_BMSR_PREABLE_SUPP_PERMITTED 0x0040 // preable suppression permitted.
#define MII_BMSR_AUTO_NEG_CMPLT 0x0020 // auto-negotiation complete.
#define MII_BMSR_REMOTE_FAULT 0x0010 // remote fault.
#define MII_BMSR_AUTO_NEG_CAPAB 0x0008 // auto-negotiation capable.
#define MII_BMSR_LINK 0x0004 // Link status.
#define MII_BMSR_JABBER 0x0002 // Jabber detected.
#define MII_BMSR_EXTENDED_REG_SET 0x0001 // PHY extended register set exists.
//@}

/** @name MII Advertisement control register definitions.
 */
//@{
#define MII_ACR_SLCT 0x001f // Selector bits mask.
#define MII_ACR_CSMA 0x0001 // This is the only selector supported.
#define MII_ACR_10HALF 0x0020 // Supports 10 Half Duplex.
#define MII_ACR_10FULL 0x0040 // Supports 10 Full Duplex.
#define MII_ACR_100HALF 0x0080 // Supports 100 Half Duplex.
#define MII_ACR_100FULL 0x0100 // Supports 100 Full Duplex.
#define MII_ACR_100BASE4 0x0200 // Supports 100 Full Duplex, 4 kB packets.
#define MII_ACR_PAUSE 0x0400 // Supports Rx Flow Control
#define MII_ACR_TFC 0x0800 // Supports Tx Flow Control
#define MII_ACR_LPACK 0x4000 // Acknowledge link partners response.
#define MII_ACR_NPAGE 0x8000 // Next page. 0 - primary capability page, 1 - protocol specific page.
//@}

/** @name MII link partner ability register definitions.
 */
//@{
#define MII_LPA_SLCT 0x001f // Selector bits mask.
#define MII_LPA_CSMA 0x0001 // This is the only selector supported.
#define MII_LPA_10HALF 0x0020 // Supports 10 Half Duplex.
#define MII_LPA_10FULL 0x0040 // Supports 10 Full Duplex.
#define MII_LPA_100HALF 0x0080 // Supports 100 Half Duplex.
#define MII_LPA_100FULL 0x0100 // Supports 100 Full Duplex.
#define MII_LPA_100BASE4 0x0200 // Supports 100 Full Duplex, 4 kB packets.
#define MII_LPA_PAUSE 0x0400 // Supports Rx Flow Control
#define MII_LPA_TFC 0x0800 // Supports Tx Flow Control
#define MII_LPA_LPACK 0x4000 // Acknowledge link partners response.
#define MII_LPA_NPAGE 0x8000 // Next page. 0 - primary capability page, 1 - protocol specific page.
//@}

/** @name MII PHY special control status register definitions.
 */
//@{
#define MII_SCSR_AUTO_DONE 0x1000 // Auto-negotiation done.
#define MII_SCSR_SPEED_100 0x0020 // 100 Mb/sec
#define MII_SCSR_SPEED_10 0x0010 // 10 Mb/sec
#define MII_SCSR_SPEED_FULL_DUPLEX 0x0040 // Full duplex
//@}

//@}

/* TYPES ***************************************************************************/

/** @brief Ethernet LED pin mode */
typedef enum
{
    ethernet_led_mode_link = 0, /*!< Link active */
    ethernet_led_mode_tx = 1,   /*!< Transmit */
    ethernet_led_mode_rx = 2,   /*!< Receive */
    ethernet_led_mode_col = 3,  /*!< Collision */
    ethernet_led_mode_fdx = 4,  /*!< Full duplex */
    ethernet_led_mode_spd = 5   /*!< Speed 10/100 */
} ethernet_led_mode_t;

/* GLOBAL VARIABLES ****************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Initialise the ethernet hardware.
 *  @param mac pointer to a six byte array containing MAC
 */
void ethernet_init(const uint8_t *mac);

/** @brief Set the mode of the led 
 *  @param led The number of the led (0 or 1)
 *  @param mode The mode which the led will be in
 */
void ethernet_led_mode(uint8_t led, ethernet_led_mode_t mode);

/** @brief Read the content of an MII register
 *
 *  @param reg register to read
 *  @returns The content of requested register
 */

uint16_t ethernet_mii_read(uint8_t reg);

/** @brief Write a value to an MII register
 *  @param reg Register to write to
 *  @param v Value to write to requested register
 *  @returns 0 on success, -1 on error
 */

int ethernet_mii_write(uint8_t reg, uint16_t v);

/** @brief Poll the ethernet peripheral for the receptition of a single packet
 *  @param buf Pointer to buffer to store the received packet. This MUST be
 *  		32 bit aligned. The maximum size of data received by the dmac is
 *  		1500 bytes. Therefore every buffer supplied must be able to
 *  		handle 1518 bytes (1500 data bytes, 16 ethernet header bytes and
 *  		2 payload length bytes).
 *  @param blen Size of packet received
 *  @returns 1 packet received, 0 no packet received
 */
int ethernet_read(size_t *blen, uint8_t *buf);

#define ETHERNET_WRITE_HEADER 14
/** @brief Outputs a packet on the ethernet interface.
 *
 *  The buffer must be in the following format:
 *  To send (n) bytes
 *
 *  | Offset    | Use                              |
 *  | --------- | -------------------------------- |
 *  | buf[0]    | lsb of payload length (n & 0xff) |
 *  | buf[1]    | msb of payload length (n >> 16)  |
 *  | buf[2]    | destination MAC[0]               |
 *  | buf[3]    | destination MAC[1]               |
 *  | buf[4]    | destination MAC[2]               |
 *  | buf[5]    | destination MAC[3]               |
 *  | buf[6]    | destination MAC[4]               |
 *  | buf[7]    | destination MAC[5]               |
 *  | buf[8]    | source MAC[0]                    |
 *  | buf[9]    | source MAC[1]                    |
 *  | buf[10]   |  source MAC[2]                   |
 *  | buf[11]   |  source MAC[3]                   |
 *  | buf[12]   |  source MAC[4]                   |
 *  | buf[13]   |  source MAC[5]                   |
 *  | buf[14]   |  packet type                     |
 *  | buf[15]   |  packet type                     |
 *  | buf[16]   |  payload                         |
 *  | buf[.]    | ...                              |
 *  | buf[n+16] |    end of payload (n bytes).     |
 *
 *  @param buf Pointer to packet to send. This MUST be 32 bit aligned.
 *  	The payload length is the length of the full packet with the
 *  	destination MAC, source MAC, type/length field size subtracted.
 *  	The payload length, however, is included.
 *  	ETHERNET_WRITE_HEADER is defined for this purpose.
 *  @param blen Total length of packet to send (in bytes). This must
 *  	include the size of the payload length, destination and
 *  	source MAC headers, packet type and payload.
 */
int ethernet_write(uint8_t *buf, size_t blen);

/** @brief Enable or disable the Ethernet receiver
 *  @param flag 0 - disable receiver, 1 - enable receiver
 */
void ethernet_rx_enable(int flag);

/** @brief Enable or disable the Ethernet transmitter
 *  @param flag 0 - disable transmitter, 1 - enable transmitter
 */
void ethernet_tx_enable(int flag);

/** @brief Enable the Ethernet peripheral to fire interrupts
 *  @param mask
 */
void ethernet_enable_interrupt(uint8_t mask);

/** @brief Set the Ethernet peripheral in promiscuous mode
 *  @details By default this is disabled.
 *  @param flag 0 - disable promiscuous mode,  1 - set promiscuous mode
 */
void ethernet_set_promiscuous(int flag);

/** @brief set the ethernet peripheral to accept multicast
 *  @details By default this is disabled.
 *  @param flag 0 - disable accept multicast mode,  1 - set accept multicast mode
 */
void ethernet_accept_multicast(int flag);

/** @brief set the ethernet peripheral to transmit in full duplex mode
 *  @details By default this is enabled.
 *  @param flag 0 - disable duplex mode,  1 - set enable duplex mode
 */
void ethernet_enable_full_duplex(int flag);

/** @brief set the ethernet peripheral to handle CRCs
 *  @details By default this is enabled.
 *  @param flag 0 - disable crc mode,  1 - set enable crc mode
 */
void ethernet_enable_crc(int flag);

/** @brief set the ethernet peripheral to handle padding
 *  @details By default this is enabled.
 *  @param flag 0 - disable pad mode,  1 - set enable pad mode
 */
void ethernet_enable_pad(int flag);

/** @brief Set the Ethernet MAC
 *  @param mac A pointer to a six byte array containing MAC
 */
void ethernet_set_mac(const uint8_t * mac);

/** @brief Return the Ethernet link status
 *  @returns 1 - link up, 0 - link is not up.
 */
int ethernet_is_link_up(void);

/** @brief Return the ethernet link speed
 *  @returns 10 - 10 Mb/sec, 100 - 100 Mb/sec.
 */
int ethernet_link_speed(void);

/** @brief Return the ethernet duplex mode
 *  @returns 0 - half duplex, 1 - full duplex.
 */
int ethernet_link_duplex(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* ETHERNET_H */
