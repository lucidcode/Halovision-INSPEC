/**
    @file

    @brief
    Interrupt management

    
**/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Future Technology Devices
 * International Limited ("Bridgetek") subject to the licence terms set out
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

/* INCLUDES ************************************************************************/
#include <ft900_interrupt.h>
#include <registers/ft900_registers.h>

/* CONSTANTS ***********************************************************************/
#define PRIORITY_BASED_INTERRUPT_CTRL          1

/* GLOBAL VARIABLES ****************************************************************/
#if defined(__GNUC__)
/* Soft ISR Vector table as defined in crt0.S */
extern isrptr_t vector_table[N_INTERRUPTS];
/* Dummy function as defined in crt0.S */
extern void nullvector(void);
#elif defined(__MIKROC_PRO_FOR_FT90x__)
/* Default interrupt handler. */
void nullvector(void) {}
/* Soft ISR Vector table as defined in crt0.S */
isrptr_t vector_table[N_INTERRUPTS] = { nullvector,
nullvector, nullvector, nullvector, nullvector, nullvector, nullvector, nullvector, nullvector,
nullvector, nullvector, nullvector, nullvector, nullvector, nullvector, nullvector, nullvector,
nullvector, nullvector, nullvector, nullvector, nullvector, nullvector, nullvector, nullvector,
nullvector, nullvector, nullvector, nullvector, nullvector, nullvector, nullvector, nullvector,
};
/* Map interrupt handlers to vector table. */
void isr1(void) iv IVT_WDT_IRQ                 { vector_table[1](); }
void isr2(void) iv IVT_PM_IRQ                  { vector_table[2](); }
void isr3(void) iv IVT_EHCI_IRQ                { vector_table[interrupt_usb_host](); }
void isr4(void) iv IVT_DUSB2_IRQ               { vector_table[interrupt_usb_device](); }
void isr5(void) iv IVT_DMAC_IRQ                { vector_table[interrupt_ethernet](); }
void isr6(void) iv IVT_SDHOST_IRQ              { vector_table[interrupt_sd_card](); }
void isr7(void) iv IVT_CAN1_IRQ                { vector_table[interrupt_can0](); }
void isr8(void) iv IVT_CAN2_IRQ                { vector_table[interrupt_can1](); }
void isr9(void) iv IVT_CAM_IRQ                 { vector_table[interrupt_camera](); }
void isr10(void) iv IVT_SPIM_IRQ               { vector_table[interrupt_spim](); }
void isr11(void) iv IVT_SPIS1_IRQ              { vector_table[interrupt_spis0](); }
void isr12(void) iv IVT_SPIS2_IRQ              { vector_table[interrupt_spis1](); }
void isr13(void) iv IVT_I2CM_IRQ               { vector_table[interrupt_i2cm](); }
void isr14(void) iv IVT_I2CS_IRQ               { vector_table[interrupt_i2cs](); }
void isr15(void) iv IVT_UART1_IRQ              { vector_table[interrupt_uart0](); }
void isr16(void) iv IVT_UART2_IRQ              { vector_table[interrupt_uart1](); }
void isr17(void) iv IVT_I2S_IRQ                { vector_table[interrupt_i2s](); }
void isr18(void) iv IVT_PWM_IRQ                { vector_table[interrupt_pwm](); }
void isr19(void) iv IVT_TIMERS_IRQ             { vector_table[interrupt_timers](); }
void isr20(void) iv IVT_GPIO_IRQ               { vector_table[interrupt_gpio](); }
void isr21(void) iv IVT_RTC_IRQ                { vector_table[interrupt_rtc](); }
void isr22(void) iv IVT_ADC_IRQ                { vector_table[interrupt_adc](); }
void isr23(void) iv IVT_DAC_IRQ                { vector_table[interrupt_dac](); }
void isr24(void) iv IVT_SLOWCLK_IRQ            { vector_table[interrupt_slowclock](); }
#endif

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/
/** @brief Attach an interrupt
 *  @param interrupt The interrupt number for which the interrupt vector need to be attached
 *  @param priority The priority to give the interrupt. The vector table is indexed on the priority.
 *  @param func The function to call when interrupted
 *  @returns 0 on a success or -1 for a failure
 */
int8_t interrupt_attach(interrupt_t interrupt, uint8_t priority, isrptr_t func)
{
    int8_t iRet = 0;

    if (interrupt == interrupt_wdg)
    {
    	vector_table[interrupt_wdg] = func;
    }
    else
    {
		if (interrupt < interrupt_0 || interrupt >= interrupt_max || priority > 32 || func == NULL)
		{
			iRet = -1;
		}
		else
		{
			vector_table[priority] = func;
			INTERRUPT->IRQ[interrupt] = priority;
		}
    }

    return iRet;
}


/** @brief Detach an interrupt
 *  @param interrupt The interrupt vector to detach
 *  @returns 0 on a success or -1 for a failure
 */
int8_t interrupt_detach(interrupt_t interrupt)
{
    int8_t iRet = 0;

    if (interrupt == interrupt_wdg)
    {
    	vector_table[interrupt_wdg] = nullvector;
    }
    {
    if(interrupt < interrupt_max)
    {
#if PRIORITY_BASED_INTERRUPT_CTRL == 1
		  int priority = INTERRUPT->IRQ[interrupt];
		  vector_table[priority] = nullvector;
		  INTERRUPT->IRQ[interrupt] = interrupt;
#else
		  vector_table[interrupt] = nullvector;
#endif

		}
		else
		{
			iRet = -1;
		}
    }

    return iRet;
}

#ifndef INTERRUPTS_INLINE_ENABLE // This function is inlined.
/** @brief Enable interrupts to fire
 *  @returns 0 on a success or -1 for a failure
 */
void interrupt_enable_globally(void)
{
	INTERRUPT->global_mask &= ~(MASK_IRQ_CTRL_MASKALL);
}
#else // INTERRUPTS_INLINE_ENABLE // End of inline.
extern void interrupt_enable_globally(void);
#endif // INTERRUPTS_INLINE_ENABLE

#ifndef INTERRUPTS_INLINE_ENABLE // This function is inlined.
/** @brief Disable all interrupts
 *  @returns 0 on a success or -1 for a failure
 */
void interrupt_disable_globally(void)
{
	INTERRUPT->global_mask |= MASK_IRQ_CTRL_MASKALL;
}
#else // INTERRUPTS_INLINE_ENABLE // End of inline.
extern void interrupt_disable_globally(void);
#endif // INTERRUPTS_INLINE_ENABLE

/** @brief Enable nesting interrupts
 *  @param max The maximum number of levels to nest (max 16)
 *  @returns 0 on a success or -1 for a failure
 */
int8_t interrupt_enable_nesting(uint8_t max)
{
    int8_t iRet = 0;

    if (max > 16)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        /* Setup maximum nesting */
        //uint8_t nesting = INTERRUPT->nesting; //a read on the register is not needed for this API
        //nesting &= ~(MASK_IRQ_CTRL_NESTMAX);  //
        uint8_t nesting = ((max-1) << BIT_IRQ_CTRL_NESTMAX) & MASK_IRQ_CTRL_NESTMAX;

        /* Enable Nesting */
        nesting |= MASK_IRQ_CTRL_NESTEN;

        INTERRUPT->nesting = nesting;
    }

    return iRet;
}


/** @brief Disable nesting interrupts
 *  @returns 0 on a success or -1 for a failure
 */
int8_t interrupt_disable_nesting(void)
{
    INTERRUPT->nesting &= ~MASK_IRQ_CTRL_NESTEN;
    return 0;
}
