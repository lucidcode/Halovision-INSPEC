#define MICROPY_HW_BOARD_NAME       "OPENMV1"
#define MICROPY_HW_MCU_NAME         "STM32F407"
#define MICROPY_PY_SYS_PLATFORM     "OpenMV1"

#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SDCARD    (1)
#define MICROPY_HW_ENABLE_SPI1      (0)
#define MICROPY_HW_ENABLE_SPI2      (0)
#define MICROPY_HW_ENABLE_SPI3      (1)

#define MICROPY_HW_CLK_PLLM (12)
#define MICROPY_HW_CLK_PLLN (336)
#define MICROPY_HW_CLK_PLLQ (7)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)

#define USBD_VID                      0x1209
#define USBD_PID                      0xabd1
#define USBD_LANGID_STRING            0x409
#define USBD_MANUFACTURER_STRING      "OpenMV"
#define USBD_PRODUCT_FS_STRING        "OpenMV Virtual Comm Port in FS Mode"
#define USBD_PRODUCT_HS_STRING        "OpenMV Virtual Comm Port in HS Mode"
#define USBD_SERIALNUMBER_FS_STRING   "000000000011"
#define USBD_SERIALNUMBER_HS_STRING   "000000000010"
#define USBD_INTERFACE_FS_STRING      "VCP Interface"
#define USBD_INTERFACE_HS_STRING      "VCP Interface"
#define USBD_CONFIGURATION_FS_STRING  "VCP Config"
#define USBD_CONFIGURATION_HS_STRING  "VCP Config"

// UART config
#define MICROPY_HW_UART3_PORT (GPIOB)
#define MICROPY_HW_UART3_PINS (GPIO_PIN_10 | GPIO_PIN_11)

// I2C busses
#define MICROPY_HW_I2C2_SCL (pin_B10)
#define MICROPY_HW_I2C2_SDA (pin_B11)

// USB config
#define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_A9)

// USRSW is pulled low. Pressing the button makes the input go high.
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
#define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_D4) // red
#define MICROPY_HW_LED2             (pin_D6) // green
#define MICROPY_HW_LED3             (pin_D5) // blue
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->BSRRH = pin->pin_mask)
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->BSRRL = pin->pin_mask)

// Servos
#define PYB_SERVO_NUM (2)
