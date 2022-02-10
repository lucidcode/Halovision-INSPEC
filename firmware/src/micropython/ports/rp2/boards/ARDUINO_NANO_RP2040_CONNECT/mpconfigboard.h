// Board and hardware specific configuration
#include "RP2040.h"
#define MICROPY_HW_BOARD_NAME                   "Arduino Nano RP2040 Connect"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (8 * 1024 * 1024)

// Enable USB Mass Storage with FatFS filesystem.
#define MICROPY_HW_USB_MSC              (1)
#define MICROPY_HW_USB_VID              (0x2341)
#define MICROPY_HW_USB_PID              (0x015e)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH (1)

// UART 1 config.
#define MICROPY_HW_UART1_TX             (8)
#define MICROPY_HW_UART1_RX             (9)
#define MICROPY_HW_UART1_CTS            (10)
#define MICROPY_HW_UART1_RTS            (11)

// I2C0 config.
#define MICROPY_HW_I2C0_SCL             (13)
#define MICROPY_HW_I2C0_SDA             (12)

// I2C1 config.
#define MICROPY_HW_I2C1_SCL             (27)
#define MICROPY_HW_I2C1_SDA             (26)

// Bluetooth config.
#define MICROPY_HW_NINA_RESET           (3)
#define MICROPY_HW_NINA_GPIO1           (9)
#define MICROPY_HW_BLE_UART_ID          (1)
#define MICROPY_HW_BLE_UART_BAUDRATE    (119600)
