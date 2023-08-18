/**
 @file ov5640_camera.c
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
#include "ov5640_camera.h"

/**
 * @brief I2C defines (from i2cm driver).
 * @details The OV5640 requires a 16-bit address and an 8-bit register. This
 *  cannot be done with the i2cm driver and therefore the appropriate functions
 *  need to be created here.
 */
#define I2C_STATUS_IDLE         0x20
#define I2C_STATUS_ADDRACK      0x04
#define I2C_STATUS_ERROR        0x02
#define I2C_STATUS_BUSY         0x01
#define I2C_FLAGS_RUN           0x01
#define I2C_FLAGS_START         0x02
#define I2C_FLAGS_STOP          0x04
#define I2C_FLAGS_ACK           0x08
#define I2C_FLAGS_HS            0x10
#define I2C_FLAGS_ADDR          0x20
#define I2C_FLAGS_SLRST         0x40
#define I2C_FLAGS_RSTB          0x80
#define I2C_READ_NOT_WRITE      0x01
#define I2C_TIMER_HS_SELECT     0x80
#define I2C_TIMER_PERIOD_MASK   0x7F

/**
 * @brief OV5640 I2C address.
 */
#define OV5640_I2C_ADDR (0x78)

/** @brief OV5640 SCCB Register Map Structure
 *  @details Used to store register address/value pairs for
 *   sensor initialisation.
 **/
struct ov5640_reg {
	uint16_t addr;
	uint8_t data;
};

/** @brief OV5640 SCCB Register Programming Macro
 *  @details Macro to program SCCB registers using address/value
 *   pairs from a struct ov5640_reg structure.
 **/
#define OV5640_WRITESCCBREG(A)	ov5640_writesccb(A, sizeof(A)/sizeof(A[0]))

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* @brief I2C master code used when reading or writing from OV5640.
 */
static uint8_t ov5640_i2c_wait_for()
{
	asm_noop();
	asm_noop();
	asm_noop();

 	while (I2CM->I2CM_CNTL_STATUS & I2C_STATUS_BUSY)
        ; /* Wait for the bus to be idle */

    return (I2CM->I2CM_CNTL_STATUS & I2C_STATUS_ERROR);
}

/* @brief I2C master code to read register from OV5640.
 * @details This requires a write of 2 bytes to the device to setup
 * 		the address then a new transaction which reads the register.
 */
static int8_t ov5640_i2c_read(const uint16_t reg, uint8_t *data)
{
	const uint8_t addr = OV5640_I2C_ADDR;
	int8_t ret = 0;
	uint8_t regl, regh;

	regl = reg & 0xff;
	regh = reg >> 8;

	do
	{
		/* Write slave address to SA */
		I2CM->I2CM_SLV_ADDR = addr;

		/* Write register high address to BUF */
		I2CM->I2CM_DATA = regh;
		/* Write command to I2CMCR to send Start and command byte. */
		I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_START | I2C_FLAGS_RUN;
		if(ov5640_i2c_wait_for()) {
			/* An Error Occurred */
			ret = -1;
			break;
		}

		/* Write register low address to BUF */
		I2CM->I2CM_DATA = regl;
		I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_RUN | I2C_FLAGS_STOP;
		if(ov5640_i2c_wait_for()) {
			/* An Error Occurred */
			ret = -2;
			break;
		}

		delayms(1);

		/* Write slave address to SA with R operation */
		I2CM->I2CM_SLV_ADDR = addr | 0x01;
		/* Receive with a NACK */
		I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_START | I2C_FLAGS_STOP | I2C_FLAGS_RUN;
		if(ov5640_i2c_wait_for()) {
			/* An Error Occurred */
			ret = -4;
			break;
		}

		/* Read data from device. */
		*data = I2CM->I2CM_DATA;

	} while (0);

	return ret;
}

static int8_t ov5640_i2c_write(const uint16_t reg,
		const uint8_t val)
{
	const uint8_t addr = OV5640_I2C_ADDR;
	int8_t ret = 0;
	uint8_t regl, regh;

	regl = reg & 0xff;
	regh = reg >> 8;

	do
	{
		/* Write slave address to SA */
		I2CM->I2CM_SLV_ADDR = addr;

		/* Write register high address to BUF */
		I2CM->I2CM_DATA = regh;
		/* Write command to I2CMCR to send Start and command byte. */
		I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_START | I2C_FLAGS_RUN;
		if(ov5640_i2c_wait_for()) {
			/* An Error Occurred */
			ret = -1;
			break;
		}

		/* Write register low address to BUF */
		I2CM->I2CM_DATA = regl;
		I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_RUN;
		if(ov5640_i2c_wait_for()) {
			/* An Error Occurred */
			ret = -2;
			break;
		}

		/* Write register value to BUF. */
		I2CM->I2CM_DATA = val;
		/* Receive with a NACK */
		I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_STOP | I2C_FLAGS_RUN;
		if(ov5640_i2c_wait_for()) {
			/* An Error Occurred */
			ret = -4;
			break;
		}
	} while (0);

	return ret;
}

static int8_t ov5640_writesccb(__flash__ const struct ov5640_reg *ptb, int32_t len)
{
	int32_t i;
	int8_t ret = 0;
	uint8_t regv;

	for (i = 0; i < len; i++) {
		if (ptb[i].addr == 0xffff)
		{
			delayms(ptb[i].data);
		}
		else
		{
			ret = ov5640_i2c_write(ptb[i].addr, ptb[i].data);

			ov5640_i2c_read(ptb[i].addr, &regv);

#if 0 // Check register written correctly.
			if (regv != ptb[i].data)
			{
				tfp_printf("%d 0x%04x 0x%02x (0x%02x)\n", ret, ptb[i].addr, regv, ptb[i].data);
			}
#endif
		}
	}
	return ret;
}

__flash__ static const struct ov5640_reg configscript_reset[] = {
		/* System Control */
		{0x3103, 0x11},
		{0x3008, 0x82},
		{0xFFFF, 100},		/* Sleep 100 ms */
};

__flash__ const struct ov5640_reg common_config_1[]=
{
		{0x3103, 0x03},
		{0x3017, 0xff},
		{0x3018, 0xff},
		{0x3108, 0x01},
		{0x3630, 0x2e},
		{0x3621, 0xe0},
		{0x3704, 0xa0},
		{0x3703, 0x5a},
		{0x3715, 0x78},
		{0x3717, 0x01},
		{0x370b, 0x60},
		{0x3705, 0x1a},
		{0x3905, 0x02},
		{0x3906, 0x10},
		{0x3901, 0x0a},
		{0x3731, 0x12},
		{0x3600, 0x08},
		{0x3601, 0x33},
		{0x302d, 0x60},
		{0x3620, 0x52},
		{0x371b, 0x20},
		{0x471c, 0x50},
		{0x3a13, 0x43},
		{0x3a18, 0x00},
		{0x3a19, 0xf8},
		{0x3634, 0x40},
		{0x3622, 0x01},
		{0x3c01, 0x34},
		{0x3c04, 0x28},
		{0x3c05, 0x98},
		{0x3c06, 0x00},
		{0x3c07, 0x07},
		{0x3c08, 0x00},
		{0x3c09, 0x1c},
		{0x3c0a, 0x9c},
		{0x3c0b, 0x40},
		{0x3820, 0x41},
};

__flash__ static const struct ov5640_reg set_qvga_15fps[] = {
		{0x3800,MSB(0)},
		{0x3801,LSB(0)},
		{0x3802,MSB(4)},
		{0x3803,LSB(4)},
		{0x3804,MSB(2623)},
		{0x3805,LSB(2623)},
		{0x3806,MSB(1947)},
		{0x3807,LSB(1947)},
		{0x3808,MSB(320)},
		{0x3809,LSB(320)},
		{0x380a,MSB(240)},
		{0x380b,LSB(240)},
		{0x3810,MSB(8)},
		{0x3811,LSB(8)},
		{0x3812,MSB(2)},
		{0x3813,LSB(2)},
		// For MJPEG output.
		{0x4602,MSB(CAMERA_PREFERRED_LINE_LENGTH)},
		{0x4603,LSB(CAMERA_PREFERRED_LINE_LENGTH)},
		{0x4604,MSB(OV5640_LINE_COUNT_MJPEG_QVGA)},
		{0x4605,LSB(OV5640_LINE_COUNT_MJPEG_QVGA)},
};

__flash__ static const struct ov5640_reg set_vga_15fps[] = {
		{0x3800,MSB(0)},
		{0x3801,LSB(0)},
		{0x3802,MSB(4)},
		{0x3803,LSB(4)},
		{0x3804,MSB(2623)},
		{0x3805,LSB(2623)},
		{0x3806,MSB(1947)},
		{0x3807,LSB(1947)},
		{0x3808,MSB(640)},
		{0x3809,LSB(640)},
		{0x380a,MSB(480)},
		{0x380b,LSB(480)},
		{0x3810,MSB(8)},
		{0x3811,LSB(8)},
		{0x3812,MSB(2)},
		{0x3813,LSB(2)},
		// For MJPEG output.
		{0x4602,MSB(CAMERA_PREFERRED_LINE_LENGTH)},
		{0x4603,LSB(CAMERA_PREFERRED_LINE_LENGTH)},
		{0x4604,MSB(OV5640_LINE_COUNT_MJPEG_VGA)},
		{0x4605,LSB(OV5640_LINE_COUNT_MJPEG_VGA)},
};

__flash__ static const struct ov5640_reg set_svga_15fps[] = {
		{0x3800,MSB(0)},
		{0x3801,LSB(0)},
		{0x3802,MSB(4)},
		{0x3803,LSB(4)},
		{0x3804,MSB(2623)},
		{0x3805,LSB(2623)},
		{0x3806,MSB(1947)},
		{0x3807,LSB(1947)},
		{0x3808,MSB(800)},
		{0x3809,LSB(800)},
		{0x380a,MSB(600)},
		{0x380b,LSB(600)},
		{0x3810,MSB(16)},
		{0x3811,LSB(16)},
		{0x3812,MSB(194)},
		{0x3813,LSB(194)},
		{0x3810,MSB(8)},
		{0x3811,LSB(8)},
		{0x3812,MSB(2)},
		{0x3813,LSB(2)},
		// For MJPEG output.
		{0x4602,MSB(CAMERA_PREFERRED_LINE_LENGTH)},
		{0x4603,LSB(CAMERA_PREFERRED_LINE_LENGTH)},
		{0x4604,MSB(OV5640_LINE_COUNT_MJPEG_SVGA)},
		{0x4605,LSB(OV5640_LINE_COUNT_MJPEG_SVGA)},
};

__flash__ static const struct ov5640_reg set_xga_15fps[] = {
		{0x3800,MSB(0)},
		{0x3801,LSB(0)},
		{0x3802,MSB(4)},
		{0x3803,LSB(4)},
		{0x3804,MSB(2623)},
		{0x3805,LSB(2623)},
		{0x3806,MSB(1947)},
		{0x3807,LSB(1947)},
		{0x3808,MSB(1024)},
		{0x3809,LSB(1024)},
		{0x380a,MSB(768)},
		{0x380b,LSB(768)},
		{0x3810,MSB(16)},
		{0x3811,LSB(16)},
		{0x3812,MSB(194)},
		{0x3813,LSB(194)},
		// For MJPEG output.
		{0x4602,MSB(CAMERA_PREFERRED_LINE_LENGTH)},
		{0x4603,LSB(CAMERA_PREFERRED_LINE_LENGTH)},
		{0x4604,MSB(OV5640_LINE_COUNT_MJPEG_XGA)},
		{0x4605,LSB(OV5640_LINE_COUNT_MJPEG_XGA)},
};

__flash__ const struct ov5640_reg set_jpeg[]= {
		{0x3034, 0x1a},
		{0x3035, 0x11},
		{0x3036, 0x46},
		{0x3631, 0x0e},
		{0x3632, 0xe2},
		{0x3633, 0x12},
		{0x3635, 0x13},
		{0x3636, 0x03},
		{0x3037, 0x13},

		{0x3821, 0x27},
		{0x3814, 0x31},
		{0x3815, 0x31},

		{0x4713, 0x02},
		{0x4600, 0x20}, // register 0x4602/4603 and 0x4604/4605 are width and height
		{0x3000, 0x00},
		{0x3002, 0x00},
		{0x3004, 0xff},
		{0x3006, 0xff},
		{0x4837, 0x08},
		{0x3824, 0x04},

		{0x380c,MSB(1896)},
		{0x380d,LSB(1896)},
		{0x380e,MSB(984)},
		{0x380f,LSB(984)},
};

__flash__ const struct ov5640_reg set_yuv[]= {
		{0x3034, 0x1a},
		{0x3035, 0x31},
		{0x3036, 0x69},
		//{0x3631, 0x0e},
		{0x3632, 0xe2},
		{0x3633, 0x23},
		{0x3635, 0x1c},
		//{0x3636, 0x03},
		{0x3037, 0x13},

		{0x3814, 0x71},
		{0x3815, 0x35},

		{0x3821, 0x01},
		{0x4713, 0x00},
		{0x4600, 0x80},
		{0x3000, 0x00},
		{0x3002, 0x1c},
		{0x3004, 0xff},
		{0x3006, 0xc3},
		{0x4837, 0x0a},
		{0x3824, 0x01},

		{0x380c,MSB(3760)},
		{0x380d,LSB(3760)},
		{0x380e,MSB(496)},
		{0x380f,LSB(496)},
};

__flash__ const struct ov5640_reg common_config_2[]=
{
		{0x3618, 0x00},
		{0x3612, 0x29},
		{0x3708, 0x66},
		{0x3709, 0x12},
		{0x370c, 0x03},
		{0x3a02, 0x01},
		{0x3a03, 0xf0},
		{0x3a08, 0x01},
		{0x3a09, 0xbe},
		{0x3a0a, 0x01},
		{0x3a0b, 0x74},
		{0x3a0e, 0x01},
		{0x3a0d, 0x01},
		{0x3a13, 0x43},
		{0x3a14, 0x01},
		{0x3a15, 0xf0},
		{0x4001, 0x02},
		{0x4004, 0x02},
		{0x3000, 0x00},
		{0x3002, 0x00},
		{0x3004, 0xff},
		{0x3006, 0xff},
		{0x300e, 0x58},
		{0x302e, 0x00},
		{0x4300, 0x30},
		{0x501f, 0x00},
		{0x5684, 0x05},
		{0x5685, 0x00},
		{0x5686, 0x03},
		{0x5687, 0xc0},
		{0x4407, 0x04},
		{0x440e, 0x00},
		{0x460b, 0x35},
		{0x460c, 0x22},
		{0x4837, 0x16},
		{0x3824, 0x04},
		{0x5000, 0xa7},
		{0x5001, 0x83},
		{0x5180, 0xff},
		{0x5181, 0xf2},
		{0x5182, 0x00},
		{0x5183, 0x14},
		{0x5184, 0x25},
		{0x5185, 0x24},
		{0x5186, 0x09},
		{0x5187, 0x09},
		{0x5188, 0x09},
		{0x5189, 0x75},
		{0x518a, 0x54},
		{0x518b, 0xe0},
		{0x518c, 0xb2},
		{0x518d, 0x42},
		{0x518e, 0x3d},
		{0x518f, 0x56},
		{0x5190, 0x46},
		{0x5191, 0xf8},
		{0x5192, 0x04},
		{0x5193, 0x70},
		{0x5194, 0xf0},
		{0x5195, 0xf0},
		{0x5196, 0x03},
		{0x5197, 0x01},
		{0x5198, 0x04},
		{0x5199, 0x12},
		{0x519a, 0x04},
		{0x519b, 0x00},
		{0x519c, 0x06},
		{0x519d, 0x82},
		{0x519e, 0x38},
		{0x5381, 0x1e},
		{0x5382, 0x5b},
		{0x5383, 0x08},
		{0x5384, 0x0a},
		{0x5385, 0x7e},
		{0x5386, 0x88},
		{0x5387, 0x7c},
		{0x5388, 0x6c},
		{0x5389, 0x10},
		{0x538a, 0x01},
		{0x538b, 0x98},
		{0x5300, 0x08},
		{0x5301, 0x30},
		{0x5302, 0x10},
		{0x5303, 0x00},
		{0x5304, 0x08},
		{0x5305, 0x30},
		{0x5306, 0x08},
		{0x5307, 0x16},
		{0x5309, 0x08},
		{0x530a, 0x30},
		{0x530b, 0x04},
		{0x530c, 0x06},
		{0x5480, 0x01},
		{0x5481, 0x08},
		{0x5482, 0x14},
		{0x5483, 0x28},
		{0x5484, 0x51},
		{0x5485, 0x65},
		{0x5486, 0x71},
		{0x5487, 0x7d},
		{0x5488, 0x87},
		{0x5489, 0x91},
		{0x548a, 0x9a},
		{0x548b, 0xaa},
		{0x548c, 0xb8},
		{0x548d, 0xcd},
		{0x548e, 0xdd},
		{0x548f, 0xea},
		{0x5490, 0x1d},
		{0x5580, 0x02},
		{0x5583, 0x40},
		{0x5584, 0x10},
		{0x5589, 0x10},
		{0x558a, 0x00},
		{0x558b, 0xf8},
		{0x5800, 0x23},
		{0x5801, 0x14},
		{0x5802, 0x0f},
		{0x5803, 0x0f},
		{0x5804, 0x12},
		{0x5805, 0x26},
		{0x5806, 0x0c},
		{0x5807, 0x08},
		{0x5808, 0x05},
		{0x5809, 0x05},
		{0x580a, 0x08},
		{0x580b, 0x0d},
		{0x580c, 0x08},
		{0x580d, 0x03},
		{0x580e, 0x00},
		{0x580f, 0x00},
		{0x5810, 0x03},
		{0x5811, 0x09},
		{0x5812, 0x07},
		{0x5813, 0x03},
		{0x5814, 0x00},
		{0x5815, 0x01},
		{0x5816, 0x03},
		{0x5817, 0x08},
		{0x5818, 0x0d},
		{0x5819, 0x08},
		{0x581a, 0x05},
		{0x581b, 0x06},
		{0x581c, 0x08},
		{0x581d, 0x0e},
		{0x581e, 0x29},
		{0x581f, 0x17},
		{0x5820, 0x11},
		{0x5821, 0x11},
		{0x5822, 0x15},
		{0x5823, 0x28},
		{0x5824, 0x46},
		{0x5825, 0x26},
		{0x5826, 0x08},
		{0x5827, 0x26},
		{0x5828, 0x64},
		{0x5829, 0x26},
		{0x582a, 0x24},
		{0x582b, 0x22},
		{0x582c, 0x24},
		{0x582d, 0x24},
		{0x582e, 0x06},
		{0x582f, 0x22},
		{0x5830, 0x40},
		{0x5831, 0x42},
		{0x5832, 0x24},
		{0x5833, 0x26},
		{0x5834, 0x24},
		{0x5835, 0x22},
		{0x5836, 0x22},
		{0x5837, 0x26},
		{0x5838, 0x44},
		{0x5839, 0x24},
		{0x583a, 0x26},
		{0x583b, 0x28},
		{0x583c, 0x42},
		{0x583d, 0xce},
		{0x5025, 0x00},
		{0x3a0f, 0x30},
		{0x3a10, 0x28},
		{0x3a1b, 0x30},
		{0x3a1e, 0x26},
		{0x3a11, 0x60},
		{0x3a1f, 0x14},
		{0x4407, 0x10},
};

__flash__ static const struct ov5640_reg ov5640_stream[] = {
		{0x300e, 0x59},
		{0x3008, 0x02},
};

__flash__ static const struct ov5640_reg ov5640_power_down[] = {
		{0x3008, 0x42},
		{0xFFFF, 100},
};

uint16_t ov5640_init(void)
{
	uint8_t pidh;
	uint8_t pidl;
	int8_t err;

	/* Wait a short period of time to allow camera to initialise. */
	delayms(10);

	do
	{
		/* Initialise the camera */
		err = ov5640_i2c_read(0x300a, &pidh);
		if (err) break;
		err = ov5640_i2c_read(0x300b, &pidl);
		if (err) break;

		tfp_printf("OV5640 version: %x\n", ((pidh << 8) | pidl));

		if ((pidh == MSB(OV5640_PID)) && (pidl == LSB(OV5640_PID)))
		{
			OV5640_WRITESCCBREG(configscript_reset);

			return ((pidh << 8) | pidl);
		}
	} while (0);

	ov5640_stop();
	return 0;
}

void ov5640_start(void)
{
	OV5640_WRITESCCBREG(ov5640_stream);

	tfp_printf("Start streaming...\n");
}

void ov5640_stop(void)
{
	OV5640_WRITESCCBREG(ov5640_power_down);

	tfp_printf("...streaming stopped.\n");
}

void ov5640_set(int resolution, int frame_rate, int format)
{

	// Set the camera module to output the requested
	// resolution, frame rate and format. If it is not
	// supported then ignore.
	OV5640_WRITESCCBREG(common_config_1);

	tfp_printf("OV5640 ");

	// QVGA supports uncompressed and MJPEG at 15 fps.
	if (resolution == CAMERA_MODE_QVGA)
	{
		tfp_printf(" QVGA");
		if (frame_rate == CAMERA_FRAME_RATE_15)
		{
			tfp_printf(" 15fps");
			OV5640_WRITESCCBREG(set_qvga_15fps);

			if (format == CAMERA_FORMAT_UNCOMPRESSED)
			{
				tfp_printf(" uncompressed");
				OV5640_WRITESCCBREG(set_yuv);
			}
			else if (format == CAMERA_FORMAT_MJPEG)
			{
				tfp_printf(" MJPEG");
				OV5640_WRITESCCBREG(set_jpeg);
			}
		}
	}
	// VGA supports uncompressed and MJPEG at 15 fps.
	else if (resolution == CAMERA_MODE_VGA)
	{
		tfp_printf(" VGA");
		if (frame_rate == CAMERA_FRAME_RATE_15)
		{
			tfp_printf(" 15fps");
			OV5640_WRITESCCBREG(set_vga_15fps);

			if (format == CAMERA_FORMAT_UNCOMPRESSED)
			{
				tfp_printf(" uncompressed");
				OV5640_WRITESCCBREG(set_yuv);
			}
			else if (format == CAMERA_FORMAT_MJPEG)
			{
				tfp_printf(" MJPEG");
				OV5640_WRITESCCBREG(set_jpeg);
			}
		}
	}
	// SVGA supports MJPEG only at 15 fps.
	else if (resolution == CAMERA_MODE_SVGA)
	{
		tfp_printf(" SVGA");

		if (frame_rate == CAMERA_FRAME_RATE_15)
		{
			tfp_printf(" 15fps");
			OV5640_WRITESCCBREG(set_svga_15fps);

			if (format == CAMERA_FORMAT_MJPEG)
			{
				tfp_printf(" MJPEG");
				OV5640_WRITESCCBREG(set_jpeg);
			}
		}
	}
	// SVGA supports MJPEG only at 15 fps.
	else if (resolution == CAMERA_MODE_XGA)
	{
		tfp_printf(" XGA");
		if (frame_rate == CAMERA_FRAME_RATE_15)
		{
			tfp_printf(" 15fps");
			OV5640_WRITESCCBREG(set_xga_15fps);

			if (format == CAMERA_FORMAT_MJPEG)
			{
				tfp_printf(" MJPEG");
				OV5640_WRITESCCBREG(set_jpeg);
			}
		}
	}

	tfp_printf("\n");

	OV5640_WRITESCCBREG(common_config_2);
}
