/**
    @file

    @brief
    Camera interface

    
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

#ifndef FT900_CAM_H_
#define FT900_CAM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/
/** @brief Camera vertical/horizontal trigger mode
    Control at what logic levels the camera will accept data.*/
typedef enum
{
    cam_trigger_mode_0 = 1, /*!< VD = L, HD = L */
    cam_trigger_mode_1 = 2, /*!< VD = L, HD = H */
    cam_trigger_mode_2 = 4, /*!< VD = H, HD = L */
    cam_trigger_mode_3 = 8  /*!< VD = H, HD = H */
} cam_trigger_mode_t;

/** @brief Camera clock polarity */
typedef enum
{
    cam_clock_pol_falling, /*!< Sample data on a falling PCLK edge */
    cam_clock_pol_raising  /*!< Sample data on a raising PCLK edge */
} cam_clock_pol_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Initialise the Camera interface
 *  @param triggers The VD/HD levels to trigger on
 *  @param clkpol The clock polarity of the input
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_init(cam_trigger_mode_t triggers, cam_clock_pol_t clkpol);

/** @brief Start capturing data
 *  @param bytes The number of bytes to capture
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_start(uint16_t bytes);

/** @brief Stop capturing data
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_stop(void);

/** @brief Set the threshold for when the camera interrupt fires
 *  @param n The number of bytes to fill the FIFO with before the interrupt fires
 *           (this must be a multiple of 4)
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_set_threshold(uint16_t n);

/** @brief Empty out the camera buffer
 */
void cam_flush(void);

/** @brief Check how many bytes are available on the FIFO
 *  @returns The number of bytes available
 */
uint16_t cam_available(void);

/** @brief Check how many bytes have been read by the Camera Interface
  *  @returns The number of bytes read
 */
uint16_t cam_total_read(void);

/** @brief Read a number of bytes from the FIFO
 *  @param b A pointer to read the data into
 *  @param len The number of bytes to read from the FIFO (this must be a multiple of 4)
 *  @returns The number of bytes read, 0 otherwise
 */
uint16_t cam_readn(uint8_t *b, size_t len);

/** @brief Enable the threshold interrupt
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_enable_interrupt(void);

/** @brief Disable the threshold interrupt
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_disable_interrupt(void);

/** @brief Check that an interrupt has occurred
 *  @returns 0 when the interrupt hasn't been fired, 1 when the interrupt has fired and -1 otherwise
 */
int8_t cam_is_interrupted(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_CAM_H_ */
