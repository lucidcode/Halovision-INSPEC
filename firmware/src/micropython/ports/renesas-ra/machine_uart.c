/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/mpirq.h"
#include "uart.h"
#include "irq.h"
#include "pendsv.h"

#define DEFAULT_UART_BAUDRATE (115200)

STATIC const char *_parity_name[] = {"None", "ODD", "EVEN"};

/// \moduleref pyb
/// \class UART - duplex serial communication bus
///
/// UART implements the standard UART/USART duplex serial communications protocol.  At
/// the physical level it consists of 2 lines: RX and TX.  The unit of communication
/// is a character (not to be confused with a string character) which can be 8 or 9
/// bits wide.
///
/// UART objects can be created and initialised using:
///
///     from pyb import UART
///
///     uart = UART(1, 9600)                         # init with given baudrate
///     uart.init(9600, bits=8, parity=None, stop=1) # init with given parameters
///
/// Bits can be 8 or 9.  Parity can be None, 0 (even) or 1 (odd).  Stop can be 1 or 2.
///
/// A UART object acts like a stream object and reading and writing is done
/// using the standard stream methods:
///
///     uart.read(10)       # read 10 characters, returns a bytes object
///     uart.read()         # read all available characters
///     uart.readline()     # read a line
///     uart.readinto(buf)  # read and store into the given buffer
///     uart.write('abc')   # write the 3 characters
///
/// Individual characters can be read/written using:
///
///     uart.readchar()     # read 1 character and returns it as an integer
///     uart.writechar(42)  # write 1 character
///
/// To check if there is anything to be read, use:
///
///     uart.any()               # returns True if any characters waiting

STATIC void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->is_enabled) {
        mp_printf(print, "UART(%u)", self->uart_id);
    } else {
        mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u",
            self->uart_id, self->baudrate, self->bits,
            _parity_name[self->parity], self->stop);
        mp_printf(print, ", tx=%q, rx=%q", self->tx->name, self->rx->name);
        if (self->rts) {
            mp_printf(print, ", rts=%q", self->rts->name);
        }
        if (self->cts) {
            mp_printf(print, ", cts=%q", self->cts->name);
        }
        mp_printf(print, ", flow=%d, rxbuf=%d, timeout=%u, timeout_char=%u",
            self->flow,
            self->read_buf_len == 0 ? 0 : self->read_buf_len - 1,   // -1 to adjust for usable length of buffer
            self->timeout, self->timeout_char);
        if (self->mp_irq_trigger != 0) {
            mp_printf(print, ", irq=0x%x", self->mp_irq_trigger);
        }
        mp_print_str(print, ")");
    }
}

/// \method init(baudrate, bits=8, parity=None, stop=1, *, timeout=1000, timeout_char=0, flow=0, read_buf_len=64)
///
/// Initialise the UART bus with the given parameters:
///
///   - `baudrate` is the clock rate.
///   - `bits` is the number of bits per byte, 7, 8 or 9.
///   - `parity` is the parity, `None`, 0 (even) or 1 (odd).
///   - `stop` is the number of stop bits, 1 or 2.
///   - `timeout` is the timeout in milliseconds to wait for the first character.
///   - `timeout_char` is the timeout in milliseconds to wait between characters.
///   - `flow` is RTS | CTS where RTS == 256, CTS == 512
///   - `read_buf_len` is the character length of the read buffer (0 to disable).
STATIC mp_obj_t machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_read_buf_len, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 512} },  // legacy
    };

    // parse args
    struct {
        mp_arg_val_t baudrate, bits, parity, stop, flow, timeout, timeout_char, rxbuf, read_buf_len;
    } args;
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t *)&args);

    // baudrate
    uint32_t baudrate = args.baudrate.u_int;
    if (baudrate == 0) {
        baudrate = DEFAULT_UART_BAUDRATE;
    }

    // parity
    uint32_t bits = args.bits.u_int;
    uint32_t parity;
    if (args.parity.u_obj == mp_const_none) {
        parity = UART_PARITY_NONE;
    } else {
        mp_int_t p = mp_obj_get_int(args.parity.u_obj);
        parity = (p & 1) ? UART_PARITY_ODD : UART_PARITY_EVEN;
        bits += 1; // STs convention has bits including parity
    }

    // number of bits
    if (!((bits == 7) | (bits == 8) | (bits == 9))) {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported combination of bits and parity"));
    }

    // stop bits
    uint32_t stop;
    switch (args.stop.u_int) {
        case 1:
            stop = UART_STOPBITS_1;
            break;
        default:
            stop = UART_STOPBITS_2;
            break;
    }

    // flow control
    uint32_t flow = args.flow.u_int;

    // Save attach_to_repl setting because uart_init will disable it.
    bool attach_to_repl = self->attached_to_repl;

    // uint32_t irq_state = disable_irq();

    // init UART (if it fails, it's because the port doesn't exist)
    if (!uart_init(self, baudrate, bits, parity, stop, flow)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), self->uart_id);
    }

    // Restore attach_to_repl setting so UART still works if attached to dupterm.
    uart_attach_to_repl(self, attach_to_repl);

    // set timeout
    self->timeout = args.timeout.u_int;

    // set timeout_char
    // make sure it is at least as long as a whole character (13 bits to be safe)
    // minimum value is 2ms because sys-tick has a resolution of only 1ms
    self->timeout_char = args.timeout_char.u_int;
    uint32_t min_timeout_char = 13000 / baudrate + 2;
    if (self->timeout_char < min_timeout_char) {
        self->timeout_char = min_timeout_char;
    }

    if (self->is_static) {
        // Static UARTs have fixed memory for the rxbuf and can't be reconfigured.
        if (args.rxbuf.u_int >= 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("UART is static and rxbuf can't be changed"));
        }
        uart_set_rxbuf(self, self->read_buf_len, self->read_buf);
    } else {
        // setup the read buffer
        m_del(byte, self->read_buf, self->read_buf_len << self->char_width);
        if (args.rxbuf.u_int >= 0) {
            // rxbuf overrides legacy read_buf_len
            args.read_buf_len.u_int = args.rxbuf.u_int;
        }
        if (args.read_buf_len.u_int <= 0) {
            // no read buffer
            uart_set_rxbuf(self, 0, NULL);
        } else {
            // read buffer using interrupts
            size_t len = args.read_buf_len.u_int + 1; // +1 to adjust for usable length of buffer
            uint8_t *buf = m_new(byte, len << self->char_width);
            uart_set_rxbuf(self, len, buf);
        }
    }

    #if RA_TODO
    // compute actual baudrate that was configured
    uint32_t actual_baudrate = uart_get_baudrate(self);

    // check we could set the baudrate within 5%
    uint32_t baudrate_diff;
    if (actual_baudrate > baudrate) {
        baudrate_diff = actual_baudrate - baudrate;
    } else {
        baudrate_diff = baudrate - actual_baudrate;
    }
    if (20 * baudrate_diff > actual_baudrate) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("set baudrate %d is not within 5%% of desired value"), actual_baudrate);
    }
    #endif

    // enable_irq(irq_state);
    return mp_const_none;
}

/// \classmethod \constructor(bus, ...)
///
/// Construct a UART object on the given bus.  `bus` can be 1-6, or 'XA', 'XB', 'YA', or 'YB'.
/// With no additional parameters, the UART object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.
///
/// The physical pins of the UART buses are:
///
///   - `UART(4)` is on `XA`: `(TX, RX) = (X1, X2) = (PA0, PA1)`
///   - `UART(1)` is on `XB`: `(TX, RX) = (X9, X10) = (PB6, PB7)`
///   - `UART(6)` is on `YA`: `(TX, RX) = (Y1, Y2) = (PC6, PC7)`
///   - `UART(3)` is on `YB`: `(TX, RX) = (Y9, Y10) = (PB10, PB11)`
///   - `UART(2)` is on: `(TX, RX) = (X3, X4) = (PA2, PA3)`
STATIC mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out port
    int uart_id = 0;
    if (mp_obj_is_str(args[0])) {
        const char *port = mp_obj_str_get_str(args[0]);
        if (0) {
        #ifdef MICROPY_HW_UART0_NAME
        } else if (strcmp(port, MICROPY_HW_UART0_NAME) == 0) {
            uart_id = HW_UART_0;
        #endif
        #ifdef MICROPY_HW_UART1_NAME
        } else if (strcmp(port, MICROPY_HW_UART1_NAME) == 0) {
            uart_id = HW_UART_1;
        #endif
        #ifdef MICROPY_HW_UART2_NAME
        } else if (strcmp(port, MICROPY_HW_UART2_NAME) == 0) {
            uart_id = HW_UART_2;
        #endif
        #ifdef MICROPY_HW_UART3_NAME
        } else if (strcmp(port, MICROPY_HW_UART3_NAME) == 0) {
            uart_id = HW_UART_3;
        #endif
        #ifdef MICROPY_HW_UART4_NAME
        } else if (strcmp(port, MICROPY_HW_UART4_NAME) == 0) {
            uart_id = HW_UART_4;
        #endif
        #ifdef MICROPY_HW_UART5_NAME
        } else if (strcmp(port, MICROPY_HW_UART5_NAME) == 0) {
            uart_id = HW_UART_5;
        #endif
        #ifdef MICROPY_HW_UART6_NAME
        } else if (strcmp(port, MICROPY_HW_UART6_NAME) == 0) {
            uart_id = HW_UART_6;
        #endif
        #ifdef MICROPY_HW_UART7_NAME
        } else if (strcmp(port, MICROPY_HW_UART7_NAME) == 0) {
            uart_id = HW_UART_7;
        #endif
        #ifdef MICROPY_HW_UART8_NAME
        } else if (strcmp(port, MICROPY_HW_UART8_NAME) == 0) {
            uart_id = HW_UART_8;
        #endif
        #ifdef MICROPY_HW_UART9_NAME
        } else if (strcmp(port, MICROPY_HW_UART9_NAME) == 0) {
            uart_id = HW_UART_9;
        #endif
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%s) doesn't exist"), port);
        }
    } else {
        uart_id = mp_obj_get_int(args[0]);
        if (!uart_exists(uart_id)) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), uart_id);
        }
    }

    // check if the UART is reserved for system use or not
    if (MICROPY_HW_UART_IS_RESERVED(uart_id)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) is reserved"), uart_id);
    }

    machine_uart_obj_t *self;
    if (MP_STATE_PORT(machine_uart_obj_all)[uart_id] == NULL) {
        // create new UART object
        self = m_new0(machine_uart_obj_t, 1);
        self->base.type = &machine_uart_type;
        self->uart_id = uart_id;
        MP_STATE_PORT(machine_uart_obj_all)[uart_id] = self;
    } else {
        // reference existing UART object
        self = MP_STATE_PORT(machine_uart_obj_all)[uart_id];
    }

    // start the peripheral
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_uart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_uart_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_init_obj, 1, machine_uart_init);

/// \method deinit()
/// Turn off the UART bus.
STATIC mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uart_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);

/// \method any()
/// Return `True` if any characters waiting, else `False`.
STATIC mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(uart_rx_any(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

/// \method writechar(char)
/// Write a single character on the bus.  `char` is an integer to write.
/// Return value: `None`.
STATIC mp_obj_t machine_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // get the character to write (might be 9 bits)
    uint16_t data = mp_obj_get_int(char_in);

    // write the character
    int errcode;
    if (uart_tx_wait(self, self->timeout)) {
        uart_tx_data(self, &data, 1, &errcode);
    } else {
        errcode = MP_ETIMEDOUT;
    }

    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_writechar_obj, machine_uart_writechar);

/// \method readchar()
/// Receive a single character on the bus.
/// Return value: The character read, as an integer.  Returns -1 on timeout.
STATIC mp_obj_t machine_uart_readchar(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (uart_rx_wait(self, self->timeout)) {
        return MP_OBJ_NEW_SMALL_INT(uart_rx_char(self));
    } else {
        // return -1 on timeout
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_readchar_obj, machine_uart_readchar);

// uart.sendbreak()
STATIC mp_obj_t machine_uart_sendbreak(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ra_sci_tx_break((uint32_t)self->uart_id);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_sendbreak_obj, machine_uart_sendbreak);

// \method uart.txdone()
// Return `True` if all characters have been sent.
STATIC mp_obj_t machine_uart_txdone(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return uart_tx_busy(self) ? mp_const_false : mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_txdone_obj, machine_uart_txdone);

// irq(handler, trigger, hard)
STATIC mp_obj_t machine_uart_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_IRQ_ARG_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_IRQ_ARG_INIT_NUM_ARGS, mp_irq_init_args, args);
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    if (self->mp_irq_obj == NULL) {
        self->mp_irq_trigger = 0;
        self->mp_irq_obj = mp_irq_new(&uart_irq_methods, MP_OBJ_FROM_PTR(self));
    }

    if (n_args > 1 || kw_args->used != 0) {
        // Check the handler
        mp_obj_t handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
            mp_raise_ValueError(MP_ERROR_TEXT("handler must be None or callable"));
        }

        // Check the trigger
        mp_uint_t trigger = args[MP_IRQ_ARG_INIT_trigger].u_int;
        mp_uint_t not_supported = trigger & ~MP_UART_ALLOWED_FLAGS;
        if (trigger != 0 && not_supported) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("trigger 0x%08x unsupported"), not_supported);
        }

        // Reconfigure user IRQs
        uart_irq_config(self, false);
        self->mp_irq_obj->handler = handler;
        self->mp_irq_obj->ishard = args[MP_IRQ_ARG_INIT_hard].u_bool;
        self->mp_irq_trigger = trigger;
        uart_irq_config(self, true);
    }

    return MP_OBJ_FROM_PTR(self->mp_irq_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_irq_obj, 1, machine_uart_irq);

STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    // instance methods

    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_txdone), MP_ROM_PTR(&machine_uart_txdone_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },

    /// \method read([nbytes])
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    /// \method readline()
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    /// \method readinto(buf[, nbytes])
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    /// \method write(buf)
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_uart_irq_obj) },

    { MP_ROM_QSTR(MP_QSTR_writechar), MP_ROM_PTR(&machine_uart_writechar_obj) },
    { MP_ROM_QSTR(MP_QSTR_readchar), MP_ROM_PTR(&machine_uart_readchar_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendbreak), MP_ROM_PTR(&machine_uart_sendbreak_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HWCONTROL_RTS) },
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HWCONTROL_CTS) },
};

STATIC MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

STATIC mp_uint_t machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    byte *buf = buf_in;

    // check that size is a multiple of character width
    if (size & self->char_width) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    // convert byte size to char size
    size >>= self->char_width;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    // wait for first char to become available
    if (!uart_rx_wait(self, self->timeout)) {
        // return EAGAIN error to indicate non-blocking (then read() method returns None)
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // read the data
    byte *orig_buf = buf;
    for (;;) {
        int data = uart_rx_char(self);
        if (self->char_width == CHAR_WIDTH_9BIT) {
            *(uint16_t *)buf = data;
            buf += 2;
        } else {
            *buf++ = data;
        }
        if (--size == 0 || !uart_rx_wait(self, self->timeout_char)) {
            // return number of bytes read
            return buf - orig_buf;
        }
    }
}

STATIC mp_uint_t machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const byte *buf = buf_in;

    // check that size is a multiple of character width
    if (size & self->char_width) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    // wait to be able to write the first character. EAGAIN causes write to return None
    if (self->timeout != 0) {
        if (!uart_tx_wait(self, self->timeout)) {
            *errcode = MP_EAGAIN;
            return MP_STREAM_ERROR;
        }
    }

    // write the data
    size_t num_tx = uart_tx_data(self, buf, size >> self->char_width, errcode);

    if (*errcode == 0 || *errcode == MP_ETIMEDOUT) {
        // return number of bytes written, even if there was a timeout
        return num_tx << self->char_width;
    } else {
        return MP_STREAM_ERROR;
    }
}

STATIC mp_uint_t machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && uart_rx_any(self)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && uart_tx_avail(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        // The timeout is estimated using the buffer size and the baudrate.
        // Take the worst case assumptions at 13 bit symbol size times 2.
        uint32_t timeout = mp_hal_ticks_ms() +
            (uint32_t)(uart_tx_txbuf(self)) * 13000ll * 2 / self->baudrate;
        do {
            if (!uart_tx_busy(self)) {
                return 0;
            }
            MICROPY_EVENT_POLL_HOOK
        } while (mp_hal_ticks_ms() < timeout);
        *errcode = MP_ETIMEDOUT;
        ret = MP_STREAM_ERROR;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = machine_uart_read,
    .write = machine_uart_write,
    .ioctl = machine_uart_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, machine_uart_make_new,
    locals_dict, &machine_uart_locals_dict,
    print, machine_uart_print,
    protocol, &uart_stream_p
    );

MP_REGISTER_ROOT_POINTER(struct _machine_uart_obj_t *machine_uart_obj_all[MICROPY_HW_MAX_UART + MICROPY_HW_MAX_LPUART]);
