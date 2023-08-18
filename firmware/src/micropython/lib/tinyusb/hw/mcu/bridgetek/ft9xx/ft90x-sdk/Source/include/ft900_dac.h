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


#ifndef FT900_DAC_H_
#define FT900_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/
/** @brief DAC run mode */
typedef enum
{
    dac_mode_single,        /*!< Run the DAC in Single Shot mode */
    dac_mode_continuous     /*!< Run the DAC in Continuous mode */
} dac_mode_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Select the mode of the DAC
 *  @param num The DAC to use
 *  @param mode The mode the DAC should be in (single or continuous)
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_mode(uint8_t num, dac_mode_t mode);

/** @brief Select the divider for the DAC conversion rate
 *
 *  \f$ f_{DAC} = \frac{f_{peripheral}}{div + 1} \f$
 *
 *  @param div The divider
 *  @warning The maximum conversion rate is 1 MHz
 *  @returns 0 on success, -1 otherwise
*/
int8_t dac_divider(uint8_t div);

/** @brief See how many samples are still being converted
 *  @param num The DAC to use
 *	@warning This API returns 0xff in case FIFO is full or empty. User need to check
 *			 the interrupt pending bit to conclude 0 samples in FIFO or 128 samples in FIFO.
 *		     This function should be called in the I2S interrupt context in case of FT900 Revision B
 *		     for a reliable count value. For more info, please refer to "TN_159 FT90x Errata Technical Note".
 *  @returns The number of samples still to be converted, or 0 otherwise
 */
uint8_t dac_available(uint8_t num);

/** @brief Start the DAC
 *  @param num The DAC to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_start(uint8_t num);

/** @brief Stop the DAC
 *  @param num The DAC to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_stop(uint8_t num);

/** @brief Write a value to the DAC
 *
 *  This function will automatically update the DAC when it is in single mode.
 *
 *  @param num The DAC to use
 *  @param data The sample to write
 *  @returns The number of bytes written, -1 otherwise
 */
int8_t dac_write(uint8_t num, uint16_t data);

/** @brief Write a series of values to the DAC
 *  @param num The DAC to use
 *  @param data The array of samples to write
 *  @param len The number of samples to write
 *  @warning This function will only work when continuous mode is selected
 *  @returns The number of samples written, -1 otherwise
 */
int dac_writen(uint8_t num, uint16_t *data, size_t len);

/** @brief Enable the interrupt on a DAC
 *
 *  Enable the DAC module to generate an interrupt.  The DAC module will generate an interrupt
 *  after 64 samples have been generated on the DAC.
 *
 *  @param num The DAC to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_enable_interrupt(uint8_t num);

/** @brief Disable the interrupt on a DAC
 *  @param num The DAC to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t dac_disable_interrupt(uint8_t num);

/** @brief Check if the DAC has fired an interrupt
 *  @param num The DAC to use
 *  @warning This function clears the current interrupt status bit
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 */
int8_t dac_is_interrupted(uint8_t num);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_DAC_H_ */
