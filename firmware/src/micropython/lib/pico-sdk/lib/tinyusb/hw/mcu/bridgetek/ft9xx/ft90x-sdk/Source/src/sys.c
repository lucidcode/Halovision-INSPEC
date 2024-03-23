/**
    @file

    @brief
    Functions for the System section

    
**/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Future Technology Devices
 * International Limited ("Bridgetek") subject to the licence terms set out
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
#include <ft900_sys.h>
#include <ft900_timers.h>
#include <ft900_delay.h>
#include <registers/ft900_registers.h>
#include <ft900_interrupt.h>

/* CONSTANTS ***********************************************************************/
#define CLK_100MHZ (100*1000*1000)

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/
/** @brief Enable a device on the FT900
 *  @param dev The device to enable
 *  @returns On success a 0, otherwise -1
 */
int sys_enable(sys_device_t dev)
{
	int iRet = 0;

	switch(dev)
	{
#if defined(__FT900__)
    case sys_device_usb_host:
    case sys_device_can0:
    case sys_device_can1:
    case sys_device_spi_slave1:
    case sys_device_i2s:
    case sys_device_camera:
#elif defined(__FT930__)
    case sys_device_uart2:
    case sys_device_uart3:
#endif
    case sys_device_usb_device:
    case sys_device_sd_card:
    case sys_device_i2c_master:
    case sys_device_i2c_slave:
    case sys_device_spi_master:
    case sys_device_spi_slave0:
    case sys_device_uart0:
    case sys_device_uart1:
    case sys_device_pwm:
        SYS->CLKCFG |= MASK_SYS_CLKCFG_ENA(dev); break;
#if defined(__FT900__)
		case sys_device_ethernet:
        {
            ETHPHY->MISC &= ~(MASK_ETHPHY_MISC_PWRDN | MASK_ETHPHY_MISC_PWRSV);
            SYS->CLKCFG |= MASK_SYS_CLKCFG_ENA(dev); 
            SYS->MSC0CFG |= MASK_SYS_MSC0CFG_MAC_RESET_PHY;
            delayms(10);
            SYS->MSC0CFG &= ~MASK_SYS_MSC0CFG_MAC_RESET_PHY;
        }
        break;
#endif
		case sys_device_timer_wdt:
        {
            TIMER->TIMER_CONTROL_0 |= MASK_TIMER_CONTROL_0_SOFT_RESET;
            TIMER->TIMER_CONTROL_0 |= MASK_TIMER_CONTROL_0_BLOCK_EN;
        }
        break;
		case sys_device_adc:  ADCDAC->ADC_CONF |= MASK_ADC_CONF_ADC_PDB; break;
		case sys_device_dac0: ADCDAC->DAC_CONF |= MASK_DAC_CONF_DAC_PDB0; break;
		case sys_device_dac1: ADCDAC->DAC_CONF |= MASK_DAC_CONF_DAC_PDB1; break;

        default: iRet = -1; break;
	}

	return iRet;
}


/** @brief Disable a device on the FT900
 *  @param dev The device to Disable
 *  @returns On success a 0, otherwise -1
 */
int sys_disable(sys_device_t dev)
{
	int iRet = 0;

	switch(dev)
	{
#if defined(__FT900__)
    case sys_device_usb_host:
    case sys_device_can0:
    case sys_device_can1:
    case sys_device_spi_slave1:
    case sys_device_i2s:
    case sys_device_camera:
#elif defined(__FT930__)
    case sys_device_uart2:
    case sys_device_uart3:
#endif
    case sys_device_usb_device:
    case sys_device_sd_card:
    case sys_device_i2c_master:
    case sys_device_i2c_slave:
    case sys_device_spi_master:
    case sys_device_spi_slave0:
    case sys_device_uart0:
    case sys_device_uart1:
    case sys_device_pwm:
        SYS->CLKCFG &= ~MASK_SYS_CLKCFG_ENA(dev); break;
#if defined(__FT900__)
		case sys_device_ethernet: 
        {
            ETHPHY->MISC |= (MASK_ETHPHY_MISC_PWRDN | MASK_ETHPHY_MISC_PWRSV);
            SYS->CLKCFG &= ~MASK_SYS_CLKCFG_ENA(dev); break;
        }
        break;
#endif
		case sys_device_timer_wdt: TIMER->TIMER_CONTROL_0 &= ~MASK_TIMER_CONTROL_0_BLOCK_EN; break;
        case sys_device_adc: ADCDAC->ADC_CONF &= ~MASK_ADC_CONF_ADC_PDB; break;
        case sys_device_dac0: ADCDAC->DAC_CONF &= ~MASK_DAC_CONF_DAC_PDB0; break;
        case sys_device_dac1: ADCDAC->DAC_CONF &= ~MASK_DAC_CONF_DAC_PDB1; break;
		default: iRet = -1; break;
	}

	return iRet;
}

/** @brief Enable a divider on the CPU
 *  @param div The divider to use
 *  @returns On success a 0, otherwise -1
 */
int sys_cpu_clock_div(sys_cpu_divider_t div)
{
    int iRet = 0;

    if (div > sys_cpu_divider_512)
    {
    	iRet = -1;
    }
    else
    {
#if defined(__FT900__)
		if (!sys_check_ft900_revB())//if 90x series is rev C
		{
            SYS->CLKCFG &= ~MASK_SYS_CLKCFG_CPU_CLK_DIV;
            SYS->CLKCFG |= (div << BIT_SYS_CLKCFG_CPU_CLK_DIV);
			asm volatile ("nop"::: "memory");
			asm volatile ("nop"::: "memory");
			asm volatile ("nop"::: "memory");
		}
		//else FT900 Rev B
		/* Unsupported
		   This causes the CPU to halt */
#else
		SYS->CLKCFG &= ~MASK_SYS_CLKCFG_CPU_CLK_DIV;
		SYS->CLKCFG |= (div << BIT_SYS_CLKCFG_CPU_CLK_DIV);
		asm volatile ("nop"::: "memory");
		asm volatile ("nop"::: "memory");
		asm volatile ("nop"::: "memory");
#endif
    }
    return iRet;
}

/** @brief Get the current clock of the CPU
 *  @returns The clock rate of the CPU in Hertz
 */
uint32_t sys_get_cpu_clock(void)
{
    uint8_t divider = (SYS->CLKCFG & MASK_SYS_CLKCFG_CPU_CLK_DIV) >> BIT_SYS_CLKCFG_CPU_CLK_DIV;
    uint32_t clk = 0;
    
    switch (divider)
    {
        case 0: clk = CLK_100MHZ/1; break;
        case 1: clk = CLK_100MHZ/2; break;
        case 2: clk = CLK_100MHZ/4; break;
        case 3: clk = CLK_100MHZ/8; break;
        case 4: clk = CLK_100MHZ/64; break;
        case 5: clk = CLK_100MHZ/128; break;
        case 6: clk = CLK_100MHZ/512; break;
        default: break;
    }
    
    return clk;
}

#if defined(__FT900__)
/** @brief Swap the I2C Master and slave pins
 *  @param swop Enable or disable the swop feature (Only for FT90x)
 *  @return  On success a 0, otherwise -1
 */
int sys_i2c_swop(uint8_t swop)
{
    if (swop)
    {
        SYS->MSC0CFG |= MASK_SYS_MSC0CFG_I2C_SWOP;
    }
    else
    {
        SYS->MSC0CFG &= (~MASK_SYS_MSC0CFG_I2C_SWOP);
    }
    return 0;
}

/** @brief Configure the External PWM trigger
 *  @param exttrigger The selection of external trigger
 *  @return  On success a 0, otherwise -1
 */
int sys_pwm_ext_trigger(sys_pwm_trigger_t exttrigger)
{
    SYS->MSC0CFG = (SYS->MSC0CFG & ~MASK_SYS_MSC0CFG_PWM_TRIG_SEL)
            | ((uint8_t)(exttrigger) << BIT_SYS_MSC0CFG_PWM_TRIG_SEL);
    return 0;
}
#endif

/** @brief Reset all peripherals */
void sys_reset_all(void)
{
	interrupt_disable_globally();
    SYS->MSC0CFG |= MASK_SYS_MSC0CFG_PERI_SOFTRESET;
    while(SYS->MSC0CFG & MASK_SYS_MSC0CFG_PERI_SOFTRESET)
        ; /* Wait for the bit to clear */

    /** The following code is to clear all the timer pending interrupt status.
      * This is needed in case firmware is directly loaded to program memory and no hw reset */
    TIMER->TIMER_CONTROL_0 = MASK_TIMER_CONTROL_0_SOFT_RESET;
    TIMER->TIMER_INT = (MASK_TIMER_INT_TIMER_INT(timer_select_a) | MASK_TIMER_INT_TIMER_INT(timer_select_b) |
              	  	    MASK_TIMER_INT_TIMER_INT(timer_select_c) | MASK_TIMER_INT_TIMER_INT(timer_select_d));
}

