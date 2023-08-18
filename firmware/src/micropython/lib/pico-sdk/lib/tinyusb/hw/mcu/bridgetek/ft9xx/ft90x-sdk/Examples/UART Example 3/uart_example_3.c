/**
  @file
  @brief
  UART Example 3

  This example program will implement transmit and receive functions that will
  use make the UART completely interrupt driven, storing the incoming and
  outgoing bytes in a section of global memory.
*/
/*
 * ============================================================================
 * History
 * =======
 * 30 Oct 2014 : Created
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

/* Change this value in order to change the size of the buffer being used */
#define RINGBUFFER_SIZE (16)

typedef struct
{
    uint8_t     data[RINGBUFFER_SIZE];
    uint16_t    wr_idx;
    uint16_t    rd_idx;
} RingBuffer_t;

void setup(void);
void loop(void);
uint16_t uart0Tx(uint8_t *data, uint16_t len);
uint16_t uart0Rx(uint8_t *data, uint16_t len);
uint16_t uart0Available(void);
uint8_t uart0Peek(void);
void uart0ISR(void);

/* Transmit buffer */
static RingBuffer_t uart0BufferIn = { {0}, 0, 0 };
/* Receive buffer */
static RingBuffer_t uart0BufferOut = { {0}, 0, 0 };

int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void setup()
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
        "Welcome to UART Example 3... \r\n"
        "\r\n"
        "Any character typed here will be echoed back on the same serial port  \r\n"
        "via an interrupt and polled every second.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Attach the interrupt so it can be called... */
    interrupt_attach(interrupt_uart0, (uint8_t) interrupt_uart0, uart0ISR);
    /* Enable the UART to fire interrupts when receiving data... */
    uart_enable_interrupt(UART0, uart_interrupt_rx);
    /* Enable the UART to fire interrupts when transmitting data... */
    uart_enable_interrupt(UART0, uart_interrupt_tx);
    /* Enable interrupts to be fired... */
    uart_enable_interrupts_globally(UART0);
    interrupt_enable_globally();
}

void loop()
{
    static uint8_t buffer[RINGBUFFER_SIZE] = {0};
    uint8_t read_bytes, sent_bytes, bytes_left;

    /* Read in as much data available... */
    read_bytes = uart0Rx(buffer, RINGBUFFER_SIZE);
    if (read_bytes)
    {
        /* Send those bytes back... */
        bytes_left = read_bytes;
        sent_bytes = 0;

        while (bytes_left)
        {
            /* Guarentee that the bytes are sent even if a single call
               does not transmit all the data... */
            sent_bytes += uart0Tx(buffer+sent_bytes, bytes_left);
            bytes_left = read_bytes - sent_bytes;
        }
    }

    /* Wait some time so we can queue up some data... */
    delayms(1000);
}

/**
 Transmit a buffer of data over UART0 asynchronously.

 @return The number of bytes written to the buffer
 */
uint16_t uart0Tx(uint8_t *data, uint16_t len)
{
    int16_t used = 0;
    uint16_t free = 0;
    uint16_t copied = 0;

    /* Determine how much space we have ... */
    used = uart0BufferOut.rd_idx - uart0BufferOut.wr_idx;
    if (used < 0)
    {
        used += RINGBUFFER_SIZE;
    }

    free = RINGBUFFER_SIZE - used;

    /* Copy in as much data as we can... */
    while(len-- && free--)
    {
        uart0BufferOut.data[uart0BufferOut.wr_idx] = *data++;

        /* Increment the pointer and wrap around */
        uart0BufferOut.wr_idx++;
        if (uart0BufferOut.wr_idx == RINGBUFFER_SIZE) uart0BufferOut.wr_idx = 0;

        copied++;
    }

    /* Start a transmission if nothing is being transmitted... */
    if (used == 0)
    {
        uint8_t c = uart0BufferOut.data[uart0BufferOut.rd_idx];

        /* Increment the pointer and wrap around */
        uart0BufferOut.rd_idx++;
        if (uart0BufferOut.rd_idx == RINGBUFFER_SIZE) uart0BufferOut.rd_idx = 0;

        uart_write(UART0, c);
    }

    return copied;
}

/**
 Receive a number of bytes from UART0

 @return The number of bytes read from UART0
 */
uint16_t uart0Rx(uint8_t *data, uint16_t len)
{
    uint16_t avail = 0;
    uint16_t copied = 0;

    avail = uart0Available();

    /* Copy in as much data as we can ...
       This can be either the maximum size of the buffer being given
       or the maximum number of bytes available in the Serial Port
       buffer */
    while(len-- && avail--)
    {
        *data = uart0BufferIn.data[uart0BufferIn.rd_idx];
        data++;

        /* Increment the pointer and wrap around */
        uart0BufferIn.rd_idx++;
        if (uart0BufferIn.rd_idx == RINGBUFFER_SIZE) uart0BufferIn.rd_idx = 0;

        copied++;
    }

    /* Report back how many bytes have been copied into the buffer...*/
    return copied;
}

/**
 See how much data is available in the UART0 ring buffer

 @return The number of bytes available
 */
uint16_t uart0Available(void)
{
    int16_t diff = uart0BufferIn.wr_idx - uart0BufferIn.rd_idx;

    if (diff < 0)
    {
        diff += RINGBUFFER_SIZE;
    }

    return (uint16_t)diff;
}

/**
 See what byte is at the head of the ring buffer

 @returns The first available byte or 0
 */
uint8_t uart0Peek(void)
{
    uint8_t b = 0;

    if (uart0BufferIn.wr_idx != uart0BufferIn.rd_idx)
    {
        b = uart0BufferIn.data[uart0BufferIn.rd_idx];
    }

    return b;
}

/**
 The Interrupt which handles asynchronous transmission and reception
 of data into the ring buffer
 */
void uart0ISR()
{
    static uint8_t c;
    static uint16_t avail;

    /* Transmit interrupt... */
    if (uart_is_interrupted(UART0, uart_interrupt_tx))
    {
        /* Check to see how much data we have to transmit... */
        avail = uart0BufferOut.rd_idx - uart0BufferOut.wr_idx;

        if (avail)
        {
            /* Copy out the byte to be transmitted so that the uart_write is
               the last thing we do... */
            c = uart0BufferOut.data[uart0BufferOut.rd_idx];

            /* Increment the pointer and wrap around */
            uart0BufferOut.rd_idx++;
            if (uart0BufferOut.rd_idx == RINGBUFFER_SIZE) uart0BufferOut.rd_idx = 0;

            /* Write out a new byte, the following Transmit interrupt should handle
               the remaning bytes... */
            uart_write(UART0, c);
        }
    }

    /* Receive interrupt... */
    if (uart_is_interrupted(UART0, uart_interrupt_rx))
    {
        /* Read a byte into the Ring Buffer... */
        uart_read(UART0, &c);

        uart0BufferIn.data[uart0BufferIn.wr_idx] = c;

        /* Increment the pointer and wrap around */
        uart0BufferIn.wr_idx++;
        if (uart0BufferIn.wr_idx == RINGBUFFER_SIZE) uart0BufferIn.wr_idx = 0;

        /* Check to see if we have hit the back of the buffer... */
        if (uart0BufferIn.wr_idx == uart0BufferIn.rd_idx)
        {
            /* Increment the pointer and wrap around */
            uart0BufferIn.rd_idx++;
            if (uart0BufferIn.rd_idx == RINGBUFFER_SIZE) uart0BufferIn.rd_idx = 0;
        }
    }
}
