/**
 * @file services_host_system.c
 *
 * @brief System Management services source file
 *
 * @par
 *
 * Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 * @ingroup host_services
 * @ingroup services-host-system
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

static const uint8_t s_alif_oid[] = {0x78, 0x59, 0x94};

/*******************************************************************************
 *  C O D E
 ******************************************************************************/

/**
 * @fn      uint32_t SERVICES_system_get_toc_version(uint32_t * toc_version)
 * @brief   Retrieves the SES SW Version number
 *
 * Number is encoded as Major Minor and Patch
 *
 * @param   services_handle
 * @param   toc_version
 * @param   error_code
 * @return  SES SW version (Semantc versioning format).
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_get_toc_version(uint32_t services_handle, 
                                         uint32_t *toc_version,
                                         uint32_t *error_code)
{
  get_toc_version_svc_t *p_svc = (get_toc_version_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(get_toc_version_svc_t)); /* create packet */

  uint32_t return_code = SERVICES_send_request(services_handle,
                                               SERVICE_SYSTEM_MGMT_GET_TOC_VERSION,
                                               DEFAULT_TIMEOUT);

  *toc_version = p_svc->resp_version;
  *error_code = p_svc->resp_error_code;

  return return_code;
}

/**
 * @fn      uint32_t SERVICES_system_get_toc_number(uint32_t toc_version)
 * @brief   get the TOC number
 *
 * Returns the number of Table of contents entries in MRAM
 *
 * @param   services_handle
 * @param   toc_number         Number of TOCs
 * @ingroup services-host-system
 * @return
 */
uint32_t SERVICES_system_get_toc_number(uint32_t services_handle, 
                                        uint32_t *toc_number,
                                        uint32_t *error_code)
{
  get_toc_number_svc_t * p_svc = (get_toc_number_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(get_toc_number_svc_t));
	
  uint32_t return_code = SERVICES_send_request(services_handle,
                                              SERVICE_SYSTEM_MGMT_GET_TOC_NUMBER,
                                              DEFAULT_TIMEOUT);

  *toc_number = p_svc->resp_number_of_toc;
  *error_code = p_svc->resp_error_code;

  return return_code;
}

/**
 * @brief   get toc info via name
 *
 * Returns the TOC information for the provided CPU name.
 *
 * @param services_handle
 * @param cpu_name
 * @param error_code
 * @return
 * @ingroup services-host-system
 * @note    function is deprecated
 * @todo    Remove function as deprecated
 */
uint32_t SERVICES_system_get_toc_via_name(uint32_t services_handle, 
                                          const uint8_t *cpu_name,
                                          uint32_t *error_code)
{
  UNUSED(services_handle);
  UNUSED(cpu_name);

  *error_code = SERVICES_RESP_UNKNOWN_COMMAND;

  return SERVICE_SUCCESS;
}

/**
 * @brief     get toc info via cpuid
 *
 * Returns the TOC information for a given CPU.
 *
 * See SERVICES_cpuid_t for details
 *
 * @param services_handle
 * @param cpuid           Which application CPU
 * @param toc_info        ATOC information return
 * @param error_code
 * @return
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_get_toc_via_cpuid(uint32_t services_handle,
                                           SERVICES_cpuid_t cpuid, 
                                           SERVICES_toc_data_t *toc_info,
                                           uint32_t *error_code)
{
  uint32_t toc_number = 0; /* Number of TOCs from SE */
  uint32_t return_code = SERVICE_SUCCESS; /* Service error code */

  /* defend against the application */
  if (toc_info == NULL)
  {
	  return SERVICE_INVALID_PARAMETER;
  }

  return_code = SERVICES_system_get_toc_number(services_handle,
                                               &toc_number,
                                               error_code);
  if (SERVICE_SUCCESS != return_code)
  {
    return return_code;
  }

  toc_info->number_of_toc_entries = 0;

  get_toc_data_t *p_svc;
  for (uint32_t i = 0; i < toc_number; i++)
  {
    p_svc = (get_toc_data_t *)
      SERVICES_prepare_packet_buffer(sizeof(get_toc_data_t));

    p_svc->send_entry_idx = i;
    if (SERVICE_SUCCESS == SERVICES_send_request(
                                              services_handle,
                                              SERVICE_SYSTEM_MGMT_GET_TOC_INFO,
                                              DEFAULT_TIMEOUT))
    {
      if (cpuid == p_svc->resp_toc_entry.resp_cpu)
      {
        memcpy(&toc_info->toc_entry[toc_info->number_of_toc_entries],
               &p_svc->resp_toc_entry,
               sizeof(get_toc_entry_t));
        toc_info->number_of_toc_entries++;
      }
    }
  }

  *error_code = p_svc->resp_error_code;

  return return_code;
}

/**
 * @brief Obtain all TOC data from SE
 *
 *The number of TOC entries found is returned followed by the TOC entry details.
 *SERVICES_NUMBER_OF_TOC_ENTRIES is set to 15 entries.
 *
 * @param services_handle
 * @param toc_info            Details for all TOCs found
 * @param error_code
 * @return
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_get_toc_data (uint32_t services_handle,
                                       SERVICES_toc_data_t *toc_info,
                                       uint32_t *error_code)
{
  uint32_t toc_number = 0;   /* retrieve number of TOCs from SE */
  uint32_t return_code = SERVICE_SUCCESS; /* Service error code */

  /* defend against the application */
  if (toc_info == NULL)
  {
     *error_code = 0;
     return SERVICE_INVALID_PARAMETER;
  }

  return_code = SERVICES_system_get_toc_number(services_handle,
                                               &toc_number,
                                               error_code);
  if (SERVICES_REQ_SUCCESS != return_code)
  {
     *error_code = 0;
    return return_code;
  }

  toc_info->number_of_toc_entries = toc_number;

  get_toc_data_t *p_svc;
  for (uint32_t i = 0; i < toc_number; i++)
  {
    p_svc = (get_toc_data_t *)
      SERVICES_prepare_packet_buffer(sizeof(get_toc_data_t));

    p_svc->send_entry_idx = i;
    if (SERVICE_SUCCESS == SERVICES_send_request(
                                              services_handle,
                                              SERVICE_SYSTEM_MGMT_GET_TOC_INFO,
                                              DEFAULT_TIMEOUT))
    {
      memcpy(&toc_info->toc_entry[i],
             &p_svc->resp_toc_entry,
             sizeof(get_toc_entry_t));
    }
  }

  *error_code = p_svc->resp_error_code;

  return return_code;
}

/**
 * @fn    uint32_t SERVICES_system_get_device_part_number(
 *                                uint32_t services_handle,
                                  uint32_t *device_part_number)
 * @brief Returns the SoC device identifier.
 *
 * device_part_number as integer e.g., 0x0000B200
 *
 * @param[in]   services_handle
 * @param[out]  device_part_number (SocID)
 * @return
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_get_device_part_number(uint32_t services_handle, 
                                                uint32_t *device_part_number,
                                                uint32_t *error_code)
{
  get_device_part_svc_t *p_svc = (get_device_part_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(get_device_part_svc_t));

  uint32_t return_code = SERVICES_send_request(services_handle,
                                       SERVICE_SYSTEM_MGMT_GET_DEVICE_PART_NUMBER, 
                                       DEFAULT_TIMEOUT);

  *device_part_number = p_svc->resp_device_string;
  *error_code = p_svc->resp_error_code;

  return return_code;
}

/**
 * @fn    uint32_t SERVICES_system_set_services_debug (uint32_t services_handle,
 *                                                     uint32_t * error_code)
 * @brief Set debug capability dynamically from SE-UART
 *
 * @param services_handle
 * @param debug_enable              Toggle debug output
 * @param error_code
 * @return
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_set_services_debug (uint32_t services_handle,
                                             bool debug_enable,
                                             uint32_t *error_code)
{
  set_services_capabilities_t *p_svc = (set_services_capabilities_t *)
      SERVICES_prepare_packet_buffer(sizeof(set_services_capabilities_t));

  p_svc->send_services_debug = debug_enable;

  uint32_t return_code = SERVICES_send_request(services_handle,
                                               SERVICE_SYSTEM_MGMT_SET_CAPABILITIES_DEBUG,
                                               DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;

  return return_code;
}

#if 0
/**
 * @brief obtain the OTP data
 * @param services_handle
 * @param toc_info
 * @param error_code
 * @return
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_get_otp_data (uint32_t services_handle,
                                       SERVICES_otp_data_t *device_info,
                                       uint32_t *error_code)
{
  get_otp_data_t *p_svc = (get_otp_data_t *)
      SERVICES_prepare_packet_buffer(sizeof(get_otp_data_t));
  uint32_t return_code;

 return_code = SERVICES_send_request(services_handle,
                                     SERVICE_SYSTEM_MGMT_GET_OTP_INFO,
                                     DEFAULT_TIMEOUT);
 /**
  * @todo
  */
 *error_code = p_svc->resp_error_code;

  UNUSED(device_info);

  return return_code;
}
#endif

/**
 * @fn uint32_t SERVICES_system_get_device_data(uint32_t, SERVICES_version_data_t*, uint32_t*)
 * @brief  Get device data information
 *
 * Retrieves the Device information.
 *
 * @param services_handle
 * @param device_info
 * @param error_code
 * @return
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_get_device_data(uint32_t services_handle,
                                         SERVICES_version_data_t *device_info,
                                         uint32_t *error_code)
{
  get_device_revision_data_t *p_svc = (get_device_revision_data_t *)
      SERVICES_prepare_packet_buffer(sizeof(get_device_revision_data_t));
  uint32_t return_code;

  return_code = SERVICES_send_request(services_handle,
                                      SERVICE_SYSTEM_MGMT_GET_DEVICE_REVISION_DATA,
                                      DEFAULT_TIMEOUT);
  if (SERVICES_REQ_SUCCESS != return_code)
  {
    return return_code;
  }

  /* unpack and return */
  device_info->revision_id = p_svc->revision_id;
  memcpy((uint8_t*)&device_info->SerialN[0], (uint8_t*)p_svc->SerialN,
         sizeof(device_info->SerialN));
  memcpy((uint8_t*)&device_info->ALIF_PN[0], (uint8_t*)p_svc->ALIF_PN,
         sizeof(device_info->ALIF_PN));
  memcpy((uint8_t*)&device_info->HBK0[0], (uint8_t*)p_svc->HBK0,
          sizeof(device_info->HBK0));
  memcpy((uint8_t*)&device_info->DCU[0], (uint8_t*)p_svc->DCU,
         sizeof(device_info->DCU));
  memcpy((uint8_t*)&device_info->config[0], (uint8_t*)p_svc->config,
         sizeof(device_info->config));
  memcpy((uint8_t*)&device_info->HBK1[0], (uint8_t*)p_svc->HBK1,
         sizeof(device_info->HBK1));
  memcpy((uint8_t*)&device_info->HBK_FW[0],(uint8_t*)p_svc->HBK_FW,
         sizeof(device_info->HBK_FW));
  memcpy((uint8_t*)&device_info->MfgData[0],(uint8_t*)p_svc->MfgData,
         sizeof(device_info->MfgData));
  device_info->LCS = p_svc->LCS,

  *error_code = p_svc->resp_error_code;

  return return_code;
}

/**
 * @fn  uint32_t SERVICES_system_read_otp(uint32_t services_handle,
 *                                        uint32_t otp_offset,
 *                                        uint32_t *otp_value_word,
 *                                        uint32_t *error_code)
 * @brief read OTP data from offset
 *
 * @param services_handle
 * @param otp_offset          OTP WORD offset to read
 * @param otp_value_word      OPT value return from offset
 * @param error_code
 * @return SERVICE_SUCCESS
 * @return SERVICE_FAIL
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_read_otp(uint32_t services_handle,
                                  uint32_t otp_offset,
                                  uint32_t *otp_value_word,
                                  uint32_t *error_code)
{
  otp_data_t *p_svc = (otp_data_t *)
      SERVICES_prepare_packet_buffer(sizeof(otp_data_t));
  uint32_t return_code;

 p_svc->send_offset = otp_offset;
 return_code = SERVICES_send_request(services_handle,
                                     SERVICE_SYSTEM_MGMT_READ_OTP,
                                     DEFAULT_TIMEOUT);
 *otp_value_word = p_svc->otp_word;
 *error_code = p_svc->resp_error_code;

  return return_code;
}

/**
 * @fn  uint32_t SERVICES_system_write_otp(uint32_t services_handle,
 *                                         uint32_t otp_offset,
 *                                         uint32_t otp_value_word,
 *                                         uint32_t *error_code)
 * @brief Write OTP data to Word offset
 *
 * @param services_handle
 * @param otp_offset          OTP WORD offset destination
 * @param otp_value_word      OTP value to write to offset
 * @param error_code
 * @return SERVICE_SUCCESS
 * @return SERVICE_FAIL
 * @ingroup services-host-system
 */
uint32_t SERVICES_system_write_otp(uint32_t services_handle,
                                   uint32_t otp_offset,
                                   uint32_t otp_value_word,
                                   uint32_t *error_code)
{
  otp_data_t *p_svc = (otp_data_t *)
      SERVICES_prepare_packet_buffer(sizeof(otp_data_t));
  uint32_t return_code;

 p_svc->send_offset = otp_offset;
 p_svc->otp_word = otp_value_word;
 return_code = SERVICES_send_request(services_handle,
                                     SERVICE_SYSTEM_MGMT_WRITE_OTP,
                                     DEFAULT_TIMEOUT);

 *error_code = p_svc->resp_error_code;
  return return_code;
}

typedef struct
{
  uint8_t x_loc  : 7;
  uint8_t zero_1 : 1;
  uint8_t y_loc  : 7;
  uint8_t zero_2 : 1;
  uint8_t wfr_id : 5;
  uint8_t zero_3 : 2;
  uint8_t fab_id : 1;
  uint8_t year   : 6;
  uint8_t zero_4 : 2;
  uint8_t week   : 6;
  uint8_t zero_5 : 2;
  uint8_t lot_no : 8;
} mfg_data_t;

/**
 * @brief   Pack manufacturing data into 40 bits for EUI-64
 * @param p_data_in
 * @param p_data_out
 */
static void get_eui64_extension(uint8_t *p_data_in, uint8_t *p_data_out)
{
  mfg_data_t *p_mfg_data = (mfg_data_t *)p_data_in;

  uint8_t seven_bits_1 = p_mfg_data->x_loc;
  uint8_t seven_bits_2 = p_mfg_data->y_loc;
  uint8_t six_bits_3   = (p_mfg_data->wfr_id << 1) | p_mfg_data->fab_id;
  uint8_t six_bits_4   = p_mfg_data->year;
  uint8_t six_bits_5   = p_mfg_data->week;
  uint8_t eight_bits   = p_mfg_data->lot_no;

  // x x x x x x x y
  *p_data_out = (seven_bits_1 << 1) | ((seven_bits_2 & 0x40) >> 6);
  p_data_out++;
  // y y y y y y wf wf
  *p_data_out = ((seven_bits_2 & 0x3F) << 2) | ((six_bits_3 & 0x30) >> 4);
  p_data_out++;
  // wf wf wf f yr yr yr yr
  *p_data_out = ((six_bits_3 & 0x0F) << 4) | ((six_bits_4 & 0x3C) >> 2);
  p_data_out++;
  // yr yr wk wk wk wk wk wk
  *p_data_out = ((six_bits_4 & 0x03) << 6) | six_bits_5;
  p_data_out++;
  *p_data_out = eight_bits;

  //memcpy(p_data_out, p_data_in, 8);
}

/**
 * @brief   Pack manufacturing data into 24 bits for EUI-48
 * @param p_data_in
 * @param p_data_out
 */
static void get_eui48_extension(uint8_t * p_data_in, uint8_t * p_data_out)
{
  mfg_data_t *p_mfg_data = (mfg_data_t *)p_data_in;

  uint8_t six_bits_1 = p_mfg_data->x_loc & 0x3F;
  uint8_t six_bits_2 = p_mfg_data->y_loc & 0x3F;
  uint8_t six_bits_3 = (p_mfg_data->wfr_id << 1) | (p_mfg_data->lot_no & 0x1);
  uint8_t six_bits_4 = p_mfg_data->week;

  // x x x x x x y y
  *p_data_out = (six_bits_1 << 2) | ((six_bits_2 & 0x30) >> 4);
  p_data_out++;
  // y y y y wf wf wf wf
  *p_data_out = ((six_bits_2 & 0x0F) << 4) | ((six_bits_3 & 0x3C) >> 2);
  p_data_out++;
  // wf lt wk wk wk wk wk wk
  *p_data_out = ((six_bits_3 & 0x03) << 6) | six_bits_4;
}

/**
 * @brief Calculate unique extension values for EUI-48 or EUI-64
 *
 * @fn    uint32_t SERVICES_system_get_eui_extension(
 *                                uint32_t services_handle,
 *                                bool is_eui48,
 *                                uint8_t *eui_extension,
 *                                uint32_t *error_code)
 *
 * @param services_handle
 * @param is_eui48       Specify whether EUI-48 or EUI-64 extension is requested
 * @param eui_extension  Buffer to store the calculated extension
 * @param error_code
 * @ingroup services-host-system
 * @return SERVICE_INVALID_PARAMETER
 * @return SERVICES_REQ_SUCCESS
 */
uint32_t SERVICES_system_get_eui_extension(uint32_t services_handle,
                                           bool is_eui48,
                                           uint8_t *eui_extension,
                                           uint32_t *error_code)
{
  SERVICES_version_data_t device_data; /* SoC Device information */
  uint32_t return_code = SERVICES_REQ_SUCCESS; /* Service error code  */

  /* defend against the application */
  if (eui_extension == NULL)
  {
    *error_code = 0;
    return SERVICE_INVALID_PARAMETER;
  }

  return_code = SERVICES_system_get_device_data(services_handle,
                                               &device_data,
                                               error_code);
  if (return_code != SERVICES_REQ_SUCCESS)
  {
    return return_code;
  }

  //init_test_mgf_data(device_data.MfgData);

  if (is_eui48)
  {
    get_eui48_extension(device_data.MfgData, eui_extension);
  }
  else
  {
    get_eui64_extension(device_data.MfgData, eui_extension);
  }

  return SERVICES_REQ_SUCCESS;
}

/**
 * @brief Calculate a 64-bit device ID based on the Mfg data and the Alif OID
 *
 * @fn    uint32_t SERVICES_system_get_device_id64(
 *                                uint32_t services_handle,
 *                                uint8_t *device_id,
 *                                uint32_t *error_code)
 *
 * @param services_handle
 * @param device_id          Buffer to store the calculated 64-bit/8-byte device ID
 * @param error_code
 * @ingroup services-host-system
 * @return SERVICE_INVALID_PARAMETER
 * @return SERVICES_REQ_SUCCESS
 */
uint32_t SERVICES_system_get_device_id64(uint32_t services_handle,
                                         uint8_t *device_id,
                                         uint32_t *error_code)
{
  uint32_t return_code = SERVICES_REQ_SUCCESS; /* Service error code  */

  // This will populate the first 5 bytes in the device ID buffer
  return_code = SERVICES_system_get_eui_extension(services_handle,
                                                  false,
                                                  device_id,
                                                  error_code);
  if (return_code != SERVICES_REQ_SUCCESS)
  {
    return return_code;
  }

  // Add the Alif Organizational ID
  memcpy(device_id + 5, s_alif_oid, sizeof(s_alif_oid));

  return SERVICES_REQ_SUCCESS;
}
