/*
 * This file is part of the OpenMV project.
 * The MIT License (MIT)
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 */

#define MICROPY_HW_BOARD_NAME       "OPENMV4-PRO"
#define MICROPY_HW_MCU_NAME         "STM32H743"
#define MICROPY_PY_SYS_PLATFORM     "OpenMV4 Pro"
#define MICROPY_STREAMS_POSIX_API   (1)

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
#define UINT_FMT                    "%u"
#define INT_FMT                     "%d"
typedef int mp_int_t;               // must be pointer size
typedef unsigned int mp_uint_t;     // must be pointer size

#define MICROPY_FATFS_EXFAT         (1)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (0)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_MMCARD    (0)
#define MICROPY_HW_CLK_USE_BYPASS   (1)
#define MICROPY_HW_TIM_IS_RESERVED(id)  (id == 1 || id == 6)

// Flash storage config
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE            (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE    (0)

#define MICROPY_BOARD_STARTUP       OPENMV4_H7_PRO_board_startup
void OPENMV4_H7_PRO_board_startup(void);

#define MICROPY_BOARD_EARLY_INIT    OPENMV4_H7_PRO_board_early_init
void OPENMV4_H7_PRO_board_early_init(void);

void OPENMV4_H7_PRO_board_low_power(int mode);
#define MICROPY_BOARD_LEAVE_STOP    OPENMV4_H7_PRO_board_low_power(0);
#define MICROPY_BOARD_ENTER_STOP    OPENMV4_H7_PRO_board_low_power(1);
#define MICROPY_BOARD_ENTER_STANDBY OPENMV4_H7_PRO_board_low_power(2);

void OPENMV4_H7_PRO_board_osc_enable(int enable);
#define MICROPY_BOARD_OSC_ENABLE    OPENMV4_H7_PRO_board_osc_enable(1);
#define MICROPY_BOARD_OSC_DISABLE   OPENMV4_H7_PRO_board_osc_enable(0);

#define MICROPY_HW_ENTER_BOOTLOADER_VIA_RESET   (0)
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args) NVIC_SystemReset()

// External 32kHz oscillator configuration
#define RTC_ASYNCH_PREDIV           (0)
#define RTC_SYNCH_PREDIV            (0x7fff)
#define MICROPY_HW_RTC_USE_BYPASS   (1)
#define MICROPY_HW_RTC_USE_US       (1)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

// UART4 config
#define MICROPY_HW_UART4_TX             (pin_A12)
#define MICROPY_HW_UART4_RX             (pin_A11)

// UART7 config
#define MICROPY_HW_UART7_TX             (pyb_pin_BT_TXD)
#define MICROPY_HW_UART7_RX             (pyb_pin_BT_RXD)
#define MICROPY_HW_UART7_RTS            (pyb_pin_BT_RTS)
#define MICROPY_HW_UART7_CTS            (pyb_pin_BT_CTS)

// I2C buses
#define MICROPY_HW_I2C1_SCL             (pin_B8)
#define MICROPY_HW_I2C1_SDA             (pin_B9)

// SPI buses
#define MICROPY_HW_SPI2_NSS             (pin_A11)
#define MICROPY_HW_SPI2_SCK             (pin_A12)
#define MICROPY_HW_SPI2_MISO            (pin_C2)
#define MICROPY_HW_SPI2_MOSI            (pin_C1)

// FDCAN bus
#define MICROPY_HW_CAN1_NAME            "FDCAN1"
#define MICROPY_HW_CAN1_TX              (pin_A12)
#define MICROPY_HW_CAN1_RX              (pin_A11)
#define MICROPY_HW_CAN_IS_RESERVED(id)  (id != PYB_CAN_1)

// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN    (pin_G7)
#define MICROPY_HW_SDCARD_DETECT_PULL   (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT (GPIO_PIN_RESET)

// SD Card SDMMC
#define MICROPY_HW_SDCARD_SDMMC         (1)
#define MICROPY_HW_SDCARD_CK            (pyb_pin_SD_CLK)
#define MICROPY_HW_SDCARD_CMD           (pyb_pin_SD_CMD)
#define MICROPY_HW_SDCARD_D0            (pyb_pin_SD_D0)
#define MICROPY_HW_SDCARD_D1            (pyb_pin_SD_D1)
#define MICROPY_HW_SDCARD_D2            (pyb_pin_SD_D2)
#define MICROPY_HW_SDCARD_D3            (pyb_pin_SD_D3)

// WiFi SDMMC
#define MICROPY_HW_SDIO_SDMMC           (2)
#define MICROPY_HW_SDIO_CK              (pyb_pin_WL_SDIO_CLK)
#define MICROPY_HW_SDIO_CMD             (pyb_pin_WL_SDIO_CMD)
#define MICROPY_HW_SDIO_D0              (pyb_pin_WL_SDIO_D0)
#define MICROPY_HW_SDIO_D1              (pyb_pin_WL_SDIO_D1)
#define MICROPY_HW_SDIO_D2              (pyb_pin_WL_SDIO_D2)
#define MICROPY_HW_SDIO_D3              (pyb_pin_WL_SDIO_D3)

// USB config
#define MICROPY_HW_USB_HS               (1)
#define MICROPY_HW_USB_HS_ULPI_NXT      (pin_C3)
#define MICROPY_HW_USB_HS_ULPI_STP      (pin_C0)
#define MICROPY_HW_USB_HS_ULPI_DIR      (pin_I11)
#define MICROPY_HW_USB_HS_ULPI3320      (1)

#define MICROPY_HW_USB_CDC_RX_DATA_SIZE (512)
#define MICROPY_HW_USB_CDC_TX_DATA_SIZE (512)
#define GPIO_AF10_OTG_HS                (GPIO_AF10_OTG2_HS)

// Bluetooth config
#define MICROPY_HW_BLE_UART_ID          (PYB_UART_7)
#define MICROPY_HW_BLE_UART_BAUDRATE    (115200)
#define MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY (3000000)

// LEDs
#define MICROPY_HW_LED1             (pyb_pin_LED_RED) // red
#define MICROPY_HW_LED2             (pyb_pin_LED_GREEN) // green
#define MICROPY_HW_LED3             (pyb_pin_LED_BLUE) // blue
#define MICROPY_HW_LED4             (pyb_pin_LED_IR) // IR
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->BSRR = (pin->pin_mask << 16))
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->BSRR = pin->pin_mask)

// Servos
#define PYB_SERVO_NUM (2)

#if (MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE == 0)
// QSPI Flash 256MBits
#define MICROPY_HW_QSPI_PRESCALER           (2) // F_CLK = F_AHB/2 (100MHz)
#define MICROPY_HW_SPIFLASH_SIZE_BITS       (248 * 1024 * 1024)
#define MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2 (28)
#define MICROPY_HW_QSPIFLASH_CS             (pyb_pin_QSPIFLASH_CS)
#define MICROPY_HW_QSPIFLASH_SCK            (pyb_pin_QSPIFLASH_SCK)
#define MICROPY_HW_QSPIFLASH_IO0            (pyb_pin_QSPIFLASH_IO0)
#define MICROPY_HW_QSPIFLASH_IO1            (pyb_pin_QSPIFLASH_IO1)
#define MICROPY_HW_QSPIFLASH_IO2            (pyb_pin_QSPIFLASH_IO2)
#define MICROPY_HW_QSPIFLASH_IO3            (pyb_pin_QSPIFLASH_IO3)

// block device config for SPI flash
extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_BDEV_IOCTL(op, arg) ( \
    (op) == BDEV_IOCTL_NUM_BLOCKS ? (MICROPY_HW_SPIFLASH_SIZE_BITS / 8 / FLASH_BLOCK_SIZE) : \
    (op) == BDEV_IOCTL_INIT ? spi_bdev_ioctl(&spi_bdev, (op), (uint32_t)&spiflash_config) : \
    spi_bdev_ioctl(&spi_bdev, (op), (arg)) \
)
#define MICROPY_HW_BDEV_READBLOCKS(dest, bl, n) spi_bdev_readblocks(&spi_bdev, (dest), (bl), (n))
#define MICROPY_HW_BDEV_WRITEBLOCKS(src, bl, n) spi_bdev_writeblocks(&spi_bdev, (src), (bl), (n))
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev)
#endif

// SDRAM
#define MICROPY_HW_SDRAM_SIZE               (32 * 1024 * 1024)
#define MICROPY_HW_SDRAM_STARTUP_TEST       (1)
#define MICROPY_HW_SDRAM_TEST_FAIL_ON_ERROR (true)

// Timing configuration for 200MHz/2=100MHz (10ns)
#define MICROPY_HW_SDRAM_CLOCK_PERIOD       2
#define MICROPY_HW_SDRAM_CAS_LATENCY        2
#define MICROPY_HW_SDRAM_FREQUENCY          (100000) // 100 MHz
#define MICROPY_HW_SDRAM_TIMING_TMRD        (2)
#define MICROPY_HW_SDRAM_TIMING_TXSR        (7)
#define MICROPY_HW_SDRAM_TIMING_TRAS        (5)
#define MICROPY_HW_SDRAM_TIMING_TRC         (6)
#define MICROPY_HW_SDRAM_TIMING_TWR         (3)
#define MICROPY_HW_SDRAM_TIMING_TRP         (2)
#define MICROPY_HW_SDRAM_TIMING_TRCD        (2)

// 32-bit SDRAM
#define MICROPY_HW_SDRAM_ROW_BITS_NUM       12
#define MICROPY_HW_SDRAM_MEM_BUS_WIDTH      32
#define MICROPY_HW_SDRAM_REFRESH_CYCLES     4096

#define MICROPY_HW_SDRAM_COLUMN_BITS_NUM    9
#define MICROPY_HW_SDRAM_INTERN_BANKS_NUM   4
#define MICROPY_HW_SDRAM_RPIPE_DELAY        0
#define MICROPY_HW_SDRAM_RBURST             (1)
#define MICROPY_HW_SDRAM_WRITE_PROTECTION   (0)

#define MICROPY_HW_SDRAM_AUTOREFRESH_NUM    (8)
#define MICROPY_HW_SDRAM_BURST_LENGTH       1

#define MICROPY_HW_SDRAM_REFRESH_RATE       (64) // ms

#define MICROPY_HW_FMC_SDCKE0   (pyb_pin_SDRAM_SDCKE0)
#define MICROPY_HW_FMC_SDNE0    (pyb_pin_SDRAM_SDNE0)
#define MICROPY_HW_FMC_SDCLK    (pyb_pin_SDRAM_SDCLK)
#define MICROPY_HW_FMC_SDNCAS   (pyb_pin_SDRAM_SDNCAS)
#define MICROPY_HW_FMC_SDNRAS   (pyb_pin_SDRAM_SDNRAS)
#define MICROPY_HW_FMC_SDNWE    (pyb_pin_SDRAM_SDNWE)
#define MICROPY_HW_FMC_BA0      (pyb_pin_SDRAM_BA0)
#define MICROPY_HW_FMC_BA1      (pyb_pin_SDRAM_BA1)
#define MICROPY_HW_FMC_NBL0     (pyb_pin_SDRAM_NBL0)
#define MICROPY_HW_FMC_NBL1     (pyb_pin_SDRAM_NBL1)
#define MICROPY_HW_FMC_NBL2     (pyb_pin_SDRAM_NBL2)
#define MICROPY_HW_FMC_NBL3     (pyb_pin_SDRAM_NBL3)
#define MICROPY_HW_FMC_A0       (pyb_pin_SDRAM_A0)
#define MICROPY_HW_FMC_A1       (pyb_pin_SDRAM_A1)
#define MICROPY_HW_FMC_A2       (pyb_pin_SDRAM_A2)
#define MICROPY_HW_FMC_A3       (pyb_pin_SDRAM_A3)
#define MICROPY_HW_FMC_A4       (pyb_pin_SDRAM_A4)
#define MICROPY_HW_FMC_A5       (pyb_pin_SDRAM_A5)
#define MICROPY_HW_FMC_A6       (pyb_pin_SDRAM_A6)
#define MICROPY_HW_FMC_A7       (pyb_pin_SDRAM_A7)
#define MICROPY_HW_FMC_A8       (pyb_pin_SDRAM_A8)
#define MICROPY_HW_FMC_A9       (pyb_pin_SDRAM_A9)
#define MICROPY_HW_FMC_A10      (pyb_pin_SDRAM_A10)
#define MICROPY_HW_FMC_A11      (pyb_pin_SDRAM_A11)
#define MICROPY_HW_FMC_D0       (pyb_pin_SDRAM_D0)
#define MICROPY_HW_FMC_D1       (pyb_pin_SDRAM_D1)
#define MICROPY_HW_FMC_D2       (pyb_pin_SDRAM_D2)
#define MICROPY_HW_FMC_D3       (pyb_pin_SDRAM_D3)
#define MICROPY_HW_FMC_D4       (pyb_pin_SDRAM_D4)
#define MICROPY_HW_FMC_D5       (pyb_pin_SDRAM_D5)
#define MICROPY_HW_FMC_D6       (pyb_pin_SDRAM_D6)
#define MICROPY_HW_FMC_D7       (pyb_pin_SDRAM_D7)
#define MICROPY_HW_FMC_D8       (pyb_pin_SDRAM_D8)
#define MICROPY_HW_FMC_D9       (pyb_pin_SDRAM_D9)
#define MICROPY_HW_FMC_D10      (pyb_pin_SDRAM_D10)
#define MICROPY_HW_FMC_D11      (pyb_pin_SDRAM_D11)
#define MICROPY_HW_FMC_D12      (pyb_pin_SDRAM_D12)
#define MICROPY_HW_FMC_D13      (pyb_pin_SDRAM_D13)
#define MICROPY_HW_FMC_D14      (pyb_pin_SDRAM_D14)
#define MICROPY_HW_FMC_D15      (pyb_pin_SDRAM_D15)
#define MICROPY_HW_FMC_D16      (pyb_pin_SDRAM_D16)
#define MICROPY_HW_FMC_D17      (pyb_pin_SDRAM_D17)
#define MICROPY_HW_FMC_D18      (pyb_pin_SDRAM_D18)
#define MICROPY_HW_FMC_D19      (pyb_pin_SDRAM_D19)
#define MICROPY_HW_FMC_D20      (pyb_pin_SDRAM_D20)
#define MICROPY_HW_FMC_D21      (pyb_pin_SDRAM_D21)
#define MICROPY_HW_FMC_D22      (pyb_pin_SDRAM_D22)
#define MICROPY_HW_FMC_D23      (pyb_pin_SDRAM_D23)
#define MICROPY_HW_FMC_D24      (pyb_pin_SDRAM_D24)
#define MICROPY_HW_FMC_D25      (pyb_pin_SDRAM_D25)
#define MICROPY_HW_FMC_D26      (pyb_pin_SDRAM_D26)
#define MICROPY_HW_FMC_D27      (pyb_pin_SDRAM_D27)
#define MICROPY_HW_FMC_D28      (pyb_pin_SDRAM_D28)
#define MICROPY_HW_FMC_D29      (pyb_pin_SDRAM_D29)
#define MICROPY_HW_FMC_D30      (pyb_pin_SDRAM_D30)
#define MICROPY_HW_FMC_D31      (pyb_pin_SDRAM_D31)

#define MICROPY_HW_USB_VID                      0x1209
#define MICROPY_HW_USB_PID                      0xabd1
#define MICROPY_HW_USB_PID_CDC_MSC              (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_HID              (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC                  (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_MSC                  (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2_MSC             (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2                 (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3                 (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3_MSC             (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC_MSC_HID          (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC2_MSC_HID         (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_PID_CDC3_MSC_HID         (MICROPY_HW_USB_PID)
#define MICROPY_HW_USB_LANGID_STRING            0x409
#define MICROPY_HW_USB_MANUFACTURER_STRING      "OpenMV"
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "OpenMV Virtual Comm Port in FS Mode"
#define MICROPY_HW_USB_PRODUCT_HS_STRING        "OpenMV Virtual Comm Port in HS Mode"
#define MICROPY_HW_USB_SERIALNUMBER_FS_STRING   "000000000011"
#define MICROPY_HW_USB_SERIALNUMBER_HS_STRING   "000000000010"
#define MICROPY_HW_USB_INTERFACE_FS_STRING      "VCP Interface"
#define MICROPY_HW_USB_INTERFACE_HS_STRING      "VCP Interface"
#define MICROPY_HW_USB_CONFIGURATION_FS_STRING  "VCP Config"
#define MICROPY_HW_USB_CONFIGURATION_HS_STRING  "VCP Config"
