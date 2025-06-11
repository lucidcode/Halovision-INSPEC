/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2013-2024 OpenMV, LLC.
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
 * OV7670 driver.
 */
#include "omv_boardconfig.h"
#if (OMV_OV7670_ENABLE == 1)

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "omv_i2c.h"
#include "omv_csi.h"
#include "ov7670.h"
#include "ov7670_regs.h"
#include "py/mphal.h"

static const uint8_t default_regs[][2] = {
    // OV7670 reference registers
    #if (OMV_OV7670_CLKRC == 0)
    { CLKRC,            CLKRC_PRESCALER_BYPASS },
    #else
    { CLKRC,            OMV_OV7670_CLKRC },
    #endif
    { TSLB,             0x04 },
    { COM7,             0x00 },
    { HSTART,           0x13 },
    { HSTOP,            0x01 },
    { HREF,             0xb6 },
    { VSTART,           0x02 },
    { VSTOP,            0x7a },
    { VREF,             0x0a },

    { COM3,             0x00 },
    { COM14,            0x00 },
    { SCALING_XSC,      0x3a },
    { SCALING_YSC,      0x35 },
    { SCALING_DCWCTR,   0x11 },
    { SCALING_PCLK_DIV, 0xf0 },
    { SCALING_PCLK,     0x02 },

    /* Gamma curve values */
    { SLOP,             0x20 },
    { GAM1,             0x10 },
    { GAM2,             0x1e },
    { GAM3,             0x35 },
    { GAM4,             0x5a },
    { GAM5,             0x69 },
    { GAM6,             0x76 },
    { GAM7,             0x80 },
    { GAM8,             0x88 },
    { GAM9,             0x8f },
    { GAM10,            0x96 },
    { GAM11,            0xa3 },
    { GAM12,            0xaf },
    { GAM13,            0xc4 },
    { GAM14,            0xd7 },
    { GAM15,            0xe8 },

    /* AGC and AEC parameters */
    { COM8,             COM8_FAST_AEC | COM8_AEC_STEP | COM8_BANDF_EN },
    { GAIN,             0x00 },
    { AECH,             0x00 },
    { COM4,             0x40 },
    { COM9,             0x18 },
    { BD50MAX,          0x05 },
    { BD60MAX,          0x07 },
    { AEW,              0x95 },
    { AEB,              0x33 },
    { VPT,              0xe3 },
    { HAECC1,           0x78 },
    { HAECC2,           0x68 },
    { 0xa1,             0x03 },
    { HAECC3,           0xd8 },
    { HAECC4,           0xd8 },
    { HAECC5,           0xf0 },
    { HAECC6,           0x90 },
    { HAECC7,           0x94 },
    { COM8,             COM8_FAST_AEC | COM8_AEC_STEP | COM8_BANDF_EN | COM8_AGC_EN | COM8_AEC_EN },

    /* Almost all of these are magic "reserved" values.  */
    { COM5,             0x61 },
    { COM6,             0x4b },
    { 0x16,             0x02 },
    { 0x21,             0x02 },
    { 0x22,             0x91 },
    { 0x29,             0x07 },
    { 0x33,             0x0b },
    { 0x35,             0x0b },
    { 0x37,             0x1d },
    { 0x38,             0x71 },
    { 0x39,             0x2a },
    { COM12,            0x78 },
    { 0x4d,             0x40 },
    { 0x4e,             0x20 },
    { GFIX,             0x00 },
    { 0x6b,             0x00 }, // PLL Bypass
    { 0x74,             0x10 },
    { 0x8d,             0x4f },
    { 0x8e,             0x00 },
    { 0x8f,             0x00 },
    { 0x90,             0x00 },
    { 0x91,             0x00 },
    { 0x96,             0x00 },
    { 0x9a,             0x00 },
    { 0xb0,             0x84 },
    { 0xb1,             0x0c },
    { 0xb2,             0x0e },
    { 0xb3,             0x82 },
    { 0xb8,             0x0a },
    { 0x43,             0x0a },
    { 0x44,             0xf0 },
    { 0x45,             0x34 },
    { 0x46,             0x58 },
    { 0x47,             0x28 },
    { 0x48,             0x3a },
    { 0x59,             0x88 },
    { 0x5a,             0x88 },
    { 0x5b,             0x44 },
    { 0x5c,             0x67 },
    { 0x5d,             0x49 },
    { 0x5e,             0x0e },
    { 0x6c,             0x0a },
    { 0x6d,             0x55 },
    { 0x6e,             0x11 },
    { 0x6f,             0x9e },
    { 0x6a,             0x40 },
    { BLUE,             0x40 },
    { RED,              0x60 },
    { COM8,             COM8_FAST_AEC | COM8_AEC_STEP | COM8_BANDF_EN | COM8_AGC_EN | COM8_AEC_EN | COM8_AWB_EN },

    /* Matrix coefficients */
    { MTX1,             0x80 },
    { MTX2,             0x80 },
    { MTX3,             0x00 },
    { MTX4,             0x22 },
    { MTX5,             0x5e },
    { MTX6,             0x80 },
    { MTXS,             0x9e },

    { COM16,            COM16_AWB_GAIN_EN },
    { EDGE,             0x00 },
    { 0x75,             0x05 },
    { 0x76,             0xe1 },
    { 0x4c,             0x00 },
    { 0x77,             0x01 },
    { COM13,            0xc3 },
    { 0x4b,             0x09 },
    { 0xc9,             0x60 },
    { COM16,            0x38 },
    { 0x56,             0x40 },

    { 0x34,             0x11 },
    { COM11,            COM11_AEC_BANDF | COM11_HZAUTO_EN },
    { 0xa4,             0x88 },
    { 0x96,             0x00 },
    { 0x97,             0x30 },
    { 0x98,             0x20 },
    { 0x99,             0x30 },
    { 0x9a,             0x84 },
    { 0x9b,             0x29 },
    { 0x9c,             0x03 },
    { 0x9d,             0x4c },
    { 0x9e,             0x3f },
    { 0x78,             0x04 },
    { 0x79,             0x01 },
    { 0xc8,             0xf0 },
    { 0x79,             0x0f },
    { 0xc8,             0x00 },
    { 0x79,             0x10 },
    { 0xc8,             0x7e },
    { 0x79,             0x0a },
    { 0xc8,             0x80 },
    { 0x79,             0x0b },
    { 0xc8,             0x01 },
    { 0x79,             0x0c },
    { 0xc8,             0x0f },
    { 0x79,             0x0d },
    { 0xc8,             0x20 },
    { 0x79,             0x09 },
    { 0xc8,             0x80 },
    { 0x79,             0x02 },
    { 0xc8,             0xc0 },
    { 0x79,             0x03 },
    { 0xc8,             0x40 },
    { 0x79,             0x05 },
    { 0xc8,             0x30 },
    { 0x79,             0x26 },

    { 0xFF,             0xFF },
};

#if (OMV_OV7670_VERSION == 75)
static const uint8_t rgb565_regs[][2] = {
    { COM7,         COM7_RGB_FMT },        /* Selects RGB mode */
    { RGB444,       0x00 },                /* No RGB444 please */
    { COM1,         0x00 },                /* CCIR601 */
    { COM15,        COM15_OUT_00_FF | COM15_FMT_RGB565},
    { COM9,         0x38 },                /* 16x gain ceiling; 0x8 is reserved bit */
    { 0x4f,         0xb3 },                /* "matrix coefficient 1" */
    { 0x50,         0xb3 },                /* "matrix coefficient 2" */
    { 0x51,         0x00 },                /* "matrix Coefficient 3" */
    { 0x52,         0x3d },                /* "matrix coefficient 4" */
    { 0x53,         0xa7 },                /* "matrix coefficient 5" */
    { 0x54,         0xe4 },                /* "matrix coefficient 6" */
    { COM13,        COM13_GAMMA_EN | COM13_UVSAT_AUTO },
    { 0xFF,         0xFF }
};
#elif (OMV_OV7670_VERSION == 70)
static const uint8_t rgb565_regs[][2] = {
    { COM7,     COM7_RGB_FMT                },    /* Selects RGB mode */
    { RGB444,   0                           },    /* No RGB444 please */
    { COM1,     0x0                         },    /* CCIR601 */
    { COM15,    COM15_FMT_RGB565 | COM15_OUT_00_FF},
    { COM9,     0x6A                        },     /* 128x gain ceiling; 0x8 is reserved bit */
    { MTX1,     0xb3                        },     /* "matrix coefficient 1" */
    { MTX2,     0xb3                        },     /* "matrix coefficient 2" */
    { MTX3,     0                           },    /* vb */
    { MTX4,     0x3d                        },     /* "matrix coefficient 4" */
    { MTX5,     0xa7                        },     /* "matrix coefficient 5" */
    { MTX6,     0xe4                        },     /* "matrix coefficient 6" */
    { COM13,    COM13_UVSAT_AUTO            },
    { 0xFF,     0xFF }
};
#else
#error "OV767x variant is Not defined."
#endif

// TODO: These registers probably need to be fixed too.
static const uint8_t yuv422_regs[][2] = {
    { COM7,         0x00 },             /* Selects YUV mode */
    { RGB444,       0x00 },                /* No RGB444 please */
    { COM1,         0x00 },                /* CCIR601 */
    { COM15,        COM15_OUT_00_FF },
    { COM9,         0x48 },             /* 32x gain ceiling; 0x8 is reserved bit */
    { 0x4f,         0x80 },                /* "matrix coefficient 1" */
    { 0x50,         0x80 },                /* "matrix coefficient 2" */
    { 0x51,         0x00 },                /* vb */
    { 0x52,         0x22 },                /* "matrix coefficient 4" */
    { 0x53,         0x5e },                /* "matrix coefficient 5" */
    { 0x54,         0x80 },                /* "matrix coefficient 6" */
    { COM13,        COM13_GAMMA_EN | COM13_UVSAT_AUTO },
    { 0xFF,         0xFF }
};

static const uint8_t vga_regs[][2] = {
    { COM3,         0x00 },
    { COM14,        0x00 },
    { 0x72,         0x11 },     // downsample by 4
    { 0x73,         0xf0 },     // divide by 4
    { HSTART,       0x13 },
    { HSTOP,        0x01 },
    { HREF,         0xb6 },
    { VSTART,       0x03 },
    { VSTOP,        0x00 },
    { VREF,         0x0a },
    { 0xFF,         0xFF },
};

#if (OMV_OV7670_VERSION == 75)
static const uint8_t qvga_regs[][2] = {
    { COM3,         COM3_DCW_EN },
    { COM14,        0x11 },      // Divide by 2
    { 0x72,         0x22 },      // This has no effect on OV7675
    { 0x73,         0xf2 },      // This has no effect on OV7675
    { HSTART,       0x15 },
    { HSTOP,        0x03 },
    { HREF,         0xC0 },
    { VSTART,       0x03 },
    { VSTOP,        0x7B },
    { VREF,         0xF0 },
    { 0xFF,         0xFF },
};
#else
static const uint8_t qvga_regs[][2] = {
    { COM3,         COM3_DCW_EN },
    { COM14,        0x19 },
    { 0x72,         0x11 },     // downsample by 2
    { 0x73,         0xf1 },     // divide by 2
    { HSTART,       0x15 },
    { HSTOP,        0x04 },
    { HREF,         0xa4 },
    { VSTART,       0x02 },
    { VSTOP,        0x7a },
    { VREF,         0x0a },
    { 0XFF,         0XFF },
};
#endif

#if (OMV_OV7670_VERSION == 75)
static const uint8_t qqvga_regs[][2] = {
    { COM3,         COM3_DCW_EN },
    { COM14,        0x11 },     // Divide by 2
    { 0x72,         0x22 },     // This has no effect on OV7675
    { 0x73,         0xf2 },     // This has no effect on OV7675
    { HSTART,       0x16 },
    { HSTOP,        0x04 },
    { HREF,         0xa4 },
    { VSTART,       0x22 },
    { VSTOP,        0x7a },
    { VREF,         0xfa },
    { 0xFF,         0xFF },
};
#else
static const uint8_t qqvga_regs[][2] = {
    { COM3,         COM3_DCW_EN },
    { COM14,        0x1a},
    { 0x72,         0x22 },     // downsample by 4
    { 0x73,         0xf2 },     // divide by 4
    { HSTART,       0x16 },
    { HSTOP,        0x04 },
    { HREF,         0xa4 },
    { VSTART,       0x02 },
    { VSTOP,        0x7a },
    { VREF,         0x0a },
    { 0XFF,         0XFF },
};
#endif

static int reset(omv_csi_t *csi) {
    // Reset all registers
    int ret = omv_i2c_writeb(&csi->i2c_bus, csi->slv_addr, COM7, COM7_RESET);

    // Delay 2 ms
    mp_hal_delay_ms(2);

    // Write default registers
    for (int i = 0; default_regs[i][0] != 0xff; i++) {
        ret |= omv_i2c_writeb(&csi->i2c_bus, csi->slv_addr, default_regs[i][0], default_regs[i][1]);
    }

    // Delay 300 ms
    if (!csi->disable_delays) {
        mp_hal_delay_ms(300);
    }

    return ret;
}

static int sleep(omv_csi_t *csi, int enable) {
    uint8_t reg;
    int ret = omv_i2c_readb(&csi->i2c_bus, csi->slv_addr, COM2, &reg);

    if (enable) {
        reg |= COM2_SOFT_SLEEP;
    } else {
        reg &= ~COM2_SOFT_SLEEP;
    }

    // Write back register
    return omv_i2c_writeb(&csi->i2c_bus, csi->slv_addr, COM2, reg) | ret;
}

static int read_reg(omv_csi_t *csi, uint16_t reg_addr) {
    uint8_t reg_data;
    if (omv_i2c_readb(&csi->i2c_bus, csi->slv_addr, reg_addr, &reg_data) != 0) {
        return -1;
    }
    return reg_data;
}

static int write_reg(omv_csi_t *csi, uint16_t reg_addr, uint16_t reg_data) {
    return omv_i2c_writeb(&csi->i2c_bus, csi->slv_addr, reg_addr, reg_data);
}

static int set_pixformat(omv_csi_t *csi, pixformat_t pixformat) {
    int ret = 0;
    const uint8_t(*regs)[2];

    switch (pixformat) {
        case PIXFORMAT_RGB565:
            regs = rgb565_regs;
            break;
        case PIXFORMAT_YUV422:
        case PIXFORMAT_GRAYSCALE:
            regs = yuv422_regs;
            break;
        default:
            return -1;
    }

    // Write pixel format registers
    for (int i = 0; regs[i][0] != 0xff; i++) {
        ret |= omv_i2c_writeb(&csi->i2c_bus, csi->slv_addr, regs[i][0], regs[i][1]);
    }

    return ret;
}

static int set_framesize(omv_csi_t *csi, omv_csi_framesize_t framesize) {
    int ret = 0;
    const uint8_t(*regs)[2];

    switch (framesize) {
        case OMV_CSI_FRAMESIZE_VGA:
            regs = vga_regs;
            break;
        case OMV_CSI_FRAMESIZE_QVGA:
            regs = qvga_regs;
            break;
        case OMV_CSI_FRAMESIZE_QQVGA:
            regs = qqvga_regs;
            break;
        default:
            return -1;
    }

    // Write pixel format registers
    for (int i = 0; regs[i][0] != 0xFF; i++) {
        ret |= omv_i2c_writeb(&csi->i2c_bus, csi->slv_addr, regs[i][0], regs[i][1]);
    }
    return ret;
}

static int set_hmirror(omv_csi_t *csi, int enable) {
    uint8_t val;
    int ret = omv_i2c_readb(&csi->i2c_bus, csi->slv_addr, MVFP, &val);
    ret |=
        omv_i2c_writeb(&csi->i2c_bus, csi->slv_addr, MVFP,
                       enable ? (val | MVFP_MIRROR) : (val & (~MVFP_MIRROR)));

    return ret;
}

static int set_vflip(omv_csi_t *csi, int enable) {
    uint8_t val;
    int ret = omv_i2c_readb(&csi->i2c_bus, csi->slv_addr, MVFP, &val);
    ret |=
        omv_i2c_writeb(&csi->i2c_bus, csi->slv_addr, MVFP,
                       enable ? (val | MVFP_VFLIP) : (val & (~MVFP_VFLIP)));

    return ret;
}
int ov7670_init(omv_csi_t *csi) {
    // Initialize csi structure.
    csi->reset = reset;
    csi->sleep = sleep;
    csi->read_reg = read_reg;
    csi->write_reg = write_reg;
    csi->set_pixformat = set_pixformat;
    csi->set_framesize = set_framesize;
    csi->set_hmirror = set_hmirror;
    csi->set_vflip = set_vflip;

    // Set csi flags
    csi->vsync_pol = 1;
    csi->hsync_pol = 0;
    csi->pixck_pol = 1;
    csi->frame_sync = 0;
    csi->mono_bpp = 2;
    csi->rgb_swap = 1;
    csi->yuv_format = SUBFORMAT_ID_YVU422;

    return 0;
}
#endif // (OMV_OV7670_ENABLE == 1)
