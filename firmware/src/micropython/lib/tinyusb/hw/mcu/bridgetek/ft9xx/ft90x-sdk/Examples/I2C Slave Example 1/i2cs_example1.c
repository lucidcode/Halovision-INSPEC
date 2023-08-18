/**
  @file
  @brief
  I2C Slave Example 1

  Have a block of memory act as registers on an I2C bus.
  Read Address = 0x39, Write Address = 0x38

  With a Bus Pirate, try the following...
  1) Connect up the bus pirate as shown:
     Bus Pirate |   FT900
     -----------+-----------
     GND        | GND
     3V3        | -
     5V         | -
     ADC        | -
     VExtern    | -
     Aux1       | -
     CLK        | GPIO46/I2CS_SCL
     MOSI       | GPIO47/I2CS_SDA
     CS         | -
     MISO       | -

  2) To set up the Bus Pirate to work in I2C mode:
     HiZ>#                              Reset the Device
     HiZ>m4                             Set to I2C Mode
     (1)>3                              ~100 kHz

  3) To write data to the I2C Slave device:
     I2C>[ 0x38 <addr> <data> ]      Write a byte of <data>
                                        to <addr>

  4) To read data from the I2C Slave device:
     I2C>[ 0x38 <addr> [ 0x39 r ]       Read one byte from <addr>



*/
/*
 * ============================================================================
 * History
 * =======
 * 3 Oct 2014 : Created
 * 4 Jun 2015 : Edited
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

void setup(void);
void loop(void);
int8_t i2cs_dev_init(uint8_t addr, uint8_t *buffer, size_t size);
int8_t i2cs_dev_add_hooks(void (*readhook)(uint8_t), void (*writehook)(uint8_t));
void i2cs_dev_ISR(void);

/* i2cs_dev variables */
uint8_t *i2cs_dev_buffer;
size_t i2cs_dev_buffer_size;
uint8_t i2cs_dev_buffer_ptr;
void (*i2cs_dev_readhook)(uint8_t) = NULL;
void (*i2cs_dev_writehook)(uint8_t) = NULL;

uint8_t registers[10] =
{
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10
};

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
#if defined(__FT900__)
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#elif defined(__FT930__)
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#endif
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
        "Welcome to I2C Slave Example 1... \r\n"
        "\r\n"
        "Have a block of memory act as registers on an I2C bus. \r\n"
        "Read Address = 0x39, Write Address = 0x38 \r\n"
        "--------------------------------------------------------------------- \r\n"
        "\r\n"
        "With a Bus Pirate, try the following...\r\n"
        "1) Connect up the bus pirate as shown:\r\n"
        "   Bus Pirate |   FT900\r\n"
        "   -----------+-----------\r\n"
        "   GND        | GND\r\n"
        "   3V3        | -  \r\n"
        "   5V         | -  \r\n"
        "   ADC        | -  \r\n"
        "   VExtern    | -  \r\n"
        "   Aux1       | -  \r\n"
        "   CLK        | GPIO46/I2CS_SCL\r\n"
        "   MOSI       | GPIO47/I2CS_SDA\r\n"
        "   CS         | - \r\n"
        "   MISO       | - \r\n"
        "\r\n"
        "2) To set up the Bus Pirate to work in I2C mode:\r\n"
        "   HiZ>#                              Reset the Device\r\n"
        "   HiZ>m4                             Set to I2C Mode\r\n"
        "   (1)>3                              ~100 kHz\r\n"
        "   I2C>P                              Enable Pullups\r\n"
        "\r\n"
        "3) To write data to the I2C Slave device:\r\n"
        "   I2C>[ 0x38 <addr> <data> ]      Write a byte of <data>\r\n"
        "                                      to <addr>\r\n"
        "\r\n"
        "4) To read data from the I2C Slave device:\r\n"
        "   I2C>[ 0x38 <addr> [ 0x39 r ]       Read one byte from <addr>\r\n"
        "\r\n"
        );

    /* Enable the I2C Slave device... */
    sys_enable(sys_device_i2c_slave);
#if defined(__FT900__)
    /* Set GPIO46 to I2CS_SCL and GPIO47 to I2CS_SDA... */
    gpio_function(46, pad_i2c1_scl);
    gpio_pull(46, pad_pull_none);
    gpio_function(47, pad_i2c1_sda);
    gpio_pull(47, pad_pull_none);

#elif defined(__FT930__)

    /* Set GPIO12 to I2CS_SCL and GPIO13 to I2CS_SDA... */
    gpio_function(12, pad_i2cs_scl);
    gpio_pull(12, pad_pull_none);
    gpio_function(13, pad_i2cs_sda);
    gpio_pull(13, pad_pull_none);

#endif
    /* Initialise the I2C slave device to base address 0x38 and give it
       ten registers */
    i2cs_dev_init(0x38, registers, 10);

    /* Enable all interrupts - needed for I2S Slave Device... */
	interrupt_enable_globally();

	uart_puts(UART0, "Interrupts enabled \r\n");
}

void loop(void)
{
    /* Do Nothing: interrupt controlled */
}

/**
 Initialise the I2S Slave to act as a device
*/
int8_t i2cs_dev_init(uint8_t addr, uint8_t *buffer, size_t size)
{
    /* Set up the local buffer... */
    i2cs_dev_buffer = buffer;
    i2cs_dev_buffer_size = size;

    /* Initialise the I2C Slave hardware... */
    i2cs_init(addr);
    /* Set up the handler for i2cs_dev... */
    interrupt_attach(interrupt_i2cs, (uint8_t)interrupt_i2cs, i2cs_dev_ISR);
    i2cs_enable_interrupt(MASK_I2CS_FIFO_INT_ENABLE_I2C_INT);

    return 0;
}

/**
 Add hooks to a read or write operation
 */
int8_t i2cs_dev_add_hooks(void (*readhook)(uint8_t), void (*writehook)(uint8_t))
{
    i2cs_dev_readhook = readhook;
    i2cs_dev_writehook = writehook;
    return 0;
}

/**
 The interrupt to handle i2cs_dev
 */
void i2cs_dev_ISR(void)
{
    static uint8_t          rx_addr     = 1;
    uint8_t                 status;

    uart_puts(UART0, "Interrupt\r\n");

    if (i2cs_is_interrupted(MASK_I2CS_FIFO_INT_PEND_I2C_INT))
    {
        status = i2cs_get_status();

        /* For a write transaction... */
        if(status & MASK_I2CS_STATUS_RX_REQ)
        {
            /* If we are wanting for the Write address to appear... */
            if (rx_addr)
            {
            	uart_puts(UART0, "Write address interrupt\r\n");

                /* Read in the initial offset to read/write... */
                rx_addr = 0;
                i2cs_read(&i2cs_dev_buffer_ptr, 1);
            }
            else
            {
            	uart_puts(UART0, "Write data interrupt\r\n");
                /* Write the byte to the register buffer... */
                i2cs_read(&(i2cs_dev_buffer[i2cs_dev_buffer_ptr]), 1);
                if (i2cs_dev_writehook) i2cs_dev_writehook(i2cs_dev_buffer_ptr);
                i2cs_dev_buffer_ptr++;

            }

        }
        /* For a read transaction... */
        else if(status & MASK_I2CS_STATUS_TX_REQ)
        {
        	uart_puts(UART0, "Read interrupt\r\n");

            /* Write the byte to the I2C bus... */
            i2cs_write(&(i2cs_dev_buffer[i2cs_dev_buffer_ptr]), 1);
            if (i2cs_dev_readhook) i2cs_dev_readhook(i2cs_dev_buffer_ptr);
            i2cs_dev_buffer_ptr++;

        }

        /* For the completion of a transaction... */
        else if (status & (MASK_I2CS_STATUS_REC_FIN | MASK_I2CS_STATUS_SEND_FIN))
        {
        	uart_puts(UART0, "Finished interrupt\r\n");

        	/* Finished transaction, reset... */
            rx_addr = 1;
        }

        /* Wrap around */
        if (i2cs_dev_buffer_ptr > i2cs_dev_buffer_size)
            i2cs_dev_buffer_ptr = 0;
    }
}
