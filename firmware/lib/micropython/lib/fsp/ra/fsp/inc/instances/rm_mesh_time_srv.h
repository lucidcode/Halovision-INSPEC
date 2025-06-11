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

#ifndef RM_MESH_TIME_SRV_H
#define RM_MESH_TIME_SRV_H

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
 * @addtogroup RM_MESH_TIME_SRV
 * @{
 **********************************************************************************************************************/

/**
 * Mesh defines times based on International Atomic Time (TAI). The base
 * representation of times is the number of seconds after 00:00:00 TAI on
 * 2000-01-01 (that is, 1999-12-31T23:59:28 UTC)
 */
typedef struct st_rm_mesh_time_srv_info
{
    /** Current TAI time in seconds since the epoch. */
    uint8_t tai_seconds[5];

    /** The sub-second time in units of 1/256s. */
    uint8_t subsecond;

    /** Estimated uncertainty in 10-millisecond steps. */
    uint8_t uncertainty;

    /**
     * 0 = No Time Authority. The element does not have a trusted OOB source of
     * time, such as GPS or NTP. 1 = Time Authority. The element has a trusted OOB
     * source of time, such as GPS or NTP or a battery-backed, properly initialized
     * RTC.
     */
    uint8_t time_authority;

    /** Current difference between TAI and UTC in seconds */
    uint16_t tai_utc_delta;

    /** The local time zone offset in 15-minute increments */
    uint8_t time_zone_offset;
} rm_mesh_time_srv_info_t;

/** Time Zone */
typedef struct st_rm_mesh_time_srv_zone_info
{
    /**
     * Current local time zone offset.
     * Meaningful only in 'Time Zone Status' response.
     */
    uint8_t time_zone_offset_current;

    /** Upcoming local time zone offset. */
    uint8_t time_zone_offset_new;

    /** Absolute TAI time when the Time Zone Offset will change from Current to New. */
    uint8_t tai_of_zone_change[5];
} rm_mesh_time_srv_zone_info_t;

/** TAI-UTC Delta */
typedef struct st_rm_mesh_time_srv_tai_utc_delta_info
{
    /**
     * Current difference between TAI and UTC in seconds.
     * Meaningful only in 'TAI-UTC Delta Status' response.
     */
    uint16_t tai_utc_delta_current;

    /** Upcoming difference between TAI and UTC in seconds. */
    uint16_t tai_utc_delta_new;

    /** Always 0b0. Other values are Prohibited. */
    uint8_t padding;

    /** TAI Seconds time of the upcoming TAI-UTC Delta change */
    uint8_t tai_of_delta_change[5];
} rm_mesh_time_srv_tai_utc_delta_info_t;

/** The Time Role state of an element */
typedef struct st_rm_mesh_time_srv_role_info
{
    /** Time Role */
    uint8_t role;
} rm_mesh_time_srv_role_info_t;

/** BLE mesh time instance control block. DO NOT INITIALIZE. Initialization occurs when RM_MESH_TIME_SRV_Open() is called. */
typedef struct st_rm_mesh_time_srv_instance_ctrl
{
    uint32_t open;                                // To check whether api has been opened or not.
    rm_ble_mesh_access_model_handle_t model_handle;
    rm_ble_mesh_access_model_handle_t setup_server_handle;

    rm_ble_mesh_model_server_cfg_t const * p_cfg; // Pointer to initial configurations.
} rm_mesh_time_srv_instance_ctrl_t;

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const rm_ble_mesh_model_server_api_t g_rm_ble_mesh_model_server_on_rm_mesh_time_srv;

/** @endcond */

/***********************************************************************************************************************
 * Public APIs
 **********************************************************************************************************************/
fsp_err_t RM_MESH_TIME_SRV_Open(rm_ble_mesh_model_server_ctrl_t * const      p_ctrl,
                                rm_ble_mesh_model_server_cfg_t const * const p_cfg);
fsp_err_t RM_MESH_TIME_SRV_Close(rm_ble_mesh_model_server_ctrl_t * const p_ctrl);
fsp_err_t RM_MESH_TIME_SRV_StateUpdate(rm_ble_mesh_model_server_ctrl_t * const         p_ctrl,
                                       rm_ble_mesh_access_server_state_t const * const p_state);

/*******************************************************************************************************************//**
 * @} (end addgroup RM_MESH_TIME_SRV)
 **********************************************************************************************************************/

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif                                 // RM_MESH_TIME_SRV_H
