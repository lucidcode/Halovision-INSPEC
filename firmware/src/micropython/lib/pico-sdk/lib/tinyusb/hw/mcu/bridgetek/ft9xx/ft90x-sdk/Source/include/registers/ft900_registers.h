/**
    @file ft900_registers.h

    @brief
    FT900 registers
    
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

#ifndef FT900_REGISTERS_H_
#define FT900_REGISTERS_H_

/* INCLUDES ************************************************************************/
#if defined(__FT930__)

#include <registers/ft930_epfifo_registers.h>
#include <registers/ft930_ipc_registers.h>
#include <registers/ft930_slave_cpu_registers.h>

#include <registers/ft900_adc_dac_registers.h>
#include <registers/ft900_gpio_registers.h>
#include <registers/ft900_i2c_registers.h>
#include <registers/ft900_interrupt_registers.h>
#include <registers/ft900_pwm_registers.h>
#include <registers/ft900_rtc_registers.h>
#include <registers/ft900_spi_registers.h>
#include <registers/ft900_sys_registers.h>
#include <registers/ft900_timer_wdt_registers.h>
#include <registers/ft900_uart_registers.h>
#include <registers/ft900_usbd_registers.h>

#else

#include <registers/ft900_can_registers.h>
#include <registers/ft900_cam_registers.h>
#include <registers/ft900_spi_registers.h>
#include <registers/ft900_sys_registers.h>
#include <registers/ft900_gpio_registers.h>
#include <registers/ft900_rtc_registers.h>
#include <registers/ft900_timer_wdt_registers.h>
#include <registers/ft900_pwm_registers.h>
#include <registers/ft900_interrupt_registers.h>
#include <registers/ft900_adc_dac_registers.h>
#include <registers/ft900_i2c_registers.h>
#include <registers/ft900_i2s_registers.h>
#include <registers/ft900_uart_registers.h>
#include <registers/ft900_usbd_registers.h>
#include <registers/ft900_ehci_registers.h>
#include <registers/ft900_eth_registers.h>
#include <registers/ft900_flash_registers.h>
#include <registers/ft900_usbd_hbw_registers.h>
#endif

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/
#if defined(__FT930__)

#define SYS         ((ft900_sys_regs_t *)           0x10000) /*Same*/
#define GPIO        ((ft900_gpio_regs_t *)          0x1001C) /*Same*/
#define SLAVECPU    ((ft930_slave_control_regs_t *) 0x100A8) /*New */
#define ADCDAC      ((ft900_adc_dac_regs_t *)       0x100B0) /*Same*/
#define SYS_RST     ((ft900_sys_reset_regs_t *)     0x100BC) /*New */
#define INTERRUPT   ((ft900_interrupt_regs_t *)     0x10100) /*Changed */
#define USBD        ((ft900_usbd_regs_t *)          0x10200) /*Changed */
#define RTC         ((ft900_rtc_regs_t *)           0x10400) /*Changed */
#define SPIM        ((ft900_spi_regs_t *)           0x10440) /*Changed */
#define SPIS0       ((ft900_spi_regs_t *)           0x10480) /*Changed */
#define SPIS1       ((ft900_spi_regs_t *)           0x104C0) /*Changed */
#define I2CM        ((ft900_i2cm_regs_t *)          0x10500) /*Changed */
#define I2CS        ((ft900_i2cs_regs_t *)          0x10510) /*Changed */
#define UART0       ((ft900_uart_regs_t *)          0x10520) /*Changed */
#define UART1       ((ft900_uart_regs_t *)          0x10530) /*Changed */
#define UART2       ((ft900_uart_regs_t *)          0x10540) /*New */
#define UART3       ((ft900_uart_regs_t *)          0x10550) /*New */
#define TIMER       ((ft900_timer_wdt_regs_t *)     0x10560) /*Changed */
#define PWM         ((ft900_pwm_regs_t *)           0x105B0) /*Changed */
#else

#define SYS         ((ft900_sys_regs_t *)           0x10000)
#define GPIO        ((ft900_gpio_regs_t *)          0x1001C)
#define ETHPHY      ((ft900_eth_phy_conf_reg_t *)   0x100A8)
#define ADCDAC      ((ft900_adc_dac_regs_t *)       0x100B0)
#define SYS_RST     ((ft900_sys_reset_regs_t *)     0x100BC) /* for FT900 Rev C onwards */
#define INTERRUPT   ((ft900_interrupt_regs_t *)     0x100C0)
#define USBD        ((ft900_usbd_regs_t *)          0x10180)
#define USBD_HBW    ((ft900_usbd_hbw_ctrl_regs_t *) 0x10A1C) /* for FT900 Rev C onwards */
#define ETH         ((ft900_eth_regs_t *)           0x10220)
#define CAN0        ((ft900_can_regs_t *)           0x10240)
#define CAN1        ((ft900_can_regs_t *)           0x10260)
#define RTCL        ((ft900_rtc_legacy_regs_t *)    0x10280) /* for FT900 Rev B */
#define RTC         ((ft900_rtc_regs_t *)           0x10900) /* for FT900 Rev C onwards */
#define SPIM        ((ft900_spi_regs_t *)           0x102A0) /* for FT900 Rev B */
#define SPIM_EX     ((ft900_spi_regs_ex_t *)        0x102A0) /* for FT900 Rev C onwards */
#define SPIS0       ((ft900_spi_regs_t *)           0x102C0) /* for FT900 Rev B */
#define SPIS0_EX    ((ft900_spi_regs_ex_t *)        0x102C0) /* for FT900 Rev C onwards */
#define SPIS1       ((ft900_spi_regs_t *)           0x102E0) /* for FT900 Rev B */
#define SPIS1_EX    ((ft900_spi_regs_ex_t *)        0x102E0) /* for FT900 Rev C onwards */
#define I2CM        ((ft900_i2cm_regs_t *)          0x10300)
#define I2CS        ((ft900_i2cs_regs_t *)          0x10310)
#define UART0       ((ft900_uart_regs_t *)          0x10320)
#define UART1       ((ft900_uart_regs_t *)          0x10330)
#define TIMER       ((ft900_timer_wdt_regs_t *)     0x10340)
#define I2S         ((ft900_i2s_regs_t *)           0x10350)
#define CAM         ((ft900_cam_regs_t *)           0x10360)
#define PWM         ((ft900_pwm_regs_t *)           0x103C0)
#define EHCI        ((ft900_ehci_regs_t *)          0x10100)
#define FLASHCTRL   ((ft900_flash_regs_t *)         0x10800)
#define EHCI_RAM    ((ft900_ehci_ram_t *)           0x11000)
#define EHCI_RAM_SIZE                               0x2000

#endif

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/


#endif /* FT900_REGISTERS_H_ */
