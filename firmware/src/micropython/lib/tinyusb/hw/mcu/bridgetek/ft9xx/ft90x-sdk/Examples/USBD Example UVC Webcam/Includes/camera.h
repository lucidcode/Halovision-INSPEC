/**
 @file camera.h
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

#ifndef SOURCES_CAMERA_H_
#define SOURCES_CAMERA_H_

/**
 @brief Supported camera interfaces.
 */
//@{
#define CAMERA_OV5640 1
#define CAMERA_OV965X 2
//@}

/**
 @brief Resolution definitions for camera interface.
 */
//@{
#define CAMERA_MODE_QVGA 1
#define CAMERA_MODE_VGA 2
#define CAMERA_MODE_SVGA 3
#define CAMERA_MODE_XGA 4
#define CAMERA_MODE_SXGA 5
//@}

/**
 @brief Output format definitions for camera interface.
 */
//@{
#define CAMERA_FORMAT_UNCOMPRESSED 1
#define CAMERA_FORMAT_MJPEG 2
//@}

/**
 @brief Frame Width and Height definitions for camera interface.
 */
//@{
#define CAMERA_FRAME_WIDTH_QQVGA 160
#define CAMERA_FRAME_WIDTH_QVGA 320
#define CAMERA_FRAME_WIDTH_VGA 640
#define CAMERA_FRAME_WIDTH_SVGA 800
#define CAMERA_FRAME_WIDTH_XGA 1024
#define CAMERA_FRAME_WIDTH_SXGA 1280

#define CAMERA_FRAME_HEIGHT_QQVGA 120
#define CAMERA_FRAME_HEIGHT_QVGA 240
#define CAMERA_FRAME_HEIGHT_VGA 480
#define CAMERA_FRAME_HEIGHT_SVGA 600
#define CAMERA_FRAME_HEIGHT_XGA 768
#define CAMERA_FRAME_HEIGHT_SXGA 1024
//@}

/**
 @brief Frame Rate definitions for camera interface.
 @details
 */
//@{
#define CAMERA_FRAME_RATE_7_5 (15/2)
#define CAMERA_FRAME_RATE_10 10
#define CAMERA_FRAME_RATE_15 15
#define CAMERA_FRAME_RATE_30 30
//@}

/**
 @brief Preferred line length for camera interface
 @details This is the line length of data from the camera where the
  line length can be set. This should be small enough to fit inside
  an isochronous transfer (including the UVC header) but not fall on
  a max packet boundary for when bulk transfers are used.
 */
//@{
#define CAMERA_PREFERRED_LINE_LENGTH 960
//@}

#endif /* SOURCES_CAMERA_H_ */
