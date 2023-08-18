/**
 @file ov_965x_camera.h
 */
/*
 * ============================================================================
 * History
 * =======
 * 2017-03-15 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
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

#ifndef SOURCES_OV965X_CAMERA_H_
#define SOURCES_OV965X_CAMERA_H_

/**
 @brief Product Identifier for the OV965x
 @details The PID is read from registers on the module to positively
   identify the module as an OV965x.
 */
#define OV965x_PID	0x9657

/**
 @brief OV965x Initialisation
 @details Initialises the OV965x module registers and checks the module is
   an OV965x.
 */
uint16_t ov965x_init(void);

/**
 @brief OV965x Start
 @details Starts streaming data from the OV965x.
 */
void ov965x_start(void);

/**
 @brief OV965x Stop
 @details Stops streaming data from the OV965x.
 */
void ov965x_stop(void);

/**
 @brief OV965x Setup
 @details Configures the OV965x module to a resolution, frame rate and
   output format.
 */
void ov965x_set(int resolution, int frame_rate, int format);

#endif /* SOURCES_OV965X_CAMERA_H_ */
