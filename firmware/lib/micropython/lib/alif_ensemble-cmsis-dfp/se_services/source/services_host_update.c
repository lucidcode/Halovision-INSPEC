/**
 * @file services_host_update.c
 * @brief Update (UPD) SERVICE.
 *
 * Used to allow updating (OTA) of SES images from an application CPU.
 *
 * Images are signed by ALIF.
 *
 * Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 * @ingroup host_services
 * @ingroup services-host-update
 */

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "services_lib_api.h"
#include "services_lib_protocol.h"
#include "services_lib_ids.h"
#if defined(A32)
#include "a32_device.h"
#else
#include "system_utils.h"
#endif

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

#define UNUSED(x) (void)(x)

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

/*******************************************************************************
 *  G L O B A L   V A R I A B L E S
 ******************************************************************************/

/*******************************************************************************
 *  C O D E
 ******************************************************************************/

/**
 * @fn      uint32_t SERVICES_update_stoc(uint32_t services_handle,
 *                                        uint32_t image_address,
 *                                        uint32_t image_size,
 *                                        uint32_t *error_code)
 *
 * @brief Update the whole STOC
 *
 * UPD image can be placed in MRAM or TCM memory pointed to by
 * image_address.
 *
 * @param services_handle
 * @param image_address Place where UPD image is stored
 * @param image_size    UPD image size (deprecated)
 * @param error_code
 * @return Returns Boot loader (BL_ERROR_ ) codes 0x00 = SUCCESS
 * @return BL_ERROR_ENTRY_NOT_SIGNED 0x17
 * @return BL_ERROR_UPD_SIGNATURE_INCORRECT 0x21 Package is not UPD format
 *
 * @ingroup services-host-update
 */
uint32_t SERVICES_update_stoc(uint32_t services_handle,
                              uint32_t image_address,
                              uint32_t image_size,
                              uint32_t *error_code)
{
  update_stoc_svc_t * p_svc = (update_stoc_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(update_stoc_svc_t));

  p_svc->send_image_address = LocalToGlobal((void *)image_address);
  p_svc->send_image_size = image_size;
  uint32_t ret = SERVICES_send_request(services_handle, 
                                       SERVICE_UPDATE_STOC,
                                       DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}
