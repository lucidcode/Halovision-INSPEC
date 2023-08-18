/**
    @file

    @brief
    PWM Audio Output

    
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
#include <ft900_pwm_pcm.h>
#include <ft900_pwm.h>
#include <ft900_asm.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/
/** @brief Initialise the PWM PCM output
 *  @param channels The number of channels to output.
 *  @param samplerate The sample rate of the audio data.
 *  @param datasize The word size of the samples (8 or 16 bit).
 *  @param endianness The endianness of the 16 bit word. In 8 bit mode this will be ignored.
 *  @param filter If a PCM filter will be used to filter out the PWM carrier.
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_open(pwm_pcm_channels_t channels, uint16_t samplerate, pwm_pcm_data_size_t datasize,
        pwm_pcm_endianness_t endianness, pwm_pcm_filter_t filter)
{
    int8_t iRet = 0;

    /* Set up Channels */
    if (channels == pwm_pcm_channels_mono)
    {
        PWM->PWM_CTRL0 |= MASK_PWM_CTRL0_AUDIOMONO;
    }
    else if (channels == pwm_pcm_channels_stereo)
    {
        PWM->PWM_CTRL0 &= ~MASK_PWM_CTRL0_AUDIOMONO;
    }
    else
    {
        iRet = -1;
    }

    /* Set up Sample rate */
    PWM->PWM_SAMPLE_FREQ_H = (samplerate & 0xFF00) >> 8;
    PWM->PWM_SAMPLE_FREQ_L = (samplerate & 0x00FF) >> 0;

    /* Set up data size */
    if (datasize == pwm_pcm_data_size_8)
    {
        PWM->PWM_CTRL0 |= MASK_PWM_CTRL0_AUDIO8BIT;
    }
    else if (datasize == pwm_pcm_data_size_16)
    {
        PWM->PWM_CTRL0 &= ~MASK_PWM_CTRL0_AUDIO8BIT;
    }
    else
    {
        iRet = -1;
    }

    /* Set up data endianness */
    if (datasize == pwm_pcm_data_size_16)
    {
        if (endianness == pwm_pcm_endianness_little)
        {
            PWM->PWM_CTRL1 |= MASK_PWM_CTRL1_PCMENDIAN;
        }
        else if (endianness == pwm_pcm_endianness_big)
        {
            PWM->PWM_CTRL1 &= ~MASK_PWM_CTRL1_PCMENDIAN;
        }
        else
        {
            iRet = -1;
        }
    }

    /* Set up the filter */
    if (filter == pwm_pcm_filter_on)
    {
        PWM->PWM_CTRL0 |= MASK_PWM_CTRL0_AUDIOFILTER;
    }
    else if (filter == pwm_pcm_filter_off)
    {
        PWM->PWM_CTRL0 &= ~MASK_PWM_CTRL0_AUDIOFILTER;
    }
    else
    {

    }

    if (iRet == 0)
    {
        /* Everything went ok... */
        pwm_levels(0, pwm_state_low, pwm_restore_enable);
        pwm_levels(1, pwm_state_low, pwm_restore_enable);
        pwm_add_toggle(0,0);
        pwm_add_toggle(1,1);

        /* Enable the device */
        PWM->PWM_CTRL0 |= MASK_PWM_CTRL0_AUDIOFIFO | /* Use internal fifo */
                      MASK_PWM_CTRL0_AUDIOSCALE |
                      MASK_PWM_CTRL0_AUDIOEN; /* Enable */
    }

    return iRet;
}


/** @brief Close the PWM PCM Output
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_close(void)
{
    pwm_remove_toggle(0,0);
    pwm_remove_toggle(1,1);
    PWM->PWM_CTRL0 &= ~MASK_PWM_CTRL0_AUDIOEN;
    return 0;
}


/** @brief Write a word of data to the PWM PCM device
 *  @param data The data to write. If 8 bit mode is selected, the top 8 bits will be ignored
 *  @returns The number of bytes written to the FIFO, otherwise -1
 */
int8_t pwm_pcm_write(uint16_t data)
{
    PWM->PWM_BUFFER = data;
    return 1;
}


/** @brief Write a word of data to the PWM PCM device
 *  @param data The data to write. If 8 bit mode is selected, the top 8 bits will be ignored
 *  @param len The size of data to write.
 *  @returns The number of bytes written to the FIFO, otherwise -1
 */
int8_t pwm_pcm_writen(uint16_t *data, size_t len)
{
    asm_streamout16(data, &(PWM->PWM_BUFFER), len*2);
    return len;
}


/** @brief Set the volume of the PWM PCM device.
 *  @param vol The volume to set to.
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_volume(pwm_pcm_volume_t vol)
{
    int8_t iRet = 0;
    switch(vol)
    {
        case pwm_pcm_volume_mute: PWM->PCM_VOLUME = 0; break;
        case pwm_pcm_volume_6: PWM->PCM_VOLUME = 1; break;
        case pwm_pcm_volume_12: PWM->PCM_VOLUME = 2; break;
        case pwm_pcm_volume_19: PWM->PCM_VOLUME = 3; break;
        case pwm_pcm_volume_25: PWM->PCM_VOLUME = 4; break;
        case pwm_pcm_volume_31: PWM->PCM_VOLUME = 5; break;
        case pwm_pcm_volume_37: PWM->PCM_VOLUME = 6; break;
        case pwm_pcm_volume_44: PWM->PCM_VOLUME = 7; break;
        case pwm_pcm_volume_50: PWM->PCM_VOLUME = 8; break;
        case pwm_pcm_volume_56: PWM->PCM_VOLUME = 9; break;
        case pwm_pcm_volume_63: PWM->PCM_VOLUME = 10; break;
        case pwm_pcm_volume_69: PWM->PCM_VOLUME = 11; break;
        case pwm_pcm_volume_75: PWM->PCM_VOLUME = 12; break;
        case pwm_pcm_volume_81: PWM->PCM_VOLUME = 13; break;
        case pwm_pcm_volume_88: PWM->PCM_VOLUME = 14; break;
        case pwm_pcm_volume_94: PWM->PCM_VOLUME = 15; break;
        case pwm_pcm_volume_100: PWM->PCM_VOLUME = 16; break;
        default: iRet = -1; break;
    }
    return iRet;
}


/** @brief Enable an interrupt
 *  @param interrupt The interrupt to enable
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_enable_interrupt(pwm_pcm_interrupt_t interrupt)
{
    int8_t iRet = 0;

    switch(interrupt)
    {
        case pwm_pcm_interrupt_empty: PWM->PWM_INTMASK |= MASK_PWM_INTMASK_FIFOEMPTY; break;
        case pwm_pcm_interrupt_full: PWM->PWM_INTMASK |= MASK_PWM_INTMASK_FIFOFULL; break;
        case pwm_pcm_interrupt_half_full: PWM->PWM_INTMASK |= MASK_PWM_INTMASK_FIFOHALF; break;
        case pwm_pcm_interrupt_overflow: PWM->PWM_INTMASK |= MASK_PWM_INTMASK_FIFOOVER; break;
        case pwm_pcm_interrupt_underflow: PWM->PWM_INTMASK |= MASK_PWM_INTMASK_FIFOUNDER; break;
        default: iRet = -1; break;
    }

    if (iRet == 0)
    {
        /* Set the PWM interrupt enable bit */
        PWM->PWM_CTRL1 |= MASK_PWM_CTRL1_PWMINTMASK;
    }

    return 0;
}


/** @brief Disable an interrupt
 *  @param interrupt The interrupt to disable
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_pcm_disable_interrupt(pwm_pcm_interrupt_t interrupt)
{
    int8_t iRet = 0;

    switch(interrupt)
    {
        case pwm_pcm_interrupt_empty: PWM->PWM_INTMASK &= ~MASK_PWM_INTMASK_FIFOEMPTY; break;
        case pwm_pcm_interrupt_full: PWM->PWM_INTMASK &= ~MASK_PWM_INTMASK_FIFOFULL; break;
        case pwm_pcm_interrupt_half_full: PWM->PWM_INTMASK &= ~MASK_PWM_INTMASK_FIFOHALF; break;
        case pwm_pcm_interrupt_overflow: PWM->PWM_INTMASK &= ~MASK_PWM_INTMASK_FIFOOVER; break;
        case pwm_pcm_interrupt_underflow: PWM->PWM_INTMASK &= ~MASK_PWM_INTMASK_FIFOUNDER; break;
        default: iRet = -1; break;
    }

    if (iRet == 0)
    {
        if (PWM->PWM_INTMASK == 0)
        {
            /* No more interrupt to fire. Clear the PWM interrupt enable bit */
            PWM->PWM_CTRL1 &= ~MASK_PWM_CTRL1_PWMINTMASK;
        }
    }

    return 0;
}


/** @brief Query if an interrupt has fired
 *  @param interrupt The interrupt to query
 *  @warning This function will clear the interrupt being queried and the global PWM interrupt flag
 *  @returns 1 for if PWM is interrupted, 0 if PWM is not interrupted, -1 otherwise
 */
int8_t pwm_pcm_is_interrupted(pwm_pcm_interrupt_t interrupt)
{
    int8_t iRet = 0;

    /* Interrupt Fired! */
    switch(interrupt)
    {
        case pwm_pcm_interrupt_empty:
        {
            if (PWM->PWM_INTSTATUS & MASK_PWM_INTSTATUS_FIFOEMPTY)
            {
                PWM->PWM_INTSTATUS |= MASK_PWM_INTSTATUS_FIFOEMPTY; /* Clear */
                iRet = 1;
            }
        }
        break;

        case pwm_pcm_interrupt_full:
        {
            if (PWM->PWM_INTSTATUS & MASK_PWM_INTSTATUS_FIFOFULL)
            {
                PWM->PWM_INTSTATUS |= MASK_PWM_INTSTATUS_FIFOFULL; /* Clear */
                iRet = 1;
            }
        }
        break;

        case pwm_pcm_interrupt_half_full:
        {
            if (PWM->PWM_INTSTATUS & MASK_PWM_INTSTATUS_FIFOHALF)
            {
                PWM->PWM_INTSTATUS |= MASK_PWM_INTSTATUS_FIFOHALF; /* Clear */
                iRet = 1;
            }
        }
        break;

        case pwm_pcm_interrupt_overflow:
        {
            if (PWM->PWM_INTSTATUS & MASK_PWM_INTSTATUS_FIFOOVER)
            {
                PWM->PWM_INTSTATUS |= MASK_PWM_INTSTATUS_FIFOOVER; /* Clear */
                iRet = 1;
            }
        }
        break;

        case pwm_pcm_interrupt_underflow:
        {
            if (PWM->PWM_INTSTATUS & MASK_PWM_INTSTATUS_FIFOUNDER)
            {
                PWM->PWM_INTSTATUS |= MASK_PWM_INTSTATUS_FIFOUNDER; /* Clear */
                iRet = 1;
            }
        }
        break;

        default: iRet = -1; break;
    }

    if (iRet == 1)
    {
        /* Clear the global interrupt */
        PWM->PWM_CTRL1 |= MASK_PWM_CTRL1_PWMINT;
    }

    return iRet;
}
