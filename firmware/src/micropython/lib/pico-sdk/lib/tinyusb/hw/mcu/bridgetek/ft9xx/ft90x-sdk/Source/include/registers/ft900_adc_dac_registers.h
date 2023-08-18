/**
    @file

    @brief
    ADC/DAC registers
    
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


#ifndef FT900_ADC_DAC_REGISTERS_H_
#define FT900_ADC_DAC_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/

#define BIT_DAC_CONF_DAC_PDB0			 	(0)
#define MASK_DAC_CONF_DAC_PDB0      	 	(1 << BIT_DAC_CONF_DAC_PDB0)
#define BIT_DAC_CONF_DAC_CONT0      	 	(1)
#define MASK_DAC_CONF_DAC_CONT0     	 	(1 << BIT_DAC_CONF_DAC_CONT0)
#define BIT_DAC_CONF_DAC_START0     	 	(3)
#define MASK_DAC_CONF_DAC_START0    	 	(1 << BIT_DAC_CONF_DAC_START0)
#define BIT_DAC_CONF_DAC_PDB1       	 	(4)
#define MASK_DAC_CONF_DAC_PDB1      	 	(1 << BIT_DAC_CONF_DAC_PDB1)
#define BIT_DAC_CONF_DAC_CONT1      	 	(5)
#define MASK_DAC_CONF_DAC_CONT1     	 	(1 << BIT_DAC_CONF_DAC_CONT1)
#define BIT_DAC_CONF_DAC_START1     	 	(7)
#define MASK_DAC_CONF_DAC_START1    	 	(1 << BIT_DAC_CONF_DAC_START1)

#define BIT_ADC_CONF_ADC_CHANNEL    	 	(0)
#define MASK_ADC_CONF_ADC_CHANNEL       	(0x7 << BIT_ADC_CONF_ADC_CHANNEL)
#define BIT_ADC_CONF_ADC_PDB            	(3)
#define MASK_ADC_CONF_ADC_PDB       	 	(1 << BIT_ADC_CONF_ADC_PDB)
#define BIT_ADC_CONF_ADC_CONT           	(4)
#define MASK_ADC_CONF_ADC_CONT          	(1 << BIT_ADC_CONF_ADC_CONT)
#define BIT_ADC_CONF_ADC_EXT_VREF   	 	(5)
#define MASK_ADC_CONF_ADC_EXT_VREF  	 	(1 << BIT_ADC_CONF_ADC_EXT_VREF)
#define BIT_ADC_CONF_ADC_START      	 	(7)
#define MASK_ADC_CONF_ADC_START     	 	(1 << BIT_ADC_CONF_ADC_START)

#define BIT_IRQ_CTRL_DAC_IRQ_EN0    	 	(0)
#define MASK_IRQ_CTRL_DAC_IRQ_EN0   	 	(1 << BIT_IRQ_CTRL_DAC_IRQ_EN0)
#define BIT_IRQ_CTRL_DAC_IRQ_EN1    	 	(1)
#define MASK_IRQ_CTRL_DAC_IRQ_EN1   	 	(1 << BIT_IRQ_CTRL_DAC_IRQ_EN1)
#define BIT_IRQ_CTRL_ADC_IRQ_EN     	 	(2)
#define MASK_IRQ_CTRL_ADC_IRQ_EN    	 	(1 << BIT_IRQ_CTRL_ADC_IRQ_EN)
#define BIT_CONF_ADC_HALF_RATE		     	(6)
#define MASK_CONF_ADC_HALF_RATE		    	(1 << BIT_CONF_ADC_HALF_RATE)
#define BIT_CONF_ADC_8BIT_MODE		     	(7)
#define MASK_CONF_ADC_8BIT_MODE		    	(1 << BIT_CONF_ADC_8BIT_MODE)

#define BIT_IRQ_STATUS_ERR_DAC_IRQ_PEND0  	(0)
#define MASK_IRQ_STATUS_ERR_DAC_IRQ_PEND0 	(1 << BIT_IRQ_STATUS_ERR_DAC_IRQ_PEND0)
#define BIT_IRQ_STATUS_ERR_DAC_IRQ_PEND1  	(1)
#define MASK_IRQ_STATUS_ERR_DAC_IRQ_PEND1  	(1 << BIT_IRQ_STATUS_ERR_DAC_IRQ_PEND1)
#define BIT_IRQ_STATUS_ERR_ADC_IRQ_PEND    	(2)
#define MASK_IRQ_STATUS_ERR_ADC_IRQ_PEND   	(1 << BIT_IRQ_STATUS_ERR_ADC_IRQ_PEND)

//#if defined(__FT930__)
#define BIT_IRQ_STATUS_ERR_ADC_BUF_ERR    	(6)
#define MASK_IRQ_STATUS_ERR_ADC_BUF_ERR   	(1 << BIT_IRQ_STATUS_ERR_ADC_BUF_ERR)
#define BIT_IRQ_STATUS_ERR_DAC1_BUF_ERR   	(5)
#define MASK_IRQ_STATUS_ERR_DAC1_BUF_ERR  	(1 << BIT_IRQ_STATUS_ERR_DAC1_BUF_ERR)
#define BIT_IRQ_STATUS_ERR_DAC0_BUF_ERR   	(4)
#define MASK_IRQ_STATUS_ERR_DAC0_BUF_ERR  	(1 << BIT_IRQ_STATUS_ERR_DAC0_BUF_ERR)
//#endif

#define BIT_DAC_ADC_CNT_DAC_DATA_COUNT0 	(0)
#define MASK_DAC_ADC_CNT_DAC_DATA_COUNT0 	(0x7F << BIT_DAC_ADC_CNT_DAC_DATA_COUNT0)
#define BIT_DAC_ADC_CNT_DAC_DATA_COUNT1 	(8)
#define MASK_DAC_ADC_CNT_DAC_DATA_COUNT1 	(0x7F << BIT_DAC_ADC_CNT_DAC_DATA_COUNT1)
#define BIT_DAC_ADC_CNT_ADC_DATA_COUNT  	(16)
#define MASK_DAC_ADC_CNT_ADC_DATA_COUNT 	(0x7F << BIT_DAC_ADC_CNT_ADC_DATA_COUNT)
#define BIT_DAC_ADC_CNT_DAC_DIVIDER     	(24)
#define MASK_DAC_ADC_CNT_DAC_DIVIDER    	(0x7F << BIT_DAC_ADC_CNT_DAC_DIVIDER)

#define BIT_DAC_ADC_DATA_DAC_DATA0      	(0)
#define MASK_DAC_ADC_DATA_DAC_DATA0     	(0x3FF << BIT_DAC_ADC_DATA_DAC_DATA0)
#define BIT_DAC_ADC_DATA_ADC_DATA       	(0)
#if defined(__FT930__)
#define MASK_DAC_ADC_DATA_ADC_DATA      	(0xFF << BIT_DAC_ADC_DATA_ADC_DATA)
#else
#define MASK_DAC_ADC_DATA_ADC_DATA      	(0x3FF << BIT_DAC_ADC_DATA_ADC_DATA)
#endif
#define BIT_DAC_ADC_DATA_DAC_DATA1      	(16)
#define MASK_DAC_ADC_DATA_DAC_DATA1     	(0x3FF << BIT_DAC_ADC_DATA_DAC_DATA1)

/* TYPES ***************************************************************************/

/** @brief Register mappings for DAC and ADC registers */
typedef struct
{
	volatile uint8_t  DAC_CONF;
	volatile uint8_t  ADC_CONF;
	volatile uint8_t  IRQ_CTRL;
	volatile uint8_t  IRQ_STATUS_ERR;
    volatile uint32_t DAC_ADC_CNT;         
    volatile uint16_t DAC_DATA0_ADC_DATA;  
    volatile uint16_t DAC_DATA1;           
} ft900_adc_dac_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/




#endif /* FT900_ADC_DAC_REGISTERS_H_ */
