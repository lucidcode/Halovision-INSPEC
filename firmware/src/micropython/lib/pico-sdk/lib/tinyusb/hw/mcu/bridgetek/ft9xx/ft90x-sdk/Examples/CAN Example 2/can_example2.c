/**
  @file
  @brief
  CAN Example 2

  Filter CAN messages arriving at CAN1.
*/
/*
 * ============================================================================
 * History
 * =======
 * 2 Oct 2014 : Created
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

void setup(void);
void loop(void);
void print_can_msg_t(can_msg_t *msg);
void myputc(void* p, char c);

/* Create a filter to accept any standard CAN message with the ID 0x123 */
static const can_filter_t filter = {
    can_type_standard,  /* type        The type of message this filter will match for */
    0x00000123,         /* id          The ID Stub to match for */
    0x00000000,         /* idmask      The ID Mask */
    can_rtr_none,       /* rtr         The RTR Stub to match for */
    1,                  /* rtrmask     The RTR Mask */
    0,                  /* data0       The stub of the first data byte to match for */
    0xFF,               /* data0mask   The mask of the first data byte */
    0,                  /* data1       The stub of the second data byte to match for */
    0xFF,               /* data1mask   The mask of the second data byte to match for */
};
static can_msg_t wanted_messages[1] = {
    /* id       = */ 0x123,
    /* type     = */ can_type_standard,
    /* rtr      = */ can_rtr_none,
    /* dlc      = */ 8,
    /* data[8]  = */ (uint8_t [8]) {'H','E','L','O','W','R','L','D'}
};
static can_msg_t unwanted_messages[5] = {
    {
        /* id       = */ 0x001,
        /* type     = */ can_type_standard,
        /* rtr      = */ can_rtr_none,
        /* dlc      = */ 2,
        /* data[8]  = */ {3,65,0,0,0,0,0,0}
    },
    {
        /* id       = */ 0x111,
        /* type     = */ can_type_standard,
        /* rtr      = */ can_rtr_none,
        /* dlc      = */ 4,
        /* data[8]  = */ {23,64,76,145,0,0,0,0}
    },
    {
        /* id       = */ 0x234,
        /* type     = */ can_type_standard,
        /* rtr      = */ can_rtr_none,
        /* dlc      = */ 8,
        /* data[8]  = */ {0,45,23,35,77,32,67,45}
    },
    {
        /* id       = */ 0x222,
        /* type     = */ can_type_standard,
        /* rtr      = */ can_rtr_none,
        /* dlc      = */ 0,
        /* data[8]  = */ {0,0,0,0,0,0,0,0}
    },
    {
        /* id       = */ 0x1FF,
        /* type     = */ can_type_standard,
        /* rtr      = */ can_rtr_none,
        /* dlc      = */ 2,
        /* data[8]  = */ {111,111,0,0,0,0,0,0}
    }
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
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to CAN Example 2... \r\n"
        "\r\n"
        "Filter CAN messages arriving at CAN1.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(NULL,myputc);

    /* Enable CAN0 and CAN1... */
    sys_enable(sys_device_can0);
    sys_enable(sys_device_can1);

    /* Set up the pins to use CAN... */
    gpio_function(15, pad_can0_txd); /* GPIO15 is CAN0_TXD */
    gpio_function(16, pad_can0_rxd); /* GPIO16 is CAN0_RXD */
    gpio_function(17, pad_can1_txd); /* GPIO17 is CAN1_TXD */
    gpio_function(18, pad_can1_rxd); /* GPIO18 is CAN1_RXD */

    /* Initialise CAN at 250 kbaud... */
    can_init(CAN0, can_mode_normal, &g_can250kbaud);
    can_init(CAN1, can_mode_normal, &g_can250kbaud);

    /* Apply the filter to CAN1... */
    /* This must be done AFTER the can_init call and BEFORE the can_open call */
    can_filter(
        CAN1,                   /* Channel */
        can_filter_mode_single, /* Filter mode */
        0,                      /* Filter number */
        &filter);               /* Filter */

    /* Start CAN... */
    can_open(CAN0);
    can_open(CAN1);
}

void loop(void)
{
    uint8_t i = 0, j = 0;
    uint16_t k = 0;
    uint8_t available = 0;
    static can_msg_t rx;

    /* Transmit every unwanted message 10 times... */
    k = 1;
    for (i = 0; i < (sizeof(unwanted_messages)/sizeof(can_msg_t)); ++i)
    {
        for (j = 0; j < 10; ++j)
        {
            /* Transmit an unwanted message on CAN0 to CAN1... */
            can_write(CAN0, unwanted_messages+i);
            tfp_printf("Transmitting %5d unwanted messages\r", k++);

            /* Sanity check to see if we go anything on CAN1 */
            if (can_available(CAN1))
            {
                tfp_printf("\r\n"
                           "ERROR: Got a CAN message on CAN1 when I wasn't expecting it\r\n"
                           "HALTING.\r\n");
                for(;;); /* HALT */
            }
        }
    }
    tfp_printf("\r\n");

    /* Transmit the wanted message to CAN1... */
    can_write(CAN0, wanted_messages);
    tfp_printf("Transmitting     1 wanted messages\r\n");

    /* Check to see if only one message has appeared at CAN1... */
    available = can_available(CAN1);
    if (available == 1)
        tfp_printf("There is 1 message available on CAN1\r\n");
    else
        tfp_printf("There are %d messages available on CAN1\r\n", available);


    /* Read out every message we got (which should be just one)... */
    while (available--)
    {
        if (can_read(CAN1, &rx) == 0)
        {
            tfp_printf("CAN1 RX-> ");
            print_can_msg_t(&rx);
        }
    }

    tfp_printf("\r\n");

    /* Wait a bit... */
    delayms(2000);
}

/**
 Output a CAN message as a human readable format

 e.g.
 A standard CAN message with ID 123(hex), the RTR bit not set, a DLC of 3, and
 data bytes 1, 2 and 3 will come out as...

 ID=_____0x123     {0x01,0x02,0x03}

 An extended CAN message with ID 123(hex), the RTR bit set, a DLC of 8, and
 data bytes 1, 2, 3, 4, 5, 6, 7, and 8 will come out as...

 ID=0x00000123 RTR {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08}
*/
void print_can_msg_t(can_msg_t *msg)
{
    uint8_t i = 0;

    if (msg->type == can_type_standard)
        tfp_printf("ID=_____0x%03lx ", msg->id); /* 11 bit ID */
    else
        tfp_printf("ID=0x%08lx ", msg->id); /* 29 bit ID */

    if (msg->rtr == can_rtr_remote_request)
        tfp_printf("RTR ");
    else
        tfp_printf("    ");

    tfp_printf("{");
    for (i = 0; i < msg->dlc; ++i)
    {
        tfp_printf("0x%02x", msg->data[i]);
        if (i < (msg->dlc - 1))
            tfp_printf(",");
    }
    tfp_printf("}"
           "\r\n");
}

void myputc(void* p, char c)
{
    uart_write(UART0, (uint8_t)c);
}
