/**
 @file ov965x_camera.c
*/
/*
 * ============================================================================
 * History
 * =======
 * 2017-03-15 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <ft900.h>

/* UART support for printf output. */
#include "tinyprintf.h"

#include "camera.h"
#include "ov965x_camera.h"

/*
 * OV965x register definitions. Taken from OV9653_register_map.pdf.
 */
#define REG_GAIN		0x00	/* Gain control, AGC[7:0] */
#define REG_BLUE		0x01	/* AWB - Blue chanel gain */
#define REG_RED			0x02	/* AWB - Red chanel gain */
#define REG_VREF		0x03	/* [7:6] - AGC[9:8], [5:3]/[2:0] */
#define REG_COM1		0x04
#define REG_BAVE 		0x05
#define REG_GEAVE 		0x06
#define REG_RESV_07		0x07
#define REG_RAVE 		0x08
#define REG_COM2		0x09
#define REG_PID			0x0a	/* Product ID MSB */
#define REG_VER			0x0b	/* Product ID LSB */
#define REG_COM3		0x0c
#define REG_COM4		0x0d	/* Vario Pixels  */
#define REG_COM5		0x0e	/* System clock options */
#define REG_COM6		0x0f	/* HREF & ADBLC options */
#define REG_AECH		0x10	/* Exposure value, AEC[9:2] */
#define REG_CLKRC		0x11	/* Clock control */
#define REG_COM7		0x12	/* SCCB reset, output format */
#define REG_COM8		0x13	/* AGC/AEC options */
#define REG_COM9		0x14	/* Gain ceiling */
#define REG_COM10		0x15	/* PCLK, HREF, HSYNC signals polarity */
#define REG_RESV_16		0x16
#define REG_HSTART		0x17	/* Horiz start high bits */
#define REG_HSTOP		0x18	/* Horiz stop high bits */
#define REG_VSTRT		0x19	/* Vert start high bits */
#define REG_VSTOP		0x1a	/* Vert stop high bits */
#define REG_PSHFT		0x1b	/* Pixel delay after HREF */
#define REG_MIDH		0x1c	/* Manufacturer ID MSB */
#define REG_MIDL		0x1d	/* Manufufacturer ID LSB */
#define REG_MVFP		0x1e	/* Image mirror/flip */
#define REG_LAEC		0x1f
#define REG_BOS			0x20	/* B channel Offset */
#define REG_GBOS		0x21	/* Gb channel Offset */
#define REG_GROS		0x22	/* Gr channel Offset */
#define REG_ROS			0x23	/* R channel Offset */
#define REG_AEW			0x24	/* AGC upper limit */
#define REG_AEB			0x25	/* AGC lower limit */
#define REG_VPT			0x26	/* AGC/AEC fast mode op region */
#define REG_BBIAS		0x27	/* B channel output bias */
#define REG_GBBIAS		0x28	/* Gb channel output bias */
#define REG_GR_COM		0x29	/* Analog BLC & regulator */
#define REG_EXHCH		0x2a	/* Dummy pixel insert MSB */
#define REG_EXHCL		0x2b	/* Dummy pixel insert LSB */
#define REG_RBIAS		0x2c	/* R channel output bias */
#define REG_ADVFL		0x2d	/* LSB of dummy line insert */
#define REG_ADVFH		0x2e	/* MSB of dummy line insert */
#define REG_YAVE		0x2f	/* Y/G channel average value */
#define REG_HSYST		0x30	/* HSYNC rising edge delay LSB*/
#define REG_HSYEN		0x31	/* HSYNC falling edge delay LSB*/
#define REG_HREF		0x32	/* HREF pieces */
#define REG_CHLF		0x33	/* reserved */
#define REG_AREF1		0x34
#define REG_AREF2		0x35
#define REG_AREF3		0x36
#define REG_ADC			0x37	/* reserved */
#define REG_ADC2		0x38	/* reserved */
#define REG_AREF4		0x39
#define REG_TSLB		0x3a	/* YUVU format */
#define REG_COM11		0x3b	/* Night mode, banding filter enable */
#define REG_COM12		0x3c	/* HREF option, UV average */
#define REG_COM13		0x3d	/* Gamma selection, Color matrix en. */
#define REG_COM14		0x3e	/* Edge enhancement options */
#define REG_EDGE		0x3f	/* Edge enhancement factor */
#define REG_COM15		0x40	/* Output range, RGB 555/565 */
#define REG_COM16		0x41	/* Color matrix coeff options */
#define REG_COM17		0x42	/* Single frame out, banding filter */
#define	REG_MTX(A)		(0x4f -1 + (A)) /* Matrix coefficient 1 to 9 */
#define REG_MTXS		0x58
#define REG_LCC(A)		(0x62 - 1 + (A)) /* Lens correction option 1 to 4 */
#define REG_MANU		0x67	/* Manual U value */
#define REG_MANV		0x68	/* Manual V value */
#define REG_HV			0x69	/* Manual banding filter MSB */
#define REG_MBD			0x6a	/* Manual banding filter value */
#define REG_DBLV		0x6b	/* reserved */
#define REG_GSP(A)		(0x6c + (A)) /* Gamma curve 0 to 14 */
#define REG_POIDX		0x72
#define REG_XINDX		0x74
#define REG_YINDX		0x75
#define REG_GST(A)		(0x7c + (A)) /* Gamma curve 0 40 14 */
#define REG_COM21		0x8b
#define REG_COM22		0x8c	/* Edge enhancement, denoising */
#define REG_COM23		0x8d	/* Color bar test, color gain */
#define REG_DBLC1		0x8f	/* Digital BLC */
#define REG_DBLC_B		0x90	/* Digital BLC B channel offset */
#define REG_DBLC_R		0x91	/* Digital BLC R channel offset */
#define REG_DM_LNL		0x92	/* Dummy line low 8 bits */
#define REG_DM_LNH		0x93	/* Dummy line high 8 bits */
#define REG_LCCFB		0x9d	/* Lens Correction B channel */
#define REG_LCCFR		0x9e	/* Lens Correction R channel */
#define REG_DBLC_GB		0x9f	/* Digital BLC GB chan offset */
#define REG_DBLC_GR		0xa0	/* Digital BLC GR chan offset */
#define REG_AECHM		0xa1	/* Exposure value - bits AEC[15:10] */
#define REG_BD50ST		0xa2	/* Banding filter value for 50Hz */
#define REG_BD60ST		0xa3	/* Banding filter value for 60Hz */
#define REG_COM25		0xa4
#define REG_COM26		0xa5
#define REG_G_GAIN		0xa6
#define REG_VGA_ST		0xa7
#define REG_ACOM(A)		(0xa8 - 1 + (A))
#define REG_NULL		0xff	/* Array end token */

/**
 * @brief OV965x I2C address.
 */
#define OV965x_I2C_ADDR (0x60)

/**
 * @brief Register initialisation and setup.
 * @details Taken from ov534-ov9xxx gspca driver (Linux/drivers/media/usb/gspca/ov534_9.c)
 */
/*
 * ov534-ov9xxx gspca driver
 *
 * Copyright (C) 2009-2011 Jean-Francois Moine http://moinejf.free.fr
 * Copyright (C) 2008 Antonio Ospite <ospite@studenti.unina.it>
 * Copyright (C) 2008 Jim Paris <jim@jtan.com>
 *
 * Based on a prototype written by Mark Ferrell <majortrips@gmail.com>
 * USB protocol reverse engineered by Jim Paris <jim@jtan.com>
 * https://jim.sh/svn/jim/devl/playstation/ps3/eye/test/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
//@{
static const uint8_t ov965x_init_regs[] = {
	REG_GAIN, 0x00,
	REG_BLUE, 0x80,
	REG_RED, 0x80,
	REG_VREF, 0x00,
	REG_COM1, 0x03,
	REG_VER, 0x57,
	REG_COM5, 0x61,	/* System clock options */
	REG_COM6, 0x42,	/* HREF & ADBLC options */
	REG_CLKRC, 0x00,
	REG_COM7, 0x02,
	REG_COM8, 0xe7,
	REG_COM9, 0x2e,
	REG_RESV_16, 0x24,
	REG_HSTART, 0x1d, //
	REG_HSTOP, 0x18,
	REG_VSTRT, 0x01,
	REG_VSTOP, 0x81,
	REG_MVFP, 0x04,
	REG_AEW, 0x3c,
	REG_AEB, 0x36,
	REG_VPT, 0x71,
	REG_BBIAS, 0x08,
	REG_GBBIAS, 0x08,
	REG_GR_COM, 0x15,
	REG_EXHCH, 0x00,
	REG_EXHCL, 0x00,
	REG_RBIAS, 0x08,
	REG_HREF, 0xff,
	REG_CHLF, 0x00,
	REG_AREF1, 0x3f,
	REG_AREF2, 0x00,
	REG_AREF3, 0xf8,
	REG_ACOM(1), 0x72,
	REG_AREF4, 0x57,
	REG_TSLB, 0x80,
	REG_COM11, 0xc4,
	REG_COM13, 0x99,	/* Gamma selection, colour matrix, UV delay */
	REG_EDGE, 0xc1,
	REG_COM15, 0xc0,
	REG_COM16, 0x40,
	REG_COM17, 0xc0,	/* Single frame out, banding filter */
	0x43, 0x0a,
	0x44, 0xf0,
	0x45, 0x46,
	0x46, 0x62,
	0x47, 0x2a,
	0x48, 0x3c,
	0x4a, 0xfc,
	0x4b, 0xfc,
	0x4c, 0x7f,
	0x4d, 0x7f,
	0x4e, 0x7f,
	REG_MTX(1), 0x98,
	REG_MTX(2), 0x98,
	REG_MTX(3), 0x00,
	REG_MTX(4), 0x28,
	REG_MTX(5), 0x70,
	REG_MTX(6), 0x98,
	REG_MTXS, 0x1a,
	0x59, 0x85,
	0x5a, 0xa9,
	0x5b, 0x64,
	0x5c, 0x84,
	0x5d, 0x53,
	0x5e, 0x0e,
	0x5f, 0xf0,
	0x60, 0xf0,
	0x61, 0xf0,
	REG_LCC(1), 0x00,
	REG_LCC(2), 0x00,
	REG_LCC(3), 0x02,
	REG_LCC(4), 0x16,
	REG_LCC(5), 0x01,
	REG_HV, 0x02,
	REG_DBLV, 0x5a,
	REG_GSP(0), 0x04,
	REG_GSP(1), 0x55,
	REG_GSP(2), 0x00,
	REG_GSP(3), 0x9d,
	REG_GSP(4), 0x21,
	REG_GSP(5), 0x78,
	REG_GSP(6), 0x00,
	REG_GSP(7), 0x01,
	REG_GSP(8), 0x3a,
	REG_GSP(9), 0x35,
	REG_GSP(10), 0x01,
	REG_GSP(11), 0x02,
	REG_GSP(14), 0x12,
	REG_GSP(15), 0x08,
	REG_GST(0), 0x16,
	REG_GST(1), 0x30,
	REG_GST(2), 0x5e,
	REG_GST(3), 0x72,
	REG_GST(4), 0x82,
	REG_GST(5), 0x8e,
	REG_GST(6), 0x9a,
	REG_GST(7), 0xa4,
	REG_GST(8), 0xac,
	REG_GST(9), 0xb8,
	REG_GST(10), 0xc3,
	REG_GST(11), 0xd6,
	REG_GST(12), 0xe6,
	REG_GST(13), 0xf2,
	REG_GST(14), 0x03,
	REG_COM22, 0x89,	/* Edge enhancement, denoising */
	REG_COM9, 0x28,
	REG_DBLC_B, 0x7d,	/* Digital BLC B chan offset */
	REG_DBLC_R, 0x7b,	/* Digital BLC R chan offset */
	REG_LCCFB, 0x03,
	REG_LCCFR, 0x04,
	REG_DBLC_GB, 0x7a,	/* Digital BLC GB chan offset */
	REG_DBLC_GR, 0x79,
	REG_AECHM, 0x40,
	REG_COM25, 0x50,   /* com21 */
	REG_COM26, 0x68,   /* com26 */
	REG_G_GAIN, 0x4a,   /* AWB green */
	REG_ACOM(1), 0xc1,   /* refa8 */
	REG_ACOM(2), 0xef,   /* refa9 */
	REG_ACOM(3), 0x92,
	REG_ACOM(4), 0x04,
	REG_ACOM(5), 0x80,   /* black level control */
	REG_ACOM(6), 0x80,
	REG_ACOM(7), 0x80,
	REG_ACOM(8), 0x80,
	0xb2, 0xf2,
	0xb3, 0x20,
	0xb4, 0x20,   /* ctrlb4 */
	0xb5, 0x00,
	0xb6, 0xaf,
	0xbb, 0xae,
	0xbc, 0x7f,   /* ADC channel offsets */
	0xdb, 0x7f,
	0xbe, 0x7f,
	0xbf, 0x7f,
	0xc0, 0xe2,
	0xc1, 0xc0,
	0xc2, 0x01,
	0xc3, 0x4e,
	0xc6, 0x85,
	0xc7, 0x80,   /* com24 */
	0xc9, 0xe0,
	0xca, 0xe8,
	0xcb, 0xf0,
	0xcc, 0xd8,
	0xcd, 0xf1,
	REG_MTX(1), 0x98,
	REG_MTX(2), 0x98,
	REG_MTX(3), 0x00,
	REG_MTX(4), 0x28,
	REG_MTX(5), 0x70,
	REG_MTX(6), 0x98,
	REG_MTXS, 0x1a,
	0xc5, 0x03,
	REG_MBD, 0x02,
	REG_COM7, 0x62,
	REG_AREF3, 0xfa,
	REG_HV, 0x0a,
	REG_COM22, 0x89,
	REG_COM9, 0x28,
	REG_COM14, 0x0c,
	REG_COM16, 0x40,
	REG_GSP(6), 0x00,
	REG_GSP(7), 0x00,
	REG_GSP(8), 0x3a,
	REG_GSP(9), 0x35,
	REG_GSP(10), 0x01,
	0xc7, 0x80,
	REG_VREF, 0x12,
	REG_HSTART, 0x16,
	REG_HSTOP, 0x02,
	REG_VSTRT, 0x01,
	REG_HREF, 0xff,
	0xc0, 0xaa,

    REG_COM11, 0xc4,
    REG_MVFP, 0x04,   /* mvfp */
    REG_COM8, 0xe0,   /* com8 */
    REG_GAIN, 0x00,   /* gain */
    REG_COM8, 0xe7,   /* com8 - everything (AGC, AWB and AEC) */
    REG_CLKRC, 0x03,   /* clkrc */
    REG_DBLV, 0x5a,   /* dblv */
    REG_MBD, 0x05,
    0xc5, 0x07,
    REG_BD50ST, 0x4b,
    REG_BD60ST, 0x3e,
    REG_ADVFL, 0x00,
    0xff, 0x42,   /* read 42, write ff 00 */
    REG_COM17, 0xc0,   /* com17 */
    REG_ADVFL, 0x00,
    0xff, 0x42,   /* read 42, write ff 00 */
	REG_COM17, 0xc1,   /* com17 */
/* sharpness */
    REG_EDGE, 0x01,
    0xff, 0x42,   /* read 42, write ff 00 */
	REG_COM17, 0xc1,   /* com17 */
/* saturation */
    REG_MTX(1), 0x98,   /* matrix */
	REG_MTX(2), 0x98,
	REG_MTX(3), 0x00,
	REG_MTX(4), 0x28,
	REG_MTX(5), 0x70,
	REG_MTX(6), 0x98,
	REG_MTX(9), 0x1a,
    0xff, 0x41,   /* read 41, write ff 00 */
    REG_COM16, 0x40,   /* com16 */
/* contrast */
    REG_MTX(8), 0x40,
/* brightness */
	REG_MTX(7), 0x8f,
/* expo */
	REG_AECH, 0x25,   /* aech - exposure high bits */
    0xff, 0x13,   /* read 13, write ff 00 */
    REG_COM8, 0xe7,   /* com8 - everything (AGC, AWB and AEC) */

	REG_COM10, 0x20,
};

// QGVA 320x240
static const uint8_t ov965x_qvga_regs[] = {
    REG_COM7, 0x62,   /* com7 - 15fps VGA YUV */
    REG_AREF3, 0xfa,   /* aref3 */
    REG_HV, 0x0a,   /* hv */
    REG_COM22, 0x89,   /* com22 */
    REG_COM9, 0x28,   /* com9 */
    REG_COM14, 0x0c,   /* com14 */
    REG_COM16, 0x41,   /* com16 */
	REG_POIDX, 0x22,
	//REG_XINDX, 0,
	//REG_YINDX, 0,
	REG_GSP(6), 0x00,
	REG_GSP(7), 0x00,
	REG_GSP(8), 0x3a,
	REG_GSP(9), 0x35,
	REG_GSP(10), 0x01,
    0xc7, 0x80,   /* com24 */
    REG_HREF, 0xff,   /* href: hstop - hstart = 320 */
    REG_HSTART, 0x36,   /* hstart @ 0x36/7 -> 0x1b7 */
    REG_HSTOP, 0x5e,   /* hstop @ 0x5e/7 -> 0x2f7 */
    REG_VREF, 0x12,   /* vref: vstop - vstrt = 239 */
    REG_VSTRT, 0x02,   /* vstrt @ 0x02/2 -> 0x012 */
    REG_VSTOP, 0x20,   /* vstop @ 0x20/1 -> 0x101 */
    0xc0, 0xaa,

    REG_COM11, 0xe4,   /* com11 - night mode 1/8 frame rate, full frame */
    REG_MVFP, 0x04,   /* mvfp */
    REG_COM8, 0xe0,   /* com8 */
	REG_GAIN, 0x00,
    REG_COM8, 0xe7,   /* com8 - everything (AGC, AWB and AEC) */
    REG_CLKRC, 0x03,   /* clkrc */
    REG_DBLV, 0x5a,   /* dblv */
    REG_MBD, 0x02,   /* 50 Hz banding filter */
    0xc5, 0x03,   /* 60 Hz banding filter */
    REG_BD50ST, 0x96,   /* bd50 */
    REG_BD60ST, 0x7d,   /* bd60 */

    0xff, 0x13,   /* read 13, write ff 00 */
    REG_COM8, 0xe7,
    REG_TSLB, 0x80,   /* tslb - yuyv */
};

// VGA 640x480
static const uint8_t ov965x_vga_regs[] = {
    REG_COM7, 0x62,   /* com7 - 15fps VGA YUV */
    REG_AREF3, 0xfa,   /* aref3 */
    REG_HV, 0x0a,   /* hv */
    REG_COM22, 0x89,   /* com22 */
    REG_COM9, 0x28,   /* com9 */
    REG_COM14, 0x0c,   /* com14 */
    REG_COM16, 0x40,   /* com16 */
	REG_GSP(6), 0x00,
	REG_GSP(7), 0x00,
	REG_GSP(8), 0x3a,
	REG_GSP(9), 0x35,
	REG_GSP(10), 0x01,
    0xc7, 0x80,   /* com24 */
    REG_VREF, 0x12,   /* vref */
    REG_HSTART, 0x16,   /* hstart */
    REG_HSTOP, 0x02,   /* hstop */
    REG_VSTRT, 0x01,   /* vstrt */
    REG_VSTOP, 0x3d,   /* vstop */
    REG_HREF, 0xff,   /* href */
	0xc0, 0xaa,

    REG_COM11, 0xc4,   /* com11 - night mode 1/4 frame rate */
    REG_MVFP, 0x04,   /* mvfp */
    REG_COM8, 0xe0,   /* com8 */
	REG_GAIN, 0x00,
    REG_COM8, 0xe7,   /* com8 - everything (AGC, AWB and AEC) */
    REG_CLKRC, 0x03,   /* clkrc */
    REG_DBLV, 0x5a,   /* dblv */
    REG_MBD, 0x05,   /* 50 Hz banding filter */
    0xc5, 0x07,   /* 60 Hz banding filter */
    REG_BD50ST, 0x4b,   /* bd50 */
    REG_BD60ST, 0x3e,   /* bd60 */

    REG_ADVFL, 0x00,   /* advfl */
};

// SVGA 800x600
static const uint8_t ov965x_svga_regs[] = {
    REG_COM7, 0x02,   /* com7 - YUYV - VGA 15 full resolution */
    REG_AREF3, 0xf8,   /* aref3 */
    REG_HV, 0x02,   /* hv */
    REG_COM22, 0x0d,   /* com22 */
    REG_COM14, 0x0c,   /* com14 */
    REG_COM16, 0x40,   /* com16 */
    0x72, 0x00,
    0x73, 0x01,
    0x74, 0x3a,
    0x75, 0x35,
    0x76, 0x01,
    0xc7, 0x80,   /* com24 */
    REG_HREF, 0xff,   /* href: hstop - hstart = 800 */
    REG_HSTART, 0x1d,   /* hstart @ 0x1d/7 -> 0x0ef */
    REG_HSTOP, 0x9a,   /* hstop @ 0x9a/7 -> 0x4d7 */
    REG_VREF, 0x3b,   /* vref: vstop - vstrt = 599 */
    REG_VSTRT, 0x01,   /* vstrt @ 0x01/b -> 0x013 */
    REG_VSTOP, 0x4d,   /* vstop @ 0x4d/3 -> 0x26b */
    0xc0, 0xe2,

    REG_COM11, 0xc4,   /* com11 - night mode 1/4 frame rate */
    REG_MVFP, 0x04,   /* mvfp */
    REG_COM8, 0xe0,   /* com8 */
    0x00, 0x00,
    REG_COM8, 0xe7,   /* com8 - everything (AGC, AWB and AEC) */
    REG_CLKRC, 0x02,   /* clkrc */
    REG_DBLV, 0x5a,   /* dblv */
    REG_MBD, 0x0c,   /* 50 Hz banding filter */
    0xc5, 0x0f,   /* 60 Hz banding filter */
    REG_BD50ST, 0x4e,   /* bd50 */
    REG_BD60ST, 0x41,   /* bd60 */
};

// XGA 1024x768 - UNTESTED
static const uint8_t ov965x_xga_regs[] = {
    REG_COM7, 0x02,   /* com7 */
    REG_AREF3, 0xf8,   /* aref3 */
    REG_HV, 0x02,   /* hv */
    REG_COM22, 0x89,   /* com22 */
    REG_COM9, 0x28,   /* com9 */
    REG_COM14, 0x0c,   /* com14 */
    REG_COM16, 0x40,   /* com16 */
    0x72, 0x00,
    0x73, 0x01,
    0x74, 0x3a,
    0x75, 0x35,
    0x76, 0x01,
    0xc7, 0x80,   /* com24 */
    REG_HREF, 0xff,   /* href */
    REG_HSTART, 0x1d,   /* hstart */
    REG_HSTOP, 0xbd,   /* hstop */
    REG_VREF, 0x1b,   /* vref */
    REG_VSTRT, 0x01,   /* vstrt */
    REG_VSTOP, 0x81,   /* vstop */
    0xc0, 0xe2,

    REG_COM11, 0xc4,   /* com11 - night mode 1/4 frame rate */
    REG_MVFP, 0x04,   /* mvfp */
    REG_COM8, 0xe0,   /* com8 */
    0x00, 0x00,
    REG_COM8, 0xe7,   /* com8 - everything (AGC, AWB and AEC) */
    REG_CLKRC, 0x01,   /* clkrc */
    REG_DBLV, 0x5a,   /* dblv */
    REG_MBD, 0x0c,   /* 50 Hz banding filter */
    0xc5, 0x0f,   /* 60 Hz banding filter */
    REG_BD50ST, 0x4e,   /* bd50 */
    REG_BD60ST, 0x41,   /* bd60 */
};

// SXGA 1280x1024 - UNTESTED
static const uint8_t ov965x_sxga_regs[] = {
    REG_COM7, 0x02,   /* com7 */
    REG_AREF3, 0xf8,   /* aref3 */
    REG_HV, 0x02,   /* hv */
    REG_COM22, 0x89,   /* com22 */
    REG_COM9, 0x28,   /* com9 */
    REG_COM14, 0x0c,   /* com14 */
    REG_COM16, 0x40,   /* com16 */
    0x72, 0x00,
    0x73, 0x01,
    0x74, 0x3a,
    0x75, 0x35,
    0x76, 0x01,
    0xc7, 0x80,   /* com24 */
    REG_VREF, 0x1b,   /* vref */
    REG_HSTART, 0x1d,   /* hstart */
    REG_HSTOP, 0x02,   /* hstop */
    REG_VSTRT, 0x01,   /* vstrt */
    REG_VSTOP, 0x81,   /* vstop */
    REG_HREF, 0xff,   /* href */
    0xc0, 0xe2,

    REG_COM8, 0xe0,   /* com8 */
    0x00, 0x00,
    REG_COM8, 0xe7,   /* com8 - everything (AGC, AWB and AEC) */
    REG_COM11, 0xc4,   /* com11 - night mode 1/4 frame rate */
    REG_MVFP, 0x04,   /* mvfp */
    REG_CLKRC, 0x01,   /* clkrc */
    REG_DBLV, 0x5a,   /* dblv */
    REG_MBD, 0x0c,   /* 50 Hz banding filter */
    0xc5, 0x0f,   /* 60 Hz banding filter */
    REG_BD50ST, 0x4e,   /* bd50 */
    REG_BD60ST, 0x41,   /* bd60 */
};
//@}

/** @brief OV965x Start and Stop commands.
 */
//@{
static const uint8_t ov965x_start_regs[] = {
	REG_COM2, 0x00,	/* Start device */
};

static const uint8_t ov965x_stop_regs[] = {
	REG_COM2, 0x10,	/* Sleep device */
};
//@}

static void ov965x_send(const uint8_t *regs, int length)
{
	int i;

	for (i = 0; i < length; i += 2)
	{
		i2cm_write(OV965x_I2C_ADDR, regs[i], &regs[i+1], 1);
	}
}

uint16_t ov965x_init(void)
{
    uint8_t dat = 0;
	uint8_t pidh;
	uint8_t pidl;

	/* Wait a short period of time to allow camera to initialise. */
	delayms(10);
    /* Initialise the camera */
	i2cm_read(OV965x_I2C_ADDR, 0x0A, &pidh, 1);
	i2cm_read(OV965x_I2C_ADDR, 0x0B, &pidl, 1);

	tfp_printf("OV965x version: %x\n", ((pidh << 8) | pidl));

	if (pidh == MSB(OV965x_PID))
	{
		dat = 0x80;
		i2cm_write(OV965x_I2C_ADDR, REG_COM7, &dat, 1);
		delayms(250);
		ov965x_send(ov965x_stop_regs, sizeof(ov965x_stop_regs));

		return ((pidh << 8) | pidl);
	}

	return 0;
}

void ov965x_start(void)
{
	ov965x_send(ov965x_start_regs, sizeof(ov965x_start_regs));

	tfp_printf("Start streaming...\n");
}

void ov965x_stop(void)
{
	ov965x_send(ov965x_stop_regs, sizeof(ov965x_stop_regs));

	tfp_printf("...streaming stopped.\n");
}

void ov965x_set(int resolution, int frame_rate, int format)
{

	// Set the camera module to output the requested
	// resolution, frame rate and format. If it is not
	// supported then ignore.
	ov965x_send(ov965x_init_regs, sizeof(ov965x_init_regs));

	tfp_printf("OV965x ");

	if (format == CAMERA_FORMAT_UNCOMPRESSED)
	{
		tfp_printf(" uncompressed");

		// QVGA supports uncompressed at 15 fps.
		if (resolution == CAMERA_MODE_QVGA)
		{
			tfp_printf(" QVGA");
			if (frame_rate == CAMERA_FRAME_RATE_15)
			{
				tfp_printf(" 15fps");
				ov965x_send(ov965x_qvga_regs, sizeof(ov965x_qvga_regs));
			}
		}
		// VGA supports uncompressed and MJPEG at 15 fps.
		else if (resolution == CAMERA_MODE_VGA)
		{
			tfp_printf(" VGA");
			if (frame_rate == CAMERA_FRAME_RATE_15)
			{
				tfp_printf(" 15fps");
				ov965x_send(ov965x_vga_regs, sizeof(ov965x_vga_regs));
			}
		}
		// SVGA supports uncompressed only at 15 fps.
		else if (resolution == CAMERA_MODE_SVGA)
		{
			tfp_printf(" SVGA");

			if (frame_rate == CAMERA_FRAME_RATE_15)
			{
				tfp_printf(" 15fps");
				ov965x_send(ov965x_svga_regs, sizeof(ov965x_svga_regs));
			}
		}
		// SVGA supports uncompressed only at 15 fps.
		else if (resolution == CAMERA_MODE_XGA)
		{
			tfp_printf(" XGA");
			if (frame_rate == CAMERA_FRAME_RATE_15)
			{
				tfp_printf(" 15fps");
				ov965x_send(ov965x_xga_regs, sizeof(ov965x_xga_regs));
			}
		}
		// SXGA supports uncompressed only at 15 fps.
		else if (resolution == CAMERA_MODE_SXGA)
		{
			tfp_printf(" SXGA");
			if (frame_rate == CAMERA_FRAME_RATE_15)
			{
				tfp_printf(" 15fps");
				ov965x_send(ov965x_sxga_regs, sizeof(ov965x_sxga_regs));
			}
		}
	}

	tfp_printf("\n");
}

