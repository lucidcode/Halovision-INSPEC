/**
    @file

    @brief
    CANBus Interface

    
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
#include <ft900_can.h>
#include <registers/ft900_registers.h>
#include <ft900_sys.h>

/* CONSTANTS ***********************************************************************/
/* Bit masks for dissecting IDs */
#define CAN_U32_MASK_0_TO_2     (0x00000007UL)
#define CAN_U32_MASK_3_TO_10    (0x000007F8UL)
#define CAN_U32_MASK_0_TO_4     (0x0000001FUL)
#define CAN_U32_MASK_5_TO_12    (0x00001FE0UL)
#define CAN_U32_MASK_13_TO_20   (0x001FE000UL)
#define CAN_U32_MASK_21_TO_28   (0x1FE00000UL)
/*#define CAN_U32_MASK_0_TO_10    (0x000003FFUL)*/
/*#define CAN_U32_MASK_5_TO_28    (0x1FFFFFE0UL)*/
#define CAN_U32_MASK_BYTE1      (0x000000FFUL)
#define CAN_U32_MASK_BYTE2      (0x0000FF00UL)
#define CAN_U32_MASK_BYTE3      (0x00FF0000UL)
#define CAN_U32_MASK_BYTE4      (0xFF000000UL)
#define CAN_U32_MASK_DLC        (0x0000000FUL)
#define CAN_U32_MASK_RTR        (0x00000040UL)
#define CAN_U32_MASK_FF         (0x00000080UL)

/* GLOBAL VARIABLES ****************************************************************/
const can_time_config_t g_can1Mbaud =
{
    4, /* baud_rate_prescaler */
    4, /* tseg1 */
    3, /* tseg2 */
    1, /* samples */
    1  /* sync_jump_width */
};

const can_time_config_t g_can500kbaud =
{
    9, /* baud_rate_prescaler */
    4, /* tseg1 */
    3, /* tseg2 */
    1, /* samples */
    1  /* sync_jump_width */
};

const can_time_config_t g_can250kbaud =
{
    19, /* baud_rate_prescaler */
    4, /* tseg1 */
    3, /* tseg2 */
    1, /* samples */
    1  /* sync_jump_width */
};

const can_time_config_t g_can125kbaud =
{
    39, /* baud_rate_prescaler */
    4, /* tseg1 */
    3, /* tseg2 */
    1, /* samples */
    1  /* sync_jump_width */
};

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/
#define can_reset_activate(dev) dev->CAN_MODE |= MASK_CAN_MODE_RST
#define can_reset_deactivate(dev) dev->CAN_MODE &= ~MASK_CAN_MODE_RST
#define can_is_reset(dev) ((dev->CAN_MODE & MASK_CAN_MODE_RST) > 0)

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

/** @brief Initialise the CAN device
 *  @param dev A pointer to the device to use
 *  @param mode The mode that this CAn device should be in (normal or listen)
 *  @param timeconfig The configuration struct that defines the timing of the device
 *  @returns 0 on a success, -1 otherwise
 */
int8_t can_init(ft900_can_regs_t *dev, can_mode_t mode, const can_time_config_t *timeconfig)
{
    int8_t iRet = 0;
        
    /* Check the number given is an actual device... */
    if (dev == NULL)
    {
        iRet = -1;
    }
    
    
    if (iRet == 0)
    {
        /* Check other parameters for sanity... */
        if (mode < can_mode_normal ||
            mode > can_mode_listen)
        {
            /* Out of range mode value */
            iRet = -1;
        }
        
        if (timeconfig->baud_rate_prescaler > 63 || 
            timeconfig->tseg1 > 15               ||
            timeconfig->tseg2 > 7                ||
            (timeconfig->samples != 1 && timeconfig->samples != 3) ||
            timeconfig->sync_jump_width > 3)
        {
            /* Something was wrong with timeconfig */
            iRet = -1;
        }
    }
    
    
    if (iRet == 0)
    {
    	if (!can_is_reset(dev))
    	{
    		/* The device is open, close it */
    		can_close(dev);
    	}
        
        /* Set up the Mode and Filter Mode... */
        if (mode == can_mode_normal)
        {
            dev->CAN_MODE &= ~MASK_CAN_MODE_LSTN_ONLY; /* Disable Listen only mode */
        }
        else
        {
            dev->CAN_MODE |= MASK_CAN_MODE_LSTN_ONLY; /* Set Listen only mode */
        }
        
        /* Set up the timings... */
        
        /*      __    __    __    __    __    __    __    __    __    __ 
        tSCL = |  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__...
        
               |<----------------- One Nominal Bit Time ------------------>|
        
               +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+...
               |SYNC |           tTSEG1            |        tTSEG2         |
               | SEG |                             |                       |
               +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+...
                                       ^     ^     ^
        Sample Points ---------------> 1     2     3
                
        In single sample mode, only Sample Point 3 is taken.
        In three sample mode, all three Sample Points are taken.
        
        tTSEG1 = tSCL * (TSEG1 + 1)
        tTSEG2 = tSCL * (TSEG2 + 1)
        */
        
        /* BTR0 -----------------------------------------------------------------
        |    7   |    6   |    5   |    4   |    3   |    2   |    1   |    0   |
        +--------+--------+--------+--------+--------+--------+--------+--------+
        | SJW[1] | SJW[0] | BRP[5] | BRP[4] | BRP[3] | BRP[2] | BRP[1] | BRP[0] |
        -------------------------------------------------------------------------
        
        SJW = Sync Jump Width 
        BRP = Baud Rate Pre-scaler
        */
        dev->CAN_BUS_TIM_0 = ((timeconfig->sync_jump_width << BIT_CAN_BUS_TIM_0_SYNC_JMP_WDT) & MASK_CAN_BUS_TIM_0_SYNC_JMP_WDT) |
                        ((timeconfig->baud_rate_prescaler << BIT_CAN_BUS_TIM_0_BAUD_PSCL) & MASK_CAN_BUS_TIM_0_BAUD_PSCL);
                     
        /* BTR1 -----------------------------------------------------------------
        |    7   |    6   |    5   |    4   |    3   |    2   |    1   |    0   |
        +--------+--------+--------+--------+--------+--------+--------+--------+
        |   SAM  |TSEG2[2]|TSEG2[1]|TSEG2[0]|TSEG1[3]|TSEG1[2]|TSEG1[1]|TSGE1[0]|
        -------------------------------------------------------------------------
        
        SAM = Number of samples (0 = once, 1 = three times)
        TSEG2 = Number of CAN clock cycles for TSEG2
        TSEG1 = Number of CAN clock cycles for TSEG1
        */
        dev->CAN_BUS_TIM_1 = ((timeconfig->tseg2 << BIT_CAN_BUS_TIM_1_TIM_SEG2) & MASK_CAN_BUS_TIM_1_TIM_SEG2) |
                        ((timeconfig->tseg1 << BIT_CAN_BUS_TIM_1_TIM_SEG1) & MASK_CAN_BUS_TIM_1_TIM_SEG1);
                      
        if (timeconfig->samples == 3)
        {
            dev->CAN_BUS_TIM_1 |= MASK_CAN_BUS_TIM_1_NUM_SAM; /* Set Three Sample Mode */
        }
        else
        {
            dev->CAN_BUS_TIM_1 &= ~MASK_CAN_BUS_TIM_1_NUM_SAM; /* Set Single Sample Mode */
        }
        
        /* Set the filter to catch all messages... */
        dev->CAN_ACC_ENABLE_0 = 0xFF;
        dev->CAN_ACC_ENABLE_1 = 0xFF;
        dev->CAN_ACC_ENABLE_2 = 0xFF;
        dev->CAN_ACC_ENABLE_3 = 0xFF;

    }
    
    return iRet;
}


/** @brief Open the CAN device for reading and writing
 *  @param dev A pointer to the device to use
 *  @returns 0 on a success, -1 otherwise
 */
int8_t can_open(ft900_can_regs_t *dev)
{
    int8_t iRet = 0;

    /* Check the number given is an actual device... */
    if (dev == NULL)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
    	if (can_is_reset(dev))
    	{
    		/* Bring the device out of reset */
    		can_reset_deactivate(dev);
    	}
    	else
    	{
    		/* Already open */
    	    iRet = -1;
    	}
    }

    return iRet;
}


/** @brief Clsoe the CAN device for reading and writing
 *  @param dev A pointer to the device to use
 *  @returns 0 on a success, -1 otherwise
 */
int8_t can_close(ft900_can_regs_t *dev)
{
    int8_t iRet = 0;

    /* Check the number given is an actual device... */
    if (dev == NULL)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
    	if (!can_is_reset(dev))
    	{
    		/* Abort the current transmission */
    		can_abort(dev);
    		/* Bring the device into reset */
    		can_reset_activate(dev);
    	}
    	else
    	{
    		/* Not open */
    	    iRet = -1;
    	}
    }

    return iRet;
}
                     

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
int8_t can_write(ft900_can_regs_t *dev, const can_msg_t *msg)
{
    int8_t iRet = 0;
        
    /* Check the number given is an actual device... */
    if (dev == NULL)
    {
        iRet = -1;
    }
    
    if (can_is_reset(dev))
    {
    	/* Device isn't open */
        iRet = -1;
    }

    if (iRet == 0)
    {
        /* Sanity check the message given... */
        if (msg->type < can_type_standard ||
            msg->type > can_type_extended ||
            msg->rtr < can_rtr_none ||
            msg->rtr > can_rtr_remote_request ||
            /*msg->dlc < 0 ||*/
            msg->dlc > 8 )
        {
            iRet = -1;
        }
    }
    
    if (iRet == 0)
    {
        /* Send a message on the CANBus... */

        /* Check that you can write into the buffer... */
        while (!(dev->CAN_STATUS & MASK_CAN_STATUS_TX_BUF_STS))
        {
            /* Do Nothing whilst the FIFO is locked */
        }


        if (msg->type == can_type_standard)
        {
            dev->CAN_TX_BUF = (0 << 7) | (msg->rtr == can_rtr_remote_request) << 6 | (msg->dlc << 0) |
                      (msg->id & CAN_U32_MASK_3_TO_10) << 5 |
                      (msg->id & CAN_U32_MASK_0_TO_2) << 21 |
                      (msg->data[0] << 24);

            if (msg->rtr == can_rtr_none && msg->dlc > 1)
            {
                dev->CAN_TX_BUF = msg->data[1] << 0 | msg->data[2] << 8 | msg->data[3] << 16 | msg->data[4] << 24;
            }

            if (msg->rtr == can_rtr_none && msg->dlc > 5)
            {
                dev->CAN_TX_BUF = msg->data[5] << 0 | msg->data[6] << 8 | msg->data[7] << 16;
            }

        }
        else /* Extended message */
        {
            dev->CAN_TX_BUF = (1 << 7) | (msg->rtr == can_rtr_remote_request) << 6 | (msg->dlc << 0) |
                      (msg->id & CAN_U32_MASK_21_TO_28) >> 13 |
                      (msg->id & CAN_U32_MASK_13_TO_20) << 3 |
                      (msg->id & CAN_U32_MASK_5_TO_12) << 19;

            dev->CAN_TX_BUF = (msg->id & CAN_U32_MASK_0_TO_4) << 3 |
                      msg->data[0] << 8 | msg->data[1] << 16 | msg->data[2] << 24;

            if (msg->rtr == can_rtr_none && msg->dlc > 3)
            {
                dev->CAN_TX_BUF = msg->data[3] << 0 | msg->data[4] << 8 | msg->data[5] << 16 | msg->data[6] << 24;
            }

            if (msg->rtr == can_rtr_none && msg->dlc > 7)
            {
                dev->CAN_TX_BUF = msg->data[7] << 0;
            }
        }

        /* Set the CAN device to transmit */
        dev->CAN_CMD |= MASK_CAN_CMD_TX_REQ;

    }
    
    return iRet;
}


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
int8_t can_read(ft900_can_regs_t *dev, can_msg_t *msg)
{
    int8_t iRet = 0;
    uint32_t lTemp32bits = 0;
        
    /* Check the number given is an actual device... */
    if (dev == NULL)
    {
        iRet = -1;
    }
    
    if (can_is_reset(dev))
    {
    	/* Device isn't open */
        iRet = -1;
    }
    
    if (iRet == 0)
    {
        /* Check that there is something in the something in the FIFO */
        if (dev->CAN_STATUS & MASK_CAN_STATUS_RX_BUF_STS)
        {
            lTemp32bits = dev->CAN_RX_BUF;
            
            if (lTemp32bits & CAN_U32_MASK_FF) /* IDE/FF bit */
            {
                msg->type = can_type_extended;
            }
            else
            {
                msg->type = can_type_standard;
            }
            
            if (lTemp32bits & CAN_U32_MASK_RTR) /* RTR bit */
            {
                msg->rtr = can_rtr_remote_request;
            }
            else
            {
                msg->rtr = can_rtr_none;
            }
            
            msg->dlc = lTemp32bits & CAN_U32_MASK_DLC;
            
            
            if (msg->type == can_type_standard)
            {
                msg->id =  (lTemp32bits & CAN_U32_MASK_BYTE2) >> 5;  /* ID [3..10] */
                msg->id |= (lTemp32bits & CAN_U32_MASK_BYTE3) >> 21; /* ID [0..2] */
                msg->data[0] = (lTemp32bits & CAN_U32_MASK_BYTE4) >> 24;
                
                if (msg->rtr == can_rtr_none && msg->dlc > 1)
                {
                    lTemp32bits = dev->CAN_RX_BUF;
                    msg->data[1] = (lTemp32bits & CAN_U32_MASK_BYTE1);
                    msg->data[2] = (lTemp32bits & CAN_U32_MASK_BYTE2) >> 8;
                    msg->data[3] = (lTemp32bits & CAN_U32_MASK_BYTE3) >> 16;
                    msg->data[4] = (lTemp32bits & CAN_U32_MASK_BYTE4) >> 24;
                }
                
                if (msg->rtr == can_rtr_none && msg->dlc > 5)
                {
                    lTemp32bits = dev->CAN_RX_BUF;
                    msg->data[5] = (lTemp32bits & CAN_U32_MASK_BYTE1);
                    msg->data[6] = (lTemp32bits & CAN_U32_MASK_BYTE2) >> 8;
                    msg->data[7] = (lTemp32bits & CAN_U32_MASK_BYTE3) >> 16;
                }
            }
            else /* Extended ID */
            {
                msg->id =  (lTemp32bits & CAN_U32_MASK_BYTE2) << 13; /* ID[21.28] */
                msg->id |= (lTemp32bits & CAN_U32_MASK_BYTE3) >> 3; /* ID[13..20] */
                msg->id |= (lTemp32bits & CAN_U32_MASK_BYTE4) >> 19; /* ID[5..12] */
                
                lTemp32bits = dev->CAN_RX_BUF;
                msg->id |= (lTemp32bits & CAN_U32_MASK_BYTE1) >> 3; /* ID[5..12] */
                msg->data[0] = (lTemp32bits & CAN_U32_MASK_BYTE2) >> 8;
                msg->data[1] = (lTemp32bits & CAN_U32_MASK_BYTE3) >> 16;
                msg->data[2] = (lTemp32bits & CAN_U32_MASK_BYTE4) >> 24;
                
                if (msg->rtr == can_rtr_none && msg->dlc > 3)
                {
                    lTemp32bits = dev->CAN_RX_BUF;
                    msg->data[3] = (lTemp32bits & CAN_U32_MASK_BYTE1);
                    msg->data[4] = (lTemp32bits & CAN_U32_MASK_BYTE2) >> 8;
                    msg->data[5] = (lTemp32bits & CAN_U32_MASK_BYTE3) >> 16;
                    msg->data[6] = (lTemp32bits & CAN_U32_MASK_BYTE4) >> 24;
                }
                
                if (msg->rtr == can_rtr_none && msg->dlc > 7)
                {
                    lTemp32bits = dev->CAN_RX_BUF;
                    msg->data[7] = (lTemp32bits & CAN_U32_MASK_BYTE1);
                }
            }

            /* Flag that a message has been received and decrement the message counter */
            dev->CAN_INT_STATUS |= MASK_CAN_INT_STATUS_RX;
            
        }
        else
        {
            /* There is nothing in the RX FIFO */
            iRet = -1;
        }
    }
    
    
    return iRet;
}


/** @brief Return how many messages are available in the receive FIFO
 *  @param dev A pointer to the device to use
 *  @returns The number of messages available in the receive FIFO
 */
uint8_t can_available(ft900_can_regs_t *dev)
{
    uint8_t iAvailable = 0;
        
    /* Check the number given is an actual device... */
    if (dev != NULL)
    {
        iAvailable = dev->CAN_RX_MSG;
    }
    
    return iAvailable;
}


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
int8_t can_abort(ft900_can_regs_t *dev)
{
    int8_t iRet = -1;
        
    /* Check the number given is an actual device... */
    if (dev != NULL)
    {
        iRet = 0;
        dev->CAN_CMD |= MASK_CAN_CMD_ABORT_TX;
    }
    
    return iRet;
}


/** @brief Enable an Interrupt
 *
 *  Enable the CAN device to generate an interrupt.  The value of mask is a bit-mask with the
 *  following format:
 *
 *  | Bit     | Name      | Description                 | Trigger                                          |
 *  | ------- | :-------: | :---------------------------| :----------------------------------------------- |
 *  | 7 (MSB) | DVOLD     | Data Overload Interrupt     | overload condn occurs due to the internal        |
 *  |		  |			  |                             |   conditions of the receiver (FT900C onwards ONLY)|
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
int8_t can_enable_interrupt(ft900_can_regs_t *dev, can_interrupt_t interrupt)
{
    int8_t iRet = 0;
        
    /* Check the number given is an actual device... */
    if (dev == NULL)
    {
        iRet = -1;
    }
    
    if (iRet == 0)
    {
        switch(interrupt)
        {
            case can_interrupt_data_overrun: dev->CAN_INT_ENABLE |= MASK_CAN_INT_ENABLE_DATA_OVRN_EN; break;
            case can_interrupt_bus_error: dev->CAN_INT_ENABLE |= MASK_CAN_INT_ENABLE_BUS_ERR_EN; break;
            case can_interrupt_transmit: dev->CAN_INT_ENABLE |= MASK_CAN_INT_ENABLE_TX_EN; break;
            case can_interrupt_receive: dev->CAN_INT_ENABLE |= MASK_CAN_INT_ENABLE_RX_EN; break;
            case can_interrupt_error_passive: dev->CAN_INT_ENABLE |= MASK_CAN_INT_ENABLE_ERR_PSV_EN; break;
            case can_interrupt_error_warning: dev->CAN_INT_ENABLE |= MASK_CAN_INT_ENABLE_ERR_WRNG_EN; break;
            case can_interrupt_arbitration_lost: dev->CAN_INT_ENABLE |= MASK_CAN_INT_ENABLE_ARB_LOST_EN; break;
            case can_interrupt_data_overload:
                	if (sys_check_ft900_revB()) //90x series rev B
                	{
                		iRet = -1;  /* Interrupt status not available for rev B */
                	}
                	else
                	{
                		dev->CAN_INT_ENABLE |= MASK_CAN_INT_ENABLE_DOVLD_EN;
                	}
                	break;
            default: iRet = -1; break;
        }
    }
    
    return iRet;
}


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
int8_t can_disable_interrupt(ft900_can_regs_t *dev, can_interrupt_t interrupt)
{
    int8_t iRet = 0;
        
    /* Check the number given is an actual device... */
    if (dev == NULL)
    {
        iRet = -1;
    }
    
    if (iRet == 0)
    {
        switch(interrupt)
        {
            case can_interrupt_data_overrun: dev->CAN_INT_ENABLE &= ~MASK_CAN_INT_ENABLE_DATA_OVRN_EN; break;
            case can_interrupt_bus_error: dev->CAN_INT_ENABLE &= ~MASK_CAN_INT_ENABLE_BUS_ERR_EN; break;
            case can_interrupt_transmit: dev->CAN_INT_ENABLE &= ~MASK_CAN_INT_ENABLE_TX_EN; break;
            case can_interrupt_receive: dev->CAN_INT_ENABLE &= ~MASK_CAN_INT_ENABLE_RX_EN; break;
            case can_interrupt_error_passive: dev->CAN_INT_ENABLE &= ~MASK_CAN_INT_ENABLE_ERR_PSV_EN; break;
            case can_interrupt_error_warning: dev->CAN_INT_ENABLE &= ~MASK_CAN_INT_ENABLE_ERR_WRNG_EN; break;
            case can_interrupt_arbitration_lost: dev->CAN_INT_ENABLE &= ~MASK_CAN_INT_ENABLE_ARB_LOST_EN; break;
            case can_interrupt_data_overload:
                	if (sys_check_ft900_revB()) //90x series rev B
                	{
                		iRet = -1;  /* Interrupt status not available for rev B */
                	}
                	else
                	{
                		dev->CAN_INT_ENABLE &= ~MASK_CAN_INT_ENABLE_DOVLD_EN;
                	}
                	break;
            default: iRet = -1; break;
        }
    }
    
    return iRet;
}


/** @brief Query the Interrupt register
 *
 *  Query the Interrupt register in order to determine what caused the interrupt.
 *
 *  @param dev A pointer to the device to use
 *  @param interrupt The interrupt to check
 *
 *  @warning This function clear the interrupt bit so that it does not fire constantly
 *
 *  @returns 0 when the interrupt hasn't been fired, 1 when the interrupt has fired and -1 otherwise
 */
int8_t can_is_interrupted(ft900_can_regs_t *dev, can_interrupt_t interrupt)
{
    int8_t iRet = -1;
        
    /* Check the number given is an actual device... */
    if (dev != NULL)
    {
        switch(interrupt)
        {
            case can_interrupt_data_overrun:
                {
                    if (dev->CAN_INT_STATUS & MASK_CAN_INT_STATUS_DATA_OVRN)
                    {
                        iRet = 1;
                        dev->CAN_INT_STATUS = MASK_CAN_INT_STATUS_DATA_OVRN;
                    }
                    else
                    {
                        iRet = 0;
                    }
                }
                break;

            case can_interrupt_bus_error:
                {
                    if (dev->CAN_INT_STATUS & MASK_CAN_INT_STATUS_BUS_ERR)
                    {
                        iRet = 1;
                        dev->CAN_INT_STATUS = MASK_CAN_INT_STATUS_BUS_ERR;
                    }
                    else
                    {
                        iRet = 0;
                    }
                }
                break;

            case can_interrupt_transmit:
                {
                    if (dev->CAN_INT_STATUS & MASK_CAN_INT_STATUS_TX)
                    {
                        iRet = 1;
                        dev->CAN_INT_STATUS = MASK_CAN_INT_STATUS_TX;
                    }
                    else
                    {
                        iRet = 0;
                    }
                }
                break;

            case can_interrupt_receive:
                {
                    if (dev->CAN_INT_STATUS & MASK_CAN_INT_STATUS_RX)
                    {
                        iRet = 1;
                        dev->CAN_INT_STATUS = MASK_CAN_INT_STATUS_RX;
                    }
                    else
                    {
                        iRet = 0;
                    }
                }
                break;

            case can_interrupt_error_passive:
                {
                    if (dev->CAN_INT_STATUS & MASK_CAN_INT_STATUS_ERR_PSV)
                    {
                        iRet = 1;
                        dev->CAN_INT_STATUS = MASK_CAN_INT_STATUS_ERR_PSV;
                    }
                    else
                    {
                        iRet = 0;
                    }
                }
                break;

            case can_interrupt_error_warning:
                {
                    if (dev->CAN_INT_STATUS & MASK_CAN_INT_STATUS_ERR_WRN)
                    {
                        iRet = 1;
                        dev->CAN_INT_STATUS = MASK_CAN_INT_STATUS_ERR_WRN;
                    }
                    else
                    {
                        iRet = 0;
                    }
                }
                break;

            case can_interrupt_arbitration_lost:
                {
                    if (dev->CAN_INT_STATUS & MASK_CAN_INT_STATUS_ARB_LOST)
                    {
                        iRet = 1;
                        dev->CAN_INT_STATUS = MASK_CAN_INT_STATUS_ARB_LOST;
                    }
                    else
                    {
                        iRet = 0;
                    }
                }
                break;

            case can_interrupt_data_overload:
                {
                	if (sys_check_ft900_revB()) //90x series rev B
                	{
                		iRet = -1;  /* Interrupt status not available for rev B */
                	}
                	else
                	{
						if (dev->CAN_INT_STATUS & MASK_CAN_INT_STATUS_DOVLD)
						{
							iRet = 1;
							dev->CAN_INT_STATUS = MASK_CAN_INT_STATUS_DOVLD;
						}
						else
						{
							iRet = 0;
						}
                	}
                }
                break;

            default: iRet = -1; break;
        }
    }
    
    return iRet;
}


/** @brief Query the status register
 *
 *  The return value is a bit-mask with the following format:
 *
 *  | Bit     | Name        | Description                 | Set when...                                   |
 *  | ------- | :---------: | :-------------------------- | :-------------------------------------------- |
 *  | 7 (MSB) | RX_BUF_STS  | Receive Buffer Status       | At least one message in the receive FIFO      |
 *  | 6       | OVRN_STS    | Data Overrun Status         | The receive FIFO has encountered an overrun   |
 *  | 5       | TX_BUF_STS  | Transmit Buffer Status      | The CPU is able to write to the transmit FIFO |
 *  | 4       | DOVLD_STS   | Data Overload status        | overload condn occurs due to the internal     |
 *  |		  |			    |                             |   conditions of the receiver (FT900C onwards ONLY)|
 *  | 3       | RX_STS      | Receive Status              | CAN is currently receiving a message          |
 *  | 2       | TX_STS      | Transmit Status             | CAN is currently transmitting a message       |
 *  | 1       | ERR_STS     | Error Status                | One CAN error counter has reached warning (96)|
 *  | 0 (LSB) | BUS_OFF_STS | Bus Off Status              | The CAN device is in a bus off state          |
 *
 *  @param dev A pointer to the device to use
 *
 *  @returns A bit-mask of the current status or 0 for an unknown device.
 */
uint8_t can_status(ft900_can_regs_t *dev)
{
    uint8_t iStatus = 0;
        
    /* Check the number given is an actual device... */
    if (dev != NULL)
    {
        iStatus = dev->CAN_STATUS;
    }
    
    return iStatus;
}


/** @brief Get the current number of receive errors reported by the CAN device
 *  @param dev A pointer to the device to use
 *  @returns The current number of receive errors (0 - 255) or 0 for an unknown device.
 */
uint8_t can_rx_error_count(ft900_can_regs_t *dev)
{
    uint8_t iRxErrCount = 0;
        
    /* Check the number given is an actual device... */
    if (dev != NULL)
    {
        iRxErrCount = dev->CAN_RX_ERR_CNTR;
    }
    
    return iRxErrCount;
}


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
uint8_t can_tx_error_count(ft900_can_regs_t *dev)
{
    uint8_t iTxErrCount = 0;
        
    /* Check the number given is an actual device... */
    if (dev != NULL)
    {
        iTxErrCount = dev->CAN_TX_ERR_CNTR;
    }
    
    return iTxErrCount;
}


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
uint8_t can_ecode(ft900_can_regs_t *dev)
{
    uint8_t iECode = 0;
        
    /* Check the number given is an actual device... */
    if (dev != NULL)
    {
        iECode = dev->CAN_ERR_CODE;
    }
    
    return iECode;
}


/** @brief Get the location where arbitration was lost.
 *  @param dev A pointer to the device to use
 *  @returns The location where arbitration was lost
 */
can_arbitration_lost_t can_arbitration_lost_location(ft900_can_regs_t *dev)
{
    can_arbitration_lost_t tArbitration = can_arbitration_lost_invalid;
        
    /* Check the number given is an actual device... */
    if (dev != NULL)
    {
        /* Cast the contents of ALC to the enum, since there is a one to one mapping and this 
           is cheap to do in C */
        tArbitration = (can_arbitration_lost_t) dev->CAN_ARB_LOST_CODE;
    }
    
    return tArbitration;    
}


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
int8_t can_filter(ft900_can_regs_t *dev, can_filter_mode_t filtmode, uint8_t filternum, const can_filter_t *filter)
{
    int8_t iRet = 0;
    can_filter_mode_t tFilterMode = can_filter_mode_single;
        
    /* Check the number given is an actual device... */
   if (dev == NULL)
    {
        iRet = -1;
    }
    
    if (!can_is_reset(dev))
    {
    	/* The device is opened, cannot change anything */
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (filtmode < can_filter_mode_single ||
            filtmode > can_filter_mode_dual)
        {
            /* Out of range filtmode value */
            iRet = -1;
        }

        if (filternum > 1)
        {
            /* filternum is out of range */
            iRet = -1;
        }
        
        if (filter == NULL)
        {
            /* We were passed a null pointer */
            iRet = -1;
        }
    }
    
    if (iRet == 0)
    {

        if (filtmode == can_filter_mode_single)
        {
            dev->CAN_MODE |= MASK_CAN_MODE_ACC_FLTR; /* Set single filter mode */
        }
        else
        {
            dev->CAN_MODE &= ~MASK_CAN_MODE_ACC_FLTR; /* Set dual filter mode */
        }

        /* Check what mode we are in and check that the filter number we've been given is ok... */
        if (dev->CAN_MODE & MASK_CAN_MODE_ACC_FLTR)
        {
            tFilterMode = can_filter_mode_single;
            if (filternum == 1)
            {
                /* The second filter is not available in single filter mode (duh!) */
                iRet = -1;
            }
        }
        else
        {
            tFilterMode = can_filter_mode_dual;
        }
    }
    
    if (iRet == 0)
    {
        if (tFilterMode == can_filter_mode_single)
        {
            if (filter->type == can_type_standard)
            {
                /* SINGLE MODE, STANDARD ID MESSAGE */
                /* Set up the ID and RTR Tag */
                dev->CAN_ACC_CODE_0 = ((filter->id & CAN_U32_MASK_3_TO_10) >> 3);
                dev->CAN_ACC_CODE_1 = ((filter->id & CAN_U32_MASK_0_TO_2) << 5) |
                             ((filter->rtr == can_rtr_remote_request) ? 0x01 : 0x00) << 4;
                /* Set up the ID and RTR Mask */
                dev->CAN_ACC_ENABLE_0 = ((filter->idmask & CAN_U32_MASK_3_TO_10) >> 3);
                dev->CAN_ACC_ENABLE_1 = ((filter->idmask & CAN_U32_MASK_0_TO_2) << 5) |
                             (filter->rtrmask << 4) |
                             0x0F; /* Added to make sure that unused bits are Dont Care */
                /* Set up the DATA Tag */
                dev->CAN_ACC_CODE_2 = filter->data0;
                dev->CAN_ACC_CODE_3 = filter->data1;
                /* Set up the DATA Mask */
                dev->CAN_ACC_ENABLE_2 = filter->data0mask;
                dev->CAN_ACC_ENABLE_3 = filter->data1mask;
            }
            else
            {
                /* SINGLE MODE, EXTENDED ID MESSAGE */
                /* Set up the ID and RTR Tag */
                dev->CAN_ACC_CODE_0 = ((filter->id & CAN_U32_MASK_21_TO_28) >> 21);
                dev->CAN_ACC_CODE_1 = ((filter->id & CAN_U32_MASK_13_TO_20) >> 13);
                dev->CAN_ACC_CODE_2 = ((filter->id & CAN_U32_MASK_5_TO_12) >> 5);
                dev->CAN_ACC_CODE_3 = ((filter->id & CAN_U32_MASK_0_TO_4) << 3) |
                             ((filter->rtr == can_rtr_remote_request) ? 0x01 : 0x00) << 2;
                /* Set up the ID and RTR Mask */
                dev->CAN_ACC_ENABLE_0 = ((filter->idmask & CAN_U32_MASK_21_TO_28) >> 21);
                dev->CAN_ACC_ENABLE_1 = ((filter->idmask & CAN_U32_MASK_13_TO_20) >> 13);
                dev->CAN_ACC_ENABLE_2 = ((filter->idmask & CAN_U32_MASK_5_TO_12) >> 5);
                dev->CAN_ACC_ENABLE_3 = ((filter->idmask & CAN_U32_MASK_0_TO_4) << 3) |
                             (filter->rtrmask << 2) |
                             0x03; /* Added to make sure that unused bits are Dont Care */
                
            }
        }
        else /* can_filter_mode_dual */
        {
            if (filter->type == can_type_standard)
            {
                if (filternum == 0)
                {
                    /* DUAL MODE, STANDARD ID MESSAGE */
                    /* Set up the ID and RTR Tag */
                    dev->CAN_ACC_CODE_0 = ((filter->id & CAN_U32_MASK_3_TO_10) >> 3);
                    dev->CAN_ACC_CODE_1 = ((filter->id & CAN_U32_MASK_0_TO_2) << 5) |
                                 ((filter->rtr == can_rtr_remote_request) ? 0x01 : 0x00) << 4;
                    /* Set up the ID and RTR Mask */
                    dev->CAN_ACC_ENABLE_0 = ((filter->idmask & CAN_U32_MASK_3_TO_10) >> 3);
                    dev->CAN_ACC_ENABLE_1 = ((filter->idmask & CAN_U32_MASK_0_TO_2) << 5) |
                                 (filter->rtrmask << 4);
                    /* Set up the DATA Tag */
                    dev->CAN_ACC_CODE_1 |= (filter->data0 & 0xF0) >> 4;
                    dev->CAN_ACC_CODE_3 = (dev->CAN_ACC_CODE_3 & 0xF0) | (filter->data0 & 0x0F);
                    /* Set up the DATA Mask */
                    dev->CAN_ACC_ENABLE_1 |= (filter->data0mask & 0xF0) >> 4;
                    dev->CAN_ACC_ENABLE_3 = (dev->CAN_ACC_ENABLE_3 & 0xF0) | (filter->data0mask & 0x0F);
                }
                else /* filternum == 1 */
                {
                    /* DUAL MODE, STANDARD ID MESSAGE */
                    /* Set up the ID and RTR Tag */
                    dev->CAN_ACC_CODE_2 = ((filter->id & CAN_U32_MASK_3_TO_10) >> 3);
                    dev->CAN_ACC_CODE_3 = (dev->CAN_ACC_CODE_3 & 0x0F) |
                                 ((filter->id & CAN_U32_MASK_0_TO_2) << 5) |
                                 ((filter->rtr == can_rtr_remote_request) ? 0x01 : 0x00) << 4;
                    /* Set up the ID and RTR Mask */
                    dev->CAN_ACC_ENABLE_2 = ((filter->idmask & CAN_U32_MASK_3_TO_10) >> 3);
                    dev->CAN_ACC_ENABLE_3 = (dev->CAN_ACC_ENABLE_3 & 0x0F) |
                                 ((filter->idmask & CAN_U32_MASK_0_TO_2) << 5) |
                                 (filter->rtrmask << 4);
                }
            }
            else /* can_type_extended */
            {
                if (filternum == 0)
                {
                    /* DUAL MODE, EXTENDED ID MESSAGE */
                    /* Set up the ID and RTR Tag */
                    dev->CAN_ACC_CODE_0 = ((filter->id & CAN_U32_MASK_21_TO_28) >> 21);
                    dev->CAN_ACC_CODE_1 = ((filter->id & CAN_U32_MASK_13_TO_20) >> 13);
                    /* Set up the ID and RTR Mask */
                    dev->CAN_ACC_ENABLE_0 = ((filter->idmask & CAN_U32_MASK_21_TO_28) >> 21);
                    dev->CAN_ACC_ENABLE_1 = ((filter->idmask & CAN_U32_MASK_13_TO_20) >> 13);
                }
                else /* filternum == 1 */
                {
                    /* DUAL MODE, EXTENDED ID MESSAGE */
                    /* Set up the ID and RTR Tag */
                    dev->CAN_ACC_CODE_2 = ((filter->id & CAN_U32_MASK_21_TO_28) >> 21);
                    dev->CAN_ACC_CODE_3 = ((filter->id & CAN_U32_MASK_13_TO_20) >> 13);
                    /* Set up the ID and RTR Mask */
                    dev->CAN_ACC_ENABLE_2 = ((filter->idmask & CAN_U32_MASK_21_TO_28) >> 21);
                    dev->CAN_ACC_ENABLE_3 = ((filter->idmask & CAN_U32_MASK_13_TO_20) >> 13);
                }
            }
        }
    }
    
    return iRet;
}

