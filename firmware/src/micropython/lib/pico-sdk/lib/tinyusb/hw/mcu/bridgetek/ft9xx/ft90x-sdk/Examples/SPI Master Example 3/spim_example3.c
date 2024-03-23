/**
  @file
  @brief
  SPI Master Example 3

  Use 4-bit mode to drive a HD44780 compatible character LCD

*/
/*
 * ============================================================================
 * History
 * =======
 * 12 Nov 2014 : Created
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
#include <string.h>
#include <ft900.h>
#include "tinyprintf.h"

#define MIN(a,b) (((a)<(b))?a:b)
#define MAX(a,b) (((a)>(b))?a:b)
#define BIT_IS_SET(x,bit) (((x) & (1<<bit)) > 0)

/* LCD Config */
#define LCD_WIDTH (16)
#define LCD_HEIGHT (2)

#if defined(__FT900__)
/* GPIO Pin assignments */
#define GPIO_UART0_TX 	48
#define GPIO_UART0_RX 	49

#define GPIO_SCK    (27)
#define PAD_SCK     (pad_spim_sck)
#define GPIO_SS     (35)
#define PAD_SS      (pad_spim_ss3)
#define GPIO_MOSI   (29)
#define PAD_MOSI    (pad_spim_mosi)
#define GPIO_MISO   (30)
#define PAD_MISO    (pad_spim_miso)
#define GPIO_IO2    (31)
#define PAD_IO2     (pad_spim_io2)
#define GPIO_IO3    (32)
#define PAD_IO3     (pad_spim_io3)

#define GPIO_RS     (GPIO_SS)
#define PAD_RS      (pad_gpio35)
#define GPIO_E      (GPIO_SCK)
#define PAD_E       (pad_gpio27)
#define GPIO_DB4    (GPIO_MOSI)
#define PAD_DB4     (pad_gpio29)
#define GPIO_DB5    (GPIO_MISO)
#define PAD_DB5     (pad_gpio30)
#define GPIO_DB6    (GPIO_IO2)
#define PAD_DB6     (pad_gpio31)
#define GPIO_DB7    (GPIO_IO3)
#define PAD_DB7     (pad_gpio32)

#elif defined(__FT930__)

#define GPIO_UART0_TX 	23
#define GPIO_UART0_RX 	22

#define GPIO_SCK    (34)
#define PAD_SCK     (pad_spim_sck)
#define GPIO_SS     (30)
#define PAD_SS      (pad30_spim_ss0)
#define GPIO_MOSI   (36)
#define PAD_MOSI    (pad_spim_mosi)
#define GPIO_MISO   (35)
#define PAD_MISO    (pad_spim_miso)
#define GPIO_IO2    (37)
#define PAD_IO2     (pad_spim_io2)
#define GPIO_IO3    (38)
#define PAD_IO3     (pad_spim_io3)

#define GPIO_RS     (GPIO_SS)
#define PAD_RS      (pad_gpio30)
#define GPIO_E      (GPIO_SCK)
#define PAD_E       (pad_gpio34)
#define GPIO_DB4    (GPIO_MOSI)
#define PAD_DB4     (pad_gpio36)
#define GPIO_DB5    (GPIO_MISO)
#define PAD_DB5     (pad_gpio35)
#define GPIO_DB6    (GPIO_IO2)
#define PAD_DB6     (pad_gpio37)
#define GPIO_DB7    (GPIO_IO3)
#define PAD_DB7     (pad_gpio38)

#endif

/* Nibble manipulation */
#define NIBBLE_HI(a) ((a >> 4) & 0xF)
#define NIBBLE_LO(a) (a & 0xF)
#define NIBBLE_SWAP(a) ((a >> 4) | (a << 4))

/* HD44780 Commands */
#define HD44780_CLEAR_DISPLAY() (0x01)
#define HD44780_HOME() (0x02)
#define HD44780_ENTRY_MODE_SET(id,sh) (0x04 | ((id&1)<<1) | ((sh&1)<<0))
#define HD44780_DISPLAY_ON_OFF(d,c,b) (0x08 | ((d&1)<<2) | ((c&1)<<1) | ((b&1)<<0))
#define HD44780_CURSOR_DISPLAY_SHIFT(sc,rl) (0x10 | ((sc&1)<<3) | ((rl&1)<<2))
#define HD44780_FUNCTION_SET(dl,n,f) (0x20 | ((dl&1)<<4) | ((n&1)<<3) | ((f&1)<<2))
#define HD44780_SET_CGRAM_ADDR(addr) (0x40 | (addr&0x3F))
#define HD44780_SET_DDRAM_ADDR(addr) (0x80 | (addr&0x7F))

#define HD44780_DDRAM_ADDR(line,col) ( (0x40 * (line&1)) | (col%27) )

/* LCD GPIO Control */
#define LCD_RS(level) gpio_write(GPIO_RS, level)
#define LCD_E(level) gpio_write(GPIO_E, level)
#define LCD_DB4(level) gpio_write(GPIO_DB4, level)
#define LCD_DB5(level) gpio_write(GPIO_DB5, level)
#define LCD_DB6(level) gpio_write(GPIO_DB6, level)
#define LCD_DB7(level) gpio_write(GPIO_DB7, level)



void setup(void);
void loop(void);
void lcd_write4(uint8_t rs, uint8_t data);
void lcd_write8(uint8_t rs, uint8_t data);
void lcd_putc(char c);
void lcd_puts(char* s);
void lcd_goto(uint8_t line, uint8_t col);
void myputc(void* p, char c);

static uint8_t lcd_last_width = 0;

const static char* anim[] = {
   /*1234567890123456*/
    "|               ",
    " \x7E              ",
    "  \x7E             ",
    "   \x7E            ",
    "    \x7E           ",
    "     \x7E          ",
    "      \x7E         ",
    "       \x7E        ",
    "        \x7E       ",
    "         \x7E      ",
    "          \x7E     ",
    "           \x7E    ",
    "            \x7E   ",
    "             \x7E  ",
    "              \x7E ",
    "               |",
    "              \x7F ",
    "             \x7F  ",
    "            \x7F   ",
    "           \x7F    ",
    "          \x7F     ",
    "         \x7F      ",
    "        \x7F       ",
    "       \x7F        ",
    "      \x7F         ",
    "     \x7F          ",
    "    \x7F           ",
    "   \x7F            ",
    "  \x7F             ",
    " \x7F              "
};
const static uint8_t anim_len = 30;

const static char scroll[] =
    "SPI Master Example 3     Copyright Bridgetek Pte Ltd";
const static uint8_t scroll_len = 52;

int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void setup(void)
{
    /* Enable the UART Device... */
    sys_enable(sys_device_uart0);
    /* Set UART0 GPIO functions to UART0_TXD and UART0_RXD... */
    gpio_function(GPIO_UART0_TX, pad_uart0_txd); /* UART0 TXD */
    gpio_function(GPIO_UART0_RX, pad_uart0_rxd); /* UART0 RXD */
    uart_open(UART0,                    /* Device */
              1,                        /* Prescaler = 1 */
              UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */

    /* Print out a welcome message... */
    uart_puts(UART0,
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to SPI Master Example 3... \r\n"
        "\r\n"
        "Use 4-bit mode to drive a HD44780 compatible PCD\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(UART0,myputc);

    /* Enable the SPI Master device... */
    sys_enable(sys_device_spi_master);


    spi_init(SPIM, spi_dir_master, spi_mode_0, 512);
    spi_option(SPIM, spi_option_bus_width, 4);

    /* Initialise the LCD... */
    tfp_printf("Initialising the LCD\r\n");

    gpio_function(GPIO_RS, PAD_RS); gpio_dir(GPIO_RS, pad_dir_output);
    gpio_function(GPIO_E, PAD_E); gpio_dir(GPIO_E, pad_dir_output);
    gpio_function(GPIO_DB4, PAD_DB4); gpio_dir(GPIO_DB4, pad_dir_output);
    gpio_function(GPIO_DB5, PAD_DB5); gpio_dir(GPIO_DB5, pad_dir_output);
    gpio_function(GPIO_DB6, PAD_DB6); gpio_dir(GPIO_DB6, pad_dir_output);
    gpio_function(GPIO_DB7, PAD_DB7); gpio_dir(GPIO_DB7, pad_dir_output);

    LCD_E(0);

    delayms(100);

    /* Function set: Interface is 8 bits long */
    lcd_write4(0, NIBBLE_HI(HD44780_FUNCTION_SET(1,0,0)));

    delayms(5);

    /* Function set: Interface is 8 bits long */
    lcd_write4(0, NIBBLE_HI(HD44780_FUNCTION_SET(1,0,0)));

    delayus(100);

    /* Function set: Interface is 8 bits long */
    lcd_write4(0, NIBBLE_HI(HD44780_FUNCTION_SET(1,0,0)));

    /* Function set: Interface is 4 bits long */
    lcd_write4(0, NIBBLE_HI(HD44780_FUNCTION_SET(0,0,0)));

    /* Function set: Interface is 4 bits long, 2 lines, 5x8 font */
    lcd_write8(0, HD44780_FUNCTION_SET(0,(LCD_HEIGHT > 1),0));
    /* Display off */
    lcd_write8(0, HD44780_DISPLAY_ON_OFF(0,0,0));
    /* Clear display */
    lcd_write8(0, HD44780_CLEAR_DISPLAY());
    delayms(2);
    /* Entry Mode Set: Increment, no shift */
    lcd_write8(0, HD44780_ENTRY_MODE_SET(1,0));
    delayms(10);

    /* Display on, no cursor, no blink */
    lcd_write8(0, HD44780_DISPLAY_ON_OFF(1,0,0));
}

void loop(void)
{
    static uint8_t anim_idx = 0;
    static uint8_t scroll_skip = 0;

    /* Scroll through some text and put it on the first line */
    scroll_skip++;
    if (scroll_skip == 10)
    {
        static char line[LCD_WIDTH+1] = {0};
        static int8_t fromoffset = 0;
        static int8_t tooffset = LCD_WIDTH;

        memset(line, ' ', LCD_WIDTH);

        /* If we are just starting... */
        if (tooffset > 0)
        {
            tooffset--;
        }
        else
        {
            /* We are midway through scrolling... */
            fromoffset++;

            /* Done? ... */
            if (fromoffset == scroll_len)
            {
                /* Restart */
                fromoffset = 0;
                tooffset = LCD_WIDTH;
            }
        }

        strncpy(line + tooffset,
                scroll + fromoffset,
                MIN(LCD_WIDTH-tooffset, scroll_len-fromoffset));

        lcd_goto(0,0);
        lcd_puts(line);

        scroll_skip = 0;
    }

    /* Go through the animation on the second line */
    {
        lcd_goto(1,0);
        lcd_puts((char *) anim[anim_idx]);

        anim_idx++;
        if (anim_idx == anim_len) anim_idx = 0;
    }

    delayms(50);
}

/** LCD 4bit write
 *  @param rs The state of RS for this transfer
 *  @param data The data to transfer (only the lower 4 bits are used) */
/*  (This function uses GPIO to write nibbles to the LCD since SPI cannot write
 *  a single nibble) */
void lcd_write4(uint8_t rs, uint8_t data)
{
    /* Check to see if we should set up the pins */
    if (lcd_last_width != 4)
    {
        gpio_function(GPIO_RS, PAD_RS); gpio_dir(GPIO_RS, pad_dir_output);
        gpio_function(GPIO_E, PAD_E); gpio_dir(GPIO_E, pad_dir_output);
        gpio_function(GPIO_DB4, PAD_DB4); gpio_dir(GPIO_DB4, pad_dir_output);
        gpio_function(GPIO_DB5, PAD_DB5); gpio_dir(GPIO_DB5, pad_dir_output);
        gpio_function(GPIO_DB6, PAD_DB6); gpio_dir(GPIO_DB6, pad_dir_output);
        gpio_function(GPIO_DB7, PAD_DB7); gpio_dir(GPIO_DB7, pad_dir_output);
        lcd_last_width = 4;
    }

    if (rs) LCD_RS(1);
    else    LCD_RS(0);

    LCD_DB4(BIT_IS_SET(data,0));
    LCD_DB5(BIT_IS_SET(data,1));
    LCD_DB6(BIT_IS_SET(data,2));
    LCD_DB7(BIT_IS_SET(data,3));

    LCD_E(1);
    delayms(1);
    LCD_E(0);
}

/** LCD 8bit write
 *  @param rs The state of RS for this transfer
 *  @param data The data to transfer */
void lcd_write8(uint8_t rs, uint8_t data)
{
    /* Check to see if we should set up the pins */
    if (lcd_last_width != 8)
    {
        gpio_function(GPIO_RS, PAD_RS); gpio_dir(GPIO_RS, pad_dir_output);
        gpio_function(GPIO_SCK, PAD_SCK);
        gpio_function(GPIO_MOSI, PAD_MOSI);
        gpio_function(GPIO_MISO, PAD_MISO);
        gpio_function(GPIO_IO2, PAD_IO2);
        gpio_function(GPIO_IO3, PAD_IO3);
        lcd_last_width = 8;
    }

    if (rs) LCD_RS(1);
    else    LCD_RS(0);

    spi_write(SPIM, data);
    delayus(100);

}

/** LCD put char
 *  @param c The character to display */
void lcd_putc(char c)
{
    lcd_write8(1, c);
}

/** LCD put string
 *  @param s The string to display */
void lcd_puts(char* s)
{
    while(*s)
        lcd_putc(*s++);
}

/** LCD goto
 *  Place the cursor at a certain position
 *  @param line The line number to go to starting from 0
 *  @param col The column number to go to starting from 0 */
void lcd_goto(uint8_t line, uint8_t col)
{
    lcd_write8(0, HD44780_SET_DDRAM_ADDR(HD44780_DDRAM_ADDR(line,col)));
}

void myputc(void* p, char c)
{
    uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}
