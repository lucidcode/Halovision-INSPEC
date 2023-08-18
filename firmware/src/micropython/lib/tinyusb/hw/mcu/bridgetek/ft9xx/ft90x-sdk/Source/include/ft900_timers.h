/**
    @file

    @brief
    Timers

    
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
 * ("Bridgetek") subject to the licence terms set out
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

#ifndef FT900_TIMERS_H_
#define FT900_TIMERS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* INCLUDES ************************************************************************/
#include <stdint.h>
#include <stddef.h>

/* CONSTANTS ***********************************************************************/

/* TYPES ***************************************************************************/

/** @brief Timer Select */
typedef enum
{
    timer_select_a, /**< Timer A */
    timer_select_b, /**< Timer B */
    timer_select_c, /**< Timer C */
    timer_select_d  /**< Timer D */
} timer_select_t;

/** @brief Timer count direction */
typedef enum
{
    timer_direction_up,     /**< Count up */
    timer_direction_down    /**< Count down */
} timer_direction_t;

/** @brief Timer count mode */
typedef enum
{
    timer_mode_continuous,  /**< Count continuous */
    timer_mode_oneshot      /**< Count one shot */
} timer_mode_t;

/** @brief Timer prescaler select */
typedef enum
{
    timer_prescaler_select_off, /**< Timer prescaler off */
    timer_prescaler_select_on   /**< Timer prescaler on */
} timer_prescaler_select_t;

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/
/**
    @brief      Macros to overload timer_prescaler() function.
    @details    Allows the timer_prescaler call to be made with either one parameter(prescaler)
    			or two parameters (timer, prescaler).
    			Based on code STARTUP_DFU(...) in ft900_startup_dfu.h
 **/
//@{

/// Macro for two parameters.
#define TIMER_PRESCALER_2(x, y) timer_prescaler_ex(x, y)
/// Macro for one parameter to use 'timer_select_a'
#define TIMER_PRESCALER_1(x) TIMER_PRESCALER_2(timer_select_a, x)

/// Macro to choose from multiple parameter list lengths.
#define TIMER_PRESCALER_CHOOSER(_f1, _f2, _f3, ...) _f3
#define TIMER_PRESCALER_RECOMPOSER(argsWithParentheses) TIMER_PRESCALER_CHOOSER argsWithParentheses
#define TIMER_PRESCALER_CHOOSE_FROM_ARG_COUNT(...) TIMER_PRESCALER_RECOMPOSER((__VA_ARGS__, TIMER_PRESCALER_2, TIMER_PRESCALER_1, ))

#define timer_prescaler(...) TIMER_PRESCALER_CHOOSE_FROM_ARG_COUNT(__VA_ARGS__)(__VA_ARGS__)

//@}
/* FUNCTION PROTOTYPES *************************************************************/

/** @brief Set up the prescaler
 *  @param timer [FT93x ONLY] The timer to set up
 *  @param prescaler The clock prescaler to apply to the timer
 *  @returns On success a 0, otherwise -1
 *  @warning 1. This can only be used before starting timers, 2.
 */
int8_t timer_prescaler_ex(timer_select_t timer, uint16_t prescaler);

/** @brief Initialise a timer
 *  @param timer The timer to set up
 *  @param initial The initial value for the timer
 *  @param dir The direction that the timer should count in
 *  @param prescaler Whether or not this timer should use the prescaler
 *  @param mode If the timer should be continuously counting or a one shot
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_init(timer_select_t timer, uint16_t initial, timer_direction_t dir,
        timer_prescaler_select_t prescaler, timer_mode_t mode);

/** @brief Start a timer
 *  @param timer The timer to start
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_start(timer_select_t timer);

/** @brief Stop a timer
 *  @param timer The timer to stop
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_stop(timer_select_t timer);

/** @brief Read the value of a timer
 *  @param timer The timer to read from
 *  @param value A pointer to store the value
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_read(timer_select_t timer, uint16_t *value);

/** @brief Enable the interrupt for a timer
 *  @param timer The timer to enable the interrupt for
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_enable_interrupt(timer_select_t timer);

/** @brief Disable the interrupt for a timer
 *  @param timer The timer to disable the interrupt for
 *  @returns On success a 0, otherwise -1
 */
int8_t timer_disable_interrupt(timer_select_t timer);

/** @brief Check if a timer has been interrupted
 *  @param timer The timer to check
 *  @warning This function clears the current interrupt status bit
 *  @returns 1 for if a timer is interrupted, 0 if the timer is not interrupted, -1 otherwise
 */
int8_t timer_is_interrupted(timer_select_t timer);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_TIMERS_H_ */
