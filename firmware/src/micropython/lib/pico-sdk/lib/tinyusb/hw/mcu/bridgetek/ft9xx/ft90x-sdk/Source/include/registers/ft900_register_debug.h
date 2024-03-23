/**
    @file ft900_register_debug.h

    @brief
    FT900 debugging registers. 

	Provides integration with register mapping section in linker script.

**/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
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

#ifndef FT900_REGISTER_DEBUG_H_
#define FT900_REGISTER_DEBUG_H_

#include <ft900.h>
#include <stdint.h>

/** @brief Pre-mapped registers for FT93x and FT90x devices.
 * @details This structure has the peripheral registers mapped to a
 * C-structure with correct padding to allow a debugger to list and show
 * the values of the peripheral registers.
 */
#if defined(__FT930__)

typedef struct  {
	  ft900_sys_regs_t _SYS;     			/* 0x810000 size 0x1C */
	  ft900_gpio_regs_t _GPIO;				/* 0x81001C size 0x8C */
	  	  uint32_t:32; uint32_t:32; 		/* 0x8100A8 size 0x08 */
	  ft900_adc_dac_regs_t _ADCDAC;			/* 0x8100B0 size 0x0C */
	  ft900_sys_reset_regs_t _SYS_RST;		/* 0x8100BC size 0x04 */
	  	  	  	  	  	  	  	  	  	  	/* 0x8100C0 size 0x40 */
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_interrupt_regs_t _INTERRUPT;	/* 0x810100 size 0x24 */
	  	  	  	  	  	  	  	  	  	  	/* 0x810128 size 0xDC */
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_usbd_regs_t _USBD;				/* 0x810200 size 0x11C */
	  	  uint32_t:32;						/* 0x81031C size 0xE4 */
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_rtc_regs_t _RTC;				/* 0x810400 size 0x2C */
	  	  uint32_t:32;						/* 0x81042C size 0x14 */
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_spi_regs_t _SPIM;				/* 0x810440 size 0x20 */
											/* 0x810460 size 0x20 */
	  	  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  	  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_spi_regs_t _SPIS0;				/* 0x810480 size 0x20 */
											/* 0x8104A0 size 0x20 */
	  	  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  	  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_spi_regs_t _SPIS1;				/* 0x8104C0 size 0x20 */
											/* 0x8104E0 size 0x20 */
	  	  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  	  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_i2cm_regs_t _I2CM;				/* 0x810500 size 0x09 */
	  	  	  	  	  	  	  	  	  	  	/* 0x810509 size 0x07 */
	  	  uint8_t:8; uint16_t:16; uint32_t:32;
	  ft900_i2cs_regs_t _I2CS;				/* 0x810510 size 0x09 */
											/* 0x810519 size 0x07 */
	  	  uint8_t:8; uint16_t:16; uint32_t:32;
	  ft900_uart_regs_t _UART0;				/* 0x810520 size 0x08 */
	  	  uint32_t:32; uint32_t:32; 		/* 0x810529 size 0x08 */
	  ft900_uart_regs_t _UART1;				/* 0x810530 size 0x08 */
		  uint32_t:32; uint32_t:32; 		/* 0x810539 size 0x08 */
	  ft900_uart_regs_t _UART2;				/* 0x810540 size 0x08 */
		  uint32_t:32; uint32_t:32; 		/* 0x810549 size 0x08 */
	  ft900_uart_regs_t _UART3;				/* 0x810550 size 0x08 */
		  uint32_t:32; uint32_t:32; 		/* 0x810559 size 0x08 */
	  ft900_timer_wdt_regs_t _TIMER;		/* 0x810560 size 0x10 */
	  	  	  	  	  	  	  	  	  	  	/* 0x810570 size 0x40 */
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_pwm_regs_t _PWM;				/* 0x8105B0 size 0x3E */
} ft930_register_block_t;

#else // __FT930__

typedef struct  {
	  ft900_sys_regs_t _SYS;     			/* 0x810000 size 0x1C */
	  ft900_gpio_regs_t _GPIO;				/* 0x81001C size 0x8C */
	  ft900_eth_phy_conf_reg_t _ETHPHY;		/* 0x8100A8 size 0x08 */
	  ft900_adc_dac_regs_t _ADCDAC;			/* 0x8100B0 size 0x0C */
	  ft900_sys_reset_regs_t _SYS_RST;		/* 0x8100BC size 0x04 */
	  ft900_interrupt_regs_t _INTERRUPT;	/* 0x8100C0 size 0x24 */
	  	  	  	  	  	  	  	  	  	  	/* 0x8100E4 size 0x1C */
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_ehci_regs_t _EHCI;				/* 0x810100 size 0x7C */
	  	  uint32_t:32;						/* 0x81017C size 0x04 */
	  ft900_usbd_regs_t _USBD;				/* 0x810180 size 0x9C */
	  	  uint32_t:32;						/* 0x81021C size 0x04 */
	  ft900_eth_regs_t _ETH;				/* 0x810220 size 0x13 */
	  	  uint32_t:32; uint8_t:8;			/* 0x810233 size 0x0D */
	  ft900_can_regs_t _CAN0;				/* 0x810240 size 0x1C */
	  	  uint32_t:32;						/* 0x81017C size 0x04 */
	  ft900_can_regs_t _CAN1;				/* 0x810260 size 0x1C */
	  	  uint32_t:32;						/* 0x81017C size 0x04 */
	  ft900_rtc_legacy_regs_t _RTCL;		/* 0x810280 size 0x20 */
	  ft900_spi_regs_t _SPIM;				/* 0x8102A0 size 0x20 */
	  ft900_spi_regs_t _SPIS0;				/* 0x8102C0 size 0x20 */
	  ft900_spi_regs_t _SPIS1;				/* 0x8102E0 size 0x20 */
	  ft900_i2cm_regs_t _I2CM;				/* 0x810300 size 0x09 */
	  	  	  	  	  	  	  	  	  	  	/* 0x810309 size 0x07 */
	  	  uint8_t:8; uint16_t:16; uint32_t:32;
	  ft900_i2cs_regs_t _I2CS;				/* 0x810310 size 0x09 */
											/* 0x810319 size 0x07 */
	  	  uint8_t:8; uint16_t:16; uint32_t:32;
	  ft900_uart_regs_t _UART0;				/* 0x810320 size 0x08 */
	  	  uint32_t:32; uint32_t:32; 		/* 0x810319 size 0x08 */
	  ft900_uart_regs_t _UART1;				/* 0x810330 size 0x08 */
	  	  uint32_t:32; uint32_t:32; 		/* 0x810319 size 0x08 */
	  ft900_timer_wdt_regs_t _TIMER;		/* 0x810340 size 0x10 */
	  ft900_i2s_regs_t _I2S;				/* 0x810350 size 0x10 */
	  ft900_cam_regs_t _CAM;				/* 0x810360 size 0x10 */
	  	  	  	  	  	  	  	  	  	  	/* 0x810370 size 0x50 */
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
		  uint32_t:32; uint32_t:32; uint32_t:32; uint32_t:32;
	  ft900_pwm_regs_t _PWM;				/* 0x8103C0 size 0x3E */
} ft900_register_block_t;

#endif // !__FT930__

/** @brief Enable mapping of the peripheral register block.
 * @details This can only be called ONCE to setup a global variable called
 * "peripherals" which will map directly to the peripheral register block.
 * Registers can be accessed using the structure members from code or can
 * be displayed in a debugger watch window.
 * Modification (and for that matter) reading of the values is allowed via
 * a debugger watch window with the caveat that the behaviour of the
 * registers is the same as if they were accessed by code.
 * e.g. FIFO registers will be affected and write to clear bits will be acted
 * upon.On FT90x the EHCI memory mapped space can be examined as well using
 */
//{
#if defined(__FT930__)

#define ENABLE_PERIPHERALS_DEBUG volatile ft930_register_block_t peripherals \
	__attribute__((section (".peripheralsSection")));

#else // __FT930__

#define ENABLE_PERIPHERALS_DEBUG volatile ft900_register_block_t peripherals \
	__attribute__((section (".peripheralsSection")));
#define ENABLE_EHCI_RAM_DEBUG volatile ft900_ehci_ram_t ehci_ram[EHCI_RAM_SIZE/sizeof(ft900_ehci_ram_t)] \
	__attribute__((section (".ehciSection")));

#endif // !__FT930__
//}

#endif /* FT900_REGISTER_DEBUG_H_ */
