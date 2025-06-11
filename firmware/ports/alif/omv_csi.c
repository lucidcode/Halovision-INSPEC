/*
 * Copyright (C) 2023-2024 OpenMV, LLC.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Any redistribution, use, or modification in source or binary form
 *    is done solely for personal benefit and not for any commercial
 *    purpose or for monetary gain. For commercial licensing options,
 *    please contact openmv@openmv.io
 *
 * THIS SOFTWARE IS PROVIDED BY THE LICENSOR AND COPYRIGHT OWNER "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE LICENSOR OR COPYRIGHT
 * OWNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alif CSI driver.
 */
#if MICROPY_PY_CSI
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "py/mphal.h"
#include "py/runtime.h"

#include "alif_hal.h"
#include "cpi.h"
#include "sys_ctrl_cpi.h"
#include "system_utils.h"

#include "omv_boardconfig.h"
#include "omv_gpio.h"
#include "omv_gpu.h"
#include "omv_i2c.h"
#include "omv_csi.h"
#include "unaligned_memcpy.h"

// Bits missing from cpi.h
#define CAM_CFG_INTERFACE_Pos       (0U)
#define CAM_CFG_CSI_HALT_EN_Pos     (1U)
#define CAM_CFG_RW_ROUNDUP_Pos      (8U)
#define CAM_CFG_PXCLK_POL_Pos       (12U)
#define CAM_CFG_ENDIANNESS_Pos      (20U)

#define CPI_VSYNC_MODE_DISABLE      (0)
#define CPI_VSYNC_MODE_ENABLE       (1)

#define CPI_VSYNC_WAIT_DISABLE      (0)
#define CPI_VSYNC_WAIT_ENABLE       (1)

#define CPI_IRQ_FLAGS   (CAM_INTR_STOP | CAM_INTR_VSYNC | CAM_INTR_HSYNC |    \
                         CAM_INTR_INFIFO_OVERRUN | CAM_INTR_OUTFIFO_OVERRUN | \
                         CAM_INTR_BRESP_ERR)

#define CPI_ERROR_FLAGS (CAM_INTR_INFIFO_OVERRUN |  \
                         CAM_INTR_OUTFIFO_OVERRUN | \
                         CAM_INTR_BRESP_ERR)

static CPI_Type *cpi_get_base_addr(omv_csi_t *csi) {
    return ((CPI_Type *) CPI_BASE);
}

void omv_csi_init0() {
    omv_csi_abort(&csi, true, false);

    // Re-init I2C to reset the bus state after soft reset, which
    // could have interrupted the bus in the middle of a transfer.
    if (csi.i2c_bus.initialized) {
        // Reinitialize the bus using the last used id and speed.
        // TODO: Causes Alif's I3C to lock up.
        //omv_i2c_init(&csi.i2c_bus, csi.i2c_bus.id, csi.i2c_bus.speed);
    }

    csi.disable_delays = false;

    // Disable VSYNC IRQ and callback
    omv_csi_set_vsync_callback(NULL);

    // Disable Frame callback.
    omv_csi_set_frame_callback(NULL);
}

int omv_csi_init() {
    int init_ret = 0;
    CPI_Type *cpi = cpi_get_base_addr(&csi);

    alif_hal_csi_init(cpi, 0);

    #if defined(OMV_CSI_POWER_PIN)
    omv_gpio_write(OMV_CSI_POWER_PIN, 0);
    #endif

    #if defined(OMV_CSI_RESET_PIN)
    omv_gpio_write(OMV_CSI_RESET_PIN, 0);
    #endif

    // Reset the csi state
    memset(&csi, 0, sizeof(omv_csi_t));

    // Set default framebuffer
    csi.fb = framebuffer_get(0);

    // Set default snapshot function.
    csi.snapshot = omv_csi_snapshot;

    // Configure the CSI external clock.
    if (omv_csi_set_clk_frequency(OMV_CSI_CLK_FREQUENCY) != 0) {
        // Failed to initialize the csi clock.
        return OMV_CSI_ERROR_TIM_INIT_FAILED;
    }

    // Detect and initialize the image csi.
    if ((init_ret = omv_csi_probe_init(OMV_CSI_I2C_ID, OMV_CSI_I2C_SPEED)) != 0) {
        // csi probe/init failed.
        return init_ret;
    }

    // Configure the CSI interface.
    if (omv_csi_config(OMV_CSI_CONFIG_INIT) != 0) {
        // CSI config failed
        return OMV_CSI_ERROR_CSI_INIT_FAILED;
    }

    // Set default color palette.
    csi.color_palette = rainbow_table;

    // Disable VSYNC IRQ and callback
    omv_csi_set_vsync_callback(NULL);

    // Disable Frame callback.
    omv_csi_set_frame_callback(NULL);

    // All good!
    csi.detected = true;

    return 0;
}

int omv_csi_config(omv_csi_config_t config) {
    if (config == OMV_CSI_CONFIG_INIT) {
        CPI_Type *cpi = cpi_get_base_addr(&csi);

        // Configure the FIFO.
        cpi->CAM_FIFO_CTRL &= ~CAM_FIFO_CTRL_RD_WMARK_Msk;
        cpi->CAM_FIFO_CTRL = 0x08;

        cpi->CAM_FIFO_CTRL &= ~CAM_FIFO_CTRL_WR_WMARK_Msk;
        cpi->CAM_FIFO_CTRL |= (0x18 << CAM_FIFO_CTRL_WR_WMARK_Pos);

        cpi->CAM_CFG = 0;
        // Configure the capture interface (CPI, LPCPI or CSI).
        cpi->CAM_CFG |= (CPI_INTERFACE_PARALLEL << CAM_CFG_INTERFACE_Pos);
        cpi->CAM_CFG |= (CPI_VSYNC_MODE_DISABLE << CAM_CFG_VSYNC_MODE_Pos);
        cpi->CAM_CFG |= (CPI_VSYNC_WAIT_DISABLE << CAM_CFG_VSYNC_WAIT_Pos);

        // Set VSYNC, HSYNC and PIXCLK polarities.
        cpi->CAM_CFG |= (csi.vsync_pol << CAM_CFG_VSYNC_POL_Pos);
        cpi->CAM_CFG |= (csi.hsync_pol << CAM_CFG_HSYNC_POL_Pos);
        cpi->CAM_CFG |= (!csi.pixck_pol << CAM_CFG_PXCLK_POL_Pos);

        // Configure the data bus width, mode, endianness.
        cpi->CAM_CFG |= (CPI_ROW_ROUNDUP_DISABLE << CAM_CFG_RW_ROUNDUP_Pos);
        cpi->CAM_CFG |= (CPI_DATA_MODE_BIT_8 << CAM_CFG_DATA_MODE_Pos);
        cpi->CAM_CFG |= (CPI_CODE10ON8_CODING_DISABLE << CAM_CFG_CODE10ON8_Pos);
        cpi->CAM_CFG |= (CPI_DATA_ENDIANNESS_LSB_FIRST << CAM_CFG_ENDIANNESS_Pos);

        // Configure the data mask (for 16-bits mode only).
        cpi->CAM_CFG &= ~CAM_CFG_DATA_MASK_Msk;
        cpi->CAM_CFG |= (0 << CAM_CFG_DATA_MASK_Pos);

        // Configure IPI color mode (for CSI mode only).
        cpi->CAM_CSI_CMCFG = CPI_COLOR_MODE_CONFIG_IPI48_RGB565;
    }
    return 0;
}

int omv_csi_abort(omv_csi_t *csi, bool fifo_flush, bool in_irq) {
    CPI_Type *cpi = cpi_get_base_addr(csi);
    cpi->CAM_CTRL = 0;
    NVIC_DisableIRQ(CAM_IRQ_IRQn);
    cpi_disable_interrupt(cpi, CPI_IRQ_FLAGS);
    cpi_irq_handler_clear_intr_status(cpi, CPI_IRQ_FLAGS);

    csi->first_line = false;
    csi->drop_frame = false;
    csi->last_frame_ms = 0;
    csi->last_frame_ms_valid = false;

    if (csi->fb) {
        if (fifo_flush) {
            framebuffer_flush_buffers(csi->fb, true);
        } else if (!csi->disable_full_flush) {
            framebuffer_flush_buffers(csi->fb, false);
        }
    }
    return 0;
}

int omv_csi_set_clk_frequency(uint32_t frequency) {
    // Configure CPI clock source (400MHz or 480MHz) and divider.
    if (frequency >= 24000000) {
        set_cpi_pixel_clk(CPI_PIX_CLKSEL_480MZ, 20);
    } else if (frequency >= 12000000) {
        set_cpi_pixel_clk(CPI_PIX_CLKSEL_480MZ, 40);
    } else if (frequency >= 6000000) {
        set_cpi_pixel_clk(CPI_PIX_CLKSEL_480MZ, 80);
    } else {
        set_cpi_pixel_clk(CPI_PIX_CLKSEL_400MZ, 100);
    }
    return 0;
}

uint32_t omv_csi_get_clk_frequency() {
    uint32_t div = (CLKCTL_PER_MST->CAMERA_PIXCLK_CTRL & CAMERA_PIXCLK_CTRL_DIVISOR_Msk) >>
                   CAMERA_PIXCLK_CTRL_DIVISOR_Pos;
    if (CLKCTL_PER_MST->CAMERA_PIXCLK_CTRL & CAMERA_PIXCLK_CTRL_CLK_SEL) {
        return 480000000 / div;
    } else {
        return 400000000 / div;
    }
}

uint32_t omv_csi_get_fb_offset(omv_csi_t *csi) {
    uint32_t offset = 0;
    uint32_t bytes_per_pixel = omv_csi_get_src_bpp();
    uint32_t line_size_bytes = resolution[csi->framesize][0] * bytes_per_pixel;
    // Offset the pixels buffer for debayering.
    if (csi->raw_output && csi->pixformat == PIXFORMAT_RGB565) {
        offset += line_size_bytes * resolution[csi->framesize][1];
    }
    return offset;
}

// This is the default snapshot function, which can be replaced in omv_csi_init functions.
int omv_csi_snapshot(omv_csi_t *csi, image_t *dst_image, uint32_t flags) {
    framebuffer_t *fb = csi->fb;

    static uint32_t frames = 0;
    static uint32_t r_stat, gb_stat, gr_stat, b_stat;

    CPI_Type *cpi = cpi_get_base_addr(csi);

    if (csi->pixformat == PIXFORMAT_INVALID) {
        return OMV_CSI_ERROR_INVALID_PIXFORMAT;
    }

    if (csi->framesize == OMV_CSI_FRAMESIZE_INVALID) {
        return OMV_CSI_ERROR_INVALID_FRAMESIZE;
    }

    if (omv_csi_check_framebuffer_size() != 0) {
        return OMV_CSI_ERROR_FRAMEBUFFER_OVERFLOW;
    }

    // Compress the framebuffer for the IDE preview.
    framebuffer_update_jpeg_buffer(fb);

    // Free the current FB head.
    framebuffer_free_current_buffer(fb);

    // Reconfigure and restart the CSI transfer if it's not running.
    if (!(cpi->CAM_CTRL & CAM_CTRL_BUSY)) {
        framebuffer_setup_buffers(fb);
        uint32_t bytes_per_pixel = omv_csi_get_src_bpp();
        uint32_t line_size_bytes = resolution[csi->framesize][0] * bytes_per_pixel;

        // Error out if the transfer size is not compatible with DMA transfer restrictions.
        if ((!line_size_bytes) ||
            (line_size_bytes % sizeof(uint64_t)) ||
            csi->transpose ||
            (csi->pixformat == PIXFORMAT_JPEG)) {
            return OMV_CSI_ERROR_INVALID_FRAMESIZE;
        }

        // Get the destination buffer address.
        vbuffer_t *buffer = framebuffer_get_tail(fb, FB_PEEK);

        // Check if buffer is not ready or is not 64-bit aligned.
        if ((!buffer) || (LocalToGlobal(buffer->data) & 0x7)) {
            return OMV_CSI_ERROR_FRAMEBUFFER_ERROR;
        }

        if (!csi->raw_output &&
            ((csi->pixformat == PIXFORMAT_RGB565 && csi->rgb_swap) ||
             (csi->pixformat == PIXFORMAT_YUV422 && csi->yuv_swap))) {
            cpi->CAM_CFG |= (CPI_DATA_ENDIANNESS_MSB_FIRST << CAM_CFG_ENDIANNESS_Pos);
        } else {
            cpi->CAM_CFG &= ~(CPI_DATA_ENDIANNESS_MSB_FIRST << CAM_CFG_ENDIANNESS_Pos);
        }

        // Find maximum burst size that perfectly fits the line size.
        cpi->CAM_FIFO_CTRL &= ~CAM_FIFO_CTRL_RD_WMARK_Msk;

        for (uint32_t i = 16; i >= 4; i--) {
            if (!(line_size_bytes % (i * 8))) {
                cpi->CAM_FIFO_CTRL |= (i << CAM_FIFO_CTRL_RD_WMARK_Pos);
                break;
            }
        }

        cpi->CAM_VIDEO_FCFG &= ~CAM_VIDEO_FCFG_DATA_Msk;
        cpi->CAM_VIDEO_FCFG = line_size_bytes;
        cpi->CAM_VIDEO_FCFG &= ~CAM_VIDEO_FCFG_ROW_Msk;
        cpi->CAM_VIDEO_FCFG |= ((resolution[csi->framesize][1] - 1) << CAM_VIDEO_FCFG_ROW_Pos);
        cpi->CAM_FRAME_ADDR = LocalToGlobal(buffer->data + omv_csi_get_fb_offset(csi));

        // Configure and enable CSI interrupts.
        cpi_irq_handler_clear_intr_status(cpi, CPI_IRQ_FLAGS);
        cpi_enable_interrupt(cpi, CPI_IRQ_FLAGS);
        NVIC_ClearPendingIRQ(CAM_IRQ_IRQn);
        NVIC_EnableIRQ(CAM_IRQ_IRQn);

        // Reset CSI and start the capture.
        cpi->CAM_CTRL = 0;
        cpi->CAM_CTRL |= CAM_CTRL_SW_RESET;
        cpi->CAM_CTRL = (CAM_CTRL_SNAPSHOT | CAM_CTRL_START | CAM_CTRL_FIFO_CLK_SEL);
        //printf("==== reconfigured ===\n");
    }

    // Let the camera know we want to trigger it now.
    #if defined(OMV_CSI_FSYNC_PIN)
    if (csi->frame_sync) {
        omv_gpio_write(OMV_CSI_FSYNC_PIN, 1);
    }
    #endif

    vbuffer_t *buffer = framebuffer_get_head(fb, FB_INVALIDATE);
    // Wait for the DMA to finish the transfer.
    for (mp_uint_t ticks = mp_hal_ticks_ms(); buffer == NULL;) {
        mp_event_handle_nowait();

        if ((mp_hal_ticks_ms() - ticks) > 3000) {
            omv_csi_abort(csi, true, false);

            #if defined(OMV_CSI_FSYNC_PIN)
            if (csi->frame_sync) {
                omv_gpio_write(OMV_CSI_FSYNC_PIN, 0);
            }
            #endif

            return OMV_CSI_ERROR_CAPTURE_TIMEOUT;
        }

        buffer = framebuffer_get_head(fb, FB_INVALIDATE);
    }

    // We're done receiving data.
    #if defined(OMV_CSI_FSYNC_PIN)
    if (csi->frame_sync) {
        omv_gpio_write(OMV_CSI_FSYNC_PIN, 0);
    }
    #endif

    // Restore the frame buffer width and height.
    if (!csi->transpose) {
        fb->w = fb->u;
        fb->h = fb->v;
    } else {
        fb->w = fb->v;
        fb->h = fb->u;
    }

    // Reset the frame buffer's pixel format.
    switch (csi->pixformat) {
        case PIXFORMAT_GRAYSCALE:
            fb->pixfmt = PIXFORMAT_GRAYSCALE;
            break;
        case PIXFORMAT_RGB565:
            fb->pixfmt = PIXFORMAT_RGB565;
            break;
        case PIXFORMAT_YUV422: {
            fb->pixfmt = PIXFORMAT_YUV;
            fb->subfmt_id = csi->yuv_format;
            fb->pixfmt = imlib_yuv_shift(fb->pixfmt, fb->x);
            break;
            case PIXFORMAT_BAYER:
                fb->pixfmt = PIXFORMAT_BAYER;
                fb->subfmt_id = csi->cfa_format;
                fb->pixfmt = imlib_bayer_shift(fb->pixfmt, fb->x, fb->y, csi->transpose);
                break;
        }
        default:
            break;
    }

    // Initialize a frame using the frame buffer.
    framebuffer_init_image(fb, dst_image);

    // Set the frame's pixel format to bayer for raw sensors.
    if (csi->raw_output && csi->pixformat != PIXFORMAT_BAYER) {
        dst_image->pixfmt = PIXFORMAT_BAYER;
        dst_image->subfmt_id = csi->cfa_format;
        dst_image->pixfmt = imlib_bayer_shift(dst_image->pixfmt, fb->x, fb->y, csi->transpose);
    }

    // Crop first to reduce the frame size before debayering.
    if (omv_csi_get_cropped()) {
        image_t src_cimage = *dst_image;
        image_t dst_cimage = *dst_image;

        src_cimage.w = resolution[csi->framesize][0];
        src_cimage.h = resolution[csi->framesize][1];

        // Offset the pixels buffer for the debayer code.
        if (csi->pixformat == PIXFORMAT_RGB565) {
            src_cimage.pixels += omv_csi_get_fb_offset(csi);
            dst_cimage.pixels += omv_csi_get_fb_offset(csi);
        }

        rectangle_t srect = { fb->x, fb->y, fb->u, fb->v };
        rectangle_t drect = { 0, 0, fb->u, fb->v };
        if (omv_gpu_draw_image(&src_cimage, &srect, &dst_cimage, &drect, 255, NULL, NULL, 0) != 0) {
            return OMV_CSI_ERROR_IO_ERROR;
        }
    }

    // Debayer the frame to match the target pixel format.
    if (csi->raw_output && csi->pixformat != PIXFORMAT_BAYER) {
        image_t src_image = *dst_image;

        // Offset the pixels buffer for the debayer code.
        if (csi->pixformat == PIXFORMAT_RGB565) {
            src_image.pixels += omv_csi_get_fb_offset(csi);
        }

        // Set the target pixel format before debayer.
        dst_image->pixfmt = fb->pixfmt;

        // Update AWB stats every n frames.
        if ((frames++ % 100) == 0) {
            omv_csi_ioctl(OMV_CSI_IOCTL_GET_RGB_STATS, &r_stat, &gb_stat, &gr_stat, &b_stat);
        }

        // Debayer frame.
        imlib_debayer_image_awb(dst_image, &src_image, false, r_stat, (gb_stat + gr_stat) / 2, b_stat);
    }
    return 0;
}

void CAM_IRQHandler(void) {
    uint32_t mask = 0;
    CPI_Type *cpi = cpi_get_base_addr(&csi);
    uint32_t status = cpi_get_interrupt_status(cpi);

    if (status & CAM_INTR_VSYNC) {
        mask |= CAM_INTR_VSYNC;
    }

    if (status & CAM_INTR_HSYNC) {
        mask |= CAM_INTR_HSYNC;
    }

    if (status & CAM_INTR_INFIFO_OVERRUN) {
        mask |= CAM_INTR_INFIFO_OVERRUN;
        omv_csi_abort(&csi, true, true);
        printf("INFIFO_OVERRUN\n");
    }

    if (status & CAM_INTR_OUTFIFO_OVERRUN) {
        mask |= CAM_INTR_OUTFIFO_OVERRUN;
        omv_csi_abort(&csi, true, true);
        printf("OUTFIFO_OVERRUN\n");
    }

    if (status & CAM_INTR_BRESP_ERR) {
        mask |= CAM_INTR_BRESP_ERR;
        omv_csi_abort(&csi, true, true);
        printf("BRESP_ERR %lu\n", cpi->CAM_AXI_ERR_STAT);
    }

    if (status & CAM_INTR_STOP) {
        mask |= CAM_INTR_STOP;
        cpi->CAM_CTRL = 0;
        if (!(status & CPI_ERROR_FLAGS)) {
            // Release the current framebuffer.
            framebuffer_get_tail(csi.fb, FB_NO_FLAGS);
        }

        // Get the current framebuffer (or new tail).
        vbuffer_t *buffer = framebuffer_get_tail(csi.fb, FB_PEEK);

        if (buffer != NULL) {
            cpi->CAM_CTRL = 0;
            cpi->CAM_CTRL |= CAM_CTRL_SW_RESET;
            cpi->CAM_FRAME_ADDR = LocalToGlobal(buffer->data + omv_csi_get_fb_offset(&csi));
            cpi_irq_handler_clear_intr_status(cpi, mask);
            cpi->CAM_CTRL = (CAM_CTRL_SNAPSHOT | CAM_CTRL_START | CAM_CTRL_FIFO_CLK_SEL);
        }

        if (!(status & CPI_ERROR_FLAGS)) {
            if (csi.frame_callback) {
                csi.frame_callback();
            }
        }
    }

    // Clear interrupts.
    cpi_irq_handler_clear_intr_status(cpi, mask);
}
#endif // MICROPY_PY_CSI
