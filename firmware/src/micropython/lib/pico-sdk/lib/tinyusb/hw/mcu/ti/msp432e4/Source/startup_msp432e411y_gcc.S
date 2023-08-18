/**
  ******************************************************************************
  *  Copyright (C) 2012 - 2017 Texas Instruments Incorporated - http://www.ti.com/ 
  * 
  *  Redistribution and use in source and binary forms, with or without 
  *  modification, are permitted provided that the following conditions 
  *  are met:
  * 
  *   Redistributions of source code must retain the above copyright 
  *   notice, this list of conditions and the following disclaimer.
  * 
  *   Redistributions in binary form must reproduce the above copyright
  *   notice, this list of conditions and the following disclaimer in the 
  *   documentation and/or other materials provided with the   
  *   distribution.
  * 
  *   Neither the name of Texas Instruments Incorporated nor the names of
  *   its contributors may be used to endorse or promote products derived
  *   from this software without specific prior written permission.
  * 
  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
  *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
  *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
  *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
  *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
  *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  * 
  *  MSP432P4111 Interrupt Vector Table
  *
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   sp, =_estack    /* set stack pointer */
/* Copy the data segment initializers from flash to SRAM */
  movs r1, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r3, =_sidata
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4

LoopCopyDataInit:
  ldr r0, =_sdata
  ldr r3, =_edata
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyDataInit
  ldr r2, =_sbss
  b LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs r3, #0
  str r3, [r2], #4

LoopFillZerobss:
  ldr r3, = _ebss
  cmp r2, r3
  bcc FillZerobss

/* Call the clock system intitialization function.*/
    bl  SystemInit
/* Call static constructors */
    bl __libc_init_array
/* Call the applications entry point.*/
  bl main
  
    /* 6. Loop forever if returning from main */
LoopForever:
  b LoopForever
  
.size Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
   .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler
  .word GPIOA_IRQHandler                        /* GPIO Port A                  */
  .word GPIOB_IRQHandler                        /* GPIO Port B                  */
  .word GPIOC_IRQHandler                        /* GPIO Port c                  */
  .word GPIOD_IRQHandler                        /* GPIO Port D                  */
  .word GPIOE_IRQHandler                        /* GPIO Port E                  */
  .word UART0_IRQHandler                        /* UART0 Rx and Tx              */
  .word UART1_IRQHandler                        /* UART1 Rx and Tx              */
  .word SSI0_IRQHandler                         /* SSI0 Rx and Tx               */
  .word I2C0_IRQHandler                         /* I2C0 Master and Slave        */
  .word PWM0_FAULT_IRQHandler                   /* PWM Fault                    */
  .word PWM0_0_IRQHandler                       /* PWM Generator 0              */
  .word PWM0_1_IRQHandler                       /* PWM Generator 1              */
  .word PWM0_2_IRQHandler                       /* PWM Generator 2              */
  .word QEI0_IRQHandler                         /* Quadrature Encoder 0         */
  .word ADC0SS0_IRQHandler                      /* ADC Sequence 0               */
  .word ADC0SS1_IRQHandler                      /* ADC Sequence 1               */
  .word ADC0SS2_IRQHandler                      /* ADC Sequence 2               */
  .word ADC0SS3_IRQHandler                      /* ADC Sequence 3               */
  .word WATCHDOG_IRQHandler                     /* Watchdog timer               */
  .word TIMER0A_IRQHandler                      /* Timer 0 subtimer A           */
  .word TIMER0B_IRQHandler                      /* Timer 0 subtimer B           */
  .word TIMER1A_IRQHandler                      /* Timer 1 subtimer A           */
  .word TIMER1B_IRQHandler                      /* Timer 1 subtimer B           */
  .word TIMER2A_IRQHandler                      /* Timer 2 subtimer A           */
  .word TIMER2B_IRQHandler                      /* Timer 2 subtimer B           */
  .word COMP0_IRQHandler                        /* Analog Comparator 0          */
  .word COMP1_IRQHandler                        /* Analog Comparator 1          */
  .word COMP2_IRQHandler                        /* Analog Comparator 2          */
  .word SYSCTL_IRQHandler                       /* System Control               */
  .word FLASH_IRQHandler                        /* FLASH Control                */
  .word GPIOF_IRQHandler                        /* GPIO Port F                  */
  .word GPIOG_IRQHandler                        /* GPIO Port G                  */
  .word GPIOH_IRQHandler                        /* GPIO Port H                  */
  .word UART2_IRQHandler                        /* UART2 Rx and Tx              */
  .word SSI1_IRQHandler                         /* SSI1 Rx and Tx               */
  .word TIMER3A_IRQHandler                      /* Timer 3 subtimer A           */
  .word TIMER3B_IRQHandler                      /* Timer 3 subtimer B           */
  .word I2C1_IRQHandler                         /* I2C1 Master and Slave        */
  .word CAN0_IRQHandler                         /* CAN0                         */
  .word CAN1_IRQHandler                         /* CAN1                         */
  .word EMAC0_IRQHandler                        /* Ethernet                     */
  .word HIBERNATE_IRQHandler                    /* Hibernate                    */
  .word USB0_IRQHandler                         /* USB0                         */
  .word PWM0_3_IRQHandler                       /* PWM Generator 3              */
  .word UDMA_IRQHandler                         /* uDMA Software Transfer       */
  .word UDMAERR_IRQHandler                      /* uDMA Error                   */
  .word ADC1SS0_IRQHandler                      /* ADC1 Sequence 0              */
  .word ADC1SS1_IRQHandler                      /* ADC1 Sequence 1              */
  .word ADC1SS2_IRQHandler                      /* ADC1 Sequence 2              */
  .word ADC1SS3_IRQHandler                      /* ADC1 Sequence 3              */
  .word EPI0_IRQHandler                         /* External Bus Interface 0     */
  .word GPIOJ_IRQHandler                        /* GPIO Port J                  */
  .word GPIOK_IRQHandler                        /* GPIO Port K                  */
  .word GPIOL_IRQHandler                        /* GPIO Port L                  */
  .word SSI2_IRQHandler                         /* SSI2 Rx and Tx               */
  .word SSI3_IRQHandler                         /* SSI3 Rx and Tx               */
  .word UART3_IRQHandler                        /* UART3 Rx and Tx              */
  .word UART4_IRQHandler                        /* UART4 Rx and Tx              */
  .word UART5_IRQHandler                        /* UART5 Rx and Tx              */
  .word UART6_IRQHandler                        /* UART6 Rx and Tx              */
  .word UART7_IRQHandler                        /* UART7 Rx and Tx              */
  .word I2C2_IRQHandler                         /* I2C3 Master and Slave        */
  .word I2C3_IRQHandler                         /* I2C4 Master and Slave        */
  .word TIMER4A_IRQHandler                      /* Timer 4 subtimer A           */
  .word TIMER4B_IRQHandler                      /* Timer 4 subtimer B           */
  .word TIMER5A_IRQHandler                      /* Timer 5 subtimer A           */
  .word TIMER5B_IRQHandler                      /* Timer 5 subtimer B           */
  .word SYSEXC_IRQHandler                       /* FPU                          */
  .word 0                                       /* Reserved                     */
  .word 0                                       /* Reserved                     */
  .word I2C4_IRQHandler                         /* I2C4 Master and Slave        */
  .word I2C5_IRQHandler                         /* I2C5 Master and Slave        */
  .word GPIOM_IRQHandler                        /* GPIO Port M                  */
  .word GPION_IRQHandler                        /* GPIO Port N                  */
  .word 0                                       /* Reserved                     */
  .word TAMPER0_IRQHandler                      /* Tamper                       */
  .word GPIOP0_IRQHandler                       /* GPIO Port P(Summary or P0)   */
  .word GPIOP1_IRQHandler                       /* GPIO Port P1                 */
  .word GPIOP2_IRQHandler                       /* GPIO Port P2                 */
  .word GPIOP3_IRQHandler                       /* GPIO Port P3                 */
  .word GPIOP4_IRQHandler                       /* GPIO Port P4                 */
  .word GPIOP5_IRQHandler                       /* GPIO Port P5                 */
  .word GPIOP6_IRQHandler                       /* GPIO Port P6                 */
  .word GPIOP7_IRQHandler                       /* GPIO Port P7                 */
  .word GPIOQ0_IRQHandler                       /* GPIO Port Q0                 */
  .word GPIOQ1_IRQHandler                       /* GPIO Port Q1                 */
  .word GPIOQ2_IRQHandler                       /* GPIO Port Q2                 */
  .word GPIOQ3_IRQHandler                       /* GPIO Port Q3                 */
  .word GPIOQ4_IRQHandler                       /* GPIO Port Q4                 */
  .word GPIOQ5_IRQHandler                       /* GPIO Port Q5                 */
  .word GPIOQ6_IRQHandler                       /* GPIO Port Q6                 */
  .word GPIOQ7_IRQHandler                       /* GPIO Port Q7                 */
  .word GPIOR_IRQHandler                        /* GPIO Port R                  */
  .word GPIOS_IRQHandler                        /* GPIO Port S                  */
  .word SHA0_IRQHandler                         /* SHA/MD5 0                    */
  .word AES0_IRQHandler                         /* AES 0                        */
  .word DES0_IRQHandler                         /* DES3DES 0                    */
  .word LCD0_IRQHandler                         /* LCD Controller 0             */
  .word TIMER6A_IRQHandler                      /* Timer 6 subtimer A           */
  .word TIMER6B_IRQHandler                      /* Timer 6 subtimer B           */
  .word TIMER7A_IRQHandler                      /* Timer 7 subtimer A           */
  .word TIMER7B_IRQHandler                      /* Timer 7 subtimer B           */
  .word I2C6_IRQHandler                         /* I2C6 Master and Slave        */
  .word I2C7_IRQHandler                         /* I2C7 Master and Slave        */
  .word 0                                       /* Reserved                     */
  .word ONEWIRE0_IRQHandler                     /* One Wire 0                   */
  .word 0                                       /* Reserved                     */
  .word 0                                       /* Reserved                     */
  .word 0                                       /* Reserved                     */
  .word I2C8_IRQHandler                         /* I2C8 Master and Slave        */
  .word I2C9_IRQHandler                         /* I2C9 Master and Slave        */
  .word GPIOT_IRQHandler                        /* GPIO Port T                  */
  

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

  .weak PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler 

  .weak GPIOA_IRQHandler
  .thumb_set GPIOA_IRQHandler,Default_Handler

  .weak GPIOB_IRQHandler
  .thumb_set GPIOB_IRQHandler,Default_Handler

  .weak GPIOC_IRQHandler
  .thumb_set GPIOC_IRQHandler,Default_Handler

  .weak GPIOD_IRQHandler
  .thumb_set GPIOD_IRQHandler,Default_Handler

  .weak GPIOE_IRQHandler
  .thumb_set GPIOE_IRQHandler,Default_Handler

  .weak UART0_IRQHandler
  .thumb_set UART0_IRQHandler,Default_Handler

  .weak UART1_IRQHandler
  .thumb_set UART1_IRQHandler,Default_Handler

  .weak SSI0_IRQHandler
  .thumb_set SSI0_IRQHandler,Default_Handler

  .weak I2C0_IRQHandler
  .thumb_set I2C0_IRQHandler,Default_Handler

  .weak PWM0_FAULT_IRQHandler
  .thumb_set PWM0_FAULT_IRQHandler,Default_Handler

  .weak PWM0_0_IRQHandler
  .thumb_set PWM0_0_IRQHandler,Default_Handler

  .weak PWM0_1_IRQHandler
  .thumb_set PWM0_1_IRQHandler,Default_Handler

  .weak PWM0_2_IRQHandler
  .thumb_set PWM0_2_IRQHandler,Default_Handler

  .weak QEI0_IRQHandler
  .thumb_set QEI0_IRQHandler,Default_Handler

  .weak ADC0SS0_IRQHandler
  .thumb_set ADC0SS0_IRQHandler,Default_Handler

  .weak ADC0SS1_IRQHandler
  .thumb_set ADC0SS1_IRQHandler,Default_Handler

  .weak ADC0SS2_IRQHandler
  .thumb_set ADC0SS2_IRQHandler,Default_Handler

  .weak ADC0SS3_IRQHandler
  .thumb_set ADC0SS3_IRQHandler,Default_Handler

  .weak WATCHDOG_IRQHandler
  .thumb_set WATCHDOG_IRQHandler,Default_Handler

  .weak TIMER0A_IRQHandler
  .thumb_set TIMER0A_IRQHandler,Default_Handler

  .weak TIMER0B_IRQHandler
  .thumb_set TIMER0B_IRQHandler,Default_Handler

  .weak TIMER1A_IRQHandler
  .thumb_set TIMER1A_IRQHandler,Default_Handler

  .weak TIMER1B_IRQHandler
  .thumb_set TIMER1B_IRQHandler,Default_Handler

  .weak TIMER2A_IRQHandler
  .thumb_set TIMER2A_IRQHandler,Default_Handler

  .weak TIMER2B_IRQHandler
  .thumb_set TIMER2B_IRQHandler,Default_Handler

  .weak COMP0_IRQHandler
  .thumb_set COMP0_IRQHandler,Default_Handler

  .weak COMP1_IRQHandler
  .thumb_set COMP1_IRQHandler,Default_Handler

  .weak COMP2_IRQHandler
  .thumb_set COMP2_IRQHandler,Default_Handler

  .weak SYSCTL_IRQHandler
  .thumb_set SYSCTL_IRQHandler,Default_Handler

  .weak FLASH_IRQHandler
  .thumb_set FLASH_IRQHandler,Default_Handler

  .weak GPIOF_IRQHandler
  .thumb_set GPIOF_IRQHandler,Default_Handler

  .weak GPIOG_IRQHandler
  .thumb_set GPIOG_IRQHandler,Default_Handler

  .weak GPIOH_IRQHandler
  .thumb_set GPIOH_IRQHandler,Default_Handler

  .weak UART2_IRQHandler
  .thumb_set UART2_IRQHandler,Default_Handler

  .weak SSI1_IRQHandler
  .thumb_set SSI1_IRQHandler,Default_Handler

  .weak TIMER3A_IRQHandler
  .thumb_set TIMER3A_IRQHandler,Default_Handler

  .weak TIMER3B_IRQHandler
  .thumb_set TIMER3B_IRQHandler,Default_Handler

  .weak I2C1_IRQHandler
  .thumb_set I2C1_IRQHandler,Default_Handler

  .weak CAN0_IRQHandler
  .thumb_set CAN0_IRQHandler,Default_Handler

  .weak CAN1_IRQHandler
  .thumb_set CAN1_IRQHandler,Default_Handler

  .weak EMAC0_IRQHandler
  .thumb_set EMAC0_IRQHandler,Default_Handler

  .weak HIBERNATE_IRQHandler
  .thumb_set HIBERNATE_IRQHandler,Default_Handler
  
  .weak USB0_IRQHandler
  .thumb_set USB0_IRQHandler,Default_Handler
  
  .weak PWM0_3_IRQHandler
  .thumb_set PWM0_3_IRQHandler,Default_Handler
  
  .weak UDMA_IRQHandler
  .thumb_set UDMA_IRQHandler,Default_Handler
  
  .weak UDMAERR_IRQHandler
  .thumb_set UDMAERR_IRQHandler,Default_Handler
  
  .weak ADC1SS0_IRQHandler
  .thumb_set ADC1SS0_IRQHandler,Default_Handler
  
  .weak ADC1SS1_IRQHandler
  .thumb_set ADC1SS1_IRQHandler,Default_Handler
  
  .weak ADC1SS2_IRQHandler
  .thumb_set ADC1SS2_IRQHandler,Default_Handler
  
  .weak ADC1SS3_IRQHandler
  .thumb_set ADC1SS3_IRQHandler,Default_Handler
  
  .weak EPI0_IRQHandler
  .thumb_set EPI0_IRQHandler,Default_Handler
  
  .weak GPIOJ_IRQHandler
  .thumb_set GPIOJ_IRQHandler,Default_Handler
  
  .weak GPIOK_IRQHandler
  .thumb_set GPIOK_IRQHandler,Default_Handler
  
  .weak GPIOL_IRQHandler
  .thumb_set GPIOL_IRQHandler,Default_Handler
  
  .weak SSI2_IRQHandler
  .thumb_set SSI2_IRQHandler,Default_Handler
  
  .weak SSI3_IRQHandler
  .thumb_set SSI3_IRQHandler,Default_Handler
  
  .weak UART3_IRQHandler
  .thumb_set UART3_IRQHandler,Default_Handler
  
  .weak UART4_IRQHandler
  .thumb_set UART4_IRQHandler,Default_Handler
  
  .weak UART5_IRQHandler
  .thumb_set UART5_IRQHandler,Default_Handler
  
  .weak UART6_IRQHandler
  .thumb_set UART6_IRQHandler,Default_Handler
  
  .weak UART7_IRQHandler
  .thumb_set UART7_IRQHandler,Default_Handler
  
  .weak I2C2_IRQHandler
  .thumb_set I2C2_IRQHandler,Default_Handler
  
  .weak I2C3_IRQHandler
  .thumb_set I2C3_IRQHandler,Default_Handler
  
  .weak TIMER4A_IRQHandler
  .thumb_set TIMER4A_IRQHandler,Default_Handler
  
  .weak TIMER4B_IRQHandler
  .thumb_set TIMER4B_IRQHandler,Default_Handler
  
  .weak TIMER5A_IRQHandler
  .thumb_set TIMER5A_IRQHandler,Default_Handler
  
  .weak TIMER5B_IRQHandler
  .thumb_set TIMER5B_IRQHandler,Default_Handler
  
  .weak SYSEXC_IRQHandler
  .thumb_set SYSEXC_IRQHandler,Default_Handler
  
  .weak I2C4_IRQHandler
  .thumb_set I2C4_IRQHandler,Default_Handler
  
  .weak I2C5_IRQHandler
  .thumb_set I2C5_IRQHandler,Default_Handler
  
  .weak GPIOM_IRQHandler
  .thumb_set GPIOM_IRQHandler,Default_Handler
  
  .weak GPION_IRQHandler
  .thumb_set GPION_IRQHandler,Default_Handler
  
  .weak TAMPER0_IRQHandler
  .thumb_set TAMPER0_IRQHandler,Default_Handler
  
  .weak GPIOP0_IRQHandler
  .thumb_set GPIOP0_IRQHandler,Default_Handler
  
  .weak GPIOP1_IRQHandler
  .thumb_set GPIOP1_IRQHandler,Default_Handler

  .weak GPIOP2_IRQHandler
  .thumb_set GPIOP2_IRQHandler,Default_Handler

  .weak GPIOP3_IRQHandler
  .thumb_set GPIOP3_IRQHandler,Default_Handler

  .weak GPIOP4_IRQHandler
  .thumb_set GPIOP4_IRQHandler,Default_Handler

  .weak GPIOP5_IRQHandler
  .thumb_set GPIOP5_IRQHandler,Default_Handler

  .weak GPIOP6_IRQHandler
  .thumb_set GPIOP6_IRQHandler,Default_Handler

  .weak GPIOP7_IRQHandler
  .thumb_set GPIOP7_IRQHandler,Default_Handler

  .weak GPIOQ0_IRQHandler
  .thumb_set GPIOQ0_IRQHandler,Default_Handler

  .weak GPIOQ1_IRQHandler
  .thumb_set GPIOQ1_IRQHandler,Default_Handler

  .weak GPIOQ2_IRQHandler
  .thumb_set GPIOQ2_IRQHandler,Default_Handler

  .weak GPIOQ3_IRQHandler
  .thumb_set GPIOQ3_IRQHandler,Default_Handler

  .weak GPIOQ4_IRQHandler
  .thumb_set GPIOQ4_IRQHandler,Default_Handler

  .weak GPIOQ5_IRQHandler
  .thumb_set GPIOQ5_IRQHandler,Default_Handler

  .weak GPIOQ6_IRQHandler
  .thumb_set GPIOQ6_IRQHandler,Default_Handler

  .weak GPIOQ7_IRQHandler
  .thumb_set GPIOQ7_IRQHandler,Default_Handler

  .weak GPIOR_IRQHandler
  .thumb_set GPIOR_IRQHandler,Default_Handler

  .weak GPIOS_IRQHandler
  .thumb_set GPIOS_IRQHandler,Default_Handler
  
  .weak SHA0_IRQHandler
  .thumb_set SHA0_IRQHandler,Default_Handler
  
  .weak AES0_IRQHandler
  .thumb_set AES0_IRQHandler,Default_Handler
  
  .weak DES0_IRQHandler
  .thumb_set DES0_IRQHandler,Default_Handler
  
  .weak LCD0_IRQHandler
  .thumb_set LCD0_IRQHandler,Default_Handler
  
  .weak TIMER6A_IRQHandler
  .thumb_set TIMER6A_IRQHandler,Default_Handler
  
  .weak TIMER6B_IRQHandler
  .thumb_set TIMER6B_IRQHandler,Default_Handler
   
  .weak TIMER7A_IRQHandler
  .thumb_set TIMER7A_IRQHandler,Default_Handler
  
  .weak TIMER7B_IRQHandler
  .thumb_set TIMER7B_IRQHandler,Default_Handler
  
  .weak I2C6_IRQHandler
  .thumb_set I2C6_IRQHandler,Default_Handler
  
  .weak I2C7_IRQHandler
  .thumb_set I2C7_IRQHandler,Default_Handler
  
  .weak ONEWIRE0_IRQHandler
  .thumb_set ONEWIRE0_IRQHandler,Default_Handler
  
  .weak I2C8_IRQHandler
  .thumb_set I2C8_IRQHandler,Default_Handler
  
  .weak I2C9_IRQHandler
  .thumb_set I2C9_IRQHandler,Default_Handler
  
  .weak GPIOT_IRQHandler
  .thumb_set GPIOT_IRQHandler,Default_Handler

.end

