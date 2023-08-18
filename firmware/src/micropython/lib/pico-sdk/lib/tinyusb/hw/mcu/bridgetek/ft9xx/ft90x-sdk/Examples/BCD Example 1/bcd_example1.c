/**
  @file
  @brief
  Battery charge detection in device mode example

  Displays the charging port the USB device is connected to.
*/
/*
 * ============================================================================
 * History
 * =======
 * UKB 03 Feb 2016 : Created
 *
 * (C) Copyright, Bridgetek Pte Ltd
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

#include <stdint.h>
#include "ft900.h"
#include "tinyprintf.h"

#if defined(__FT930__)
#define USBD_VBUS_DTC_PIN			39
#else
#define USBD_VBUS_DTC_PIN			3
#endif

static int bcd_detect = 0;
static volatile int vbus_detect = 0;

void setup(void);
void power_management_ISR(void);
void loop(void);
void myputc(void* p, char c);


int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void __attribute__((optimize("O0"))) ft900_bcd_detection(void)
{
	uint8_t reg;


	/* Step 1: */
	// Enable Chip USB device clock/PM configuration.
	sys_enable(sys_device_usb_device);

	/* Step 2: BCD Init */
	SYS->MSC0CFG |= MASK_SYS_MSC0CFG_BCD_SOFTRESET; /* Generate software reset to BCD Dev : if BCDDEV_EN = 1. It is automatically cleared immediately */

	SYS->MSC0CFG |=  (MASK_SYS_MSC0CFG_BCDDEV_EN 					/* enable BCD Device */
						| MASK_SYS_MSC0CFG_BCDDEV_SD_EN 			/* enable secondary detection */
						| MASK_SYS_MSC0CFG_BCDDEV_VDP_EN_POST_DCP 	/* enable connection of VDP_SRC after DCP detection */
						| MASK_SYS_MSC0CFG_BCDDEV_LGC_COMP_INHIB); 	/* disable logic comparison during BCD detections */

	/* Step 3: Check Detection */
	reg = (uint8_t)SYS->MSC0CFG;


#if 0
    while(!(reg & MASK_SYS_MSC0CFG_BCDDEV_DETECT_RUNNING))
    {
    	reg = (uint8_t)SYS->MSC0CFG; /* Wait until BCD detection running goes HIGH */
    }
#endif

    while(reg & MASK_SYS_MSC0CFG_BCDDEV_DETECT_RUNNING)
    {
    	reg = (uint8_t)SYS->MSC0CFG; /* Wait until BCD detection running goes LOW */
    }

    while(!(reg & MASK_SYS_MSC0CFG_BCDDEV_DETECT_COMPLETE))
    {
    	reg = (uint8_t)SYS->MSC0CFG; /* Wait until BCD detection completion */
    }

	reg = (uint8_t)SYS->MSC0CFG;

	/* Step 5: End BCD detection */
	SYS->MSC0CFG |= MASK_SYS_MSC0CFG_BCD_SOFTRESET; /* Generate software reset to BCD Dev : if BCDDEV_EN = 1. It is automatically cleared immediately */

	if (vbus_detect) /* Check if we are still attached */
	{
		if (reg & 1)
		{
			/* DCP mode found */
			tfp_printf("DCP mode found \r\n");
		}
		else if (reg & 2)
		{
			/* CDP mode found */
			tfp_printf("CDP mode found \r\n");
		}
		else if (reg & 4)
		{
			/* SDP mode found */
			tfp_printf("SDP mode found \r\n");
		}
		else
		{
			/* No charging port detected */
			/* or Wrong mode detected */
			tfp_printf("No charging port detected !!!!, reg: %02X \r\n", reg);
		}
	}


}


void setup(void)
{

    /* Enable the UART Device... */
    sys_enable(sys_device_uart0);
#if defined(__FT930__)
    /* Make GPIO23 function as UART0_TXD and GPIO22 function as UART0_RXD... */
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#else
	/* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
	gpio_function(48, pad_uart0_txd); /* UART0 TXD */
	gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#endif
    uart_open(UART0,                    /* Device */
              1,                        /* Prescaler = 1 */
			  UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */

    /* Print out a welcome message... */
    uart_puts(UART0,
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "(C) Copyright, Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to BCD Example... \r\n"
        "\r\n"
        "Displays the charging port that the USB device is connected to...\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(UART0,myputc);

/* This example performs BCD detection whenever a USB connection is detected.
 * Otherwise, BCD is independent of VBUS detection.
 */
	// Disable device connect/disconnect/host reset detection.
	SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_DIS_DEV;
	SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_CONN_DEV;
	SYS->PMCFG_L = SYS->PMCFG_L & (~MASK_SYS_PMCFG_DEV_DETECT_EN);

	// For detecting 'USB attach and detach events'
	interrupt_attach(interrupt_0, (int8_t)interrupt_0, power_management_ISR);

	// Setup VBUS detect GPIO. If the device is connected then this
	// will set the MASK_SYS_PMCFG_DEV_DETECT_EN bit in PMCFG.
	gpio_interrupt_disable(USBD_VBUS_DTC_PIN);
	gpio_function(USBD_VBUS_DTC_PIN, pad_vbus_dtc);
	gpio_pull(USBD_VBUS_DTC_PIN, pad_pull_pulldown);
	gpio_dir(USBD_VBUS_DTC_PIN, pad_dir_input);

	SYS->PMCFG_L = SYS->PMCFG_L | MASK_SYS_PMCFG_DEV_DETECT_EN;
#if defined(__FT930__)
	// Setup VBUS detect
	SYS->MSC0CFG = SYS->MSC0CFG | MASK_SYS_MSC0CFG_USB_VBUS_EN;
#endif

	delayms(1);
	if (vbus_detect == 0)
	{
		tfp_printf("Please plug in your USB cable \r\n");
	}

}

/* Power management ISR */
void power_management_ISR(void)
{

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_DEV_CONN_DEV)
	{
		// Clear connection interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_CONN_DEV;
		vbus_detect = 1;
		bcd_detect = 1;
	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_DEV_DIS_DEV)
	{
		// Clear disconnection interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_DIS_DEV;
		vbus_detect = 0;

	}
}

void loop(void)
{
	if(bcd_detect)
	{
		bcd_detect = 0;
		ft900_bcd_detection();
	}
}

void myputc(void* p, char c)
{

	uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}
