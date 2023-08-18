/**
    @file ft900_startup_dfu.h

    @brief
    Startup DFU Feature.

    The Startup DFU library allows application to enable the USB device on the
    FT900 temporarily to present a DFU interface to the USB host. Software on
    the USB host can then update the application stored in Flash on the FT900
    regardless of the functionality or features of the existing application.

    The feature can be added to any application by adding a call to the function
    STARTUP_DFU. This call can be made under any conditions - maybe a button
    press at power-up detected by a GPIO or just unconditionally when the
    application is started.

    The USB interface remains active for a short period of time (~200ms) and
    once activated by enumeration from the USB host will continue to stay active
    for around 1000ms after activity has ceased.

    This file contains Startup DFU feature function definitions, constants
    and structures which are exposed in the API.

    Note that as this is a USB device all transaction nomenclature is from the
    point of view from the host. If the device sends data to the host then it
    is called an IN transaction, if it receives data from the host then it is
    an OUT transaction.
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef FT900_STARTUP_DFU_H_
#define FT900_STARTUP_DFU_H_

/* INCLUDES ************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <ft900_usb.h>
#include <ft900_usbd.h>

/* CONSTANTS ***********************************************************************/

/**
 @name DFU Connection Timeouts
 @brief Number of milliseconds to wait for a USB Device connection before
 	 deciding to return to the main application. 1000 ms is realistic.
 */
//@{
#define DFU_CONNECTION_TIMEOUT 1000
//@}

/* TYPES ***************************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS ****************************************************************/

/**
    @brief      Macros to overload STARTUP_DFU() function.
    @details    Allows the STARTUP_DFU call to be made with either no parameters
    			or with one parameter. This permits an optional timeout to be
    			passed to the startup_dfu() function.
    			Based on code in https://github.com/jason-deng/C99FunctionOverload
 **/
//@{
/// Macro for infinte timeout. Zero value is infinite timeout.
#define GO_DFU() startup_dfu(0)
/// Macro for no parameters - take default.
#define STARTUP_DFU_0() startup_dfu(DFU_CONNECTION_TIMEOUT)
/// Macro to choose from multiple parameter list lengths (we only use one additional
/// parameter).
#define STARTUP_DFU_CHOOSER(_f1, _f2, ...) _f2
#define STARTUP_DFU_RECOMPOSER(A) STARTUP_DFU_CHOOSER A
#define STARTUP_DFU_CHOOSE_FROM_ARG_COUNT(...) STARTUP_DFU_RECOMPOSER((__VA_ARGS__, startup_dfu, ))
/// Action when no parameters.
#define STARTUP_DFU_NO_ARG_EXPANDER() ,STARTUP_DFU_0
#define STARTUP_DFU_MACRO_CHOOSER(...) \
			STARTUP_DFU_CHOOSE_FROM_ARG_COUNT(STARTUP_DFU_NO_ARG_EXPANDER __VA_ARGS__ ())
//@}

/**
    @brief      Temporarily start the USB device with a DFU interface.
    @details    When called the USB device will be enabled for a period of time
    			defined around 200ms allowing
    			a USB host to enumerate the device. Once enumerated, the function
    			will wait for around 1000ms for a DFU connection from the USB host.
    			This will allow a program on the host PC to download new firmware
    			to the device.
    			The function returns after one of the timeouts has completed. If the
    			firmware on the device is updated or the device is reset via a
    			USB reset then the device will be reset.
 **/
#define STARTUP_DFU(...) STARTUP_DFU_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

/* FUNCTION PROTOTYPES *************************************************************/

/**
    @brief      Temporarily start the USB device with a DFU interface.
    @details    When called the USB device will be enabled for around 200ms allowing
    			a USB host to enumerate the device. Once enumerated, the function
    			will wait for around 1000ms for a DFU connection from the USB host.
    			This will allow a program on the host PC to download new firmware
    			to the device.
    			The function returns after one of the timeouts has completed. If the
    			firmware on the device is updated or the device is reset via a
    			USB reset then the device will be reset.
    @param[in]  timeout Number of milliseconds to wait until a connection from a
    				host controller is established and a DFU_DETACH request sent
    				to the device. A value of zero will result in the default t
 **/
void startup_dfu(int timeout);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_STARTUP_DFU_H_ */
