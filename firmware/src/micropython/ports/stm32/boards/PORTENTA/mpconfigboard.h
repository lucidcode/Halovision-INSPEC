#define MICROPY_HW_BOARD_NAME       "PORTENTA"
#define MICROPY_HW_MCU_NAME         "STM32H747"
#define MICROPY_HW_FLASH_FS_LABEL	"portenta"

#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SDCARD    (0)
#define MICROPY_HW_ENABLE_MMCARD    (0)

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

#define MICROPY_HW_CLK_USE_BYPASS	(1)

#define MICROPY_PY_USOCKET          (1)
#define MICROPY_PY_NETWORK          (1)
#define MICROPY_PY_THREAD           (1)
#define MICROPY_PY_RNDIS			(0)
#define OPENAMP_PY					(0)
#define MICROPY_JERRYSCRIPT         (0)

#define MICROPY_BOARD_EARLY_INIT PORTENTA_board_early_init
void PORTENTA_board_early_init(void);

#define ARDUINO_1200BPS_TOUCH PORTENTA_reboot_to_bootloader
void PORTENTA_reboot_to_bootloader(void);

// The board has an 25MHz HSE, the following gives 450MHz CPU speed
#define MICROPY_HW_CLK_PLLM (5)
#define MICROPY_HW_CLK_PLLN (180)
#define MICROPY_HW_CLK_PLLFRACN (0)
#define MICROPY_HW_CLK_PLLP (2)
#define MICROPY_HW_CLK_PLLQ (10)
#define MICROPY_HW_CLK_PLLR (2)

// The USB clock is set using PLL3
#define MICROPY_HW_CLK_PLL3M (25)
#define MICROPY_HW_CLK_PLL3N (336)
#define MICROPY_HW_CLK_PLL3FRACN (0)
#define MICROPY_HW_CLK_PLL3P (2)
#define MICROPY_HW_CLK_PLL3Q (7)
#define MICROPY_HW_CLK_PLL3R (2)

// There is an external 32kHz oscillator
#define RTC_ASYNCH_PREDIV           (0)
#define RTC_SYNCH_PREDIV            (0x7fff)
#define MICROPY_HW_RTC_USE_BYPASS   (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

// QSPI flash #2, to be memory mapped
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (27)
#define MICROPY_HW_SPIFLASH_SIZE_BITS (128 * 1024 * 1024)
#define MICROPY_HW_QSPIFLASH_CS     (pyb_pin_QSPI2_CS)
#define MICROPY_HW_QSPIFLASH_SCK    (pyb_pin_QSPI2_CLK)
#define MICROPY_HW_QSPIFLASH_IO0    (pyb_pin_QSPI2_D0)
#define MICROPY_HW_QSPIFLASH_IO1    (pyb_pin_QSPI2_D1)
#define MICROPY_HW_QSPIFLASH_IO2    (pyb_pin_QSPI2_D2)
#define MICROPY_HW_QSPIFLASH_IO3    (pyb_pin_QSPI2_D3)

#define MICROPY_HW_QSPI_SAMPLE_SHIFT	(0)

// SPI flash #2, block device config
#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE == 0
  // SPI flash #2, block device config
  extern const struct _mp_spiflash_config_t spiflash2_config;
  extern struct _spi_bdev_t spi_bdev2;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev2, (op), (uint32_t)&spiflash2_config) : \
    spi_bdev_ioctl(&spi_bdev2, (op), (arg)) \
)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev2, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev2, (src), (bl), (n))
#endif

// 4 wait states
#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_2

// UART config
#define MICROPY_HW_UART1_TX         (pin_A9)
#define MICROPY_HW_UART1_RX         (pin_A10)
#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C3_SCL         (pin_H7)
#define MICROPY_HW_I2C3_SDA         (pin_H8)

// SPI
//#define MICROPY_HW_SPI2_NSS         (pin_I0)
//#define MICROPY_HW_SPI2_SCK         (pin_I1)
//#define MICROPY_HW_SPI2_MISO        (pin_B14)
//#define MICROPY_HW_SPI2_MOSI        (pin_B15)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LEDR) // red
#define MICROPY_HW_LED2             (pyb_pin_LEDG) // green
#define MICROPY_HW_LED3             (pyb_pin_LEDB) // yellow
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_high(pin))

// USB config
#define MICROPY_HW_USB_HS           (1)
//#define MICROPY_HW_USB_FS			(1)
#define USBD_CDC_RX_DATA_SIZE       (512)
#define USBD_CDC_TX_DATA_SIZE       (512)
#define GPIO_AF10_OTG_HS            (GPIO_AF10_OTG2_HS)

// SDRAM
#define MICROPY_HW_SDRAM_SIZE  			(64 / 8 * 1024 * 1024)  // 64 Mbit
#define MICROPY_HW_SDRAM_STARTUP_TEST   (1)
//#define MICROPY_HEAP_START              sdram_start()
//#define MICROPY_HEAP_END                sdram_end()

// Timing configuration for 90 Mhz (11.90ns) of SD clock frequency (180Mhz/2)
#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (7)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (4)
#define MICROPY_HW_SDRAM_TIMING_TRC         (7)
#define MICROPY_HW_SDRAM_TIMING_TWR         (2)
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)
#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms
#define MICROPY_HW_SDRAM_FREQUENCY          (90000) // 100 MHz

#define MICROPY_HW_SDRAM_BURST_LENGTH       2
#define MICROPY_HW_SDRAM_CAS_LATENCY        3
#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    8
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       12
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      16
#define MICROPY_HW_SDRAM_REFRESH_CYCLES     8192
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2
#define MICROPY_HW_SDRAM_RPIPE_DELAY        1
#define MICROPY_HW_SDRAM_RBURST             (0)
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)
#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (4)

#define MICROPY_HW_FMC_SDCKE0   (pin_H2)
#define MICROPY_HW_FMC_SDNE0    (pin_H3)  // SD_CS ????
#define MICROPY_HW_FMC_SDNBL0   (pin_E0)
#define MICROPY_HW_FMC_SDNBL1   (pin_E1)
//#define MICROPY_HW_FMC_SDNE1    (pin_B6)
#define MICROPY_HW_FMC_SDCLK    (pin_G8)
#define MICROPY_HW_FMC_SDNCAS   (pin_G15)
#define MICROPY_HW_FMC_SDNRAS   (pin_F11)
#define MICROPY_HW_FMC_SDNWE    (pin_H5)
#define MICROPY_HW_FMC_BA0      (pin_G4)
#define MICROPY_HW_FMC_BA1      (pin_G5)
#define MICROPY_HW_FMC_NBL0     (pin_E0)
#define MICROPY_HW_FMC_NBL1     (pin_E1)
#define MICROPY_HW_FMC_A0       (pin_F0)
#define MICROPY_HW_FMC_A1       (pin_F1)
#define MICROPY_HW_FMC_A2       (pin_F2)
#define MICROPY_HW_FMC_A3       (pin_F3)
#define MICROPY_HW_FMC_A4       (pin_F4)
#define MICROPY_HW_FMC_A5       (pin_F5)
#define MICROPY_HW_FMC_A6       (pin_F12)
#define MICROPY_HW_FMC_A7       (pin_F13)
#define MICROPY_HW_FMC_A8       (pin_F14)
#define MICROPY_HW_FMC_A9       (pin_F15)
#define MICROPY_HW_FMC_A10      (pin_G0)
#define MICROPY_HW_FMC_A11      (pin_G1)
#define MICROPY_HW_FMC_A12      (pin_G2)
#define MICROPY_HW_FMC_D0       (pin_D14)
#define MICROPY_HW_FMC_D1       (pin_D15)
#define MICROPY_HW_FMC_D2       (pin_D0)
#define MICROPY_HW_FMC_D3       (pin_D1)
#define MICROPY_HW_FMC_D4       (pin_E7)
#define MICROPY_HW_FMC_D5       (pin_E8)
#define MICROPY_HW_FMC_D6       (pin_E9)
#define MICROPY_HW_FMC_D7       (pin_E10)
#define MICROPY_HW_FMC_D8       (pin_E11)
#define MICROPY_HW_FMC_D9       (pin_E12)
#define MICROPY_HW_FMC_D10      (pin_E13)
#define MICROPY_HW_FMC_D11      (pin_E14)
#define MICROPY_HW_FMC_D12      (pin_E15)
#define MICROPY_HW_FMC_D13      (pin_D8)
#define MICROPY_HW_FMC_D14      (pin_D9)
#define MICROPY_HW_FMC_D15      (pin_D10)

// Ethernet via RMII
/*
#define MICROPY_HW_ETH_MDC          (pin_C1)
#define MICROPY_HW_ETH_MDIO         (pin_A2)
#define MICROPY_HW_ETH_RMII_REF_CLK (pin_A1)
#define MICROPY_HW_ETH_RMII_CRS_DV  (pin_A7)
#define MICROPY_HW_ETH_RMII_RXD0    (pin_C4)
#define MICROPY_HW_ETH_RMII_RXD1    (pin_C5)
#define MICROPY_HW_ETH_RMII_TX_EN   (pin_G11)
#define MICROPY_HW_ETH_RMII_TXD0    (pin_G13)
#define MICROPY_HW_ETH_RMII_TXD1    (pin_G12)
*/

#define USBD_MANUFACTURER_STRING      "Arduino"
#define USBD_PRODUCT_HS_STRING        "Portenta Virtual Comm Port in HS Mode"
#define USBD_PRODUCT_FS_STRING        "Portenta Virtual Comm Port in FS Mode"
#define USBD_CONFIGURATION_HS_STRING  "Portenta Config"
#define USBD_INTERFACE_HS_STRING      "Portenta Interface"
#define USBD_CONFIGURATION_FS_STRING  "Portenta Config"
#define USBD_INTERFACE_FS_STRING      "Portenta Interface"
#define MICROPY_PY_SYS_PLATFORM       "Portenta"
#define USBD_PID_RNDIS_CDC_MSC        0x005B
#define USBD_PID                      0x005B
#define USBD_VID                      0x2341
