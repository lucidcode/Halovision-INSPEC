/**
    @file

    @brief
    Chip management registers

    
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

#ifndef FT900_SYS_REGISTERS_H_
#define FT900_SYS_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/

#define BIT_SYS_CLKCFG_ENA(__peri__)     (__peri__)
#define MASK_SYS_CLKCFG_ENA(__peri__)    (1 << BIT_SYS_CLKCFG_ENA(__peri__))

/* !!!AVAILABLE from FT930 and FT900 REV C ONWARDS!!!
 * CPU_CLK_DIV - CPU Clock frequency setting:
 * 0: CPU runs at system clock speed (100 MHz)
 * 1: CPU runs at system clock / 2
 * 2: CPU runs at system clock / 4
 * 3: CPU runs at system clock / 8
 * 4: CPU runs at system clock / 64
 * 5: CPU runs at system clock / 128
 * 6: CPU runs at system clock / 512
 * Others: CPU is stopped.
 * */
#define BIT_SYS_CLKCFG_CPU_CLK_DIV  (16)
#define MASK_SYS_CLKCFG_CPU_CLK_DIV (0xF << BIT_SYS_CLKCFG_CPU_CLK_DIV)

/******************************** START of ***********************************/
/**************** BIT MASKS FOR HIGHER 16-bit WORD of PMCFG ******************/
/* Bit masks for the PMCFG register */
#if defined(__FT930__)
/** RTC Alarm Wake Up interrupt pending; write 1 to clear. This bit will be set when RTC alarm is active while it is in sleep mode. */
#define BIT_SYS_PMCFG_RTC_ALARM_IRQ_PEND  (15)
#define MASK_SYS_PMCFG_RTC_ALARM_IRQ_PEND (0x1 << BIT_SYS_PMCFG_RTC_ALARM_IRQ_PEND)

/** SLAVE Wake Up interrupt pending; write 1 to clear. This bit will be set when there is wake up event from SLAVE. */
#define BIT_SYS_PMCFG_SLAVE_PERI_IRQ_PEND  (10)
#define MASK_SYS_PMCFG_SLAVE_PERI_IRQ_PEND (0x1 << BIT_SYS_PMCFG_SLAVE_PERI_IRQ_PEND)
#else
/** RTC Alarm Wake Up interrupt pending; write 1 to clear. This bit will be set when RTC alarm is active while it is in sleep mode.
 * !!!AVAILABLE FROM FT900 REV C ONWARDS!!!
 * */
#define BIT_SYS_PMCFG_RTC_ALARM_IRQ_PEND  (15)
#define MASK_SYS_PMCFG_RTC_ALARM_IRQ_PEND (0x1 << BIT_SYS_PMCFG_RTC_ALARM_IRQ_PEND)

/** RTC Battery Low Detected pending; write 1 to clear. If RTC_VBAT_IRQ_EN=1, an interrupt will be generated.
 * Note: Clear this bit before set RTC_VBAT_IRQ_EN to “1”.
 * !!!AVAILABLE FROM FT900 REV C ONWARDS!!!
 * */
#define BIT_SYS_PMCFG_RTC_VBAT_LOW_PEND  (14)
#define MASK_SYS_PMCFG_RTC_VBAT_LOW_PEND (0x1 << BIT_SYS_PMCFG_RTC_VBAT_LOW_PEND)

/** RTC good Battery replacement Detected pending; write 1 to clear. If RTC_VBAT_IRQ_EN=1, an interrupt will be generated.
 * Note: Clear this bit before set RTC_VBAT_IRQ_EN to “1”.
 * !!!AVAILABLE FROM FT900 REV C ONWARDS!!!
 * */
#define BIT_SYS_PMCFG_RTC VBAT REPLACED_PEND  (13)
#define MASK_SYS_PMCFG_RTC VBAT REPLACED_PEND (0x1 << BIT_SYS_PMCFG_RTC VBAT REPLACED_PEND)
#endif //#if defined(__FT930__)

/** GPIO interrupt during system shut down with clock not running. */
#define BIT_SYS_PMCFG_PM_GPIO_IRQ_PEND  (9)
#define MASK_SYS_PMCFG_PM_GPIO_IRQ_PEND (0x1 << BIT_SYS_PMCFG_PM_GPIO_IRQ_PEND)

/** Slow clock 5ms timer interrupt pending; write 1 to clear. If enabled, an interrupt will be generated. */
#define BIT_SYS_PMCFG_SLOWCLK_5ms_IRQ_PEND  (8)
#define MASK_SYS_PMCFG_SLOWCLK_5ms_IRQ_PEND (0x1 << BIT_SYS_PMCFG_)

/** Remote Wakeup Interrupt pending to USB Host; write 1 to clear. If enabled, an interrupt will be
 *  generated on PM irq. */
#define BIT_SYS_PMCFG_RM_WK_HOST  (7)
#define MASK_SYS_PMCFG_RM_WK_HOST (0x1 << BIT_SYS_PMCFG_RM_WK_HOST)

/** Device Connect Interrupt pending to USB Host; write 1 to clear. If enabled, an interrupt will be
 * generated on PM irq. */
#define BIT_SYS_PMCFG_DEV_CONN_HOST  (6)
#define MASK_SYS_PMCFG_DEV_CONN_HOST (0x1 << BIT_SYS_PMCFG_DEV_CONN_HOST)

/** Device Disconnect Interrupt pending to USB Host; write 1 to clear. If enabled, an interrupt will be
 *  generated on PM irq. */
#define BIT_SYS_PMCFG_DEV_DIS_HOST  (5)
#define MASK_SYS_PMCFG_DEV_DIS_HOST (0x1 << BIT_SYS_PMCFG_DEV_DIS_HOST)

/** Device Connect Interrupt pending to USB Device; write 1 to clear. If enabled, an interrupt will be
 *  generated on PM irq. */
#define BIT_SYS_PMCFG_DEV_CONN_DEV  (4)
#define MASK_SYS_PMCFG_DEV_CONN_DEV (0x1 << BIT_SYS_PMCFG_DEV_CONN_DEV)

/** Device Disconnect Interrupt pending to USB Device; write 1 to clear. If enabled, an interrupt will
 *  be generated on PM irq. */
#define BIT_SYS_PMCFG_DEV_DIS_DEV  (3)
#define MASK_SYS_PMCFG_DEV_DIS_DEV (0x1 << BIT_SYS_PMCFG_DEV_DIS_DEV)

/** Host Reset Interrupt pending to USB Device; write 1 to clear. If enabled, an interrupt will be
 *  generated on PM irq. */
#define BIT_SYS_PMCFG_HOST_RST_DEV  (2)
#define MASK_SYS_PMCFG_HOST_RST_DEV (0x1 << BIT_SYS_PMCFG_HOST_RST_DEV)

/** Host Resume Interrupt pending to USB Device; write 1 to clear. If enabled, an interrupt will be
 *  generated on PM irq. */
#define BIT_SYS_PMCFG_HOST_RESUME_DEV  (1)
#define MASK_SYS_PMCFG_HOST_RESUME_DEV (0x1 << BIT_SYS_PMCFG_HOST_RESUME_DEV)

/** Over current detected Interrupt pending if enabled; write 1 to clear. If enabled, an interrupt will
 *  be generated on PM irq. */
#define BIT_SYS_PMCFG_OC_DETECT  (0)
#define MASK_SYS_PMCFG_OC_DETECT (0x1 << BIT_SYS_PMCFG_OC_DETECT)
/********************************  END ***************************************/
/*****************************************************************************/

/******************************** START of ***********************************/
/**************** BIT MASKS FOR LOWER 16-bit WORD of PMCFG *******************/
#if defined(__FT900__)
/** 0 : Enables external VBUS, 1: Disables external VBUS
 * Note: When OC_DETECT_EN is set and an over current condition occurs, hardware will automatically set this bit.
 * This bit cannot be cleared by SW as long as Over Current condition exists.
 * !!!AVAILABLE FROM FT900 REV C ONWARDS!!!
 */
#define BIT_SYS_PMCFG_PSWN  (15)
#define MASK_SYS_PMCFG_PSWN (0x1 << BIT_SYS_PMCFG_PSWN)

/** 0 : RTC Battery low detection is enabled by default; if this bit is set, PM will detect for replacement of Battery.
 * !!!AVAILABLE FROM FT900 REV C ONWARDS!!!
 * */
#define BIT_SYS_PMCFG_RTC_VBAT_STATUS_DETECT  (14)
#define MASK_SYS_PMCFG_RTC_VBAT_STATUS_DETECT (0x1 << BIT_SYS_PMCFG_RTC_VBAT_STATUS_DETECT)

/** 1 : RTC Battery Interrupt Enable; if this bit is set, an interrupt will be generated on PM irq when RTC_VBAT_IRQ_PEND becomes active.
 * !!!AVAILABLE FROM FT900 REV C ONWARDS!!!
 * */
#define BIT_SYS_PMCFG_RTC_VBAT_IRQ_EN  (13)
#define MASK_SYS_PMCFG_RTC_VBAT_IRQ_EN (0x1 << BIT_SYS_PMCFG_RTC_VBAT_IRQ_EN)

/** 1 : RTC Alarm Wake Up Interrupt Enable; if this bit is set, an interrupt will be generated on PM irq when RTC_ALARM_IRQ_PEND becomes active.
 * !!!AVAILABLE FROM FT900 REV C ONWARDS!!!
 * */
#define BIT_SYS_PMCFG_RTC_ALARM_IRQ_EN  (12)
#define MASK_SYS_PMCFG_RTC_ALARM_IRQ_EN (0x1 << BIT_SYS_PMCFG_RTC_ALARM_IRQ_EN)
#elif defined(__FT930__)
/** 1 : RTC Alarm Wake Up Interrupt Enable; if this bit is set, an interrupt will be generated on PM irq when RTC_ALARM_IRQ_PEND becomes active. */
#define BIT_SYS_PMCFG_RTC_ALARM_IRQ_EN  (12)
#define MASK_SYS_PMCFG_RTC_ALARM_IRQ_EN (0x1 << BIT_SYS_PMCFG_RTC_ALARM_IRQ_EN)

/** 1 : SLAVE Wake Up Interrupt Enable; If this bit is set, an interrupt will be generated on PM irq when SLAVE_PERI_IRQ_PEND becomes active. */
#define BIT_SYS_PMCFG_SLAVE_PERI_IRQ_EN  (11)
#define MASK_SYS_PMCFG_SLAVE_PERI_IRQ_EN (0x1 << BIT_SYS_PMCFG_SLAVE_PERI_IRQ_EN)
#endif

/** 1 : Enable USB Device Phy. */
#define BIT_SYS_PMCFG_DEV_PHY_EN  (10)
#define MASK_SYS_PMCFG_DEV_PHY_EN (0x1 << BIT_SYS_PMCFG_DEV_PHY_EN)

/** 1 : disable system oscillator when powering down.\n
 *  0 : do not disable system oscillator when powering down. */
#define BIT_SYS_PMCFG_PM_PWRDN_MODE  (9)
#define MASK_SYS_PMCFG_PM_PWRDN_MODE (0x1 << BIT_SYS_PMCFG_PM_PWRDN_MODE)

/** 1 : power down system.\n
 *  This bit should be cleared after the system wakes up or at least 60-100us prior to setting it 1 again. */
#define BIT_SYS_PMCFG_PM_PWRDN  (8)
#define MASK_SYS_PMCFG_PM_PWRDN (0x1 << BIT_SYS_PMCFG_PM_PWRDN)

/** 1 : enable slow clock 5ms timer interrupt. */
#define BIT_SYS_PMCFG_SLOWCLOCK_5ms_IRQ_EN  (7)
#define MASK_SYS_PMCFG_SLOWCLOCK_5ms_IRQ_EN (0x1 << BIT_SYS_PMCFG_SLOWCLOCK_5ms_IRQ_EN)

/** 1 : To start the 1-shot slow clock 5ms timer; once started it cannot be stopped.\n
 *  This bit will be cleared automatically when the timer expires. */
#define BIT_SYS_PMCFG_SLOWCLOCK_5ms_START  (6)
#define MASK_SYS_PMCFG_SLOWCLOCK_5ms_START (0x1 << BIT_SYS_PMCFG_SLOWCLOCK_5ms_START)

/** Normally host activity detection is performed only when required, setting this bit will force the PM to
 *  check for host connection activities regardless. */
#define BIT_SYS_PMCFG_FORCE_HOST_DET  (5)
#define MASK_SYS_PMCFG_FORCE_HOST_DET (0x1 << BIT_SYS_PMCFG_FORCE_HOST_DET)

/** Normally device activity detection is performed only when required, setting this bit will force the PM
 *  to check for device connection activities regardless. */
#define BIT_SYS_PMCFG_FORCE_DEV_DET  (4)
#define MASK_SYS_PMCFG_FORCE_DEV_DET (0x1 << BIT_SYS_PMCFG_FORCE_DEV_DET)

/** 1 : Enable remote wake up detection to USB host.\n
 *      Enable interrupt to pm irq when RM_WK_HOST is set. */
#define BIT_SYS_PMCFG_RM_WK_HOST_EN  (3)
#define MASK_SYS_PMCFG_RM_WK_HOST_EN (0x1 << BIT_SYS_PMCFG_RM_WK_HOST_EN)

/** 1 : Enable device connection/disconnect detection to USB Host.\n
 *      Enable interrupt to pm irq when either DEV_CONN_HOST or DEV_DIS_HOST is set. */
#define BIT_SYS_PMCFG_HOST_DETECT_EN  (2)
#define MASK_SYS_PMCFG_HOST_DETECT_EN (0x1 << BIT_SYS_PMCFG_HOST_DETECT_EN)

/** 1 : Enable device connect/disconnect to external host or external host reset detection.\n
 *  Enable interrupt to pm irq when any of DEV_CONN_DEV, DEV_DIS_DEV and HOST_RST_DEV is set. */
#define BIT_SYS_PMCFG_DEV_DETECT_EN  (1)
#define MASK_SYS_PMCFG_DEV_DETECT_EN (0x1 << BIT_SYS_PMCFG_DEV_DETECT_EN)

/** 1 : Enable Over current detection.\n
 *  Enable interrupt to pm irq when OC_DETECT is set */
#define BIT_SYS_PMCFG_OC_DETECT_EN  (0)
#define MASK_SYS_PMCFG_OC_DETECT_EN (0x1 << BIT_SYS_PMCFG_OC_DETECT_EN)
/********************************  END ***************************************/
/*****************************************************************************/

/* Bit masks for the MSC0CFG register */

/** @brief Reset all peripherals
 *  Write 1 to cause soft reset to all peripherals. It is automatically cleared. */
#define BIT_SYS_MSC0CFG_PERI_SOFTRESET  (31)
#define MASK_SYS_MSC0CFG_PERI_SOFTRESET (0x1 << BIT_SYS_MSC0CFG_PERI_SOFTRESET)

/** @brief Reset PWM
 *  Write 1 to cause soft reset to PWM. It is automatically cleared. */
#define BIT_SYS_MSC0CFG_PWM_SOFTRESET  (30)
#define MASK_SYS_MSC0CFG_PWM_SOFTRESET (0x1 << BIT_SYS_MSC0CFG_PWM_SOFTRESET)

#if defined(__FT930__)
/** Clock Select for UART4 */
#define BIT_SYS_MSC0CFG_UART4_CLKSEL  (29)
#define MASK_SYS_MSC0CFG_UART4_CLKSEL (0x1 << BIT_SYS_MSC0CFG_UART4_CLKSEL)

/** FIFO Select for UART4 */
#define BIT_SYS_MSC0CFG_UART4_FIFOSEL  (28)
#define MASK_SYS_MSC0CFG_UART4_FIFOSEL (0x1 << BIT_SYS_MSC0CFG_UART4_FIFOSEL)

/** INT Select for UART4 */
#define BIT_SYS_MSC0CFG_UART4_INTSEL  (27)
#define MASK_SYS_MSC0CFG_UART4_INTSEL (0x1 << BIT_SYS_MSC0CFG_UART4_INTSEL)

/** Clock Select for UART3 */
#define BIT_SYS_MSC0CFG_UART3_CLKSEL  (26)
#define MASK_SYS_MSC0CFG_UART3_CLKSEL (0x1 << BIT_SYS_MSC0CFG_UART3_CLKSEL)

/** FIFO Select for UART3 */
#define BIT_SYS_MSC0CFG_UART3_FIFOSEL  (25)
#define MASK_SYS_MSC0CFG_UART3_FIFOSEL (0x1 << BIT_SYS_MSC0CFG_UART3_FIFOSEL)

/** INT Select for UART3 */
#define BIT_SYS_MSC0CFG_UART3_INTSEL  (24)
#define MASK_SYS_MSC0CFG_UART3_INTSEL (0x1 << BIT_SYS_MSC0CFG_UART3_INTSEL)

/** @brief VBUS detection enable
 *  Write 1 to switch on VBUS to the detection circuitry.
 *  Write 0 to cut-off VBUS from the detection circuitry.
 *  */
#define BIT_SYS_MSC0CFG_USB_VBUS_EN  (22)
#define MASK_SYS_MSC0CFG_USB_VBUS_EN (0x1 << BIT_SYS_MSC0CFG_USB_VBUS_EN)

#else

    /** @brief Swap I2C master and slave
 *   1 : swop the I2C master and I2C slave pad positions */
#define BIT_SYS_MSC0CFG_I2C_SWOP  (29)
#define MASK_SYS_MSC0CFG_I2C_SWOP (0x1 << BIT_SYS_MSC0CFG_I2C_SWOP)

/** @brief PWM count external trigger selection
 *  If any of the GPIO is used for this purpose, the pad must be configured solely for this use.
 *  - 0 : none
 *  - 1 : gpio 18
 *  - 2 : gpio 26
 *  - 3 : gpio 35
 *  - 4 : gpio 40
 *  - 5 : gpio 46
 *  - 6 : gpio 52
 *  - 7 : gpio 58 */
#define BIT_SYS_MSC0CFG_PWM_TRIG_SEL  (26)
#define MASK_SYS_MSC0CFG_PWM_TRIG_SEL (0x7 << BIT_SYS_MSC0CFG_PWM_TRIG_SEL)

/** @brief CAN 0 divider
 *  1 : Extend further the divider of CAN 0 by a  factor of 16. */
#define BIT_SYS_MSC0CFG_CAN0_SLOW  (25)
#define MASK_SYS_MSC0CFG_CAN0_SLOW (0x1 << BIT_SYS_MSC0CFG_CAN0_SLOW)

/** @brief CAN 1 divider
 *  1 : Extend further the divider of CAN 1 by a  factor of 16. */
#define BIT_SYS_MSC0CFG_CAN1_SLOW  (24)
#define MASK_SYS_MSC0CFG_CAN1_SLOW (0x1 << BIT_SYS_MSC0CFG_CAN1_SLOW)

#endif

/** @brief HIGH SPEED MODE monitoring (Read Only)
 *  Available for FT930 and FT900 Rev.C ONLY. This Bit is reserved for FT900 Rev B.
 *  1: it indicates USB is in high speed mode.
 *  0: it indicates USB is in full speed mode.
 */
#define BIT_SYS_MSC0CFG_HIGH_SPED_MODE  (23)
#define MASK_SYS_MSC0CFG_HIGH_SPED_MODE (0x1 << BIT_SYS_MSC0CFG_HIGH_SPED_MODE)

/** Clock Select for UART1 */
#define BIT_SYS_MSC0CFG_UART1_CLKSEL  (21)
#define MASK_SYS_MSC0CFG_UART1_CLKSEL (0x1 << BIT_SYS_MSC0CFG_UART1_CLKSEL)

/** FIFO Select for UART1 */
#define BIT_SYS_MSC0CFG_UART1_FIFOSEL  (20)
#define MASK_SYS_MSC0CFG_UART1_FIFOSEL (0x1 << BIT_SYS_MSC0CFG_UART1_FIFOSEL)

/** INT Select for UART1 */
#define BIT_SYS_MSC0CFG_UART1_INTSEL  (19)
#define MASK_SYS_MSC0CFG_UART1_INTSEL (0x1 << BIT_SYS_MSC0CFG_UART1_INTSEL)

/** Clock Select for UART2 */
#define BIT_SYS_MSC0CFG_UART2_CLKSEL  (18)
#define MASK_SYS_MSC0CFG_UART2_CLKSEL (0x1 << BIT_SYS_MSC0CFG_UART2_CLKSEL)

/** FIFO Select for UART2 */
#define BIT_SYS_MSC0CFG_UART2_FIFOSEL  (17)
#define MASK_SYS_MSC0CFG_UART2_FIFOSEL (0x1 << BIT_SYS_MSC0CFG_UART2_FIFOSEL)

/** INT Select for UART2 */
#define BIT_SYS_MSC0CFG_UART2_INTSEL  (16)
#define MASK_SYS_MSC0CFG_UART2_INTSEL (0x1 << BIT_SYS_MSC0CFG_UART2_INTSEL)

/** @brief USB Host EHCI and USB Host ATX Reset
 *  Write 1 to cause USB Host EHCI and ATX reset; it is automatically cleared immediately.
 *  Software needs to wait for EHCI to complete its reset (~200ms). */
#define BIT_SYS_MSC0CFG_HOST_RESET_ALL  (15)
#define MASK_SYS_MSC0CFG_HOST_RESET_ALL (0x1 << BIT_SYS_MSC0CFG_HOST_RESET_ALL)

/** @brief USB Host EHCI Reset.
 *  Write 1 to cause USB Host EHCI reset; it is automatically cleared immediately.
 *  Software needs to wait for EHCI to complete its reset (~200ms). */
#define BIT_SYS_MSC0CFG_HOST_RESET_EHCI  (14)
#define MASK_SYS_MSC0CFG_HOST_RESET_EHCI (0x1 << BIT_SYS_MSC0CFG_HOST_RESET_EHCI)

/** @brief USB Host ATX Reset
 *  Write 1 to cause USB Host ATX reset; it is automatically cleared
        immediately.
**/
#define BIT_SYS_MSC0CFG_HOST_RESET_ATX  (13)
#define MASK_SYS_MSC0CFG_HOST_RESET_ATX (0x1 << BIT_SYS_MSC0CFG_HOST_RESET_ATX)

/** @brief USB Device Wakeup
 *  1 : Drive K-state on Device USB port; software must maintain the 1ms requirement before turning it off. */
#define BIT_SYS_MSC0CFG_DEV_RMWAKEUP  (12)
#define MASK_SYS_MSC0CFG_DEV_RMWAKEUP (0x1 << BIT_SYS_MSC0CFG_DEV_RMWAKEUP)

/** @brief USB Device and USB Device ATX Reset
 *  Write 1 to cause USB Dev Controller and ATX reset; it is automatically cleared immediately. */
#define BIT_SYS_MSC0CFG_DEV_RESET_ALL  (11)
#define MASK_SYS_MSC0CFG_DEV_RESET_ALL (0x1 << BIT_SYS_MSC0CFG_DEV_RESET_ALL)

/** @brief USB Device Reset
 *  Write 1 to cause USB Dev Controller reset; it is automatically cleared immediately. */
#define BIT_SYS_MSC0CFG_DEV_RESET_CONTROLLER  (10)
#define MASK_SYS_MSC0CFG_DEV_RESET_CONTROLLER (0x1 << BIT_SYS_MSC0CFG_DEV_RESET_CONTROLLER)

/** @brief USB Device ATX Reset
 *  Write 1 to cause USB Dev ATX reset; it is automatically cleared immediately. */
#define BIT_SYS_MSC0CFG_DEV_RESET_ATX  (9)
#define MASK_SYS_MSC0CFG_DEV_RESET_ATX (0x1 << BIT_SYS_MSC0CFG_DEV_RESET_ATX)

/** @brief Ethernet PHY Reset
 *  Write 1 to cause Ethernet phy reset; it is automatically cleared immediately. */
#define BIT_SYS_MSC0CFG_MAC_RESET_PHY  (8)
#define MASK_SYS_MSC0CFG_MAC_RESET_PHY (0x1 << BIT_SYS_MSC0CFG_MAC_RESET_PHY)

/** @brief BCD Host Mode
 *  - 0 : SDP
 *  - 1 : DCP
 *  - 2 : CDPI
 *  - 3 : CDPII */
#define BIT_SYS_MSC0CFG_BCDHOST_MODE  (6)
#define MASK_SYS_MSC0CFG_BCDHOST_MODE (0x3 << BIT_SYS_MSC0CFG_BCDHOST_MODE)

/** @brief Enable BCD Host
 *  1 : enable BCD Host */
#define BIT_SYS_MSC0CFG_BCDHOST_EN  (5)
#define MASK_SYS_MSC0CFG_BCDHOST_EN (0x1 << BIT_SYS_MSC0CFG_BCDHOST_EN)

/** @brief BCD Host Software Reset (Write)
 *  - 1 : Generate software reset to
 *    + BCD Host : if BCDHOST_EN = 1
 *    + BCD Dev : if BCDDEV_EN = 1
 *  It is automatically cleared immediately */
#define BIT_SYS_MSC0CFG_BCD_SOFTRESET  (4)
#define MASK_SYS_MSC0CFG_BCD_SOFTRESET (0x1 << BIT_SYS_MSC0CFG_BCD_SOFTRESET)

/** @brief BCD Device Detection Running (Read)
 *  1 : indicates BCD Device detection is running */
#define BIT_SYS_MSC0CFG_BCDDEV_DETECT_RUNNING  (4)
#define MASK_SYS_MSC0CFG_BCDDEV_DETECT_RUNNING (0x1 << BIT_SYS_MSC0CFG_BCDDEV_DETECT_RUNNING)

/** @brief Enable BCD Device (Write)
 *  1 : enable BCD Device */
#define BIT_SYS_MSC0CFG_BCDDEV_EN  (3)
#define MASK_SYS_MSC0CFG_BCDDEV_EN (0x1 << BIT_SYS_MSC0CFG_BCDDEV_EN)

/** @brief BCD Device Detection Complete (Read)
 *  1 : indicates BCD Device detection is done */
#define BIT_SYS_MSC0CFG_BCDDEV_DETECT_COMPLETE  (3)
#define MASK_SYS_MSC0CFG_BCDDEV_DETECT_COMPLETE (0x1 << BIT_SYS_MSC0CFG_BCDDEV_DETECT_COMPLETE)

/** @brief Enable BCD Secondary Detection (Write)
 *  1 : enable secondary detection. */
#define BIT_SYS_MSC0CFG_BCDDEV_SD_EN  (2)
#define MASK_SYS_MSC0CFG_BCDDEV_SD_EN (0x1 << BIT_SYS_MSC0CFG_BCDDEV_SD_EN)

/** @brief BCD SDP Detected (Read)
 *  1 : SDP detected. */
#define BIT_SYS_MSC0CFG_BCDDEV_SDP_FOUND  (2)
#define MASK_SYS_MSC0CFG_BCDDEV_SDP_FOUND (0x1 << BIT_SYS_MSC0CFG_BCDDEV_SDP_FOUND)

/** @brief BCD Connection Enable (Write)
 *  1 : enable connection of VDP_SRC after DCP detection. */
#define BIT_SYS_MSC0CFG_BCDDEV_VDP_EN_POST_DCP  (1)
#define MASK_SYS_MSC0CFG_BCDDEV_VDP_EN_POST_DCP (0x1 << BIT_SYS_MSC0CFG_BCDDEV_VDP_EN_POST_DCP)

/** @brief BCD CPD Detected (Read)
 *  1 : CDP detected. */
#define BIT_SYS_MSC0CFG_BCDDEV_CDP_FOUND  (1)
#define MASK_SYS_MSC0CFG_BCDDEV_CDP_FOUND (0x1 << BIT_SYS_MSC0CFG_BCDDEV_CDP_FOUND)

/** @brief BCD Logic Comparison
 *  1 : disable logic comparison during BCD detections.*/
#define BIT_SYS_MSC0CFG_BCDDEV_LGC_COMP_INHIB  (0)
#define MASK_SYS_MSC0CFG_BCDDEV_LGC_COMP_INHIB (0x1 << BIT_SYS_MSC0CFG_BCDDEV_LGC_COMP_INHIB)
/** @brief BCD DCP Detected (Read)
 *  1 : DCP detected */
#define BIT_SYS_MSC0CFG_BCDDEV_DCP_FOUND  (0)
#define MASK_SYS_MSC0CFG_BCDDEV_DCP_FOUND (0x1 << BIT_SYS_MSC0CFG_BCDDEV_DCP_FOUND)


#if defined(__FT930__)
/** 1 : WatchDog Second Level Reset;
 * if this bit is set it indicates the second level watchdog reset has
 *    occurred in the previous operation.
 *      Write “1” to clear this bit.
 *      !!! Please note this bit must be cleared to enable this function again !!! */
#define BIT_SYS_GLOBAL_RST_WDG_RESET  (2)
#define MASK_SYS_GLOBAL_RST_WDG_RESET (0x1 << BIT_SYS_GLOBAL_RST_WDG_RESET)
#endif

/* Below defines are: 
 * !!!AVAILABLE FROM FT900 REV C ONWARDS!!!
 */
/** 1 : Soft Global Reset;
 * write “1” to reset the system as to POR state.
 * Reading “1” from this bit indicates a soft global reset has been performed from the previous operation.
 * Write “0” to clear this bit.
 * NOTE: This bit must be cleared before it can be enabled again.
 */
#define BIT_SYS_GLOBAL_RST_SOFT_RESET  (1)
#define MASK_SYS_GLOBAL_RST_SOFT_RESET (0x1 << BIT_SYS_GLOBAL_RST_SOFT_RESET)

/** 1 : External Hardware Reset;
 * if it is set it indicates the external reset has been activated.
 * Write “1” to clear it. */
#define BIT_SYS_GLOBAL_RST_HW_RESET  (0)
#define MASK_SYS_GLOBAL_RST_HW_RESET (0x1 << BIT_SYS_GLOBAL_RST_HW_RESET)


/* TYPES ***************************************************************************/
/** @brief Register mappings for System level registers */
typedef struct
{
	volatile uint32_t HIPID;
	volatile uint32_t EFCFG;
	volatile uint32_t CLKCFG;
	volatile uint16_t PMCFG_L;
	volatile uint16_t PMCFG_H;
	volatile uint32_t PTSTNSET;
	volatile uint32_t PTSTNSETR;
	volatile uint32_t MSC0CFG;
} ft900_sys_regs_t;

typedef struct
{
	volatile uint8_t RESET_STATUS;
	volatile uint8_t RESERVED1;
	volatile uint8_t RESERVED2;
	volatile uint8_t RESERVED3;
}
ft900_sys_reset_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_SYS_REGISTERS_H_ */



