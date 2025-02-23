/***********************************************************************************************************************
 * Copyright [2020-2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

#ifndef RM_MESH_LIGHT_XYL_SRV_H
#define RM_MESH_LIGHT_XYL_SRV_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/

#include "rm_ble_mesh_model_server_api.h"
#include "rm_ble_mesh_access.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/

/*********************************************************************************************************************
 * Typedef definitions
 *********************************************************************************************************************/

/*******************************************************************************************************************//**
 *
 * @addtogroup RM_MESH_LIGHT_XYL_SRV
 * @{
 **********************************************************************************************************************/

/**
 * Light xyL state is a composite state that includes the xyL Lightness, the
 * Light xyL x and the Light xyL y states
 */
typedef struct st_rm_mesh_light_xyl_srv_info
{
    /** The perceived lightness of a light emitted by the element */
    uint16_t xyl_lightness;

    /** Target perceived lightness - used in response path */
    uint16_t target_xyl_lightness;

    /** The 16-bit value representing the x coordinate of a CIE1931 color light */
    uint16_t xyl_x;

    /** Target x coordinate - used in response path */
    uint16_t target_xyl_x;

    /** The 16-bit value representing the y coordinate of a CIE1931 color light */
    uint16_t xyl_y;

    /** Target y coordinate - used in response path */
    uint16_t target_xyl_y;

    /** TID - Used in request path */
    uint8_t tid;

    /**
     * Transition Time - Used in request path.
     * Used as remaining time in response path.
     */
    uint8_t transition_time;

    /** Delay - Used in request path */
    uint8_t delay;

    /** Transition Timer Handle */
    uint16_t transition_time_handle;
} rm_mesh_light_xyl_srv_info_t;

/**
 * Light xyL target state is a composite state that includes the xyL Lightness,
 * the Light xyL x and the Light xyL y states
 */
typedef struct st_rm_mesh_light_xyl_srv_target_info
{
    /** The perceived lightness of a light emitted by the element */
    uint16_t xyl_lightness;

    /** The 16-bit value representing the x coordinate of a CIE1931 color light */
    uint16_t xyl_x;

    /** The 16-bit value representing the y coordinate of a CIE1931 color light */
    uint16_t xyl_y;
} rm_mesh_light_xyl_srv_target_info_t;

/**
 * Light xyL default state is a composite state that includes the xyL Lightness,
 * the Light xyL x and the Light xyL y states
 */
typedef struct st_rm_mesh_light_xyl_srv_default_info
{
    /** The perceived lightness of a light emitted by the element */
    uint16_t xyl_lightness;

    /** The 16-bit value representing the x coordinate of a CIE1931 color light */
    uint16_t xyl_x;

    /** The 16-bit value representing the y coordinate of a CIE1931 color light */
    uint16_t xyl_y;
} rm_mesh_light_xyl_srv_default_info_t;

/**
 * Light xyL Range state determines the minimum and maximum values of the Light
 * xyL x and syL y state of an element
 */
typedef struct st_rm_mesh_light_xyl_srv_range_info
{
    /** The minimum value of a Light xyL x state of an element */
    uint16_t xyl_x_range_min;

    /** The maximum value of a Light xyL x state of an element */
    uint16_t xyl_x_range_max;

    /** The minimum  value of a Light xyL y state of an element */
    uint16_t xyl_y_range_min;

    /** The maximum  value of a Light xyL y state of an element */
    uint16_t xyl_y_range_max;

    /** Status - Used in the response path */
    uint8_t status;
} rm_mesh_light_xyl_srv_range_info_t;

/** BLE mesh light xyl instance control block. DO NOT INITIALIZE. Initialization occurs when RM_MESH_LIGHT_XYL_SRV_Open() is called. */
typedef struct st_rm_mesh_light_xyl_srv_instance_ctrl
{
    uint32_t open;                                // To check whether api has been opened or not.
    rm_ble_mesh_access_model_handle_t model_handle;
    rm_ble_mesh_access_model_handle_t setup_server_handle;

    rm_ble_mesh_model_server_cfg_t const * p_cfg; // Pointer to initial configurations.
} rm_mesh_light_xyl_srv_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const rm_ble_mesh_model_server_api_t g_rm_ble_mesh_model_server_on_rm_mesh_light_xyl_srv;

/** @endcond */

/***********************************************************************************************************************
 * Public APIs
 **********************************************************************************************************************/
fsp_err_t RM_MESH_LIGHT_XYL_SRV_Open(rm_ble_mesh_model_server_ctrl_t * const      p_ctrl,
                                     rm_ble_mesh_model_server_cfg_t const * const p_cfg);
fsp_err_t RM_MESH_LIGHT_XYL_SRV_Close(rm_ble_mesh_model_server_ctrl_t * const p_ctrl);
fsp_err_t RM_MESH_LIGHT_XYL_SRV_StateUpdate(rm_ble_mesh_model_server_ctrl_t * const         p_ctrl,
                                            rm_ble_mesh_access_server_state_t const * const p_state);

/*******************************************************************************************************************//**
 * @} (end addgroup RM_MESH_LIGHT_XYL_SRV)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MESH_LIGHT_XYL_SRV_H
