// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

// Board-specific definitions
#include "mpconfigboard.h"

#include <stdint.h>
#include <alloca.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"

#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL            (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#endif

// object representation and NLR handling
#define MICROPY_OBJ_REPR                    (MICROPY_OBJ_REPR_A)
#define MICROPY_NLR_SETJMP                  (1)
#if CONFIG_IDF_TARGET_ESP32C3
#define MICROPY_GCREGS_SETJMP               (1)
#endif

// memory allocation policies
#define MICROPY_ALLOC_PATH_MAX              (128)

// emitters
#define MICROPY_PERSISTENT_CODE_LOAD        (1)
#if !CONFIG_IDF_TARGET_ESP32C3
#define MICROPY_EMIT_XTENSAWIN              (1)
#endif

// workaround for xtensa-esp32-elf-gcc esp-2020r3, which can generate wrong code for loops
// see https://github.com/espressif/esp-idf/issues/9130
// this was fixed in newer versions of the compiler by:
//   "gas: use literals/const16 for xtensa loop relaxation"
//   https://github.com/jcmvbkbc/binutils-gdb-xtensa/commit/403b0b61f6d4358aee8493cb1d11814e368942c9
#define MICROPY_COMP_CONST_FOLDING_COMPILER_WORKAROUND (1)

// optimisations
#define MICROPY_OPT_COMPUTED_GOTO           (1)

// Python internal features
#define MICROPY_READER_VFS                  (1)
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_NORMAL)
#define MICROPY_WARNINGS                    (1)
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_STREAMS_POSIX_API           (1)
#define MICROPY_MODULE_FROZEN_STR           (0)
#define MICROPY_MODULE_FROZEN_MPY           (1)
#define MICROPY_QSTR_EXTRA_POOL             mp_qstr_frozen_const_pool
#define MICROPY_USE_INTERNAL_ERRNO          (0) // errno.h from xtensa-esp32-elf/sys-include/sys
#define MICROPY_USE_INTERNAL_PRINTF         (0) // ESP32 SDK requires its own printf
#define MICROPY_SCHEDULER_DEPTH             (8)
#define MICROPY_VFS                         (1)

// control over Python builtins
#define MICROPY_PY_STR_BYTES_CMP_WARN       (1)
#define MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       esp32_help_text
#define MICROPY_PY_IO_BUFFEREDWRITER        (1)
#define MICROPY_PY_UTIME_MP_HAL             (1)
#define MICROPY_PY_THREAD                   (1)
#define MICROPY_PY_THREAD_GIL               (1)
#define MICROPY_PY_THREAD_GIL_VM_DIVISOR    (32)

// extended modules
#ifndef MICROPY_PY_BLUETOOTH
#define MICROPY_PY_BLUETOOTH                (1)
#define MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS (1)
#define MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS_WITH_INTERLOCK (1)
#define MICROPY_PY_BLUETOOTH_SYNC_EVENT_STACK_SIZE (CONFIG_BT_NIMBLE_TASK_STACK_SIZE)
#define MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE (1)
#define MICROPY_PY_BLUETOOTH_ENABLE_PAIRING_BONDING (1)
#define MICROPY_BLUETOOTH_NIMBLE            (1)
#define MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY (1)
#endif
#define MICROPY_PY_UTIMEQ                   (1)
#define MICROPY_PY_UHASHLIB_SHA1            (1)
#define MICROPY_PY_UHASHLIB_SHA256          (1)
#define MICROPY_PY_UCRYPTOLIB               (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC   (esp_random())
#define MICROPY_PY_UOS_INCLUDEFILE          "ports/esp32/moduos.c"
#define MICROPY_PY_OS_DUPTERM               (1)
#define MICROPY_PY_UOS_DUPTERM_NOTIFY       (1)
#define MICROPY_PY_UOS_UNAME                (1)
#define MICROPY_PY_UOS_URANDOM              (1)
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_PY_MACHINE_BITSTREAM        (1)
#define MICROPY_PY_MACHINE_PULSE            (1)
#define MICROPY_PY_MACHINE_PWM              (1)
#define MICROPY_PY_MACHINE_PWM_INIT         (1)
#define MICROPY_PY_MACHINE_PWM_DUTY         (1)
#define MICROPY_PY_MACHINE_PWM_DUTY_U16_NS  (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE  "ports/esp32/machine_pwm.c"
#define MICROPY_PY_MACHINE_I2C              (1)
#define MICROPY_PY_MACHINE_I2C_TRANSFER_WRITE1 (1)
#define MICROPY_PY_MACHINE_SOFTI2C          (1)
#define MICROPY_PY_MACHINE_SPI              (1)
#define MICROPY_PY_MACHINE_SPI_MSB          (0)
#define MICROPY_PY_MACHINE_SPI_LSB          (1)
#define MICROPY_PY_MACHINE_SOFTSPI          (1)
#ifndef MICROPY_PY_MACHINE_DAC
#define MICROPY_PY_MACHINE_DAC              (1)
#endif
#ifndef MICROPY_PY_MACHINE_I2S
#define MICROPY_PY_MACHINE_I2S              (1)
#endif
#define MICROPY_PY_NETWORK (1)
#ifndef MICROPY_PY_NETWORK_HOSTNAME_DEFAULT
#if CONFIG_IDF_TARGET_ESP32
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32"
#elif CONFIG_IDF_TARGET_ESP32S2
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32s2"
#elif CONFIG_IDF_TARGET_ESP32S3
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32s3"
#elif CONFIG_IDF_TARGET_ESP32C3
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-esp32c3"
#endif
#endif
#define MICROPY_PY_NETWORK_INCLUDEFILE      "ports/esp32/modnetwork.h"
#define MICROPY_PY_NETWORK_MODULE_GLOBALS_INCLUDEFILE "ports/esp32/modnetwork_globals.h"
#ifndef MICROPY_PY_NETWORK_WLAN
#define MICROPY_PY_NETWORK_WLAN             (1)
#endif
#ifndef MICROPY_HW_ENABLE_SDCARD
#define MICROPY_HW_ENABLE_SDCARD            (1)
#endif
#define MICROPY_HW_SOFTSPI_MIN_DELAY        (0)
#define MICROPY_HW_SOFTSPI_MAX_BAUDRATE     (ets_get_cpu_frequency() * 1000000 / 200) // roughly
#define MICROPY_PY_USSL                     (1)
#define MICROPY_SSL_MBEDTLS                 (1)
#define MICROPY_PY_USSL_FINALISER           (1)
#define MICROPY_PY_UWEBSOCKET               (1)
#define MICROPY_PY_WEBREPL                  (1)
#define MICROPY_PY_ONEWIRE                  (1)
#define MICROPY_PY_UPLATFORM                (1)
#define MICROPY_PY_USOCKET_EVENTS           (MICROPY_PY_WEBREPL)
#define MICROPY_PY_BLUETOOTH_RANDOM_ADDR    (1)
#define MICROPY_PY_BLUETOOTH_DEFAULT_GAP_NAME ("ESP32")

// fatfs configuration
#define MICROPY_FATFS_ENABLE_LFN            (1)
#define MICROPY_FATFS_RPATH                 (2)
#define MICROPY_FATFS_MAX_SS                (4096)
#define MICROPY_FATFS_LFN_CODE_PAGE         437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */

#define MP_STATE_PORT MP_STATE_VM

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p)))
void *esp_native_code_commit(void *, size_t, void *);
#define MP_PLAT_COMMIT_EXEC(buf, len, reloc) esp_native_code_commit(buf, len, reloc)
#define MP_SSIZE_MAX (0x7fffffff)

// Note: these "critical nested" macros do not ensure cross-CPU exclusion,
// the only disable interrupts on the current CPU.  To full manage exclusion
// one should use portENTER_CRITICAL/portEXIT_CRITICAL instead.
#include "freertos/FreeRTOS.h"
#define MICROPY_BEGIN_ATOMIC_SECTION() portENTER_CRITICAL_NESTED()
#define MICROPY_END_ATOMIC_SECTION(state) portEXIT_CRITICAL_NESTED(state)

#if MICROPY_PY_USOCKET_EVENTS
#define MICROPY_PY_USOCKET_EVENTS_HANDLER extern void usocket_events_handler(void); usocket_events_handler();
#else
#define MICROPY_PY_USOCKET_EVENTS_HANDLER
#endif

#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MICROPY_PY_USOCKET_EVENTS_HANDLER \
        MP_THREAD_GIL_EXIT(); \
        ulTaskNotifyTake(pdFALSE, 1); \
        MP_THREAD_GIL_ENTER(); \
    } while (0);
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MICROPY_PY_USOCKET_EVENTS_HANDLER \
        asm ("waiti 0"); \
    } while (0);
#endif

// Functions that should go in IRAM
#define MICROPY_WRAP_MP_BINARY_OP(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_EXECUTE_BYTECODE(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_LOAD_GLOBAL(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_LOAD_NAME(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_MAP_LOOKUP(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_OBJ_GET_TYPE(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_SCHED_EXCEPTION(f) IRAM_ATTR f
#define MICROPY_WRAP_MP_SCHED_KEYBOARD_INTERRUPT(f) IRAM_ATTR f

#define UINT_FMT "%u"
#define INT_FMT "%d"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
// ssize_t, off_t as required by POSIX-signatured functions in stream.h
#include <sys/types.h>

// board specifics
#define MICROPY_PY_SYS_PLATFORM "esp32"

// ESP32-S3 extended IO for 47 & 48
#ifndef MICROPY_HW_ESP32S3_EXTENDED_IO
#define MICROPY_HW_ESP32S3_EXTENDED_IO      (1)
#endif

#ifndef MICROPY_HW_ENABLE_MDNS_QUERIES
#define MICROPY_HW_ENABLE_MDNS_QUERIES      (1)
#endif

#ifndef MICROPY_HW_ENABLE_MDNS_RESPONDER
#define MICROPY_HW_ENABLE_MDNS_RESPONDER    (1)
#endif

#ifndef MICROPY_BOARD_STARTUP
#define MICROPY_BOARD_STARTUP boardctrl_startup
#endif

void boardctrl_startup(void);

#ifndef MICROPY_PY_NETWORK_LAN
#if (ESP_IDF_VERSION_MAJOR == 4) && (ESP_IDF_VERSION_MINOR >= 1) && (CONFIG_IDF_TARGET_ESP32 || (CONFIG_ETH_USE_SPI_ETHERNET && (CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL || CONFIG_ETH_SPI_ETHERNET_DM9051 || CONFIG_ETH_SPI_ETHERNET_W5500)))
#define MICROPY_PY_NETWORK_LAN              (1)
#else
#define MICROPY_PY_NETWORK_LAN              (0)
#endif
#endif

#if MICROPY_PY_NETWORK_LAN && CONFIG_ETH_USE_SPI_ETHERNET
#ifndef MICROPY_PY_NETWORK_LAN_SPI_CLOCK_SPEED_MZ
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
#define MICROPY_PY_NETWORK_LAN_SPI_CLOCK_SPEED_MZ       (12)
#else
#define MICROPY_PY_NETWORK_LAN_SPI_CLOCK_SPEED_MZ       (36)
#endif
#endif
#endif
