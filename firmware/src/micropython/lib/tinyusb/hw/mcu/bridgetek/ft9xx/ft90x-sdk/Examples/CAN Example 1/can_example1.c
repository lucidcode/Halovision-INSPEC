/**
  @file
  @brief
  CAN Example 1

  Send and Receive messages between CAN0 and CAN1 and poll the response.
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
void can_perror(uint8_t errno);
void myputc(void* p, char c);

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
        "Welcome to CAN Example 1... \r\n"
        "\r\n"
        "Send and Receive messages between CAN0 and CAN1 and poll the response.\r\n"
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

    /* Start CAN... */
    can_open(CAN0);
    can_open(CAN1);
}

void loop(void)
{
    uint8_t i = 0;
    uint8_t ok = 1;
    static can_msg_t tx = {
        /* id       = */ 0x123,
        /* type     = */ can_type_standard,
        /* rtr      = */ can_rtr_none,
        /* dlc      = */ 8,
        /* data[8]  = */ {0,0,0,0,0,0,0,0}
    };
    static can_msg_t rx;

    /* Output the transmitted message... */
    printf("CAN0 TX-> ");
    print_can_msg_t(&tx);

    /* Transmit the message on CAN0 to CAN1... */
    can_write(CAN0, &tx);

    /* Wait a bit... */
    delayms(100);

    /* If everything is ok and a message is available on CAN 1... */
    if (ok && can_read(CAN1, &rx) != -1)
    {
        /* Output the received message... */
        printf("CAN1 RX<- ");
        print_can_msg_t(&rx);

        /* Change the data in the received message... */
        for (i = 0; i < rx.dlc; ++i)
        {
            rx.data[i]++;
        }

        /* Output the transmitted message... */
        printf("CAN1 TX-> ");
        print_can_msg_t(&rx);

        /* Transmit the new message on CAN1 to CAN0... */
        can_write(CAN1, &rx);

        /* Wait a bit... */
        delayms(100);
    }
    else
    {
        /* Something bad happened, stop... */
        ok = 0;
    }

    /* If everything is ok and a message is available on CAN 1... */
    if (ok && can_read(CAN0, &rx) != -1)
    {
        /* Output the received message... */
        printf("CAN0 RX<- ");
        print_can_msg_t(&rx);

        /* Store the received data so that it can be transmitted again
           on the next pass... */
        tx.data[0] = rx.data[0];
        tx.data[1] = rx.data[1];
        tx.data[2] = rx.data[2];
        tx.data[3] = rx.data[3];
        tx.data[4] = rx.data[4];
        tx.data[5] = rx.data[5];
        tx.data[6] = rx.data[6];
        tx.data[7] = rx.data[7];
    }
    else
    {
        /* Something bad happened, stop... */
        ok = 0;
    }

    /* Something bad happened, stop... */
    if (!ok)
    {
        can_perror(can_ecode(CAN0));
        can_perror(can_ecode(CAN1));
        tfp_printf("HALTING.\r\n");
        for(;;);
    }

    /* Insert an empty line so the output can be read easier... */
    printf("\r\n");

    /* Wait a bit... */
    delayms(500);
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
        printf("ID=_____0x%03lx ", msg->id); /* 11 bit ID */
    else
        printf("ID=0x%08lx ", msg->id); /* 29 bit ID */

    if (msg->rtr == can_rtr_remote_request)
        printf("RTR ");
    else
        printf("    ");

    printf("{");
    for (i = 0; i < msg->dlc; ++i)
    {
        printf("0x%02x", msg->data[i]);
        if (i < (msg->dlc - 1))
            printf(",");
    }
    printf("}"
           "\r\n");
}

/**
 Output a string of errors from an error code
 */
void can_perror(uint8_t errno)
{
    static const char* error_codes[] = {
        "RX_WRN: Receive Warning. The number of receive errors is >= 96",
        "TX_WRN: Transmit Warning. The number of transmit errors is >= 96",
        "ACK_ERR: Acknowledge Error Occurred",
        "FRM_ERR: Form Error Occurred",
        "CRC_ERR: CRC Error Occurred",
        "STF_ERR: Stuff Error Occurred",
        "BIT_ERR: Bit Error Occurred"
    };
    static const char* direction[] = {
        "Receiving",
        "Transmitting"
    };

    if (errno & 0xDF)
    {
        tfp_printf("Error whilst ");

        if (errno & (1 << 5))
            tfp_printf("%s : \r\n",direction[0]);
        else
            tfp_printf("%s : \r\n",direction[1]);

        if (errno & (1 << 7)) tfp_printf("\t%s \r\n", error_codes[0]);
        if (errno & (1 << 6)) tfp_printf("\t%s \r\n", error_codes[1]);
        if (errno & (1 << 4)) tfp_printf("\t%s \r\n", error_codes[2]);
        if (errno & (1 << 3)) tfp_printf("\t%s \r\n", error_codes[3]);
        if (errno & (1 << 2)) tfp_printf("\t%s \r\n", error_codes[4]);
        if (errno & (1 << 1)) tfp_printf("\t%s \r\n", error_codes[5]);
        if (errno & (1 << 0)) tfp_printf("\t%s \r\n", error_codes[6]);
    }
}

void myputc(void* p, char c)
{
    uart_write(UART0, (uint8_t)c);
}
