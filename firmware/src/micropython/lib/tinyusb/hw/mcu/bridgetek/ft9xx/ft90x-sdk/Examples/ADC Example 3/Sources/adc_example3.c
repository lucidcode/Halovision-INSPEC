/**
  @file
  @brief
  ADC Example 3

  Use ADC interrupt to capture multiple channels ADC samples and stored in circular buffer,  print 16 samples of different ADC channel in regular interval.
*/
/*
 * ============================================================================
 * History
 * =======
 * 2017-08-31 : Created
 *
 * (C) Copyright Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 *  ("Bridgetek ") subject to the licence terms set out
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
#include <stdbool.h>
#include <ft900.h>
#include "tinyprintf.h"
#include <string.h>
void setup(void);
void myputc(void* p, char c);
void adcISR(void);
void loop(void);

/*switch adc to half frequency (12.5Mhz to 6.25Mhz)*/
#define SWITCH_FREQUENCY
/*switch adc resolution (from 10bit to 8bit)*/
#define SWITCH_RESOLUTION
#define ADC_DATA (ADCDAC->DAC_DATA0_ADC_DATA)

#define ADC_INTERRUPT_READ_SAMPLE			64 // Must be 64
#define ADC_NUMBER_OF_BUFFER_COUNT			32
#define ADC_TOTAL_SAMPLE_COUNT				(ADC_NUMBER_OF_BUFFER_COUNT*ADC_INTERRUPT_READ_SAMPLE)
#define ADC_BUFFER_STOP_COUNT				((ADC_NUMBER_OF_BUFFER_COUNT/2)*ADC_INTERRUPT_READ_SAMPLE)

#if defined(__FT900__)
#define ADC_CHANNEL_COUNT					7
#define ADC_CHANNEL_MASK					7
#else
#define ADC_CHANNEL_COUNT					3
#define ADC_CHANNEL_MASK					3
#endif

#define SAMPLES_TO_IGNORE					1
#define ADC_APP_DUMP_SAMPLE_COUNT			16

/*
 *  Variables used for ADC buffer
 */
static uint16_t	g_u16AdcBuffer[ADC_TOTAL_SAMPLE_COUNT];
static volatile uint32_t g_uAdcIndex;

static uint16_t	 g_u16AdcChannels[ADC_CHANNEL_COUNT][ADC_APP_DUMP_SAMPLE_COUNT];

/* Variable uses to store current active ADC channel */
static int g_nChannel = 1;

/* Return number of samples captured */
uint32_t adc_sample_count()
{
	uint32_t count;
	CRITICAL_SECTION_BEGIN
	count = g_uAdcIndex;
    CRITICAL_SECTION_END
	return count;
}

/* Enable the ADC respective channel */
void adc_start_channel(int nChannel)
{
    /* Start the ADC nChannel... */
    adc_start(nChannel);
}

/* Stop the ADC capturing */
void adc_stop_channel(void)
{
	adc_stop();
}

/* Reset the ADC buffer pointer */
void buffer_init()
{
	g_uAdcIndex = 0;
}


int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

void setup(void)
{
	sys_reset_all();
    /* Enable the UART Device... */
    sys_enable(sys_device_uart0);
    /* Set UART0 GPIO functions to UART0_TXD and UART0_RXD... */
#if defined(__FT900__)
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
#elif defined(__FT930__)
    gpio_function(23, pad_uart0_txd); /* UART0 TXD */
    gpio_function(22, pad_uart0_rxd); /* UART0 RXD */
#endif
    uart_open(UART0,                    /* Device */
              1,                        /* Prescaler = 1 */
			  UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
              uart_data_bits_8,         /* No. Data Bits */
              uart_parity_none,         /* Parity */
              uart_stop_bits_1);        /* No. Stop Bits */

    /* Enable tfp_printf() functionality... */
    init_printf(NULL,myputc);
    /* Print out a welcome message... */
    tfp_printf(
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to ADC Example 3... \r\n"
        "\r\n"
		"Use interrupt to capture samples from multiple ADC channels,  \r\n"
        " store them in a circular buffer and print %d samples of  \r\n"
		" different ADC channels at regular interval.  \r\n"
        "--------------------------------------------------------------------- \r\n", ADC_APP_DUMP_SAMPLE_COUNT
        );

    /* Enable the ADCs... */
    sys_enable(sys_device_adc);

#if defined(__FT900__)
    /* Set GPIO6 to ADC1... */
    gpio_function(6, pad_adc1);
    /* Set GPIO7 to ADC2... */
    gpio_function(7, pad_adc2);
    /* Set GPIO8 to ADC3... */
    gpio_function(8, pad_adc3);
    /* Set GPIO9 to ADC4... */
    gpio_function(9, pad_adc4);
    /* Set GPIO10 to ADC5... */
    gpio_function(10, pad_adc5);
    /* Set GPIO11 to ADC6... */
    gpio_function(11, pad_adc6);
    /* Set GPIO12 to ADC7... */
    gpio_function(12, pad_adc7);
#endif

    /* Continuously read data... */
    adc_mode(adc_mode_continuous);

#if defined(SWITCH_RESOLUTION)
	adc_select_resolution(adc_8bit);
#endif
#if defined(SWITCH_FREQUENCY)
	adc_select_frequency(adc_6_25_MHz);
#endif
	/* Register the ADC interrupt... */
	interrupt_attach(interrupt_adc, (uint8_t) interrupt_adc, adcISR);
	interrupt_enable_globally();

	/* Initialize ADC buffer pointer */
	buffer_init();
	/* Enable the ADC to fire an interrupt... */
	adc_enable_interrupt();
    /* Start the ADC ... */
	adc_start_channel(g_nChannel);
}

void myputc(void* p, char c)
{
    uart_write(UART0, (uint8_t)c);
}

void adcISR()
{
    if (adc_is_interrupted())
    {
    	/* Get number of available sample available in hardware FIFO */
    	uint32_t uCount = adc_available();

    	if (0 < uCount) {
			/*
			 * Safe guard prevent overwrite end of buffer
			 */
			if (ADC_TOTAL_SAMPLE_COUNT <= (g_uAdcIndex+uCount)) {
				g_uAdcIndex = 0;
			}
			adc_readn(&g_u16AdcBuffer[g_uAdcIndex], uCount);
			g_uAdcIndex += uCount;
    	}
    }
}

void loop(void)
{
	uint32_t count = adc_sample_count();
	/* Check whether samples captured has more than ADC_BUFFER_STOP_COUNT */
	if (ADC_BUFFER_STOP_COUNT <= count) {

		/* Stop the ADC capturing */
		adc_stop_channel();

		/* We only copy ADC_APP_DUMP_SAMPLE_COUNT samples from the global ADC buffer */
		memcpy(&g_u16AdcChannels[g_nChannel-1][0], &g_u16AdcBuffer[SAMPLES_TO_IGNORE], ADC_APP_DUMP_SAMPLE_COUNT*sizeof(uint16_t));
		
		/* Output the respective ADC Channel samples */
		tfp_printf("ADC Channel %d\n", g_nChannel);
		for (int i=0; i<ADC_APP_DUMP_SAMPLE_COUNT; i++) {
			tfp_printf("%04d ", (uint16_t)g_u16AdcChannels[g_nChannel-1][i]);
		}
		tfp_printf("\n");

		/* Re-initialize the global ADC buffer index pointer */
		buffer_init();
		/* Switching ADC Channel */
		g_nChannel++;
		g_nChannel &= ADC_CHANNEL_MASK;
		/* Check whether is it wrap around */
		if (!g_nChannel)
			g_nChannel++;

		/* Delay 1 seconds */
		delayms(1000);
		/* Restart ADC capturing */
		adc_start_channel(g_nChannel);
		
	}
}
