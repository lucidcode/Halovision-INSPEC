/**
    @file

    @brief
     SPI

    
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

#ifndef FT900_SPI_H_
#define FT900_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/** @brief SPI Direction */
typedef enum
{
    spi_dir_slave = 0, /**< The SPI Device is in Slave mode */
    spi_dir_master = 1 /**< The SPI Device is in Master mode */
} spi_dir_t;

/** @brief The SPI mode */
typedef enum
{
    spi_mode_0,  /**< CPOL = 0, CPHA = 0 */
    spi_mode_1,  /**< CPOL = 0, CPHA = 1 */
    spi_mode_2,  /**< CPOL = 1, CPHA = 0 */
    spi_mode_3   /**< CPOL = 1, CPHA = 1 */
} spi_clock_mode_t;

/** @brief SPI Data Bus Width */
typedef enum
{
    spi_width_1bit = 1, /**< The SPI Device is working in 1 bit wide mode (i.e. 4 wire SPI) */
    spi_width_2bit = 2, /**< The SPI Device is working in 2 bit wide mode */
    spi_width_4bit = 4 /**< The SPI Device is working in 4 bit wide mode */
} spi_width_t;

/** @brief SPI FIFO size */
typedef enum
{
    spi_fifo_size_16 = 16,  /**< Use a 16 level FIFO */
    spi_fifo_size_64 = 64 /**< Use a 64 Byte FIFO */
} spi_fifo_size_t;

/** @brief SPI Interrupts 
 *  Note: Call spi_is_interrupted() to clear interrupt flags */
typedef enum
{
    spi_interrupt_transmit_empty,           /**< Either the FIFO or the data register are empty.*/
    spi_interrupt_data_ready,               /**< A transmission or reception was completed *or* the FIFO was filled to a trigger level.*/
    spi_interrupt_transmit_1bit_complete,   /**< Transmission was complete when using the SPI1BIT method.*/
    spi_interrupt_fault                     /**< The SPI device was asserted when in Master mode.*/
} spi_interrupt_t;

/** @brief SS Assertion control */
typedef enum
{
    spi_ss_assertions_force, /**< SS will reflect the staus of SPISS */
    spi_ss_assertions_auto   /**< SS will go low during transmissions if selected */
} spi_ss_assertions_t;

/** @brief SPI Options */
typedef enum
{
    spi_option_fifo,                  /**< Enable or disable the FIFO */
    spi_option_fifo_size,             /**< Set the size of the FIFO */
    spi_option_fifo_receive_trigger,  /**< Set the FIFO receive trigger level */
    spi_option_force_ss_assertions,   /**< Force SS to go low in assert */
    spi_option_bus_width,             /**< Set the SPI bus width */
    spi_option_multi_receive,       /**< Set the SPI device to clock in data without loading it into the RX FIFO */
    spi_option_fifo_transmit_trigger,  /**< Set the FIFO transmit trigger level */
	spi_option_baud_factor,         /**< Applicable in FT930 and FT900 RevC. SCK frequency is determined by BAUD FACTOR ( divided 2 to divided by 256)*/
	spi_option_fast_spi,              /**< Applicable in FT900 RevC. The master clock is set to CLK (system clock)/2 */
	spi_option_change_spi_rate,     /**< SCK frequency is determined by CHG_SPI and SPR[0:2] bits ( CLK gets divided 2 to divided by 511)*/
    spi_option_ignore_incoming
} spi_option_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Initialise the SPI device
 *
 *  @param dev the device to use
 *  @param dir The direction for the device to work in (Master/Slave)
 *  @param clock_mode The SPI Clock mode to use
 *  @param div The clock divider to use (4,8,16,32,64,128,256,512)
 *
 *  @returns 0 on a success or -1 for a failure
 *
 *  @warning This will reset the peripheral and all options
 */
int8_t spi_init(ft900_spi_regs_t* dev, spi_dir_t dir, spi_clock_mode_t clock_mode, uint16_t div);

/** @brief Disable the SPI device
 *  @param dev the device to use
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_uninit(ft900_spi_regs_t* dev);

/** @brief Select a device to start communicating with
 *  @param dev the device to use
 *  @param num The device to select
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_open(ft900_spi_regs_t* dev, uint8_t num);

/** @brief Stop communicating with a certain device.
 *  @param dev the device to use
 *  @param num The device to select
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_close(ft900_spi_regs_t* dev, uint8_t num);

/** @brief Writes a byte to the SPI device
 *  @param dev the device to use
 *  @param b The byte to send
 *  @returns The number of bytes written or -1 for a failure
 */
#define spi_write(a,b) 	spi_writen((a),&(b),1)

/** @brief Writes several bytes to the SPI device
 *
 *  @param dev the device to use
 *  @param b A pointer to the array to sendspi_open
 *  @param len The number of bytes to write
 *
 *  @returns The number of bytes written or -1 for a failure
 */
int32_t spi_writen(ft900_spi_regs_t* dev, const uint8_t *b, size_t len);

/** @brief Reads a byte from the SPI device
 *
 *  Read the most current byte in the buffer.
 *  For multi-bit wide busses (Dual and Quad), the SPI device will cause a read on the bus
 *
 *  @param dev the device to use
 *  @param b A variable to store the byte
 *
 *  @returns The number of bytes read or -1 for a failure
 */
#define spi_read(a,b) spi_readn((a),&(b),1)

/** @brief Reads several bytes from the SPI device
 *
 *  Read the most current bytes in the buffer, for a Non-FIFO mode one byte will be read
 *  For multi-bit wide busses (Dual and Quad), the SPI device will cause multiple reads on the bus
 *
 *  @param dev the device to use
 *  @param b A pointer to the array to read into
 *  @param len The number of bytes to read
 *
 *  @returns The number of bytes read or -1 for a failure
 */
int32_t spi_readn(ft900_spi_regs_t* dev, uint8_t *b, size_t len);


/** @brief Transfers single byte from the SPI device
 *
 *  Transfer single byte by sending and receiving.
 *  Transfer is valid only for single channel use case, using this api in dual/quad setting will return error.
 *
 *  @param dev the device to use
 *  @param binp A pointer to input buffer/read buffer
 *  @param bout A pointer to output buffer/write buffer
 *
 *  @returns The number of bytes transferred or -1 for a failure
 */
#define spi_xchange(a,b,c) spi_xchangen((a),(b),&(c),1)

/** @brief Exchange several bytes from the SPI device
 *
 *  Exchange len number of bytes.
 *  Exchange is valid only for single SPI channel use case and returns error otherwise.
 *
 *  @param dev the device to use
 *  @param binp A pointer to input buffer/read buffer
 *  @param bout A pointer to output buffer/write buffer
 *  @param len The number of bytes to transfer
 *
 *  @returns The number of bytes transferred or -1 for a failure
 */
int32_t spi_xchangen(ft900_spi_regs_t* dev, uint8_t *binp, uint8_t *bout, size_t len);

/** @brief Enables the SPI device to generate an interrupt
 *  @param dev the device to use
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_enable_interrupts_globally(ft900_spi_regs_t* dev);

/** @brief Disables the SPI device from generating an interrupt
 *  @param dev the device to use
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_disable_interrupts_globally(ft900_spi_regs_t* dev);

/** @brief Enables an interrupt for the SPI device
 *  @param dev the device to use
 *  @param interrupt The interrupt to enable
 *  @returns 0 on a success or -1 for a failure
 *  @see spim_enable_interrupts_globally
 */
int8_t spi_enable_interrupt(ft900_spi_regs_t* dev, spi_interrupt_t interrupt);

/** @brief Disables an interrupt for the SPI device
 *  @param dev The device to use
 *  @param interrupt The interrupt to disable
 *  @returns 0 on a success or -1 for a failure
 *  @see spim_disable_interrupts_globally
 */
int8_t spi_disable_interrupt(ft900_spi_regs_t* dev, spi_interrupt_t interrupt);

/** @brief Disables an interrupt for the QSPI device
 *  @param dev The device to use
 *  @param interrupt The interrupt to check
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 *  @see spi_disable_interrupts_globally
 */
int8_t spi_is_interrupted(ft900_spi_regs_t* dev, spi_interrupt_t interrupt);

/** @brief Return the status register
 *
 *  | Bit  | Name   | When set to 1                          | When set to 0                     |
 *  | :--: | :----- | :------------------------------------- | :-------------------------------- |
 *  | 0    | AUTOSS | Auto SS Assertions Enabled             | SSO always shows contents of SSCR |
 *  | 1    | RXFULL | Receiver FIFO is full                  | Receiver FIFO not is full         |
 *  | 2    | EMPTY  | TX FIFO or TX register empty           | TX FIFO or TX register not empty  |
 *  | 3    | IDLE   | SPI Device Idle                        | Transmission in progress          |
 *  | 4    | FAULT  | Mode Fault (SS Low in Master mode)     | --                                |
 *  | 5    | 1BITTX | End of TX from SPDR BIS Register       | --                                |
 *  | 6    | WCOL   | Data Register Write Collision occurred | --                                |
 *  | 7    | IRQ    | An interrupt occurred                  | --                                |
 *
 *  @param dev The device to use
 *  @returns A copy of the SPISTAT register
 */
uint8_t spi_status(ft900_spi_regs_t* dev);

/** @brief Control the SPI device
 *
 *  This function will set various options for the driver.
 *
 *  | Option                           | Description                  | Values                                         |
 *  | :------------------------------- | :--------------------------- | :--------------------------------------------- |
 *  | spim_option_fifo                 | Enable or disable the FIFO   | 0 = Disabled (Default) \n 1 = Enabled [Note 1] |
 *  | spim_option_fifo_size            | Set the size of the FIFO     | 16 = 16 Byte FIFO (default) \n 64 = Byte FIFO  |
 *  | spim_option_fifo_receive_trigger | Set the FIFO trigger level   | For 16 Byte FIFOs: 1, 4, 8, 14 \n For 64 Byte FIFOs: 1, 16, 32, 56 |
 *  | spim_option_force_ss_assertions  | Force SS to go low in assert | 0 = Automatic Assertions \n 1 = Force Assertions (Default) |
 *  | spim_option_bus_width            | Set the SPI bus width        | 1 = Single (Default) \n 2 = Dual \n 4 = Quad   |
 *  | spim_option_dual_quad_direction  | Set the multi-bit direction  | 0 = Read \n 1 = Write                          |
 *
 *  Note 1: Enabling the FIFO will cause the driver to clear its contents.
 *
 *  @param dev The device to use
 *  @param opt The option to configure
 *  @param val The value to use
 *
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_option(ft900_spi_regs_t* dev, spi_option_t opt, uint8_t val);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FT900_SPI_H_ */
