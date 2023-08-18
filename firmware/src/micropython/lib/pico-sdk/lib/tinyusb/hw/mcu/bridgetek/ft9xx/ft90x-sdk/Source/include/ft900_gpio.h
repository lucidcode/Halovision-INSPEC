/**
    @file ft900_gpio.h

    @brief
    General Purpose I/O and Pad control

	@details
	
    | Pin                              | pad_func_0     | pad_func_1     | pad_func_2     | pad_func_3     |
    | -------------------------------- | -------------- | -------------- | -------------- | -------------- |
    | VBUS_DISCH/GPIO0                 | GPIO0          |                |                |                |
    | OC_N/GPIO1                       | GPIO1          | OC_N           |                |                |
    | PSW_N/GPIO2                      | GPIO2          |                |                |                |
    | VBUS_DTC/GPIO3                   | GPIO3          | VBUS_DTC       |                |                |
    | ENET_LED0/GPIO4                  | GPIO4          | ENET_LED0      |                |                |
    | ENET_LED1/GPIO5                  | GPIO5          | ENET_LED1      |                |                |
    | ADC1/CAM_XCLK/GPIO6              | GPIO6          | CAM_XCLK       |                | ADC1           |
    | ADC2/CAM_PCLK/GPIO7              | GPIO7          | CAM_PCLK       |                | ADC2           |
    | ADC3/CAM_VD/GPIO8                | GPIO8          | CAM_VD         |                | ADC3           |
    | ADC4/CAM_HD/GPIO9                | GPIO9          | CAM_HD         |                | ADC4           |
    | ADC5/CAM_D7/GPIO10               | GPIO10         | CAM_D7         |                | ADC5           |
    | ADC6/CAM_D6/GPIO11               | GPIO11         | CAM_D6         |                | ADC6           |
    | ADC7/CAM_D5/GPIO12               | GPIO12         | CAM_D5         |                | ADC7           |
    | DAC1/CAM_D4/GPIO13               | GPIO13         | CAM_D4         |                | DAC1           |
    | DAC0/CAM_D3/GPIO14               | GPIO14         | CAM_D3         |                | DAC0           |
    | CAN0_TXD/CAM_D2/GPIO15           | GPIO15         | CAM_D2         | CAN0_TXD       |                |
    | CAN0_RXD/CAM_D1/GPIO16           | GPIO16         | CAM_D1         | CAN0_RXD       |                |
    | CAN1_TXD/CAM_D0/GPIO17           | GPIO17         | CAM_D0         | CAN1_TXD       |                |
    | CAN1_RXD/GPIO18                  | GPIO18         |                | CAN1_RXD       |                |
    | SD_CLK/GPIO19                    | GPIO19         | SD_CLK         |                |                |
    | SD_CMD/GPIO20                    | GPIO20         | SD_CMD         |                |                |
    | SD_DATA3/GPIO21                  | GPIO21         | SD_DATA3       |                |                |
    | SD_DATA2/GPIO22                  | GPIO22         | SD_DATA2       |                |                |
    | SD_DATA1/GPIO23                  | GPIO23         | SD_DATA1       |                |                |
    | SD_DATA0/GPIO24                  | GPIO24         | SD_DATA0       |                |                |
    | SD_CD/GPIO25                     | GPIO25         | SD_CD          |                |                |
    | SD_WP/GPIO26                     | GPIO26         | SD_WP          |                |                |
    | SPIM_CLK/GPIO27                  | GPIO27         | SPIM_CLK       |                |                |
    | SPIM_SS0/GPIO28                  | GPIO28         | SPIM_SS0       |                |                |
    | SPIM_MOSI/GPIO29                 | GPIO29         | SPIM_MOSI      |                |                |
    | SPIM_MISO/GPIO30                 | GPIO30         | SPIM_MISO      |                |                |
    | SPIM_IO2/GPIO31                  | GPIO31         | SPIM_IO2       |                |                |
    | SPIM_IO3/GPIO32                  | GPIO32         | SPIM_IO3       |                |                |
    | SPIM_SS1/GPIO33                  | GPIO33         | SPIM_SS1       |                |                |
    | SPIM_SS2/GPIO34                  | GPIO34         | SPIM_SS2       |                |                |
    | SPIM_SS3/GPIO35                  | GPIO35         | SPIM_SS3       |                |                |
    | SPIS0_CLK/GPIO36                 | GPIO36         | SPIS0_CLK      |                |                |
    | SPIS0_SS/GPIO37                  | GPIO37         | SPIS0_SS       |                |                |
    | SPIS0_MOSI/GPIO38                | GPIO38         | SPIS0_MOSI     |                |                |
    | SPIS0_MISO/GPIO39                | GPIO39         | SPIS0_MISO     |                |                |
    | SPIS1_CLK/GPIO40                 | GPIO40         | SPIS1_CLK      |                |                |
    | SPIS1_SS/GPIO41                  | GPIO41         | SPIS1_SS       |                |                |
    | SPIS1_MOSI/GPIO42                | GPIO42         | SPIS1_MOSI     |                |                |
    | SPIS1_MISO/GPIO43                | GPIO43         | SPIS1_MISO     |                |                |
    | I2C0_SCL/GPIO44                  | GPIO44         | I2C0_SCL       |                |                |
    | I2C0_SDA/GPIO45                  | GPIO45         | I2C0_SDA       |                |                |
    | I2C1_SCL/GPIO46                  | GPIO46         | I2C1_SCL       |                |                |
    | I2C1_SDA/GPIO47                  | GPIO47         | I2C1_SDA       |                |                |
    | UART0_TXD/GPIO48                 | GPIO48         |                |                | UART0_TXD      |
    | UART0_RXD/GPIO49                 | GPIO49         |                |                | UART0_RXD      |
    | UART0_RTS/GPIO50                 | GPIO50         |                |                | UART0_RTS      |
    | UART0_CTS/GPIO51                 | GPIO51         |                |                | UART0_CTS      |
    | UART0_DTR/UART1_TXD/PWM4/GPIO52  | GPIO52         | PWM4           | UART1_TXD      | UART0_DTR      |
    | UART0_DSR/UART1_RXD/PWM5/GPIO53  | GPIO53         | PWM5           | UART1_RXD      | UART0_DSR      |
    | UART0_DCD/UART1_RTS/PWM6/GPIO54  | GPIO54         | PWM6           | UART1_RTS      | UART0_DCD      |
    | UART0_RI/UART1_CTS/PWM7/GPIO55   | GPIO55         | PWM7           | UART1_CTS      | UART0_RI       |
    | PWM0/GPIO56                      | GPIO56         | PWM0           |                |                |
    | PWM1/GPIO57                      | GPIO57         | PWM1           |                |                |
    | PWM2/GPIO58                      | GPIO58         | PWM2           |                |                |
    | PWM3/GPIO59                      | GPIO59         | PWM3           |                |                |
    | I2S_SDAO/GPIO60                  | GPIO60         | I2S_SDAO       |                |                |
    | I2S_SDAI/GPIO61                  | GPIO61         | I2S_SDAI       |                |                |
    | I2S_BCLK/GPIO62                  | GPIO62         | I2S_BCLK       | I2SS_BCLK      |                |
    | I2S_LRCLK/GPIO63                 | GPIO63         | I2S_LRCLK      | I2SS_LRCLK     |                |
    | I2S_MCLK/GPIO64                  | GPIO64         | I2S_MCLK       |                |                |
    | I2S_CLK22/GPIO65                 | GPIO65         | I2S_CLK22      |                |                |
    | I2S_CLK24/GPIO66                 | GPIO66         | I2S_CLK24      |                |                |
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


#ifndef FT900_GPIO_H_
#define FT900_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/
/** @brief Pad slew rate control. */
typedef enum
{
    pad_slew_fast,  /**< Fast Slew Rate */
    pad_slew_slow   /**< Slow Slew Rate */
} pad_slew_t;

/** @brief Pad Schmitt trigger control. */
typedef enum
{
    pad_schmitt_off,    /**< Pad input is filtered through a schmitt trigger */
    pad_schmitt_on      /**< Pad input is unfiltered */
} pad_schmitt_t;

/** @brief Pad pull up and pull downs control. */
typedef enum
{
    pad_pull_none,      /**< No pull up or pull down */
    pad_pull_pullup,    /**< Weak pull up enabled */
    pad_pull_pulldown,  /**< Weak pull down enabled */
    pad_pull_keeper     /**< Weak pull up/down reflects output */
} pad_pull_t;

/** @brief Pad current drive control. */
typedef enum
{
    pad_drive_4mA,  /**< 4mA maximum current */
    pad_drive_8mA,  /**< 8mA maximum current */
    pad_drive_12mA, /**< 12mA maximum current */
    pad_drive_16mA  /**< 16mA maximum current */
} pad_drive_t;

/** @brief Pad function control. */

#if defined(__FT930__)
typedef enum
{
    pad_func_0      = 0, /**< Pad function 0 */
    pad_func_1      = 1, /**< Pad function 1 */
    pad_func_2      = 2, /**< Pad function 2 */
    pad_func_3      = 3, /**< Pad function 3 */

    pad_gpio0       = 0, /**< SD_CLK/SPIS_CLK/GPIO0 Pad function 0 */
    pad_gpio1       = 0, /**< SD_CMD/SPIS_MISO/GPIO1 Pad function 0 */
    pad_gpio2       = 0, /**< SD_CD/SPIS_MOSI/GPIO2 Pad function 0 */
    pad_gpio3       = 0, /**< SD_DATA0/SPIS_SS/GPIO3 Pad function 0 */
    pad_gpio4       = 0, /**< PWM0/SD_DATA1/PWM7/GPIO4 Pad function 0 */
    pad_gpio5       = 0, /**< PWM6/SD_DATA2/PWM1/GPIO5 Pad function 0 */
    pad_gpio6       = 0, /**< SD_DATA3/PWM5/GPIO6 Pad function 0 */
    pad_gpio7       = 0, /**< SD_WP/PWM4/GPIO7 Pad function 0 */
    pad_gpio8       = 0, /**< PWM3/GPIO8 Pad function 0 */
    pad_gpio9       = 0, /**< PWM2/GPIO9 Pad function 0 */
    pad_gpio10      = 0, /**< PWM1/GPIO10 Pad function 0 */
    pad_gpio11      = 0, /**< PWM0/GPIO11 Pad function 0 */
    pad_gpio12      = 0, /**< I2CS_SCL/I2CM_SCL/GPIO12 Pad function 0 */
    pad_gpio13      = 0, /**< I2CS_SDA/I2CM_SDA/GPIO13 Pad function 0 */
    pad_gpio14      = 0, /**< UART2_RXD/GPIO14 Pad function 0 */
    pad_gpio15      = 0, /**< UART2_TXD/GPIO15 Pad function 0 */
    pad_gpio16      = 0, /**< UART2_RTS/GPIO16 Pad function 0 */
    pad_gpio17      = 0, /**< UART2_CTS/GPIO17 Pad function 0 */
    pad_gpio18      = 0, /**< UART3_RXD/UART2_DTR/GPIO18 Pad function 0 */
    pad_gpio19      = 0, /**< UART3_TXD/UART2_DSR/GPIO19 Pad function 0 */
    pad_gpio20      = 0, /**< UART3_RTS/UART2_DCD/GPIO20 Pad function 0 */
    pad_gpio21      = 0, /**< UART3_CTS/UART2_RI/GPIO21 Pad function 0 */
    pad_gpio22      = 0, /**< PWM3/UART0_RXD/GPIO22 Pad function 0 */
    pad_gpio23      = 0, /**< PWM2/UART0_TXD/GPIO23 Pad function 0 */
    pad_gpio24      = 0, /**< PWM1/UART0_RTS/GPIO24 Pad function 0 */
    pad_gpio25      = 0, /**< PWM0/UART0_CTS/GPIO25 Pad function 0 */
    pad_gpio26      = 0, /**< UART1_RXD/UART0_DTR/GPIO26 Pad function 0 */
    pad_gpio27      = 0, /**< UART1_TXD/UART0_DSR/GPIO27 Pad function 0 */
    pad_gpio28      = 0, /**< UART1_RTS/UART0_DCD/GPIO28 Pad function 0 */
    pad_gpio29      = 0, /**< SPIM_SS0/UART1_CTS/UART0_RI/GPIO29 Pad function 0 */
    pad_gpio30      = 0, /**< SPIM_SS0/GPIO30 Pad function 0 */
    pad_gpio31      = 0, /**< SPIM_SS1/GPIO31 Pad function 0 */
    pad_gpio32      = 0, /**< SPIM_SS2/GPIO32 Pad function 0 */
    pad_gpio33      = 0, /**< SPIM_SS3/GPIO33 Pad function 0 */
    pad_gpio34      = 0, /**< SPIS_CLK/SPIM_CLK/GPIO34 Pad function 0 */
    pad_gpio35      = 0, /**< SPIS_MISO/SPIM_MISO/GPIO35 Pad function 0 */
    pad_gpio36      = 0, /**< SPIS_MOSI/SPIM_MOSI/GPIO36 Pad function 0 */
    pad_gpio37      = 0, /**< SPIS_SS/SPIM_IO2/GPIO37 Pad function 0 */
    pad_gpio38      = 0, /**< RTC_REF/SPIM_IO3/GPIO38 Pad function 0 */
    pad_gpio39      = 0, /**< VBUS_DTC/GPIO39 Pad function 0 */

    pad0_spis0_clk   = 1, /**< SD_CLK/SPIS_CLK/GPIO0 Pad function 1 */
    pad1_spis0_miso  = 1, /**< SD_CMD/SPIS_MISO/GPIO1 Pad function 1 */
    pad2_spis0_mosi  = 1, /**< SD_CD/SPIS_MOSI/GPIO2 Pad function 1 */
    pad3_spis0_ss    = 1, /**< SD_DATA0/SPIS_SS/GPIO3 Pad function 1 */
    pad_pwm7        = 1, /**< PWM0/SD_DATA1/PWM7/GPIO4 Pad function 1 */
    pad_pwm6        = 1, /**< PWM6/SD_DATA2/PWM1/GPIO5 Pad function 1 */
    pad_pwm5        = 1, /**< SD_DATA3/PWM5/GPIO6 Pad function 1 */
    pad_pwm4        = 1, /**< SD_WP/PWM4/GPIO7 Pad function 1 */
    pad8_pwm3       = 1, /**< PWM3/GPIO8 Pad function 1 */
    pad9_pwm2       = 1, /**< PWM2/GPIO9 Pad function 1 */
    pad11_pwm0      = 1, /**< PWM0/GPIO11 Pad function 1 */
    pad10_pwm1      = 1, /**< PWM1/GPIO10 Pad function 1 */
    pad_i2cm_scl    = 1, /**< I2CS_SCL/I2CM_SCL/GPIO12 Pad function 1 */
    pad_i2cm_sda    = 1, /**< I2CS_SDA/I2CM_SDA/GPIO13 Pad function 1 */
    pad_uart2_rxd   = 1, /**< UART2_RXD/GPIO14 Pad function 1 */
    pad_uart2_txd   = 1, /**< UART2_TXD/GPIO15 Pad function 1 */
    pad_uart2_rts   = 1, /**< UART2_RTS/GPIO16 Pad function 1 */
    pad_uart2_cts   = 1, /**< UART2_CTS/GPIO17 Pad function 1 */
    pad_uart2_dtr   = 1, /**< UART3_RXD/UART2_DTR/GPIO18 Pad function 1 */
    pad_uart2_dsr   = 1, /**< UART3_TXD/UART2_DSR/GPIO19 Pad function 1 */
    pad_uart2_dcd   = 1, /**< UART3_RTS/UART2_DCD/GPIO20 Pad function 1 */
    pad_uart2_ri    = 1, /**< UART3_CTS/UART2_RI/GPIO21 Pad function 1 */
    pad_uart0_rxd   = 1, /**< PWM3/UART0_RXD/GPIO22 Pad function 1 */
    pad_uart0_txd   = 1, /**< PWM2/UART0_TXD/GPIO23 Pad function 1 */
    pad_uart0_rts   = 1, /**< PWM1/UART0_RTS/GPIO24 Pad function 1 */
    pad_uart0_cts   = 1, /**< PWM0/UART0_CTS/GPIO25 Pad function 1 */
    pad_uart0_dtr   = 1, /**< UART1_RXD/UART0_DTR/GPIO26 Pad function 1 */
    pad_uart0_dsr   = 1, /**< UART1_TXD/UART0_DSR/GPIO27 Pad function 1 */
    pad_uart0_dcd   = 1, /**< UART1_RTS/UART0_DCD/GPIO28 Pad function 1 */
    pad_uart0_ri    = 1, /**< SPIM_SS0/UART1_CTS/UART0_RI/GPIO29 Pad function 1 */
    pad30_spim_ss0  = 1, /**< SPIM_SS0/GPIO30 Pad function 1 */
    pad_spim_ss1    = 1, /**< SPIM_SS1/GPIO31 Pad function 1 */
    pad_spim_ss2    = 1, /**< SPIM_SS2/GPIO32 Pad function 1 */
    pad_spim_ss3    = 1, /**< SPIM_SS3/GPIO33 Pad function 1 */
    pad_spim_sck    = 1, /**< SPIM_CLK/SPIS_CLK/GPIO34 Pad function 1 */
    pad_spim_miso   = 1, /**< SPIS_MISO/SPIM_MISO/GPIO35 Pad function 1 */
    pad_spim_mosi   = 1, /**< SPIS_MOSI/SPIM_MOSI/GPIO36 Pad function 1 */
    pad_spim_io2    = 1, /**< SPIS_SS/SPIM_IO2/GPIO37 Pad function 1 */
    pad_spim_io3    = 1, /**< RTC_REF/SPIM_IO3/GPIO38 Pad function 1 */
    pad_vbus_dtc    = 1, /**< VBUS_DTC/GPIO39 Pad function 1 */
    

    pad_sd_clk      = 2, /**< SD_CLK/SPIS_CLK/GPIO0 Pad function 2 */
    pad_sd_cmd      = 2, /**< SD_CMD/SPIS_MISO/GPIO1 Pad function 2 */
    pad_sd_cd       = 2, /**< SD_CD/SPIS_MOSI/GPIO2 Pad function 2 */
    pad_sd_data0    = 2, /**< SD_DATA0/SPIS_SS/GPIO3 Pad function 2 */
    pad_sd_data1    = 2, /**< PWM0/SD_DATA1/PWM7/GPIO4 Pad function 2 */
    pad_sd_data2    = 2, /**< PWM6/SD_DATA2/PWM1/GPIO5 Pad function 2 */
    pad_sd_data3    = 2, /**< SD_DATA3/PWM5/GPIO6 Pad function 2 */
    pad_sd_wp       = 2, /**< SD_WP/PWM4/GPIO7 Pad function 2 */
    
    pad_i2cs_scl    = 2, /**< I2CS_SCL/I2CM_SCL/GPIO12 Pad function 2 */
    pad_i2cs_sda    = 2, /**< I2CS_SDA/I2CM_SDA/GPIO13 Pad function 2 */
    
    pad_uart3_rxd   = 2, /**< UART3_RXD/UART2_DTR/GPIO18 Pad function 2 */
    pad_uart3_txd   = 2, /**< UART3_TXD/UART2_DSR/GPIO19 Pad function 2 */
    pad_uart3_rts   = 2, /**< UART3_RTS/UART2_DCD/GPIO20 Pad function 2 */
    pad_uart3_cts   = 2, /**< UART3_CTS/UART2_RI/GPIO21 Pad function 2 */
    pad22_pwm3      = 2, /**< PWM3/UART0_RXD/GPIO22 Pad function 1 */
    pad23_pwm2      = 2, /**< PWM2/UART0_TXD/GPIO23 Pad function 1 */
    pad24_pwm1      = 2, /**< PWM1/UART0_RTS/GPIO24 Pad function 2 */
    pad25_pwm0      = 2, /**< PWM0/UART0_CTS/GPIO25 Pad function 2 */
    pad_uart1_txd   = 2, /**< UART1_RXD/UART0_DTR/GPIO26 Pad function 2 */
    pad_uart1_rxd   = 2, /**< UART1_TXD/UART0_DSR/GPIO27 Pad function 2 */
    pad_uart1_rts   = 2, /**< UART1_RTS/UART0_DCD/GPIO28 Pad function 2 */
    pad_uart1_cts   = 2, /**< SPIM_SS0/UART1_CTS/UART0_RI/GPIO29 Pad function 2 */
    
    pad_spis0_clk   = 2, /**< SPIM_CLK/SPIS_CLK/GPIO34 Pad function 2 */
    pad_spis0_miso  = 2, /**< SPIS_MISO/SPIM_MISO/GPIO35 Pad function 2 */
    pad_spis0_mosi  = 2, /**< SPIS_MOSI/SPIM_MOSI/GPIO36 Pad function 2 */
    pad_spis0_ss    = 2, /**< SPIS_SS/SPIM_IO2/GPIO37 Pad function 2 */
    pad_rtc_ref     = 2, /**< RTC_REF/SPIM_IO3/GPIO38 Pad function 2 */

    pad29_spim_ss0   = 3, /**< SPIM_SS0/UART1_CTS/UART0_RI/GPIO29 Pad function 3 */
    pad4_pwm0        = 3, /**< PWM0/SD_DATA1/PWM7/GPIO4 Pad function 3 */
    pad5_pwm1        = 3, /**< PWM1/SD_DATA2/PWM6/GPIO5 Pad function 3 */
} pad_func_t;
#else
typedef enum
{
    pad_func_0      = 0, /**< Pad function 0 */
    pad_func_1      = 1, /**< Pad function 1 */
    pad_func_2      = 2, /**< Pad function 2 */
    pad_func_3      = 3, /**< Pad function 3 */
    
    pad_gpio0       = 0, /**< VBUS_DISCH/GPIO0 Pad function 0 */
    pad_gpio1       = 0, /**< OC_N/GPIO1 Pad function 0 */
    pad_gpio2       = 0, /**< PSW_N/GPIO2 Pad function 0 */
    pad_gpio3       = 0, /**< VBUS_DTC/GPIO3 Pad function 0 */
    pad_gpio4       = 0, /**< ENET_LED0/GPIO4 Pad function 0 */
    pad_gpio5       = 0, /**< ENET_LED1/GPIO5 Pad function 0 */
    pad_gpio6       = 0, /**< ADC1/CAM_XCLK/GPIO6 Pad function 0 */
    pad_gpio7       = 0, /**< ADC2/CAM_PCLK/GPIO7 Pad function 0 */
    pad_gpio8       = 0, /**< ADC3/CAM_VD/GPIO8 Pad function 0 */
    pad_gpio9       = 0, /**< ADC4/CAM_HD/GPIO9 Pad function 0 */
    pad_gpio10      = 0, /**< ADC5/CAM_D7/GPIO10 Pad function 0 */
    pad_gpio11      = 0, /**< ADC6/CAM_D6/GPIO11 Pad function 0 */
    pad_gpio12      = 0, /**< ADC7/CAM_D5/GPIO12 Pad function 0 */
    pad_gpio13      = 0, /**< DAC1/CAM_D4/GPIO13 Pad function 0 */
    pad_gpio14      = 0, /**< DAC0/CAM_D3/GPIO14 Pad function 0 */
    pad_gpio15      = 0, /**< CAN0_TXD/CAM_D2/GPIO15 Pad function 0 */
    pad_gpio16      = 0, /**< CAN0_RXD/CAM_D1/GPIO16 Pad function 0 */
    pad_gpio17      = 0, /**< CAN1_TXD/CAM_D0/GPIO17 Pad function 0 */
    pad_gpio18      = 0, /**< CAN1_RXD/GPIO18 Pad function 0 */
    pad_gpio19      = 0, /**< SD_CLK/GPIO19 Pad function 0 */
    pad_gpio20      = 0, /**< SD_CMD/GPIO20 Pad function 0 */
    pad_gpio21      = 0, /**< SD_DATA3/GPIO21 Pad function 0 */
    pad_gpio22      = 0, /**< SD_DATA2/GPIO22 Pad function 0 */
    pad_gpio23      = 0, /**< SD_DATA1/GPIO23 Pad function 0 */
    pad_gpio24      = 0, /**< SD_DATA0/GPIO24 Pad function 0 */
    pad_gpio25      = 0, /**< SD_CD/GPIO25 Pad function 0 */
    pad_gpio26      = 0, /**< SD_WP/GPIO26 Pad function 0 */
    pad_gpio27      = 0, /**< SPIM_CLK/GPIO27 Pad function 0 */
    pad_gpio28      = 0, /**< SPIM_SS0/GPIO28 Pad function 0 */
    pad_gpio29      = 0, /**< SPIM_MOSI/GPIO29 Pad function 0 */
    pad_gpio30      = 0, /**< SPIM_MISO/GPIO30 Pad function 0 */
    pad_gpio31      = 0, /**< SPIM_IO2/GPIO31 Pad function 0 */
    pad_gpio32      = 0, /**< SPIM_IO3/GPIO32 Pad function 0 */
    pad_gpio33      = 0, /**< SPIM_SS1/GPIO33 Pad function 0 */
    pad_gpio34      = 0, /**< SPIM_SS2/GPIO34 Pad function 0 */
    pad_gpio35      = 0, /**< SPIM_SS3/GPIO35 Pad function 0 */
    pad_gpio36      = 0, /**< SPIS0_CLK/GPIO36 Pad function 0 */
    pad_gpio37      = 0, /**< SPIS0_SS/GPIO37 Pad function 0 */
    pad_gpio38      = 0, /**< SPIS0_MOSI/GPIO38 Pad function 0 */
    pad_gpio39      = 0, /**< SPIS0_MISO/GPIO39 Pad function 0 */
    pad_gpio40      = 0, /**< SPIS1_CLK/GPIO40 Pad function 0 */
    pad_gpio41      = 0, /**< SPIS1_SS/GPIO41 Pad function 0 */
    pad_gpio42      = 0, /**< SPIS1_MOSI/GPIO42 Pad function 0 */
    pad_gpio43      = 0, /**< SPIS1_MISO/GPIO43 Pad function 0 */
    pad_gpio44      = 0, /**< I2C0_SCL/GPIO44 Pad function 0 */
    pad_gpio45      = 0, /**< I2C0_SDA/GPIO45 Pad function 0 */
    pad_gpio46      = 0, /**< I2C1_SCL/GPIO46 Pad function 0 */
    pad_gpio47      = 0, /**< I2C1_SDA/GPIO47 Pad function 0 */
    pad_gpio48      = 0, /**< UART0_TXD/GPIO48 Pad function 0 */
    pad_gpio49      = 0, /**< UART0_RXD/GPIO49 Pad function 0 */
    pad_gpio50      = 0, /**< UART0_RTS/GPIO50 Pad function 0 */
    pad_gpio51      = 0, /**< UART0_CTS/GPIO51 Pad function 0 */
    pad_gpio52      = 0, /**< UART0_DTR/UART1_TXD/PWM4/GPIO52 Pad function 0 */
    pad_gpio53      = 0, /**< UART0_DSR/UART1_RXD/PWM5/GPIO53 Pad function 0 */
    pad_gpio54      = 0, /**< UART0_DCD/UART1_RTS/PWM6/GPIO54 Pad function 0 */
    pad_gpio55      = 0, /**< UART0_RI/UART1_CTS/PWM7/GPIO55 Pad function 0 */
    pad_gpio56      = 0, /**< PWM0/GPIO56 Pad function 0 */
    pad_gpio57      = 0, /**< PWM1/GPIO57 Pad function 0 */
    pad_gpio58      = 0, /**< PWM2/GPIO58 Pad function 0 */
    pad_gpio59      = 0, /**< PWM3/GPIO59 Pad function 0 */
    pad_gpio60      = 0, /**< I2S_SDAO/GPIO60 Pad function 0 */
    pad_gpio61      = 0, /**< I2S_SDAI/GPIO61 Pad function 0 */
    pad_gpio62      = 0, /**< I2S_BCLK/GPIO62 Pad function 0 */
    pad_gpio63      = 0, /**< I2S_LRCLK/GPIO63 Pad function 0 */
    pad_gpio64      = 0, /**< I2S_MCLK/GPIO64 Pad function 0 */
    pad_gpio65      = 0, /**< I2S_CLK22/GPIO65 Pad function 0 */
    pad_gpio66      = 0, /**< I2S_CLK24/GPIO66 Pad function 0 */
    
    pad_oc_n        = 1, /**< OC_N/GPIO1 Pad function 1 */
    pad_vbus_dtc    = 1, /**< VBUS_DTC/GPIO3 Pad function 1 */
    
    pad_enet_led0   = 1, /**< ENET_LED0/GPIO4 Pad function 1 */
    pad_enet_led1   = 1, /**< ENET_LED1/GPIO5 Pad function 1 */
        
    pad_cam_xclk    = 1, /**< ADC1/CAM_XCLK/GPIO6 Pad function 1 */
    pad_cam_pclk    = 1, /**< ADC2/CAM_PCLK/GPIO7 Pad function 1 */
    pad_cam_vd      = 1, /**< ADC3/CAM_VD/GPIO8 Pad function 1 */
    pad_cam_hd      = 1, /**< ADC4/CAM_HD/GPIO9 Pad function 1 */
    pad_cam_d7      = 1, /**< ADC5/CAM_D7/GPIO10 Pad function 1 */
    pad_cam_d6      = 1, /**< ADC6/CAM_D6/GPIO11 Pad function 1 */
    pad_cam_d5      = 1, /**< ADC7/CAM_D5/GPIO12 Pad function 1 */
    pad_cam_d4      = 1, /**< DAC1/CAM_D4/GPIO13 Pad function 1 */
    pad_cam_d3      = 1, /**< DAC0/CAM_D3/GPIO14 Pad function 1 */
    pad_cam_d2      = 1, /**< CAN0_TXD/CAM_D2/GPIO15 Pad function 1 */
    pad_cam_d1      = 1, /**< CAN0_RXD/CAM_D1/GPIO16 Pad function 1 */
    pad_cam_d0      = 1, /**< CAN1_TXD/CAM_D0/GPIO17 Pad function 1 */
    
    pad_sd_clk      = 1, /**< SD_CLK/GPIO19 Pad function 1 */
    pad_sd_cmd      = 1, /**< SD_CMD/GPIO20 Pad function 1 */
    pad_sd_data3    = 1, /**< SD_DATA3/GPIO21 Pad function 1 */
    pad_sd_data2    = 1, /**< SD_DATA2/GPIO22 Pad function 1 */
    pad_sd_data1    = 1, /**< SD_DATA1/GPIO23 Pad function 1 */
    pad_sd_data0    = 1, /**< SD_DATA0/GPIO24 Pad function 1 */
    pad_sd_cd       = 1, /**< SD_CD/GPIO25 Pad function 1 */
    pad_sd_wp       = 1, /**< SD_WP/GPIO26 Pad function 1 */
    
    pad_spim_sck    = 1, /**< SPIM_CLK/GPIO27 Pad function 1 */
    pad_spim_ss0    = 1, /**< SPIM_SS0/GPIO28 Pad function 1 */
    pad_spim_mosi   = 1, /**< SPIM_MOSI/GPIO29 Pad function 1 */
    pad_spim_miso   = 1, /**< SPIM_MISO/GPIO30 Pad function 1 */
    pad_spim_io2    = 1, /**< SPIM_IO2/GPIO31 Pad function 1 */
    pad_spim_io3    = 1, /**< SPIM_IO3/GPIO32 Pad function 1 */
    pad_spim_ss1    = 1, /**< SPIM_SS1/GPIO33 Pad function 1 */
    pad_spim_ss2    = 1, /**< SPIM_SS2/GPIO34 Pad function 1 */
    pad_spim_ss3    = 1, /**< SPIM_SS3/GPIO35 Pad function 1 */
    
    pad_spis0_clk   = 1, /**< SPIS0_CLK/GPIO36 Pad function 1 */
    pad_spis0_ss    = 1, /**< SPIS0_SS/GPIO37 Pad function 1 */
    pad_spis0_mosi  = 1, /**< SPIS0_MOSI/GPIO38 Pad function 1 */
    pad_spis0_miso  = 1, /**< SPIS0_MISO/GPIO39 Pad function 1 */
    pad_spis1_clk   = 1, /**< SPIS1_CLK/GPIO40 Pad function 1 */
    pad_spis1_ss    = 1, /**< SPIS1_SS/GPIO41 Pad function 1 */
    pad_spis1_mosi  = 1, /**< SPIS1_MOSI/GPIO42 Pad function 1 */
    pad_spis1_miso  = 1, /**< SPIS1_MISO/GPIO43 Pad function 1 */
    
    pad_i2c0_scl    = 1, /**< I2C0_SCL/GPIO44 Pad function 1 */
    pad_i2c0_sda    = 1, /**< I2C0_SDA/GPIO45 Pad function 1 */
    pad_i2c1_scl    = 1, /**< I2C1_SCL/GPIO46 Pad function 1 */
    pad_i2c1_sda    = 1, /**< I2C1_SDA/GPIO47 Pad function 1 */
    
    pad_pwm4        = 1, /**< UART0_DTR/UART1_TXD/PWM4/GPIO52 Pad function 1 */
    pad_pwm5        = 1, /**< UART0_DSR/UART1_RXD/PWM5/GPIO53 Pad function 1 */
    pad_pwm6        = 1, /**< UART0_DCD/UART1_RTS/PWM6/GPIO54 Pad function 1 */
    pad_pwm7        = 1, /**< UART0_RI/UART1_CTS/PWM7/GPIO55 Pad function 1 */
    pad_pwm0        = 1, /**< PWM0/GPIO56 Pad function 1 */
    pad_pwm1        = 1, /**< PWM1/GPIO57 Pad function 1 */
    pad_pwm2        = 1, /**< PWM2/GPIO58 Pad function 1 */
    pad_pwm3        = 1, /**< PWM3/GPIO59 Pad function 1 */
    
    pad_i2s_sdao    = 1, /**< I2S_SDAO/GPIO60 Pad function 1 */
    pad_i2s_sdai    = 1, /**< I2S_SDAI/GPIO61 Pad function 1 */
    pad_i2s_bclk    = 1, /**< I2S_BCLK/GPIO62 Pad function 1 */
    pad_i2s_lrclk   = 1, /**< I2S_LRCLK/GPIO63 Pad function 1 */
    pad_i2s_mclk    = 1, /**< I2S_MCLK/GPIO64 Pad function 1 */
    pad_i2s_clk22   = 1, /**< I2S_CLK22/GPIO65 Pad function 1 */
    pad_i2s_clk24   = 1, /**< I2S_CLK24/GPIO66 Pad function 1 */
    
    pad_can0_txd    = 2, /**< CAN0_TXD/CAM_D2/GPIO15 Pad function 2 */
    pad_can0_rxd    = 2, /**< CAN0_RXD/CAM_D1/GPIO16 Pad function 2 */
    pad_can1_txd    = 2, /**< CAN1_TXD/CAM_D0/GPIO17 Pad function 2 */
    pad_can1_rxd    = 2, /**< CAN1_RXD/GPIO18 Pad function 2 */
    
    pad_uart1_txd   = 2, /**< UART0_DTR/UART1_TXD/PWM4/GPIO52 Pad function 2 */
    pad_uart1_rxd   = 2, /**< UART0_DSR/UART1_RXD/PWM5/GPIO53 Pad function 2 */
    pad_uart1_rts   = 2, /**< UART0_DCD/UART1_RTS/PWM6/GPIO54 Pad function 2 */
    pad_uart1_cts   = 2, /**< UART0_RI/UART1_CTS/PWM7/GPIO55 Pad function 2 */
    
    pad_i2ss_bclk   = 2, /**< I2S_BCLK/GPIO62 Pad function 2 */
    pad_i2ss_lrclk  = 2, /**< I2S_LRCLK/GPIO63 Pad function 2 */
    
    pad_adc1        = 3, /**< ADC1/CAM_XCLK/GPIO6 Pad function 3 */
    pad_adc2        = 3, /**< ADC2/CAM_PCLK/GPIO7 Pad function 3 */
    pad_adc3        = 3, /**< ADC3/CAM_VD/GPIO8 Pad function 3 */
    pad_adc4        = 3, /**< ADC4/CAM_HD/GPIO9 Pad function 3 */
    pad_adc5        = 3, /**< ADC5/CAM_D7/GPIO10 Pad function 3 */
    pad_adc6        = 3, /**< ADC6/CAM_D6/GPIO11 Pad function 3 */
    pad_adc7        = 3, /**< ADC7/CAM_D5/GPIO12 Pad function 3 */
    
    pad_dac1        = 3, /**< DAC1/CAM_D4/GPIO13 Pad function 3 */
    pad_dac0        = 3, /**< DAC0/CAM_D3/GPIO14 Pad function 3 */
        
    pad_uart0_txd   = 3, /**< UART0_TXD/GPIO48 Pad function 3 */
    pad_uart0_rxd   = 3, /**< UART0_RXD/GPIO49 Pad function 3 */
    pad_uart0_rts   = 3, /**< UART0_RTS/GPIO50 Pad function 3 */
    pad_uart0_cts   = 3, /**< UART0_CTS/GPIO51 Pad function 3 */
    pad_uart0_dtr   = 3, /**< UART0_DTR/UART1_TXD/PWM4/GPIO52 Pad function 3 */
    pad_uart0_dsr   = 3, /**< UART0_DSR/UART1_RXD/PWM5/GPIO53 Pad function 3 */
    pad_uart0_dcd   = 3, /**< UART0_DCD/UART1_RTS/PWM6/GPIO54 Pad function 3 */
    pad_uart0_ri    = 3, /**< UART0_RI/UART1_CTS/PWM7/GPIO55 Pad function 3 */
} pad_func_t;

#endif

/** @brief Pad direction control. */
typedef enum
{
    pad_dir_input,      /**< Input */
    pad_dir_output,     /**< Output */
    pad_dir_open_drain  /**< Open Drain */
} pad_dir_t;

/** @brief GPIO Interrupt control */
typedef enum
{
    gpio_int_edge_falling,  /**< Interrupt on a falling edge */
    gpio_int_edge_raising   /**< Interrupt on a raising edge */
} gpio_int_edge_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Configure the direction of a pin
 *  @param num The GPIO number
 *  @param dir The direction
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_dir(uint8_t num, pad_dir_t dir);

/** @brief Configure the slew rate for a pin
 *  @param num The GPIO number
 *  @param slew The slew rate of the pin
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_slew(uint8_t num, pad_slew_t slew);

/** @brief Configure the schmitt trigger for a pin
 *  @param num The GPIO number
 *  @param schmitt The Schmitt trigger configuration
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_schmitt(uint8_t num, pad_schmitt_t schmitt);

/** @brief Configure the pull up/down for a pin
 *  @param num The GPIO number
 *  @param pull The pullup/down configuration
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_pull(uint8_t num, pad_pull_t pull);

/** @brief Configure the maximum current drive for a pin
 *  @param num The GPIO number
 *  @param drive The maximum current
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_idrive(uint8_t num, pad_drive_t drive);

/** @brief Configure the alternative function for a pin
 *  @param num The GPIO number
 *  @param func The function that the pin should use
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_function(uint8_t num, pad_func_t func);

/** @brief Write a value to a GPIO pin
 *  @param num The GPIO number
 *  @param val The value to write
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_write(uint8_t num, uint8_t val);

/** @brief Read a value from a GPIO pin
 *  @param num The GPIO number
 *  @returns The value of the pin (1 = high, 0 = low), otherwise -1
 */
int8_t gpio_read(uint8_t num);

/** @brief Toggle the value of a GPIO pin
 *  @param num The GPIO number
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_toggle(uint8_t num);

/** @brief Enable an interrupt on a GPIO pin
 *  @param num The GPIO number
 *  @param edge The edge at which to trigger on
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_interrupt_enable(uint8_t num, gpio_int_edge_t edge);

/** @brief Disable an interrupt on a GPIO pin
 *  @param num The GPIO number
 *  @returns On success a 0, otherwise -1
 */
int8_t gpio_interrupt_disable(uint8_t num);

/** @brief Check if an interrupt has happened on a GPIO pin
 *  @param num The GPIO number
 *  @returns On no interrupt 0, on an interrupt 1, otherwise -1
 */
int8_t gpio_is_interrupted(uint8_t num);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_GPIO_H_ */
