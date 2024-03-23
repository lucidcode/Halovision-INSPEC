#pragma once

#include <stdbool.h>
#include <stdint.h>

#define _VC_MAX(x, y) (((x) > (y)) ? (x) : (y))

// Request and response buffer. The request will be overwritten by the response.
typedef struct {
    uint32_t buffer_size;
    uint32_t code;
    // Series of tagged data where the final tag is 0x0.
    uint8_t data[];
} vcmailbox_buffer_t;

#define VCMAILBOX_HEADER_SIZE (2 * sizeof(uint32_t))

#define VCMAILBOX_CODE_PROCESS_REQUEST    0x00000000
#define VCMAILBOX_CODE_REQUEST_SUCCESSFUL 0x80000000
#define VCMAILBOX_CODE_PROCESSING_ERROR   0x80000001

typedef enum {
    VCMAILBOX_CHANNEL_POWER = 0x0,
    VCMAILBOX_CHANNEL_FRAMEBUFFER = 0x1,
    VCMAILBOX_CHANNEL_UART = 0x2,
    VCMAILBOX_CHANNEL_VCHIQ = 0x3,
    VCMAILBOX_CHANNEL_LEDS = 0x4,
    VCMAILBOX_CHANNEL_BUTTONS = 0x5,
    VCMAILBOX_CHANNEL_TOUCH_SCREEN = 0x6,
    VCMAILBOX_CHANNEL_COUNT = 0x7,
    VCMAILBOX_CHANNEL_PROPERTY_ARM_TO_VC = 0x8,
    VCMAILBOX_CHANNEL_PROPERTY_VC_TO_ARM = 0x9,
} vcmailbox_channel_t;

typedef enum {
    VCMAILBOX_DEVICE_SD_CARD = 0x00000000,
    VCMAILBOX_DEVICE_UART0 = 0x00000001,
    VCMAILBOX_DEVICE_UART1 = 0x00000002,
    VCMAILBOX_DEVICE_USB_HCD = 0x00000003,
    VCMAILBOX_DEVICE_I2C0 = 0x00000004,
    VCMAILBOX_DEVICE_I2C1 = 0x00000005,
    VCMAILBOX_DEVICE_I2C2 = 0x00000006,
    VCMAILBOX_DEVICE_SPI = 0x00000007,
    VCMAILBOX_DEVICE_CCP2TX = 0x00000008,
    VCMAILBOX_DEVICE_UNKNOWN_RPI4_1 = 0x00000009,
    VCMAILBOX_DEVICE_UNKNOWN_RPI4_2 = 0x0000000a,
} vcmailbox_device_t;

typedef enum {
    VCMAILBOX_CLOCK_RESERVED = 0x00000000,
    VCMAILBOX_CLOCK_EMMC = 0x00000001,
    VCMAILBOX_CLOCK_UART = 0x00000002,
    VCMAILBOX_CLOCK_ARM = 0x00000003,
    VCMAILBOX_CLOCK_CORE = 0x00000004,
    VCMAILBOX_CLOCK_V3D = 0x00000005,
    VCMAILBOX_CLOCK_H264 = 0x00000006,
    VCMAILBOX_CLOCK_ISP = 0x00000007,
    VCMAILBOX_CLOCK_SDRAM = 0x00000008,
    VCMAILBOX_CLOCK_PIXEL = 0x00000009,
    VCMAILBOX_CLOCK_PWM = 0x0000000a,
    VCMAILBOX_CLOCK_HEVC = 0x0000000b,
    VCMAILBOX_CLOCK_EMMC2 = 0x0000000c,
    VCMAILBOX_CLOCK_M2MC = 0x0000000d,
    VCMAILBOX_CLOCK_PIXEL_BVB = 0x0000000e,
} vcmailbox_clock_id_t;

typedef enum {
    VCMAILBOX_VOLTAGE_RESERVED = 0x00000000,
    VCMAILBOX_VOLTAGE_CORE = 0x00000001,
    VCMAILBOX_VOLTAGE_SDRAM_C = 0x00000002,
    VCMAILBOX_VOLTAGE_SDRAM_P = 0x00000003,
    VCMAILBOX_VOLTAGE_SDRAM_I = 0x00000004,
} vcmailbox_voltage_id_t;        

// Can be resized to 0 at any time. Use for cached data
#define VCMAILBOX_MEM_FLAG_DISCARDABLE 0x00000001
// Normal allocating alias. Don't use from ARM
#define VCMAILBOX_MEM_FLAG_NORMAL 0x00000000
// 0xC alias uncached
#define VCMAILBOX_MEM_FLAG_DIRECT 0x00000004
// 0x8 alias. Non-callocating in L2 but coherent
#define VCMAILBOX_MEM_FLAG_COHERENT 0x00000008
// Allocating in L2
#define VCMAILBOX_MEM_FLAG_L1_NONALLOCATING 0x0000000c
// initialise buffer to all zeros
#define VCMAILBOX_MEM_FLAG_ZERO 0x00000010
// Don't initialise (default is to initialise to all ones)
#define VCMAILBOX_MEM_FLAG_NO_INIT 0x00000020
// Likely to be locked for long periods of time.
#define VCMAILBOX_MEM_FLAG_HINT_PERMALOCK 0x00000040

// Get firmware revision
//
#define VCMAILBOX_TAG_GET_FIRMWARE_REVISION 0x00000001
#define VCMAILBOX_GET_FIRMWARE_REVISION_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_firmware_revision_response_t))
#define VCMAILBOX_GET_FIRMWARE_REVISION_DEFAULTS (vcmailbox_get_firmware_revision_t) { \
    .tag = VCMAILBOX_TAG_GET_FIRMWARE_REVISION, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_FIRMWARE_REVISION_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_FIRMWARE_REVISION_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t firmware_revision; // firmware revision
} vcmailbox_get_firmware_revision_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_firmware_revision_response_t response;
} vcmailbox_get_firmware_revision_t;

// Get board model
//
#define VCMAILBOX_TAG_GET_BOARD_MODEL 0x00010001
#define VCMAILBOX_GET_BOARD_MODEL_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_board_model_response_t))
#define VCMAILBOX_GET_BOARD_MODEL_DEFAULTS (vcmailbox_get_board_model_t) { \
    .tag = VCMAILBOX_TAG_GET_BOARD_MODEL, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_BOARD_MODEL_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_BOARD_MODEL_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t board_model; // board model
} vcmailbox_get_board_model_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_board_model_response_t response;
} vcmailbox_get_board_model_t;

// Get board revision
//
#define VCMAILBOX_TAG_GET_BOARD_REVISION 0x00010002
#define VCMAILBOX_GET_BOARD_REVISION_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_board_revision_response_t))
#define VCMAILBOX_GET_BOARD_REVISION_DEFAULTS (vcmailbox_get_board_revision_t) { \
    .tag = VCMAILBOX_TAG_GET_BOARD_REVISION, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_BOARD_REVISION_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_BOARD_REVISION_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t board_revision; // board revision
} vcmailbox_get_board_revision_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_board_revision_response_t response;
} vcmailbox_get_board_revision_t;

// Get board MAC address
//
#define VCMAILBOX_TAG_GET_BOARD_MAC_ADDRESS 0x00010003
#define VCMAILBOX_GET_BOARD_MAC_ADDRESS_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_board_mac_address_response_t))
#define VCMAILBOX_GET_BOARD_MAC_ADDRESS_DEFAULTS (vcmailbox_get_board_mac_address_t) { \
    .tag = VCMAILBOX_TAG_GET_BOARD_MAC_ADDRESS, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_BOARD_MAC_ADDRESS_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_BOARD_MAC_ADDRESS_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint8_t mac_address[6]; // MAC address in network byte order
} vcmailbox_get_board_mac_address_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_board_mac_address_response_t response;
} vcmailbox_get_board_mac_address_t;

// Get board serial
//
#define VCMAILBOX_TAG_GET_BOARD_SERIAL 0x00010004
#define VCMAILBOX_GET_BOARD_SERIAL_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_board_serial_response_t))
#define VCMAILBOX_GET_BOARD_SERIAL_DEFAULTS (vcmailbox_get_board_serial_t) { \
    .tag = VCMAILBOX_TAG_GET_BOARD_SERIAL, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_BOARD_SERIAL_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_BOARD_SERIAL_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint64_t board_serial; // board serial
} vcmailbox_get_board_serial_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_board_serial_response_t response;
} vcmailbox_get_board_serial_t;

// Get ARM memory
//
// Future formats may specify multiple base+size combinations.
// 
#define VCMAILBOX_TAG_GET_ARM_MEMORY 0x00010005
#define VCMAILBOX_GET_ARM_MEMORY_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_arm_memory_response_t))
#define VCMAILBOX_GET_ARM_MEMORY_DEFAULTS (vcmailbox_get_arm_memory_t) { \
    .tag = VCMAILBOX_TAG_GET_ARM_MEMORY, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_ARM_MEMORY_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_ARM_MEMORY_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t base_address; // base address in bytes
    uint32_t size; // size in bytes
} vcmailbox_get_arm_memory_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_arm_memory_response_t response;
} vcmailbox_get_arm_memory_t;

// Get VC memory
//
// Future formats may specify multiple base+size combinations.
// 
#define VCMAILBOX_TAG_GET_VC_MEMORY 0x00010006
#define VCMAILBOX_GET_VC_MEMORY_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_vc_memory_response_t))
#define VCMAILBOX_GET_VC_MEMORY_DEFAULTS (vcmailbox_get_vc_memory_t) { \
    .tag = VCMAILBOX_TAG_GET_VC_MEMORY, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_VC_MEMORY_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_VC_MEMORY_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t base_address; // base address in bytes
    uint32_t size; // size in bytes
} vcmailbox_get_vc_memory_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_vc_memory_response_t response;
} vcmailbox_get_vc_memory_t;

// Get clocks
//
//      * (repeated)
// 
// Returns all clocks that exist **in top down breadth first order**. Clocks
// that depend on another clock should be defined as children of that clock.
// Clocks that depend on no other clocks should have no parent. Clock IDs are as
// in the clock section below.
// 
// (swarren: This clock message is much less well defined than the other clock
// message below. To be useful, you'd need to either return a clock name along
// with each clock so the caller knew what they all were, or pre-define the list
// of valid clock IDs as the other clock message does below)
// 
// (lp0: I've now made it clear that the clock IDs are as below, which is why I
// reserved 0)
// 
#define VCMAILBOX_TAG_GET_CLOCKS 0x00010007
#define VCMAILBOX_GET_CLOCKS_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_clocks_response_t))
#define VCMAILBOX_GET_CLOCKS_DEFAULTS (vcmailbox_get_clocks_t) { \
    .tag = VCMAILBOX_TAG_GET_CLOCKS, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_CLOCKS_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_CLOCKS_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t parent_clock_id; // parent clock id (0 for a root clock)
    uint32_t clock_id; // clock id
} vcmailbox_get_clocks_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_clocks_response_t response;
} vcmailbox_get_clocks_t;

// Get command line
//
// Caller should not assume the string is null terminated.
// 
#define VCMAILBOX_TAG_GET_COMMAND_LINE 0x00050001
#define VCMAILBOX_GET_COMMAND_LINE_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_command_line_response_t))
#define VCMAILBOX_GET_COMMAND_LINE_DEFAULTS (vcmailbox_get_command_line_t) { \
    .tag = VCMAILBOX_TAG_GET_COMMAND_LINE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_COMMAND_LINE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_COMMAND_LINE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint8_t ascii_command_line_string[128]; // ASCII command line string
} vcmailbox_get_command_line_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_command_line_response_t response;
} vcmailbox_get_command_line_t;

// Get DMA channels
//
// Caller assumes that the VC has enabled all the usable DMA channels.
// 
// Unique device IDs:
// 
#define VCMAILBOX_TAG_GET_DMA_CHANNELS 0x00060001
#define VCMAILBOX_GET_DMA_CHANNELS_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_dma_channels_response_t))
#define VCMAILBOX_GET_DMA_CHANNELS_DEFAULTS (vcmailbox_get_dma_channels_t) { \
    .tag = VCMAILBOX_TAG_GET_DMA_CHANNELS, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_DMA_CHANNELS_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_DMA_CHANNELS_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t mask; // mask
    uint16_t usable_dma_channels; // 0-15: DMA channels 0-15 (0=do not use, 1=usable)
    uint16_t reserved; // 16-31: reserved for future use
} vcmailbox_get_dma_channels_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_dma_channels_response_t response;
} vcmailbox_get_dma_channels_t;

// Get power state
//
//      * Bit 0: 0=off, 1=on
// 
//      * Bit 1: 0=device exists, 1=device does not exist
// 
//      * Bits 2-31: reserved for future use
// 
// Response indicates current state.
// 
#define VCMAILBOX_TAG_GET_POWER_STATE 0x00020001
#define VCMAILBOX_GET_POWER_STATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_power_state_request_t), sizeof(vcmailbox_get_power_state_response_t))
#define VCMAILBOX_GET_POWER_STATE_DEFAULTS (vcmailbox_get_power_state_t) { \
    .tag = VCMAILBOX_TAG_GET_POWER_STATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_POWER_STATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_POWER_STATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t device_id; // device id
} vcmailbox_get_power_state_request_t;

typedef struct __attribute__((packed)) {
    uint32_t device_id; // device id
    uint32_t state; // state
} vcmailbox_get_power_state_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_power_state_request_t request;
        vcmailbox_get_power_state_response_t response;
    };
} vcmailbox_get_power_state_t;

// Get timing
//
// Response indicates wait time required after turning a device on before power
// is stable. Returns 0 wait time if the device does not exist.
// 
#define VCMAILBOX_TAG_GET_TIMING 0x00020002
#define VCMAILBOX_GET_TIMING_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_timing_request_t), sizeof(vcmailbox_get_timing_response_t))
#define VCMAILBOX_GET_TIMING_DEFAULTS (vcmailbox_get_timing_t) { \
    .tag = VCMAILBOX_TAG_GET_TIMING, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_TIMING_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_TIMING_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t device_id; // device id
} vcmailbox_get_timing_request_t;

typedef struct __attribute__((packed)) {
    uint32_t device_id; // device id
    uint32_t enable_wait_time; // enable wait time in microseconds
} vcmailbox_get_timing_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_timing_request_t request;
        vcmailbox_get_timing_response_t response;
    };
} vcmailbox_get_timing_t;

// Set power state
//
//      * Bit 0: 0=off, 1=on
// 
//      * Bit 1: 0=do not wait, 1=wait
// 
//      * Bits 2-31: reserved for future use (set to 0)
// 
//      * Bit 0: 0=off, 1=on
// 
//      * Bit 1: 0=device exists, 1=device does not exist
// 
//      * Bits 2-31: reserved for future use
// 
// Response indicates new state, with/without waiting for the power to become
// stable.
// 
// Unique clock IDs:
// 
// (swarren: I imagine there are more clocks than that; the clock message
// earlier returned clock parent information, and I doubt any of the clocks
// listed here are parents of each-other. At the very least I'd expect one more
// defined clock ID to represent the root crystal/PLL, and I'd expect there are
// a actually a variety of intermediate clocks between this and these
// peripherals).
// 
// All clocks are the **base clocks** for those peripherals, e.g. 3MHz for UART,
// 50/100MHz for EMMC, not the dividers applied using the peripheral.
// 
#define VCMAILBOX_TAG_SET_POWER_STATE 0x00028001
#define VCMAILBOX_SET_POWER_STATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_power_state_request_t), sizeof(vcmailbox_set_power_state_response_t))
#define VCMAILBOX_SET_POWER_STATE_DEFAULTS (vcmailbox_set_power_state_t) { \
    .tag = VCMAILBOX_TAG_SET_POWER_STATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_POWER_STATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_POWER_STATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t device_id; // device id
    uint32_t state; // state
} vcmailbox_set_power_state_request_t;

typedef struct __attribute__((packed)) {
    uint32_t device_id; // device id
    uint32_t state; // state
} vcmailbox_set_power_state_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_power_state_request_t request;
        vcmailbox_set_power_state_response_t response;
    };
} vcmailbox_set_power_state_t;

// Get clock state
//
//      * Bit 0: 0=off, 1=on
// 
//      * Bit 1: 0=clock exists, 1=clock does not exist
// 
//      * Bits 2-31: reserved for future use
// 
#define VCMAILBOX_TAG_GET_CLOCK_STATE 0x00030001
#define VCMAILBOX_GET_CLOCK_STATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_clock_state_request_t), sizeof(vcmailbox_get_clock_state_response_t))
#define VCMAILBOX_GET_CLOCK_STATE_DEFAULTS (vcmailbox_get_clock_state_t) { \
    .tag = VCMAILBOX_TAG_GET_CLOCK_STATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_CLOCK_STATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_CLOCK_STATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
} vcmailbox_get_clock_state_request_t;

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t state; // state
} vcmailbox_get_clock_state_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_clock_state_request_t request;
        vcmailbox_get_clock_state_response_t response;
    };
} vcmailbox_get_clock_state_t;

// Set clock state
//
//      * Bit 0: 0=off, 1=on
// 
//      * Bit 1: 0=clock exists, 1=clock does not exist
// 
//      * Bits 2-31: reserved for future use (set to 0)
// 
//      * Bit 0: 0=off, 1=on
// 
//      * Bit 1: 0=clock exists, 1=clock does not exist
// 
//      * Bits 2-31: reserved for future use
// 
// (swarren: it doesn't seem to make sense for the request to have a "clock
// exists" bit, only the response)
// 
// (lp0: the lack of a response makes it unclear if the tag itself was
// unsupported)
// 
#define VCMAILBOX_TAG_SET_CLOCK_STATE 0x00038001
#define VCMAILBOX_SET_CLOCK_STATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_clock_state_request_t), sizeof(vcmailbox_set_clock_state_response_t))
#define VCMAILBOX_SET_CLOCK_STATE_DEFAULTS (vcmailbox_set_clock_state_t) { \
    .tag = VCMAILBOX_TAG_SET_CLOCK_STATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_CLOCK_STATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_CLOCK_STATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t state; // state
} vcmailbox_set_clock_state_request_t;

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t state; // state
} vcmailbox_set_clock_state_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_clock_state_request_t request;
        vcmailbox_set_clock_state_response_t response;
    };
} vcmailbox_set_clock_state_t;

// Get clock rate
//
// Next enable rate should be returned even if the clock is not running. A rate
// of 0 is returned if the clock does not exist.
// 
#define VCMAILBOX_TAG_GET_CLOCK_RATE 0x00030002
#define VCMAILBOX_GET_CLOCK_RATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_clock_rate_request_t), sizeof(vcmailbox_get_clock_rate_response_t))
#define VCMAILBOX_GET_CLOCK_RATE_DEFAULTS (vcmailbox_get_clock_rate_t) { \
    .tag = VCMAILBOX_TAG_GET_CLOCK_RATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_CLOCK_RATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_CLOCK_RATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
} vcmailbox_get_clock_rate_request_t;

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t rate_in_hz; // rate (in Hz)
} vcmailbox_get_clock_rate_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_clock_rate_request_t request;
        vcmailbox_get_clock_rate_response_t response;
    };
} vcmailbox_get_clock_rate_t;

// Get clock rate measured
//
// Get the true/actual clock rate (instead of the last requested value returned
// by "Get clock rate"/0x0003002) which respects clamping, throttling and clock
// divider limitations.
// 
#define VCMAILBOX_TAG_GET_CLOCK_RATE_MEASURED 0x00030047
#define VCMAILBOX_GET_CLOCK_RATE_MEASURED_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_clock_rate_measured_request_t), sizeof(vcmailbox_get_clock_rate_measured_response_t))
#define VCMAILBOX_GET_CLOCK_RATE_MEASURED_DEFAULTS (vcmailbox_get_clock_rate_measured_t) { \
    .tag = VCMAILBOX_TAG_GET_CLOCK_RATE_MEASURED, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_CLOCK_RATE_MEASURED_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_CLOCK_RATE_MEASURED_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
} vcmailbox_get_clock_rate_measured_request_t;

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t rate_in_hz; // rate (in Hz)
} vcmailbox_get_clock_rate_measured_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_clock_rate_measured_request_t request;
        vcmailbox_get_clock_rate_measured_response_t response;
    };
} vcmailbox_get_clock_rate_measured_t;

// Set clock rate
//
// Next supported enable rate should be returned even if the clock is not
// running. A rate of 0 is returned if the clock does not exist. The clock rate
// may be clamped to the supported range.
// 
// By default when setting arm freq above default, other turbo settings will be
// enabled (e.g. voltage, sdram and gpu frequencies). You can disable this
// effect by setting "skip setting turbo" to 1.
// 
// (swarren: As lp0 mentioned in the issue, min/max rates, and a list of valid
// parents would be useful response data from some message. Also, a set parent
// message might be useful if there are messages to get parenting information)
// 
#define VCMAILBOX_TAG_SET_CLOCK_RATE 0x00038002
#define VCMAILBOX_SET_CLOCK_RATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_clock_rate_request_t), sizeof(vcmailbox_set_clock_rate_response_t))
#define VCMAILBOX_SET_CLOCK_RATE_DEFAULTS (vcmailbox_set_clock_rate_t) { \
    .tag = VCMAILBOX_TAG_SET_CLOCK_RATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_CLOCK_RATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_CLOCK_RATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t rate_in_hz; // rate (in Hz)
    uint32_t skip_setting_turbo; // skip setting turbo
} vcmailbox_set_clock_rate_request_t;

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t rate_in_hz; // rate (in Hz)
} vcmailbox_set_clock_rate_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_clock_rate_request_t request;
        vcmailbox_set_clock_rate_response_t response;
    };
} vcmailbox_set_clock_rate_t;

// Get max clock rate
//
// Return the maximum supported clock rate for the given clock. Clocks should
// not be set higher than this.
// 
#define VCMAILBOX_TAG_GET_MAX_CLOCK_RATE 0x00030004
#define VCMAILBOX_GET_MAX_CLOCK_RATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_max_clock_rate_request_t), sizeof(vcmailbox_get_max_clock_rate_response_t))
#define VCMAILBOX_GET_MAX_CLOCK_RATE_DEFAULTS (vcmailbox_get_max_clock_rate_t) { \
    .tag = VCMAILBOX_TAG_GET_MAX_CLOCK_RATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_MAX_CLOCK_RATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_MAX_CLOCK_RATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
} vcmailbox_get_max_clock_rate_request_t;

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t rate_in_hz; // rate (in Hz)
} vcmailbox_get_max_clock_rate_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_max_clock_rate_request_t request;
        vcmailbox_get_max_clock_rate_response_t response;
    };
} vcmailbox_get_max_clock_rate_t;

// Get min clock rate
//
// Return the minimum supported clock rate for the given clock. This may be used
// when idle.
// 
#define VCMAILBOX_TAG_GET_MIN_CLOCK_RATE 0x00030007
#define VCMAILBOX_GET_MIN_CLOCK_RATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_min_clock_rate_request_t), sizeof(vcmailbox_get_min_clock_rate_response_t))
#define VCMAILBOX_GET_MIN_CLOCK_RATE_DEFAULTS (vcmailbox_get_min_clock_rate_t) { \
    .tag = VCMAILBOX_TAG_GET_MIN_CLOCK_RATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_MIN_CLOCK_RATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_MIN_CLOCK_RATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
} vcmailbox_get_min_clock_rate_request_t;

typedef struct __attribute__((packed)) {
    uint32_t clock_id; // clock id
    uint32_t rate_in_hz; // rate (in Hz)
} vcmailbox_get_min_clock_rate_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_min_clock_rate_request_t request;
        vcmailbox_get_min_clock_rate_response_t response;
    };
} vcmailbox_get_min_clock_rate_t;

// Get turbo
//
// Get the turbo state for index id. id should be 0. level will be zero for non-
// turbo and one for turbo.
// 
#define VCMAILBOX_TAG_GET_TURBO 0x00030009
#define VCMAILBOX_GET_TURBO_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_turbo_request_t), sizeof(vcmailbox_get_turbo_response_t))
#define VCMAILBOX_GET_TURBO_DEFAULTS (vcmailbox_get_turbo_t) { \
    .tag = VCMAILBOX_TAG_GET_TURBO, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_TURBO_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_TURBO_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t id; // id
} vcmailbox_get_turbo_request_t;

typedef struct __attribute__((packed)) {
    uint32_t id; // id
    uint32_t level; // level
} vcmailbox_get_turbo_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_turbo_request_t request;
        vcmailbox_get_turbo_response_t response;
    };
} vcmailbox_get_turbo_t;

// Set turbo
//
// Set the turbo state for index id. id should be zero. level will be zero for
// non-turbo and one for turbo.
// 
// This will cause GPU clocks to be set to maximum when enabled and minimum when
// disabled.
// 
// Unique voltage IDs:
// 
#define VCMAILBOX_TAG_SET_TURBO 0x00038009
#define VCMAILBOX_SET_TURBO_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_turbo_request_t), sizeof(vcmailbox_set_turbo_response_t))
#define VCMAILBOX_SET_TURBO_DEFAULTS (vcmailbox_set_turbo_t) { \
    .tag = VCMAILBOX_TAG_SET_TURBO, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_TURBO_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_TURBO_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t id; // id
    uint32_t level; // level
} vcmailbox_set_turbo_request_t;

typedef struct __attribute__((packed)) {
    uint32_t id; // id
    uint32_t level; // level
} vcmailbox_set_turbo_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_turbo_request_t request;
        vcmailbox_set_turbo_response_t response;
    };
} vcmailbox_set_turbo_t;

// Get voltage
//
// The voltage value may be clamped to the supported range.
// 
// A value of 0x80000000 means the id was not valid.
// 
#define VCMAILBOX_TAG_GET_VOLTAGE 0x00030003
#define VCMAILBOX_GET_VOLTAGE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_voltage_request_t), sizeof(vcmailbox_get_voltage_response_t))
#define VCMAILBOX_GET_VOLTAGE_DEFAULTS (vcmailbox_get_voltage_t) { \
    .tag = VCMAILBOX_TAG_GET_VOLTAGE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_VOLTAGE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_VOLTAGE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t voltage_id; // voltage id
} vcmailbox_get_voltage_request_t;

typedef struct __attribute__((packed)) {
    uint32_t voltage_id; // voltage id
    uint32_t value; // value (offset from 1.2V in units of 0.025V)
} vcmailbox_get_voltage_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_voltage_request_t request;
        vcmailbox_get_voltage_response_t response;
    };
} vcmailbox_get_voltage_t;

// Set voltage
//
// The voltage value may be clamped to the supported range.
// 
// A value of 0x80000000 means the id was not valid.
// 
#define VCMAILBOX_TAG_SET_VOLTAGE 0x00038003
#define VCMAILBOX_SET_VOLTAGE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_voltage_request_t), sizeof(vcmailbox_set_voltage_response_t))
#define VCMAILBOX_SET_VOLTAGE_DEFAULTS (vcmailbox_set_voltage_t) { \
    .tag = VCMAILBOX_TAG_SET_VOLTAGE, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_VOLTAGE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_VOLTAGE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t voltage_id; // voltage id
    uint32_t value; // value (offset from 1.2V in units of 0.025V)
} vcmailbox_set_voltage_request_t;

typedef struct __attribute__((packed)) {
    uint32_t voltage_id; // voltage id
    uint32_t value; // value (offset from 1.2V in units of 0.025V)
} vcmailbox_set_voltage_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_voltage_request_t request;
        vcmailbox_set_voltage_response_t response;
    };
} vcmailbox_set_voltage_t;

// Get max voltage
//
// Return the maximum supported voltage rate for the given id. Voltages should
// not be set higher than this.
// 
#define VCMAILBOX_TAG_GET_MAX_VOLTAGE 0x00030005
#define VCMAILBOX_GET_MAX_VOLTAGE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_max_voltage_request_t), sizeof(vcmailbox_get_max_voltage_response_t))
#define VCMAILBOX_GET_MAX_VOLTAGE_DEFAULTS (vcmailbox_get_max_voltage_t) { \
    .tag = VCMAILBOX_TAG_GET_MAX_VOLTAGE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_MAX_VOLTAGE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_MAX_VOLTAGE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t voltage_id; // voltage id
} vcmailbox_get_max_voltage_request_t;

typedef struct __attribute__((packed)) {
    uint32_t voltage_id; // voltage id
    uint32_t value; // value (offset from 1.2V in units of 0.025V)
} vcmailbox_get_max_voltage_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_max_voltage_request_t request;
        vcmailbox_get_max_voltage_response_t response;
    };
} vcmailbox_get_max_voltage_t;

// Get min voltage
//
// Return the minimum supported voltage rate for the given id. This may be used
// when idle.
// 
#define VCMAILBOX_TAG_GET_MIN_VOLTAGE 0x00030008
#define VCMAILBOX_GET_MIN_VOLTAGE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_min_voltage_request_t), sizeof(vcmailbox_get_min_voltage_response_t))
#define VCMAILBOX_GET_MIN_VOLTAGE_DEFAULTS (vcmailbox_get_min_voltage_t) { \
    .tag = VCMAILBOX_TAG_GET_MIN_VOLTAGE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_MIN_VOLTAGE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_MIN_VOLTAGE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t voltage_id; // voltage id
} vcmailbox_get_min_voltage_request_t;

typedef struct __attribute__((packed)) {
    uint32_t voltage_id; // voltage id
    uint32_t value; // value (offset from 1.2V in units of 0.025V)
} vcmailbox_get_min_voltage_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_min_voltage_request_t request;
        vcmailbox_get_min_voltage_response_t response;
    };
} vcmailbox_get_min_voltage_t;

// Get temperature
//
// Return the temperature of the SoC in thousandths of a degree C. id should be
// zero.
// 
#define VCMAILBOX_TAG_GET_TEMPERATURE 0x00030006
#define VCMAILBOX_GET_TEMPERATURE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_temperature_request_t), sizeof(vcmailbox_get_temperature_response_t))
#define VCMAILBOX_GET_TEMPERATURE_DEFAULTS (vcmailbox_get_temperature_t) { \
    .tag = VCMAILBOX_TAG_GET_TEMPERATURE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_TEMPERATURE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_TEMPERATURE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t temperature_id; // temperature id
} vcmailbox_get_temperature_request_t;

typedef struct __attribute__((packed)) {
    uint32_t temperature_id; // temperature id
    uint32_t value; // value
} vcmailbox_get_temperature_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_temperature_request_t request;
        vcmailbox_get_temperature_response_t response;
    };
} vcmailbox_get_temperature_t;

// Get max temperature
//
// Return the maximum safe temperature of the SoC in thousandths of a degree C.
// id should be zero.
// 
// Overclock may be disabled above this temperature.
// 
#define VCMAILBOX_TAG_GET_MAX_TEMPERATURE 0x0003000a
#define VCMAILBOX_GET_MAX_TEMPERATURE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_max_temperature_request_t), sizeof(vcmailbox_get_max_temperature_response_t))
#define VCMAILBOX_GET_MAX_TEMPERATURE_DEFAULTS (vcmailbox_get_max_temperature_t) { \
    .tag = VCMAILBOX_TAG_GET_MAX_TEMPERATURE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_MAX_TEMPERATURE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_MAX_TEMPERATURE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t temperature_id; // temperature id
} vcmailbox_get_max_temperature_request_t;

typedef struct __attribute__((packed)) {
    uint32_t temperature_id; // temperature id
    uint32_t value; // value
} vcmailbox_get_max_temperature_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_max_temperature_request_t request;
        vcmailbox_get_max_temperature_response_t response;
    };
} vcmailbox_get_max_temperature_t;

// Allocate Memory
//
// Allocates contiguous memory on the GPU. size and alignment are in bytes.
// flags contain:
// 
// ```
// 
// MEM_FLAG_DISCARDABLE = 1 << 0, /* can be resized to 0 at any time. Use for
// cached data */
// 
// MEM_FLAG_NORMAL = 0 << 2, /* normal allocating alias. Don't use from ARM */
// 
// MEM_FLAG_DIRECT = 1 << 2, /* 0xC alias uncached */
// 
// MEM_FLAG_COHERENT = 2 << 2, /* 0x8 alias. Non-allocating in L2 but coherent
// */
// 
// MEM_FLAG_L1_NONALLOCATING = (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT), /*
// Allocating in L2 */
// 
// MEM_FLAG_ZERO = 1 << 4,  /* initialise buffer to all zeros */
// 
// MEM_FLAG_NO_INIT = 1 << 5, /* don't initialise (default is initialise to all
// ones */
// 
// MEM_FLAG_HINT_PERMALOCK = 1 << 6, /* Likely to be locked for long periods of
// time. */
// 
// ```
// 
#define VCMAILBOX_TAG_ALLOCATE_MEMORY 0x0003000c
#define VCMAILBOX_ALLOCATE_MEMORY_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_allocate_memory_request_t), sizeof(vcmailbox_allocate_memory_response_t))
#define VCMAILBOX_ALLOCATE_MEMORY_DEFAULTS (vcmailbox_allocate_memory_t) { \
    .tag = VCMAILBOX_TAG_ALLOCATE_MEMORY, \
    .is_response = false, \
    .value_size = VCMAILBOX_ALLOCATE_MEMORY_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_ALLOCATE_MEMORY_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t size; // size
    uint32_t alignment; // alignment
    uint32_t flags; // flags
} vcmailbox_allocate_memory_request_t;

typedef struct __attribute__((packed)) {
    uint32_t handle; // handle
} vcmailbox_allocate_memory_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_allocate_memory_request_t request;
        vcmailbox_allocate_memory_response_t response;
    };
} vcmailbox_allocate_memory_t;

// Lock memory
//
// Lock buffer in place, and return a bus address. Must be done before memory
// can be accessed
// 
#define VCMAILBOX_TAG_LOCK_MEMORY 0x0003000d
#define VCMAILBOX_LOCK_MEMORY_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_lock_memory_request_t), sizeof(vcmailbox_lock_memory_response_t))
#define VCMAILBOX_LOCK_MEMORY_DEFAULTS (vcmailbox_lock_memory_t) { \
    .tag = VCMAILBOX_TAG_LOCK_MEMORY, \
    .is_response = false, \
    .value_size = VCMAILBOX_LOCK_MEMORY_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_LOCK_MEMORY_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t handle; // handle
} vcmailbox_lock_memory_request_t;

typedef struct __attribute__((packed)) {
    uint32_t bus_address; // bus address
} vcmailbox_lock_memory_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_lock_memory_request_t request;
        vcmailbox_lock_memory_response_t response;
    };
} vcmailbox_lock_memory_t;

// Unlock memory
//
// Unlock buffer. It retains contents, but may move. Needs to be locked before
// next use.
// 
// status=0 is success.
// 
#define VCMAILBOX_TAG_UNLOCK_MEMORY 0x0003000e
#define VCMAILBOX_UNLOCK_MEMORY_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_unlock_memory_request_t), sizeof(vcmailbox_unlock_memory_response_t))
#define VCMAILBOX_UNLOCK_MEMORY_DEFAULTS (vcmailbox_unlock_memory_t) { \
    .tag = VCMAILBOX_TAG_UNLOCK_MEMORY, \
    .is_response = false, \
    .value_size = VCMAILBOX_UNLOCK_MEMORY_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_UNLOCK_MEMORY_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t handle; // handle
} vcmailbox_unlock_memory_request_t;

typedef struct __attribute__((packed)) {
    uint32_t status; // status
} vcmailbox_unlock_memory_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_unlock_memory_request_t request;
        vcmailbox_unlock_memory_response_t response;
    };
} vcmailbox_unlock_memory_t;

// Release Memory
//
// Free the memory buffer. status=0 is success.
// 
#define VCMAILBOX_TAG_RELEASE_MEMORY 0x0003000f
#define VCMAILBOX_RELEASE_MEMORY_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_release_memory_request_t), sizeof(vcmailbox_release_memory_response_t))
#define VCMAILBOX_RELEASE_MEMORY_DEFAULTS (vcmailbox_release_memory_t) { \
    .tag = VCMAILBOX_TAG_RELEASE_MEMORY, \
    .is_response = false, \
    .value_size = VCMAILBOX_RELEASE_MEMORY_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_RELEASE_MEMORY_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t handle; // handle
} vcmailbox_release_memory_request_t;

typedef struct __attribute__((packed)) {
    uint32_t status; // status
} vcmailbox_release_memory_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_release_memory_request_t request;
        vcmailbox_release_memory_response_t response;
    };
} vcmailbox_release_memory_t;

// Execute Code
//
// Calls the function at given (bus) address and with arguments given. E.g.
// 
// r0 = fn(r0, r1, r2, r3, r4, r5);
// 
// It blocks until call completes. The (GPU) instruction cache is implicitly
// flushed.
// 
// Setting the lsb of function pointer address will suppress the instruction
// cache flush if you know the buffer hasn't changed since last execution.
// 
#define VCMAILBOX_TAG_EXECUTE_CODE 0x00030010
#define VCMAILBOX_EXECUTE_CODE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_execute_code_request_t), sizeof(vcmailbox_execute_code_response_t))
#define VCMAILBOX_EXECUTE_CODE_DEFAULTS (vcmailbox_execute_code_t) { \
    .tag = VCMAILBOX_TAG_EXECUTE_CODE, \
    .is_response = false, \
    .value_size = VCMAILBOX_EXECUTE_CODE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_EXECUTE_CODE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t function_pointer; // function pointer
    uint32_t r0; // r0
    uint32_t r1; // r1
    uint32_t r2; // r2
    uint32_t r3; // r3
    uint32_t r4; // r4
    uint32_t r5; // r5
} vcmailbox_execute_code_request_t;

typedef struct __attribute__((packed)) {
    uint32_t r0; // r0
} vcmailbox_execute_code_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_execute_code_request_t request;
        vcmailbox_execute_code_response_t response;
    };
} vcmailbox_execute_code_t;

// Get Dispmanx Resource mem handle
//
// Gets the mem_handle associated with a created dispmanx resource.
// 
// This can be locked and the memory directly written from the arm to avoid
// having to copy the image data to GPU.
// 
#define VCMAILBOX_TAG_GET_DISPMANX_RESOURCE_MEM_HANDLE 0x00030014
#define VCMAILBOX_GET_DISPMANX_RESOURCE_MEM_HANDLE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_dispmanx_resource_mem_handle_request_t), sizeof(vcmailbox_get_dispmanx_resource_mem_handle_response_t))
#define VCMAILBOX_GET_DISPMANX_RESOURCE_MEM_HANDLE_DEFAULTS (vcmailbox_get_dispmanx_resource_mem_handle_t) { \
    .tag = VCMAILBOX_TAG_GET_DISPMANX_RESOURCE_MEM_HANDLE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_DISPMANX_RESOURCE_MEM_HANDLE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_DISPMANX_RESOURCE_MEM_HANDLE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t dispmanx_resource_handle; // dispmanx resource handle
} vcmailbox_get_dispmanx_resource_mem_handle_request_t;

typedef struct __attribute__((packed)) {
    uint32_t error_code; // 0 is successful
    uint32_t mem_handle_for_resource; // mem handle for resource
} vcmailbox_get_dispmanx_resource_mem_handle_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_dispmanx_resource_mem_handle_request_t request;
        vcmailbox_get_dispmanx_resource_mem_handle_response_t response;
    };
} vcmailbox_get_dispmanx_resource_mem_handle_t;

// Get EDID block
//
// This reads the specified EDID block from attached HDMI/DVI device.
// 
// There will always be at least one block of 128 bytes, but there may be
// additional blocks. You should keep requesting blocks (starting from 0) until
// the status returned is non-zero.
// 
// unless it can be accommodated without changing the buffer base or size.
// 
// For example:
// 
//  1. The current values/defaults are loaded into a temporary struct
// 
//  2. The tags are used to overwrite some or all of the values
// 
//  3. Validation of Test/Set tags occurs
// 
//  4. The Set changes are applied and responses based on the requested
// Get/Test/Set tags are written to the buffer
// 
// Duplicating the same tag in one request/response is prohibited. The expected
// result is either an error or implementation specified undefined behaviour
// (such as only using the last instance of the tag).
// 
#define VCMAILBOX_TAG_GET_EDID_BLOCK 0x00030020
#define VCMAILBOX_GET_EDID_BLOCK_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_get_edid_block_request_t), sizeof(vcmailbox_get_edid_block_response_t))
#define VCMAILBOX_GET_EDID_BLOCK_DEFAULTS (vcmailbox_get_edid_block_t) { \
    .tag = VCMAILBOX_TAG_GET_EDID_BLOCK, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_EDID_BLOCK_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_EDID_BLOCK_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t block_number; // block number
} vcmailbox_get_edid_block_request_t;

typedef struct __attribute__((packed)) {
    uint32_t block_number; // block number
    uint32_t status; // status
    uint8_t edid_block[128]; // bytes: EDID block
} vcmailbox_get_edid_block_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_get_edid_block_request_t request;
        vcmailbox_get_edid_block_response_t response;
    };
} vcmailbox_get_edid_block_t;

// Allocate buffer
//
// If the requested alignment is unsupported then the current base and size
// (which may be 0 if not allocated) is returned and no change occurs.
// 
#define VCMAILBOX_TAG_ALLOCATE_BUFFER 0x00040001
#define VCMAILBOX_ALLOCATE_BUFFER_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_allocate_buffer_request_t), sizeof(vcmailbox_allocate_buffer_response_t))
#define VCMAILBOX_ALLOCATE_BUFFER_DEFAULTS (vcmailbox_allocate_buffer_t) { \
    .tag = VCMAILBOX_TAG_ALLOCATE_BUFFER, \
    .is_response = false, \
    .value_size = VCMAILBOX_ALLOCATE_BUFFER_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_ALLOCATE_BUFFER_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t alignment; // alignment in bytes
} vcmailbox_allocate_buffer_request_t;

typedef struct __attribute__((packed)) {
    uint32_t frame_buffer_base_address; // frame buffer base address in bytes
    uint32_t frame_buffer_size; // frame buffer size in bytes
} vcmailbox_allocate_buffer_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_allocate_buffer_request_t request;
        vcmailbox_allocate_buffer_response_t response;
    };
} vcmailbox_allocate_buffer_t;

// Release buffer
//
// Releases and disables the frame buffer.
// 
#define VCMAILBOX_TAG_RELEASE_BUFFER 0x00048001
#define VCMAILBOX_RELEASE_BUFFER_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_release_buffer_response_t))
#define VCMAILBOX_RELEASE_BUFFER_DEFAULTS (vcmailbox_release_buffer_t) { \
    .tag = VCMAILBOX_TAG_RELEASE_BUFFER, \
    .is_response = false, \
    .value_size = VCMAILBOX_RELEASE_BUFFER_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_RELEASE_BUFFER_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
} vcmailbox_release_buffer_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_release_buffer_response_t response;
} vcmailbox_release_buffer_t;

// Blank screen
//
//     * Bit 0: 0=off, 1=on
// 
//     * Bits 1-31: reserved for future use (set to 0)
// 
//     * Bit 0: 0=off, 1=on
// 
//     * Bits 1-31: reserved for future use
// 
#define VCMAILBOX_TAG_BLANK_SCREEN 0x00040002
#define VCMAILBOX_BLANK_SCREEN_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_blank_screen_request_t), sizeof(vcmailbox_blank_screen_response_t))
#define VCMAILBOX_BLANK_SCREEN_DEFAULTS (vcmailbox_blank_screen_t) { \
    .tag = VCMAILBOX_TAG_BLANK_SCREEN, \
    .is_response = false, \
    .value_size = VCMAILBOX_BLANK_SCREEN_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_BLANK_SCREEN_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t state; // state
} vcmailbox_blank_screen_request_t;

typedef struct __attribute__((packed)) {
    uint32_t state; // state
} vcmailbox_blank_screen_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_blank_screen_request_t request;
        vcmailbox_blank_screen_response_t response;
    };
} vcmailbox_blank_screen_t;

// Get physical (display) width/height
//
// Note that the "physical (display)" size is the size of the allocated buffer
// in memory, not the resolution of the video signal sent to the display device.
// 
#define VCMAILBOX_TAG_GET_PHYSICAL_DISPLAY_WIDTH_HEIGHT 0x00040003
#define VCMAILBOX_GET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_physical_display_width_height_response_t))
#define VCMAILBOX_GET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_DEFAULTS (vcmailbox_get_physical_display_width_height_t) { \
    .tag = VCMAILBOX_TAG_GET_PHYSICAL_DISPLAY_WIDTH_HEIGHT, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_get_physical_display_width_height_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_physical_display_width_height_response_t response;
} vcmailbox_get_physical_display_width_height_t;

// Test physical (display) width/height
//
// Response is the same as the request (or modified), to indicate if this
// configuration is supported (in combination with all the other settings). Does
// not modify the current hardware or frame buffer state.
// 
#define VCMAILBOX_TAG_TEST_PHYSICAL_DISPLAY_WIDTH_HEIGHT 0x00044003
#define VCMAILBOX_TEST_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_test_physical_display_width_height_request_t), sizeof(vcmailbox_test_physical_display_width_height_response_t))
#define VCMAILBOX_TEST_PHYSICAL_DISPLAY_WIDTH_HEIGHT_DEFAULTS (vcmailbox_test_physical_display_width_height_t) { \
    .tag = VCMAILBOX_TAG_TEST_PHYSICAL_DISPLAY_WIDTH_HEIGHT, \
    .is_response = false, \
    .value_size = VCMAILBOX_TEST_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_TEST_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_test_physical_display_width_height_request_t;

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_test_physical_display_width_height_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_test_physical_display_width_height_request_t request;
        vcmailbox_test_physical_display_width_height_response_t response;
    };
} vcmailbox_test_physical_display_width_height_t;

// Set physical (display) width/height
//
// The response may not be the same as the request so it must be checked. May be
// the previous width/height or 0 for unsupported.
// 
#define VCMAILBOX_TAG_SET_PHYSICAL_DISPLAY_WIDTH_HEIGHT 0x00048003
#define VCMAILBOX_SET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_physical_display_width_height_request_t), sizeof(vcmailbox_set_physical_display_width_height_response_t))
#define VCMAILBOX_SET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_DEFAULTS (vcmailbox_set_physical_display_width_height_t) { \
    .tag = VCMAILBOX_TAG_SET_PHYSICAL_DISPLAY_WIDTH_HEIGHT, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_set_physical_display_width_height_request_t;

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_set_physical_display_width_height_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_physical_display_width_height_request_t request;
        vcmailbox_set_physical_display_width_height_response_t response;
    };
} vcmailbox_set_physical_display_width_height_t;

// Get virtual (buffer) width/height
//
// Note that the "virtual (buffer)" size is the portion of buffer that is sent
// to the display device, not the resolution the buffer itself. This may be
// smaller than the allocated buffer size in order to implement panning.
// 
#define VCMAILBOX_TAG_GET_VIRTUAL_BUFFER_WIDTH_HEIGHT 0x00040004
#define VCMAILBOX_GET_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_virtual_buffer_width_height_response_t))
#define VCMAILBOX_GET_VIRTUAL_BUFFER_WIDTH_HEIGHT_DEFAULTS (vcmailbox_get_virtual_buffer_width_height_t) { \
    .tag = VCMAILBOX_TAG_GET_VIRTUAL_BUFFER_WIDTH_HEIGHT, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_get_virtual_buffer_width_height_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_virtual_buffer_width_height_response_t response;
} vcmailbox_get_virtual_buffer_width_height_t;

// Test virtual (buffer) width/height
//
// Response is the same as the request (or modified), to indicate if this
// configuration is supported (in combination with all the other settings). Does
// not modify the current hardware or frame buffer state.
// 
#define VCMAILBOX_TAG_TEST_VIRTUAL_BUFFER_WIDTH_HEIGHT 0x00044004
#define VCMAILBOX_TEST_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_test_virtual_buffer_width_height_request_t), sizeof(vcmailbox_test_virtual_buffer_width_height_response_t))
#define VCMAILBOX_TEST_VIRTUAL_BUFFER_WIDTH_HEIGHT_DEFAULTS (vcmailbox_test_virtual_buffer_width_height_t) { \
    .tag = VCMAILBOX_TAG_TEST_VIRTUAL_BUFFER_WIDTH_HEIGHT, \
    .is_response = false, \
    .value_size = VCMAILBOX_TEST_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_TEST_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_test_virtual_buffer_width_height_request_t;

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_test_virtual_buffer_width_height_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_test_virtual_buffer_width_height_request_t request;
        vcmailbox_test_virtual_buffer_width_height_response_t response;
    };
} vcmailbox_test_virtual_buffer_width_height_t;

// Set virtual (buffer) width/height
//
// The response may not be the same as the request so it must be checked. May be
// the previous width/height or 0 for unsupported.
// 
#define VCMAILBOX_TAG_SET_VIRTUAL_BUFFER_WIDTH_HEIGHT 0x00048004
#define VCMAILBOX_SET_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_virtual_buffer_width_height_request_t), sizeof(vcmailbox_set_virtual_buffer_width_height_response_t))
#define VCMAILBOX_SET_VIRTUAL_BUFFER_WIDTH_HEIGHT_DEFAULTS (vcmailbox_set_virtual_buffer_width_height_t) { \
    .tag = VCMAILBOX_TAG_SET_VIRTUAL_BUFFER_WIDTH_HEIGHT, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_VIRTUAL_BUFFER_WIDTH_HEIGHT_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_set_virtual_buffer_width_height_request_t;

typedef struct __attribute__((packed)) {
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
} vcmailbox_set_virtual_buffer_width_height_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_virtual_buffer_width_height_request_t request;
        vcmailbox_set_virtual_buffer_width_height_response_t response;
    };
} vcmailbox_set_virtual_buffer_width_height_t;

// Get depth
//
#define VCMAILBOX_TAG_GET_DEPTH 0x00040005
#define VCMAILBOX_GET_DEPTH_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_depth_response_t))
#define VCMAILBOX_GET_DEPTH_DEFAULTS (vcmailbox_get_depth_t) { \
    .tag = VCMAILBOX_TAG_GET_DEPTH, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_DEPTH_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_DEPTH_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t bits_per_pixel; // bits per pixel
} vcmailbox_get_depth_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_depth_response_t response;
} vcmailbox_get_depth_t;

// Test depth
//
// Response is the same as the request (or modified), to indicate if this
// configuration is supported (in combination with all the other settings). Does
// not modify the current hardware or frame buffer state.
// 
#define VCMAILBOX_TAG_TEST_DEPTH 0x00044005
#define VCMAILBOX_TEST_DEPTH_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_test_depth_request_t), sizeof(vcmailbox_test_depth_response_t))
#define VCMAILBOX_TEST_DEPTH_DEFAULTS (vcmailbox_test_depth_t) { \
    .tag = VCMAILBOX_TAG_TEST_DEPTH, \
    .is_response = false, \
    .value_size = VCMAILBOX_TEST_DEPTH_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_TEST_DEPTH_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t bits_per_pixel; // bits per pixel
} vcmailbox_test_depth_request_t;

typedef struct __attribute__((packed)) {
    uint32_t bits_per_pixel; // bits per pixel
} vcmailbox_test_depth_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_test_depth_request_t request;
        vcmailbox_test_depth_response_t response;
    };
} vcmailbox_test_depth_t;

// Set depth
//
// The response may not be the same as the request so it must be checked. May be
// the previous depth or 0 for unsupported.
// 
#define VCMAILBOX_TAG_SET_DEPTH 0x00048005
#define VCMAILBOX_SET_DEPTH_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_depth_request_t), sizeof(vcmailbox_set_depth_response_t))
#define VCMAILBOX_SET_DEPTH_DEFAULTS (vcmailbox_set_depth_t) { \
    .tag = VCMAILBOX_TAG_SET_DEPTH, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_DEPTH_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_DEPTH_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t bits_per_pixel; // bits per pixel
} vcmailbox_set_depth_request_t;

typedef struct __attribute__((packed)) {
    uint32_t bits_per_pixel; // bits per pixel
} vcmailbox_set_depth_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_depth_request_t request;
        vcmailbox_set_depth_response_t response;
    };
} vcmailbox_set_depth_t;

// Get pixel order
//
#define VCMAILBOX_TAG_GET_PIXEL_ORDER 0x00040006
#define VCMAILBOX_GET_PIXEL_ORDER_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_pixel_order_response_t))
#define VCMAILBOX_GET_PIXEL_ORDER_DEFAULTS (vcmailbox_get_pixel_order_t) { \
    .tag = VCMAILBOX_TAG_GET_PIXEL_ORDER, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_PIXEL_ORDER_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_PIXEL_ORDER_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t state; // state
} vcmailbox_get_pixel_order_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_pixel_order_response_t response;
} vcmailbox_get_pixel_order_t;

// Test pixel order
//
// Response is the same as the request (or modified), to indicate if this
// configuration is supported (in combination with all the other settings). Does
// not modify the current hardware or frame buffer state.
// 
#define VCMAILBOX_TAG_TEST_PIXEL_ORDER 0x00044006
#define VCMAILBOX_TEST_PIXEL_ORDER_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_test_pixel_order_request_t), sizeof(vcmailbox_test_pixel_order_response_t))
#define VCMAILBOX_TEST_PIXEL_ORDER_DEFAULTS (vcmailbox_test_pixel_order_t) { \
    .tag = VCMAILBOX_TAG_TEST_PIXEL_ORDER, \
    .is_response = false, \
    .value_size = VCMAILBOX_TEST_PIXEL_ORDER_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_TEST_PIXEL_ORDER_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t state; // state (as above)
} vcmailbox_test_pixel_order_request_t;

typedef struct __attribute__((packed)) {
    uint32_t state; // state (as above)
} vcmailbox_test_pixel_order_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_test_pixel_order_request_t request;
        vcmailbox_test_pixel_order_response_t response;
    };
} vcmailbox_test_pixel_order_t;

// Set pixel order
//
// The response may not be the same as the request so it must be checked.
// 
#define VCMAILBOX_TAG_SET_PIXEL_ORDER 0x00048006
#define VCMAILBOX_SET_PIXEL_ORDER_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_pixel_order_request_t), sizeof(vcmailbox_set_pixel_order_response_t))
#define VCMAILBOX_SET_PIXEL_ORDER_DEFAULTS (vcmailbox_set_pixel_order_t) { \
    .tag = VCMAILBOX_TAG_SET_PIXEL_ORDER, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_PIXEL_ORDER_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_PIXEL_ORDER_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t state; // state (as above)
} vcmailbox_set_pixel_order_request_t;

typedef struct __attribute__((packed)) {
    uint32_t state; // state (as above)
} vcmailbox_set_pixel_order_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_pixel_order_request_t request;
        vcmailbox_set_pixel_order_response_t response;
    };
} vcmailbox_set_pixel_order_t;

// Get alpha mode
//
//        * 0x0: Alpha channel enabled (0 = fully opaque)
// 
//        * 0x1: Alpha channel reversed (0 = fully transparent)
// 
//        * 0x2: Alpha channel ignored
// 
#define VCMAILBOX_TAG_GET_ALPHA_MODE 0x00040007
#define VCMAILBOX_GET_ALPHA_MODE_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_alpha_mode_response_t))
#define VCMAILBOX_GET_ALPHA_MODE_DEFAULTS (vcmailbox_get_alpha_mode_t) { \
    .tag = VCMAILBOX_TAG_GET_ALPHA_MODE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_ALPHA_MODE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_ALPHA_MODE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t state; // state
} vcmailbox_get_alpha_mode_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_alpha_mode_response_t response;
} vcmailbox_get_alpha_mode_t;

// Test alpha mode
//
// Response is the same as the request (or modified), to indicate if this
// configuration is supported (in combination with all the other settings). Does
// not modify the current hardware or frame buffer state.
// 
#define VCMAILBOX_TAG_TEST_ALPHA_MODE 0x00044007
#define VCMAILBOX_TEST_ALPHA_MODE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_test_alpha_mode_request_t), sizeof(vcmailbox_test_alpha_mode_response_t))
#define VCMAILBOX_TEST_ALPHA_MODE_DEFAULTS (vcmailbox_test_alpha_mode_t) { \
    .tag = VCMAILBOX_TAG_TEST_ALPHA_MODE, \
    .is_response = false, \
    .value_size = VCMAILBOX_TEST_ALPHA_MODE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_TEST_ALPHA_MODE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t state; // state (as above)
} vcmailbox_test_alpha_mode_request_t;

typedef struct __attribute__((packed)) {
    uint32_t state; // state (as above)
} vcmailbox_test_alpha_mode_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_test_alpha_mode_request_t request;
        vcmailbox_test_alpha_mode_response_t response;
    };
} vcmailbox_test_alpha_mode_t;

// Set alpha mode
//
// The response may not be the same as the request so it must be checked.
// 
#define VCMAILBOX_TAG_SET_ALPHA_MODE 0x00048007
#define VCMAILBOX_SET_ALPHA_MODE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_alpha_mode_request_t), sizeof(vcmailbox_set_alpha_mode_response_t))
#define VCMAILBOX_SET_ALPHA_MODE_DEFAULTS (vcmailbox_set_alpha_mode_t) { \
    .tag = VCMAILBOX_TAG_SET_ALPHA_MODE, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_ALPHA_MODE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_ALPHA_MODE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t state; // state (as above)
} vcmailbox_set_alpha_mode_request_t;

typedef struct __attribute__((packed)) {
    uint32_t state; // state (as above)
} vcmailbox_set_alpha_mode_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_alpha_mode_request_t request;
        vcmailbox_set_alpha_mode_response_t response;
    };
} vcmailbox_set_alpha_mode_t;

// Get pitch
//
#define VCMAILBOX_TAG_GET_PITCH 0x00040008
#define VCMAILBOX_GET_PITCH_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_pitch_response_t))
#define VCMAILBOX_GET_PITCH_DEFAULTS (vcmailbox_get_pitch_t) { \
    .tag = VCMAILBOX_TAG_GET_PITCH, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_PITCH_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_PITCH_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t bytes_per_line; // bytes per line
} vcmailbox_get_pitch_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_pitch_response_t response;
} vcmailbox_get_pitch_t;

// Get virtual offset
//
#define VCMAILBOX_TAG_GET_VIRTUAL_OFFSET 0x00040009
#define VCMAILBOX_GET_VIRTUAL_OFFSET_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_virtual_offset_response_t))
#define VCMAILBOX_GET_VIRTUAL_OFFSET_DEFAULTS (vcmailbox_get_virtual_offset_t) { \
    .tag = VCMAILBOX_TAG_GET_VIRTUAL_OFFSET, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_VIRTUAL_OFFSET_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_VIRTUAL_OFFSET_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t x; // X in pixels
    uint32_t y; // Y in pixels
} vcmailbox_get_virtual_offset_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_virtual_offset_response_t response;
} vcmailbox_get_virtual_offset_t;

// Test virtual offset
//
// Response is the same as the request (or modified), to indicate if this
// configuration is supported (in combination with all the other settings). Does
// not modify the current hardware or frame buffer state.
// 
#define VCMAILBOX_TAG_TEST_VIRTUAL_OFFSET 0x00044009
#define VCMAILBOX_TEST_VIRTUAL_OFFSET_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_test_virtual_offset_request_t), sizeof(vcmailbox_test_virtual_offset_response_t))
#define VCMAILBOX_TEST_VIRTUAL_OFFSET_DEFAULTS (vcmailbox_test_virtual_offset_t) { \
    .tag = VCMAILBOX_TAG_TEST_VIRTUAL_OFFSET, \
    .is_response = false, \
    .value_size = VCMAILBOX_TEST_VIRTUAL_OFFSET_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_TEST_VIRTUAL_OFFSET_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t x; // X in pixels
    uint32_t y; // Y in pixels
} vcmailbox_test_virtual_offset_request_t;

typedef struct __attribute__((packed)) {
    uint32_t x; // X in pixels
    uint32_t y; // Y in pixels
} vcmailbox_test_virtual_offset_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_test_virtual_offset_request_t request;
        vcmailbox_test_virtual_offset_response_t response;
    };
} vcmailbox_test_virtual_offset_t;

// Set virtual offset
//
// The response may not be the same as the request so it must be checked. May be
// the previous offset or 0 for unsupported.
// 
#define VCMAILBOX_TAG_SET_VIRTUAL_OFFSET 0x00048009
#define VCMAILBOX_SET_VIRTUAL_OFFSET_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_virtual_offset_request_t), sizeof(vcmailbox_set_virtual_offset_response_t))
#define VCMAILBOX_SET_VIRTUAL_OFFSET_DEFAULTS (vcmailbox_set_virtual_offset_t) { \
    .tag = VCMAILBOX_TAG_SET_VIRTUAL_OFFSET, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_VIRTUAL_OFFSET_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_VIRTUAL_OFFSET_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t x; // X in pixels
    uint32_t y; // Y in pixels
} vcmailbox_set_virtual_offset_request_t;

typedef struct __attribute__((packed)) {
    uint32_t x; // X in pixels
    uint32_t y; // Y in pixels
} vcmailbox_set_virtual_offset_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_virtual_offset_request_t request;
        vcmailbox_set_virtual_offset_response_t response;
    };
} vcmailbox_set_virtual_offset_t;

// Get overscan
//
#define VCMAILBOX_TAG_GET_OVERSCAN 0x0004000a
#define VCMAILBOX_GET_OVERSCAN_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_overscan_response_t))
#define VCMAILBOX_GET_OVERSCAN_DEFAULTS (vcmailbox_get_overscan_t) { \
    .tag = VCMAILBOX_TAG_GET_OVERSCAN, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_OVERSCAN_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_OVERSCAN_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t top; // top in pixels
    uint32_t bottom; // bottom in pixels
    uint32_t left; // left in pixels
    uint32_t right; // right in pixels
} vcmailbox_get_overscan_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_overscan_response_t response;
} vcmailbox_get_overscan_t;

// Test overscan
//
// Response is the same as the request (or modified), to indicate if this
// configuration is supported (in combination with all the other settings). Does
// not modify the current hardware or frame buffer state.
// 
#define VCMAILBOX_TAG_TEST_OVERSCAN 0x0004400a
#define VCMAILBOX_TEST_OVERSCAN_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_test_overscan_request_t), sizeof(vcmailbox_test_overscan_response_t))
#define VCMAILBOX_TEST_OVERSCAN_DEFAULTS (vcmailbox_test_overscan_t) { \
    .tag = VCMAILBOX_TAG_TEST_OVERSCAN, \
    .is_response = false, \
    .value_size = VCMAILBOX_TEST_OVERSCAN_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_TEST_OVERSCAN_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t top; // top in pixels
    uint32_t bottom; // bottom in pixels
    uint32_t left; // left in pixels
    uint32_t right; // right in pixels
} vcmailbox_test_overscan_request_t;

typedef struct __attribute__((packed)) {
    uint32_t top; // top in pixels
    uint32_t bottom; // bottom in pixels
    uint32_t left; // left in pixels
    uint32_t right; // right in pixels
} vcmailbox_test_overscan_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_test_overscan_request_t request;
        vcmailbox_test_overscan_response_t response;
    };
} vcmailbox_test_overscan_t;

// Set overscan
//
// The response may not be the same as the request so it must be checked. May be
// the previous overscan or 0 for unsupported.
// 
#define VCMAILBOX_TAG_SET_OVERSCAN 0x0004800a
#define VCMAILBOX_SET_OVERSCAN_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_overscan_request_t), sizeof(vcmailbox_set_overscan_response_t))
#define VCMAILBOX_SET_OVERSCAN_DEFAULTS (vcmailbox_set_overscan_t) { \
    .tag = VCMAILBOX_TAG_SET_OVERSCAN, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_OVERSCAN_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_OVERSCAN_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t top; // top in pixels
    uint32_t bottom; // bottom in pixels
    uint32_t left; // left in pixels
    uint32_t right; // right in pixels
} vcmailbox_set_overscan_request_t;

typedef struct __attribute__((packed)) {
    uint32_t top; // top in pixels
    uint32_t bottom; // bottom in pixels
    uint32_t left; // left in pixels
    uint32_t right; // right in pixels
} vcmailbox_set_overscan_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_overscan_request_t request;
        vcmailbox_set_overscan_response_t response;
    };
} vcmailbox_set_overscan_t;

// Get palette
//
#define VCMAILBOX_TAG_GET_PALETTE 0x0004000b
#define VCMAILBOX_GET_PALETTE_VALUE_SIZE _VC_MAX(0, sizeof(vcmailbox_get_palette_response_t))
#define VCMAILBOX_GET_PALETTE_DEFAULTS (vcmailbox_get_palette_t) { \
    .tag = VCMAILBOX_TAG_GET_PALETTE, \
    .is_response = false, \
    .value_size = VCMAILBOX_GET_PALETTE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_GET_PALETTE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t rgba_palette_values[256]; // RGBA palette values (index 0 to 255)
} vcmailbox_get_palette_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    vcmailbox_get_palette_response_t response;
} vcmailbox_get_palette_t;

// Test palette
//
// Response is the same as the request (or modified), to indicate if this
// configuration is supported (in combination with all the other settings). Does
// not modify the current hardware or frame buffer state.
// 
#define VCMAILBOX_TAG_TEST_PALETTE 0x0004400b
#define VCMAILBOX_TEST_PALETTE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_test_palette_request_t), sizeof(vcmailbox_test_palette_response_t))
#define VCMAILBOX_TEST_PALETTE_DEFAULTS (vcmailbox_test_palette_t) { \
    .tag = VCMAILBOX_TAG_TEST_PALETTE, \
    .is_response = false, \
    .value_size = VCMAILBOX_TEST_PALETTE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_TEST_PALETTE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t offset; // offset: first palette index to set (0-255)
    uint32_t length; // length: number of palette entries to set (1-256)
    uint32_t rgba_palette_values[256]; // RGBA palette values (offset to offset+length-1)
} vcmailbox_test_palette_request_t;

typedef struct __attribute__((packed)) {
    uint32_t valid; // 0=valid, 1=invalid
} vcmailbox_test_palette_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_test_palette_request_t request;
        vcmailbox_test_palette_response_t response;
    };
} vcmailbox_test_palette_t;

// Set palette
//
// The response may not be the same as the request so it must be checked.
// Palette changes should not be partially applied.
// 
#define VCMAILBOX_TAG_SET_PALETTE 0x0004800b
#define VCMAILBOX_SET_PALETTE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_palette_request_t), sizeof(vcmailbox_set_palette_response_t))
#define VCMAILBOX_SET_PALETTE_DEFAULTS (vcmailbox_set_palette_t) { \
    .tag = VCMAILBOX_TAG_SET_PALETTE, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_PALETTE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_PALETTE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t offset; // offset: first palette index to set (0-255)
    uint32_t length; // length: number of palette entries to set (1-256)
    uint32_t rgba_palette_values[256]; // RGBA palette values (offset to offset+length-1)
} vcmailbox_set_palette_request_t;

typedef struct __attribute__((packed)) {
    uint32_t valid; // 0=valid, 1=invalid
} vcmailbox_set_palette_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_palette_request_t request;
        vcmailbox_set_palette_response_t response;
    };
} vcmailbox_set_palette_t;

// Set Cursor Info
//
// Format is 32bpp (ARGB).
// 
// Width and height should be >= 16 and (width * height) <= 64.
// 
#define VCMAILBOX_TAG_SET_CURSOR_INFO 0x00008010
#define VCMAILBOX_SET_CURSOR_INFO_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_cursor_info_request_t), sizeof(vcmailbox_set_cursor_info_response_t))
#define VCMAILBOX_SET_CURSOR_INFO_DEFAULTS (vcmailbox_set_cursor_info_t) { \
    .tag = VCMAILBOX_TAG_SET_CURSOR_INFO, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_CURSOR_INFO_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_CURSOR_INFO_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t width; // width
    uint32_t height; // height
    uint32_t unused; // (unused)
    uint32_t pointer_to_pixels; // pointer to pixels
    uint32_t hotspotx; // hotspotX
    uint32_t hotspoty; // hotspotY
} vcmailbox_set_cursor_info_request_t;

typedef struct __attribute__((packed)) {
    uint32_t valid; // 0=valid, 1=invalid
} vcmailbox_set_cursor_info_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_cursor_info_request_t request;
        vcmailbox_set_cursor_info_response_t response;
    };
} vcmailbox_set_cursor_info_t;

// Set Cursor State
//
// The flags control:
// 
// bit0: clean=display coords, set=framebuffer coords
// 
// if Set Cursor Info hasn't been called a default cursor will be used (64x64
// with hotspot at 0,0).
// 
// (swarren: I agree that a single message for each of framebuffer get/test/set
// might be better - that way, the complete configuration is always available
// for test or set to operate on, although for test, the individual test
// messages might be useful alongside the overall configuration test message.
// What about multi-head; can both HDMI and TV out be active at once, or is
// there only 1 head? Are options like HDMI YUV vs. RGB, HDMI audio, <del>HDMI
// EDID retrieval,</del> HDMI reduced CE vs. PC RGB space, ... supported?)
// 
// (lp0: having a single message does not imply that the complete configuration
// is always available - the caller may be using an older version that lacks
// additional information, and all these extra options would be more easily
// supported as separate values than adding to a single large struct)
// 
#define VCMAILBOX_TAG_SET_CURSOR_STATE 0x00008011
#define VCMAILBOX_SET_CURSOR_STATE_VALUE_SIZE _VC_MAX(sizeof(vcmailbox_set_cursor_state_request_t), sizeof(vcmailbox_set_cursor_state_response_t))
#define VCMAILBOX_SET_CURSOR_STATE_DEFAULTS (vcmailbox_set_cursor_state_t) { \
    .tag = VCMAILBOX_TAG_SET_CURSOR_STATE, \
    .is_response = false, \
    .value_size = VCMAILBOX_SET_CURSOR_STATE_VALUE_SIZE, \
    .response_value_size = VCMAILBOX_SET_CURSOR_STATE_VALUE_SIZE \
}

typedef struct __attribute__((packed)) {
    uint32_t enable; // enable (1=visible, 0=invisible)
    uint32_t x; // x
    uint32_t y; // y
    uint32_t flags; // flags
} vcmailbox_set_cursor_state_request_t;

typedef struct __attribute__((packed)) {
    uint32_t valid; // 0=valid, 1=invalid
} vcmailbox_set_cursor_state_response_t;

typedef struct __attribute__((packed)) {
    uint32_t tag;
    uint32_t value_size; // in bytes
    uint32_t response_value_size : 31;
    bool is_response : 1;
    union {
        vcmailbox_set_cursor_state_request_t request;
        vcmailbox_set_cursor_state_response_t response;
    };
} vcmailbox_set_cursor_state_t;

