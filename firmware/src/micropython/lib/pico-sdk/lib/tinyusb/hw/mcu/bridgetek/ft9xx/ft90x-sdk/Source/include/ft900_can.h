/**
    @file ft900_can.h

    @brief
    CANBus

    
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

#ifndef FT900_CAN_H_
#define FT900_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/** @brief The mode of the CAN device */
typedef enum
{
    can_mode_normal, /**< The CAN Device is operating normally */
    can_mode_listen  /**< The CAN Device is in listen-only mode and will not allow data transmission
                          or send ACKs on the bus */
} can_mode_t;

/** @brief The mode of the filter */
typedef enum
{
    can_filter_mode_single, /**< Single Filter mode */
    can_filter_mode_dual    /**< Dual Filter mode */
} can_filter_mode_t;

/** @brief The location of where arbitration was lost */
typedef enum
{
    can_arbitration_lost_id28_11 = 0,   /**< Arbitration was lost on the 12th bit of the ID (29th bit extended) */
    can_arbitration_lost_id27_10,       /**< Arbitration was lost on the 11th bit of the ID (28th bit extended) */
    can_arbitration_lost_id26_9,        /**< Arbitration was lost on the 10th bit of the ID (27th bit extended) */
    can_arbitration_lost_id25_8,        /**< Arbitration was lost on the 9th bit of the ID (26th bit extended) */
    can_arbitration_lost_id24_7,        /**< Arbitration was lost on the 8th bit of the ID (25th bit extended) */
    can_arbitration_lost_id23_6,        /**< Arbitration was lost on the 7th bit of the ID (24th bit extended) */
    can_arbitration_lost_id22_5,        /**< Arbitration was lost on the 6th bit of the ID (23th bit extended) */
    can_arbitration_lost_id21_4,        /**< Arbitration was lost on the 5th bit of the ID (22th bit extended) */
    can_arbitration_lost_id20_3,        /**< Arbitration was lost on the 4th bit of the ID (21th bit extended) */
    can_arbitration_lost_id19_2,        /**< Arbitration was lost on the 3rd bit of the ID (20th bit extended) */
    can_arbitration_lost_id18_1,        /**< Arbitration was lost on the 2nd bit of the ID (19th bit extended) */
    can_arbitration_lost_id17_0,        /**< Arbitration was lost on the 1st bit of the ID (18th bit extended) */
    can_arbitration_lost_srtr_rtr,      /**< Arbitration was lost on the SRTR/RTR bit */
    can_arbitration_lost_id16,          /**< Arbitration was lost on the 17th bit of the extended ID */
    can_arbitration_lost_id15,          /**< Arbitration was lost on the 16th bit of the extended ID */
    can_arbitration_lost_id14,          /**< Arbitration was lost on the 15th bit of the extended ID */
    can_arbitration_lost_id13,          /**< Arbitration was lost on the 14th bit of the extended ID */
    can_arbitration_lost_id12,          /**< Arbitration was lost on the 13th bit of the extended ID */
    can_arbitration_lost_id11,          /**< Arbitration was lost on the 12th bit of the extended ID */
    can_arbitration_lost_id10,          /**< Arbitration was lost on the 11th bit of the extended ID */
    can_arbitration_lost_id9,           /**< Arbitration was lost on the 10th bit of the extended ID */
    can_arbitration_lost_id8,           /**< Arbitration was lost on the 9th bit of the extended ID */
    can_arbitration_lost_id7,           /**< Arbitration was lost on the 8th bit of the extended ID */
    can_arbitration_lost_id6,           /**< Arbitration was lost on the 7th bit of the extended ID */
    can_arbitration_lost_id5,           /**< Arbitration was lost on the 6th bit of the extended ID */
    can_arbitration_lost_id4,           /**< Arbitration was lost on the 5th bit of the extended ID */
    can_arbitration_lost_id3,           /**< Arbitration was lost on the 4th bit of the extended ID */
    can_arbitration_lost_id2,           /**< Arbitration was lost on the 3rd bit of the extended ID */
    can_arbitration_lost_id1,           /**< Arbitration was lost on the 2nd bit of the extended ID */
    can_arbitration_lost_id0,           /**< Arbitration was lost on the 1st bit of the extended ID */
    can_arbitration_lost_rtr,           /**< Arbitration was lost on the RTR bit */
    can_arbitration_lost_invalid = 255  /**< No valid arbitration could be found */
} can_arbitration_lost_t;

/** @brief CAN message type. */
typedef enum
{
    can_type_standard,  /**< CAN2.0A Standard Frame */
    can_type_extended   /**< CAN2.0B Extended Frame */
} can_type_t;

/** @brief Remote transfer request flag. */
typedef enum
{
    can_rtr_none,           /**< No RTR */
    can_rtr_remote_request  /**< RTR Set */
} can_rtr_t;

/** @brief CAN Peripheral Interrupts */
typedef enum
{
    can_interrupt_data_overrun,
    can_interrupt_bus_error,
    can_interrupt_transmit,
    can_interrupt_receive,
    can_interrupt_error_passive,
    can_interrupt_error_warning,
    can_interrupt_arbitration_lost,
	can_interrupt_data_overload  /* This enum for FT900 Rev C onwards only */
} can_interrupt_t;

/** @brief A CAN Message */
typedef struct
{
    uint32_t            id;                     /**< The ID of the CAN Message */
    can_type_t          type;                   /**< The ID Extension bit of the CAN Message (1 = Extended) */
    can_rtr_t           rtr;                    /**< The Remote Transmit Request bit of the CAN Message */
    uint8_t             dlc;                    /**< The Data Length Code of the CAN Message (0 to 8) */
    uint8_t             data[8];                /**< The payload of the CAN Message */
} can_msg_t;

/** @brief A CAN Filter configuration
 *
 *  This struct is used to configure the filters within the CAN device.
 *
 *  @warning Masks are inverted meaning that a set bit refers to a "don't care"
 */
typedef struct
{
    can_type_t          type;                   /**< The type of message this filter will match for */
    uint32_t            id;                     /**< The ID Stub to match for */
    uint32_t            idmask;                 /**< The ID Mask */
    can_rtr_t           rtr;                    /**< The RTR Stub to match for */
    uint8_t             rtrmask;                /**< The RTR Mask */
    uint8_t             data0;                  /**< The stub of the first data byte to match for */
    uint8_t             data0mask;              /**< The mask of the first data byte */
    uint8_t             data1;                  /**< The stub of the second data byte to match for */
    uint8_t             data1mask;              /**< The mask of the second data byte to match for */
} can_filter_t;

/** @brief A struct used to configure the timing of the CAN device */
typedef struct
{
    uint8_t             baud_rate_prescaler;    /**< The prescaler to determine internal clock of the CAN device (0 to 63) */
    uint8_t             tseg1;                  /**< The value of TSEG1 for determining the sample position (0 to 15) */
    uint8_t             tseg2;                  /**< The value of TSEG2 for determining the sample position (0 to 7) */
    uint8_t             samples;                /**< The number of samples to use to sample bits (1 or 3) */
    uint8_t             sync_jump_width;        /**< Sync Jump Width (0 to 3) */
} can_time_config_t;

/* GLOBAL VARIABLES ****************************************************************/
/* Default speeds for 100 MHz Fperiph */
extern const can_time_config_t g_can1Mbaud;     /**< Configuration for 1 MBaud at \f$ f_{cpu} \f$ = 100 MHz */
extern const can_time_config_t g_can500kbaud;   /**< Configuration for 500 kBaud at \f$ f_{cpu} \f$ = 100 MHz */
extern const can_time_config_t g_can250kbaud;   /**< Configuration for 250 kBaud at \f$ f_{cpu} \f$ = 100 MHz */
extern const can_time_config_t g_can125kbaud;   /**< Configuration for 125 kBaud at \f$ f_{cpu} \f$ = 100 MHz */

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Initialise the CAN device
 *  @param dev A pointer to the device to use
 *  @param mode The mode that this CAn device should be in (normal or listen)
 *  @param timeconfig The configuration struct that defines the timing of the device
 *  @returns 0 on a success, -1 otherwise
 */
int8_t can_init(ft900_can_regs_t *dev, can_mode_t mode, const can_time_config_t *timeconfig);

/** @brief Open the CAN device for reading and writing
 *  @param dev A pointer to the device to use
 *  @returns 0 on a success, -1 otherwise
 */
int8_t can_open(ft900_can_regs_t *dev);

/** @brief Clsoe the CAN device for reading and writing
 *  @param dev A pointer to the device to use
 *  @returns 0 on a success, -1 otherwise
 */
int8_t can_close(ft900_can_regs_t *dev);

/** @brief Send a message on the CAN Bus
 *
 *  This function will accept a can_msg_t and pack it into a format to be fed into the CAN Transmit
 *  FIFO.
 *
 *  @param dev A pointer to the device to use
 *  @param msg The message to send
 *  @returns 0 on a success, -1 otherwise
 *  @warning This command only works when the CAN device is open
 */
int8_t can_write(ft900_can_regs_t *dev, const can_msg_t *msg);

/** @brief Receive a message from the CAN Bus
 *
 *  This function will take the first available message from the Receive FIFO.
 *
 *  @param dev A pointer to the device to use
 *  @param msg The struct to pack the message into
 *
 *  @returns 0 on a success, -1 otherwise
 *
 *  @warning This command only works when the CAN device is open
 *
 *  @warning This function will automatically clear the Receive interrupt flag and increment the
 *           Receive message counter.
 */
int8_t can_read(ft900_can_regs_t *dev, can_msg_t *msg);

/** @brief Return how many messages are available in the receive FIFO
 *  @param dev A pointer to the device to use
 *  @returns The number of messages available in the receive FIFO
 */
uint8_t can_available(ft900_can_regs_t *dev);

/** @brief Abort the transmission of messages on the CAN Bus
 *
 *  This function will cause the CAN device to abort transmission on the CAN Bus. After the
 *  transmission of the current message on the Bus, no further transmissions will occur (including
 *  retransmissions for erroneous messages).
 *
 *  @param dev A pointer to the device to use
 *
 *  @returns 0 on a success, -1 otherwise
 */
int8_t can_abort(ft900_can_regs_t *dev);

/** @brief Enable an Interrupt
 *
 *  Enable the CAN device to generate an interrupt.  The value of mask is a bit-mask with the
 *  following format:
 *
 *  | Bit     | Name      | Description                 | Trigger                                          |
 *  | ------- | :-------: | :---------------------------| :----------------------------------------------- |
 *  | 7 (MSB) |           | Unused                      |                                                  |
 *  | 6       | ARB_LOST  | Arbitration Lost Interrupt  | Arbitration is lost                              |
 *  | 5       | ERR_WARN  | Error Warning Interrupt     | Changes in ES or BS of the Status register       |
 *  | 4       | ERR_PSV   | Error Passive Interrupt     | Bus enters or exits a passive state              |
 *  | 3       | RX        | Receive Interrupt           | A message is received on CAN                     |
 *  | 2       | TX        | Transmit Interrupt          | A message is successfully received on CAN        |
 *  | 1       | BUS_ERR   | Bus Error Interrupt         | A bus error occurred when transmitting\\receiving |
 *  | 0 (LSB) | DATA_OVRN | Data Overrun Interrupt      | A receive FIFO overrun occurred                  |
 *
 *  @param dev A pointer to the device to use
 *  @param interrupt The interrupt to enable
 *
 *  @returns 0 on a success, -1 otherwise
 *
 *  @warning This command only works when the CAN device is closed
 *
 */
int8_t can_enable_interrupt(ft900_can_regs_t *dev, can_interrupt_t interrupt);

/** @brief Disable an Interrupt
 *
 *  Disable the CAN device from generate an interrupt.
 *
 *  @param dev A pointer to the device to use
 *  @param interrupt The interrupt to disable
 *
 *  @returns 0 on a success, -1 otherwise
 *
 *  @warning This command only works when the CAN device is closed
 */
int8_t can_disable_interrupt(ft900_can_regs_t *dev, can_interrupt_t interrupt);

/** @brief Query the Interrupt register
 *
 *  Query the Interrupt register in order to determine what caused the interrupt.
 *
 *  @param dev A pointer to the device to use
 *  @param interrupt The interrupt to check
 *
 *  @warning This function clears the interrupt bit so that it does not fire constantly
 *
 *  @returns 0 when the interrupt hasn't been fired, 1 when the interrupt has fired and -1 otherwise
 */
int8_t can_is_interrupted(ft900_can_regs_t *dev, can_interrupt_t interrupt);

/** @brief Query the status register
 *
 *  The return value is a bit-mask with the following format:
 *
 *  | Bit     | Name        | Description                 | Set when...                                   |
 *  | ------- | :---------: | :-------------------------- | :-------------------------------------------- |
 *  | 7 (MSB) | RX_BUF_STS  | Receive Buffer Status       | At least one message in the receive FIFO      |
 *  | 6       | OVRN_STS    | Data Overrun Status         | The receive FIFO has encountered an overrun   |
 *  | 5       | TX_BUF_STS  | Transmit Buffer Status      | The CPU is able to write to the transmit FIFO |
 *  | 4       | -           |                             |                                               |
 *  | 3       | RX_STS      | Receive Status              | CAN is currently receiving a message          |
 *  | 2       | TX_STS      | Transmit Status             | CAN is currently transmitting a message       |
 *  | 1       | ERR_STS     | Error Status                | One CAN error counter has reached warning (96)|
 *  | 0 (LSB) | BUS_OFF_STS | Bus Off Status              | The CAN device is in a bus off state          |
 *
 *  @param dev A pointer to the device to use
 *
 *  @returns A bit-mask of the current status or 0 for an unknown device.
 */
uint8_t can_status(ft900_can_regs_t *dev);

/** @brief Get the current number of receive errors reported by the CAN device
 *  @param dev A pointer to the device to use
 *  @returns The current number of receive errors (0 - 255) or 0 for an unknown device.
 */
uint8_t can_rx_error_count(ft900_can_regs_t *dev);

/** @brief Get the current number of transmit errors reported by the CAN device
 *
 *  When the transmit error counter exceeds limit of 255, the Bus Status bit in the Status register
 *  is set to logic 1 (bus off), the CAN controller set reset mode, and if enabled an error warning
 *  interrupt is generated. The transmit error counter is then set to 127 and receive error counter
 *  is cleared.
 *
 *  @param dev A pointer to the device to use
 *
 *  @returns The current number of receive errors (0 - 255) or 0 for an unknown device.
 */
uint8_t can_tx_error_count(ft900_can_regs_t *dev);

/** @brief Get the current value of the ECC (Error Code Capture) register.
 *
 *  This function will return the value of the ECC (Error Code Capture) register. This register
 *  holds the error code for the _LAST_ bus error that occurred on the CAN network.
 *
 *  The return value is a bit-mask with the following format:
 *
 *  | Bit     | Name     | Description           | Set when...                                   |
 *  | ------- | :------: | :-------------------- | :-------------------------------------------- |
 *  | 7 (MSB) | RX_WRN   | Receive Warning       | The number of receive errors is >= 96         |
 *  | 6       | TX_WRN   | Transmit Warning      | The number of transmit errors is >= 96        |
 *  | 5       | ERR_DIR  | Direction             | The error occurred on reception.              |
 *  | 4       | ACK_ERR  | Acknowledgement Error | An ACK Error occurred                         |
 *  | 3       | FRM_ERR  | Form Error            | A Form Error occurred                         |
 *  | 2       | CRC_ERR  | CRC Error             | A CRC Error occurred                          |
 *  | 1       | STF_ERR  | Stuff Error           | A Bit Stuffing Error occurred                 |
 *  | 0 (LSB) | BIT_ERR  | Bit Error             | A Bit Error occurred                          |
 *
 *  @param dev A pointer to the device to use
 *
 *  @returns The value of the ECC (Error Code Capture) register or 0 for an unknown device.
 */
uint8_t can_ecode(ft900_can_regs_t *dev);

/** @brief Get the location where arbitration was lost.
 *  @param dev A pointer to the device to use
 *  @returns The location where arbitration was lost
 */
can_arbitration_lost_t can_arbitration_lost_location(ft900_can_regs_t *dev);

/** @brief Set up a filter for the CAN device
 *
 *  Set up the CAN device to filter for specific criteria.
 *
 *  The filter can work in two modes: single or dual. Depending on which mode the filter is in
 *  certain types of information can be used, as shown in the table below.
 *
 *  | Mode   | Message Type | ID                 | RTR | Data[0]       | Data[1] |
 *  | :----- | :----------- | :----------------: | :-: | :-----------: | :-----: |
 *  | Single | Standard     | Yes                | Yes | Yes           | Yes     |
 *  | Single | Extended     | Yes                | Yes | No            | No      |
 *  | Dual   | Standard     | Yes                | Yes | Filter 0 Only | No      |
 *  | Dual   | Extended     | Only bits 13 to 28 | No  | No            | No      |
 *
 *  Any field which is not used in a certain configuration will be ignored.
 *
 *  @param dev A pointer to the device to use
 *  @param filtmode The mode that the filters should be in (single or dual)
 *  @param filternum The number of filter to use. When in single mode, only 0 will work.
 *  @param filter A pointer to the configuration to use
 *
 *  @returns 0 on a success, -1 otherwise
 *
 *  @warning This command only works when the CAN device is closed
 *
 *  @see can_filter_t
 */
int8_t can_filter(ft900_can_regs_t *dev, can_filter_mode_t filtmode, uint8_t filternum, const can_filter_t *filter);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_CAN_H_ */
