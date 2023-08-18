/**
  @file
  @brief
  SPI Master Example 1

  Read and Write from a serial EEPROM (AT93C46D) using FIFOs to streamline
  transfers

  Using a 16 byte FIFO speeds up SPI transfers by eliminating the polling library
  calls to SPI.

*/
/*
 * ============================================================================
 * History
 * =======
 * 13 Nov 2014 : Created
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
#include <ft900.h>
#include "tinyprintf.h"



#if defined(__FT900__)

#define GPIO_UART0_TX 	48
#define GPIO_UART0_RX 	49

#define GPIO_SPIM_CLK	27
#define GPIO_SPIM_MISO	30
#define GPIO_SPIM_MOSI	29

#define SPI_CHANNEL (3)

#if SPI_CHANNEL == 0
#   define SPI_SS_GPIO (28)
#   define SPI_SS_FUNC (pad_gpio28)
#elif SPI_CHANNEL == 1
#   define SPI_SS_GPIO (33)
#   define SPI_SS_FUNC (pad_gpio33)
#elif SPI_CHANNEL == 2
#   define SPI_SS_GPIO (34)
#   define SPI_SS_FUNC (pad_gpio34)
#elif SPI_CHANNEL == 3
#   define SPI_SS_GPIO (35)
#   define SPI_SS_FUNC (pad_gpio35)
#else
#   error Set SPI_CHANNEL to 0, 1, 2, or 3
#endif

#elif defined(__FT930__)

#define GPIO_UART0_TX 	23
#define GPIO_UART0_RX 	22

#define GPIO_SPIM_CLK	34
#define GPIO_SPIM_MISO	35
#define GPIO_SPIM_MOSI	36

#define SPI_CHANNEL (0)

#if SPI_CHANNEL == 0
#   define SPI_SS_GPIO (30)
#   define SPI_SS_FUNC (pad_gpio30)
#elif SPI_CHANNEL == 1
#   define SPI_SS_GPIO (31)
#   define SPI_SS_FUNC (pad_gpio31)
#elif SPI_CHANNEL == 2
#   define SPI_SS_GPIO (32)
#   define SPI_SS_FUNC (pad_gpio32)
#elif SPI_CHANNEL == 3
#   define SPI_SS_GPIO (33)
#   define SPI_SS_FUNC (pad_gpio33)
#else
#   error Set SPI_CHANNEL to 0, 1, 2, or 3
#endif

#endif

#define EEPROM_SIZE (1024/8)
#define EEPROM_CMD_SIZE (3)
#define EEPROM_CMD_PACK(cmd,op,addr,data)           \
    do {                                            \
        cmd[0] = 0x80 | ((op&0x3)<<5) | ((addr&0x7F)>>2);  \
        cmd[1] = ((addr&0x3)<<6) | (data>>2);       \
        cmd[2] = ((data&0x3)<<6);                   \
    } while (0);
#define EEPROM_CMD_UNPACK_DATA(x) (uint8_t)(((x[1]<<3)&0xFF) | ((x[2]>>5)))



void setup(void);
void loop(void);
void ee_xfer(uint8_t *outp, uint8_t *inp, uint8_t size);
uint8_t ee_read(uint8_t addr);
void ee_ewen(void);
void ee_erase(uint8_t addr);
void ee_write(uint8_t addr, uint8_t data);
void ee_dump(void);
void hexdump(uint8_t *data, uint16_t len);
void myputc(void* p, char c);

static const uint8_t LOREM_IPSUM[EEPROM_SIZE] = {
    'L','o','r','e','m',' ','i','p','s','u','m',' ','d','o','l','o',
    'r',' ','s','i','t',' ','a','m','e','t',',',' ','c','o','n','s',
    'e','c','t','e','t','u','r',' ','a','d','i','p','i','s','c','i',
    'n','g',' ','e','l','i','t','.',' ','A','l','i','q','u','a','m',
    ' ','i','n','t','e','r','d','u','m',' ','e','r','o','s',' ','s',
    'i','t',' ','a','m','e','t',' ','l','o','r','e','m',' ','p','u',
    'l','v','i','n','a','r',',',' ','v','e','l',' ','p','o','s','u',
    'e','r','e',' ','l','e','o',' ','p','o','s','u','e','r','e','.'
};


/** Main Program */
int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

/** Setup */
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
        "Welcome to SPI Master Example 2... \r\n"
        "\r\n"
        "Read and Write from a serial EEPROM (AT93C46D) using FIFOs to \r\n"
        "streamline transfers\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(UART0,myputc);

    /* Enable the SPI Master device... */
    sys_enable(sys_device_spi_master);

    gpio_function(GPIO_SPIM_CLK, pad_spim_sck);
    gpio_function(SPI_SS_GPIO, SPI_SS_FUNC);
    gpio_dir(SPI_SS_GPIO, pad_dir_output);
    gpio_function(GPIO_SPIM_MOSI, pad_spim_mosi);
    gpio_function(GPIO_SPIM_MISO, pad_spim_miso);

    spi_init(SPIM, spi_dir_master, spi_mode_0, 32);

    /* Enable 16 byte FIFOs */
    spi_option(SPIM, spi_option_fifo, 1);

    ee_ewen(); /* Enable writing */

}

/** Loop */
void loop(void)
{
    tfp_printf("Displaying the current contents of EEPROM\r\n");
    ee_dump();

    tfp_printf("\r\n" "Writing every Odd location with 1\r\n");
    {
        uint8_t i;
        for (i = 0; i < EEPROM_SIZE; i+=2)
        {
            ee_write(i,1);
        }
    }
    ee_dump();

    tfp_printf("\r\n" "Filling with example text\r\n");
    {
        uint8_t i;
        for(i = 0; i < sizeof(LOREM_IPSUM); ++i)
        {
            ee_write(i, LOREM_IPSUM[i]);
        }
    }
    ee_dump();

    tfp_printf("\r\n" "Clear all values to 0xFF\r\n");
    {
        uint8_t i;
        for (i = 0; i < EEPROM_SIZE; i++)
        {
            ee_write(i,0xFF);
        }
    }
    ee_dump();

    for(;;);
}

/** EEPROM Transfer
 *  @param outp A pointer to the data being transferred out
 *  @param inp A pointer to the data being transferred in
 *  @param size The size of data to transfer */
void ee_xfer(uint8_t *outp, uint8_t *inp, uint8_t size)
{
    spi_open(SPIM, SPI_CHANNEL);
    gpio_write(SPI_SS_GPIO, 1); /* Need to use GPIO since we need a CS Active High */

    /* Exchange data between mcu and eeprom */
    spi_xchangen(SPIM,inp,outp,size);

    gpio_write(SPI_SS_GPIO, 0);
    spi_close(SPIM, SPI_CHANNEL);
}

/** EEPROM Read
 *  @param addr The address to read
 *  @return The data at that address */
uint8_t ee_read(uint8_t addr)
{
    uint8_t cmd[EEPROM_CMD_SIZE];
    uint8_t retval[EEPROM_CMD_SIZE];

    EEPROM_CMD_PACK(cmd,2,addr,0);

    ee_xfer(cmd, retval, 3);

    return EEPROM_CMD_UNPACK_DATA(retval);
}

/** EEPROM Write Enable
 *  Must be called before data can be written to the EEPROM */
void ee_ewen(void)
{
    uint8_t cmd[EEPROM_CMD_SIZE];
    uint8_t retval[EEPROM_CMD_SIZE];

    EEPROM_CMD_PACK(cmd,0,0x7F,0);

    hexdump((void*)cmd, sizeof(cmd));

    ee_xfer(cmd, retval, 2);
}

/** EEPROM Erase
 *  @param address The address to erase */
void ee_erase(uint8_t addr)
{
    uint8_t cmd[EEPROM_CMD_SIZE];
    uint8_t retval[EEPROM_CMD_SIZE];

    EEPROM_CMD_PACK(cmd,3,addr,0);

    ee_xfer(cmd, retval, 3);
}

/** EEPROM Write
 *  @param addr The address to write to
 *  @param data The data to write to that address */
void ee_write(uint8_t addr, uint8_t data)
{
    uint8_t cmd[EEPROM_CMD_SIZE];
    uint8_t retval[EEPROM_CMD_SIZE];

    EEPROM_CMD_PACK(cmd,1,addr,data);

    ee_xfer(cmd, retval, 3);

    delayms(5);
}

/** EEPROM Dump. Dump contents of EEPROM to screen */
void ee_dump(void)
{
    uint8_t buff[EEPROM_SIZE] = {0};
    uint16_t i;

    for (i = 0; i < EEPROM_SIZE; ++i)
    {
        buff[i] = ee_read(i);
    }

    hexdump(buff, EEPROM_SIZE);
}

/** Display the contents of a variable in a human readable format
 *  @param data A pointer to the variable
 *  @param len The size of the variable */
void hexdump(uint8_t *data, uint16_t len)
{
    static const uint8_t col_width = 16;
    uint16_t i,j;

    j = 0;
    do
    {
        tfp_printf("0x%04x: ", j);
        for (i = 0; i < col_width; i++)
        {
            if (i+j < len)
                tfp_printf("%02X ", data[i+j]);
            else
                tfp_printf("   ");
        }

        tfp_printf(" | ");

        for (i = 0; i < col_width; i++)
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

        j += col_width;
    }
    while(j < len);
}

void myputc(void* p, char c)
{
    uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}
