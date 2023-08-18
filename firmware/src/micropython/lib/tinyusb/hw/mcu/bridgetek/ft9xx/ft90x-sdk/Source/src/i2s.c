/**
    @file

    @brief
    I2S

    
**/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek") subject to the licence terms set out
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

/* INCLUDES ************************************************************************/
#include <ft900_i2s.h>
#include <ft900_asm.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/
i2s_mode_t i2s_mode;      /**< Variable to indicate if the device is I2S master or slave. */

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

void i2s_init(i2s_mode_t mode, i2s_length_t length, i2s_format_t format,
              i2s_padding_t padding, i2s_master_input_clk_t mclk_in,
              i2s_bclk_div bclk_div, i2s_mclk_div_t mclk_div,
              i2s_bclk_per_channel_t bclk_per_channel)
{
	uint16_t tmp = 0x0000;
	
	if (mode == i2s_mode_master)
		i2s_mode = i2s_mode_master;
	else
		i2s_mode = i2s_mode_slave;

	tmp = (uint16_t) (length)	|
		  (uint16_t) (format)	|
		  (uint16_t) (padding);
          
	/* Input CLK */
	if (mclk_in == i2s_master_input_clk_22mhz)
	{
		tmp |= MASK_I2S_CR_IS_MASTER_22;
	}
	else if (mclk_in == i2s_master_input_clk_24mhz)
	{
		tmp &= ~MASK_I2S_CR_IS_MASTER_22;
	}
    
	/* Number of BLCK cycles per channel */
	if (bclk_per_channel == i2s_bclk_per_channel_16)
	{
		tmp &= ~MASK_I2S_CR_IS_MASTER_64;
	}
	else if (bclk_per_channel == i2s_bclk_per_channel_32)
	{
		tmp |= MASK_I2S_CR_IS_MASTER_64;
	}
	I2S->I2S_CR = tmp;
	
	I2S->I2S_CR2 = (uint16_t) (bclk_div) | (uint16_t) (mclk_div);

    /* Clear interrupt pending register right after filling the TX fifo with 
       enough data and just before enabling interrupt!!!!
       If not done properly, interrupt will come not at the right time. Only 
       TX FIFO EMPTY can be cleared now, not RX FIFO EMPTY. */
    I2S->I2S_IRQ_PEND = 0xFF;
}

#define MASK_I2S_CR_BIT_LENGTH		(MASK_I2S_CR_BIT_LENGTH_1 | MASK_I2S_CR_BIT_LENGTH_0)
#define MASK_I2S_CR_FORMAT			(MASK_I2S_CR_FORMAT_1 | MASK_I2S_CR_FORMAT_0)
#define MASK_I2S_CR_PADDING			(MASK_I2S_CR_PADDING_2 | MASK_I2S_CR_PADDING_1 | MASK_I2S_CR_PADDING_0)

size_t i2s_write(const uint8_t *data, const size_t num_bytes)
{
    register uint8_t *r_data = (uint8_t *)data;
    register uint32_t r_n_bytes = num_bytes;
    register volatile uint16_t* r_fifo = &(I2S->I2S_RW_DATA);
    size_t _size = 0;

    /* Start filling up TX FIFO. TX FIFO can hold up to a max of 2048 bytes 
      (1024 16-bit words). */
    if ((I2S->I2S_CR & MASK_I2S_CR_BIT_LENGTH) == i2s_length_24)
	{
        register uint32_t temp1 = 0;
        register uint32_t temp2 = 0;

        r_n_bytes -= r_n_bytes % 6; /* make sure we are on a 6 byte boundary */

        while (r_n_bytes)
        {
            /* Stream in 3 byte samples into the FIFO */
        	__asm__ volatile (
				// WORD 0
				// temp1 = *(uint16_t)(r_data[0]);
				"ldi.s  %2, %1, 0 \n\t" // ldi.s temp1, r_data, 0
				// *r_fifo = temp1;
				"sti.s  %0, 0, %2 \n\t" // sti.s r_fifo, 0, temp1

				// WORD 1
				// temp1 = *(r_data[2]);
				"ldi.s  %2, %1, 2 \n\t" // ldi.b temp1, r_data, 2
				// *r_fifo = temp1;
				"sti.s  %0, 0, %2 \n\t" // sti.s r_fifo, 0, temp1

				// WORD 2
				// temp1 = temp1 >> 8;
				"ashr.s %2, %2, 8 \n\t" // ashl.s temp1, temp1, 8
				// temp2 = *(&r_data[4]);
				"ldi.s  %3, %1, 4 \n\t" // ldi.b temp2, r_data, 4
				// temp2 = temp2 << 8;
				"ashl.s %3, %3, 8 \n\t" // ashl.s temp2, temp2, 8
				// temp1 = temp1 | temp2;
				"or.l   %2, %2, %3 \n\t" // or.s temp1, temp1, temp2
				// *r_fifo = temp1;
				"sti.s  %0, 0, %2 \n\t" // sti.s r_fifo, 0, temp1

				// WORD 3
				// temp1 = temp1 >> 16;
				"ashr.l %2, %2, 16 \n\t" // ashl.s temp1, temp1, 8
				// *r_fifo = temp1;
				"sti.s  %0, 0, %2 \n\t" // sti.s r_fifo, 0, temp1

				"add.l %1, %1, 6 \n\t" // add.l r_data, r_data, 6
				"sub.l %4, %4, 6 \n\t" // sub.l r_n_bytes, r_n_bytes, 6

				: /* Output */
				: /* Input */ "r"(r_fifo), "r"(r_data), "r"(temp1), "r"(temp2), "r"(r_n_bytes)
			);

            _size += 8; /* Save the size value to return */
        }
    }
    else /* 16, 20 and 32 bit streaming */
    {
        //r_n_bytes -= r_n_bytes % 2; /* Make sure we are a multiple of 2 */
        _size = r_n_bytes; /* Save the size value to return */

        asm_streamout16(r_data, r_fifo, r_n_bytes);
    }


    return _size;
}

size_t i2s_read(uint8_t *data, const size_t num_bytes)
{
    register uint8_t *r_data = (uint8_t *)data;
    register uint32_t r_n_bytes = num_bytes & (~1UL);
    register volatile uint16_t* r_fifo = &(I2S->I2S_RW_DATA);
    size_t _size = r_n_bytes; /* Save the size value to return */

    asm_streamin16(r_fifo, r_data, r_n_bytes);

/*	__asm__
//	(
//			"streamin.s		%0, %1, %2	\n\t"
//			: 																// output registers
//			: "r" (data), "r" (&(I2S->r_w_data)), "r" (num_bytes)	// input registers
//	);
*/

	return _size;
}

void i2s_enable_int(uint16_t mask)
{
	I2S->I2S_IRQ_EN |= mask;
}

void i2s_disable_int(uint16_t mask)
{
	I2S->I2S_IRQ_EN &= ~mask;
}

void i2s_clear_int_flag(uint16_t mask)
{
	I2S->I2S_IRQ_PEND |= mask;
}

uint16_t i2s_get_status(void)
{
    return I2S->I2S_IRQ_PEND;
}

int8_t i2s_is_interrupted(uint16_t mask)
{
    int8_t iRet = 0;
    uint16_t interrupts = I2S->I2S_IRQ_PEND & mask;

    if(interrupts)
    {
        I2S->I2S_IRQ_PEND = interrupts;
        iRet = 1;
    }

    return iRet;
}

uint16_t i2s_get_rx_count(void)
{
	return (I2S->I2S_RX_COUNT * 2); // return interms of bytes in the RX FIFO
}

uint16_t i2s_get_tx_count(void)
{
	return (I2S->I2S_TX_COUNT * 2); // return interms of bytes in the TX FIFO
}

void i2s_start_tx(void)
{
	/* If master mode is selected, switch on Master bit in I2SCR to generate clock. 
	   Enable transmit after the TX fifo is filled. 
	   Important: Both must be done at the same time. */
	I2S->I2S_CR |= (uint16_t)(i2s_mode) | (uint16_t)MASK_I2S_CR_TX_ENABLE;
}

void i2s_stop_tx(void)
{
	/* If master mode is selected, turn off clock by switching mode to slave, 
       then disable transmit */
	I2S->I2S_CR &= (uint16_t)(~i2s_mode) & (uint16_t)(~MASK_I2S_CR_TX_ENABLE);
}

void i2s_start_rx(void)
{
	/* If master mode is selected, switch on Master bit in I2SCR to generate clock. 
	   Enable receive. 
	   Important: Both must be done at the same time. */
	I2S->I2S_CR |= (uint16_t)(i2s_mode) | (uint16_t)MASK_I2S_CR_RX_ENABLE;
}

void i2s_stop_rx(void)
{
	/* If master mode is selected, turn off clock by switching mode to slave, 
       then disable receive */
	I2S->I2S_CR &= (uint16_t)(~i2s_mode) & (uint16_t)(~MASK_I2S_CR_RX_ENABLE);
}
