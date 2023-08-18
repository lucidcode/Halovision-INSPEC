/**
  @file
  @brief
  Camera Example 1
  Get a frame from a CMOS Camera and print it out to the console.
*/
/*
 * ============================================================================
 * History
 * =======
 * 3 Oct 2014 : Created
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd 
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
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
#include "ft900.h"
#include "tinyprintf.h"

#define QCIF_HBLANK (12)
#define QCIF_WIDTH (176+QCIF_HBLANK)
#define QCIF_HEIGHT (144)
#define QCIF_BPP (2)
#define QCIF_LINE_BYTES (QCIF_WIDTH*QCIF_BPP)
#define QCIF_FRAME_BYTES (QCIF_LINE_BYTES*QCIF_HEIGHT)

#define OV7670_I2C_ADDR (0x42)
#define OV9650_I2C_ADDR (0x60)

#define RED(x) ((x >> 8) & 0x00F8)
#define GREEN(x) ((x >> 3) & 0x00F8)
#define BLUE(x) ((x << 3) & 0x00F8)
#define VALUE(r,g,b) (r > g ? (r > b ? r : b) : (g > b ? g : b))
#define COLOR_THRES (128)
#define COL_STEP (3)
#define LINE_STEP (4)

static uint16_t volatile camera_buffer[QCIF_HEIGHT][QCIF_WIDTH]; /* 50688 */
static uint16_t volatile camera_line = 0;

static volatile uint8_t vsync = 0;

void setup(void);
void loop(void);
void wait_for_vsync(void);
void vsync_ISR(void);
void cam_ISR(void);
void ov7670_init(void);
void ov9650_init(void);
void myputc(void* p, char c);

int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void hexdump(uint8_t *data, uint16_t len)
{
    uint16_t i,j;

    j = 0;
    do
    {
        tfp_printf("0x%04x: ", j);
        for (i = 0; i < 32; i++)
        {
            if (i+j < len)
                tfp_printf("%02X ", data[i+j]);
            else
                tfp_printf("   ");
        }

        tfp_printf(" | ");

        for (i = 0; i < 32; i++)
        {
            if (i+j < len)
            {
                if (data[i+j] < ' ' || data[i+j] > '~' )
                    tfp_printf(".");
                else
                    tfp_printf("%c", data[i+j]);
            }
            else
            {
                tfp_printf(" ");
            }
        }
        tfp_printf("\r\n");

        j += 32;
    }
    while(j < len);
}

void setup(void)
{
    /* Enable the UART Device... */
    sys_enable(sys_device_uart0);
    /* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
    uart_open(UART0,                    /* Device */
              1,                        /* Prescaler = 1 */
              UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */

    /* Print out a welcome message... */
    uart_puts(UART0,
        "\x1B[0m" /* ANSI/VT100 - Reset All Atributes */
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to Camera Example 1... \r\n"
        "\r\n"
        "Get a frame from a CMOS Camera and print it out to the console.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(NULL,myputc);

    /* Set up the Camera Interface */
    cam_disable_interrupt();
    cam_stop();
    cam_flush();

    sys_enable(sys_device_camera);
    gpio_function(6, pad_cam_xclk);  /* XCLK */
    gpio_function(7, pad_cam_pclk);  /* PCLK */
    //gpio_function(8, pad_cam_vd);  /* VD */
    gpio_function(9, pad_cam_hd);  /* HD */
    gpio_function(10, pad_cam_d7); /* D7 */
    gpio_function(11, pad_cam_d6); /* D6 */
    gpio_function(12, pad_cam_d5); /* D5 */
    gpio_function(13, pad_cam_d4); /* D4 */
    gpio_function(14, pad_cam_d3); /* D3 */
    gpio_function(15, pad_cam_d2); /* D2 */
    gpio_function(16, pad_cam_d1); /* D1 */
    gpio_function(17, pad_cam_d0); /* D0 */

    /* Set VD as a GPIO input */
    gpio_function(8, pad_gpio8);
    gpio_dir(8, pad_dir_input);
    interrupt_attach(interrupt_gpio, (uint8_t)interrupt_gpio, vsync_ISR);
    gpio_interrupt_enable(8, gpio_int_edge_falling); /* VD */
	delayms(1);
    /* Set up I2C */
    sys_enable(sys_device_i2c_master);

    gpio_function(44, pad_i2c0_scl); /* I2C0_SCL */
    gpio_function(45, pad_i2c0_sda); /* I2C0_SDA */
    gpio_function(46, pad_i2c1_scl); /* I2C1_SCL */
    gpio_function(47, pad_i2c1_sda); /* I2C1_SDA */

    i2cm_init(I2CM_NORMAL_SPEED, 100000);

    /* Initialise the camera */
    {
        uint8_t pidh;

        i2cm_read(OV7670_I2C_ADDR, 0x0A, &pidh, 1);
        if (pidh == 0x76)
        {
            uart_puts(UART0, "Found OV7670\r\n");
            ov7670_init();
        }

        i2cm_read(OV9650_I2C_ADDR, 0x0A, &pidh, 1);
        if (pidh == 0x96)
        {
            uart_puts(UART0, "Found OV965x\r\n");
            ov9650_init();
        }
    }

    /* Clock data in when VREF is low and HREF is high */
    cam_init(cam_trigger_mode_1, cam_clock_pol_raising);
    interrupt_attach(interrupt_camera, (uint8_t)interrupt_camera, cam_ISR);

    interrupt_enable_globally();


    uart_puts(UART0,"3...");
    delayms(1000);
    uart_puts(UART0,"2...");
    delayms(1000);
    uart_puts(UART0,"1...\r\n");
    delayms(1000);

    /* Capture a frame */
    camera_line = 0;

    cam_set_threshold(QCIF_LINE_BYTES);

    wait_for_vsync();
    cam_enable_interrupt();
    cam_start(QCIF_LINE_BYTES);

}

void loop(void)
{
    uint16_t pix = 0;
    uint8_t r, g, b, v, pix2;
    uint16_t line, col;
    static const char* vt100_colors[8] = {
        "\x1B[30;40m", /* 000: Black */
        "\x1B[34;40m", /* 001: Blue */
        "\x1B[32;40m", /* 010: Green */
        "\x1B[36;40m", /* 011: Cyan */
        "\x1B[31;40m", /* 100: Red */
        "\x1B[35;40m", /* 101: Magenta */
        "\x1B[33;40m", /* 110: Yellow */
        "\x1B[37;40m", /* 111: White */
    };
    static const char vt100_values[4] = {
        0xB0, // 25% block
        0xB1, // 50% block
        0xB2, // 75% block
        0xDB, // Full block
    };

    /* Wait for a frame to be captured... */
    while(camera_line < QCIF_HEIGHT);
    tfp_printf("%d by %d ASCII Art\r\n", QCIF_HEIGHT/LINE_STEP, QCIF_WIDTH/COL_STEP);

    for (line = 0; line < QCIF_HEIGHT; line+=LINE_STEP)
    {
        for (col = 0; col < QCIF_WIDTH; col+=COL_STEP)
        {
            pix = camera_buffer[line][col];
            r = RED(pix);
            g = GREEN(pix);
            b = BLUE(pix);
            v = VALUE(r,g,b);
            pix2 = (r>COLOR_THRES?0x04:0) | (g>COLOR_THRES?0x02:0) | (b>COLOR_THRES?0x01:0);

            uart_puts(UART0, (char *) vt100_colors[pix2]);
            uart_write(UART0, vt100_values[v >> 6]);
        }
        uart_puts(UART0, (char *) vt100_colors[0]);
        uart_puts(UART0, "\r\n");
    }

    uart_puts(UART0, "\x1B[0m"); /* ANSI/VT100 - Reset All Atributes */

    /* Halt... */
    for(;;);
}

void cam_ISR(void)
{
    static const uint16_t linebytes = QCIF_LINE_BYTES;

    while (cam_available() >= QCIF_LINE_BYTES)
    {
        asm("streamin.l %0,%1,%2" \
        : \
        :"r"(camera_buffer[camera_line]), "r"(&(CAM->CAM_REG3)), "r"(linebytes));

        camera_line++;

        if (camera_line == QCIF_HEIGHT)
        {
            cam_stop();
            cam_disable_interrupt();
            break;
        }
    }
}

void vsync_ISR(void)
{
    if (gpio_is_interrupted(8))
    {
        vsync = 1;
    }
}

void wait_for_vsync(void)
{
    vsync = 0;
    while (!vsync)
    {
        cam_flush();
    }
}

void ov7670_init(void)
{
    static const uint8_t init_qcif[][2] =
    {
        {0x12, 0x08 /*COM7_FMT_QCIF*/ | 0x04 /*COM7_RGB*/},
        {0x8c, 0x00},//2 / RGB444: 0b [7-2] Reserved, [1] RGB444 [0] XR GB(0) , RG BX(1)
        {0x04, 0x00},//3 / COM1 : CCIR656 Format Disable
        {0x40, 0x10}, //COM15 DVE: RGB565 mode
        {0x14, 0x18},//5 / COM9 : 4x gain ceiling

        {0x11, 2},
        {0x6b, 0x4a},//72/ DBLV : PPL Control, PLL clk x 4, together with CLKRC it defines the frame rate

        {0x32, 0x86},//7 / HREF : DVE; this one MUST be 0xb6 or otherwise my images get all scrambled DVE: was 0xb6
        {0x17, 0x13},//8 / HSTART: HREF Column start high 8-bit DVE: was 0x13
        {0x18, 0x0d},//9 / HSTOP : HREF Column end high 8-bit  DVE: was 0x01

        {0x19, 0x02},//10/ VSTRT : VSYNC Start high 8-bit DVE: was 0x02
        {0x1a, 0x79},//11/ VSTOP : VSYNC End high 8-bit, DVE: was 0x7a
        {0x03, 0x0f},//12/ VREF : Vertical Frame Control, DVE: was 0x0a

        {0x0c, 0x00},//13/ COM3 : DVE: this one can enable windowing, scaling, digital cropping and since it was set to 0x00 the settings after this line were NOT effective! (
        {0x3e, 0x00},//14/ COM14 : Pixel Clock Devider DVE: was 0x00
        //
        // Mystery Scaling Numbers
        //
        {0x70, 0x3a},//15/ SCALING XSC : Horizontal Scale Factor    DVE: was 0x3a
        {0x71, 0x35},//16/ SCALING YSC : Vertical Scale Factor DVE: was 0x35
        {0x72, 0x11},//17/ SCALING DCW : DCW Control     DVE: was 0x11
        {0x73, 0xf0},//18/ SCALING PCLK: Scaling Pixel Clock Devider    DVE: was 0xf0
        {0xa2, 0x02},//19/ SCALING PCLK: Scaling Pixel Delay
        {0x15, 0x00},//20/ COM10 : VSYNC , HREF , PCLK Settings

        //
        // Gamma Curve Values
        //
        {0x7a, 0x20},//21/ SLOP : Gamma Curve Highest Segment Slope
        {0x7b, 0x10},//22/ GAM1 : Gamme Curve 1st Segment
        {0x7c, 0x1e},//23/ GAM2 : Gamme Curve 2st Segment
        {0x7d, 0x35},//24/ GAM3 : Gamme Curve 3st Segment
        {0x7e, 0x5a},//25/ GAM4 : Gamme Curve 4st Segment
        {0x7f, 0x69},//26/ GAM5 : Gamme Curve 5st Segment
        {0x80, 0x76},//27/ GAM6 : Gamme Curve 6st Segment
        {0x81, 0x80},//28/ GAM7 : Gamme Curve 7st Segment
        {0x82, 0x88},//29/ GAM8 : Gamme Curve 8st Segment
        {0x83, 0x8f},//30/ GAM9 : Gamme Curve 9st Segment
        {0x84, 0x96},//31/ GAM10: Gamme Curve 10st Segment
        {0x85, 0xa3},//32/ GAM11: Gamme Curve 11st Segment
        {0x86, 0xaf},//33/ GAM12: Gamme Curve 12st Segment
        {0x87, 0xc4},//34/ GAM13: Gamme Curve 13st Segment
        {0x88, 0xd7},//35/ GAM14: Gamme Curve 14st Segment
        {0x89, 0xe8},//36/ GAM15: Gamme Curve 15st Segment

        //
        // Automatic Gain Control and AEC Parameters
        //
        {0x13, 0x00},//37/ COM8 : Fast AGC/AEC Algorithm
        {0x00, 0x00},//38/ GAIN
        {0x10, 0x00},//39/ AECH
        {0x0d, 0x00},//40/ COM4 :
        {0x14, 0x18},//41/ COM9 : Automatic Gain Ceiling : 8x
        {0xa5, 0x05},//42/ BD50MAX: 50 Hz Banding Step Limit
        {0xab, 0x07},//43/ BD60MAX: 60 Hz Banding Step Limit
        {0x24, 0x95},//44/ AGC - Stable Operating Region Upper Limit
        {0x25, 0x33},//45/ AGC - Stable Operating Region Lower Limit
        {0x26, 0xe3},//46/ AGC - Fast Mode Operating Region
        {0x9f, 0x78},//47/ HAECC1 : Histogram based AEC Control 1
        {0xa0, 0x68},//48/ HAECC2 : Histogram based AEC Control 2
        {0xa1, 0x03},//49/ Reserved
        {0xa6, 0xd8},//50/ HAECC3 : Histogram based AEC Control 3
        {0xa7, 0xd8},//51/ HAECC4 : Histogram based AEC Control 4
        {0xa8, 0xf0},//52/ HAECC5 : Histogram based AEC Control 5
        {0xa9, 0x90},//53/ HAECC6 : Histogram based AEC Control 6
        {0xaa, 0x94},//54/ HAECC7 : AEC Algorithm Selection
        {0x13, 0xe5},//55/ COM8 : Fast AGC Algorithm, Unlimited Step Size , Banding Filter ON, AGC and AEC enable.

        //
        // Reserved Values without function specification
        //
        {0x0e, 0x61},//56/ COM5 : Reserved
        {0x0f, 0x4b},//57/ COM6 : Reserved
        {0x16, 0x02},//58/ Reserved
        //{0x1e, 0xb},//59/ MVFP : Mirror/Vflip disabled ( 0x37 enabled ) //DVE: was 0x0f is now 0x2f so mirror ENABLED. Was required for my TFT to show pictures without mirroring (probably my TFT setting is also mirrored)
        {0x1e, 0x1b}, //59 MVFP: Mirrored
        {0x21, 0x02},//60/ Reserved
        {0x22, 0x91},//61/ Reserved
        {0x29, 0x07},//62/ Reserved
        {0x33, 0x0b},//63/ Reserved
        {0x35, 0x0b},//64/ Reserved
        {0x37, 0x1d},//65/ Reserved
        {0x38, 0x71},//66/ Reserved
        {0x39, 0x2a},//67/ Reserved
        {0x3c, 0x78},//68/ COM12 : Reserved
        {0x4d, 0x40},//69/ Reserved
        {0x4e, 0x20},//70/ Reserved
        {0x69, 0x00},//71/ GFIX : Fix Gain for RGB Values
        //{0x6b, 0x80},//DEBUG DBLV: aantal frames per seconde dmv PCLK hoger of lager, staat nu op INPUT CLK x6 en INT regulator enabled
        {0x74, 0x10},//73/ Reserved
        {0x8d, 0x4f},//74/ Reserved
        {0x8e, 0x00},//75/ Reserved
        {0x8f, 0x00},//76/ Reserved
        {0x90, 0x00},//77/ Reserved
        {0x91, 0x00},//78/ Reserved
        {0x92, 0x00},//79/ Reserved
        {0x96, 0x00},//80/ Reserved
        {0x9a, 0x00},//81/ Reserved
        {0xb0, 0x84},//82/ Reserved
        {0xb1, 0x0c},//83/ Reserved
        {0xb2, 0x0e},//84/ Reserved
        {0xb3, 0x82},//85/ Reserved
        {0xb8, 0x0a},//86/ Reserved

        //
        // Reserved Values without function specification
        //
        {0x43, 0x0a},//87/ Reserved
        {0x44, 0xf0},//88/ Reserved
        {0x45, 0x34},//89/ Reserved
        {0x46, 0x58},//90/ Reserved
        {0x47, 0x28},//91/ Reserved
        {0x48, 0x3a},//92/ Reserved
        {0x59, 0x88},//93/ Reserved
        {0x5a, 0x88},//94/ Reserved
        {0x5b, 0x44},//95/ Reserved
        {0x5c, 0x67},//96/ Reserved
        {0x5d, 0x49},//97/ Reserved
        {0x5e, 0x0e},//98/ Reserved
        {0x64, 0x04},//99/ Reserved
        {0x65, 0x20},//100/ Reserved
        {0x66, 0x05},//101/ Reserved
        {0x94, 0x04},//102/ Reserved
        {0x95, 0x08},//103/ Reserved
        {0x6c, 0x0a},//104/ Reserved
        {0x6d, 0x55},//105/ Reserved
        {0x6e, 0x11},//106/ Reserved
        {0x6f, 0x9f},//107/ Reserved
        {0x6a, 0x40},//108/ Reserved
        {0x01, 0x40},//109/ REG BLUE : Reserved
        {0x02, 0x40},//110/ REG RED : Reserved
        {0x13, 0xe7},//111/ COM8 : FAST AEC, AEC unlimited STEP, Band Filter, AGC , ARC , AWB enable.

        //
        // Matrix Coefficients
        //
        {0x4f, 0x80},//112/ MTX 1 : Matrix Coefficient 1
        {0x50, 0x80},//113/ MTX 2 : Matrix Coefficient 2
        {0x51, 0x00},//114/ MTX 3 : Matrix Coefficient 3
        {0x52, 0x22},//115/ MTX 4 : Matrix Coefficient 4
        {0x53, 0x5e},//116/ MTX 5 : Matrix Coefficient 5
        {0x54, 0x80},//117/ MTX 6 : Matrix Coefficient 6
        {0x58, 0x9e},//118/ MTXS : Matrix Coefficient Sign for Coefficient 5 to 0
        {0x41, 0x08},//119/ COM16 : AWB Gain enable
        {0x3f, 0x00},//120/ EDGE : Edge Enhancement Adjustment
        {0x75, 0x05},//121/ Reserved
        {0x76, 0xe1},//122/ Reserved
        {0x4c, 0x00},//123/ Reserved
        {0x77, 0x01},//124/ Reserved
        {0x3d, 0xc0},//125/ COM13
        {0x4b, 0x09},//126/ Reserved
        {0xc9, 0x60},//127/ Reserved
        {0x41, 0x38},//128/ COM16
        {0x56, 0x40},//129/ Reserved
        {0x34, 0x11},//130/ Reserved
        {0x3b, 0x12},//131/ COM11 : Exposure and Hz Auto detect enabled.
        {0xa4, 0x88},//132/ Reserved
        {0x96, 0x00},//133/ Reserved
        {0x97, 0x30},//134/ Reserved
        {0x98, 0x20},//135/ Reserved
        {0x99, 0x30},//136/ Reserved
        {0x9a, 0x84},//137/ Reserved
        {0x9b, 0x29},//138/ Reserved
        {0x9c, 0x03},//139/ Reserved
        {0x9d, 0x4c},//140/ Reserved
        {0x9e, 0x3f},//141/ Reserved
        {0x78, 0x04},//142/ Reserved

        //
        // Mutliplexing Registers
        //
        {0x79, 0x01},//143/ Reserved
        {0xc8, 0xf0},//144/ Reserved
        {0x79, 0x0f},//145/ Reserved
        {0xc8, 0x00},//146/ Reserved
        {0x79, 0x10},//147/ Reserved
        {0xc8, 0x7e},//148/ Reserved
        {0x79, 0x0a},//149/ Reserved
        {0xc8, 0x80},//150/ Reserved
        {0x79, 0x0b},//151/ Reserved
        {0xc8, 0x01},//152/ Reserved
        {0x79, 0x0c},//153/ Reserved
        {0xc8, 0x0f},//154/ Reserved
        {0x79, 0x0d},//155/ Reserved
        {0xc8, 0x20},//156/ Reserved
        {0x79, 0x09},//157/ Reserved
        {0xc8, 0x80},//158/ Reserved
        {0x79, 0x02},//159/ Reserved
        {0xc8, 0xc0},//160/ Reserved
        {0x79, 0x03},//161/ Reserved
        {0xc8, 0x40},//162/ Reserved
        {0x79, 0x05},//163/ Reserved
        {0xc8, 0x30},//164/ Reserved
        {0x79, 0x26},//165/ Reserved

        //
        // Additional Settings
        //
        {0x09, 0x00},//166/ COM2 : Output Drive Capability
        {0x55, 0x00},//167/ Brightness Control
    };

    uint32_t cmd = 0;
    uint32_t num_cmds = 0;
    uint8_t dat = 0;

    /* SCCB register reset */
    dat = 0x80;
    i2cm_write(OV7670_I2C_ADDR, 0x12, &dat, 1);
    delayms(250);

    num_cmds = sizeof(init_qcif)/2;
    for (cmd = 0; cmd < num_cmds; ++cmd)
    {
        i2cm_write(OV7670_I2C_ADDR, init_qcif[cmd][0], &(init_qcif[cmd][1]), 1);
        delayus(100);
    }
}

void ov9650_init(void)
{
	const uint8_t init_qcif[] =
	{
	       /*00*//*01*//*02*//*03*//*04*//*05*//*06*//*07*//*08*//*09*//*0a*//*0b*//*0c*//*0d*//*0e*//*0f*/
	/*00*/  0x12, 0x78, 0x48, 0x12, 0x03, 0x39, 0x39, 0x39, 0x38, 0x00, 0x96, 0x57, 0x00, 0x00, 0x01, 0x00
	/*10*/, 0x7B, 0x03, 0x63, 0xEF, 0x2A, 0x00, 0x24, 0x18, 0x0c, 0x01, 0x81, 0x00, 0x7F, 0xA2, 0x00, 0x00
	/*20*/, 0x80, 0x80, 0x80, 0x80, 0x3C, 0x36, 0x72, 0x08, 0x08, 0x15, 0x00, 0x00, 0x08, 0x00, 0x00, 0x38
	/*30*/, 0x08, 0x30, 0xa4, 0x00, 0x3F, 0x00, 0x3A, 0x04, 0x72, 0x57, 0xCA, 0x04, 0x0C, 0x99, 0x0e, 0x83
	/*40*/, 0xD0, 0x01, 0xC0, 0x0A, 0xF0, 0x46, 0x62, 0x2A, 0x3C, 0x48, 0xEC, 0xE9, 0xE9, 0xE9, 0xE9, 0x98
	/*50*/, 0x98, 0x00, 0x28, 0x70, 0x98, 0x00, 0x40, 0x80, 0x1A, 0x85, 0xA9, 0x64, 0x84, 0x53, 0x0E, 0xF0
	/*60*/, 0xF0, 0xF0, 0x00, 0x00, 0x02, 0x20, 0x00, 0x80, 0x80, 0x0A, 0x00, 0x4A, 0x04, 0x55, 0x00, 0x9D
	/*70*/, 0x06, 0x78, 0x11, 0x02, 0x3a, 0x35, 0x01, 0x02, 0x28, 0x00, 0x12, 0x08, 0x16, 0x30, 0x5E, 0x72
	/*80*/, 0x82, 0x8E, 0x9A, 0xA4, 0xAC, 0xB8, 0xC3, 0xD6, 0xE6, 0xF2, 0x24, 0x04, 0x80, 0x00, 0x00, 0x00
	/*90*/, 0x71, 0x6F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x80, 0x00, 0x00, 0x00, 0x02, 0x02, 0x6F
	/*a0*/, 0x6D, 0x40, 0x9D, 0x83, 0x50, 0x68, 0x40, 0x10, 0xC1, 0xEF, 0x92, 0x04, 0x80, 0x80, 0x80, 0x80
	/*b0*/, 0x04, 0x00, 0xF2, 0x20, 0x20, 0x00, 0xAF, 0xEE, 0xEE, 0x0C, 0x00, 0xAE, 0x7C, 0x7D, 0x7E, 0x7F
	/*c0*/, 0xAA, 0xC0, 0x01, 0x4E, 0x00, 0x2E, 0x05, 0x82, 0x06, 0xE0, 0xE8, 0xF0, 0xD8, 0x93, 0xE3, 0x74
	/*d0*/, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74
	/*e0*/, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74
	/*f0*/, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74, 0x74
	};

    uint8_t dat = 0;

    dat = 0x80;
    i2cm_write(OV9650_I2C_ADDR, 0x12, &dat, 1);
    delayms(250);

    i2cm_write(OV9650_I2C_ADDR, 0, init_qcif, sizeof(init_qcif));
}

void myputc(void* p, char c)
{
    uart_write(UART0, (uint8_t)c);
}
