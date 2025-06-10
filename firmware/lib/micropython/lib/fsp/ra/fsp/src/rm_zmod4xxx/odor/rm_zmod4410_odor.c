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

/**********************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#if defined(__CCRX__) || defined(__ICCRX__) || defined(__RX__)
 #include "r_zmod4xxx_if.h"
#elif defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL78__)
 #include "r_zmod4xxx_if.h"
#else
 #define RM_ZMOD4410_ODOR_CFG_LIB_ENABLE    (1)
#endif

#if RM_ZMOD4410_ODOR_CFG_LIB_ENABLE
 #include "rm_zmod4xxx.h"
 #include "../zmod4xxx_types.h"
 #include "odor.h"
 #include "zmod4410_config_odor.h"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/

/* Definitions of Odor Parameter */
 #define RM_ZMOD4410_ODOR_PARAM_ALPHA           (0.8F)
 #define RM_ZMOD4410_ODOR_PARAM_STOP_DELAY      (60)
 #define RM_ZMOD4410_ODOR_PARAM_THRESHOLD       (1.3F)
 #define RM_ZMOD4410_ODOR_PARAM_TAU             (1800)
 #define RM_ZMOD4410_ODOR_PARAM_STAB_SAMPLES    (15)

/* Definitions of Calculation */
 #define RM_ZMOD4410_ODOR_1E_3                  (1e-3)
 #define RM_ZMOD4410_ODOR_10E9                  (10e9)
 #define RM_ZMOD4410_ODOR_1E3                   (1e3)

/**********************************************************************************************************************
 * Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private (static) variables and functions
 *********************************************************************************************************************/

static fsp_err_t rm_zmod4410_odor_open(rm_zmod4xxx_ctrl_t * const p_api_ctrl, rm_zmod4xxx_cfg_t const * const p_cfg);
static fsp_err_t rm_zmod4410_odor_measurement_start(rm_zmod4xxx_ctrl_t * const p_api_ctrl);
static fsp_err_t rm_zmod4410_odor_measurement_stop(rm_zmod4xxx_ctrl_t * const p_api_ctrl);
static fsp_err_t rm_zmod4410_odor_status_check(rm_zmod4xxx_ctrl_t * const p_api_ctrl);
static fsp_err_t rm_zmod4410_odor_read(rm_zmod4xxx_ctrl_t * const     p_api_ctrl,
                                       rm_zmod4xxx_raw_data_t * const p_raw_data);
static fsp_err_t rm_zmod4410_odor_temperature_and_humidity_set(rm_zmod4xxx_ctrl_t * const p_api_ctrl,
                                                               float                      temperature,
                                                               float                      humidity);
static fsp_err_t rm_zmod4410_odor_iaq_1st_gen_data_calculate(rm_zmod4xxx_ctrl_t * const         p_api_ctrl,
                                                             rm_zmod4xxx_raw_data_t * const     p_raw_data,
                                                             rm_zmod4xxx_iaq_1st_data_t * const p_zmod4xxx_data);
static fsp_err_t rm_zmod4410_odor_iaq_2nd_gen_data_calculate(rm_zmod4xxx_ctrl_t * const         p_api_ctrl,
                                                             rm_zmod4xxx_raw_data_t * const     p_raw_data,
                                                             rm_zmod4xxx_iaq_2nd_data_t * const p_zmod4xxx_data);
static fsp_err_t rm_zmod4410_odor_data_calculate(rm_zmod4xxx_ctrl_t * const      p_api_ctrl,
                                                 rm_zmod4xxx_raw_data_t * const  p_raw_data,
                                                 rm_zmod4xxx_odor_data_t * const p_zmod4xxx_data);
static fsp_err_t rm_zmod4410_odor_sulfur_odor_data_calculate(rm_zmod4xxx_ctrl_t * const             p_api_ctrl,
                                                             rm_zmod4xxx_raw_data_t * const         p_raw_data,
                                                             rm_zmod4xxx_sulfur_odor_data_t * const p_zmod4xxx_data);
static fsp_err_t rm_zmod4410_odor_oaq_1st_gen_data_calculate(rm_zmod4xxx_ctrl_t * const         p_api_ctrl,
                                                             rm_zmod4xxx_raw_data_t * const     p_raw_data,
                                                             rm_zmod4xxx_oaq_1st_data_t * const p_zmod4xxx_data);
static fsp_err_t rm_zmod4410_odor_oaq_2nd_gen_data_calculate(rm_zmod4xxx_ctrl_t * const         p_api_ctrl,
                                                             rm_zmod4xxx_raw_data_t * const     p_raw_data,
                                                             rm_zmod4xxx_oaq_2nd_data_t * const p_zmod4xxx_data);
static fsp_err_t rm_zmod4410_odor_raq_data_calculate(rm_zmod4xxx_ctrl_t * const     p_api_ctrl,
                                                     rm_zmod4xxx_raw_data_t * const p_raw_data,
                                                     rm_zmod4xxx_raq_data_t * const p_zmod4xxx_data);
static fsp_err_t rm_zmod4410_odor_close(rm_zmod4xxx_ctrl_t * const p_api_ctrl);
static fsp_err_t rm_zmod4410_odor_device_error_check(rm_zmod4xxx_ctrl_t * const p_api_ctrl);
static fsp_err_t rm_zmod4410_odor_calc_rmox(rm_zmod4xxx_instance_ctrl_t * const p_ctrl,
                                            uint8_t                           * adc_result,
                                            float                             * rmox);

/**********************************************************************************************************************
 * Exported global variables
 *********************************************************************************************************************/
rm_zmod4xxx_api_t const g_zmod4xxx_on_zmod4410_odor =
{
    .open                      = rm_zmod4410_odor_open,
    .close                     = rm_zmod4410_odor_close,
    .measurementStart          = rm_zmod4410_odor_measurement_start,
    .measurementStop           = rm_zmod4410_odor_measurement_stop,
    .statusCheck               = rm_zmod4410_odor_status_check,
    .read                      = rm_zmod4410_odor_read,
    .iaq1stGenDataCalculate    = rm_zmod4410_odor_iaq_1st_gen_data_calculate,
    .iaq2ndGenDataCalculate    = rm_zmod4410_odor_iaq_2nd_gen_data_calculate,
    .odorDataCalculate         = rm_zmod4410_odor_data_calculate,
    .sulfurOdorDataCalculate   = rm_zmod4410_odor_sulfur_odor_data_calculate,
    .oaq1stGenDataCalculate    = rm_zmod4410_odor_oaq_1st_gen_data_calculate,
    .oaq2ndGenDataCalculate    = rm_zmod4410_odor_oaq_2nd_gen_data_calculate,
    .raqDataCalculate          = rm_zmod4410_odor_raq_data_calculate,
    .temperatureAndHumiditySet = rm_zmod4410_odor_temperature_and_humidity_set,
    .deviceErrorCheck          = rm_zmod4410_odor_device_error_check,
};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Initialize the Odor library
 *
 * @retval FSP_SUCCESS              Successfully started.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_open (rm_zmod4xxx_ctrl_t * const p_api_ctrl, rm_zmod4xxx_cfg_t const * const p_cfg)
{
    rm_zmod4xxx_instance_ctrl_t    * p_ctrl = (rm_zmod4xxx_instance_ctrl_t *) p_api_ctrl;
    rm_zmod4xxx_lib_extended_cfg_t * p_lib  = p_ctrl->p_zmod4xxx_lib;
    odor_params * p_handle = (odor_params *) p_lib->p_handle;

    FSP_PARAMETER_NOT_USED(p_cfg);

/* Initialize the library */
    p_handle->alpha                 = RM_ZMOD4410_ODOR_PARAM_ALPHA;
    p_handle->stop_delay            = RM_ZMOD4410_ODOR_PARAM_STOP_DELAY;
    p_handle->threshold             = RM_ZMOD4410_ODOR_PARAM_THRESHOLD;
    p_handle->tau                   = RM_ZMOD4410_ODOR_PARAM_TAU;
    p_handle->stabilization_samples = RM_ZMOD4410_ODOR_PARAM_STAB_SAMPLES;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Calculate the Odor data with the library API.
 *
 * @retval FSP_SUCCESS                            Successfully results are read.
 * @retval FSP_ERR_ASSERTION                      Null pointer passed as a parameter.
 * @retval FSP_ERR_SENSOR_IN_STABILIZATION        Module is stabilizing.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_data_calculate (rm_zmod4xxx_ctrl_t * const      p_api_ctrl,
                                                  rm_zmod4xxx_raw_data_t * const  p_raw_data,
                                                  rm_zmod4xxx_odor_data_t * const p_zmod4xxx_data)
{
    rm_zmod4xxx_instance_ctrl_t * p_ctrl = (rm_zmod4xxx_instance_ctrl_t *) p_api_ctrl;
    float rmox = 0.0F;
    rm_zmod4xxx_lib_extended_cfg_t * p_lib = p_ctrl->p_zmod4xxx_lib;
    odor_params    * p_handle              = (odor_params *) p_lib->p_handle;
    odor_results_t * p_results             = (odor_results_t *) p_lib->p_results;
    int8_t           lib_err               = 0;

    /* Calculate Odor data form ADC data */
    rm_zmod4410_odor_calc_rmox(p_ctrl, &p_raw_data->adc_data[0], &rmox);
    lib_err = calc_odor(rmox, p_handle, p_results);
    FSP_ERROR_RETURN(0 <= lib_err, FSP_ERR_ASSERTION);

    /* Set Data */
    p_zmod4xxx_data->odor = p_results->conc_ratio;
    if (ON == p_results->cs_state)
    {
        p_zmod4xxx_data->control_signal = true;
    }
    else
    {
        p_zmod4xxx_data->control_signal = false;
    }

    FSP_ERROR_RETURN(ODOR_STABILIZATION != lib_err, FSP_ERR_SENSOR_IN_STABILIZATION);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_close (rm_zmod4xxx_ctrl_t * const p_api_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_measurement_start (rm_zmod4xxx_ctrl_t * const p_api_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_measurement_stop (rm_zmod4xxx_ctrl_t * const p_api_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_status_check (rm_zmod4xxx_ctrl_t * const p_api_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_read (rm_zmod4xxx_ctrl_t * const     p_api_ctrl,
                                        rm_zmod4xxx_raw_data_t * const p_raw_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_temperature_and_humidity_set (rm_zmod4xxx_ctrl_t * const p_api_ctrl,
                                                                float                      temperature,
                                                                float                      humidity)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(temperature);
    FSP_PARAMETER_NOT_USED(humidity);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_iaq_1st_gen_data_calculate (rm_zmod4xxx_ctrl_t * const         p_api_ctrl,
                                                              rm_zmod4xxx_raw_data_t * const     p_raw_data,
                                                              rm_zmod4xxx_iaq_1st_data_t * const p_zmod4xxx_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(p_zmod4xxx_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_iaq_2nd_gen_data_calculate (rm_zmod4xxx_ctrl_t * const         p_api_ctrl,
                                                              rm_zmod4xxx_raw_data_t * const     p_raw_data,
                                                              rm_zmod4xxx_iaq_2nd_data_t * const p_zmod4xxx_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(p_zmod4xxx_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_sulfur_odor_data_calculate (rm_zmod4xxx_ctrl_t * const             p_api_ctrl,
                                                              rm_zmod4xxx_raw_data_t * const         p_raw_data,
                                                              rm_zmod4xxx_sulfur_odor_data_t * const p_zmod4xxx_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(p_zmod4xxx_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_oaq_1st_gen_data_calculate (rm_zmod4xxx_ctrl_t * const         p_api_ctrl,
                                                              rm_zmod4xxx_raw_data_t * const     p_raw_data,
                                                              rm_zmod4xxx_oaq_1st_data_t * const p_zmod4xxx_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(p_zmod4xxx_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_oaq_2nd_gen_data_calculate (rm_zmod4xxx_ctrl_t * const         p_api_ctrl,
                                                              rm_zmod4xxx_raw_data_t * const     p_raw_data,
                                                              rm_zmod4xxx_oaq_2nd_data_t * const p_zmod4xxx_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(p_zmod4xxx_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_raq_data_calculate (rm_zmod4xxx_ctrl_t * const     p_api_ctrl,
                                                      rm_zmod4xxx_raw_data_t * const p_raw_data,
                                                      rm_zmod4xxx_raq_data_t * const p_zmod4xxx_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(p_zmod4xxx_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_device_error_check (rm_zmod4xxx_ctrl_t * const p_api_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief Calculate mox resistance.
 *
 * @retval FSP_SUCCESS              successfully configured.
 **********************************************************************************************************************/
static fsp_err_t rm_zmod4410_odor_calc_rmox (rm_zmod4xxx_instance_ctrl_t * const p_ctrl,
                                             uint8_t                           * adc_result,
                                             float                             * rmox)
{
    rm_zmod4xxx_lib_extended_cfg_t * p_lib             = p_ctrl->p_zmod4xxx_lib;
    zmod4xxx_dev_t                 * p_zmod4xxx_device = (zmod4xxx_dev_t *) p_lib->p_device;
    uint8_t  i;
    uint16_t adc_value  = 0;
    float  * p          = rmox;
    float    rmox_local = 0;

    i = 0;
    while (i < p_zmod4xxx_device->meas_conf->r.len)
    {
        adc_value = (uint16_t) ((((uint16_t) adc_result[i]) << 8) | adc_result[i + 1]);
        if (0.0 > (adc_value - p_zmod4xxx_device->mox_lr))
        {
            *p = (float) RM_ZMOD4410_ODOR_1E_3;
            p++;
        }
        else if (0.0 >= (p_zmod4xxx_device->mox_er - adc_value))
        {
            *p = (float) RM_ZMOD4410_ODOR_10E9;
            p++;
        }
        else
        {
            rmox_local = (float) p_zmod4xxx_device->config[0] * (float) RM_ZMOD4410_ODOR_1E3 *
                         (float) (adc_value - p_zmod4xxx_device->mox_lr) /
                         (float) (p_zmod4xxx_device->mox_er - adc_value);
            *p = rmox_local;
            p++;
        }

        i = (uint8_t) (i + 2);
    }

    return FSP_SUCCESS;
}

#endif
