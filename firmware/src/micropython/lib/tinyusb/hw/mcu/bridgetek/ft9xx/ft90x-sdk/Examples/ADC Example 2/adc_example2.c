/**
  @file
  @brief
  ADC Example 2

  Use interrupt to capture samples and stored in circular buffer,
  print 64 samples of ADC 1 from circular buffer in regular interval.
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
#include <ft900.h>
#include "tinyprintf.h"
#include <string.h>

void setup(void);
void myputc(void* p, char c);
void adcISR(void);
void loop(void);

#define ADC_DATA (ADCDAC->DAC_DATA0_ADC_DATA)

#define ADC_INTERRUPT_READ_SAMPLE			64 // Must be 64
#define ADC_NUMBER_OF_BUFFER_COUNT			32 // Value must be power of 2, the more data print the larger the value
#define ADC_CIRCULAR_TOTAL_SAMPLE_COUNT		(32*ADC_INTERRUPT_READ_SAMPLE)
#define ADC_INTERRUPT_READ_SAMPLE_MASK		(ADC_CIRCULAR_TOTAL_SAMPLE_COUNT-1)
#define MAX_ADC_CAPTURE_SAMPLE_COUNT		1024

/*
 *  Variables used for circular buffer
 */
static uint16_t	 g_u16AdcBuffer[ADC_CIRCULAR_TOTAL_SAMPLE_COUNT];
static volatile uint32_t g_uHead;
static volatile uint32_t g_uTail;
/*
 * Variable is used for application loop dumping sample from circular buffer.
 */
static uint16_t	 g_u16AppBuffer[ADC_INTERRUPT_READ_SAMPLE];

/*
 * Return number of unread samples stored in circular buffer(g_u16AdcBuffer)
 */
static __inline uint32_t adc_sample_available()
{
	uint32_t uAvail;
	CRITICAL_SECTION_BEGIN
	uAvail = g_uHead - g_uTail;
	uAvail &= ADC_INTERRUPT_READ_SAMPLE_MASK;
	CRITICAL_SECTION_END
	return uAvail;
}

/*
 *  pBuf:		pointer to buffer for writing adc samples
 *  uSamples:	number of sample to copy
 *
 *  Application should call adc_sample_available() for uSamples to read.
 */
static __inline void adc_read_samples(uint16_t* pBuf, uint32_t uSamples)
{
	uint32_t uRead;
	uint16_t* pSrc;

	CRITICAL_SECTION_BEGIN
	while (0 < uSamples)
	{
    	pSrc = &g_u16AdcBuffer[g_uTail];
		uRead = ADC_CIRCULAR_TOTAL_SAMPLE_COUNT - g_uTail;
    	if (uRead > uSamples) {
    		uRead = uSamples;
    	}
		memcpy(pBuf, pSrc, uRead*2);
		pBuf += uRead;
		uSamples -= uRead;
		g_uTail += uRead;
		g_uTail &= ADC_INTERRUPT_READ_SAMPLE_MASK;
	}
	CRITICAL_SECTION_END
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

    /* Print out a welcome message... */
    uart_puts(UART0,
        "\x1B[2J" /* ANSI/VT100 - Clear the Screen */
        "\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
        "Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to ADC Example 2... \r\n"
        "\r\n"
        "Use interrupt to capture samples and store them in a circular buffer, \n"
		" print 64 samples of ADC 1 from the circular buffer at a regular interval.\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(NULL,myputc);
    /* Enable the ADCs... */
    sys_enable(sys_device_adc);

#if defined(__FT900__)
    /* Set GPIO6 to ADC1... */
    gpio_function(6, pad_adc1);
#endif

    /* Continuously read data... */
    adc_mode(adc_mode_continuous);

	/* Register the ADC interrupt... */
	interrupt_attach(interrupt_adc, (uint8_t) interrupt_adc, adcISR);
	interrupt_enable_globally();

	/* Enable the ADC to fire an interrupt... */
	adc_enable_interrupt();

    /* Start the ADC and use ADC1... */
    adc_start(1);
    
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
    		/* Hardware FIFO return 1 less sample than actual count,
    		 * so we need incrementing 1 more sample to read
    		 */
			uCount++;
			uint32_t uRead;
	    	uint16_t* pBuf;
	    	while (0 < uCount) {
	    		/*
	    		 * We need to make sure we won't write over to end of circular buffer,
	    		 * uRead is the count from g_uHead to the end of circular buffer.
	    		 * */
				uRead = ADC_CIRCULAR_TOTAL_SAMPLE_COUNT - g_uHead;
		    	/* pBuf will point to start of current g_uHead location */
				pBuf = &g_u16AdcBuffer[g_uHead];
		    	/* Make sure we won't read more than available to read */
		    	if (uRead > uCount) {
		    		uRead = uCount;
		    	}
		    	adc_readn(pBuf, uRead);
				//asm_streamin16(&ADCDAC->DAC_DATA0_ADC_DATA, pBuf, uRead*2);
				g_uHead += uRead;
		    	g_uHead &= ADC_INTERRUPT_READ_SAMPLE_MASK;
		    	uCount -= uRead;
			}
    	}
    }
}

void loop(void)
{
#define LOOP_COUNT			0x400
#define LOOP_COUNT_MASK		(LOOP_COUNT - 1)
	static uint32_t uLoopCount = 0;
	/* uAdcCount stores number of un-read sample from circular buffer */
	uint32_t uAdcCount = adc_sample_available();

	if (ADC_INTERRUPT_READ_SAMPLE <= uAdcCount) {
		while (ADC_INTERRUPT_READ_SAMPLE <= uAdcCount)
		{
			// We keep consume adc buffer from circular buffer
			adc_read_samples(g_u16AppBuffer, ADC_INTERRUPT_READ_SAMPLE);
			uAdcCount -= ADC_INTERRUPT_READ_SAMPLE;
		}
		uLoopCount++;
		uLoopCount &= LOOP_COUNT_MASK;
		/* We will only output samples for every LOOP_COUNT count */
		if (!uLoopCount) {
			for (uint32_t i=0; i<ADC_INTERRUPT_READ_SAMPLE; i++) {
				tfp_printf("%03d ", (uint16_t)g_u16AppBuffer[i]);
			}
			tfp_printf("\n\n");
		}
	}
	else {
		/* Samples less than ADC_INTERRUPT_READ_SAMPLE, delay 1 ms and return */
		delayms(1);
	}
}
