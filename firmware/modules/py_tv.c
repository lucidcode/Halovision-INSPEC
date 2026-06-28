/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2013-2024 OpenMV, LLC.
 * Copyright (c) 2019-2019 Kaizhi Wong <kidswong999@gmail.com>
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
 * TV Shield Python module.
 */
#include "board_config.h"

#if MICROPY_PY_DISPLAY && MICROPY_PY_TV

#include "py/obj.h"
#include "py/nlr.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "py_display.h"
#include "py_helper.h"
#include "py_image.h"
#include "omv_gpio.h"
#include "omv_spi.h"
#include "umalloc.h"

#define TV_WIDTH      352
#define TV_HEIGHT     240
#define TV_REFRESH    60

#if ((TV_WIDTH) % 2)
#error "TV_WIDTH not even"
#endif

#if ((TV_HEIGHT) % 2)
#error "TV_HEIGHT not even"
#endif

#ifdef OMV_SPI_DISPLAY_CONTROLLER
/////////////////////////////////////////////////////////////
// http://www.vsdsp-forum.com/phpbb/viewtopic.php?f=14&t=1801
/////////////////////////////////////////////////////////////

// Crystal frequency in MHZ (float, observe accuracy)
#define XTAL_MHZ                     3.579545

// Line length in microseconds (float, observe accuracy)
#define LINE_LENGTH_US               63.556

#define FIXED_VCLK_CYCLES            10
#define FIXED_CSCLK_CYCLES           ((FIXED_VCLK_CYCLES) / 8.0)

// Normal visible picture line sync length is 4.7 us
#define SYNC_US                      4.7
#define SYNC                         ((uint16_t) (((SYNC_US) *(XTAL_MHZ)) - (FIXED_CSCLK_CYCLES) +0.5))

// Color burst starts at 5.3 us
#define BURST_US                     5.3
#define BURST                        ((uint16_t) (((BURST_US) *(XTAL_MHZ)) - (FIXED_CSCLK_CYCLES) +0.5))

// Color burst duration is 2.5 us
#define BURST_DUR_US                 2.5
#define BURST_DUR                    ((uint16_t) (((BURST_DUR_US) *(XTAL_MHZ)) + 0.5))

// Black video starts at 9.4 us
#define BLACK_US                     9.4
#define BLACK                        ((uint16_t) (((BLACK_US) *(XTAL_MHZ)) - (FIXED_CSCLK_CYCLES) +0.5))

// Black video duration is 52.656 us
#define BLACK_DUR_US                 52.656
#define BLACK_DUR                    ((uint16_t) (((BLACK_DUR_US) *(XTAL_MHZ)) + 0.5))

// Define NTSC video timing constants
// NTSC short sync duration is 2.3 us
#define SHORT_SYNC_US                2.3

// For the start of the line, the first 10 extra PLLCLK sync (0) cycles are subtracted.
#define SHORTSYNC                    ((uint16_t) (((SHORT_SYNC_US) *(XTAL_MHZ)) - (FIXED_CSCLK_CYCLES) +0.5))

// For the middle of the line the whole duration of sync pulse is used.
#define SHORTSYNCM                   ((uint16_t) (((SHORT_SYNC_US) *(XTAL_MHZ)) + 0.5))

// NTSC long sync duration is 27.078 us
#define LONG_SYNC_US                 27.078
#define LONGSYNC                     ((uint16_t) (((LONG_SYNC_US) *(XTAL_MHZ)) - (FIXED_CSCLK_CYCLES) +0.5))
#define LONGSYNCM                    ((uint16_t) (((LONG_SYNC_US) *(XTAL_MHZ)) + 0.5))

// Number of lines used after the VSYNC but before visible area.
#define VSYNC_LINES                  9
#define FRONT_PORCH_LINES            13

// Definitions for picture lines
// On which line the picture area begins, the Y direction.
#define STARTLINE                    ((VSYNC_LINES) + (FRONT_PORCH_LINES))

// Frame length in lines (visible lines + nonvisible lines)
// Amount has to be odd for NTSC and RGB colors
#define TOTAL_LINES                  ((STARTLINE) + (TV_HEIGHT) +1)
#if ((TOTAL_LINES) != 263)
#error "Progressive NTSC must have 263 lines!"
#endif

// Width, in PLL clocks, of each pixel.
#define PLLCLKS_PER_PIXEL            4

// The first pixel of the picture area, the X direction.
#define STARTPIX                     ((BLACK) +7)

// The last pixel of the picture area.
#define ENDPIX                       ((uint16_t) ((STARTPIX) + (((PLLCLKS_PER_PIXEL) *(TV_WIDTH)) / 8)))

// Reserve memory for this number of different prototype lines
// (prototype lines are used for sync timing, porch and border area)
#define PROTOLINES                   3

// PLL frequency
#define PLL_MHZ                      ((XTAL_MHZ) * 8)

// 10 first pllclks, which are not in the counters are decremented here
#define PLLCLKS_PER_LINE             ((uint16_t) (((LINE_LENGTH_US) *(PLL_MHZ)) - (FIXED_VCLK_CYCLES)))

// 10 first pllclks, which are not in the counters are decremented here
#define COLORCLKS_PER_LINE           ((uint16_t) ((((((LINE_LENGTH_US) *(PLL_MHZ)) / 1) + 7) / 8) - (FIXED_CSCLK_CYCLES)))
#define COLORCLKS_LINE_HALF          ((uint16_t) ((((((LINE_LENGTH_US) *(PLL_MHZ)) / 2) + 7) / 8) - (FIXED_CSCLK_CYCLES)))

#define PROTO_AREA_WORDS             ((COLORCLKS_PER_LINE) *(PROTOLINES))
#define INDEX_START_LONGWORDS        (((PROTO_AREA_WORDS) +1) / 2)
#define INDEX_START_BYTES            ((INDEX_START_LONGWORDS) * 4)

// Protoline 0 starts always at address 0
#define PROTOLINE_BYTE_ADDRESS(n)    ((COLORCLKS_PER_LINE) * 2 * (n))
#define PROTOLINE_WORD_ADDRESS(n)    ((COLORCLKS_PER_LINE) * 1 * (n))

// Calculate picture lengths in pixels and bytes, coordinate areas for picture area
#define PICBITS                      12
#define PICLINE_LENGTH_BYTES         (((TV_WIDTH) *(PICBITS)) / 8)

#define LINE_INDEX_BYTE_SIZE         3

// Picture area memory start point
#define PICLINE_START                ((INDEX_START_BYTES) + ((TOTAL_LINES) *(LINE_INDEX_BYTE_SIZE)))

// Picture area line start addresses
#define PICLINE_BYTE_ADDRESS(n)      ((PICLINE_START) + ((PICLINE_LENGTH_BYTES) *(n)))

// Pattern generator microcode
// ---------------------------
// Bits 7:6  a=00|b=01|y=10|-=11
// Bits 5:3  n pick bits 1..8
// bits 2:0  shift 0..6
#define PICK_A                       (0 << 6)
#define PICK_B                       (1 << 6)
#define PICK_Y                       (2 << 6)
#define PICK_NOTHING                 (3 << 6)
#define PICK_BITS(a)                 (((a) - 1) << 3)
#define SHIFT_BITS(a)                (a)

// 16 bits per pixel, U4 V4 Y8
// PICK_B is U
#define OP1                          (PICK_B + PICK_BITS(4) + SHIFT_BITS(4))
// PICK_A is V
#define OP2                          (PICK_A + PICK_BITS(4) + SHIFT_BITS(4))
#define OP3                          (PICK_Y + PICK_BITS(8) + SHIFT_BITS(6))
#define OP4                          (PICK_NOTHING + SHIFT_BITS(2))

// General VS23 commands
#define WRITE_STATUS                 0x01
#define WRITE_SRAM                   0x02
#define WRITE_GPIO                   0x82
#define READ_GPIO                    0x84
#define WRITE_MULTIIC                0xb8
#define WRITE_BLOCKMVC1              0x34

// Bit definitions
#define VDCTRL1                      0x2B
#define VDCTRL1_UVSKIP               (1 << 0)
#define VDCTRL1_PLL_ENABLE           (1 << 12)
#define VDCTRL2                      0x2D
#define VDCTRL2_LINECOUNT            (1 << 0)
#define VDCTRL2_PIXEL_WIDTH          (1 << 10)
#define VDCTRL2_ENABLE_VIDEO         (1 << 15)
#define BLOCKMVC1_PYF                (1 << 4)

// VS23 video commands
#define PROGRAM                      0x30
#define PICSTART                     0x28
#define PICEND                       0x29
#define LINELEN                      0x2a
#define INDEXSTART                   0x2c

// Sync, blank, burst and white level definitions, here are several options
// These are for proto lines and so format is VVVVUUUUYYYYYYYY

// Sync is always 0
#define SYNC_LEVEL                   0x0000

// 285 mV to 75 ohm load
#define BLANK_LEVEL                  0x0066

// 285 mV burst
#define BURST_LEVEL                  0x0d66

#define SPI_RAM_SIZE                 (128 * 1024)

// COLORCLKS_PER_LINE can't be used in pre-processor logic.
#if ((((((227 * (PROTOLINES)) + 1) / 2) * 4) + ((TOTAL_LINES) *(LINE_INDEX_BYTE_SIZE)) + \
    ((PICLINE_LENGTH_BYTES) *(TV_HEIGHT))) > (SPI_RAM_SIZE))
#error "TV_WIDTH * TV_HEIGHT is too big!"
#endif

#define TV_BAUDRATE                  (TV_WIDTH * TV_HEIGHT * TV_REFRESH * PICBITS)

#if OMV_SPI_DISPLAY_TRIPLE_BUFFER
#define TV_TRIPLE_BUFFER_DEFAULT   (true)
#else
#define TV_TRIPLE_BUFFER_DEFAULT   (false)
#endif

static void SpiTransmitReceivePacket(py_display_obj_t *self, uint8_t *txdata, uint8_t *rxdata,
                                     uint16_t size, bool end) {
    omv_spi_transfer_t spi_xfer = {
        .txbuf = txdata,
        .rxbuf = rxdata,
        .size = size,
        .timeout = OMV_SPI_MAX_TIMEOUT,
        .flags = OMV_SPI_XFER_BLOCKING
    };

    omv_gpio_write(OMV_SPI_DISPLAY_SSEL_PIN, 0);
    omv_spi_transfer_start(&self->spi_bus, &spi_xfer);

    if (end) {
        omv_gpio_write(OMV_SPI_DISPLAY_SSEL_PIN, 1);
    }
}

static void SpiRamWriteByteRegister(py_display_obj_t *self, int opcode, int data) {
    uint8_t packet[2] = {opcode, data};
    SpiTransmitReceivePacket(self, packet, NULL, sizeof(packet), true);
}

static int SpiRamReadByteRegister(py_display_obj_t *self, int opcode) {
    uint8_t packet[2] = {opcode, 0};
    SpiTransmitReceivePacket(self, packet, packet, sizeof(packet), true);
    return packet[1];
}

static void SpiRamWriteWordRegister(py_display_obj_t *self, int opcode, int data) {
    uint8_t packet[3] = {opcode, data >> 8, data};
    SpiTransmitReceivePacket(self, packet, NULL, sizeof(packet), true);
}

static void SpiClearRam(py_display_obj_t *self) {
    uint8_t packet[4] = {WRITE_SRAM, 0, 0, 0};
    SpiTransmitReceivePacket(self, packet, NULL, sizeof(packet), false);
    packet[0] = 0;

    for (int i = 0; i < (SPI_RAM_SIZE / sizeof(packet)); i++) {
        SpiTransmitReceivePacket(self, packet, NULL, sizeof(packet), (i + 1) == (SPI_RAM_SIZE / sizeof(packet)));
    }
}

static void SpiRamWriteProgram(py_display_obj_t *self, int data0, int data1, int data2, int data3) {
    uint8_t packet[5] = {PROGRAM, data3, data2, data1, data0};
    SpiTransmitReceivePacket(self, packet, NULL, sizeof(packet), true);
}

static void SpiRamWriteLowPassFilter(py_display_obj_t *self, int data) {
    uint8_t packet[6] = {WRITE_BLOCKMVC1, 0, 0, 0, 0, data};
    SpiTransmitReceivePacket(self, packet, NULL, sizeof(packet), true);
}

static void SpiRamWriteWord(py_display_obj_t *self, int w_address, int data) {
    int address = w_address * sizeof(uint16_t);
    uint8_t packet[6] = {WRITE_SRAM, address >> 16, address >> 8, address, data >> 8, data};
    SpiTransmitReceivePacket(self, packet, NULL, sizeof(packet), true);
}

static void SpiRamWriteVSyncProtoLine(py_display_obj_t *self, int line, int length_1, int length_2) {
    int w0 = PROTOLINE_WORD_ADDRESS(line);
    for (int i = 0; i < COLORCLKS_PER_LINE; i++) {
        SpiRamWriteWord(self, w0++, BLANK_LEVEL);
    }

    int w1 = PROTOLINE_WORD_ADDRESS(line);
    for (int i = 0; i < length_1; i++) {
        SpiRamWriteWord(self, w1++, SYNC_LEVEL);
    }

    int w2 = PROTOLINE_WORD_ADDRESS(line) + COLORCLKS_LINE_HALF;
    for (int i = 0; i < length_2; i++) {
        SpiRamWriteWord(self, w2++, SYNC_LEVEL);
    }
}

static void SpiRamWriteLine(py_display_obj_t *self, int line, int index) {
    int address = INDEX_START_BYTES + (line * LINE_INDEX_BYTE_SIZE);
    int data = index << 7;
    uint8_t packet[7] = {WRITE_SRAM, address >> 16, address >> 8, address, data, data >> 8, data >> 16};
    SpiTransmitReceivePacket(self, packet, NULL, sizeof(packet), true);
}

static void SpiRamVideoInit(py_display_obj_t *self) {
    // Select the first VS23 for following commands in case there
    // are several VS23 ICs connected to same SPI bus.
    SpiRamWriteByteRegister(self, WRITE_MULTIIC, 0xe);

    // Set SPI memory address autoincrement
    SpiRamWriteByteRegister(self, WRITE_STATUS, 0x40);

    // Reset the video display controller
    SpiRamWriteWordRegister(self, VDCTRL1, 0);
    SpiRamWriteWordRegister(self, VDCTRL2, 0);

    // Write picture start and end
    SpiRamWriteWordRegister(self, PICSTART, (STARTPIX - 1));
    SpiRamWriteWordRegister(self, PICEND, (ENDPIX - 1));

    // Enable PLL clock
    SpiRamWriteWordRegister(self, VDCTRL1, VDCTRL1_PLL_ENABLE | VDCTRL1_UVSKIP);

    // Clear the video memory
    SpiClearRam(self);

    // Set length of one complete line (unit: PLL clocks)
    SpiRamWriteWordRegister(self, LINELEN, PLLCLKS_PER_LINE);

    // Set microcode program for picture lines
    SpiRamWriteProgram(self, OP1, OP2, OP3, OP4);

    // Define where Line Indexes are stored in memory
    SpiRamWriteWordRegister(self, INDEXSTART, INDEX_START_LONGWORDS);

    // At this time, the chip would continuously output the proto line 0.
    // This protoline will become our most "normal" horizontal line.
    // For TV-Out, fill the line with black level,
    // and insert a few pixels of sync level (0) and color burst to the beginning.
    // Note that the chip hardware adds black level to all nonproto areas so
    // protolines and normal picture have different meaning for the same Y value.
    // In protolines, Y=0 is at sync level and in normal picture Y=0 is at black level (offset +102).

    // In protolines, each pixel is 8 PLLCLKs, which in TV-out modes means one color
    // subcarrier cycle. Each pixel has 16 bits (one word): VVVVUUUUYYYYYYYY.

    SpiRamWriteVSyncProtoLine(self, 0, SYNC, 0);

    int w = PROTOLINE_WORD_ADDRESS(0) + BURST;
    for (int i = 0; i < BURST_DUR; i++) {
        SpiRamWriteWord(self, w++, BURST_LEVEL);
    }

    // short_low + long_high + short_low + long_high
    SpiRamWriteVSyncProtoLine(self, 1, SHORTSYNC, SHORTSYNCM);

    // long_low + short_high + long_low + short_high
    SpiRamWriteVSyncProtoLine(self, 2, LONGSYNC, LONGSYNCM);

    for (int i = 0; i <= 2; i++) {
        SpiRamWriteLine(self, i, PROTOLINE_BYTE_ADDRESS(1)); // short_low + long_high + short_low + long_high
    }

    for (int i = 3; i <= 5; i++) {
        SpiRamWriteLine(self, i, PROTOLINE_BYTE_ADDRESS(2)); // long_low + short_high + long_low + short_high
    }

    for (int i = 6; i <= 8; i++) {
        SpiRamWriteLine(self, i, PROTOLINE_BYTE_ADDRESS(1)); // short_low + long_high + short_low + long_high
    }

    // Set pic line indexes to point to protoline 0 and their individual picture line.
    for (int i = 0; i < TV_HEIGHT; i++) {
        SpiRamWriteLine(self, STARTLINE + i, PICLINE_BYTE_ADDRESS(i));
    }

    // Set number of lines, length of pixel and enable video generation
    SpiRamWriteWordRegister(self, VDCTRL2, (VDCTRL2_LINECOUNT * (TOTAL_LINES - 1))
                            | (VDCTRL2_PIXEL_WIDTH * (PLLCLKS_PER_PIXEL - 1))
                            | (VDCTRL2_ENABLE_VIDEO));

    // Enable the low-pass Y filter.
    SpiRamWriteLowPassFilter(self, BLOCKMVC1_PYF);
}
#endif // OMV_SPI_DISPLAY_CONTROLLER

// TV lines are converted from 16-bit RGB565 to 12-bit YUV.
#define TV_WIDTH_RGB565      ((TV_WIDTH) * 2) // bytes

#if ((PICLINE_LENGTH_BYTES) > (TV_WIDTH_RGB565))
#error "PICLINE_LENGTH_BYTES > TV_WIDTH_RGB565"
#endif

#ifdef OMV_SPI_DISPLAY_CONTROLLER
enum {
    TV_IDLE = 0,
    TV_WRITE_CMD,
    TV_WRITE,
};

static const uint8_t write_sram[] = {
    // Cannot be allocated on the stack.
    WRITE_SRAM,
    (uint8_t) (PICLINE_BYTE_ADDRESS(0) >> 16),
    (uint8_t) (PICLINE_BYTE_ADDRESS(0) >> 8),
    (uint8_t) (PICLINE_BYTE_ADDRESS(0) >> 0)
};

static void spi_tv_callback(omv_spi_t *spi, void *userdata, void *buf) {
    py_display_obj_t *self = (py_display_obj_t *) userdata;

    switch (self->spi_state) {
        case TV_WRITE_CMD: {
            omv_gpio_write(OMV_SPI_DISPLAY_SSEL_PIN, 1);
            self->spi_state = TV_WRITE;
            self->spi_write_addr = (uint8_t *) self->framebuffers[self->framebuffer_head];
            self->spi_write_count = PICLINE_LENGTH_BYTES * TV_HEIGHT;
            self->framebuffer_tail = self->framebuffer_head;
            omv_gpio_write(OMV_SPI_DISPLAY_SSEL_PIN, 0);
            // When starting the interrupt chain the first transfer is not executed
            // in interrupt context. So, disable interrupts for the first transfer so
            // that it completes first and unlocks the SPI bus before allowing the interrupt
            // it causes to trigger starting the interrupt chain.
            omv_spi_transfer_t spi_xfer = {
                .txbuf = (uint8_t *) write_sram,
                .size = sizeof(write_sram),
                .flags = OMV_SPI_XFER_NONBLOCK,
                .userdata = self,
                .callback = spi_tv_callback,
            };
            uint32_t irq_state = MICROPY_BEGIN_ATOMIC_SECTION();
            omv_spi_transfer_start(&self->spi_bus, &spi_xfer);
            MICROPY_END_ATOMIC_SECTION(irq_state);
            break;
        }
        case TV_WRITE: {
            uint8_t *addr = self->spi_write_addr;
            size_t count = IM_MIN(self->spi_write_count, OMV_SPI_MAX_8BIT_XFER);
            self->spi_state = (self->spi_write_count > OMV_SPI_MAX_8BIT_XFER) ? TV_WRITE : TV_WRITE_CMD;
            self->spi_write_addr += count;
            self->spi_write_count -= count;
            omv_spi_transfer_t spi_xfer = {
                .txbuf = addr,
                .size = count,
                .flags = OMV_SPI_XFER_DMA,
                .userdata = self,
                .callback = spi_tv_callback,
            };
            omv_spi_transfer_start(&self->spi_bus, &spi_xfer);
            break;
        }
        default: {
            break;
        }
    }
}

// Convert a 8-bit Grayscale line of pixels to 12-bit YUV422 with padding (line is 16-bit per pixel).
static void spi_tv_draw_image_cb_convert_grayscale(uint8_t *row_pointer_i, uint8_t *row_pointer_o) {
    for (int i = TV_WIDTH - 2, j = ((TV_WIDTH * 3) / 2) - 3; i >= 0; i -= 2, j -= 3) {
        int y0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_pointer_i, i);
        int y1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_pointer_i, i + 1);
        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_pointer_o, j, 0);
        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_pointer_o, j + 1, y0);
        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_pointer_o, j + 2, y1);
    }
}

// Convert a 16-bit RGB565 line of pixels to 12-bit YUV422 with padding (line is 16-bit per pixel).
static void spi_tv_draw_image_cb_convert_rgb565(uint16_t *row_pointer_i, uint8_t *row_pointer_o) {
    for (int i = 0, j = 0; i < TV_WIDTH; i += 2, j += 3) {
        #if defined(ARM_MATH_DSP)

        int pixels = *((uint32_t *) (row_pointer_i + i));
        int r_pixels = ((pixels >> 8) & 0xf800f8) | ((pixels >> 13) & 0x70007);
        int g_pixels = ((pixels >> 3) & 0xfc00fc) | ((pixels >> 9) & 0x30003);
        int b_pixels = ((pixels << 3) & 0xf800f8) | ((pixels >> 2) & 0x70007);

        int y = ((r_pixels * 38) + (g_pixels * 75) + (b_pixels * 15)) >> 7;
        int u = __SSUB16(b_pixels * 64, (r_pixels * 21) + (g_pixels * 43));
        int v = __SSUB16(r_pixels * 64, (g_pixels * 54) + (b_pixels * 10));

        int y0 = __UXTB_RORn(y, 0), y1 = __UXTB_RORn(y, 16);

        int u_avg = __SMUAD(u, 0x00010001) >> 7;
        int v_avg = __SMUAD(v, 0x00010001) >> 7;

        #else

        int pixel0 = IMAGE_GET_RGB565_PIXEL_FAST(row_pointer_i, i);
        int r0 = COLOR_RGB565_TO_R8(pixel0);
        int g0 = COLOR_RGB565_TO_G8(pixel0);
        int b0 = COLOR_RGB565_TO_B8(pixel0);
        int y0 = COLOR_RGB888_TO_Y(r0, g0, b0);
        int u0 = COLOR_RGB888_TO_U(r0, g0, b0);
        int v0 = COLOR_RGB888_TO_V(r0, g0, b0);

        int pixel1 = IMAGE_GET_RGB565_PIXEL_FAST(row_pointer_i, i + 1);
        int r1 = COLOR_RGB565_TO_R8(pixel1);
        int g1 = COLOR_RGB565_TO_G8(pixel1);
        int b1 = COLOR_RGB565_TO_B8(pixel1);
        int y1 = COLOR_RGB888_TO_Y(r1, g1, b1);
        int u1 = COLOR_RGB888_TO_U(r1, g1, b1);
        int v1 = COLOR_RGB888_TO_V(r1, g1, b1);

        int u_avg = u0 + u1;
        int v_avg = v0 + v1;

        #endif

        int uv = ((u_avg >> 1) & 0xf0) | (((-v_avg) >> 5) & 0xf);

        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_pointer_o, j, uv);
        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_pointer_o, j + 1, y0);
        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_pointer_o, j + 2, y1);
    }
}

static void spi_tv_draw_image_cb_grayscale(int x_start, int x_end, int y_row, imlib_draw_row_data_t *data) {
    py_display_obj_t *self = (py_display_obj_t *) data->callback_arg;
    memset(((uint8_t *) data->dst_row_override) + x_end, 0, TV_WIDTH - x_end); // clear trailing bytes.
    spi_tv_draw_image_cb_convert_grayscale((uint8_t *) data->dst_row_override, (uint8_t *) data->dst_row_override);
    SpiTransmitReceivePacket(self, data->dst_row_override, NULL, PICLINE_LENGTH_BYTES, false);
}

static void spi_tv_draw_image_cb_rgb565(int x_start, int x_end, int y_row, imlib_draw_row_data_t *data) {
    py_display_obj_t *self = (py_display_obj_t *) data->callback_arg;
    memset(data->dst_row_override, 0, x_start * sizeof(uint16_t)); // clear leading bytes.
    spi_tv_draw_image_cb_convert_rgb565((uint16_t *) data->dst_row_override, (uint8_t *) data->dst_row_override);
    SpiTransmitReceivePacket(self, data->dst_row_override, NULL, PICLINE_LENGTH_BYTES, false);
}

static void spi_tv_display(py_display_obj_t *self, image_t *src_img, int dst_x_start, int dst_y_start,
                           float x_scale, float y_scale, rectangle_t *roi, int rgb_channel, int alpha,
                           const uint16_t *color_palette, const uint8_t *alpha_palette, image_hint_t hint) {
    bool rgb565 = ((rgb_channel == -1) && src_img->is_color) || color_palette;
    imlib_draw_row_callback_t cb = rgb565 ? spi_tv_draw_image_cb_rgb565 : spi_tv_draw_image_cb_grayscale;

    image_t dst_img;
    dst_img.w = TV_WIDTH;
    dst_img.h = TV_HEIGHT;
    dst_img.pixfmt = rgb565 ? PIXFORMAT_RGB565 : PIXFORMAT_GRAYSCALE;

    point_t p0, p1;
    imlib_draw_image_get_bounds(&dst_img, src_img, dst_x_start, dst_y_start, x_scale, y_scale,
                                roi, alpha, alpha_palette, hint, &p0, &p1);
    bool black = p0.x == -1;

    if (!self->triple_buffer) {
        dst_img.data = uma_calloc(TV_WIDTH_RGB565, UMA_FAST);

        SpiTransmitReceivePacket(self, (uint8_t *) write_sram, NULL, sizeof(write_sram), false);

        if (black) {
            // zero the whole image
            for (int i = 0; i < TV_HEIGHT; i++) {
                SpiTransmitReceivePacket(self, dst_img.data, NULL, PICLINE_LENGTH_BYTES, false);
            }
        } else {
            // Zero the top rows
            for (int i = 0; i < p0.y; i++) {
                SpiTransmitReceivePacket(self, dst_img.data, NULL, PICLINE_LENGTH_BYTES, false);
            }

            // Transmits left/right parts already zeroed...
            imlib_draw_image(&dst_img, src_img, dst_x_start, dst_y_start, x_scale, y_scale, roi,
                             rgb_channel, alpha, color_palette, alpha_palette, hint | IMAGE_HINT_BLACK_BACKGROUND,
                             NULL, cb, self, dst_img.data);

            // Zero the bottom rows
            if (p1.y < TV_HEIGHT) {
                memset(dst_img.data, 0, TV_WIDTH_RGB565);
            }

            for (int i = p1.y; i < TV_HEIGHT; i++) {
                SpiTransmitReceivePacket(self, dst_img.data, NULL, PICLINE_LENGTH_BYTES, false);
            }
        }

        omv_gpio_write(OMV_SPI_DISPLAY_SSEL_PIN, 1);
        uma_free(dst_img.data);
    } else {
        // For triple buffering we are never drawing where head or tail (which may instantly update to
        // to be equal to head) is.
        int new_framebuffer_head = (self->framebuffer_head + 1) % FRAMEBUFFER_COUNT;
        if (new_framebuffer_head == self->framebuffer_tail) {
            new_framebuffer_head = (new_framebuffer_head + 1) % FRAMEBUFFER_COUNT;
        }

        dst_img.data = (uint8_t *) self->framebuffers[new_framebuffer_head];

        if (rgb565) {
            if (black) {
                // zero the whole image
                memset(dst_img.data, 0, TV_WIDTH * TV_HEIGHT * sizeof(uint16_t));
            } else {
                // Zero the top rows
                if (p0.y) {
                    memset(dst_img.data, 0, TV_WIDTH * p0.y * sizeof(uint16_t));
                }

                if (p0.x) {
                    for (int i = p0.y; i < p1.y; i++) {
                        // Zero left
                        memset(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&dst_img, i), 0, p0.x * sizeof(uint16_t));
                    }
                }

                imlib_draw_image(&dst_img, src_img, dst_x_start, dst_y_start, x_scale, y_scale, roi,
                                 rgb_channel, alpha, color_palette, alpha_palette, hint | IMAGE_HINT_BLACK_BACKGROUND,
                                 NULL, NULL, NULL, NULL);

                if (TV_WIDTH - p1.x) {
                    for (int i = p0.y; i < p1.y; i++) {
                        // Zero right
                        memset(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&dst_img, i) + p1.x, 0,
                               (TV_WIDTH - p1.x) * sizeof(uint16_t));
                    }
                }

                // Zero the bottom rows
                if (TV_HEIGHT - p1.y) {
                    memset(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&dst_img, p1.y), 0,
                           TV_WIDTH * (TV_HEIGHT - p1.y) * sizeof(uint16_t));
                }
            }

            for (int i = 0; i < TV_HEIGHT; i++) {
                // Convert the image.
                spi_tv_draw_image_cb_convert_rgb565(IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(&dst_img, i),
                                                    dst_img.data + (PICLINE_LENGTH_BYTES * i));
            }
        } else {
            if (black) {
                // zero the whole image
                memset(dst_img.data, 0, TV_WIDTH * TV_HEIGHT * sizeof(uint8_t));
            } else {
                // Zero the top rows
                if (p0.y) {
                    memset(dst_img.data, 0, TV_WIDTH * p0.y * sizeof(uint8_t));
                }

                if (p0.x) {
                    for (int i = p0.y; i < p1.y; i++) {
                        // Zero left
                        memset(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&dst_img, i), 0, p0.x * sizeof(uint8_t));
                    }
                }

                imlib_draw_image(&dst_img, src_img, dst_x_start, dst_y_start, x_scale, y_scale, roi,
                                 rgb_channel, alpha, color_palette, alpha_palette, hint | IMAGE_HINT_BLACK_BACKGROUND,
                                 NULL, NULL, NULL, NULL);

                if (TV_WIDTH - p1.x) {
                    for (int i = p0.y; i < p1.y; i++) {
                        // Zero right
                        memset(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&dst_img, i) + p1.x, 0,
                               (TV_WIDTH - p1.x) * sizeof(uint8_t));
                    }
                }

                // Zero the bottom rows
                if (TV_HEIGHT - p1.y) {
                    memset(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&dst_img, p1.y), 0,
                           TV_WIDTH * (TV_HEIGHT - p1.y) * sizeof(uint8_t));
                }
            }

            for (int i = TV_HEIGHT - 1; i >= 0; i--) {
                // Convert the image.
                spi_tv_draw_image_cb_convert_grayscale(IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(&dst_img, i),
                                                       dst_img.data + (PICLINE_LENGTH_BYTES * i));
            }
        }

        #ifdef __DCACHE_PRESENT
        // Flush data for DMA
        SCB_CleanDCache_by_Addr((uint32_t *) dst_img.data, image_size(&dst_img));
        #endif

        // Update head which means a new image is ready.
        self->framebuffer_head = new_framebuffer_head;

        // Kick off an update of the display.
        if (self->spi_state == TV_IDLE) {
            self->spi_state = TV_WRITE_CMD;
            spi_tv_callback(&self->spi_bus, self, NULL);
        }
    }
}
#endif // OMV_SPI_DISPLAY_CONTROLLER

static void tv_display_write(py_display_obj_t *self, image_t *src_img, int dst_x_start, int dst_y_start,
                             float x_scale, float y_scale, rectangle_t *roi, int rgb_channel, int alpha,
                             const uint16_t *color_palette, const uint8_t *alpha_palette, image_hint_t hint) {
    #ifdef OMV_SPI_DISPLAY_CONTROLLER
    spi_tv_display(self, src_img, dst_x_start, dst_y_start, x_scale, y_scale, roi,
                   rgb_channel, alpha, color_palette, alpha_palette, hint);
    #endif
}

static void tv_display_clear(py_display_obj_t *self, bool display_off) {
    #ifdef OMV_SPI_DISPLAY_CONTROLLER
    spi_tv_display(self, NULL, 0, 0, 1.f, 1.f, NULL, 0, 0, NULL, NULL, 0);
    #endif
}

static void tv_display_deinit(py_display_obj_t *self) {
    #ifdef OMV_SPI_DISPLAY_CONTROLLER
    if (self->triple_buffer) {
        omv_spi_transfer_abort(&self->spi_bus);
        self->spi_state = TV_IDLE;
        for (int i = 0; i < FRAMEBUFFER_COUNT; i++) {
            uma_free(self->framebuffers[i]);
            self->framebuffers[i] = NULL;
        }
    }
    omv_spi_deinit(&self->spi_bus);
    #endif
}

static mp_obj_t tv_display_ioctl(py_display_obj_t *self, size_t n_args, const mp_obj_t *args) {
    #ifdef OMV_SPI_DISPLAY_CONTROLLER
    if (self->triple_buffer) {
        omv_spi_transfer_abort(&self->spi_bus);
        self->spi_state = TV_IDLE;
        omv_gpio_write(OMV_SPI_DISPLAY_SSEL_PIN, 1);
    }

    if (!n_args || mp_obj_get_int(args[0]) != DISPLAY_IOCTL_CHANNEL) {
        return mp_const_none;
    }

    if (n_args > 1) {
        int channel = mp_obj_get_int(args[1]);

        if ((channel < 1) || (8 < channel)) {
            mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Channel ranges between 1 and 8"));
        }

        SpiRamWriteByteRegister(self, WRITE_GPIO, 0x70 | (channel - 1));
        return mp_const_none;
    } else {
        #ifdef OMV_SPI_DISPLAY_RX_CLK_DIV
        omv_spi_set_baudrate(&self->spi_bus, TV_BAUDRATE / OMV_SPI_DISPLAY_RX_CLK_DIV);
        #endif
        int channel = SpiRamReadByteRegister(self, READ_GPIO);
        #ifdef OMV_SPI_DISPLAY_RX_CLK_DIV
        omv_spi_set_baudrate(&self->spi_bus, TV_BAUDRATE);
        #endif
        return mp_obj_new_int((channel & 0x7) + 1);
    }
    #else
    return mp_const_none;
    #endif
}

mp_obj_t tv_display_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_triple_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_triple_buffer, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = TV_TRIPLE_BUFFER_DEFAULT} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    py_display_obj_t *self = mp_obj_malloc_with_finaliser(py_display_obj_t, &py_tv_display_type);
    self->framebuffer_tail = 0;
    self->framebuffer_head = 0;
    self->width = TV_WIDTH;
    self->height = TV_HEIGHT;
    self->refresh = TV_REFRESH;
    self->triple_buffer = args[ARG_triple_buffer].u_bool;
    self->bgr = false;
    self->byte_swap = false;
    self->controller = mp_const_none;
    self->bl_controller = mp_const_none;

    #ifdef OMV_SPI_DISPLAY_CONTROLLER
    omv_spi_config_t spi_config;
    omv_spi_default_config(&spi_config, OMV_SPI_DISPLAY_CONTROLLER);

    self->spi_baudrate = TV_BAUDRATE;
    spi_config.baudrate = TV_BAUDRATE;
    spi_config.nss_enable = false;
    spi_config.dma_flags = self->triple_buffer ? OMV_SPI_DMA_NORMAL : 0;
    omv_spi_init(&self->spi_bus, &spi_config);

    omv_gpio_write(OMV_SPI_DISPLAY_SSEL_PIN, 1);

    SpiRamVideoInit(self);

    // Set default channel.
    SpiRamWriteByteRegister(self, WRITE_GPIO, 0x77);

    if (self->triple_buffer) {
        for (int i = 0; i < FRAMEBUFFER_COUNT; i++) {
            self->framebuffers[i] = (uint16_t *) uma_calloc(TV_WIDTH_RGB565 * TV_HEIGHT,
                                                            UMA_PERSIST | UMA_CACHE);
        }
    }
    #endif

    return MP_OBJ_FROM_PTR(self);
}

static const py_display_p_t py_tv_display_p = {
    .deinit = tv_display_deinit,
    .clear = tv_display_clear,
    .write = tv_display_write,
    .ioctl = tv_display_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    py_tv_display_type,
    MP_QSTR_TVDisplay,
    MP_TYPE_FLAG_NONE,
    make_new, tv_display_make_new,
    protocol, &py_tv_display_p,
    locals_dict, &py_display_locals_dict
    );

#endif // MICROPY_PY_DISPLAY && MICROPY_PY_TV
