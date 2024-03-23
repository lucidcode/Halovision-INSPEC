/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_MIMXRT_ETH_H
#define MICROPY_INCLUDED_MIMXRT_ETH_H

typedef struct _eth_t eth_t;
extern eth_t eth_instance0;
extern eth_t eth_instance1;
void eth_init_0(eth_t *self, int mac_idx, const phy_operations_t *phy_ops, int phy_addr, bool phy_clock);

#if defined(ENET_DUAL_PORT)
void eth_init_1(eth_t *self, int mac_idx, const phy_operations_t *phy_ops, int phy_addr, bool phy_clock);
#endif

void eth_set_trace(eth_t *self, uint32_t value);
struct netif *eth_netif(eth_t *self);
int eth_link_status(eth_t *self);
int eth_start(eth_t *self);
int eth_stop(eth_t *self);
void eth_low_power_mode(eth_t *self, bool enable);

enum {
    PHY_KSZ8081 = 0,
    PHY_DP83825,
    PHY_DP83848,
    PHY_LAN8720,
    PHY_RTL8211F,
};

enum {
    PHY_TX_CLK_IN = false,
    PHY_TX_CLK_OUT = true,
};

#endif // MICROPY_INCLUDED_MIMXRT_ETH_H
