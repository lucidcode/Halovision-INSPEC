#include <stdint.h> 
#include <system_TM4C123.h>
#include <TM4C123.h>

#define SRAM_START 0x20000000
#define SRAM_SIZE  (32*1024) 
#define SRAM_END   (SRAM_START + SRAM_SIZE)

//*****************************************************************************
//
//  Bit definitions for the FPU CPAC register.
//
//*****************************************************************************
#define CPAC_CP11_M        0x00C00000  // CP11 Coprocessor Access
                                            // Privilege
#define CPAC_CP11_DIS      0x00000000  // Access Denied
#define CPAC_CP11_PRIV     0x00400000  // Privileged Access Only
#define CPAC_CP11_FULL     0x00C00000  // Full Access
#define CPAC_CP10_M        0x00300000  // CP10 Coprocessor Access
                                            // Privilege
#define CPAC_CP10_DIS      0x00000000  // Access Denied
#define CPAC_CP10_PRIV     0x00100000  // Privileged Access Only
#define CPAC_CP10_FULL     0x00300000  // Full Access


#define init_MSP   SRAM_END 

void Reset_Handler(void) ; 
void NMI_Handler 					(void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler 				(void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler 				(void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler 				(void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler 			(void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler 					(void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler 				(void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler   				(void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler  				(void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOA_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOB_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOC_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOD_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOE_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void SSI0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void PMW0_FAULT_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM0_0_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM0_1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM0_2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void QEI0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0SS0_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0SS1_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0SS2_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0SS3_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void WDT0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0A_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0B_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER1A_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER1B_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER2A_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER2B_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void COMP0_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void COMP1_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void COMP2_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void SYSCTL_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOF_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOG_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOH_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void SSI1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER3A_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER3B_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void QEI1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void HIB_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void USB0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM0_3_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void UDMA_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UDMAERR_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1SS0_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1SS1_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1SS2_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1SS3_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOJ_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOK_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOL_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void SSI2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SSI3_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void UART5_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void UART6_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void UART7_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C3_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER4A_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER4B_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER5A_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER5B_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER0A_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER0B_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER1A_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER1B_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER2A_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER2B_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER3A_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER3B_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER4A_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER4B_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER5A_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void WTIMER5B_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void FPU_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C4_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C5_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOM_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPION_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void QEI2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOP0_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOP1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOP2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOP3_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOP4_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOP5_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOP6_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOP7_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOQ0_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOQ1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOQ2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOQ3_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOQ4_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOQ5_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOQ6_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOQ7_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOR_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIOS_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void PMW1_0_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM1_1_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM1_2_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM1_3_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM1_FAULT_Handle              (void) __attribute__ ((weak, alias("Default_Handler")));

extern void __libc_init_array(void) ; 
int main(void); 

__attribute__((section(".vectors"))) uint32_t vectors [] = 
{
    init_MSP,
    (uint32_t)Reset_Handler,
    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    (uint32_t)MemManage_Handler,
    (uint32_t)BusFault_Handler,
    (uint32_t)UsageFault_Handler,
    0x00,
    0x00,
    0x00,
    0x00,
    (uint32_t)SVC_Handler,
    (uint32_t)DebugMon_Handler,
    0x00,
    (uint32_t)PendSV_Handler,
    (uint32_t)SysTick_Handler,
    (uint32_t)GPIOA_Handler ,
    (uint32_t)GPIOB_Handler ,
    (uint32_t)GPIOC_Handler ,
    (uint32_t)GPIOD_Handler ,
    (uint32_t)GPIOE_Handler ,
    (uint32_t)UART0_Handler ,
    (uint32_t)UART1_Handler ,
    (uint32_t)SSI0_Handler ,
    (uint32_t)I2C0_Handler ,
    (uint32_t)PMW0_FAULT_Handler ,
    (uint32_t)PWM0_0_Handler ,
    (uint32_t)PWM0_1_Handler ,
    (uint32_t)PWM0_2_Handler ,
    (uint32_t)QEI0_Handler ,
    (uint32_t)ADC0SS0_Handler ,
    (uint32_t)ADC0SS1_Handler ,
    (uint32_t)ADC0SS2_Handler ,
    (uint32_t)ADC0SS3_Handler ,
    (uint32_t)WDT0_Handler ,
    (uint32_t)TIMER0A_Handler ,
    (uint32_t)TIMER0B_Handler ,
    (uint32_t)TIMER1A_Handler ,
    (uint32_t)TIMER1B_Handler ,
    (uint32_t)TIMER2A_Handler ,
    (uint32_t)TIMER2B_Handler ,
    (uint32_t)COMP0_Handler ,
    (uint32_t)COMP1_Handler ,
    (uint32_t)0x00 ,
    (uint32_t)SYSCTL_Handler ,
    (uint32_t)FLASH_Handler ,
    (uint32_t)GPIOF_Handler ,
    (uint32_t)0x00 ,
    (uint32_t)0x00 ,
    (uint32_t)UART2_Handler ,
    (uint32_t)SSI1_Handler ,
    (uint32_t)TIMER3A_Handler ,
    (uint32_t)TIMER3B_Handler ,
    (uint32_t)I2C1_Handler ,
    (uint32_t)QEI1_Handler ,
    (uint32_t)CAN0_Handler ,
    (uint32_t)CAN1_Handler ,
    (uint32_t)CAN2_Handler ,
    (uint32_t)0x00,
    (uint32_t)HIB_Handler ,
    (uint32_t)USB0_Handler ,
    (uint32_t)PWM0_3_Handler ,
    (uint32_t)UDMA_Handler ,
    (uint32_t)UDMAERR_Handler ,
    (uint32_t)ADC1SS0_Handler ,
    (uint32_t)ADC1SS1_Handler ,
    (uint32_t)ADC1SS2_Handler ,
    (uint32_t)ADC1SS3_Handler ,
    (uint32_t)0,
    (uint32_t)0,
    (uint32_t)GPIOJ_Handler ,
    (uint32_t)GPIOK_Handler ,
    (uint32_t)GPIOL_Handler ,
    (uint32_t)SSI2_Handler ,
    (uint32_t)SSI3_Handler ,
    (uint32_t)UART3_Handler ,
    (uint32_t)UART4_Handler ,
    (uint32_t)UART5_Handler ,
    (uint32_t)UART6_Handler ,
    (uint32_t)UART7_Handler ,
     0x00,
     0x00,
     0x00,
     0x00,
    (uint32_t)I2C2_Handler ,
    (uint32_t)I2C3_Handler ,
    (uint32_t)TIMER4A_Handler ,
    (uint32_t)TIMER4B_Handler ,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (uint32_t)TIMER5A_Handler ,
    (uint32_t)TIMER5B_Handler ,
    (uint32_t)WTIMER0A_Handler ,
    (uint32_t)WTIMER0B_Handler ,
    (uint32_t)WTIMER1A_Handler ,
    (uint32_t)WTIMER1B_Handler ,
    (uint32_t)WTIMER2A_Handler ,
    (uint32_t)WTIMER2B_Handler ,
    (uint32_t)WTIMER3A_Handler ,
    (uint32_t)WTIMER3B_Handler ,
    (uint32_t)WTIMER4A_Handler ,
    (uint32_t)WTIMER4B_Handler ,
    (uint32_t)WTIMER5A_Handler ,
    (uint32_t)WTIMER5B_Handler ,
    (uint32_t)FPU_Handler ,
    0,
    0,
    (uint32_t)I2C4_Handler ,
    (uint32_t)I2C5_Handler ,
    (uint32_t)GPIOM_Handler ,
    (uint32_t)GPION_Handler ,
    (uint32_t)QEI2_Handler ,
    0,
    0,
    (uint32_t)GPIOP0_Handler ,
    (uint32_t)GPIOP1_Handler ,
    (uint32_t)GPIOP2_Handler ,
    (uint32_t)GPIOP3_Handler ,
    (uint32_t)GPIOP4_Handler ,
    (uint32_t)GPIOP5_Handler ,
    (uint32_t)GPIOP6_Handler ,
    (uint32_t)GPIOP7_Handler ,
    (uint32_t)GPIOQ0_Handler ,
    (uint32_t)GPIOQ1_Handler ,
    (uint32_t)GPIOQ2_Handler ,
    (uint32_t)GPIOQ3_Handler ,
    (uint32_t)GPIOQ4_Handler ,
    (uint32_t)GPIOQ5_Handler ,
    (uint32_t)GPIOQ6_Handler ,
    (uint32_t)GPIOQ7_Handler ,
    (uint32_t)GPIOR_Handler ,
    (uint32_t)GPIOS_Handler ,
    (uint32_t)PMW1_0_Handler ,
    (uint32_t)PWM1_1_Handler ,
    (uint32_t)PWM1_2_Handler ,
    (uint32_t)PWM1_3_Handler ,
    (uint32_t)PWM1_FAULT_Handle ,   
} ;
 
extern volatile uint32_t __start_data ;
extern volatile uint32_t __end_data; 
extern volatile uint32_t __start_bss;
extern volatile uint32_t __end_bss; 
extern volatile uint32_t __end_text;
extern volatile uint32_t __la_data;

void Default_Handler(void) 
{

    while(1); 

}

void Reset_Handler(void)
{
   uint32_t copySize = (uint32_t)&__end_data - (uint32_t)&__start_data ; 
   
   uint8_t *pDest = (uint8_t*)&__start_data ; 
   uint8_t *psrc =(uint8_t*)&__la_data ; 
   
   /* Copy data segment to RAM */ 
   for (uint32_t i = 0; i < copySize; i++)
   {
       *pDest++ = *psrc++ ; 
   }

    /* Zero fill .bss section */ 
       __asm(" ldr     r0, =__start_bss\n"
          "    ldr     r1, =__end_bss\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "    zero_loop:\n"
          "    cmp     r0, r1\n"
          "    it      lt\n"
          "    strlt   r2, [r0], #4\n"
          "    blt     zero_loop");
   

    // Enable the floating-point unit.  This must be done here to handle the
    // case where main() uses floating-point and the function prologue saves
    // floating-point registers (which will fault if floating-point is not
    // enabled).
    SCB->CPACR &= ~ (CPAC_CP10_M | CPAC_CP11_M);
    SCB->CPACR |= (CPAC_CP10_FULL | CPAC_CP11_FULL);

   //__libc_init_array();
   SystemInit();
   main() ; 
     
}
