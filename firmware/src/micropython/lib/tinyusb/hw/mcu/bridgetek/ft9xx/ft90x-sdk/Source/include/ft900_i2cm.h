/**
    @file

    @brief
    I2C Master.

    
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

#ifndef FT900_I2C_MASTER_H_
#define FT900_I2C_MASTER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/
/** @brief Available speed modes */
typedef enum
{
	I2CM_NORMAL_SPEED = 0,  /*!< Range: 98Kbps to 100 kbps */
	I2CM_FAST_SPEED,        /*!< Range: 130Kbps to 400 kbps */
	I2CM_FAST_PLUS_SPEED,   /*!< Range: 130Kbps to 1000 kbps */
	I2CM_HIGH_SPEED         /*!< Range: 529Kbps to 3400 kbps */
} I2CM_speed_mode;

/** @brief Some common I2C clock speeds to be used in i2cm_init() */
typedef enum
{
	I2CM_100_Kbps = 100000,  	/*!< 100 kbps */
	I2CM_400_Kbps = 400000,     /*!< 400 kbps */
	I2CM_1000_Kbps = 1000000,   /*!< 1000 kbps */
	I2CM_3400_Kbps = 3400000    /*!< 3400 kbps */
} I2CM_clk_speed;


/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Call once, to initialise the master and reset it to a known state.
 *  @details It is not valid to call any other I2C Master functions before this one.
 *  @param[in] mode 		I2C Master Clock mode
 *  @param[in] i2c_clk_speed 	The clock speed of the I2C bus
 */
void i2cm_init(I2CM_speed_mode mode, uint32_t i2c_clk_speed);

/** @brief Read a specified number of bytes from an I2C device.
 *
 *  Automatically adopts burst mode if the slave supports it, and
 *  more than one byte is to be read.
 *
 *  @param[in]  addr             I2C address to write to.
 *  @param[in]  cmd              I2C command byte
 *  @param[in]  data             Buffer containing bytes to write.
 *  @param[in]  number_to_read   Number of bytes to read.
 *  @return 0 on success, -1 on an error
 */
int8_t i2cm_read(const uint8_t addr, const uint8_t cmd,
									uint8_t *data, const uint16_t number_to_read);

/** @brief   Write a specified number of bytes to an I2C device.
 *
 *  Automatically adopts burst mode if the slave supports it, and
 *  more than one byte is to be written.
 *
 *  @param[in]  addr             I2C address to write to.
 *  @param[in]  cmd              I2C command byte
 *  @param[in]  data             Buffer containing bytes to write.
 *  @param[in]  number_to_write  Number of bytes to read.
 *  @return 0 on success, -1 on an error
 */
int8_t i2cm_write(const uint8_t addr, const uint8_t cmd,
        const uint8_t *data, const uint16_t number_to_write);

/** @brief Determine I2C Master status.
 *
 *  | Bit     | Mask Name                  | Set when...                                      |
 *  | ------- | -------------------------- | ------------------------------------------------ |
 *  | 0 (LSB) | MASK_I2CM_STATUS_I2C_BUSY  | The Bus is currently busy transmitting/reveiving |
 *  | 1       | MASK_I2CM_STATUS_I2C_ERR   | An error occurred (ADDR_ACK, DATA_ACK, ARB_LOST) |
 *  | 2       | MASK_I2CM_STATUS_ADDR_ACK  | Slave address was not acknowledged               |
 *  | 3       | MASK_I2CM_STATUS_DATA_ACK  | Data was not acknowledged                        |
 *  | 4       | MASK_I2CM_STATUS_ARB_LOST  | Arbitration lost                                 |
 *  | 5       | MASK_I2CM_STATUS_I2C_IDLE  | The I2C Controller is idle                       |
 *  | 6       | MASK_I2CM_STATUS_BUS_BUSY  | The I2C Bus is busy                              |
 *  | 7 (MSB) | -                          | -                                                |
 *
 *  @return 0 on success, -1 on an error
 */
uint8_t i2cm_get_status(void);

/** @brief Enable an interrupt
 *  @param mask The bit pattern of interrupts to enable
 *
 *  | Bit     | Mask Name                           | Interrupts on...                                |
 *  | ------- | ----------------------------------- | ----------------------------------------------- |
 *  | 0 (LSB) | MASK_I2CM_FIFO_INT_ENABLE_TX_EMPTY  | When the Transmit FIFO is empty                 |
 *  | 1       | MASK_I2CM_FIFO_INT_ENABLE_TX_HALF   | When the Transmit FIFO is half empty            |
 *  | 2       | MASK_I2CM_FIFO_INT_ENABLE_TX_FULL   | When the Transmit FIFO is full                  |
 *  | 3       | MASK_I2CM_FIFO_INT_ENABLE_RX_EMPTY  | When the Receive FIFO is empty                  |
 *  | 4       | MASK_I2CM_FIFO_INT_ENABLE_RX_HALF   | When the Receive FIFO is half full              |
 *  | 5       | MASK_I2CM_FIFO_INT_ENABLE_RX_FULL   | When the Receive FIFO is full                   |
 *  | 6       | MASK_I2CM_FIFO_INT_ENABLE_I2C_INT   | When an operation is complete on the I2C Master |
 *  | 7 (MSB) | MASK_I2CM_FIFO_INT_ENABLE_DONE      |                                                 |
 *
 *  @return 0 on success, -1 on an error
 */
int8_t i2cm_interrupt_enable(uint8_t mask);

/** @brief Disable an interrupt
 *  @see i2cm_interrupt_enable
 *  @param mask The bit pattern of interrupts to disable
 *  @return 0 on success, -1 on an error
 */
int8_t i2cm_interrupt_disable(uint8_t mask);

/** @brief Check that an interrupt has been fired
 *  @see i2cm_interrupt_enabled
 *  @param mask The bit pattern of interrupts to check
 *  @return 1 if the interrupt has been fired, 0 if the interrupt has not been fired, -1 otherwise
 */
int8_t i2cm_is_interrupted(uint8_t mask);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* FT900_I2C_MASTER_H_ */
