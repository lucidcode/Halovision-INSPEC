/**
  ******************************************************************************
  * @file    system_stm32f4xx.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
  *
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f4xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include STM32_HAL_H
#include "omv_boardconfig.h"

/** This variable is updated in two ways:
  * 1) by calling HAL API function HAL_RCC_GetHCLKFreq()
  * 2) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  */
uint32_t SystemCoreClock = 16000000;
extern void __fatal_error(const char *msg);

#if defined(MCU_SERIES_H7)

#define SRAM_BASE           D1_AXISRAM_BASE
#define FLASH_BASE          FLASH_BANK1_BASE

#define CONFIG_RCC_CR_1ST   (RCC_CR_HSION)
#define CONFIG_RCC_CR_2ND   (0xEAF6ED7F)
#define CONFIG_RCC_PLLCFGR  (0x00000000)

uint32_t SystemD2Clock = 64000000;
const uint8_t D1CorePrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

#elif defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)

#define CONFIG_RCC_CR_1ST   (RCC_CR_HSION)
#define CONFIG_RCC_CR_2ND   (0xFEF6FFFF)
#define CONFIG_RCC_PLLCFGR  (0x24003010)

const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

#endif

/**
  * @brief  Setup the microcontroller system
  *         Initialize the FPU setting, vector table location and External memory
  *         configuration.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
    /* FPU settings ------------------------------------------------------------*/
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
      SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
    #endif
    /* Reset the RCC clock configuration to the default reset state ------------*/
    
    /* Set HSION bit */
    RCC->CR |= CONFIG_RCC_CR_1ST;
    
    /* Reset CFGR register */
    RCC->CFGR = 0x00000000;
    
    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t) CONFIG_RCC_CR_2ND;
    
    /* Reset PLLCFGR register */
    RCC->PLLCFGR = CONFIG_RCC_PLLCFGR;
    
    #if defined(MCU_SERIES_H7)
    /* Reset D1CFGR register */
    RCC->D1CFGR = 0x00000000;
    
    /* Reset D2CFGR register */
    RCC->D2CFGR = 0x00000000;
    
    /* Reset D3CFGR register */
    RCC->D3CFGR = 0x00000000;
    
    /* Reset PLLCKSELR register */
    RCC->PLLCKSELR = 0x00000000;
    
    /* Reset PLL1DIVR register */
    RCC->PLL1DIVR = 0x00000000;
    
    /* Reset PLL1FRACR register */
    RCC->PLL1FRACR = 0x00000000;
    
    /* Reset PLL2DIVR register */
    RCC->PLL2DIVR = 0x00000000;
    
    /* Reset PLL2FRACR register */
    RCC->PLL2FRACR = 0x00000000;
    
    /* Reset PLL3DIVR register */
    RCC->PLL3DIVR = 0x00000000;
    
    /* Reset PLL3FRACR register */
    RCC->PLL3FRACR = 0x00000000;
    #endif // defined(MCU_SERIES_H7)
    
    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    
    /* Disable all interrupts */
    #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    RCC->CIR = 0x00000000;
    #elif defined(MCU_SERIES_L4) || defined(MCU_SERIES_H7)
    RCC->CIER = 0x00000000;
    #endif
    
    #if defined(MCU_SERIES_H7)
    /* Change  the switch matrix read issuing capability to 1 for the AXI SRAM target (Target 7) */
    *((__IO uint32_t*)0x51008108) = 0x00000001;
    #endif // defined(MCU_SERIES_H7)
    
    /* Configure the Vector Table location add offset address ------------------*/
    #ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
    #else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
    #endif
    
    /* dpgeorge: enable 8-byte stack alignment for IRQ handlers, in accord with EABI */
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
}

void SystemClock_Config(void)
{
    uint32_t flash_latency;
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    #if defined(MCU_SERIES_H7)
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    #endif

    #if defined(MCU_SERIES_H7)
    /* Supply configuration update enable */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    #else
    /* Enable Power Control clock */
    __PWR_CLK_ENABLE();
    #endif

    /* The voltage scaling allows optimizing the power consumption when the device is
       clocked below the maximum system frequency, to update the voltage scaling value
       regarding system frequency refer to product datasheet.  */
    #if defined(MCU_SERIES_H7)
    // Enable VSCALE0 for revision V devices.
    if (HAL_GetREVID() >= 0x2003) {
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
    } else {
    #else
    if (1) {
    #endif
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    }

    // Wait for PWR_FLAG_VOSRDY
    #if defined(MCU_SERIES_H7)
    while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY) == RESET) {
    }
    #endif

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    #if defined(MCU_SERIES_H7)
    RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
    #endif
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    #if   defined (STM32F407xx) // 168MHz/48MHz
    flash_latency = FLASH_LATENCY_5;
    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    #elif defined (STM32F427xx)// 192MHz/48MHz
    flash_latency = FLASH_LATENCY_7;
    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 384;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    #elif defined (STM32F765xx)// 216MHz/48MHz
    flash_latency = FLASH_LATENCY_7;
    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLQ = 9;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    #elif defined (STM32H743xx)// 480MHz/48MHz
    //PLL1 48MHz for USB, SDMMC and FDCAN
    flash_latency = FLASH_LATENCY_2;
    RCC_OscInitStruct.PLL.PLLM = 3;
    RCC_OscInitStruct.PLL.PLLN = 240;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 20;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    #endif
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        // Initialization Error
        __fatal_error("HAL_RCC_OscConfig");
    }

    #if defined(STM32H743xx)
    // PLL2 200MHz for FMC and QSPI.
    PeriphClkInitStruct.PLL2.PLL2M = 3;
    PeriphClkInitStruct.PLL2.PLL2N = 100;
    PeriphClkInitStruct.PLL2.PLL2P = 2;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 2;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;

    // PLL3 160MHz for ADC and SPI123
    PeriphClkInitStruct.PLL3.PLL3M = 3;
    PeriphClkInitStruct.PLL3.PLL3N = 80;
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 2;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_FDCAN
                                              |RCC_PERIPHCLK_SPI3|RCC_PERIPHCLK_SPI2
                                              |RCC_PERIPHCLK_SDMMC|RCC_PERIPHCLK_I2C2
                                              |RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB
                                              |RCC_PERIPHCLK_QSPI|RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
    PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
    PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL3;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        // Initialization Error
        __fatal_error("HAL_RCCEx_PeriphCLKConfig");
    }
    #endif

    #if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7)
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        // Initialization Error
        __fatal_error("HAL_PWREx_EnableOverDrive");
    }
    #endif

    /* Select PLL as system clock source and configure the HCLK, PCLK clocks dividers */
    #if defined(MCU_SERIES_H7)
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 |
                                   RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
    #else
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    #endif

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flash_latency) != HAL_OK) {
        // Initialization Error
        __fatal_error("HAL_RCC_ClockConfig");
    }

    #if defined(MCU_SERIES_H7)
    // Activate CSI clock mondatory for I/O Compensation Cell
    __HAL_RCC_CSI_ENABLE() ;

    // Enable SYSCFG clock mondatory for I/O Compensation Cell
    __HAL_RCC_SYSCFG_CLK_ENABLE() ;

    // Enables the I/O Compensation Cell
    HAL_EnableCompensationCell();

    // Enable the USB voltage level detector
    HAL_PWREx_EnableUSBVoltageDetector();
    #endif
}
