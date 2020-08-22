/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image library configuration.
 */
#ifndef __IMLIB_CONFIG_H__
#define __IMLIB_CONFIG_H__

// Enable LAB LUT
//#define IMLIB_ENABLE_LAB_LUT

// Enable YUV LUT
//#define IMLIB_ENABLE_YUV_LUT

// Enable remove_shadows()
//#define IMLIB_ENABLE_REMOVE_SHADOWS

// Enable linpolar()
//#define IMLIB_ENABLE_LINPOLAR

// Enable logpolar()
//#define IMLIB_ENABLE_LOGPOLAR

// Enable chrominvar()
//#define IMLIB_ENABLE_CHROMINVAR

// Enable illuminvar()
//#define IMLIB_ENABLE_ILLUMINVAR

// Enable invariant table
//#define IMLIB_ENABLE_INVARIANT_TABLE

// Enable lens_corr()
//#define IMLIB_ENABLE_LENS_CORR

// Enable rotation_corr()
//#define IMLIB_ENABLE_ROTATION_CORR

// Enable phasecorrelate()
//#define IMLIB_ENABLE_FIND_DISPLACEMENT

// rotation_corr() is required by phasecorrelate()
#if defined(IMLIB_ENABLE_FIND_DISPLACEMENT)\
    && !defined(IMLIB_ENABLE_ROTATION_CORR)
    #define IMLIB_ENABLE_ROTATION_CORR
#endif

// Enable get_similarity()
//#define IMLIB_ENABLE_GET_SIMILARITY

// Enable find_lines()
//#define IMLIB_ENABLE_FIND_LINES

// Enable find_line_segments()
//#define IMLIB_ENABLE_FIND_LINE_SEGMENTS

// find_lines() is required by the old find_line_segments()
//#if defined(IMLIB_ENABLE_FIND_LINE_SEGMENTS)\
//    && !defined(IMLIB_ENABLE_FIND_LINES)
//    #define IMLIB_ENABLE_FIND_LINES
//#endif

// Enable find_circles()
//#define IMLIB_ENABLE_FIND_CIRCLES

// Enable find_rects()
//#define IMLIB_ENABLE_FIND_RECTS

// Enable find_qrcodes() (14 KB)
//#define IMLIB_ENABLE_QRCODES

// Enable find_apriltags() (64 KB)
//#define IMLIB_ENABLE_APRILTAGS

// Enable find_datamatrices() (26 KB)
//#define IMLIB_ENABLE_DATAMATRICES

// Enable find_barcodes() (42 KB)
//#define IMLIB_ENABLE_BARCODES

// Enable Tensor Flow
//#define IMLIB_ENABLE_TF

#endif //__IMLIB_CONFIG_H__
