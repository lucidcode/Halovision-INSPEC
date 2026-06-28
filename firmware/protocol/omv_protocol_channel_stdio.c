/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2022-2024 OpenMV, LLC.
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
 *
 * OpenMV Protocol Channels.
 */

#include "py/mphal.h"
#include "py/nlr.h"
#include "py/gc.h"
#include "py/ringbuf.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/runtime/pyexec.h"

#include "omv_common.h"
#include "omv_protocol.h"
#include "board_config.h"

#ifndef OMV_PROTOCOL_STDIO_BUFFER_SIZE
#define OMV_PROTOCOL_STDIO_BUFFER_SIZE  (1024)
#endif

#ifndef OMV_PROTOCOL_STDIO_FLUSH_MS
#define OMV_PROTOCOL_STDIO_FLUSH_MS     (50)
#endif

typedef struct {
    vstr_t vstrbuf;
    bool script_running;
    bool notified;
    uint32_t last_notify_ms;
    ringbuf_t ringbuf;
    uint8_t rawbuf[OMV_PROTOCOL_STDIO_BUFFER_SIZE];
} stdio_channel_context_t;

static stdio_channel_context_t stdio_channel_ctx;

static int stdin_channel_init(const omv_protocol_channel_t *channel) {
    stdio_channel_context_t *ctx = channel->priv;
    ctx->script_running = false;
    vstr_init(&ctx->vstrbuf, 2048);
    return 0;
}

static int stdout_channel_init(const omv_protocol_channel_t *channel) {
    stdio_channel_context_t *ctx = channel->priv;
    ctx->last_notify_ms = 0;

    // Initialize ring buffer once to keep output from previous runs.
    if (ctx->ringbuf.buf == NULL) {
        ctx->ringbuf = (ringbuf_t) {
            ctx->rawbuf, sizeof(ctx->rawbuf), 0, 0
        };
    }
    return 0;
}

void stdio_channel_pyexec_hook(bool running) {
    stdio_channel_ctx.script_running = running;
    omv_protocol_send_event(OMV_PROTOCOL_CHANNEL_ID_STDIN, running, false);
}

static bool stdin_channel_poll(const omv_protocol_channel_t *channel) {
    stdio_channel_context_t *ctx = channel->priv;
    return ctx->script_running;
}

static bool stdout_channel_poll(const omv_protocol_channel_t *channel) {
    stdio_channel_context_t *ctx = channel->priv;
    return ringbuf_avail(&ctx->ringbuf);
}

static size_t stdio_channel_size(const omv_protocol_channel_t *channel) {
    stdio_channel_context_t *ctx = channel->priv;
    return ringbuf_avail(&ctx->ringbuf);
}

static int stdio_channel_read(const omv_protocol_channel_t *channel,
                              uint32_t offset, size_t size, void *data) {
    stdio_channel_context_t *ctx = channel->priv;

    size = OMV_MIN(size, ringbuf_avail(&ctx->ringbuf));
    int ret = !ringbuf_get_bytes(&ctx->ringbuf, data, size) ? size : -1;
    ctx->notified = false;
    return ret;
}

static int stdio_channel_write(const omv_protocol_channel_t *channel,
                               uint32_t offset, size_t size, const void *data) {
    stdio_channel_context_t *ctx = channel->priv;

    if (offset == 0) {
        vstr_reset(&ctx->vstrbuf);
    }

    nlr_buf_t nlr;
    if (!gc_is_locked() && nlr_push(&nlr) == 0) {
        char *buf = vstr_add_len(&ctx->vstrbuf, size);
        memcpy(buf, data, size);
        nlr_pop();
        return size;
    }

    return -1;
}

static bool stdin_channel_exec(const omv_protocol_channel_t *channel) {
    stdio_channel_context_t *ctx = channel->priv;

    if (!vstr_len(&ctx->vstrbuf)) {
        return false;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Execute the script.
        pyexec_vstr(&ctx->vstrbuf, true);
        nlr_pop();
    } else {
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t) nlr.ret_val);
    }

    vstr_reset(&ctx->vstrbuf);
    return true;
}

static int stdio_channel_ioctl(const omv_protocol_channel_t *channel, uint32_t cmd, size_t len, void *arg) {
    stdio_channel_context_t *ctx = channel->priv;

    switch (cmd) {
        case OMV_CHANNEL_IOCTL_STDIN_STOP:
            if (mp_interrupt_char != -1) {
                mp_sched_vm_abort();
                mp_sched_keyboard_interrupt();
            }
            break;
        case OMV_CHANNEL_IOCTL_STDIN_EXEC:
            if (!vstr_len(&ctx->vstrbuf)) {
                return -1;
            }
            if (mp_interrupt_char != -1) {
                mp_sched_vm_abort();
                mp_sched_keyboard_interrupt();
            }
            break;
        case OMV_CHANNEL_IOCTL_STDIN_RESET:
            vstr_reset(&ctx->vstrbuf);
            break;
        default:
            return -1;
    }

    return 0;
}

// Wrap MicroPython stdio functions to intercept REPL.
extern uintptr_t __real_mp_hal_stdio_poll(uintptr_t poll_flags);
uintptr_t __wrap_mp_hal_stdio_poll(uintptr_t poll_flags) {
    if (!omv_protocol_is_active()) {
        return __real_mp_hal_stdio_poll(poll_flags);
    }
    return 0;
}

extern mp_uint_t __real_mp_hal_stdout_tx_strn(const char *str, mp_uint_t len);
mp_uint_t __wrap_mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    if (!omv_protocol_is_active()) {
        return __real_mp_hal_stdout_tx_strn(str, len);
    }

    const omv_protocol_channel_t *channel = omv_protocol_find_channel(OMV_PROTOCOL_CHANNEL_ID_STDOUT);
    stdio_channel_context_t *ctx = channel->priv;

    ringbuf_t *rb = &ctx->ringbuf;
    size_t free = ringbuf_free(rb);

    if (len <= free) {
        // Enough space for the entire string.
        ringbuf_memcpy_put_internal(rb, (const uint8_t *) str, len);
    } else {
        // Not enough free space, reset and keep the tail of the string.
        size_t cap = rb->size - 1;
        size_t offset = (len > cap) ? len - cap : 0;

        rb->iget = 0;
        rb->iput = 0;
        ringbuf_memcpy_put_internal(rb, (const uint8_t *) str + offset, len - offset);
        ctx->notified = false;
    }

    // Notify the host once when the buffer crosses the 50% threshold.
    // Cleared when the host reads, or when the ringbuffer overflows.
    if (!ctx->notified && ringbuf_avail(rb) > (rb->size / 2)) {
        ctx->notified = true;
        ctx->last_notify_ms = mp_hal_ticks_ms();
        omv_protocol_send_event(OMV_PROTOCOL_CHANNEL_ID_STDOUT, OMV_PROTOCOL_EVENT_NOTIFY, false);
    }

    return len;
}

const omv_protocol_channel_t omv_stdin_channel = {
    .priv = &stdio_channel_ctx,
    .id = OMV_PROTOCOL_CHANNEL_ID_STDIN,
    .flags = OMV_PROTOCOL_CHANNEL_FLAG_WRITE | OMV_PROTOCOL_CHANNEL_FLAG_EXEC,
    .name = "stdin",
    .init = stdin_channel_init,
    .poll = stdin_channel_poll,
    .write = stdio_channel_write,
    .ioctl = stdio_channel_ioctl,
    .exec = stdin_channel_exec
};

static void stdout_channel_tick(const omv_protocol_channel_t *channel) {
    stdio_channel_context_t *ctx = channel->priv;
    if (ringbuf_avail(&ctx->ringbuf)
        && (mp_hal_ticks_ms() - ctx->last_notify_ms) >= OMV_PROTOCOL_STDIO_FLUSH_MS) {
        ctx->last_notify_ms = mp_hal_ticks_ms();
        omv_protocol_send_event(OMV_PROTOCOL_CHANNEL_ID_STDOUT, OMV_PROTOCOL_EVENT_NOTIFY, false);
    }
}

const omv_protocol_channel_t omv_stdout_channel = {
    .priv = &stdio_channel_ctx,
    .id = OMV_PROTOCOL_CHANNEL_ID_STDOUT,
    .flags = OMV_PROTOCOL_CHANNEL_FLAG_READ,
    .name = "stdout",
    .init = stdout_channel_init,
    .poll = stdout_channel_poll,
    .size = stdio_channel_size,
    .read = stdio_channel_read,
    .tick = stdout_channel_tick,
};
