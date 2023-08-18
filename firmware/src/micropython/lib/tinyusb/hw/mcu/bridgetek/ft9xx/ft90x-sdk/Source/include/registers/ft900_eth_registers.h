/**
    @file

    @brief
    Ethernet registers

    
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


#ifndef FT900_ETH_REGISTERS_H_
#define FT900_ETH_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/

/*
 *  definitions for ETH_MNG_CNTL
 */

#define BIT_ETH_MCR_RGAD 3
#define MASK_ETH_MCR_RGAD (0x1F<<BIT_ETH_MCR_RGAD)   /* Management control address register (MII) for write. */
#define BIT_ETH_MCR_WRITE 1
#define MASK_ETH_MCR_WRITE (1<<BIT_ETH_MCR_WRITE)   /* write bit of management control register (MII) */
#define BIT_ETH_MCR_START 0
#define MASK_ETH_MCR_START (1<<BIT_ETH_MCR_START)   /* start bit of management control register (MII) */

/*
 *  definitions for ETH_INT_STATUS
 */

#define BIT_ETH_IACK_MD_INT 5
#define MASK_ETH_IACK_MD_INT (1<<BIT_ETH_IACK_MD_INT)   /* MII transaction has completed */
#define BIT_ETH_IACK_RX_ERR 4
#define MASK_ETH_IACK_RX_ERR (1<<BIT_ETH_IACK_RX_ERR)   /* Receive error */
#define BIT_ETH_IACK_FIFO_OV 3
#define MASK_ETH_IACK_FIFO_OV (1<<BIT_ETH_IACK_FIFO_OV)   /* FIFO overflow */
#define BIT_ETH_IACK_TX_EMPTY 2
#define MASK_ETH_IACK_TX_EMPTY (1<<BIT_ETH_IACK_TX_EMPTY)   /* Transmit empty */
#define BIT_ETH_IACK_TX_ERR 1
#define MASK_ETH_IACK_TX_ERR (1<<BIT_ETH_IACK_TX_ERR)   /* Transmit error */
#define BIT_ETH_IACK_RX_INT 0
#define MASK_ETH_IACK_RX_INT (1<<BIT_ETH_IACK_RX_INT)   /* Packet received */

/*
 *  definitions for ETH_INT_ENABLE
 */

#define BIT_ETH_IMR_MD_INT_MASK 5
#define MASK_ETH_IMR_MD_INT_MASK (1<<BIT_ETH_IMR_MD_INT_MASK)   /* MII transaction has completed mask */
#define BIT_ETH_IMR_RX_ERR_MASK 4
#define MASK_ETH_IMR_RX_ERR_MASK (1<<BIT_ETH_IMR_RX_ERR_MASK)   /* Receive error mask */
#define BIT_ETH_IMR_FIFO_OV_MASK 3
#define MASK_ETH_IMR_FIFO_OV_MASK (1<<BIT_ETH_IMR_FIFO_OV_MASK)   /* FIFO overflow mask */
#define BIT_ETH_IMR_TX_EMPTY_MASK 2
#define MASK_ETH_IMR_TX_EMPTY_MASK (1<<BIT_ETH_IMR_TX_EMPTY_MASK)   /* Transmit empty mask */
#define BIT_ETH_IMR_TX_ERR_MASK 1
#define MASK_ETH_IMR_TX_ERR_MASK (1<<BIT_ETH_IMR_TX_ERR_MASK)   /* Transmit error mask */
#define BIT_ETH_IMR_RX_INT_MASK 0
#define MASK_ETH_IMR_RX_INT_MASK (1<<BIT_ETH_IMR_RX_INT_MASK)   /* Packet received mask */

/*
 *  definitions for ETH_RX_CNTL
 */

#define BIT_ETH_RCR_RX_MEM_SIZE 6
#define MASK_ETH_RCR_RX_MEM_SIZE (3<<BIT_ETH_RCR_RX_MEM_SIZE)    /* memory size - 2048 bytes */
#define BIT_ETH_RCR_RESET_FIFO 4
#define MASK_ETH_RCR_RESET_FIFO (1<<BIT_ETH_RCR_RESET_FIFO)    /* reset receive FIFO */
#define BIT_ETH_RCR_BAD_CRC 3
#define MASK_ETH_RCR_BAD_CRC (1<<BIT_ETH_RCR_BAD_CRC)    /* discard frames with bad CRC */
#define BIT_ETH_RCR_PRMS_MODE 2
#define MASK_ETH_RCR_PRMS_MODE (1<<BIT_ETH_RCR_PRMS_MODE)    /* set promiscuous mode */
#define BIT_ETH_RCR_ACC_MULTI 1
#define MASK_ETH_RCR_ACC_MULTI (1<<BIT_ETH_RCR_ACC_MULTI)    /* accept multicast */
#define BIT_ETH_RCR_RX_ENABLE 0
#define MASK_ETH_RCR_RX_ENABLE (1<<BIT_ETH_RCR_RX_ENABLE)    /* enable receiver */

/*
 * definitions for ETH_TX_CNTL
 */

#define BIT_ETH_TCR_TX_MEM_SIZE 6
#define MASK_ETH_TCR_TX_MEM_SIZE (3<<BIT_ETH_TCR_TX_MEM_SIZE)    /* memory size - 2048 bytes */
#define BIT_ETH_TCR_DUPLEX_MODE 4
#define MASK_ETH_TCR_DUPLEX_MODE (1<<BIT_ETH_TCR_DUPLEX_MODE)    /* enable ethernet full duplex mode */
#define BIT_ETH_TCR_CRC_ENABLE 2
#define MASK_ETH_TCR_CRC_ENABLE (1<<BIT_ETH_TCR_CRC_ENABLE)    /* append CRC to frames */
#define BIT_ETH_TCR_PAD_ENABLE 1
#define MASK_ETH_TCR_PAD_ENABLE (1<<BIT_ETH_TCR_PAD_ENABLE)    /* pad to minimum frame size */
#define BIT_ETH_TCR_TX_ENABLE 0
#define MASK_ETH_TCR_TX_ENABLE (1<<BIT_ETH_TCR_TX_ENABLE)    /* enable transmitter */

/*
 *  definitions for ETH_TR_REQ
 */

#define BIT_ETH_TRR_NEXTX 0
#define MASK_ETH_TRR_NEXTX (1<<BIT_ETH_TRR_NEXTX)   /* Start packet write. */

/*
 * definitions for the physical interface
 */
#define BIT_ETHPHY_MISC_PHYAD (0)
#define MASK_ETHPHY_MISC_PHYAD (0x1F << BIT_ETHPHY_MISC_PHYAD)
 
#define BIT_ETHPHY_MISC_PWRDN (8)
#define MASK_ETHPHY_MISC_PWRDN (1 << BIT_ETHPHY_MISC_PWRDN)

#define BIT_ETHPHY_MISC_PWRSV (9)
#define MASK_ETHPHY_MISC_PWRSV (1 << BIT_ETHPHY_MISC_PWRSV)

#define BIT_ETHPHY_MISC_LED1_SEL (16)
#define MASK_ETHPHY_MISC_LED1_SEL (0x7 << BIT_ETHPHY_MISC_LED1_SEL)

#define BIT_ETHPHY_MISC_LED0_SEL (20)
#define MASK_ETHPHY_MISC_LED0_SEL (0x7 << BIT_ETHPHY_MISC_LED0_SEL)

/* Note: Below READ ONLY bits are available from FT900 Rev C */
#define BIT_ETHPHY_MISC_LINK_SPEED_LED (24)    /*Active LOW indicates the link speed is 100Mbps*/
#define MASK_ETHPHY_MISC_LINK_SPEED_LED (0x1 << BIT_ETHPHY_MISC_LINK_SPEED_LED)

#define BIT_ETHPHY_MISC_FULL_DUPLEX_LED (25)    /*Active LOW indicates the status of the full-duplex mode.*/
#define MASK_ETHPHY_MISC_FULL_DUPLEX_LED (0x1 << BIT_ETHPHY_MISC_FULL_DUPLEX_LED)

#define BIT_ETHPHY_MISC_COLLISION_LED (26)    /*Active LOW indicates the presence of a collision.*/
#define MASK_ETHPHY_MISC_COLLISION_LED (0x1 << BIT_ETHPHY_MISC_COLLISION_LED)

#define BIT_ETHPHY_MISC_RX_ACTIVITY (27)    /*Active LOW indicates the presence of an active receive.*/
#define MASK_ETHPHY_MISC_RX_ACTIVITY (0x1 << BIT_ETHPHY_MISC_RX_ACTIVITY)

#define BIT_ETHPHY_MISC_TX_ACTIVITY (28)    /*Active LOW indicates the presence of an active transmit.*/
#define MASK_ETHPHY_MISC_TX_ACTIVITY (0x1 << BIT_ETHPHY_MISC_TX_ACTIVITY)

#define BIT_ETHPHY_MISC_LINK_TRAFFIC_LED (29)    /*Link State: Active Low indicates a good link status for 10/100M.
												  Traffic State: The traffic status is always ON (LOW) when the link
												  is OK and toggles between the TX and RX state.*/
#define MASK_ETHPHY_MISC_LINK_TRAFFIC_LED (0x1 << BIT_ETHPHY_MISC_LINK_TRAFFIC_LED)

#define BIT_ETHPHY_MISC_LINK_STATUS (31)    /*Link Status:0: Link is down,1: Link is up.*/
#define MASK_ETHPHY_MISC_LINK_STATUS (0x1 << BIT_ETHPHY_MISC_LINK_STATUS)
/* TYPES ***************************************************************************/
/** @brief Register mappings for Ethernet PHY registers */
typedef struct {
    volatile uint32_t MISC;
    volatile uint32_t ID;
} ft900_eth_phy_conf_reg_t;

/** @brief Register mappings for Ethernet MAC registers */
typedef struct {
    volatile uint8_t ETH_INT_STATUS;
    volatile uint8_t ETH_INT_ENABLE;
    volatile uint8_t ETH_RX_CNTL;
    volatile uint8_t ETH_TX_CNTL;
    volatile uint32_t ETH_DATA;
    volatile uint8_t ETH_ADDR[6];
    volatile uint8_t ETH_THRESHOLD;
    volatile uint8_t ETH_MNG_CNTL;
    volatile uint8_t ETH_MNG_DIV;
    volatile uint8_t ETH_MNG_ADDR;
    volatile uint16_t ETH_MNG_TX0;
    volatile uint16_t ETH_MNG_RX0;
    volatile uint8_t ETH_NUM_PKT;
    volatile uint8_t ETH_TR_REQ;
} ft900_eth_regs_t;


/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_ETH_REGISTERS_H_ */
