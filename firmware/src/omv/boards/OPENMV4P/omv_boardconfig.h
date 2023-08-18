/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Board configuration and pin definitions.
 */
#ifndef __OMV_BOARDCONFIG_H__
#define __OMV_BOARDCONFIG_H__

// Architecture info
#define OMV_ARCH_STR                          "OMV4P H7 32768 SDRAM" // 33 chars max
#define OMV_BOARD_TYPE                        "H7"
#define OMV_UNIQUE_ID_ADDR                    0x1FF1E800 // Unique ID address.
#define OMV_UNIQUE_ID_SIZE                    3 // Unique ID size in words.
#define OMV_UNIQUE_ID_OFFSET                  4 // Bytes offset for multi-word UIDs.

// Needed by the SWD JTAG testrig - located at the bottom of the frame buffer overlay stack.
#define OMV_SELF_TEST_SWD_ADDR                *((uint32_t *) 0x24000018)

#define OMV_XCLK_MCO                          (0U)
#define OMV_XCLK_TIM                          (1U)

// Sensor external clock source.
#define OMV_XCLK_SOURCE                       (OMV_XCLK_TIM)

// Sensor external clock timer frequency.
#define OMV_XCLK_FREQUENCY                    (12000000)

// Sensor PLL register value.
#define OMV_OV7725_PLL_CONFIG                 (0x41) // x4

// Sensor Banding Filter Value
#define OMV_OV7725_BANDING                    (0x7F)

// OV5640 Sensor Settings
#define OMV_OV5640_XCLK_FREQ                  (24000000)
#define OMV_OV5640_PLL_CTRL2                  (0x64)
#define OMV_OV5640_PLL_CTRL3                  (0x13)
#define OMV_OV5640_REV_Y_CHECK                (1)
#define OMV_OV5640_REV_Y_FREQ                 (25000000)
#define OMV_OV5640_REV_Y_CTRL2                (0x54)
#define OMV_OV5640_REV_Y_CTRL3                (0x13)

// Enable hardware JPEG
#define OMV_HARDWARE_JPEG                     (1)

// Enable MDMA sensor offload.
#define OMV_ENABLE_SENSOR_MDMA                (1)

// Enable sensor drivers
#define OMV_ENABLE_OV2640                     (1)
#define OMV_ENABLE_OV5640                     (1)
#define OMV_ENABLE_OV7690                     (0)
#define OMV_ENABLE_OV7725                     (1)
#define OMV_ENABLE_OV9650                     (1)
#define OMV_ENABLE_MT9M114                    (1)
#define OMV_ENABLE_MT9V0XX                    (1)
#define OMV_ENABLE_LEPTON                     (1)
#define OMV_ENABLE_HM01B0                     (0)
#define OMV_ENABLE_PAJ6100                    (1)
#define OMV_ENABLE_FROGEYE2020                (1)
#define OMV_ENABLE_FIR_MLX90621               (1)
#define OMV_ENABLE_FIR_MLX90640               (1)
#define OMV_ENABLE_FIR_MLX90641               (1)
#define OMV_ENABLE_FIR_AMG8833                (1)
#define OMV_ENABLE_FIR_LEPTON                 (1)

// Enable additional GPIO banks.
#define OMV_ENABLE_GPIO_BANK_F                (1)
#define OMV_ENABLE_GPIO_BANK_G                (1)
#define OMV_ENABLE_GPIO_BANK_H                (1)
#define OMV_ENABLE_GPIO_BANK_I                (1)

// Enable sensor features
#define OMV_ENABLE_OV5640_AF                  (0)

// Enable WiFi debug
#define OMV_ENABLE_WIFIDBG                    (1)

// Enable self-tests on first boot
#define OMV_ENABLE_SELFTEST                   (1)

// If buffer size is bigger than this threshold, the quality is reduced.
// This is only used for JPEG images sent to the IDE not normal compression.
#define JPEG_QUALITY_THRESH                   (1920 * 1080 * 2)

// Low and high JPEG QS.
#define JPEG_QUALITY_LOW                      50
#define JPEG_QUALITY_HIGH                     90

// FB Heap Block Size
#define OMV_UMM_BLOCK_SIZE                    256

// Core VBAT for selftests
#define OMV_CORE_VBAT                         "3.3"

// USB IRQn.
#define OMV_USB_IRQN                          (OTG_FS_IRQn)

//PLL1 48MHz for USB, SDMMC and FDCAN
#define OMV_OSC_PLL1M                         (3)
#define OMV_OSC_PLL1N                         (240)
#define OMV_OSC_PLL1P                         (2)
#define OMV_OSC_PLL1Q                         (20)
#define OMV_OSC_PLL1R                         (2)
#define OMV_OSC_PLL1VCI                       (RCC_PLL1VCIRANGE_2)
#define OMV_OSC_PLL1VCO                       (RCC_PLL1VCOWIDE)
#define OMV_OSC_PLL1FRAC                      (0)

// PLL2 200MHz for FMC and QSPI.
#define OMV_OSC_PLL2M                         (3)
#define OMV_OSC_PLL2N                         (100)
#define OMV_OSC_PLL2P                         (2)
#define OMV_OSC_PLL2Q                         (2)
#define OMV_OSC_PLL2R                         (2)
#define OMV_OSC_PLL2VCI                       (RCC_PLL2VCIRANGE_2)
#define OMV_OSC_PLL2VCO                       (RCC_PLL2VCOWIDE)
#define OMV_OSC_PLL2FRAC                      (0)

// PLL3 160MHz for ADC and SPI123
#define OMV_OSC_PLL3M                         (3)
#define OMV_OSC_PLL3N                         (80)
#define OMV_OSC_PLL3P                         (2)
#define OMV_OSC_PLL3Q                         (2)
#define OMV_OSC_PLL3R                         (2)
#define OMV_OSC_PLL3VCI                       (RCC_PLL3VCIRANGE_2)
#define OMV_OSC_PLL3VCO                       (RCC_PLL3VCOWIDE)
#define OMV_OSC_PLL3FRAC                      (0)

// Clock Sources
#define OMV_OSC_PLL_CLKSOURCE                 RCC_PLLSOURCE_HSE
#define OMV_OSC_USB_CLKSOURCE                 RCC_USBCLKSOURCE_HSI48
#define OMV_OSC_RNG_CLKSOURCE                 RCC_RNGCLKSOURCE_HSI48
#define OMV_OSC_ADC_CLKSOURCE                 RCC_ADCCLKSOURCE_PLL3
#define OMV_OSC_SPI123_CLKSOURCE              RCC_SPI123CLKSOURCE_PLL3

// HSE/HSI/CSI State
#define OMV_OSC_HSE_STATE                     (RCC_HSE_ON)
#define OMV_OSC_HSI48_STATE                   (RCC_HSI48_ON)

// Flash Latency
#define OMV_FLASH_LATENCY                     (FLASH_LATENCY_2)

// Power supply configuration
#define OMV_PWR_SUPPLY                        (PWR_LDO_SUPPLY)

// Linker script constants (see the linker script template stm32fxxx.ld.S).
// Note: fb_alloc is a stack-based, dynamically allocated memory on FB.
// The maximum available fb_alloc memory = FB_ALLOC_SIZE + FB_SIZE - (w*h*bpp).
#define OMV_MAIN_MEMORY                       SRAM1 // data, bss and heap
#define OMV_STACK_MEMORY                      ITCM // stack memory
#define OMV_DMA_MEMORY                        SRAM3 // Misc DMA buffers memory.
#define OMV_FB_MEMORY                         DRAM // Framebuffer, fb_alloc
#define OMV_JPEG_MEMORY                       DRAM // JPEG buffer memory buffer.
#define OMV_JPEG_MEMORY_OFFSET                (31M) // JPEG buffer is placed after FB/fballoc memory.
#define OMV_VOSPI_MEMORY                      SRAM4 // VoSPI buffer memory.
#define OMV_FB_OVERLAY_MEMORY                 AXI_SRAM // Fast fb_alloc memory.

#define OMV_FB_SIZE                           (20M) // FB memory: header + VGA/GS image
#define OMV_FB_ALLOC_SIZE                     (11M) // minimum fb alloc size
#define OMV_FB_OVERLAY_SIZE                   (496 * 1024) // Fast fb_alloc memory size.
#define OMV_STACK_SIZE                        (64K)
#define OMV_HEAP_SIZE                         (240K)
#define OMV_SDRAM_SIZE                        (32 * 1024 * 1024) // This needs to be here for UVC firmware.

#define OMV_LINE_BUF_SIZE                     (11 * 1024) // Image line buffer round(2592 * 2BPP * 2 buffers).
#define OMV_MSC_BUF_SIZE                      (2K) // USB MSC bot data
#define OMV_VFS_BUF_SIZE                      (1K) // VFS sturct + FATFS file buffer (624 bytes)
#define OMV_FIR_LEPTON_BUF_SIZE               (1K) // FIR Lepton Packet Double Buffer (328 bytes)
#define OMV_JPEG_BUF_SIZE                     (1024 * 1024) // IDE JPEG buffer (header + data).

// Memory map.
#define OMV_FLASH_ORIGIN                      0x08000000
#define OMV_FLASH_LENGTH                      2048K
#define OMV_DTCM_ORIGIN                       0x20000000 // Note accessible by CPU and MDMA only.
#define OMV_DTCM_LENGTH                       128K
#define OMV_ITCM_ORIGIN                       0x00000000
#define OMV_ITCM_LENGTH                       64K
#define OMV_SRAM1_ORIGIN                      0x30000000
#define OMV_SRAM1_LENGTH                      256K // SRAM1 + SRAM2 + 1/2 SRAM3
#define OMV_SRAM3_ORIGIN                      0x30040000
#define OMV_SRAM3_LENGTH                      32K
#define OMV_SRAM4_ORIGIN                      0x38000000
#define OMV_SRAM4_LENGTH                      64K
#define OMV_AXI_SRAM_ORIGIN                   0x24000000
#define OMV_AXI_SRAM_LENGTH                   512K
#define OMV_DRAM_ORIGIN                       0xC0000000
#define OMV_DRAM_LENGTH                       32M

// Flash configuration.
#define OMV_FLASH_FFS_ORIGIN                  0x08020000
#define OMV_FLASH_FFS_LENGTH                  128K
#define OMV_FLASH_TXT_ORIGIN                  0x08040000
#define OMV_FLASH_TXT_LENGTH                  1792K

// Domain 1 DMA buffers region.
#define OMV_DMA_MEMORY_D1                     AXI_SRAM
#define OMV_DMA_MEMORY_D1_SIZE                (16 * 1024) // Reserved memory for DMA buffers
#define OMV_DMA_REGION_D1_BASE                (OMV_AXI_SRAM_ORIGIN + OMV_FB_OVERLAY_SIZE)
#define OMV_DMA_REGION_D1_SIZE                MPU_REGION_SIZE_16KB

// Domain 2 DMA buffers region.
#define OMV_DMA_MEMORY_D2                     SRAM3
#define OMV_DMA_MEMORY_D2_SIZE                (16 * 1024) // Reserved memory for DMA buffers
#define OMV_DMA_REGION_D2_BASE                (OMV_SRAM3_ORIGIN + (0 * 1024))
#define OMV_DMA_REGION_D2_SIZE                MPU_REGION_SIZE_32KB

// Domain 3 DMA buffers region.
#define OMV_DMA_MEMORY_D3                     SRAM4
#define OMV_DMA_MEMORY_D3_SIZE                (64 * 1024) // Reserved memory for DMA buffers
#define OMV_DMA_REGION_D3_BASE                (OMV_SRAM4_ORIGIN + (0 * 1024))
#define OMV_DMA_REGION_D3_SIZE                MPU_REGION_SIZE_64KB

// AXI QoS - Low-High (0:15) - default 0
#define OMV_AXI_QOS_MDMA_R_PRI                15 // Max pri to move data.
#define OMV_AXI_QOS_MDMA_W_PRI                15 // Max pri to move data.

// Main image sensor I2C bus
#define ISC_I2C_ID                            (1)
#define ISC_I2C_SPEED                         (OMV_I2C_SPEED_STANDARD)

// Thermal image sensor I2C bus
#define FIR_I2C_ID                            (2)
#define FIR_I2C_SPEED                         (OMV_I2C_SPEED_FULL)

// Soft I2C bus.
#define SOFT_I2C_SIOC_PIN                     (&omv_pin_B10_GPIO)
#define SOFT_I2C_SIOD_PIN                     (&omv_pin_B11_GPIO)
#define SOFT_I2C_SPIN_DELAY                   64

// Main image sensor SPI bus
#define ISC_SPI_ID                            (3)
#define ISC_SPI_BAUDRATE                      (20000000)

// WINC1500 WiFi module SPI bus
#define WINC_SPI_ID                           (2)
#define WINC_SPI_BAUDRATE                     (40000000)
#define WINC_EN_PIN                           (&omv_pin_A5_GPIO)
#define WINC_RST_PIN                          (&omv_pin_D12_GPIO)
#define WINC_IRQ_PIN                          (&omv_pin_D13_GPIO)

// DCMI timer.
#define DCMI_TIM                              (TIM1)
#define DCMI_TIM_PIN                          (&omv_pin_A8_TIM1)
#define DCMI_TIM_CHANNEL                      (TIM_CHANNEL_1)
#define DCMI_TIM_CLK_ENABLE()                 __TIM1_CLK_ENABLE()
#define DCMI_TIM_CLK_DISABLE()                __TIM1_CLK_DISABLE()
#define DCMI_TIM_PCLK_FREQ()                  HAL_RCC_GetPCLK2Freq()

// DCMI pins.
#define DCMI_RESET_PIN                        (&omv_pin_A10_GPIO)
#define DCMI_POWER_PIN                        (&omv_pin_D7_GPIO)
#define DCMI_FSYNC_PIN                        (&omv_pin_B4_GPIO)

#define DCMI_D0_PIN                           (&omv_pin_C6_DCMI)
#define DCMI_D1_PIN                           (&omv_pin_C7_DCMI)
#define DCMI_D2_PIN                           (&omv_pin_G10_DCMI)
#define DCMI_D3_PIN                           (&omv_pin_G11_DCMI)
#define DCMI_D4_PIN                           (&omv_pin_E4_DCMI)
#define DCMI_D5_PIN                           (&omv_pin_B6_DCMI)
#define DCMI_D6_PIN                           (&omv_pin_E5_DCMI)
#define DCMI_D7_PIN                           (&omv_pin_E6_DCMI)

#define DCMI_HSYNC_PIN                        (&omv_pin_A4_DCMI)
#define DCMI_VSYNC_PIN                        (&omv_pin_B7_DCMI)
#define DCMI_PXCLK_PIN                        (&omv_pin_A6_DCMI)

// Physical I2C buses.
// I2C bus 1
#define I2C1_ID                               (1)
#define I2C1_SCL_PIN                          (&omv_pin_B8_I2C1)
#define I2C1_SDA_PIN                          (&omv_pin_B9_I2C1)
// I2C bus 2
#define I2C2_ID                               (2)
#define I2C2_SCL_PIN                          (&omv_pin_B10_I2C2)
#define I2C2_SDA_PIN                          (&omv_pin_B11_I2C2)

// Physical SPI buses.
// SPI bus 2
#define SPI2_ID                               (2)
#define SPI2_SCLK_PIN                         (&omv_pin_B13_SPI2)
#define SPI2_MISO_PIN                         (&omv_pin_B14_SPI2)
#define SPI2_MOSI_PIN                         (&omv_pin_B15_SPI2)
#define SPI2_SSEL_PIN                         (&omv_pin_B12_SPI2)
#define SPI2_DMA_TX_CHANNEL                   (DMA1_Stream4)
#define SPI2_DMA_RX_CHANNEL                   (DMA1_Stream3)

#define SPI3_ID                               (3)
#define SPI3_SCLK_PIN                         (&omv_pin_B3_SPI3)
#define SPI3_MISO_PIN                         (&omv_pin_B4_SPI3)
#define SPI3_MOSI_PIN                         (&omv_pin_B5_SPI3)
#define SPI3_SSEL_PIN                         (&omv_pin_A15_SPI3)
#define SPI3_DMA_TX_CHANNEL                   (DMA1_Stream7)
#define SPI3_DMA_RX_CHANNEL                   (DMA1_Stream2)

// SPI LCD Interface
#define OMV_SPI_LCD_CONTROLLER                (&spi_obj[1])
#define OMV_SPI_LCD_CONTROLLER_INSTANCE       (SPI2)

#define OMV_SPI_LCD_MOSI_PIN                  (&omv_pin_B15_SPI2)
#define OMV_SPI_LCD_MISO_PIN                  (&omv_pin_B14_SPI2)
#define OMV_SPI_LCD_SCLK_PIN                  (&omv_pin_B13_SPI2)
#define OMV_SPI_LCD_SSEL_PIN                  (&omv_pin_B12_GPIO)

#define OMV_SPI_LCD_RS_PIN                    (&omv_pin_D13_GPIO)
#define OMV_SPI_LCD_BL_PIN                    (&omv_pin_A5_GPIO)
#define OMV_SPI_LCD_RST_PIN                   (&omv_pin_D12_GPIO)

#define OMV_SPI_LCD_BL_DAC                    (DAC1)
#define OMV_SPI_LCD_BL_DAC_CHANNEL            (DAC_CHANNEL_2)
#define OMV_SPI_LCD_BL_DAC_CLK_ENABLE()       __HAL_RCC_DAC12_CLK_ENABLE()
#define OMV_SPI_LCD_BL_DAC_CLK_DISABLE()      __HAL_RCC_DAC12_CLK_DISABLE()
#define OMV_SPI_LCD_BL_DAC_FORCE_RESET()      __HAL_RCC_DAC12_FORCE_RESET()
#define OMV_SPI_LCD_BL_DAC_RELEASE_RESET()    __HAL_RCC_DAC12_RELEASE_RESET()

// FIR Lepton
#define OMV_FIR_LEPTON_I2C_BUS                (FIR_I2C_ID)
#define OMV_FIR_LEPTON_I2C_BUS_SPEED          (FIR_I2C_SPEED)
#define OMV_FIR_LEPTON_CONTROLLER             (&spi_obj[1])
#define OMV_FIR_LEPTON_CONTROLLER_INSTANCE    (SPI2)

#define OMV_FIR_LEPTON_MOSI_PIN               (&omv_pin_B15_SPI2)
#define OMV_FIR_LEPTON_MISO_PIN               (&omv_pin_B14_SPI2)
#define OMV_FIR_LEPTON_SCLK_PIN               (&omv_pin_B13_SPI2)
#define OMV_FIR_LEPTON_SSEL_PIN               (&omv_pin_B12_GPIO)

#endif //__OMV_BOARDCONFIG_H__
