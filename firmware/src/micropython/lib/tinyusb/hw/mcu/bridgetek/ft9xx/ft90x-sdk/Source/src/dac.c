/**
    @file

    @brief
    Digital to Analogue Converter

    
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
#include <ft900_dac.h>
#include <ft900_asm.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/
static inline int8_t dac_is_valid(uint8_t num)
{
    return (num < 2);
}

/* FUNCTIONS ***********************************************************************/
/** @brief Select the mode of the DAC
 *  @param num The DAC to use
 *  @param mode The mode the DAC should be in (single or continuous)
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_mode(uint8_t num, dac_mode_t mode)
{
    int8_t iRet = 0;

    if (!dac_is_valid(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (num == 0)
        {
            if (mode == dac_mode_single)
            {
                ADCDAC->DAC_CONF &= ~MASK_DAC_CONF_DAC_CONT0;
            }
            else
            {
                ADCDAC->DAC_CONF |= MASK_DAC_CONF_DAC_CONT0;
            }
        }
        else if (num == 1)
        {
            if (mode == dac_mode_single)
            {
                ADCDAC->DAC_CONF &= ~MASK_DAC_CONF_DAC_CONT1;
            }
            else
            {
                ADCDAC->DAC_CONF |= MASK_DAC_CONF_DAC_CONT1;
            }
        }
        else
        {
        }
    }

    return iRet;
}


/** @brief Select the divider for the DAC conversion rate
 *
 *  \f$ f_DAC = \frac{f_peripheral}{DAC_DIVIDER + 1} \f$
 *
 *  @param div The divider
 *  @warning The maximum conversion rate is 1 MHz
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_divider(uint8_t div)
{
    int8_t iRet = 0;

    if (div > 127)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        ADCDAC->DAC_ADC_CNT &= ~MASK_DAC_ADC_CNT_DAC_DIVIDER;
        ADCDAC->DAC_ADC_CNT |= (div << BIT_DAC_ADC_CNT_DAC_DIVIDER);
    }

    return iRet;
}


/** @brief See how many samples are still being converted
 *  @param num The DAC to use
 *	@warning This API returns 0xff in case FIFO is full or empty. User need to check
 *			 the interrupt pending bit to conclude 0 samples in FIFO or 128 samples in FIFO.
 *		     This function should be called in the I2S interrupt context in case of FT900 Revision B
 *		     for a reliable count value. For more info, please refer to "TN_159 FT90x Errata Technical Note".
 *  @returns The number of samples still to be converted, or 0 otherwise
 */
uint8_t dac_available(uint8_t num)
{
    uint8_t iRet = 0;

    if (dac_is_valid(num))
    {
        if (num == 0)
        {
            iRet = (ADCDAC->DAC_ADC_CNT & MASK_DAC_ADC_CNT_DAC_DATA_COUNT0) >> BIT_DAC_ADC_CNT_DAC_DATA_COUNT0;
        }
        else
        {
            iRet = (ADCDAC->DAC_ADC_CNT & MASK_DAC_ADC_CNT_DAC_DATA_COUNT1) >> BIT_DAC_ADC_CNT_DAC_DATA_COUNT1;
        }

        /* Workaround for the number of samples when the count is more than 125 samples */
        /* 0xFE will convert to 0x7E, after increment will change to 0x7F(127 samples) */
        if (iRet)
        {
        	iRet &= 0x7F;
        	iRet++;
        }
        else
        {
        	/* 0 samples here */
        	/* Error case where fifo count is either 128 or 0 */
        	/* interrupt pending bit 1 indicates 0 samples */
        	/* interrupt pending bit 0 indicates 128 samples */
        	if (0 == num)
        	{
        		if (!(ADCDAC->IRQ_STATUS_ERR & MASK_IRQ_STATUS_ERR_DAC_IRQ_PEND0))
        		{
            		/* if interrupt pending flag is not set then it is full */
        			iRet = 0x80;
        		}
        	}
        	else /* 1 == num */
        	{
        		if (!(ADCDAC->IRQ_STATUS_ERR & MASK_IRQ_STATUS_ERR_DAC_IRQ_PEND1))
        		{
            		/* if interrupt pending flag is not set then it is full */
        			iRet = 0x80;
        		}
        	}
        }
    }

    return iRet;
}


/** @brief Start the DAC
 *  @param num The DAC to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_start(uint8_t num)
{
    int8_t iRet = 0;

    if (!dac_is_valid(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (num == 0)
        {
            if (!(ADCDAC->DAC_CONF & MASK_DAC_CONF_DAC_START0))
                ADCDAC->DAC_CONF |= MASK_DAC_CONF_DAC_START0;
            else
                iRet = -1; /* Already started */
        }
        else if (num == 1)
        {
            if (!(ADCDAC->DAC_CONF & MASK_DAC_CONF_DAC_START1))
                ADCDAC->DAC_CONF |= MASK_DAC_CONF_DAC_START1;
            else
                iRet = -1; /* Already started */
        }
    }

    return iRet;
}


/** @brief Stop the DAC
 *  @param num The DAC to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_stop(uint8_t num)
{
    int8_t iRet = 0;

    if (!dac_is_valid(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (num == 0)
        {
            ADCDAC->DAC_CONF &= ~MASK_DAC_CONF_DAC_START0;
        }
        else if (num == 1)
        {
            ADCDAC->DAC_CONF &= ~MASK_DAC_CONF_DAC_START1;
        }
        else
        {
        }
    }

    return iRet;
}


/** @brief Disable the threshold interrupt
 *
 *  This function will automatically update the DAC when it is in single mode.
 *
 *  @param num The DAC to use
 *  @param data The sample to write
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_write(uint8_t num, uint16_t data)
{
    int8_t iRet = 0;

    if (!dac_is_valid(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (num == 0)
        {
            /*ADCDAC->DAC_ADC_DATA |= (data << BIT_DAC_ADC_DATA_DAC_DATA0) & MASK_DAC_ADC_DATA_DAC_DATA0;*/
            ADCDAC->DAC_DATA0_ADC_DATA = data & 0x3FF;
            /* Start the DAC again if a new value appears */
            if (!(ADCDAC->DAC_CONF & MASK_DAC_CONF_DAC_CONT0))
            {
                ADCDAC->DAC_CONF |= MASK_DAC_CONF_DAC_START0;
            }
            iRet = 1;
        }
        else if (num == 1)
        {
            /*ADCDAC->DAC_ADC_DATA |= (data << BIT_DAC_ADC_DATA_DAC_DATA1) & MASK_DAC_ADC_DATA_DAC_DATA1;*/
            ADCDAC->DAC_DATA1 = data & 0x3FF;
            /* Start the DAC again if a new value appears */
            if (!(ADCDAC->DAC_CONF & MASK_DAC_CONF_DAC_CONT1))
            {
                ADCDAC->DAC_CONF |= MASK_DAC_CONF_DAC_START1;
            }
            iRet = 1;
        }
        else
        {
        }
    }

    return iRet;
}


/**
 *  @brief Disable the threshold interrupt
 *  @param num The DAC to use
 *  @param data The array of samples to write
 *  @param len The number of samples to write
 *  @warning This function will only work when continuous mode is selected
 *  @returns number samples on success, -1 otherwise
 */
int dac_writen(uint8_t num, uint16_t *data, size_t len)
{
    int iRet = 0;

    if (!dac_is_valid(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (num == 0)
        {
            if (ADCDAC->DAC_CONF & MASK_DAC_CONF_DAC_CONT0)
            {
#if 0
                while(len)
                {
                    ADCDAC->DAC_DATA0_ADC_DATA = (*data) & 0x3FF;
                    data++;
                    len--;
                    iRet++;
                }
#else
                asm_streamout16(data, &(ADCDAC->DAC_DATA0_ADC_DATA), len*2);
                iRet = len;
#endif
            }
            else
            {
                iRet = -1;
            }
        }
        else if (num == 1)
        {
            if (ADCDAC->DAC_CONF & MASK_DAC_CONF_DAC_CONT1)
            {
#if 0
                while(len)
                {
                    ADCDAC->DAC_DATA1 = (*data) & 0x3FF;
                    data++;
                    len--;
                    iRet++;
                }
#else
                asm_streamout16(data, &(ADCDAC->DAC_DATA1), len*2);
                iRet = len;
#endif
            }
            else
            {
                iRet = -1;
            }
        }
        else
        {
        }
    }

    return iRet;
}


/** @brief Enable the interrupt on a DAC
 *  @param num The DAC to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_enable_interrupt(uint8_t num)
{
    int8_t iRet = 0;

    if (!dac_is_valid(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (num == 0)
        {
            ADCDAC->IRQ_CTRL |= MASK_IRQ_CTRL_DAC_IRQ_EN0;
        }
        else if (num == 1)
        {
            ADCDAC->IRQ_CTRL |= MASK_IRQ_CTRL_DAC_IRQ_EN1;
        }
        else
        {
        }
    }

    return iRet;
}


/** @brief Disable the interrupt on a DAC
 *  @param num The DAC to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_disable_interrupt(uint8_t num)
{
    int8_t iRet = 0;

    if (!dac_is_valid(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (num == 0)
        {
            ADCDAC->IRQ_CTRL &= ~MASK_IRQ_CTRL_DAC_IRQ_EN0;
        }
        else if (num == 1)
        {
            ADCDAC->IRQ_CTRL &= ~MASK_IRQ_CTRL_DAC_IRQ_EN1;
        }
        else
        {
        }
    }

    return iRet;
}


/** @brief Check if the DAC has fired an interrupt
 *  @param num The DAC to use
 *  @warning This function clears the current interrupt status bit
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 */
int8_t dac_is_interrupted(uint8_t num)
{
    int8_t iRet = 0;

    if (!dac_is_valid(num))
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (num == 0)
        {
            if (ADCDAC->IRQ_STATUS_ERR & MASK_IRQ_STATUS_ERR_DAC_IRQ_PEND0)
            {
                iRet = 1;
                ADCDAC->IRQ_STATUS_ERR = MASK_IRQ_STATUS_ERR_DAC_IRQ_PEND0;
            }
        }
        else if (num == 1)
        {
            if (ADCDAC->IRQ_STATUS_ERR & MASK_IRQ_STATUS_ERR_DAC_IRQ_PEND1)
            {
                iRet = 1;
                ADCDAC->IRQ_STATUS_ERR = MASK_IRQ_STATUS_ERR_DAC_IRQ_PEND1;
            }
        }
        else
        {
        }
    }

    return iRet;
}

