/**
    @file ft900_i2cs.h

    @brief
    I2C Slave.

    
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


#ifndef FT900_I2C_SLAVE_H_
#define FT900_I2C_SLAVE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief   Call once, to initialise the slave and reset it to a known state.
 *  @param[in]  addr Slave (read or write) address
 */
void i2cs_init(uint8_t addr);

/** @brief Read a specified number of bytes from the I2C Slave.
 *
 *  This transaction is orchestrated by the I2C Master.
 *  The number of bytes written may be less than the number
 *  requested if the master terminates the transaction early.
 *
 *  @param[out] data Caller-allocated buffer to receive any bytes read.
 *  @param[in] size The number of bytes to read.
 *  @return 0 on success, -1 on an error
 */
int8_t i2cs_read(uint8_t *data, size_t size);

/** @brief Write a specified number of bytes to an open device.
 *
 *  This transaction is orchestrated by the I2C Master.
 *  The number of bytes written may be less than the number
 *  requested if the master terminates the transaction early.
 *
 *  @param[in] data Buffer containing bytes to write.
 *  @param[in] size The number of bytes to write.
 *  @return 0 on success, -1 on an error
 */
int8_t i2cs_write(const uint8_t *data, size_t size);

/** @brief   Determine I2C Slave status.
 *
 *  | Bit     | Mask Name                  | Set when...                                      |
 *  | ------- | -------------------------- | ------------------------------------------------ |
 *  | 0 (LSB) | MASK_I2CS_STATUS_RX_REQ    | The slave controller head received data          |
 *  | 1       | MASK_I2CS_STATUS_TX_REQ    | The slave controller is transmitter and requires data |
 *  | 2       | MASK_I2CS_STATUS_SEND_FIN  | The master has ended the receive operation       |
 *  | 3       | MASK_I2CS_STATUS_REC_FIN   | The master has ended the transmit operation      |
 *  | 4       | MASK_I2CS_STATUS_BUS_ACTV  | The bus is currently busy with an operation      |
 *  | 5       | -                          | -                                                |
 *  | 6       | MASK_I2CS_STATUS_DEV_ACTV  | The slave controller is enabled                  |
 *  | 7 (MSB) | -                          | -                                                |
 *
 *  @return Returns the status byte of the I2C Slave Status register
 */
uint8_t i2cs_get_status(void);

/** @brief Enable an interrupt
 *  @param mask The bit pattern of interrupts to enable
 *  @return 0 on success, -1 on an error
 */
int8_t i2cs_enable_interrupt(uint8_t mask);

/** @brief Disable an interrupt
 *  @param mask The bit pattern of interrupts to disable
 *  @return 0 on success, -1 on an error
 */
int8_t i2cs_disable_interrupt(uint8_t mask);

/** @brief Check the status of an interrupt
 *  @param mask The bit pattern of interrupts to check
 *  @return 1 if the interrupt has been fired, 0 if the interrupt has not been fired, -1 otherwise
 */
int8_t i2cs_is_interrupted (uint8_t mask);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* FT900_I2C_SLAVE_H_ */
