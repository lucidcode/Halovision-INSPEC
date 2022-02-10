/**************************************************************************//**
 * @file     adc.c
 * @version  V3.00
 * $Revision: 6 $
 * $Date: 16/10/25 4:25p $
 * @brief    M051 series ADC driver source file
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NUC126.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup ADC_Driver ADC Driver
  @{
*/

/** @addtogroup ADC_EXPORTED_FUNCTIONS ADC Exported Functions
  @{
*/

/**
  * @brief This API configures ADC module to be ready for convert the input from selected channel
  * @param[in] adc The pointer of the specified ADC module
  * @param[in] u32InputMode Decides the ADC analog input mode. Valid values are:
  *                          - \ref ADC_ADCR_DIFFEN_SINGLE_END      :Single end input mode
  *                          - \ref ADC_ADCR_DIFFEN_DIFFERENTIAL    :Differential input type
  * @param[in] u32OpMode Decides the ADC operation mode. Valid values are:
  *                       - \ref ADC_ADCR_ADMD_SINGLE               :Single mode.
  *                       - \ref ADC_ADCR_ADMD_BURST                :Burst mode.
  *                       - \ref ADC_ADCR_ADMD_SINGLE_CYCLE         :Single cycle scan mode.
  *                       - \ref ADC_ADCR_ADMD_CONTINUOUS           :Continuous scan mode.
  * @param[in] u32ChMask Channel enable bit. Each bit corresponds to a input channel. Bit 0 is channel 0, bit 1 is channel 1..., bit 7 is channel 7.
  * @return  None
  * @note M051 series MCU ADC can only convert 1 channel at a time. If more than 1 channels are enabled, only channel
  *       with smallest number will be convert.
  * @note This API does not turn on ADC power nor does trigger ADC conversion
  */
void ADC_Open(ADC_T *adc,
              uint32_t u32InputMode,
              uint32_t u32OpMode,
              uint32_t u32ChMask)
{
    (void) adc;

    ADC->ADCR = (ADC->ADCR & (~(ADC_ADCR_DIFFEN_Msk | ADC_ADCR_ADMD_Msk))) | \
                (u32InputMode) | \
                (u32OpMode);

    ADC->ADCHER = (ADC->ADCHER & ~ADC_ADCHER_CHEN_Msk) | (u32ChMask);

    return;
}

/**
  * @brief Disable ADC module
  * @param[in] adc The pointer of the specified ADC module
  * @return None
  */
void ADC_Close(ADC_T *adc)
{
    (void) adc;
    SYS->IPRST1 |= SYS_IPRST1_ADCRST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST1_ADCRST_Msk;
    return;

}

/**
  * @brief Configure the hardware trigger condition and enable hardware trigger
  * @param[in] adc The pointer of the specified ADC module
  * @param[in] u32Source Decides the hardware trigger source. Valid values are:
  *                       - \ref ADC_ADCR_TRGS_STADC            :A/D conversion is started by external STADC pin.
  *                       - \ref ADC_ADCR_TRGS_TIMER            :A/D conversion is started by Timer.
  *                       - \ref ADC_ADCR_TRGS_PWM              :A/D conversion is started by PWM.
  * @param[in] u32Param While ADC trigger by PWM, this parameter is used to set the delay between PWM
  *                     trigger and ADC conversion. Valid values are from 0 ~ 0xFF, and actual delay
  *                     time is (4 * u32Param * HCLK). While ADC trigger by external pin, this parameter
  *                     is used to set trigger condition. While ADC trigger by Timer, this parameter
  *                     is u non-used. Valid values are:
  *                      - \ref ADC_ADCR_TRGCOND_LOW_LEVEL     :STADC Low level active
  *                      - \ref ADC_ADCR_TRGCOND_HIGH_LEVEL    :STADC High level active
  *                      - \ref ADC_ADCR_TRGCOND_FALLING_EDGE  :STADC Falling edge active
  *                      - \ref ADC_ADCR_TRGCOND_RISING_EDGE   :STADC Rising edge active
  * @return None
  * @note Software should disable TRGEN (ADCR[8]) and ADST (ADCR[11]) before change TRGS(ADCR[5:4]).
  */
void ADC_EnableHWTrigger(ADC_T *adc,
                         uint32_t u32Source,
                         uint32_t u32Param)
{
    (void) adc;

    if(u32Source == ADC_ADCR_TRGS_STADC)
    {
        ADC->ADCR = (ADC->ADCR & ~(ADC_ADCR_TRGS_Msk | ADC_ADCR_TRGCOND_Msk | ADC_ADCR_TRGEN_Msk)) | (u32Source) | (u32Param) | ADC_ADCR_TRGEN_Msk;
    }
    else if(u32Source == ADC_ADCR_TRGS_TIMER)
    {
        ADC->ADCR = (ADC->ADCR & ~(ADC_ADCR_TRGS_Msk | ADC_ADCR_TRGCOND_Msk | ADC_ADCR_TRGEN_Msk)) | (u32Source) | ADC_ADCR_TRGEN_Msk;
    }
    else
    {
        ADC->ADTDCR = (ADC->ADTDCR & ~ADC_ADTDCR_PTDT_Msk) | (u32Param);
        ADC->ADCR = (ADC->ADCR & ~(ADC_ADCR_TRGS_Msk | ADC_ADCR_TRGCOND_Msk | ADC_ADCR_TRGEN_Msk)) | (u32Source) | ADC_ADCR_TRGEN_Msk;
    }
    return;
}

/**
  * @brief Disable hardware trigger ADC function.
  * @param[in] adc The pointer of the specified ADC module
  * @return None
  */
void ADC_DisableHWTrigger(ADC_T *adc)
{
    (void) adc;
    ADC->ADCR &= ~(ADC_ADCR_TRGS_Msk | ADC_ADCR_TRGCOND_Msk | ADC_ADCR_TRGEN_Msk);
    return;
}

/**
  * @brief Enable the interrupt(s) selected by u32Mask parameter.
  * @param[in] adc The pointer of the specified ADC module
  * @param[in] u32Mask The combination of interrupt status bits listed below. Each bit
  *                    corresponds to a interrupt status. This parameter decides which
  *                    interrupts will be enabled.
  *                     - \ref ADC_ADF_INT    :ADC convert complete interrupt
  *                     - \ref ADC_CMP0_INT   :ADC comparator 0 interrupt
  *                     - \ref ADC_CMP1_INT   :ADC comparator 1 interrupt
  * @return None
  */
void ADC_EnableInt(ADC_T *adc, uint32_t u32Mask)
{
    (void) adc;

    if((u32Mask) & ADC_ADF_INT)
        ADC->ADCR |= ADC_ADCR_ADIE_Msk;
    if((u32Mask) & ADC_CMP0_INT)
        ADC->ADCMPR[0] |= ADC_ADCMPR_CMPIE_Msk;
    if((u32Mask) & ADC_CMP1_INT)
        ADC->ADCMPR[1] |= ADC_ADCMPR_CMPIE_Msk;

    return;
}

/**
  * @brief Disable the interrupt(s) selected by u32Mask parameter.
  * @param[in] adc The pointer of the specified ADC module
  * @param[in] u32Mask The combination of interrupt status bits listed below. Each bit
  *                    corresponds to a interrupt status. This parameter decides which
  *                    interrupts will be disabled.
  *                     - \ref ADC_ADF_INT     :ADC convert complete interrupt
  *                     - \ref ADC_CMP0_INT    :ADC comparator 0 interrupt
  *                     - \ref ADC_CMP1_INT    :ADC comparator 1 interrupt
  * @return None
  */
void ADC_DisableInt(ADC_T *adc, uint32_t u32Mask)
{
    (void) adc;

    if((u32Mask) & ADC_ADF_INT)
        ADC->ADCR &= ~ADC_ADCR_ADIE_Msk;
    if((u32Mask) & ADC_CMP0_INT)
        ADC->ADCMPR[0] &= ~ADC_ADCMPR_CMPIE_Msk;
    if((u32Mask) & ADC_CMP1_INT)
        ADC->ADCMPR[1] &= ~ADC_ADCMPR_CMPIE_Msk;

    return;
}



/*@}*/ /* end of group ADC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group ADC_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/

