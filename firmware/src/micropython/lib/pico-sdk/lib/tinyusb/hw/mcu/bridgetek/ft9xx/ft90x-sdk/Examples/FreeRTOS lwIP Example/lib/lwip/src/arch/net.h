/**
  @file net.h
  @brief Abstraction file for handling access to lwIP library.
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

#ifndef NET_H_
#define NET_H_

#include <stdint.h>
#include <stdlib.h>

#include <lwip/inet.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>
#include <lwip/tcpip.h>
#include <lwip/timeouts.h>
#include <lwip/netif.h>
#include <lwip/init.h>
#include <lwip/sys.h>
#include <lwip/igmp.h>
#include <netif/etharp.h>
#include <lwip/dhcp.h>
#include <lwip/ip_addr.h>
#include <lwip/igmp.h>

#include "netif_arch.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 @brief Write the IP address configuration information to the EEPROM.
 @details Define macro to make an address change event update the EEPROM.
 */
#define NET_USE_EEPROM // lets make this enabled by default same as DHCP


#ifdef NET_USE_EEPROM
/**
 @brief Structure to hold IP address configuration in EEPROM.
 @details This is used to both keep settings stored persistently in EEPROM.
 */
struct eeprom_net_config {
	uint16_t key; // Must contain EEPROM_VALID_KEY
	uint8_t dhcp;
	ip_addr_t ip;
	ip_addr_t gw;
	ip_addr_t mask;
} __attribute__((packed));
/**
 @brief Key value to signify valid data in EEPROM.
 @details This must be unique to the layout of the eeprom_net_config structure.
 It is safest, when using this code as an example to build on, to assign a new
 unique number whenever the base structure changes.
 */
#define EEPROM_VALID_KEY 0x5A45

/**
 @brief Address of EEPROM on I2C bus.
 */
#define NET_EEPROM_ADDR 0xA0

/**
 @brief Offset in EEPROM for MAC address.
*/
#define NET_EEPROM_OFFSET_MACADDRESS 0xfa
#endif

/**
 @brief Callback function for network available/unavailable signal and packet available.
*/
typedef void (*fn_status_cb)(int netif_up, int link_up, int packet_available);

err_t net_init(ip_addr_t ip,
		ip_addr_t gw, ip_addr_t mask,
		int dhcp, char *hostname,
		fn_status_cb pfn_status);
err_t net_tick(void);

uint8_t net_is_link_up(void);
void net_set_link_up();
void net_set_link_down();
uint8_t net_is_up();

int8_t net_get_eeprom(struct eeprom_net_config *getval);
int8_t net_update_eeprom(struct eeprom_net_config *setval);
uint8_t net_get_dhcp();
uint8_t *net_get_mac();
ip_addr_t net_get_ip();
ip_addr_t net_get_gateway();
ip_addr_t net_get_netmask();
struct netif* net_get_netif();
void net_packet_available();

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* NET_H_ */
