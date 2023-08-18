/**
  @file
  @brief
  I2C Master Example 1

  Read and write to an I2C EEPROM (24LC01)
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
#include <string.h> /* For memset */
#include <ft900.h>
#include "tinyprintf.h"

#define EEPROM_ADDR (0xA0)
#define EEPROM_SIZE (128/8)
#define EEPROM_PAGE_SIZE (8)

#define I2C_CHANNEL (1)

/* Function definitions */
void setup(void);
void loop(void);
int8_t ee_write(const uint8_t location, const uint8_t *data, const uint8_t len);
int8_t ee_read(const uint8_t location, const uint8_t *data, const uint8_t len);
void ee_dump(void);
void hexdump(uint8_t *data, uint16_t len);
void myputc(void* p, char c);

/** Main program */
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
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to I2C Master Example 2... \r\n"
        "\r\n"
        "Read and write to the on-board MAC address EEPROM (24AA02E48T) \r\n"
        "--------------------------------------------------------------------- \r\n"
        );
    /* Enable tfp_printf() functionality... */
    init_printf(UART0,myputc);

    /* Enable the I2C Slave device... */
    sys_enable(sys_device_i2c_master);

#if I2C_CHANNEL == 0
    /* Set GPIO44 to I2CM_SCL and GPIO45 to I2CM_SDA... */
    gpio_function(44, pad_i2c0_scl);
    gpio_pull(44, pad_pull_none);
    gpio_function(45, pad_i2c0_sda);
    gpio_pull(45, pad_pull_none);
#elif I2C_CHANNEL == 1
    /* Set GPIO46 to I2CM_SCL and GPIO47 to I2CM_SDA... */
    gpio_function(44, pad_i2c0_scl);
    gpio_pull(46, pad_pull_none);
    gpio_function(45, pad_i2c0_sda);
    gpio_pull(47, pad_pull_none);

    /* Set the I2C Master pins to channel 1 */
    sys_i2c_swop(1);

#else
#   error Please set I2C_CHANNEL to either 0 or 1
#endif /* I2C_CHANNEL */

    /* Set up I2C */
    i2cm_init(I2CM_NORMAL_SPEED, 100000);
}

/** Program loop */
void loop(void)
{
    uint8_t eeprom_buffer[EEPROM_SIZE] = {0};

    /* Read all of EEPROM and dump it out to the user... */
    tfp_printf("\r\n" "Reading all %d bytes of EEPROM\r\n",EEPROM_SIZE);

    ee_dump();

    /* Set all of EEPROM... */
    tfp_printf("\r\n" "Setting the EEPROM to 0xBB\r\n");

    memset(eeprom_buffer, 0xBB, EEPROM_SIZE);
    ee_write(0, eeprom_buffer, EEPROM_SIZE);

    ee_dump();

    for(;;);
}

/** EEPROM Write routine
 *  @param location The location to start writing
 *  @param data A pointer to the data to write
 *  @param len The size of data to write */
int8_t ee_write(const uint8_t location, const uint8_t *data, const uint8_t len)
{
    uint8_t i;
    int8_t retval = 0;

    /* Transfer a block of data by carrying out a series of byte writes
       and waiting for the write to complete */
    for (i = 0; (i < len) && (retval == 0); ++i)
    {
        retval = i2cm_write(EEPROM_ADDR, location+i, data+i, 1);

        delayms(5); // Wait a write cycle time
    }

    return retval;
}

/** EEPROM Read routine
 *  @param location The location to start reading
 *  @param data A pointer to the data to read into
 *  @param len The size of data to read */
int8_t ee_read(const uint8_t location, const uint8_t *data, const uint8_t len)
{
    return i2cm_read(EEPROM_ADDR, location, (uint8_t *) data, len);

}

/** EEPROM Dump */
void ee_dump(void)
{
    uint8_t eeprom_buffer[EEPROM_SIZE] = {0};

    ee_read(0, eeprom_buffer, EEPROM_SIZE);
    hexdump(eeprom_buffer, EEPROM_SIZE);
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

/** Printf putc */
void myputc(void* p, char c)
{
    uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}
