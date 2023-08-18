/**
    @file ft900_i2s.h

    @brief
    I2S Audio

    
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

#ifndef FT900_I2S_H_
#define FT900_I2S_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/** @brief I2S mode definitions */
typedef enum 
{
	i2s_mode_slave  = 0x0000, /*!< I2S Slave */
	i2s_mode_master = 0x0040  /*!< I2S Master */
} i2s_mode_t;

/** @brief I2S RX definitions */
typedef enum
{
	i2s_rx_disabled = 0,    /*!< Receive Disabled */
	i2s_rx_enabled          /*!< Receive Enabled */
} i2s_rx_t;

/** @brief I2S TX definitions */
typedef enum
{
	i2s_tx_disabled = 0,    /*!< Transmit Disabled */
	i2s_tx_enabled          /*!< Transmit Enabled */
} i2s_tx_t;

/** @brief I2S data length definitions */
typedef enum
{
	i2s_length_16 = 0x0000, /*!< 16 bit data length */
	i2s_length_20 = 0x0100, /*!< 20 bit data length */
	i2s_length_24 = 0x0200, /*!< 24 bit data length */
	i2s_length_32 = 0x0300  /*!< 32 bit data length */
} i2s_length_t;

/** @brief I2S format definitions */
typedef enum
{
	i2s_format_i2s = 0x0000,        /*!< I2S format */
	i2s_format_leftjust = 0x0400,   /*!< Left justified format */
	i2s_format_rightjust = 0x0800   /*!< Right justified format */
} i2s_format_t;

/** @brief I2S padding definitions */
typedef enum
{
	i2s_padding_0  = 0x0000,    /*!< No padding */
	i2s_padding_4  = 0x1000,    /*!< 4 bits of padding */
	i2s_padding_8  = 0x2000,    /*!< 8 bits of padding */
	i2s_padding_12 = 0x3000,    /*!< 12 bits of padding */
	i2s_padding_16 = 0x4000     /*!< 16 bits of padding */
} i2s_padding_t;

/** @brief I2S master input clk frequency definitions */
typedef enum
{
	i2s_master_input_clk_22mhz, /*!< 22.5792 MHz Master clock */
	i2s_master_input_clk_24mhz  /*!< 24.576 MHz Master clock */
} i2s_master_input_clk_t;

/** @brief I2S BCLK speed definitions */
typedef enum
{
	i2s_bclk_div_1  = 0x0000,   /*!< Divide BCLK by 1 */
	i2s_bclk_div_2  = 0x0001,   /*!< Divide BCLK by 2 */
	i2s_bclk_div_3  = 0x0002,   /*!< Divide BCLK by 3 */
	i2s_bclk_div_4  = 0x0003,   /*!< Divide BCLK by 4 */
	i2s_bclk_div_6  = 0x0004,   /*!< Divide BCLK by 6 */
	i2s_bclk_div_8  = 0x0005,   /*!< Divide BCLK by 8 */
	i2s_bclk_div_12 = 0x0006,   /*!< Divide BCLK by 12 */
	i2s_bclk_div_16 = 0x0007,   /*!< Divide BCLK by 16 */
	i2s_bclk_div_24 = 0x0008,   /*!< Divide BCLK by 24 */
	i2s_bclk_div_32 = 0x0009,   /*!< Divide BCLK by 32 */
	i2s_bclk_div_48 = 0x000A,   /*!< Divide BCLK by 48 */
	i2s_bclk_div_64 = 0x000B    /*!< Divide BCLK by 64 */
} i2s_bclk_div;

/** @brief I2S MCLK speed definitions */
typedef enum
{
	i2s_mclk_div_1  = 0x0000,   /*!< Divide MCLK by 1 */
	i2s_mclk_div_2  = 0x0100,   /*!< Divide MCLK by 2 */
	i2s_mclk_div_3  = 0x0200,   /*!< Divide MCLK by 3 */
	i2s_mclk_div_4  = 0x0300,   /*!< Divide MCLK by 4 */
	i2s_mclk_div_6  = 0x0400,   /*!< Divide MCLK by 6 */
	i2s_mclk_div_8  = 0x0500,   /*!< Divide MCLK by 8 */
	i2s_mclk_div_12 = 0x0600,   /*!< Divide MCLK by 12 */
	i2s_mclk_div_16 = 0x0700,   /*!< Divide MCLK by 16 */
	i2s_mclk_div_24 = 0x0800,   /*!< Divide MCLK by 24 */
	i2s_mclk_div_32 = 0x0900,   /*!< Divide MCLK by 32 */
	i2s_mclk_div_48 = 0x0A00,   /*!< Divide MCLK by 48 */
	i2s_mclk_div_64 = 0x0B00    /*!< Divide MCLK by 64 */
} i2s_mclk_div_t;

/** @brief I2S BCLK cycles per channel, used in master mode only */
typedef enum
{
	i2s_bclk_per_channel_16,    /*!< 16 BCLK per channel */
	i2s_bclk_per_channel_32     /*!< 32 BCLK per channel */
} i2s_bclk_per_channel_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Call once, to initialise the peripheral and reset it to a known state.
 *  @param[in] mode The I2S Mode
 *  @param[in] length The transfer length
 *  @param[in] format The format of the transfer
 *  @param[in] padding The number of padding buts that have the be added to the transfer
 *  @param[in] mclk_in The MCLK to use
 *  @param[in] bclk_div The BCLK divider
 *  @param[in] mclk_div The MCLK divider
 *  @param[in] bclk_per_channel The number of BCLK per channel
 */
void i2s_init(i2s_mode_t mode, i2s_length_t length, i2s_format_t format,
              i2s_padding_t padding, i2s_master_input_clk_t mclk_in,
              i2s_bclk_div bclk_div, i2s_mclk_div_t mclk_div,
              i2s_bclk_per_channel_t bclk_per_channel);

/** @brief   Read a specified number of bytes from the I2S FIFO.
 *
 *  Reads x number of bytes from the I2S FIFO.
 *
 *  @param[out] data            Caller-allocated buffer to receive any bytes read.
 *  @param[in] num_bytes		Number of bytes to read from the FIFO.
 */
size_t i2s_read(uint8_t *data, const size_t num_bytes);

/** @brief Write a specified number of bytes to the I2S FIFO.
 *
 *  Writes x number of bytes to the I2S FIFO.
 *
 *  @warning Due to the hardware implementation of I2S, there is a performance hit when
 *           using this function with 24 bit input.
 *
 *  @param[in] data Buffer containing bytes to write.
 *  @param[in] num_bytes Number of bytes to write to the FIFO.
 */
size_t i2s_write(const uint8_t *data, const size_t num_bytes);

/** @brief Enable interrupts on the I2S module
 *
 *  | Bit     | Mask Name                     | Interrupts when...             |
 *  | ------- | ----------------------------- | -------------------------------|
 *  | 0 (LSB) | MASK_I2S_IE_FIFO_TX_UNDER     | Transmit FIFO has underflowed  |
 *  | 1       | MASK_I2S_IE_FIFO_TX_EMPTY     | Transmit FIFO is empty         |
 *  | 2       | MASK_I2S_IE_FIFO_TX_HALF_FULL | Transmit FIFO is half empty    |
 *  | 3       | MASK_I2S_IE_FIFO_TX_FULL      | Transmit FIFO is full          |
 *  | 4       | MASK_I2S_IE_FIFO_TX_OVER      | Transmit FIFO has overflowed   |
 *  | 5       | -                             | -                              |
 *  | 6       | -                             | -                              |
 *  | 7       | -                             | -                              |
 *  | 8       | MASK_I2S_IE_FIFO_RX_UNDER     | Receive FIFO has underflowed   |
 *  | 9       | MASK_I2S_IE_FIFO_RX_EMPTY     | Receive FIFO is empty          |
 *  | 10      | MASK_I2S_IE_FIFO_RX_HALF_FULL | Receive FIFO is half empty     |
 *  | 11      | MASK_I2S_IE_FIFO_RX_FULL      | Receive FIFO is full           |
 *  | 12      | MASK_I2S_IE_FIFO_RX_OVER      | Receive FIFO has overflowed    |
 *  | 13      | -                             | -                              |
 *  | 14      | -                             | -                              |
 *  | 15 (MSB)| -                             | -                              |
 *
 *  @param mask The mask of bits to enable
 */
void i2s_enable_int(uint16_t mask);

/** @brief Disable interrupts on the I2S module
 *  @param mask The mask of bits to disable
 *  @see i2s_enable_int
 */
void i2s_disable_int(uint16_t mask);

/** @brief Clear interrupt flags on the I2S module
 *  @param mask The mask of bits to clear
 */
void i2s_clear_int_flag(uint16_t mask);

/** @brief Get the status of the interrupts on the I2S module
 *  @return The copy of the Interrupt Status Register
 */
uint16_t i2s_get_status(void);

/** @brief Check if an interrupt has been fired
 *  @warning This function will clear the current interrupts you are checking for
 *  @param mask The mask of interrupts to check for
 *  @return 1 when an interrupt has fired, 0 otherwise.
 *  @see i2s_enable_int
 */
int8_t i2s_is_interrupted(uint16_t mask);

/** @brief Get the receive count of the I2S Module
 *  @return The number of receptions that have been made
 */
uint16_t i2s_get_rx_count(void);

/** @brief Get the transmit count of the I2S Module
 *  @return The number of transmissions that have been made
 */
uint16_t i2s_get_tx_count(void);

/** @brief Start transmission of the I2S Module
 *
 */
void i2s_start_tx(void);

/** @brief Stop transmission of the I2S Module
 *
 */
void i2s_stop_tx(void);

/** @brief Start reception of the I2S Module
 *
 */
void i2s_start_rx(void);

/** @brief Stop reception of the I2S Module
 *
 */
void i2s_stop_rx(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* FT900_I2S_H_ */
