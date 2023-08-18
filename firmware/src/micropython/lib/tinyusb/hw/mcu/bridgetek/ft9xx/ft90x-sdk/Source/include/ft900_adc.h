/**
    @file

    @brief
    Analogue to Digital Converter

    
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

#ifndef FT900_ADC_H_
#define FT900_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/
/** @brief ADC Run Mode */
typedef enum
{
    adc_mode_single,        /*!< One analogue reading will be taken and then the ADC stopped */
    adc_mode_continuous     /*!< The ADC will continuously aquire analogue readings */
} adc_mode_t;

/** @brief ADC Resolution. ADC is configurable to support 8 or 10 bit resolution. */
typedef enum
{
	adc_10bit,				/*!< 10-bit ADC resolution */
	adc_8bit				/*!< 8-bit ADC resolution */
}adc_resolution_t;

/** @brief ADC Frequency */
typedef enum
{
	adc_12_5_MHz,			/*!< ADC clock is 12.5 MHz */
	adc_6_25_MHz			/*!< ADC clock is 6.25 MHz */
}adc_clock_t;
/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Choose the mode that the ADC will run in
 *  @param mode The mode (single or continuous)
 *  @returns 0 on success, -1 otherwise
 */
int8_t adc_mode(adc_mode_t mode);

/** @brief Start the ADC capturing
 *  @param channel The channel to select
 *  @returns 0 on success, -1 otherwise
 */
int8_t adc_start(uint8_t channel);

/** @brief Stop the ADC from capturing
 *  @returns 0 on success, -1 otherwise
 */
int8_t adc_stop(void);

/** @brief Get the number of ADC samples available
 *  @warning This function should only be called when ADC continuous mode is enabled,
 *  	     and should be called inside ISR context.
 *  @returns The number of ADC samples available in FIFO
 */
uint8_t adc_available(void);

/** @brief Get the next sample from the ADC
 *  @param val A pointer to store the sample
 *  @returns The number of samples read, -1 otherwise
 */
int8_t adc_read(uint16_t *val);

/** @brief Get a collection of samples from the ADC
 *  @param val An array pointer to store the samples
 *  @param len The length of the array
 *  @warning This function will only work when the ADC is in continuous mode
 *  @returns The number of samples read, -1 otherwise
 */
int adc_readn(uint16_t *val, size_t len);

/** @brief Enable the Interrupt on the ADC
 *  @returns 0 on success, -1 otherwise
 */
int8_t adc_enable_interrupt(void);

/** @brief Disable the Interrupt on the ADC
 *  @returns 0 on success, -1 otherwise
 */
int8_t adc_disable_interrupt(void);

/** @brief Check that the ADC has been interrupted
 *  @warning This function will clear the current interrupt bit when called
 *  @returns 1 interrupted, 0 not interrupted
 */
int8_t adc_is_interrupted(void);

/** @brief Select ADC resolution.
 *  @param resolution The resolution(10bit/8bit)
 *  @returns On success a 0, otherwise -1
 *  @warning This feature is only supported in FT900 revision C
 */
int8_t adc_select_resolution(adc_resolution_t resolution);

/** @brief Select ADC frequency.
 *  @param frequency The frequency(12.5MHz/6.25MHz)
 *  @returns On success a 0, otherwise -1
 *  @warning This feature is only supported in FT900 revision C
 */
int8_t adc_select_frequency(adc_clock_t frequency);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_ADC_H_ */
