/**
    @file ft900_interrupt.h

    @brief
    Interrupt management

    
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


#ifndef FT900_INTERRUPT_H_
#define FT900_INTERRUPT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/
#define N_INTERRUPTS    (34)    /* includes watch dog interrupt */
#define INTERRUPTS_INLINE_ENABLE

/* TYPES ***************************************************************************/
typedef void(*isrptr_t)(void);

#if defined(__FT930__)

/** @brief Interrupt vector */
typedef enum
{
    interrupt_0                 = 0,  /*!< Power Management */
    interrupt_1                 = 1,  /*!< Reserved */
    interrupt_usb_device        = 2,  /*!< USB Device Interrupt */
    interrupt_3                 = 3,  /*!< Ethernet Interrupt */
    interrupt_sd_card           = 4,  /*!< SD Card Interrupt */
    interrupt_mailbox_source    = 5,  /*!< Mailbox Source Interrupt */
    interrupt_mailbox_dest      = 6,  /*!< Mailbox Destination Interrupt */
    interrupt_uart3             = 7,  /*!< UART3 Interrupt */
    interrupt_spim              = 8,  /*!< SPI Master Interrupt */
    interrupt_spis0             = 9,  /*!< SPI Slave Interrupt */
    interrupt_10                = 10, /*!< Reserved*/
    interrupt_i2cm              = 11, /*!< I2C Master Interrupt */
    interrupt_i2cs              = 12, /*!< I2C Slave Interrupt */
    interrupt_uart0             = 13, /*!< UART0 Interrupt */
    interrupt_uart1             = 14, /*!< UART1 Interrupt */
    interrupt_uart2             = 15, /*!< UART2 Interrupt */
    interrupt_pwm               = 16, /*!< PWM Interrupt */
    interrupt_timers            = 17, /*!< Timers Interrupt */
    interrupt_gpio              = 18, /*!< GPIO Interrupt */
    interrupt_rtc               = 19, /*!< RTC Interrupt */
    interrupt_adc               = 20, /*!< ADC Interrupt */
    interrupt_dac               = 21, /*!< DAC Interrupt */
    interrupt_slowclock         = 22, /*!< Slow Clock Timer Interrupt */
    interrupt_7channel_fifo     = 23, /*!< 7 Channel Fifo Interrupt */
    interrupt_max               = 24, /*!< Interrupt controller takes in 32 interrupts, in which 24 to 31 are unused by default*/
	interrupt_wdg				= 33  /*!< Watchdog vector !!!USAGE ONLY WITHIN FT9xx LIBRARY!!! */
    
    
} interrupt_t;

#else

/** @brief Interrupt vector */
typedef enum
{
    interrupt_0          =  0,  /*!< Power Management */
    interrupt_usb_host   =  1,  /*!< USB Host Interrupt */
    interrupt_usb_device =  2,  /*!< USB Device Interrupt */
    interrupt_ethernet   =  3,  /*!< Ethernet Interrupt */
    interrupt_sd_card    =  4,  /*!< SD Card Interrupt */
    interrupt_can0       =  5,  /*!< CAN0 Interrupt */
    interrupt_can1       =  6,  /*!< CAN1 Interrupt */
    interrupt_camera     =  7,  /*!< Camera Interrupt */
    interrupt_spim       =  8,  /*!< SPI Master Interrupt */
    interrupt_spis0      =  9,  /*!< SPI Slave 0 Interrupt */
    interrupt_spis1      = 10,  /*!< SPI Slave 1 Interrupt */
    interrupt_i2cm       = 11,  /*!< I2C Master Interrupt */
    interrupt_i2cs       = 12,  /*!< I2C Slave Interrupt */
    interrupt_uart0      = 13,  /*!< UART0 Interrupt */
    interrupt_uart1      = 14,  /*!< UART1 Interrupt */
    interrupt_i2s        = 15,  /*!< I2S Interrupt */
    interrupt_pwm        = 16,  /*!< PWM Interrupt */
    interrupt_timers     = 17,  /*!< Timers Interrupt */
    interrupt_gpio       = 18,  /*!< GPIO Interrupt */
    interrupt_rtc        = 19,  /*!< RTC Interrupt */
    interrupt_adc        = 20,  /*!< ADC Interrupt */
    interrupt_dac        = 21,  /*!< DAC Interrupt */
    interrupt_slowclock  = 22,  /*!< Slow Clock Timer Interrupt */

    interrupt_max        = 23,  /*!< Interrupt controller takes in 32 interrupts, in which 23 to 31 are unused by default*/
	interrupt_wdg		 = 33   /*!< Watchdog vector !!!USAGE ONLY WITHIN FT9xx LIBRARY!!! */
} interrupt_t;

#endif /* __FT930__ */



/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Attach an interrupt.
 *   NOTE: Interrupt enable of a module shall not be called prior to interrupt attach
 *         or else it will lead to a hang.
 *  @param interrupt The interrupt number for which the interrupt vector need to be attached
 *  @param priority The priority to give the interrupt. The vector table is indexed on the priority.
 *  @param func The function to call when interrupted
 *  @returns 0 on a success or -1 for a failure
 */
int8_t interrupt_attach(interrupt_t interrupt, uint8_t priority, isrptr_t func);

/** @brief Detach an interrupt
 *  @param interrupt The interrupt vector to detach
 *  @returns 0 on a success or -1 for a failure
 */
int8_t interrupt_detach(interrupt_t interrupt);

/** @brief Enable interrupts to fire
 */
#ifndef INTERRUPTS_INLINE_ENABLE
void interrupt_enable_globally(void);
#else // INTERRUPTS_INLINE_ENABLE
inline void interrupt_enable_globally(void)
{
	INTERRUPT->global_mask &= ~(MASK_IRQ_CTRL_MASKALL);
};
#endif // INTERRUPTS_INLINE_ENABLE

/** @brief Disable all interrupts
 */
#ifndef INTERRUPTS_INLINE_ENABLE
void interrupt_disable_globally(void);
#else // INTERRUPTS_INLINE_ENABLE
inline void interrupt_disable_globally(void)
{
    INTERRUPT->global_mask |= MASK_IRQ_CTRL_MASKALL;
    INTERRUPT->global_mask |= MASK_IRQ_CTRL_MASKALL;
    INTERRUPT->global_mask |= MASK_IRQ_CTRL_MASKALL;
};
#endif // INTERRUPTS_INLINE_ENABLE

/** @brief Enable nesting interrupts
 *  @param max The maximum number of levels to nest (range: 1 to 16)
 *  @returns 0 on a success or -1 for a failure
 */
int8_t interrupt_enable_nesting(uint8_t max);

/** @brief Disable nesting interrupts
 *  @returns 0 on a success or -1 for a failure
 */
int8_t interrupt_disable_nesting(void);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_INTERRUPT_H_ */
