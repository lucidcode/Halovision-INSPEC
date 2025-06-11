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
 #include "r_ob1203_if.h"
#elif defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL78__)
 #include "r_ob1203_if.h"
#else
 #define RM_OB1203_PPG_MODE_ENABLE    (1)
#endif

#if RM_OB1203_PPG_MODE_ENABLE
 #include "rm_ob1203.h"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/

/* Definitions of Command */
 #define RM_OB1203_COMMAND_MEASUREMENT_START     (0x01)

/* Definitions of Register address */
 #define RM_OB1203_REG_ADDR_PPG_IRLED_CURR_0     (0x30) // PPG IR LED (LED1) current, LSB
 #define RM_OB1203_REG_ADDR_PPG_CAN_ANA          (0x34) // PPG analog cancellation value
 #define RM_OB1203_REG_ADDR_PPG_AVG              (0x35) // Number of averaged PPG samples
 #define RM_OB1203_REG_ADDR_PPG_PWIDTH_PERIOD    (0x36) // PPG pulse width and measurement period
 #define RM_OB1203_REG_ADDR_FIFO_CFG             (0x37) // FIFO rollover and almost full configuration
 #define RM_OB1203_REG_ADDR_FIFO_WR_PTR          (0x38) // FIFO write pointer
 #define RM_OB1203_REG_ADDR_FIFO_DATA            (0x3B) // FIFO mirrored PPG data
 #define RM_OB1203_REG_ADDR_DIG_LED1_TRIM        (0x42) // Digital trimming for LED1 (IR LED)

/* Definitions of Mask */
 #define RM_OB1203_MASK_IR_LED_CURRENT           (0x03FF)
 #define RM_OB1203_MASK_RED_LED_CURRENT          (0x01FF)
 #define RM_OB1203_MASK_8BITS                    (0x00FF)
 #define RM_OB1203_MASK_FIFO_AFULL               (0x0F)
 #define RM_OB1203_MASK_FIFO_DATA                (0x03)

/* Definitions of Calculation */
 #define RM_OB1203_REG_DATA_PPG_AVG              (0x0A)
 #define RM_OB1203_MAX_NUM_OF_SAMPLES            (32)
 #define RM_OB1203_MAX_NUM_OF_SAMPLE_BYTES       (RM_OB1203_MAX_NUM_OF_SAMPLES * 3) // One sample is 3 bytes

/**********************************************************************************************************************
 * Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global functions
 *********************************************************************************************************************/
extern fsp_err_t rm_ob1203_read(rm_ob1203_ctrl_t * const p_api_ctrl, rm_comms_write_read_params_t write_read_params);
extern fsp_err_t rm_ob1203_write(rm_ob1203_ctrl_t * const p_api_ctrl, uint8_t * const p_src, uint8_t const bytes);
extern fsp_err_t rm_ob1203_main_ctrl_register_write(rm_ob1203_ctrl_t * const p_api_ctrl,
                                                    uint8_t const            main_ctrl_0,
                                                    uint8_t const            main_ctrl_1);
extern fsp_err_t rm_ob1203_int_cfg_register_write(rm_ob1203_ctrl_t * const p_api_ctrl,
                                                  uint8_t const            int_cfg_0,
                                                  uint8_t const            int_cfg_1,
                                                  uint8_t const            int_pst);
extern fsp_err_t rm_ob1203_ppg_ps_gain_register_write(rm_ob1203_ctrl_t * const p_api_ctrl, uint8_t const ppg_ps_gain);
extern fsp_err_t rm_ob1203_ppg_ps_cfg_register_write(rm_ob1203_ctrl_t * const p_api_ctrl, uint8_t const ppg_ps_cfg);
extern fsp_err_t rm_ob1203_fifo_info_reset(rm_ob1203_ctrl_t * const p_api_ctrl);

 #if BSP_CFG_RTOS
extern fsp_err_t rm_ob1203_os_semaphore_acquire(rm_ob1203_semaphore_t const * p_semaphore, uint32_t const timeout);

 #endif

/**********************************************************************************************************************
 * Private (static) variables and functions
 *********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_open(rm_ob1203_ctrl_t * const p_api_ctrl, rm_ob1203_cfg_t const * const p_cfg);
static fsp_err_t rm_ob1203_ppg_close(rm_ob1203_ctrl_t * const p_api_ctrl);
static fsp_err_t rm_ob1203_ppg_measurement_start(rm_ob1203_ctrl_t * const p_api_ctrl);
static fsp_err_t rm_ob1203_ppg_measurement_stop(rm_ob1203_ctrl_t * const p_api_ctrl);
static fsp_err_t rm_ob1203_ppg_light_read(rm_ob1203_ctrl_t * const     p_api_ctrl,
                                          rm_ob1203_raw_data_t * const p_raw_data,
                                          rm_ob1203_light_data_type_t  type);
static fsp_err_t rm_ob1203_ppg_light_data_calculate(rm_ob1203_ctrl_t * const       p_api_ctrl,
                                                    rm_ob1203_raw_data_t * const   p_raw_data,
                                                    rm_ob1203_light_data_t * const p_ob1203_data);
static fsp_err_t rm_ob1203_ppg_prox_read(rm_ob1203_ctrl_t * const p_api_ctrl, rm_ob1203_raw_data_t * const p_raw_data);
static fsp_err_t rm_ob1203_ppg_prox_data_calculate(rm_ob1203_ctrl_t * const      p_api_ctrl,
                                                   rm_ob1203_raw_data_t * const  p_raw_data,
                                                   rm_ob1203_prox_data_t * const p_ob1203_data);
static fsp_err_t rm_ob1203_ppg_read(rm_ob1203_ctrl_t * const     p_api_ctrl,
                                    rm_ob1203_raw_data_t * const p_raw_data,
                                    uint8_t const                number_of_samples);
static fsp_err_t rm_ob1203_ppg_data_calculate(rm_ob1203_ctrl_t * const     p_api_ctrl,
                                              rm_ob1203_raw_data_t * const p_raw_data,
                                              rm_ob1203_ppg_data_t * const p_ob1203_data);
static fsp_err_t rm_ob1203_ppg_gain_set(rm_ob1203_ctrl_t * const p_api_ctrl, rm_ob1203_gain_t const gain);
static fsp_err_t rm_ob1203_ppg_led_current_set(rm_ob1203_ctrl_t * const      p_api_ctrl,
                                               rm_ob1203_led_current_t const led_current);
static fsp_err_t rm_ob1203_ppg_device_interrupt_cfg_set(rm_ob1203_ctrl_t * const               p_api_ctrl,
                                                        rm_ob1203_device_interrupt_cfg_t const interrupt_cfg);
static fsp_err_t rm_ob1203_ppg_fifo_info_get(rm_ob1203_ctrl_t * const      p_api_ctrl,
                                             rm_ob1203_fifo_info_t * const p_fifo_info);
static fsp_err_t rm_ob1203_ppg_device_status_get(rm_ob1203_ctrl_t * const          p_api_ctrl,
                                                 rm_ob1203_device_status_t * const p_status);

/**********************************************************************************************************************
 * Exported global variables
 *********************************************************************************************************************/
rm_ob1203_api_t const g_ob1203_on_ob1203_ppg =
{
    .open                  = rm_ob1203_ppg_open,
    .close                 = rm_ob1203_ppg_close,
    .measurementStart      = rm_ob1203_ppg_measurement_start,
    .measurementStop       = rm_ob1203_ppg_measurement_stop,
    .lightRead             = rm_ob1203_ppg_light_read,
    .lightDataCalculate    = rm_ob1203_ppg_light_data_calculate,
    .proxRead              = rm_ob1203_ppg_prox_read,
    .proxDataCalculate     = rm_ob1203_ppg_prox_data_calculate,
    .ppgRead               = rm_ob1203_ppg_read,
    .ppgDataCalculate      = rm_ob1203_ppg_data_calculate,
    .gainSet               = rm_ob1203_ppg_gain_set,
    .ledCurrentSet         = rm_ob1203_ppg_led_current_set,
    .deviceInterruptCfgSet = rm_ob1203_ppg_device_interrupt_cfg_set,
    .fifoInfoGet           = rm_ob1203_ppg_fifo_info_get,
    .deviceStatusGet       = rm_ob1203_ppg_device_status_get,
};

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Configures the OB1203 PPG mode.
 *
 * @retval FSP_SUCCESS              OB1203 successfully configured.
 * @retval FSP_ERR_TIMEOUT          Communication is timeout.
 * @retval FSP_ERR_ABORTED          Communication is aborted.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_open (rm_ob1203_ctrl_t * const p_api_ctrl, rm_ob1203_cfg_t const * const p_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;
    uint16_t current_tmp;
    uint8_t  int_cfg_1 = 0x00;

    FSP_PARAMETER_NOT_USED(p_cfg);

    /* Set gain */
    err = rm_ob1203_ppg_ps_gain_register_write(p_ctrl, (uint8_t) p_ctrl->p_mode->ppg_prox_gain);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Set LED current */
    p_ctrl->buf[0] = RM_OB1203_REG_ADDR_PPG_IRLED_CURR_0;
    current_tmp    = p_ctrl->p_mode->ppg_ir_led_current & RM_OB1203_MASK_IR_LED_CURRENT;
    p_ctrl->buf[1] = (uint8_t) (current_tmp & RM_OB1203_MASK_8BITS);
    p_ctrl->buf[2] = (uint8_t) (current_tmp >> 8);
    current_tmp    = p_ctrl->p_mode->ppg_red_led_current & RM_OB1203_MASK_RED_LED_CURRENT;
    p_ctrl->buf[3] = (uint8_t) (current_tmp & RM_OB1203_MASK_8BITS);
    p_ctrl->buf[4] = (uint8_t) (current_tmp >> 8);
    err            = rm_ob1203_write(p_ctrl, &p_ctrl->buf[0], 5);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Set power save mode and LED order */
    err =
        rm_ob1203_ppg_ps_cfg_register_write(p_ctrl,
                                            (uint8_t) ((uint8_t) p_ctrl->p_mode->ppg_power_save_mode |
                                                       (uint8_t) p_ctrl->p_mode->led_order));
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Set analog cancellations */
    p_ctrl->buf[0] = RM_OB1203_REG_ADDR_PPG_CAN_ANA;
    p_ctrl->buf[1] =
        (uint8_t) ((uint8_t) (p_ctrl->p_mode->ppg_ir_led_ana_can << 2) | (uint8_t) p_ctrl->p_mode->ppg_red_led_ana_can);
    err = rm_ob1203_write(p_ctrl, &p_ctrl->buf[0], 2);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Set number of averaged samples */
    p_ctrl->buf[0] = RM_OB1203_REG_ADDR_PPG_AVG;
    p_ctrl->buf[1] = (uint8_t) ((uint8_t) p_ctrl->p_mode->ppg_num_averaged_samples | RM_OB1203_REG_DATA_PPG_AVG);
    err            = rm_ob1203_write(p_ctrl, &p_ctrl->buf[0], 2);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Set pulse width and measurement period */
    p_ctrl->buf[0] = RM_OB1203_REG_ADDR_PPG_PWIDTH_PERIOD;
    p_ctrl->buf[1] = (uint8_t) p_ctrl->p_mode->ppg_width_period;
    err            = rm_ob1203_write(p_ctrl, &p_ctrl->buf[0], 2);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Set FIFO configuration */
    p_ctrl->buf[0] = RM_OB1203_REG_ADDR_FIFO_CFG;
    p_ctrl->buf[1] =
        (uint8_t) ((uint8_t) p_ctrl->p_mode->ppg_fifo_rollover |
                   (uint8_t) (p_ctrl->p_mode->ppg_fifo_empty_num & RM_OB1203_MASK_FIFO_AFULL));
    err = rm_ob1203_write(p_ctrl, &p_ctrl->buf[0], 2);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Reset FIFO information */
    p_ctrl->buf[0] = RM_OB1203_REG_ADDR_FIFO_WR_PTR;
    p_ctrl->buf[1] = 0x00;
    p_ctrl->buf[2] = 0x00;
    p_ctrl->buf[3] = 0x00;
    err            = rm_ob1203_write(p_ctrl, &p_ctrl->buf[0], 4);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Set device interrupt configurations */
    if (NULL != p_ctrl->p_irq_instance)
    {
        p_ctrl->p_mode->mode_irq = RM_OB1203_OPERATION_MODE_PPG;
        int_cfg_1                = (uint8_t) p_ctrl->p_mode->ppg_interrupt_type;
    }
    else
    {
        p_ctrl->p_mode->mode_irq = RM_OB1203_OPERATION_MODE_STANDBY;
        int_cfg_1                = 0;
    }

    err = rm_ob1203_int_cfg_register_write(p_ctrl, 0x00, int_cfg_1, 0x00);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Set LED digital trimming */
    p_ctrl->buf[0] = RM_OB1203_REG_ADDR_DIG_LED1_TRIM;
    p_ctrl->buf[1] = 0x00;
    p_ctrl->buf[2] = 0x00;
    err            = rm_ob1203_write(p_ctrl, &p_ctrl->buf[0], 3);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_close (rm_ob1203_ctrl_t * const p_api_ctrl)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Start a periodic measurement
 *
 * @retval FSP_SUCCESS              OB1203 successfully configured.
 * @retval FSP_ERR_TIMEOUT          Communication is timeout.
 * @retval FSP_ERR_ABORTED          Communication is aborted.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_measurement_start (rm_ob1203_ctrl_t * const p_api_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;
    uint8_t main_ctrl_1;

    /* Start measurement */
    main_ctrl_1 = (uint8_t) ((uint8_t) p_ctrl->p_mode->ppg_sensor_mode | RM_OB1203_COMMAND_MEASUREMENT_START);
    err         = rm_ob1203_main_ctrl_register_write(p_ctrl, 0x00, main_ctrl_1);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Stop a periodic measurement
 *
 * @retval FSP_SUCCESS              OB1203 successfully configured.
 * @retval FSP_ERR_TIMEOUT          Communication is timeout.
 * @retval FSP_ERR_ABORTED          Communication is aborted.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_measurement_stop (rm_ob1203_ctrl_t * const p_api_ctrl)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;

    /* Set flag to reset FIFO info. after measurement stop */
    p_ctrl->fifo_reset = true;

    /* Stop measurement */
    err = rm_ob1203_main_ctrl_register_write(p_ctrl, 0x00, 0x00);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

 #if BSP_CFG_RTOS

    /* Acquire a semaphore */
    err = rm_ob1203_os_semaphore_acquire(p_ctrl->p_semaphore, p_ctrl->p_cfg->semaphore_timeout);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    /* Reset FIFO info. */
    err = rm_ob1203_fifo_info_reset(p_ctrl);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);
 #endif

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_light_read (rm_ob1203_ctrl_t * const     p_api_ctrl,
                                           rm_ob1203_raw_data_t * const p_raw_data,
                                           rm_ob1203_light_data_type_t  type)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(type);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_light_data_calculate (rm_ob1203_ctrl_t * const       p_api_ctrl,
                                                     rm_ob1203_raw_data_t * const   p_raw_data,
                                                     rm_ob1203_light_data_t * const p_ob1203_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(p_ob1203_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_prox_read (rm_ob1203_ctrl_t * const p_api_ctrl, rm_ob1203_raw_data_t * const p_raw_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Calculate proximity data from raw data.
 *
 * @retval FSP_SUCCESS              Successfully results are read.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_prox_data_calculate (rm_ob1203_ctrl_t * const      p_api_ctrl,
                                                    rm_ob1203_raw_data_t * const  p_raw_data,
                                                    rm_ob1203_prox_data_t * const p_ob1203_data)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_raw_data);
    FSP_PARAMETER_NOT_USED(p_ob1203_data);

    return FSP_ERR_UNSUPPORTED;
}

/*******************************************************************************************************************//**
 * @brief  Reads PPG ADC data from OB1203 device. One sample requires three bytes. 0 cannot set to the number of samples.
 *
 * @retval FSP_SUCCESS              Successfully results are read.
 * @retval FSP_ERR_TIMEOUT          Communication is timeout.
 * @retval FSP_ERR_ABORTED          Communication is aborted.
 * @retval FSP_ERR_INVALID_ARGUMENT Invalid number.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_read (rm_ob1203_ctrl_t * const     p_api_ctrl,
                                     rm_ob1203_raw_data_t * const p_raw_data,
                                     uint8_t const                number_of_samples)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;
    uint8_t bytes;
    rm_comms_write_read_params_t write_read_params;

 #if RM_OB1203_CFG_PARAM_CHECKING_ENABLE
    FSP_ERROR_RETURN(0 != number_of_samples, FSP_ERR_INVALID_ARGUMENT);
 #endif

    /* Set number of samples and bytes (one PPG sample is 3 bytes). */
    if (RM_OB1203_MAX_NUM_OF_SAMPLES > number_of_samples)
    {
        p_ctrl->p_mode->ppg_number_of_samples = number_of_samples;
        bytes = number_of_samples * 3;
    }
    else
    {
        p_ctrl->p_mode->ppg_number_of_samples = RM_OB1203_MAX_NUM_OF_SAMPLES;
        bytes = RM_OB1203_MAX_NUM_OF_SAMPLE_BYTES;
    }

    /* Read PPG FIFO data. */
    p_ctrl->register_address     = RM_OB1203_REG_ADDR_FIFO_DATA;
    write_read_params.p_src      = &p_ctrl->register_address;
    write_read_params.src_bytes  = 1;
    write_read_params.p_dest     = (uint8_t *) p_raw_data;
    write_read_params.dest_bytes = bytes;
    err = rm_ob1203_read(p_ctrl, write_read_params);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Calculate PPG data from raw data.
 *
 * @retval FSP_SUCCESS              Successfully results are read.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_data_calculate (rm_ob1203_ctrl_t * const     p_api_ctrl,
                                               rm_ob1203_raw_data_t * const p_raw_data,
                                               rm_ob1203_ppg_data_t * const p_ob1203_data)
{
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;
    uint8_t i = 0;

    /* Calculate PPG data */
    for (i = 0; i < p_ctrl->p_mode->ppg_number_of_samples; i++)
    {
        p_ob1203_data->ppg_data[i] =
            (uint32_t) (((p_raw_data->adc_data[3 * i + 2] & RM_OB1203_MASK_FIFO_DATA) << 16) +
                        (p_raw_data->adc_data[3 * i + 1] << 8) + p_raw_data->adc_data[3 * i]);
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Set gain.
 *
 * @retval FSP_ERR_TIMEOUT          Communication is timeout.
 * @retval FSP_ERR_ABORTED          Communication is aborted.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_gain_set (rm_ob1203_ctrl_t * const p_api_ctrl, rm_ob1203_gain_t const gain)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;

    /* Set gain */
    p_ctrl->p_mode->ppg_prox_gain = gain.ppg_prox;
    err = rm_ob1203_ppg_ps_gain_register_write(p_ctrl, (uint8_t) p_ctrl->p_mode->ppg_prox_gain);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Set current.
 *
 * @retval FSP_ERR_TIMEOUT          Communication is timeout.
 * @retval FSP_ERR_ABORTED          Communication is aborted.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_led_current_set (rm_ob1203_ctrl_t * const      p_api_ctrl,
                                                rm_ob1203_led_current_t const led_current)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;
    uint16_t current_tmp;

    /* Set LED current */
    p_ctrl->p_mode->ppg_ir_led_current  = led_current.ir_led;
    p_ctrl->p_mode->ppg_red_led_current = led_current.red_led;
    p_ctrl->buf[0] = RM_OB1203_REG_ADDR_PPG_IRLED_CURR_0;
    current_tmp    = p_ctrl->p_mode->ppg_ir_led_current & RM_OB1203_MASK_IR_LED_CURRENT;
    p_ctrl->buf[1] = (uint8_t) (current_tmp & RM_OB1203_MASK_8BITS);
    p_ctrl->buf[2] = (uint8_t) (current_tmp >> 8);
    current_tmp    = p_ctrl->p_mode->ppg_red_led_current & RM_OB1203_MASK_RED_LED_CURRENT;
    p_ctrl->buf[3] = (uint8_t) (current_tmp & RM_OB1203_MASK_8BITS);
    p_ctrl->buf[4] = (uint8_t) (current_tmp >> 8);
    err            = rm_ob1203_write(p_ctrl, &p_ctrl->buf[0], 5);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Set device interrupt configurations.
 *
 * @retval FSP_ERR_TIMEOUT          Communication is timeout.
 * @retval FSP_ERR_ABORTED          Communication is aborted.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_device_interrupt_cfg_set (rm_ob1203_ctrl_t * const               p_api_ctrl,
                                                         rm_ob1203_device_interrupt_cfg_t const interrupt_cfg)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;

    /* Set device interrupt configurations */
    p_ctrl->p_mode->mode_irq           = RM_OB1203_OPERATION_MODE_PPG;
    p_ctrl->p_mode->ppg_interrupt_type = interrupt_cfg.ppg_type;

    err = rm_ob1203_int_cfg_register_write(p_ctrl, 0x00, (uint8_t) p_ctrl->p_mode->ppg_interrupt_type, 0x00);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Get FIFO information.
 *
 * @retval FSP_ERR_TIMEOUT          Communication is timeout.
 * @retval FSP_ERR_ABORTED          Communication is aborted.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_fifo_info_get (rm_ob1203_ctrl_t * const      p_api_ctrl,
                                              rm_ob1203_fifo_info_t * const p_fifo_info)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_ob1203_instance_ctrl_t  * p_ctrl = (rm_ob1203_instance_ctrl_t *) p_api_ctrl;
    rm_comms_write_read_params_t write_read_params;

    p_ctrl->p_fifo_info          = p_fifo_info;
    p_ctrl->register_address     = RM_OB1203_REG_ADDR_FIFO_WR_PTR;
    write_read_params.p_src      = &p_ctrl->register_address;
    write_read_params.src_bytes  = 1;
    write_read_params.p_dest     = &p_ctrl->buf[0];
    write_read_params.dest_bytes = 3;
    err = rm_ob1203_read(p_ctrl, write_read_params);
    FSP_ERROR_RETURN(FSP_SUCCESS == err, err);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief  Unsupported API.
 *
 * @retval FSP_ERR_UNSUPPORTED                    Operation mode is not supported.
 **********************************************************************************************************************/
static fsp_err_t rm_ob1203_ppg_device_status_get (rm_ob1203_ctrl_t * const          p_api_ctrl,
                                                  rm_ob1203_device_status_t * const p_status)
{
    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_status);

    return FSP_ERR_UNSUPPORTED;
}

#endif
