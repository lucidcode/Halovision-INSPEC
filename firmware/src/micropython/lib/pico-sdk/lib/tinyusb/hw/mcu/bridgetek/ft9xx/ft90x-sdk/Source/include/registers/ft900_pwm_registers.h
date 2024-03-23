/**
    @file

    @brief
    Pulse Width Modulation registers

    
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

#ifndef FT900_PWM_REGISTERS_H_
#define FT900_PWM_REGISTERS_H_

/* INCLUDES ************************************************************************/
#include <stdint.h>

/* CONSTANTS ***********************************************************************/
#define BIT_PWM_CTRL0_SOFTRESET     (0)
#define MASK_PWM_CTRL0_SOFTRESET    (1 << BIT_PWM_CTRL0_SOFTRESET)
#define BIT_PWM_CTRL0_AUDIOEN       (2)
#define MASK_PWM_CTRL0_AUDIOEN      (1 << BIT_PWM_CTRL0_AUDIOEN)
#define BIT_PWM_CTRL0_AUDIOFILTER   (3)
#define MASK_PWM_CTRL0_AUDIOFILTER  (1 << BIT_PWM_CTRL0_AUDIOFILTER)
#define BIT_PWM_CTRL0_AUDIOSCALE    (4)
#define MASK_PWM_CTRL0_AUDIOSCALE   (1 << BIT_PWM_CTRL0_AUDIOSCALE)
#define BIT_PWM_CTRL0_AUDIO8BIT     (5)
#define MASK_PWM_CTRL0_AUDIO8BIT    (1 << BIT_PWM_CTRL0_AUDIO8BIT)
#define BIT_PWM_CTRL0_AUDIOMONO     (6)
#define MASK_PWM_CTRL0_AUDIOMONO    (1 << BIT_PWM_CTRL0_AUDIOMONO)
#define BIT_PWM_CTRL0_AUDIOFIFO     (7)
#define MASK_PWM_CTRL0_AUDIOFIFO    (1 << BIT_PWM_CTRL0_AUDIOFIFO)

#define BIT_PWM_CTRL1_PWMEN         (0)
#define MASK_PWM_CTRL1_PWMEN        (1 << BIT_PWM_CTRL1_PWMEN)
#define BIT_PWM_CTRL1_PWMTRIG       (1)
#define MASK_PWM_CTRL1_PWMTRIG      (0x3 << BIT_PWM_CTRL1_PWMTRIG)
#define BIT_PWM_CTRL1_PWMBUSY       (3)
#define MASK_PWM_CTRL1_PWMBUSY      (1 << BIT_PWM_CTRL1_PWMBUSY)
#define BIT_PWM_CTRL1_PWMINTMASK    (4)
#define MASK_PWM_CTRL1_PWMINTMASK   (1 << BIT_PWM_CTRL1_PWMINTMASK)
#define BIT_PWM_CTRL1_PWMINT        (5)
#define MASK_PWM_CTRL1_PWMINT       (1 << BIT_PWM_CTRL1_PWMINT)
#define BIT_PWM_CTRL1_PCMENDIAN     (6)
#define MASK_PWM_CTRL1_PCMENDIAN    (1 << BIT_PWM_CTRL1_PCMENDIAN)

#define BIT_PWM_INTMASK_INTMASK     (0)
#define MASK_PWM_INTMASK_INTMASK    (1 << BIT_PWM_INTMASK_INTMASK)
#define BIT_PWM_INTMASK_FIFOUNDER   (1)
#define MASK_PWM_INTMASK_FIFOUNDER  (1 << BIT_PWM_INTMASK_FIFOUNDER)
#define BIT_PWM_INTMASK_FIFOOVER    (2)
#define MASK_PWM_INTMASK_FIFOOVER   (1 << BIT_PWM_INTMASK_FIFOOVER)
#define BIT_PWM_INTMASK_FIFOHALF    (3)
#define MASK_PWM_INTMASK_FIFOHALF   (1 << BIT_PWM_INTMASK_FIFOHALF)
#define BIT_PWM_INTMASK_FIFOFULL    (4)
#define MASK_PWM_INTMASK_FIFOFULL   (1 << BIT_PWM_INTMASK_FIFOFULL)
#define BIT_PWM_INTMASK_FIFOEMPTY   (5)
#define MASK_PWM_INTMASK_FIFOEMPTY  (1 << BIT_PWM_INTMASK_FIFOEMPTY)

#define BIT_PWM_INTSTATUS_INTMASK   (0)
#define MASK_PWM_INTSTATUS_INTMASK  (1 << BIT_PWM_INTSTATUS_INTMASK)
#define BIT_PWM_INTSTATUS_FIFOUNDER (1)
#define MASK_PWM_INTSTATUS_FIFOUNDER (1 << BIT_PWM_INTSTATUS_FIFOUNDER)
#define BIT_PWM_INTSTATUS_FIFOOVER  (2)
#define MASK_PWM_INTSTATUS_FIFOOVER (1 << BIT_PWM_INTSTATUS_FIFOOVER)
#define BIT_PWM_INTSTATUS_FIFOHALF  (3)
#define MASK_PWM_INTSTATUS_FIFOHALF (1 << BIT_PWM_INTSTATUS_FIFOHALF)
#define BIT_PWM_INTSTATUS_FIFOFULL  (4)
#define MASK_PWM_INTSTATUS_FIFOFULL (1 << BIT_PWM_INTSTATUS_FIFOFULL)
#define BIT_PWM_INTSTATUS_FIFOEMPTY (5)
#define MASK_PWM_INTSTATUS_FIFOEMPTY (1 << BIT_PWM_INTSTATUS_FIFOEMPTY)

#define BIT_PCM_VOLUME_VOLUME       (0)
#define MASK_PCM_VOLUME_VOLUME      (0x1F << BIT_PWM_VOLUME_VOLUME)

/* TYPES ***************************************************************************/
/** @brief Registers mappings for PWM and PWM Audio registers */
typedef struct
{
    volatile uint8_t PWM_CTRL0;
    volatile uint8_t PWM_CTRL1;
    volatile uint8_t PWM_PRESCALER;
    volatile uint8_t PWM_CNTL;
    volatile uint8_t PWM_CNTH;
    volatile uint8_t PWM_CMP0L;
    volatile uint8_t PWM_CMP0H;
    volatile uint8_t PWM_CMP1L;
    volatile uint8_t PWM_CMP1H;
    volatile uint8_t PWM_CMP2L;
    volatile uint8_t PWM_CMP2H;
    volatile uint8_t PWM_CMP3L;
    volatile uint8_t PWM_CMP3H;
    volatile uint8_t PWM_CMP4L;
    volatile uint8_t PWM_CMP4H;
    volatile uint8_t PWM_CMP5L;
    volatile uint8_t PWM_CMP5H;
    volatile uint8_t PWM_CMP6L;
    volatile uint8_t PWM_CMP6H;
    volatile uint8_t PWM_CMP7L;
    volatile uint8_t PWM_CMP7H;
    volatile uint8_t PWM_TOGGLE0;
    volatile uint8_t PWM_TOGGLE1;
    volatile uint8_t PWM_TOGGLE2;
    volatile uint8_t PWM_TOGGLE3;
    volatile uint8_t PWM_TOGGLE4;
    volatile uint8_t PWM_TOGGLE5;
    volatile uint8_t PWM_TOGGLE6;
    volatile uint8_t PWM_TOGGLE7;
    volatile uint8_t PWM_OUT_CLR_EN;
    volatile uint8_t PWM_CTRL_BL_CMP8;
    volatile uint8_t PWM_INIT;
    volatile uint8_t PWM_INTMASK;
    volatile uint8_t PWM_INTSTATUS;
    volatile uint8_t PWM_SAMPLE_FREQ_H;
    volatile uint8_t PWM_SAMPLE_FREQ_L;
    volatile uint8_t PCM_VOLUME;
    volatile uint8_t UNUSED37;
    volatile uint8_t UNUSED38;
    volatile uint8_t UNUSED39;
    volatile uint8_t UNUSED40;
    volatile uint8_t UNUSED41;
    volatile uint8_t UNUSED42;
    volatile uint8_t UNUSED43;
    volatile uint8_t UNUSED44;
    volatile uint8_t UNUSED45;
    volatile uint8_t UNUSED46;
    volatile uint8_t UNUSED47;
    volatile uint8_t UNUSED48;
    volatile uint8_t UNUSED49;
    volatile uint8_t UNUSED50;
    volatile uint8_t UNUSED51;
    volatile uint8_t UNUSED52;
    volatile uint8_t UNUSED53;
    volatile uint8_t UNUSED54;
    volatile uint8_t UNUSED55;
    volatile uint8_t UNUSED56;
    volatile uint8_t UNUSED57;
    volatile uint8_t UNUSED58;
    volatile uint8_t UNUSED59;
    volatile uint16_t PWM_BUFFER;
} ft900_pwm_regs_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

#endif /* FT900_PWM_REGISTERS_H_ */
