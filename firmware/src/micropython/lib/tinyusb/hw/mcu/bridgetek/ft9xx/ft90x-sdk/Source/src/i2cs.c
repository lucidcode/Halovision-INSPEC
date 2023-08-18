/**
    @file

    @brief
    I2C Slave

    
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
#include <ft900_i2cs.h>
#include <ft900_asm.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/
void i2cs_init(uint8_t addr)
{
	/* Reset I2CS first */
	I2CS->I2CS_CNTL_STATUS = MASK_I2CS_CNTL_I2C_RST;

    /* Slave address */
    I2CS->I2CS_OWN_ADDR = addr >> 1;

	I2CS->I2CS_CNTL_STATUS = MASK_I2CS_CNTL_DEV_ACTV;

	I2CS->I2CS_FIFO_INT_PEND = 0xFF; /* Clear all pending interrupts */
}

int8_t i2cs_read(uint8_t *data, size_t size)
{
	asm_streamin8(&(I2CS->I2CS_DATA), data, size);
    return 0;
}

int8_t i2cs_write(const uint8_t *data, size_t size)
{
    asm_streamout8(data, &(I2CS->I2CS_DATA), size);
    return 0;
}

uint8_t i2cs_get_status(void)
{
    uint8_t status = I2CS->I2CS_CNTL_STATUS;

    if (status & MASK_I2CS_STATUS_REC_FIN)
        I2CS->I2CS_CNTL_STATUS |= MASK_I2CS_CNTL_REC_FIN_CLR;
    if (status & MASK_I2CS_STATUS_SEND_FIN)
        I2CS->I2CS_CNTL_STATUS |= MASK_I2CS_CNTL_SEND_FIN_CLR;

    return status;
}


int8_t i2cs_enable_interrupt(uint8_t mask)
{
    I2CS->I2CS_FIFO_INT_ENABLE |= mask;
    return 0;
}


int8_t i2cs_disable_interrupt(uint8_t mask)
{
    I2CS->I2CS_FIFO_INT_ENABLE &= ~mask;
    return 0;
}


int8_t i2cs_is_interrupted (uint8_t mask)
{
    int8_t ret = 0;

    if ((I2CS->I2CS_FIFO_INT_PEND & mask) == mask)
    {
        I2CS->I2CS_FIFO_INT_PEND |= mask;
        ret = 1;
    }

    return ret;
}

