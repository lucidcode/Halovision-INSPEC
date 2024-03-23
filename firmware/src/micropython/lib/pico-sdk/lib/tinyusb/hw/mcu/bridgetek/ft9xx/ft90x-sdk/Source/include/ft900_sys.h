/**
    @file ft900_sys.h

    @brief
    Chip management


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

#ifndef FT900_SYS_H_
#define FT900_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/** @brief FT930 Device */
#if defined(__FT930__)

typedef enum
{
/* NOTE: The order of the enums here *must* match the order of enable bits in the SYS->CLKCFG register */
	sys_device_uart2       = 0,  /**< Camera */
	sys_device_uart3          = 1,  /**< I2S */
	sys_device_pwm          = 2,  /**< PWM */
	sys_device_uart1        = 3,  /**< UART1 */
	sys_device_uart0        = 4,  /**< UART0 */
	sys_device_spi_slave0   = 6,  /**< SPI Slave 0 */
	sys_device_spi_master   = 7,  /**< SPI Master */
	sys_device_i2c_slave    = 8,  /**< I2C Slave */
	sys_device_i2c_master   = 9,  /**< I2C Master */
	sys_device_sd_card      = 12, /**< SD Card */
	sys_device_usb_device   = 14, /**< USB Device */

/* The order of the next enums does not matter */
	sys_device_timer_wdt    = 16, /**< Timer and Watchdog Timer */
	sys_device_adc          = 17, /**< Analogue to Digital Converter */
	sys_device_dac0         = 18, /**< Digital to Analogue Converter 0 */
	sys_device_dac1         = 19  /**< Digital to Analogue Converter 1 */
} sys_device_t;

#else
/** @brief FT900 Device */
typedef enum
{
/* NOTE: The order of the enums here *must* match the order of enable bits in the SYS->CLKCFG register */
	sys_device_camera       = 0,  /**< Camera */
	sys_device_i2s          = 1,  /**< I2S */
	sys_device_pwm          = 2,  /**< PWM */
	sys_device_uart1        = 3,  /**< UART1 */
	sys_device_uart0        = 4,  /**< UART0 */
	sys_device_spi_slave1   = 5,  /**< SPI Slave 1 */
	sys_device_spi_slave0   = 6,  /**< SPI Slave 0 */
	sys_device_spi_master   = 7,  /**< SPI Master */
	sys_device_i2c_slave    = 8,  /**< I2C Slave */
	sys_device_i2c_master   = 9,  /**< I2C Master */
	sys_device_can1         = 10, /**< CAN1 */
	sys_device_can0         = 11, /**< CAN0 */
	sys_device_sd_card      = 12, /**< SD Card */
	sys_device_ethernet     = 13, /**< Ethernet */
	sys_device_usb_device   = 14, /**< USB Device */
	sys_device_usb_host     = 15, /**< USB Host */

/* The order of the next enums does not matter */
	sys_device_timer_wdt    = 16, /**< Timer and Watchdog Timer */
	sys_device_adc          = 17, /**< Analogue to Digital Converter */
	sys_device_dac0         = 18, /**< Digital to Analogue Converter 0 */
	sys_device_dac1         = 19  /**< Digital to Analogue Converter 1 */
} sys_device_t;

#endif


/** @brief CPU Clock divider */
typedef enum
{
    sys_cpu_divider_1,      /**< No clock divider (Default) */
    sys_cpu_divider_2,      /**< Divide Input Clock by 2 */
    sys_cpu_divider_4,      /**< Divide Input Clock by 4 */
    sys_cpu_divider_8,      /**< Divide Input Clock by 8 */
    sys_cpu_divider_64,     /**< Divide Input Clock by 64 */
    sys_cpu_divider_128,    /**< Divide Input Clock by 128 */
    sys_cpu_divider_512     /**< Divide Input Clock by 512 */
} sys_cpu_divider_t;

#if !defined(__FT930__)
/** @brief PWM External Trigger pin */
typedef enum
{
    sys_pwm_trigger_none    = 0,    /**< None */
    sys_pwm_trigger_gpio18  = 1,    /**< GPIO18 */
    sys_pwm_trigger_gpio26  = 2,    /**< GPIO26 */
    sys_pwm_trigger_gpio35  = 3,    /**< GPIO35 */
    sys_pwm_trigger_gpio40  = 4,    /**< GPIO40 */
    sys_pwm_trigger_gpio46  = 5,    /**< GPIO46 */
    sys_pwm_trigger_gpio52  = 6,    /**< GPIO52 */
    sys_pwm_trigger_gpio58  = 7     /**< GPIO58 */
} sys_pwm_trigger_t;
#endif

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/
/** @brief Check whether the revision of FT900 is RevB and return True
 *  @param None
 *  @returns True if Rev B, otherwise (if Rev C or more, or FT930) returns False
 */
#define sys_check_ft900_revB()	(((SYS->HIPID | 0x000F0000U) == 0x090F0000U) || ((SYS->HIPID | 0x000F0000U) == 0x090F0001U))

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Enable a device on the FT900
 *  @param dev The device to enable
 *  @returns On success a 0, otherwise -1
 */
int sys_enable(sys_device_t dev);

/** @brief Disable a device on the FT900
 *  @param dev The device to Disable
 *  @returns On success a 0, otherwise -1
 */
int sys_disable(sys_device_t dev);

/** @brief Enable a divider on the CPU
 *  @param div The divider to use
 *  @returns On success a 0, otherwise -1
 *  Note: Only applicable for FT930 and FT900 Rev C onwards
 */
int sys_cpu_clock_div(sys_cpu_divider_t div);

/** @brief Get the current clock of the CPU
 *  @returns The clock rate of the CPU in Hertz
 */
uint32_t sys_get_cpu_clock(void);

#if !defined(__FT930__)
/** @brief Swap the I2C Master and slave pins
 *  @param swop Enable or disable the swop feature (Only for FT90x)
 *  @return  On success a 0, otherwise -1
 */
int sys_i2c_swop(uint8_t swop);

/** @brief Configure the External PWM trigger
 *  @param exttrigger The selection of external trigger
 *  @return  On success a 0, otherwise -1
 */
int sys_pwm_ext_trigger(sys_pwm_trigger_t exttrigger);

#endif
/** @brief Reset all peripherals */
void sys_reset_all(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_SYS_H_ */
