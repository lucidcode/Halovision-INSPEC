/**
    @file

    @brief
    PWM Audio

    
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

#ifndef FT900_PWM_PCM_H_
#define FT900_PWM_PCM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/
/** @brief PWM Channel selection */
typedef enum
{
    pwm_pcm_channels_mono = 1,  /*!< Mono */
    pwm_pcm_channels_stereo = 2 /*!< Stereo */
} pwm_pcm_channels_t;

/** @brief PWM data size selection */
typedef enum
{
    pwm_pcm_data_size_8 = 8,    /*!< 8 bit */
    pwm_pcm_data_size_16 = 16   /*!< 16 bit */
} pwm_pcm_data_size_t ;

/** @brief PWM endianness selection */
typedef enum
{
    pwm_pcm_endianness_big,     /*!< Big endian data */
    pwm_pcm_endianness_little   /*!< Little endian data */
} pwm_pcm_endianness_t;

/** @brief PWM PCM Filter */
typedef enum
{
    pwm_pcm_filter_off = 0, /*!< Off */
    pwm_pcm_filter_on = 1   /*!< On */
} pwm_pcm_filter_t;

/** @brief PWM PCM Volume */
typedef enum
{
    pwm_pcm_volume_mute = 0,
    pwm_pcm_volume_6,           /**< 6.25% volume */
    pwm_pcm_volume_12,          /**< 12.5% volume */
    pwm_pcm_volume_19,          /**< 19% volume */
    pwm_pcm_volume_25,          /**< 25% volume */
    pwm_pcm_volume_31,          /**< 31% volume */
    pwm_pcm_volume_37,          /**< 37% volume */
    pwm_pcm_volume_44,          /**< 44% volume */
    pwm_pcm_volume_50,          /**< 50% volume */
    pwm_pcm_volume_56,          /**< 56% volume */
    pwm_pcm_volume_63,          /**< 63% volume */
    pwm_pcm_volume_69,          /**< 69% volume */
    pwm_pcm_volume_75,          /**< 75% volume */
    pwm_pcm_volume_81,          /**< 81% volume */
    pwm_pcm_volume_88,          /**< 88% volume */
    pwm_pcm_volume_94,          /**< 94% volume */
    pwm_pcm_volume_100          /**< 100% volume */
} pwm_pcm_volume_t;

/** @brief PWM PCM Interrupt selection */
typedef enum
{
    pwm_pcm_interrupt_empty,        /*!< FIFO Empty */
    pwm_pcm_interrupt_full,         /*!< FIFO Full */
    pwm_pcm_interrupt_half_full,    /*!< FIFO Half Full */
    pwm_pcm_interrupt_overflow,     /*!< FIFO Overflow */
    pwm_pcm_interrupt_underflow     /*!< FIFO Underflow */
} pwm_pcm_interrupt_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Initialise the PWM PCM output
 *  @param channels The number of channels to output.
 *  @param samplerate The sample rate of the audio data.
 *  @param datasize The word size of the samples (8 or 16 bit).
 *  @param endianness The endianness of the 16 bit word. In 8 bit mode this will be ignored.
 *  @param filter If a PCM filter will be used to filter out the PWM carrier.
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_open(pwm_pcm_channels_t channels, uint16_t samplerate, pwm_pcm_data_size_t datasize,
        pwm_pcm_endianness_t endianness, pwm_pcm_filter_t filter);

/** @brief Close the PWM PCM Output
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_close(void);

/** @brief Write a word of data to the PWM PCM device
 *  @param data The data to write. If 8 bit mode is selected, the top 8 bits will be ignored
 *  @returns The number of bytes written to the FIFO, otherwise -1
 */
int8_t pwm_pcm_write(uint16_t data);

/** @brief Write a word of data to the PWM PCM device
 *  @param data The data to write. If 8 bit mode is selected, the top 8 bits will be ignored
 *  @param len The size of data to write.
 *  @returns The number of bytes written to the FIFO, otherwise -1
 */
int8_t pwm_pcm_writen(uint16_t *data, size_t len);

/** @brief Set the volume of the PWM PCM device.
 *  @param vol The volume to set to.
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_volume(pwm_pcm_volume_t vol);

/** @brief Enable an interrupt
 *  @param interrupt The interrupt to enable
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_enable_interrupt(pwm_pcm_interrupt_t interrupt);

/** @brief Disable an interrupt
 *  @param interrupt The interrupt to disable
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_disable_interrupt(pwm_pcm_interrupt_t interrupt);

/** @brief Query if an interrupt has fired
 *  @param interrupt The interrupt to query
 *  @warning This function will clear the interrupt being queried and the global PWM interrupt flag
 *  @returns 1 for if PWM is interrupted, 0 if PWM is not interrupted, -1 otherwise
 */
int8_t pwm_pcm_is_interrupted(pwm_pcm_interrupt_t interrupt);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_PWM_PCM_H_ */
