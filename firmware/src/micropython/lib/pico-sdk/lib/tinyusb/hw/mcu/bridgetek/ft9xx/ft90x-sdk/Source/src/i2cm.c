/**
    @file

    @brief
    I2C Master

    
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

/* TODO: Clean this up */

/* INCLUDES ************************************************************************/
#include <ft900.h>

/* CONSTANTS ***********************************************************************/
/* I2C #defines */
#define I2C_STATUS_IDLE         0x20
#define I2C_STATUS_ADDRACK      0x04
#define I2C_STATUS_ERROR        0x02
#define I2C_STATUS_BUSY         0x01
#define I2C_FLAGS_RUN           0x01
#define I2C_FLAGS_START         0x02
#define I2C_FLAGS_STOP          0x04
#define I2C_FLAGS_ACK           0x08
#define I2C_FLAGS_HS            0x10
#define I2C_FLAGS_ADDR          0x20
#define I2C_FLAGS_SLRST         0x40
#define I2C_FLAGS_RSTB          0x80
#define I2C_READ_NOT_WRITE      0x01
#define I2C_TIMER_HS_SELECT     0x80
#define I2C_TIMER_PERIOD_MASK   0x7F

/* I2CM SCL_LP, SCL_HP definitions */
/* SCL Period constant Normal speed mode */
#define I2CM_PERIOD_CONST_STD 					4
/* SCL Period constant for other modes */
#define I2CM_PERIOD_CONST_FAST_FASTPLUS_HS 		3

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/
uint8_t isHighSpeed;		/**< Variable to indicate if high speed mode is enabled */

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static uint8_t i2c_wait_for(void)
{
	// Noops added because i2cm driver was failing on -02 optimization (works with -O0)
  // It takes some time for the BUSY bit to be set. The NOOPs delay the wait-till-not-busy check 
  // to until the BUSY bit is set.
	asm_noop();
	asm_noop();
	asm_noop();
	
 	while (I2CM->I2CM_CNTL_STATUS & I2C_STATUS_BUSY)
        ; /* Wait for the bus to be idle */

    return (I2CM->I2CM_CNTL_STATUS & I2C_STATUS_ERROR);
}

/* MACROS **************************************************************************/

/* FUNCTIONS ***********************************************************************/
void i2cm_init(I2CM_speed_mode mode, uint32_t i2c_clk_speed)
{
	uint32_t TPVal, lTPVal, period_constant;
	
	/* Reset I2CM first */
	I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_RSTB;

	isHighSpeed = 0;	/* high speed mode is not enabled by default */

	/* Set I2CM clock frequency by writing to I2CM_TIMER_PERIOD – Timer Period Register (address offset: 0x03).
	 * Frequency scaler (TIMER_PRD)is used in Standard/Fast/Fast-plus modes to calculate the I2C clock period (SCL_PRD).
	 *
	 * In Standard speed mode:
	 * SCL_PRD = (TIMER_PRD * 2 + 1) * 4 * CLK_PERIOD
	 *
	 * In Fast/Fast-plus speed mode:
	 * SCL_PRD = (TIMER_PRD * 2 + 1) * 3 * CLK_PERIOD
	 *
	 * High-speed mode:
	 * SCL_PRD = (TIMER_PRD * 2 + 1) * 3 * CLK_PERIOD
	 *
	 * where CLK_PERIOD is 10 ns, based on the 100-MHz system clock.
     */
	if (mode == I2CM_NORMAL_SPEED)
	{
		period_constant = I2CM_PERIOD_CONST_STD;
	}
	else
	{
		period_constant = I2CM_PERIOD_CONST_FAST_FASTPLUS_HS;
		if (mode == I2CM_HIGH_SPEED)/* I2CM_HIGH_SPEED */
		{
			isHighSpeed = 1;
		}
	}

    TPVal = ((sys_get_cpu_clock() / (i2c_clk_speed * period_constant)) - 1) >> 1;
	/* In high speed mode, another TP is used and the MSB of TPVal has to be 1 */
	if (mode == I2CM_HIGH_SPEED)
	{
		//Programming standard speed for master code transmission
		//lTPVal = ((sys_get_cpu_clock() / (I2CM_100_Kbps * I2CM_PERIOD_CONST_STD)) - 1) >> 1;
		lTPVal = ((sys_get_cpu_clock() / (I2CM_400_Kbps * I2CM_PERIOD_CONST_FAST_FASTPLUS_HS)) - 1) >> 1;
		lTPVal &= MASK_I2CM_TIME_PERIOD_STD_FAST_FPLUS;

		/* Then write timer value to the Normal register with the MSB set to 0 for standard/fast speed */
	    I2CM->I2CM_TIME_PERIOD = (uint8_t)lTPVal;
	    //tfp_printf("l:%02x\n",lTPVal);
		//Programming High speed for data transmission
		/* To write to the High-speed register, the MSB must be 1. FAST bit is cleared. Timer value is written*/
		TPVal &= MASK_I2CM_TIME_PERIOD_HIGHSPEED;
		//TPVal |= MASK_I2CM_TIME_PERIOD_HS_SELECT;
		TPVal |= MASK_I2CM_TIME_PERIOD_HS_SELECT | MASK_I2CM_TIME_PERIOD_FS_SELECT;

		I2CM->I2CM_TIME_PERIOD = TPVal;
		//tfp_printf("h:%02x\n",TPVal);
	}
	else
	{
		/* In normal & fast speed modes, the MSB of TPVal has to be 0 */
		TPVal &= MASK_I2CM_TIME_PERIOD_STD_FAST_FPLUS;
		if ((mode == I2CM_FAST_SPEED) || (mode == I2CM_FAST_PLUS_SPEED))
		{
			/* To write to the High-speed register, the MSB must be 1. You should write to it first to set the FAST bit. */
			I2CM->I2CM_TIME_PERIOD = MASK_I2CM_TIME_PERIOD_HS_SELECT | MASK_I2CM_TIME_PERIOD_FS_SELECT;
		}
		/* Then write timer value to the Normal register with the MSB set to 1 or 0 depending on high speed or not */
	    I2CM->I2CM_TIME_PERIOD = (uint8_t)TPVal;
	    //tfp_printf("l:%02x\n",TPVal);
	}

	return;
}

int8_t i2cm_write(const uint8_t addr, const uint8_t cmd,
        const uint8_t *data, uint16_t number_to_write)
{
    int8_t ret = 0;

	if (isHighSpeed)
	{
		/* In high speed mode, the following sequence has to be done before each data transfer
		   Write Master code 0x09 to I2CMSA and the HIGH SPEED command to I2CMCR */
	    I2CM->I2CM_SLV_ADDR = 0x09;
        I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_HS | I2C_FLAGS_RUN;

		/* Wait until I2CM is no longer busy */
		if (i2c_wait_for())	{ /* Ignore ACK Error in case of master code  ret = -1; */}
	}
	
	if (ret == 0)
	{
	    /* Write slave address to SA */
	    I2CM->I2CM_SLV_ADDR = addr;
	    /* Write command byte address to BUF */
	    I2CM->I2CM_DATA = cmd;

	    /* Check that the bus is not occupied by another master
	       (this step is not needed in single-master system or in high speed mode since
	       arbitration takes place during transmission of the master code) */
	    if (isHighSpeed == 0)
	    {
	        while (I2CM->I2CM_CNTL_STATUS & MASK_I2CM_STATUS_BUS_BUSY)
	        {
	        }
	    }

	    /* Write command to I2CMCR to send Start and command byte. */
	    I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_START | I2C_FLAGS_RUN;
	    if (i2c_wait_for()) { /* An Error Occurred */ ret = -1; }
	}

	if (ret == 0)
	{
	    if (number_to_write)
	    {
	        while (number_to_write-- && ret == 0)
	        {
	            /* Write command to I2CMCR to send data byte. */
	            I2CM->I2CM_DATA = *data++;
	            I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_RUN;
	            if (i2c_wait_for()) { /* An Error Occurred */ ret = -1; }
	        }
	    }
	}

	/* Write command to I2CMCR to send Stop. */
    I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_STOP;
	if (ret == 0)
	{
        i2c_wait_for();
	}
	
	return ret;
}

int8_t i2cm_read(const uint8_t addr, const uint8_t cmd,
                uint8_t *data, uint16_t number_to_read)
{
    int8_t ret = 0;

	if (isHighSpeed)
	{
		/* In high speed mode, the following sequence has to be done before each data transfer
		   Write Master code 0x09 to I2CMSA and the HIGH SPEED command to I2CMCR */
		I2CM->I2CM_SLV_ADDR = 0x09;
		I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_HS | I2C_FLAGS_RUN;

		/* Wait until I2CM is no longer busy */
		if (i2c_wait_for())	
		{ /* Ignore ACK Error in case of master code  ret = -1; */}
	}

	if (ret == 0)
	{
	    /* Write slave address to SA */
	    I2CM->I2CM_SLV_ADDR = addr;
	    /* Write command byte address to BUF */
	    I2CM->I2CM_DATA = cmd;

	    /* Check that the bus is not occupied by another master
	       (this step is not needed in single-master system or in high speed mode since
	       arbitration takes place during transmission of the master code) */
	    if (isHighSpeed == 0)
	    {
	        while (I2CM->I2CM_CNTL_STATUS & MASK_I2CM_STATUS_BUS_BUSY)
	        {
	        }
	    }

	    /* Write command to I2CMCR to send Start and command byte. */
	    I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_START | I2C_FLAGS_RUN;

	    if (i2c_wait_for()) 
		{ 
			ret = -1; 
		}
	}

	if (ret == 0)
	{
	    /* Write slave address to SA with R operation */
	    I2CM->I2CM_SLV_ADDR = addr | 0x01;

        if (number_to_read <= 1)
		{
            /* Receive with a NACK */
            I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_START | I2C_FLAGS_RUN;
		}
        else
        {
			/* Receive with an ACK */
            I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_START | I2C_FLAGS_RUN | I2C_FLAGS_ACK;
		}
	    if (i2c_wait_for()) 
		{ 
			ret = -1; 
		}
	}

	if (ret == 0)
	{
        while (number_to_read)
        {
            *data++ = I2CM->I2CM_DATA;
            number_to_read--;
            
            if (number_to_read)
            {
                if (number_to_read == 1)
                    /* Last one, Receive with a NACK */
                    I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_RUN;
                else
                    /* Receive with an ACK */
                    I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_RUN | I2C_FLAGS_ACK;
            
                /* Write command to I2CMCR to read data byte.
                   I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_RUN; */
                if (i2c_wait_for())
                {
                    ret = -1;
                    break;
                }
            }
        }
	}

    /* Write command to I2CMCR to send Stop. */
    I2CM->I2CM_CNTL_STATUS = I2C_FLAGS_STOP;
	if (ret == 0)
	{
	    i2c_wait_for();
	}

	return ret;
}

uint8_t i2cm_get_status(void)
{
    return I2CM->I2CM_CNTL_STATUS;
}

int8_t i2cm_interrupt_enable(uint8_t mask)
{
    I2CM->I2CM_FIFO_INT_ENABLE |= mask;
    return 0;
}

int8_t i2cm_interrupt_disable(uint8_t mask)
{
    I2CM->I2CM_FIFO_INT_ENABLE &= ~mask;
    return 0;
}

int8_t i2cm_is_interrupted(uint8_t mask)
{
    int8_t ret = 0;
    if (I2CM->I2CM_FIFO_INT_PEND & mask)
    {
        ret = 1;
        I2CM->I2CM_FIFO_INT_ENABLE = mask;
    }
    return ret;
}
