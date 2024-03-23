/**
    @file ft900_pwm.h

    @brief
    Pulse Width Modulation

    
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


#ifndef FT900_PWM_H_
#define FT900_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/
/** @brief PWM Triggering */
typedef enum
{
    pwm_trigger_disabled,       /*!< Do not trigger */
    pwm_trigger_positive_edge,  /*!< Trigger on a positive edge */
    pwm_trigger_negative_edge,  /*!< Trigger on a negative edge */
    pwm_trigger_any_edge        /*!< Trigger on any edge */
} pwm_trigger_t;

/*!< @brief PWM setup state */
typedef enum
{
    pwm_state_low,  /*!< Setup as low */
    pwm_state_high  /*!< Setup as high */
} pwm_state_t;

typedef enum
{
    pwm_restore_disable, /*!< Do not restore the setup state on wrap around */
    pwm_restore_enable /*!< Do not restore the setup state on wrap around */
} pwm_restore_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Initialise the PWM subsystem
 *  @param prescaler The prescaler for the PWM subsystem
 *  @param maxcount The maximum count of the 16 bit master counter
 *  @param shots The number of loops the PWM subsystem will make, 0 is infinity
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_init(uint8_t prescaler, uint16_t maxcount, uint8_t shots);

/** @brief Enable the PWM subsystem
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_enable(void);

/** @brief Disable the PWM subsystem
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_disable(void);

/** @brief Set up the logic levels for a PWM counter
 *  @param channel The channel to use
 *  @param initstate The initial state of the counter (high or low)
 *  @param restorestate The rollover restore setting
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_levels(uint8_t channel, pwm_state_t initstate, pwm_restore_t restorestate);

/** @brief Set a compare value for a PWM counter
 *  @param channel The channel to use
 *  @param value The value to toggle on
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_compare(uint8_t channel, uint16_t value);

/** @brief Add a toggle to a specific PWM channel
 *  @param channel The channel to add the toggle to
 *  @param toggle The channel to toggle on
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_add_toggle(uint8_t channel, uint8_t toggle);

/** @brief Remove a toggle to a specific PWM channel
 *  @param channel The channel to remove the toggle from
 *  @param toggle The channel to remove the toggle of
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_remove_toggle(uint8_t channel, uint8_t toggle);

/** @brief Set the external trigger settings
 *  @param trigger The trigger setting
 *  @returns On success a 0, otherwise -1
 */
int8_t pwm_trigger(pwm_trigger_t trigger);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_PWM_H_ */
