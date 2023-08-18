/**
    @file

    @brief
    Simple UART

    
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
 *  ("Bridgetek") subject to the licence terms set out
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
#include <string.h>
#include <stdlib.h>
#include "ft900_uart_simple.h"
#include <ft900_asm.h>
#include <registers/ft900_registers.h>
#include <private/ft900_internals.h>


/* CONSTANTS ***********************************************************************/
#define PRESCALER_MAX       (31)
#define DIVISOR_MAX         (65535)
#define MODE_650			(0xBF)/*Constant to be written to LCR register to access 650 mode registers*/

/* TYPES ***************************************************************************/

/**
 @struct lc_regs
 @brief structure holding local
 copy of LCR and ACR registers
 **/
typedef struct
{
	uint8_t acr_copy;
	uint8_t lcr_copy;
} lc_regs;

/**
 @struct reg_copy_t
 @brief structure holding local
 copy of LCR and ACR registers
 for different uart channels
 **/
typedef struct
{
	lc_regs uart0;
	lc_regs uart1;
#if defined(__FT930__)
lc_regs uart2;
lc_regs uart3;
#endif
} reg_copy_t;

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/
#if defined(__FT900__)
reg_copy_t reg_copy = { { 0, 0 }, { 0, 0 } };
#elif defined(__FT930__)
reg_copy_t reg_copy = { {0,0}, {0,0}, {0,0}, {0,0}};
#endif
/* MACROS **************************************************************************/

#define uart_wait_whilst_transmitting(pUart) \
    while (!(pUart->LSR_ICR_XON2 & MASK_UART_LSR_THRE))
#define uart_wait_whilst_receiving(pUart) \
    while (!(pUart->LSR_ICR_XON2 & MASK_UART_LSR_DR))

/* PROTOTYPES***********************************************************************/
static inline lc_regs* uart_get_copy(ft900_uart_regs_t *dev);
static inline uint8_t uart_lcr_lastWrite(ft900_uart_regs_t *dev);
static inline void uart_spr_write(ft900_uart_regs_t *uart, uint8_t location,
	uint8_t val);
static inline uint8_t uart_spr_read(ft900_uart_regs_t *uart, uint8_t location);
static inline void uart_enable_650mode(ft900_uart_regs_t *dev);
static inline void uart_disable_650mode(ft900_uart_regs_t *dev);
static int8_t uart_write_index_register(ft900_uart_regs_t *dev, uint32_t reg,
	uint8_t val);
static uint8_t uart_read_index_register(ft900_uart_regs_t *dev, int8_t reg);
static inline void uart_write_lcr(ft900_uart_regs_t *dev, uint8_t val);
static inline uint8_t uart_read_lcr(ft900_uart_regs_t *dev);

/* LOCAL FUNCTIONS / INLINES *******************************************************/
static inline lc_regs* uart_get_copy(ft900_uart_regs_t *dev)
{
	lc_regs *local = NULL;
	switch ((uint32_t) dev)
	{
	case (uint32_t) UART0:
		local = &reg_copy.uart0;
		break;
	case (uint32_t) UART1:
		local = &reg_copy.uart1;
		break;
	#if defined(__FT930__)
		case (uint32_t)UART2:
		local = &reg_copy.uart2;
		break;
		case (uint32_t)UART3:
		local = &reg_copy.uart3;
		break;
	#endif
	default:
		break;
}

	return local;
}

static inline void uart_spr_write(ft900_uart_regs_t *uart, uint8_t location,
	uint8_t val)
{
	if (location == OFFSET_UART_SPR_ACR)
	{
		lc_regs *local = uart_get_copy(uart);
		local->acr_copy = val; /*make a copy for acr val*/
	}
	/* A memory barrier is necessary to stop the compiler from optimizing away multiple writes to the
	 LSR_ICR_XON2 when this function is inlined */
	uart->SPR_XOFF2 = location;
	uart->LSR_ICR_XON2 = val;
	__asm__("" ::: "memory");
}

static inline uint8_t uart_spr_read(ft900_uart_regs_t *uart, uint8_t location)
{
	uint8_t val = 0;
	if (location == OFFSET_UART_SPR_ACR)
	{
		lc_regs *local = uart_get_copy(uart);
		val = local->acr_copy; /*return the previous written value of acr*/
	}
	else
	{
		/* A memory barrier is necessary to stop the compiler from optimizing away multiple writes to the
		 LSR_ICR_XON2 when this function is inlined */
		uart->SPR_XOFF2 = location;
		val = uart->LSR_ICR_XON2;
		__asm__("" ::: "memory");
	}
	return val;
}


/** @brief Enable xBF mode
 *	@details To access 650 registers 0xBF has
 *	to be written into LCR register
 *  @param dev The device to use
 *
 *  @returns void
 */
static inline void uart_enable_650mode(ft900_uart_regs_t *dev)
{
	uart_write_lcr(dev, MODE_650);
}

/** @brief Disable xBF mode
 *	@details To access normal registers other
 *	than 650 registers, some value other than
 *	 0xBF has to be written into LCR register
 *  @param dev The device to use
 *
 *  @returns void
 */
static inline void uart_disable_650mode(ft900_uart_regs_t *dev)
{
	uart_write_lcr(dev, uart_read_lcr(dev)); /*uart_read_lcr() returns lcr value removing xBF mode*/
}

/** @brief Write value to LCR register
 *  @param dev The device to use
 *	@param val input value
 *
 *  @returns void
 */
static inline void uart_write_lcr(ft900_uart_regs_t *dev, uint8_t val)
{
	lc_regs *local = uart_get_copy(dev);
	local->lcr_copy = val;
	dev->LCR_RFL = val;
	__asm__("" ::: "memory");

}

/** @brief Get last value Written to LCR
 * 	@details To access some of the registers
 * 	last value written to LCR should not be
 * 	0xBF. This function returns last value
 * 	written to LCR
 *  @param dev The device to use
 *
 *  @returns last value of LCR
 */
static inline uint8_t uart_lcr_lastWrite(ft900_uart_regs_t *dev)
{
	lc_regs *local = uart_get_copy(dev);
	return local->lcr_copy;
}

/** @brief Read LCR register
 *  @param dev The device to use
 *
 *  @returns LCR register contents
 */
static inline uint8_t uart_read_lcr(ft900_uart_regs_t *dev)
{
	uint8_t tempLCR = 0;

	lc_regs *local = uart_get_copy(dev);
	if (local->lcr_copy == MODE_650)
	{
		tempLCR = dev->LCR_RFL & ~MASK_UART_LCR_DLAB; /*writing 0xBF to LCR only sets DLAB bit, remaining unchanged*/
	}
	else
		tempLCR = dev->LCR_RFL;

	return tempLCR;
}

/** @brief Write index register
 *  @param dev The device to use
 *  @param reg index register
 *  @param input value
 *
 *  @returns 0 on success, -1 otherwise (invalid device).
 */
static int8_t uart_write_index_register(ft900_uart_regs_t *dev, uint32_t reg,
	uint8_t val)
{
	int8_t iRet = 0;
	uint8_t tempLCR = 0;
	if ((dev == NULL) || (reg > OFFSET_UART_SPR_GDS))
	{
		/* Unknown device */
		iRet = -1;
	}

	if (uart_lcr_lastWrite(dev) == MODE_650)
	{
		tempLCR = MODE_650;
		uart_disable_650mode(dev);
	}

	uart_spr_write(dev, reg, val);

	if (tempLCR == MODE_650)
	{
		uart_enable_650mode(dev);
	}

	return iRet;
}

/** @brief read index register
 *  @param dev The device to use
 *  @param reg index register
 *
 *   @returns index register contents.
 */
static uint8_t uart_read_index_register(ft900_uart_regs_t *dev, int8_t reg)
{
	int8_t val = 0;
	uint8_t tempLCR = 0;

	if (uart_lcr_lastWrite(dev) == MODE_650)
	{
		tempLCR = MODE_650;
		uart_disable_650mode(dev);
	}

	val = uart_spr_read(dev, OFFSET_UART_SPR_ACR);
	if (reg != OFFSET_UART_SPR_ACR)
	{
		uint8_t temp;
		uart_spr_write(dev, OFFSET_UART_SPR_ACR, val | MASK_UART_ACR_ICR); /*Enable access, ACR[6] (ICR) <- '1'*/
		temp = uart_spr_read(dev, reg); /*get the value*/
		uart_spr_write(dev, OFFSET_UART_SPR_ACR, val & ~MASK_UART_ACR_ICR);/*Disable access, ACR[6] (ICR) <- '0'*/
		val = temp;
	}

	if (tempLCR == MODE_650)
	{
		uart_enable_650mode(dev);
	}

	return val;
}

/** @brief Read Additional Status Register(950 mode)
 *  @param dev The device to use
 *
 *  @returns Contents of ASR register.
 */
static inline uint8_t uart_read_asr(ft900_uart_regs_t *dev)
{
	uint8_t val, acr;

	acr = uart_read_index_register(dev, OFFSET_UART_SPR_ACR);
	uart_write_index_register(dev, OFFSET_UART_SPR_ACR, (acr | MASK_UART_ACR_ASE)); /*ACR[7] = 1 for accessing ASE registers*/
	val = dev->IER_DLH_ASR;
	uart_write_index_register(dev, OFFSET_UART_SPR_ACR, acr); /*revert access*/
	return val;
}

/** @brief Write Additional Status Register(950 mode)
 * 	@details only bit 0 and bit 1 are writable in ASR
 *  @param dev The device to use
 *  @param data input data
 *
 *   @returns 0 on success, -1 otherwise (invalid parameter).
 */
static inline int8_t uart_write_asr(ft900_uart_regs_t *dev, uint8_t data)
{
	uint8_t acr, iRet = 0;

	if (data & ~(MASK_UART_ASR_TX_DIS | MASK_UART_ASR_REM_TX_DIS)) /*only bits 0 and 1 are RW in ASR*/
	{
		iRet = -1;
	}

	if (iRet == 0)
	{
		acr = uart_read_index_register(dev, OFFSET_UART_SPR_ACR);
		uart_write_index_register(dev, OFFSET_UART_SPR_ACR,
				(acr | MASK_UART_ACR_ASE)); /*ACR[7] = 1 for accessing ASE registers*/
		dev->IER_DLH_ASR = data;
		uart_write_index_register(dev, OFFSET_UART_SPR_ACR,
				(acr & ~MASK_UART_ACR_ASE)); /*revert access*/
	}
	return iRet;
}

/* FUNCTIONS ***********************************************************************/

/** @brief Open a UART for communication
 *
 *  @param dev The device to use
 *  @param prescaler The value of the prescaler
 *  @param divisor The value of the divisor
 *  @param databits The number of data bits
 *  @param parity The parity scheme
 *  @param stop The number of stop bits
 *
 *  @warning 1.5 stop bits is only available in 5 bit mode
 *
 *  @returns 0 on success, -1 otherwise (invalid device).
 */
int8_t uart_open(ft900_uart_regs_t *dev, uint8_t prescaler, uint32_t divisor, uart_data_bits_t databits,
        uart_parity_t parity, uart_stop_bits_t stop)
{
	int8_t iRet = 0;
	uint8_t LCR_RFL;
	uint8_t tempLCR = 0;
    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
    	if (uart_lcr_lastWrite(dev) == MODE_650) {
			tempLCR = MODE_650;
			uart_disable_650mode(dev);
		}
//        uart_spr_write(dev, OFFSET_UART_SPR_ACR, 0x00);

        /* Write the prescaler... */
    	uart_write_index_register(dev, OFFSET_UART_SPR_CPR, prescaler << 3);
        /* Write the clock samples... */
    	uart_write_index_register(dev, OFFSET_UART_SPR_TCR, 4);

        /* Write the divisor... */
    	uart_write_lcr(dev, uart_read_lcr(dev) | MASK_UART_LCR_DLAB);/* Enable the divisor latch */
		dev->RHR_THR_DLL = (divisor >> 0) & 0xFF;
		dev->IER_DLH_ASR = (divisor >> 8) & 0xFF;
		uart_write_lcr(dev, uart_read_lcr(dev) & ~MASK_UART_LCR_DLAB); /* Disable the divisor latch */

        LCR_RFL = 0x00;
        /* Set up data bits... */
        switch (databits)
        {
            case uart_data_bits_5:
            case uart_data_bits_6:
            case uart_data_bits_7:
            case uart_data_bits_8:
                LCR_RFL = (databits - uart_data_bits_5) << BIT_UART_LCR_WLS;
            break;
	case uart_data_bits_9:
		uart_write_index_register(dev, OFFSET_UART_SPR_NMR,
				uart_read_index_register(dev, OFFSET_UART_SPR_NMR)|MASK_UART_NMR_9_EN);
		break;

            default: break;
        }


        /* Set up stop bits... */
        switch(stop)
        {
            case uart_stop_bits_1:
                LCR_RFL &= ~MASK_UART_LCR_STB;
            break;

            case uart_stop_bits_1_5:
                if (databits == uart_data_bits_5)
                {
                    LCR_RFL |= MASK_UART_LCR_STB;
                }
                else
                {
                    iRet = -1;
                }
            break;

            case uart_stop_bits_2:
                if (databits != uart_data_bits_5)
                {
                    LCR_RFL |= MASK_UART_LCR_STB;
                }
                else
                {
                    iRet = -1;
                }
            break;

            default: break;
        }


        /* Set up parity... */
        switch(parity)
        {
            case uart_parity_none:
                LCR_RFL &= ~MASK_UART_LCR_PEN; /* Disable parity */
            break;

            case uart_parity_odd:
                LCR_RFL |= MASK_UART_LCR_PEN; /* Enable parity */
                LCR_RFL &= ~MASK_UART_LCR_EPS;
            break;

            case uart_parity_even:
                LCR_RFL |= MASK_UART_LCR_PEN | MASK_UART_LCR_EPS; /* Enable parity */
            break;

            default: break;
        }
        uart_write_lcr(dev, uart_read_lcr(dev) | LCR_RFL);
//        dev->ISR_FCR_EFR = 0x00; /* FIFOs off 16450 mode. */
        dev->MCR_XON1_TFL = 0x02; /* RTS High */

        /* Enable transmit and receive */

        /* Check to see if there is any garbage left over */
        if (dev->LSR_ICR_XON2 & MASK_UART_LSR_DR)
        {
            FT900_ATTR_UNUSED char c;
            c = dev->RHR_THR_DLL;
        }

        if (tempLCR == MODE_650) {
			uart_enable_650mode(dev);
		}
    }

    return iRet;
}


/** @brief Calculate the prescaler and divisor from a baudrate
 *
 *  @param target_baud The baud rate to use
 *  @param samples The number of samples the UART will take for a bit, the default for this is 4
 *  @param f_perif Peripheral frequency, the default for this is 100,000,000
 *  @param divisor A pointer to store the divisor
 *  @param prescaler A pointer to store the prescaler, if this is NULL the prescaler will be set to 1
 *
 *  Baud rate is calculated by:
 *  \f[ baud = f_{peripherals} / (N_{samples} \times Divisor \times Prescaler) \f]
 *
 *  For \f$ N_{samples} = 4 \f$ the following baud rates can be obtained from
 *  these Divisors and Prescalers:
 *
 *  | Desired baud | Prescaler | Divisor | Actual baud   | Error   |
 *  | :----------- | --------: | ------: | ------------: | ------: |
 *  | 110          | 4         |   56818 |     110.00035 | ~0.000% |
 *  | 150          | 3         |   55555 |     150.00150 | +0.001% |
 *  | 300          | 3         |   27778 |     299.99760 | ~0.000% |
 *  | 1200         | 1         |   20833 |    1200.01920 | +0.002% |
 *  | 2400         | 1         |   10417 |    2399.92320 | -0.003% |
 *  | 4800         | 1         |    5208 |    4800.30720 | +0.006% |
 *  | 9600         | 1         |    2604 |    9600.61440 | +0.006% |
 *  | 31250        | 1         |     800 |   31250.00000 |  0.000% |
 *  | 38400        | 1         |     651 |   38402.45800 | +0.006% |
 *  | 57600        | 1         |     434 |   57603.68700 | +0.006% |
 *  | 115200       | 1         |     217 |  115207.37000 | +0.006% |
 *  | 230400       | 1         |     109 |  229357.80000 | -0.452% |
 *  | 460800       | 1         |      54 |  462962.96000 | -0.469% |
 *  | 921600       | 1         |      27 |  925925.93000 | -0.469% |
 *  | 1000000      | 1         |      25 | 1000000.00000 |  0.000% |
 *
 *  @returns The absolute error from the target baud rate
 */
int32_t uart_calculate_baud(uint32_t target_baud, uint8_t samples, uint32_t f_perif, uint16_t *divisor, uint8_t *prescaler)
{
    uint32_t lBaud = 0;
    uint16_t wDivisor = 0;
    uint8_t bPrescaler = 1, bPrescalerLoopEnd = PRESCALER_MAX+1;
    uint16_t wHopSize = 0;

    uint8_t bPrescalerBestMatch = 1;
    uint16_t wDivisorBestMatch = 65535;
    int32_t lBaudErrorBestMatch = target_baud;

    /* Only work on a prescaler of 1 if the user doesn't ask for a prescaler back */
    if (prescaler == NULL) bPrescalerLoopEnd = 2;

    /* Loop through all the possible current permutations to calculate the baud rate */
    /* Exit after all calculations or if we find an exact match */

    for (bPrescaler = 1; (bPrescaler < bPrescalerLoopEnd) && (lBaudErrorBestMatch != 0UL); ++bPrescaler)
    {
        /* Binary search through the available range, hone in on the right divisor... */
        wDivisor = wHopSize = (DIVISOR_MAX / 2) + 1; /* log2(2^16) hops = 16 hops */

        while((wHopSize > 0) && (lBaudErrorBestMatch != 0UL))
        {
            lBaud = f_perif / (samples * wDivisor * bPrescaler);

            /* Check to see if we got any closer to the target */
            if ((lBaud - target_baud) < labs(lBaudErrorBestMatch))
            {
                /* We found a better match... */
                lBaudErrorBestMatch = lBaud - target_baud;
                wDivisorBestMatch = wDivisor;
                bPrescalerBestMatch = bPrescaler;
            }

            /* Half our jump size */
            wHopSize = wHopSize >> 1;

            if (lBaud < target_baud)
            {
                /* Too slow, lower divisor */
                wDivisor = wDivisor - wHopSize;
            }
            else
            {
                /* Too fast, higher divisor */
                wDivisor = wDivisor + wHopSize;
            }
        }
    }


    /* Copy out the calculated settings */
    *divisor = wDivisorBestMatch;
    *prescaler = bPrescalerBestMatch;

    /* Return what our error was */
    return lBaudErrorBestMatch;
}


/** @brief Close a UART for communication
 *  @param dev The device to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t uart_close(ft900_uart_regs_t *dev)
{
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
        /* Disable the clock to the baudrate generator */
        dev->LCR_RFL |= MASK_UART_LCR_DLAB; /* Enable the divisor latch */
        dev->RHR_THR_DLL = 0;
        dev->IER_DLH_ASR = 0;
        dev->LCR_RFL &= ~MASK_UART_LCR_DLAB; /* Disable the divisor latch */
    }

    return iRet;
}


/** @brief Write a data word to a UART
 *
 *  @param dev The device to use
 *  @param buffer The data to send
 *
 *  @returns The number of bytes written or -1 otherwise
 */
size_t uart_write(ft900_uart_regs_t *dev, uint8_t buffer)
{
        size_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
        uart_wait_whilst_transmitting(dev);
        dev->RHR_THR_DLL = buffer;
        iRet = 1;
    }

    return iRet;
}


/** @brief Write a series of data words to a UART
 *
 *  @param dev The device to use
 *  @param buffer A pointer to the array to send
 *  @param len The size of buffer
 *
 *  @returns The number of bytes written or -1 otherwise
 */
size_t uart_writen(ft900_uart_regs_t *dev, uint8_t *buffer, size_t len)
{
        size_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
        while (len)
        {
            uart_wait_whilst_transmitting(dev);
            dev->RHR_THR_DLL = *buffer;
            buffer++;
            iRet++;
            len--;
        }
    }

    return iRet;
}

/** @brief Check UART receive buffer for data
 *
 *  @param dev The device to use
 *
 *  @returns True if there is data available to read
 */
uint8_t uart_rx_has_data(ft900_uart_regs_t *dev)
{
	return (dev->LSR_ICR_XON2 & MASK_UART_LSR_DR) == MASK_UART_LSR_DR;
}

/** @brief Check UART transmit buffer for data
 *
 *  @param dev The device to use
 *
 *  @returns True if there is no data waiting to transmit
 */
uint8_t uart_tx_is_empty(ft900_uart_regs_t *dev)
{
	return (dev->LSR_ICR_XON2 & MASK_UART_LSR_THRE) == MASK_UART_LSR_THRE;
}

/** @brief Read a data word from a UART
 *
 *  @param dev The device to use
 *  @param buffer A pointer to the data word to store into
 *
 *  @returns The number of bytes read or -1 otherwise
 */
size_t uart_read(ft900_uart_regs_t *dev, uint8_t *buffer)
{
        size_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
        uart_wait_whilst_receiving(dev);
        *buffer = dev->RHR_THR_DLL;
        iRet = 1;
    }

    return iRet;
}


/** @brief Read a series of data words from a UART
 *
 *  @param dev The device to use
 *  @param buffer A pointer to the array of data words to store into
 *  @param len The number of data words to read
 *
 *  @returns The number of bytes read or -1 otherwise
 */
size_t uart_readn(ft900_uart_regs_t *dev, uint8_t *buffer, size_t len)
{
        size_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
        while (len)
        {
            uart_wait_whilst_receiving(dev);
            *buffer = dev->RHR_THR_DLL;
            buffer++;
            iRet++;
            len--;
        }
    }

    return iRet;
}


/** @brief Write a string to the serial port
 *  @param dev The device to use
 *  @param str The null-terminated string to write
 *  @return The number of bytes written or -1 otherwise
 */
size_t uart_puts(ft900_uart_regs_t *dev, char *str)
{
    return uart_writen(dev, (uint8_t *)str, strlen(str));
}


/** @brief Enable an interrupt on the UART
 *  @param dev The device to use
 *  @param interrupt The interrupt to enable
 *  @returns 0 on success, -1 otherwise
 */
int8_t uart_enable_interrupt(ft900_uart_regs_t *dev, uart_interrupt_t interrupt)
{
	int8_t iRet = 0;

	if (dev == NULL)
	{
		/* Unknown device */
		iRet = -1;
	}

	if (iRet == 0)
	{
		switch (interrupt)
		{
		case uart_interrupt_tx:
			dev->IER_DLH_ASR |= MASK_UART_IER_ETBEI;
			break;

		case uart_interrupt_rx:
			dev->IER_DLH_ASR |= MASK_UART_IER_ERBFI;
			break;

		case uart_interrupt_9th_bit:
			uart_write_index_register(dev, OFFSET_UART_SPR_NMR,
					uart_read_index_register(dev, OFFSET_UART_SPR_NMR)
							| MASK_UART_NMR_9_INT_EN);
			break;

		case uart_interrupt_dcd_ri_dsr_cts:
			dev->IER_DLH_ASR |= MASK_UART_IER_EDSSI;
			break;

			/*This interrupt is multiplexed with 9-bit special character detection
			 and special character detection */
		case uart_interrupt_xon_xoff:
			dev->IER_DLH_ASR |= MASK_UART_IER_ESCH;
			break;
		default:
			iRet = -1;
			break;
		}
	}

	return iRet;
}

/** @brief Disable an interrupt on the UART
 *  @param dev The device to use
 *  @param interrupt The interrupt to disable
 *  @returns 0 on success, -1 otherwise
 */
int8_t uart_disable_interrupt(ft900_uart_regs_t *dev, uart_interrupt_t interrupt)
{
	int8_t iRet = 0;

	if (dev == NULL)
	{
		/* Unknown device */
		iRet = -1;
	}

	if (iRet == 0)
	{
		switch (interrupt)
		{
		case uart_interrupt_tx:
			dev->IER_DLH_ASR &= ~MASK_UART_IER_ETBEI;
			break;

		case uart_interrupt_rx:
			dev->IER_DLH_ASR &= ~MASK_UART_IER_ERBFI;
			break;

		case uart_interrupt_9th_bit:
			uart_write_index_register(dev, OFFSET_UART_SPR_NMR,
					uart_read_index_register(dev, OFFSET_UART_SPR_NMR)
							& ~MASK_UART_NMR_9_INT_EN);
			break;

		case uart_interrupt_dcd_ri_dsr_cts:
			dev->IER_DLH_ASR &= ~MASK_UART_IER_EDSSI;
			break;

			/*This interrupt is multiplexed with 9-bit special character detection
			 and special character detection */
		case uart_interrupt_xon_xoff:
			dev->IER_DLH_ASR &= ~MASK_UART_IER_ESCH;
			break;
		default:
			iRet = -1;
			break;
		}
	}

	return iRet;
}


/** @brief Enable a UART to interrupt
 *  @param dev The device to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t uart_enable_interrupts_globally(ft900_uart_regs_t *dev)
{
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (dev == UART0)
        {
            SYS->MSC0CFG |= MASK_SYS_MSC0CFG_UART1_INTSEL;
        }
        else if (dev == UART1)
        {
            SYS->MSC0CFG |= MASK_SYS_MSC0CFG_UART2_INTSEL;
        }
#if defined(__FT930__)
        else if (dev == UART2)
        {
            SYS->MSC0CFG |= MASK_SYS_MSC0CFG_UART3_INTSEL;
        }
        else if (dev == UART3)
        {
            SYS->MSC0CFG |= MASK_SYS_MSC0CFG_UART4_INTSEL;
        }
#endif
        else
        {
            iRet = -1;
        }
    }

    return 0;
}


/** @brief Disable a UART from interrupting
 *  @param dev The device to use
 *  @returns 0 on success, -1 otherwise
 */
int8_t uart_disable_interrupts_globally(ft900_uart_regs_t *dev)
{
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
        if (dev == UART0)
        {
            SYS->MSC0CFG &= ~MASK_SYS_MSC0CFG_UART1_INTSEL;
        }
        else if (dev == UART1)
        {
            SYS->MSC0CFG &= ~MASK_SYS_MSC0CFG_UART2_INTSEL;
        }
#if defined(__FT930__)
        else if (dev == UART2)
        {
            SYS->MSC0CFG &= ~MASK_SYS_MSC0CFG_UART3_INTSEL;
        }
        else if (dev == UART3)
        {
            SYS->MSC0CFG &= ~MASK_SYS_MSC0CFG_UART4_INTSEL;
        }
#endif
        else
        {
            iRet = -1;
        }
    }

    return 0;
}

/** @brief Return the currently indicated interrupt.
 *  @param dev The device to use
 *  @returns enum of possible interrupt levels. Cast to
 *  	values defined in enum uart_interrupt_t.
 */
uint8_t uart_get_interrupt(ft900_uart_regs_t *dev)
{
    /* UART ISR is auto clearing */
    uint8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = 0xff;
    }

    if (iRet == 0)
    {
    	iRet = dev->ISR_FCR_EFR & 0x3F;
    }

    return iRet;
}

/** @brief Check if an interrupt has been triggered
 *  @param dev The device to use
 *  @param interrupt The interrupt to check
 *  @warning This function clears the current interrupt status bit
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 */
int8_t uart_is_interrupted(ft900_uart_regs_t *dev, uart_interrupt_t interrupt)
{
	/* UART ISR is auto clearing */
	int8_t iRet = 0;

	if (dev == NULL)
	{
		/* Unknown device */
		iRet = -1;
	}

	if (iRet == 0)
	{
		switch (interrupt)
		{
		case uart_interrupt_tx:
			if ((dev->ISR_FCR_EFR & 0x3F) == uart_interrupt_tx)
			{
				iRet = 1;
			}
			break;

		case uart_interrupt_rx:
			if ((dev->ISR_FCR_EFR & 0x3F) == uart_interrupt_rx)
			{
				iRet = 1;
			}
			break;

		case uart_interrupt_rx_time_out:
			if ((dev->ISR_FCR_EFR & 0x3F) == uart_interrupt_rx_time_out)
			{
				iRet = 1;
			}
			break;

		case uart_interrupt_dcd_ri_dsr_cts:
			if ((dev->ISR_FCR_EFR & 0x3F) == uart_interrupt_dcd_ri_dsr_cts)
			{
				iRet = 1;
			}
			break;

		case uart_interrupt_9th_bit:
			if (((dev->ISR_FCR_EFR & 0x3F) == uart_interrupt_9th_bit)
					&& (((dev->LSR_ICR_XON2	& (MASK_UART_LSR_PE_RXDATA | MASK_UART_LSR_RHRERR))
						== MASK_UART_LSR_PE_RXDATA)))
			{
				iRet = 1;
			}
			break;

			/*This interrupt is multiplexed with 9-bit special character detection
			 and special character detection.*/
		case uart_interrupt_xon_xoff:
			if ((dev->ISR_FCR_EFR & 0x3F) == uart_interrupt_xon_xoff)
			{
				iRet = 1;
			}
			break;

		default:
			iRet = -1;
			break;
		}
}

return iRet;
}

int8_t uart_rts(ft900_uart_regs_t *dev, int active)
{
	uint8_t mcr;
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
		mcr = dev->MCR_XON1_TFL;
		if (active) mcr |= MASK_UART_MCR_RTS;
		else mcr &= (~MASK_UART_MCR_RTS);
		dev->MCR_XON1_TFL = mcr;
    }

    return iRet;
}

int8_t uart_dtr(ft900_uart_regs_t *dev, int active)
{
	uint8_t mcr;
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
		mcr = dev->MCR_XON1_TFL;
		if (active) mcr |= MASK_UART_MCR_DTR;
		else mcr &= (~MASK_UART_MCR_DTR);
		dev->MCR_XON1_TFL = mcr;
    }

	return iRet;
}

int8_t uart_cts(ft900_uart_regs_t *dev)
{
	uint8_t mcr;
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
		mcr = dev->MSR_XOFF1;
		if (mcr & MASK_UART_MSR_CTS) return 1;
		return 0;
    }

    return iRet;
}

int8_t uart_dsr(ft900_uart_regs_t *dev)
{
	uint8_t mcr;
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
		mcr = dev->MSR_XOFF1;
		if (mcr & MASK_UART_MSR_DSR) return 1;
		return 0;
    }

	return iRet;
}

int8_t uart_ri(ft900_uart_regs_t *dev)
{
	uint8_t mcr;
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
		mcr = dev->MSR_XOFF1;
		if (mcr & MASK_UART_MSR_RI) return 1;
		return 0;
    }

	return iRet;
}

int8_t uart_dcd(ft900_uart_regs_t *dev)
{
	uint8_t mcr;
    int8_t iRet = 0;

    if (dev == NULL)
    {
        /* Unknown device */
        iRet = -1;
    }

    if (iRet == 0)
    {
		mcr = dev->MSR_XOFF1;
		if (mcr & MASK_UART_MSR_DCD) return 1;
		return 0;
    }

	return iRet;
}

/** @brief Set the mode of the UART
 *  @details After the mode is selected all flow control and UART
 *  settings are reset. The uart_open function must be called
 *  again to re-initialise the UART.
 *  @param dev The device to use
 *  @param mode The mode to select.
 *  @returns 0 if successful, -1 otherwise (invalid device).
 */
int8_t uart_mode(ft900_uart_regs_t *dev, uart_mode_t mode)
{
	int8_t iRet = 0;
	uint8_t tempLCR, temp = 0;
	if (dev == NULL)
	{
		/* Unknown device */
		iRet = -1;
	}

	if (iRet == 0)
	{
		switch (mode)
		{
		case uart_mode_16450:
			temp = uart_read_index_register(dev, OFFSET_UART_SPR_RFC);
			temp &= ~MASK_UART_FCR_EN; /* no FIFOs in 16450 mode. */
			dev->ISR_FCR_EFR = temp;
			break;

		case uart_mode_16550:
			if (uart_lcr_lastWrite(dev) != MODE_650)
			{
				temp = MODE_650;
				uart_enable_650mode(dev);
			}
			dev->ISR_FCR_EFR &= ~MASK_UART_EFR_EM; /*reset Enhanced mode if set*/
			if (temp == MODE_650)
				uart_disable_650mode(dev);
			temp = uart_read_index_register(dev, OFFSET_UART_SPR_RFC);
			temp &= ~MASK_UART_FCR_FIFO_SIZE; /*reset 128 byte FIFO size if set*/
			temp |= MASK_UART_FCR_EN; /*Enable FIFO*/
			dev->ISR_FCR_EFR = temp; /* 16 byte FIFOs 16550 mode. */
			break;

		case uart_mode_16650:
		case uart_mode_16950:
			if (uart_lcr_lastWrite(dev) != MODE_650)
			{
				temp = MODE_650;
				uart_enable_650mode(dev);
			}
			dev->ISR_FCR_EFR |= MASK_UART_EFR_EM;/* Set EFR[4] */
			if (temp == MODE_650)
				uart_disable_650mode(dev);
			dev->ISR_FCR_EFR |= (MASK_UART_FCR_EN); // | MASK_UART_FCR_RCVR_RST | MASK_UART_FCR_XMIT_RST); /* FIFO enable, Tx FIFO 				reset, Rx FIFO reset */
			break;

		case uart_mode_16750:
			if (uart_lcr_lastWrite(dev) != MODE_650)
			{
				temp = MODE_650;
				uart_enable_650mode(dev);
			}
			dev->ISR_FCR_EFR &= ~MASK_UART_EFR_EM; /*reset Enhanced mode if set*/
			if (temp == MODE_650)
				uart_disable_650mode(dev);
			tempLCR = uart_read_lcr(dev);
			// FCR[5] can be modified only with LCR[7] (DLAB) set
			if ((tempLCR & MASK_UART_LCR_DLAB) == 0)
			{
				uart_write_lcr(dev, tempLCR | MASK_UART_LCR_DLAB);
			}
			dev->ISR_FCR_EFR = uart_read_index_register(dev, OFFSET_UART_SPR_RFC)
					| (MASK_UART_FCR_FIFO_SIZE | MASK_UART_FCR_EN);
			uart_write_lcr(dev, tempLCR);
			break;
		}
	}
	return iRet;
}


/** @brief UART Software reset
 * 	@details Issues UART Soft reset, which resets
 * 	all uart registers to default value except
 * 	clock select register(CKS) and
 * 	clock alteration(CKA) registers.
 *  @param dev The device to use
 *
 *   @returns void.
 */
void uart_soft_reset(ft900_uart_regs_t *dev)
{
	uart_write_index_register(dev, OFFSET_UART_SPR_CSR, 0);

}

/** @brief Set TriggerLevel for 950 mode
 *  @param dev The device to use
 *  @param mode UART mode
 *  @param  rxIntLevel receiver interrupt level
 *  @param  txIntLevel transmitter interrupt level
 *  @param  FCH Flow Control High
 *  @param  FCL Flow Control Low
 *
 *  @returns 0 on success, -1 otherwise (invalid device or invalid parameter).
 */
int8_t uart_set_trigger_level(ft900_uart_regs_t *dev, uart_mode_t mode,
	uart_fifo_trigger_level_t rxIntLevel, uart_fifo_trigger_level_t txIntLevel,
	uint8_t FCH, uint8_t FCL)
{
	int8_t iRet = 0;
	uint8_t val = 0, level = 0, rxlevel = 0, txlevel = 0;
	if (dev == NULL)
	{
		/* Unknown device */
		iRet = -1;
	}

	if (iRet == 0)
	{
		switch (mode)
		{
		case uart_mode_16450:
			/*nothing to do*/
			break;

		case uart_mode_16550:
			switch (rxIntLevel)
			{
			case uart_fifo_trigger_level_1:
			case uart_fifo_trigger_level_0:
				/*Nothing to do in this mode*/
				break;

			case uart_fifo_trigger_level_4:
				level |= MASK_UART_FCR_RCRV_TRG0;
				break;

			case uart_fifo_trigger_level_8:
				level |= MASK_UART_FCR_RCRV_TRG1;
				break;
			case uart_fifo_trigger_level_14:
				level |= MASK_UART_FCR_RCRV_TRG;
				break;

			default:
				iRet = -1;
				break;
			}

			if (iRet == 0)
			{
				val = uart_read_index_register(dev, OFFSET_UART_SPR_RFC);
				val &= ~MASK_UART_FCR_RCRV_TRG;
				val |= level;
				dev->ISR_FCR_EFR = val;
			}
			break;

		case uart_mode_16650:
			switch (rxIntLevel)
			{
			case uart_fifo_trigger_level_1_16:
				/*Nothing to do in this mode*/
				break;

			case uart_fifo_trigger_level_16_32:
				rxlevel |= MASK_UART_FCR_RCRV_TRG0;
				break;

			case uart_fifo_trigger_level_32_112:
				rxlevel |= MASK_UART_FCR_RCRV_TRG1;
				break;
			case uart_fifo_trigger_level_112_120:
				rxlevel |= MASK_UART_FCR_RCRV_TRG;
				break;

			default:
				iRet = -1;
				break;
			}

			switch (txIntLevel)
			{
			case uart_fifo_trigger_level_16:
				/*Nothing to do in this mode*/
				break;

			case uart_fifo_trigger_level_32:
				txlevel |= MASK_UART_FCR_THR_TRG0;
				break;

			case uart_fifo_trigger_level_64:
				txlevel |= MASK_UART_FCR_THR_TRG1;
				break;
			case uart_fifo_trigger_level_112:
				txlevel |= MASK_UART_FCR_THR_TRG;
				break;

			default:
				iRet = -1;
				break;
			}

			if (iRet == 0)
			{
				val = uart_read_index_register(dev, OFFSET_UART_SPR_RFC);
				val &= ~(MASK_UART_FCR_RCRV_TRG | MASK_UART_FCR_THR_TRG);
				val |= (rxlevel | txlevel);
				dev->ISR_FCR_EFR = val;
			}
			break;

		case uart_mode_16750:
			switch (rxIntLevel)
			{
			case uart_fifo_trigger_level_1:
			case uart_fifo_trigger_level_0:
				/*Nothing to do in this mode*/
				break;

			case uart_fifo_trigger_level_32:
				level |= MASK_UART_FCR_RCRV_TRG0;
				break;

			case uart_fifo_trigger_level_64:
				level |= MASK_UART_FCR_RCRV_TRG1;
				break;
			case uart_fifo_trigger_level_112:
				level |= MASK_UART_FCR_RCRV_TRG;
				break;

			default:
				iRet = -1;
				break;
			}

			if (iRet == 0)
			{
				val = uart_read_index_register(dev, OFFSET_UART_SPR_RFC);
				val &= ~MASK_UART_FCR_RCRV_TRG;
				val |= level;
				dev->ISR_FCR_EFR = val;
			}
			break;

		case uart_mode_16950:
			if ((dev == NULL) || (rxIntLevel > 127) || (txIntLevel > 127)
					|| (FCH > 127) || (FCL > 127))
			{
				iRet = -1;
			}

			if (iRet == 0)
		{
			uart_write_index_register(dev, OFFSET_UART_SPR_RTL, rxIntLevel);
			uart_write_index_register(dev, OFFSET_UART_SPR_TTL, txIntLevel);
			uart_write_index_register(dev, OFFSET_UART_SPR_FCH, FCH);
			uart_write_index_register(dev, OFFSET_UART_SPR_FCL, FCL);
			uart_write_index_register(dev, OFFSET_UART_SPR_ACR,
					uart_read_index_register(dev, OFFSET_UART_SPR_ACR)
							| MASK_UART_ACR_950TLE); //ACR_950TLE
		}
			break;

		default:
			iRet = -1;
			break;
		}
	}
	return iRet;
}

/** @brief Get receiver FIFO level
 *  @param dev The device to use
 *
 *  @returns number of bytes in receiver FIFO.
 */
uint8_t uart_get_rx_fifo_level(ft900_uart_regs_t *dev)
{
	uint8_t val, acr;

	acr = uart_read_index_register(dev, OFFSET_UART_SPR_ACR);
	uart_write_index_register(dev, OFFSET_UART_SPR_ACR, (acr | MASK_UART_ACR_ASE)); /*ACR[7] = 1 for accessing ASE registers*/
	val = dev->LCR_RFL;
	uart_write_index_register(dev, OFFSET_UART_SPR_ACR, acr); /*revert access*/
	return val;
}

/** @brief Get transmitter FIFO level
 *  @param dev The device to use
 *
 *  @returns number of bytes in transmitter FIFO.
 */
uint8_t uart_get_tx_fifo_level(ft900_uart_regs_t *dev)
{
	uint8_t val, acr;

	acr = uart_read_index_register(dev, OFFSET_UART_SPR_ACR);
	uart_write_index_register(dev, OFFSET_UART_SPR_ACR, (acr | MASK_UART_ACR_ASE)); /*ACR[7] = 1 for accessing ASE registers*/
	val = dev->MCR_XON1_TFL;
	uart_write_index_register(dev, OFFSET_UART_SPR_ACR, acr); /*revert access*/
	return val;
}

/** @brief Set flow control
 *  @param dev The device to use
 *  @param mode flow control mode to enable.
 *  @param xOn xOn character(only used in XON-XOFF flow control, ignored in other cases).
 *  @param xOff xOff character(only used in XON-XOFF flow control, ignored in other cases).
 *  @returns 0 if successful, -1 otherwise (invalid device).
 */
int8_t uart_set_flow_control(ft900_uart_regs_t *dev, uart_flow_t mode,
	uint8_t xOn, uint8_t xOff)
{
	int8_t iRet = 0;
	uint8_t cks, efr;

	switch (mode)
	{
	case uart_flow_none:
		/*Disable flow control*/
		dev->MCR_XON1_TFL &= ~MASK_UART_MCR_XON;
		/*enter 650 mode to access EFR register*/
		uart_enable_650mode(dev);
		/* if enhanced mode is enabled, disable auto in-band flow control*/
		if ((dev->ISR_FCR_EFR & MASK_UART_EFR_EM) == MASK_UART_EFR_EM)
		{
			dev->ISR_FCR_EFR &= ~(MASK_UART_EFR_IBRFCM | MASK_UART_EFR_IBTFCM);
		}
		/*revert to normal mode*/
		uart_disable_650mode(dev);
		break;

	case uart_flow_rts_cts:
		/*If enhanced mode, set MCR[1], EFR[6] and EFR[7]*/
		/*enter 650 mode to access EFR register*/
		uart_enable_650mode(dev);
		efr = dev->ISR_FCR_EFR;
		uart_disable_650mode(dev);

		/* if enhanced mode is enabled, disable auto in-band flow control*/
		if ((efr & MASK_UART_EFR_EM) == MASK_UART_EFR_EM)
		{
			dev->MCR_XON1_TFL |= MASK_UART_MCR_RTS;
			uart_enable_650mode(dev);
			dev->ISR_FCR_EFR |= (MASK_UART_EFR_CTSFC | MASK_UART_EFR_RTSFC);
			uart_disable_650mode(dev);
		}
		else
		{
			dev->MCR_XON1_TFL |= (MASK_UART_MCR_RTS | MASK_UART_MCR_XON);
		}
		break;

	case uart_flow_dtr_dsr:
		/*set ASR[2] for DSR*/
		uart_write_index_register(dev, OFFSET_UART_SPR_ACR,
				uart_read_index_register(dev,
				OFFSET_UART_SPR_ACR) | MASK_UART_ACR_DSR);
		cks = uart_read_index_register(dev, OFFSET_UART_SPR_CKS);
		/*CKS[4] and CKS[5] = 0 to enable DTR flow control*/
		if ((cks & MASK_UART_CKS_TCRG0) || (cks & MASK_UART_CKS_TCRG1))
		{
			uart_write_index_register(dev, OFFSET_UART_SPR_CKS,
					cks & ~MASK_UART_CKS_TCRG);
		}
		/*Enable DTR is inactive*/
		dev->MCR_XON1_TFL |= MASK_UART_MCR_DTR;
		/*set ACR[4] = 0 and ACR[3] = 1 [page 68]*/
		uart_write_index_register(dev, OFFSET_UART_SPR_ACR,
				(uart_read_index_register(dev, OFFSET_UART_SPR_ACR)
						& ~MASK_UART_ACR_DTR1) | MASK_UART_ACR_DTR0);
		break;

	case uart_flow_xon_xoff:
		/*enter 650 mode to access EFR register*/
		uart_enable_650mode(dev);
		efr = dev->ISR_FCR_EFR;
		uart_disable_650mode(dev);
		/* if enhanced mode is enabled, disable auto in-band flow control*/
		if ((efr & MASK_UART_EFR_EM) == MASK_UART_EFR_EM)
		{
			/*clear ASR[1]*/
			uart_write_asr(dev, uart_read_asr(dev) & ~MASK_UART_ASR_REM_TX_DIS);
			uart_enable_650mode(dev);
			/*register XON1*/
			dev->MCR_XON1_TFL = xOn;
			/*register XOFF1*/
			dev->MSR_XOFF1 = xOff;
			/*Enable flow control for xOn and xOff*/
			efr = dev->ISR_FCR_EFR;
			efr = ((efr & ~MASK_UART_EFR_IBTFCM0) | MASK_UART_EFR_IBTFCM1); /*In-band transmit: recognize XON1 as XON and XFF1 as XOFF*/
			efr = ((efr & ~MASK_UART_EFR_IBRFCM0) | MASK_UART_EFR_IBRFCM1); /*In-band receive: recognize XON1 as XON and XFF1 as XOFF*/
			dev->ISR_FCR_EFR = efr;
			uart_disable_650mode(dev);
		}
		else
		{
			iRet = -1;
		}
		break;

	default:
		iRet = -1;
		break;
	}

	return iRet;
}

/** @brief Configure 9-bit mode address registers of UART
 *  @details Configure address for 9-bit mode
 *  address detection
 *  @param dev The device to use
 *  @param address_1 one among 4 address for address detection
 *  @param address_2 one among 4 address for address detection
 *  @param address_3 one among 4 address for address detection
 *  @param address_4 one among 4 address for address detection
 *  @warning if 9th bit of the address is not set, then that
 *  Particular address will not be configured or disabled for
 *  address detection.
 *  address_4 is not applicable for FT900 Rev B.
 *  @returns 0 if successful, -1 otherwise (invalid device).
 */
int8_t uart_configure_9bit_address(ft900_uart_regs_t *dev, uint16_t address_1,
	uint16_t address_2, uint16_t address_3, uint16_t address_4)
{
	uint8_t tempLCR = 0, tempNMR = 0;
	int8_t iRet = 0;

	if (dev == NULL)
	{
		/* Unknown device */
		iRet = -1;
	}

	if (iRet == 0)
	{
		tempNMR = uart_read_index_register(dev, OFFSET_UART_SPR_NMR);

		if (uart_lcr_lastWrite(dev) != MODE_650)
		{
			tempLCR = MODE_650;
			uart_enable_650mode(dev);
		}

		if ((dev->ISR_FCR_EFR & MASK_UART_EFR_EM) == MASK_UART_EFR_EM) //9bit mode address detection is available only in enhanced mode.
		{

			if (address_1)
			{
				dev->MCR_XON1_TFL = address_1 & 0xFF;

				if (address_1 & 0x100)
					tempNMR |= MASK_UART_NMR_9_SC1;
				else
					tempNMR &= ~MASK_UART_NMR_9_SC1;
			}

			if (address_2)
			{
				dev->LSR_ICR_XON2 = address_2 & 0xFF;

				if (address_2 & 0x100)
					tempNMR |= MASK_UART_NMR_9_SC2;
				else
					tempNMR &= ~MASK_UART_NMR_9_SC2;
			}

			if (address_3)
			{
				dev->MSR_XOFF1 = address_3 & 0xFF;

				if (address_3 & 0x100)
					tempNMR |= MASK_UART_NMR_9_SC3;
				else
					tempNMR &= ~MASK_UART_NMR_9_SC3;
			}

			if (address_4)
			{
				dev->SPR_XOFF2 = address_4 & 0xFF;

				if (address_4 & 0x100)
					tempNMR |= MASK_UART_NMR_9_SC4;
				else
					tempNMR &= ~MASK_UART_NMR_9_SC4;
			}

			uart_write_index_register(dev, OFFSET_UART_SPR_NMR, tempNMR);
		}
		else
		{
			iRet = -1;
		}

		if (tempLCR == MODE_650)
			uart_disable_650mode(dev);
	}
	return iRet;
}

/** @brief Flush receiver FIFO
 *  @details Clears all bytes in the receiver FIFO and
 *  resets its counter logic to 0
 *  @param dev The device to use
 *  @warning The shitf register is not cleared
 *  @returns 0 if successful, -1 otherwise (invalid device).
 */
int8_t uart_flush_rx_fifo(ft900_uart_regs_t *dev)
{
	int8_t iRet = 0;

	if (dev == NULL)
	{
		/* Unknown device */
		iRet = -1;
	}
	else
	{
		dev->ISR_FCR_EFR = uart_read_index_register(dev, OFFSET_UART_SPR_RFC)
				| MASK_UART_FCR_RCVR_RST;
	}

	return iRet;
}

/** @brief Flush transmitter FIFO
 *  @details Clears all bytes in the transmitter FIFO and
 *  resets its counter logic to 0
 *  @param dev The device to use
 *  @warning The shitf register is not cleared
 *  @returns 0 if successful, -1 otherwise (invalid device).
 */
int8_t uart_flush_tx_fifo(ft900_uart_regs_t *dev)
{
	int8_t iRet = 0;

	if (dev == NULL)
	{
		/* Unknown device */
		iRet = -1;
	}
	else
	{
		dev->ISR_FCR_EFR = uart_read_index_register(dev, OFFSET_UART_SPR_RFC)
				| MASK_UART_FCR_XMIT_RST;
	}

	return iRet;
}

/** @brief Send 9-bit address
 *  @details Send 9-bit address characters from uART.
 *  before sending 9-bit address, UART has to enabled
 *  for 9-bit mode.
 *  @param dev The device to use
 *  @param address 9 bit address to send out
 *  @returns 0 if successful, -1 otherwise (invalid device).
 */
int8_t uart_send_9bit_address(ft900_uart_regs_t *dev, uint16_t address)
{
	int8_t iRet = 0;

	if (dev == NULL)
	{
		iRet = -1;
	}

	if (iRet == 0)
	{
		if ((address & 0x100) == 0x100)
			dev->SPR_XOFF2 |= MASK_UART_SPR_TX_9BIT;
		else
			dev->SPR_XOFF2 &= ~MASK_UART_SPR_TX_9BIT;
		dev->RHR_THR_DLL = (address & 0xFF);
		uart_wait_whilst_transmitting(dev);
		dev->SPR_XOFF2 &= ~MASK_UART_SPR_TX_9BIT;
	}

	return iRet;
}
