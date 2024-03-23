/**
    @file

    @brief
    The internal SPI implementation

    
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
#include <ft900.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/
static inline void spi_wait_whilst_transmitting(ft900_spi_regs_t* dev)
{
    while (!(dev->SPI_STATUS & MASK_SPIM_SPIM_STATUS_TX_EMPTY))
    {
    }
}

static inline uint8_t spi_fifos_enabled(ft900_spi_regs_t* dev)
{
    return (dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_FIFO_EN) > 0;
}

/* FUNCTIONS ***********************************************************************/
/** @brief Initialize the SPI device
 *
 *  @param dev the device to use
 *  @param dir The direction for the device to work in (Master/Slave)
 *  @param clock_mode The SPI Clock mode to use
 *  @param div The clock divider to use (2 (Fast mode),4,8,16,32,64,128,256,512)
 *
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_init(ft900_spi_regs_t* dev, spi_dir_t dir, spi_clock_mode_t clock_mode, uint16_t div)
{
    int8_t iRet = 0;

    if (dir < spi_dir_slave || dir > spi_dir_master)
    {
        iRet = -1; /* Out of range */
        return -1;
    }

    if (clock_mode < spi_mode_0 || clock_mode > spi_mode_3)
    {
        iRet = -1; /* Out of range */
        return -1;
    }

    /*if (div == 2) {iRet = 0;}
    else*/ if (div == 4) {iRet = 0;}
    else if (div == 8) {iRet = 0;}
    else if (div == 16) {iRet = 0;}
    else if (div == 32) {iRet = 0;}
    else if (div == 64) {iRet = 0;}
    else if (div == 128) {iRet = 0;}
    else if (div == 256) {iRet = 0;}
    else if (div == 512) {iRet = 0;}
    else {iRet = -1;}


    /* Turn on the SPI Device ... */
    if (iRet == 0)
    {
        /* Reset all registers */
        dev->SPI_CNTL = 0x00;
        dev->SPI_STATUS = 0x00;
        dev->SPI_FIFO_CNTL = 0x00;
        dev->SPI_TNSFR_FRMT_CNTL = 0x00;


        if (dir == spi_dir_master)
        {
            /* Master */
            dev->SPI_CNTL |= MASK_SPIM_SPIM_CNTL_MSTR;
        }
        else
        {
            /* Slave */
            /*SPI->SPCR &= ~SPIM_SPCR_MSTR;*/
        }

        /* Set up the Clock modes on the SPI device... */
        /*SPI->SPCR &= ~(SPIM_SPCR_CPOL | SPIM_SPCR_CPHA);*/ /* Mask */
        if (clock_mode == spi_mode_0)
        {
            /* Don't need to do anything */
        }
        else if (clock_mode == spi_mode_1)
        {
            dev->SPI_CNTL |= MASK_SPIM_SPIM_CNTL_CLK_PHA;
        }
        else if (clock_mode == spi_mode_2)
        {
            dev->SPI_CNTL |= MASK_SPIM_SPIM_CNTL_CLK_POL;
        }
        else if (clock_mode == spi_mode_3)
        {
            SPIM->SPI_CNTL |= (MASK_SPIM_SPIM_CNTL_CLK_PHA | MASK_SPIM_SPIM_CNTL_CLK_POL);
        }
        else
        {
            /* Do Nothing, shouldn't get here */
        }


        /* Set up the clock divider... */
        /*SPIM->SPCR &= ~(SPIM_SPCR_SPR2 | SPIM_SPCR_SPR1 | SPIM_SPCR_SPR0);*/ /* Mask */
        if (div == 4) { /* SPIM->SPCR |= 0x00; */ /* SPR = 0b000 */ }
        else if (div == 8) { dev->SPI_CNTL |= (MASK_SPIM_SPIM_CNTL_SP_R0); /* SPR = 0b001 */ }
        else if (div == 16) { dev->SPI_CNTL |= (MASK_SPIM_SPIM_CNTL_SP_R1); /* SPR = 0b010 */ }
        else if (div == 32) { dev->SPI_CNTL |= (MASK_SPIM_SPIM_CNTL_SP_R1 | MASK_SPIM_SPIM_CNTL_SP_R0); /* SPR = 0b011 */ }
        else if (div == 64) { dev->SPI_CNTL |= (MASK_SPIM_SPIM_CNTL_SP_R2); /* SPR = 0b100 */ }
        else if (div == 128) { dev->SPI_CNTL |= (MASK_SPIM_SPIM_CNTL_SP_R2 | MASK_SPIM_SPIM_CNTL_SP_R0); /* SPR = 0b101 */ }
        else if (div == 256) { dev->SPI_CNTL |= (MASK_SPIM_SPIM_CNTL_SP_R2 | MASK_SPIM_SPIM_CNTL_SP_R1); /* SPR = 0b110 */ }
        else if (div == 512) { dev->SPI_CNTL |= (MASK_SPIM_SPIM_CNTL_SP_R2 | MASK_SPIM_SPIM_CNTL_SP_R1 | MASK_SPIM_SPIM_CNTL_SP_R0); /* SPR = 0b111 */ }
        else {}

        dev->SPI_SLV_SEL_CNTL = 0xFF; /* Deassert all devices */
        
        dev->SPI_CNTL |= MASK_SPIM_SPIM_CNTL_SP_E; /* Enable the SPI device */

    }

    return iRet;
}

/** @brief Enable the SPI device
 *  @param dev the device to use
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_enable(ft900_spi_regs_t* dev)
{

	dev->SPI_CNTL |= MASK_SPIM_SPIM_CNTL_SP_E; /* Enable the SPI device */

	return 0;
}

/** @brief Disable the SPI device
 *  @param dev the device to use
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_uninit(ft900_spi_regs_t* dev)
{
    dev->SPI_CNTL &= ~MASK_SPIM_SPIM_CNTL_SP_E; /* Disable the SPI device */

    return 0;
}


/** @brief Select a device to start communicating with
 *  @param dev the device to use
 *  @param num The device to select
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_open(ft900_spi_regs_t* dev, uint8_t num)
{
    int8_t iRet = 0;

    /* TBD - replace the number 7 with macro */
    if (/*num < 0 ||*/ num > 7)
    {
        iRet = -1; /* Out of range */
    }

    if (iRet == 0)
    {
        if (((dev->SPI_CNTL & MASK_SPIM_SPIM_CNTL_MSTR)) && (dev->SPI_SLV_SEL_CNTL == 0xFF))
        {
            /* No devices currently asserted */
            if (spi_fifos_enabled(dev))
            {
                /* Reset the FIFOs - clear out any latent data from the last
                   transfer */
                dev->SPI_FIFO_CNTL |= MASK_SPIM_SPIM_FIFO_CNTL_RCVR_RST | MASK_SPIM_SPIM_FIFO_CNTL_TX_RST;
                while (dev->SPI_FIFO_CNTL & (MASK_SPIM_SPIM_FIFO_CNTL_RCVR_RST | MASK_SPIM_SPIM_FIFO_CNTL_TX_RST))
                {
                    /* Wait for the FIFOs to reset */
                }
            }
            dev->SPI_SLV_SEL_CNTL &= ~(1 << num); /* Assert */
        }
        else
        {
            /* A device is currently asserted */
            iRet = -1;
        }
    }

    return iRet;
}


/** @brief Stop communicating with a certain device.
 *  @param dev the device to use
 *  @param num The device to select
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_close(ft900_spi_regs_t* dev, uint8_t num)
{
    int8_t iRet = 0;

    /* TBD - replace the number 7 with macro */
    if (/*num < 0 ||*/ num > 7)
    {
        iRet = -1; /* Out of range */
    }

    if (iRet == 0)
    {
        /* Need to cast to a byte because of 32 bit memory accesses */
        if ( (uint8_t)(~(dev->SPI_SLV_SEL_CNTL | (1 << num))) )
        {
            /* Another device is currently opened */
            iRet = -1;
        }
        else if ( (uint8_t)((~dev->SPI_SLV_SEL_CNTL) & (1 << num)) == 0 )
        {
            /* The device is not open */
            iRet = -1;
        }
        else
        {
            /* Flush out all data bytes in the FIFO before deasserting */
            /*if (spi_fifos_enabled(dev))
            {*/
                spi_wait_whilst_transmitting(dev);
            /*}*/

            dev->SPI_SLV_SEL_CNTL |= (1 << num); /* Deassert */
        }
    }

    return iRet;
}

/** @brief Writes several bytes to the SPI device
 *
 *  @param dev the device to use
 *  @param b A pointer to the array to send
 *  @param len The number of bytes to write
 *
 *  @returns The number of bytes written or -1 for a failure
 */
int32_t spi_writen(ft900_spi_regs_t* dev, const uint8_t *b, size_t len)
{
    int32_t iRet = 0;
    size_t iFifoSize;
    const uint8_t *p8 = b;
    uint8_t FifoScratch[64];

    if (dev->SPI_TNSFR_FRMT_CNTL & (MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI | MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI))
    {
        /* Set to write */
        dev->SPI_TNSFR_FRMT_CNTL &= ~MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DIR;
    }

    /* Check if FIFO mode is on */
    if (spi_fifos_enabled(dev))
    {
    	int32_t chunksz;
        /* FIFOs enabled */
        //asm_streamout8(p8, &(dev->SPI_DATA), len);
		/* FIFOs are enabled... */
		if (dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_64_BYTE)
		{
			iFifoSize = 64;
		}
		else
		{
			iFifoSize = 16;
		}

        while(len > 0)
        {
        	chunksz = (len > iFifoSize)? iFifoSize : len;

        	asm_streamout8(b,&(dev->SPI_DATA), chunksz);

        	spi_wait_whilst_transmitting(dev);

			//TODO - scope for optimization by using RCVR_RST approach to clear the receive fifo instead of streamin

			/* Do not perform read of data in case of quad or dual mode */
			if( !(dev->SPI_TNSFR_FRMT_CNTL & (MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI | MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI)))
			{
				while(!(dev->SPI_STATUS & MASK_SPIM_SPIM_STATUS_SPI_FLAG))
				{

				}
				asm_streamin8(&(dev->SPI_DATA), FifoScratch, chunksz);
			}

            len -= chunksz;
            b += chunksz;
            iRet += chunksz;
        }
    }
    else
    {
        /* FIFOs disabled */
        while(len > 0)
        {
        	uint8_t __attribute__ ((unused)) DummyByte;
            dev->SPI_DATA = *p8++;

            spi_wait_whilst_transmitting(dev);
            DummyByte = dev->SPI_DATA;

            len--;
            iRet++;
        }
    }

    return iRet;
}

/** @brief Reads several bytes from the SPI device
 *
 *  Read the most current bytes in the buffer, for a Non-FIFO mode one byte will be read
 *  For multi-bit wide busses (Dual and Quad), the SPI device will cause multiple reads on the bus
 *
 *  @param dev the device to use
 *  @param b A pointer to the array to read into
 *  @param len The number of bytes to read
 *
 *  @returns The number of bytes read or -1 for a failure
 */
int32_t spi_readn(ft900_spi_regs_t* dev, uint8_t *b, size_t len)
{
    int32_t iRet = 0;
    size_t i = 0, iFifoSize = 0;
    uint8_t FifoScratch[64];

    asm_memset32(0, FifoScratch, sizeof(FifoScratch));

    /* Sync up so we don't step on our toes */
    spi_wait_whilst_transmitting(dev);


    if (dev->SPI_TNSFR_FRMT_CNTL & (MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI | MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI))
    {

        /* Dual or Quad Mode... */
        /* FIFOs enabled - stream in dummy bytes */

        /* Set to read */
        dev->SPI_TNSFR_FRMT_CNTL |= MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DIR;
    }

    if (dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_FIFO_EN)
    {
    	int32_t chunksz = 0;

        /* FIFOs are enabled... */
        if (dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_64_BYTE)
        {
            iFifoSize = 64;
        }
        else
        {
            iFifoSize = 16;
        }

		while(len > 0)
		{
			chunksz = (len > iFifoSize) ? iFifoSize : len;

			asm_streamout8(FifoScratch,&(dev->SPI_DATA), chunksz);

			/* Check for tx empty flag */
			spi_wait_whilst_transmitting(dev);

			/* Check for the transmission emplty flag */
			while(!(dev->SPI_STATUS & MASK_SPIM_SPIM_STATUS_SPI_FLAG))
			{

			}


			asm_streamin8(&(dev->SPI_DATA), b, chunksz);

			len -= chunksz;
			b += chunksz;
			iRet += chunksz;
		}
    }
    else
    {
		/* Synchronously read data from the SPI device */
		for (i = 0; i < len; ++i)
		{
			dev->SPI_DATA = 0xDE; /* Poke the SPI device to read */
			spi_wait_whilst_transmitting(dev);
			while(!(dev->SPI_STATUS & MASK_SPIM_SPIM_STATUS_SPI_FLAG))
			{

			}

			*b++ = dev->SPI_DATA; /* Read the byte into the buffer */
			iRet++;
		}
    }

    return iRet;
}

/** @brief Transfers several bytes from the SPI device
 *
 *  Transfer len number of bytes by sending and receiving.
 *  Transfer is valid only for single channel use case, using this api in dual/quad setting will return error.
 *
 *  @param dev the device to use
 *  @param binp A pointer to input buffer/read buffer
 *  @param bout A pointer to output buffer/write buffer
 *  @param len The number of bytes to transfer
 *
 *  @returns The number of bytes transferred or -1 for a failure
 */
int32_t spi_xchangen(ft900_spi_regs_t* dev, uint8_t *binp, uint8_t *bout, size_t len)
{
    int32_t iRet = 0;
    size_t i = 0, iFifoSize = 0;

    if (dev->SPI_TNSFR_FRMT_CNTL & (MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI | MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI))
    {

        /* Error case for Dual or Quad Mode... */
        return -1;
    }

    /* Sync up so we don't step on our toes */
    spi_wait_whilst_transmitting(dev);

    /* Check for fifo enabled or disabled */
    if (dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_FIFO_EN)
    {
    	int32_t chunksz = 0;

        /* FIFOs are enabled... */
        if (dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_64_BYTE)
        {
            iFifoSize = 64;
        }
        else
        {
            iFifoSize = 16;
        }

    	/* Fifo use case - Write followed by read */
		while(len > 0)
		{
			chunksz = (len > iFifoSize) ? iFifoSize : len;

			asm_streamout8(bout,&(dev->SPI_DATA), chunksz);

			spi_wait_whilst_transmitting(dev);
			while(!(dev->SPI_STATUS & MASK_SPIM_SPIM_STATUS_SPI_FLAG))
			{

			}

			asm_streamin8(&(dev->SPI_DATA), binp, chunksz);

			len -= chunksz;
			binp += chunksz;
			bout += chunksz;
			iRet += chunksz;
		}
    }
    else
    {
		/* Only read out one byte (the one in the shift register) */
		for (i = 0; i < len; i++)
		{
			dev->SPI_DATA = *bout++; /* transfer dummy data to read */
			spi_wait_whilst_transmitting(dev);
			while(!(dev->SPI_STATUS & MASK_SPIM_SPIM_STATUS_SPI_FLAG))
			{

			}

			*binp++ = dev->SPI_DATA;
			iRet++;
		}
    }

    return iRet;
}

/** @brief Enables the SPI device to generate an interrupt
 *  @param dev the device to use
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_enable_interrupts_globally(ft900_spi_regs_t* dev)
{
    dev->SPI_CNTL |= MASK_SPIM_SPIM_CNTL_SP_IE;
    return 0;
}


/** @brief Disables the SPI device from generating an interrupt
 *  @param dev the device to use
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_disable_interrupts_globally(ft900_spi_regs_t* dev)
{
    dev->SPI_CNTL &= ~MASK_SPIM_SPIM_CNTL_SP_IE;
    return 0;
}


/** @brief Enables an interrupt for the SPI device
 *  @param dev the device to use
 *  @param interrupt The interrupt to enable
 *  @returns 0 on a success or -1 for a failure
 *  @see spim_enable_interrupts_globally
 */
int8_t spi_enable_interrupt(ft900_spi_regs_t* dev, spi_interrupt_t interrupt)
{
    int8_t iRet = 0;
#if defined(__FT900__)
	ft900_spi_regs_ex_t *device = (ft900_spi_regs_ex_t *)dev;
#else
	ft900_spi_regs_t *device = dev;
#endif

    switch(interrupt)
    {
        case spi_interrupt_transmit_empty:
            dev->SPI_TNSFR_FRMT_CNTL |= MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_TX_IEN;
            break;
#if 0
        case spi_interrupt_receiver_fifo_timeout:
            dev->SPIFIFO |= MASK_SPI_SPIFIFO_TIMEOUT;
            break;
#endif
        case spi_interrupt_transmit_1bit_complete:
            dev->SPI_TNSFR_FRMT_CNTL |= MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_BISINT_EN;
            break;
        /* Now all the stuff that is set automatically */
        case spi_interrupt_data_ready:
        case spi_interrupt_fault:
            break;
        case spi_interrupt_ex_tx_trigger_en:
        	device->SPI_FIFO_CNTL_2 |= MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_EN;
            break;
        case spi_interrupt_ex_rx_trigger_en:
        	device->SPI_FIFO_CNTL_2 |= MASK_SPIM_SPIM_FIFO_CNTL_2_RX_TRIG_EN;
            break;
        default:
            iRet = -1;
            break;
    }

    return iRet;
}


/** @brief Disables an interrupt for the SPI device
 *  @param dev The device to use
 *  @param interrupt The interrupt to disable
 *  @returns 0 on a success or -1 for a failure
 *  @see spim_disable_interrupts_globally
 */
int8_t spi_disable_interrupt(ft900_spi_regs_t* dev, spi_interrupt_t interrupt)
{
    int8_t iRet = 0;

    switch(interrupt)
    {
        case spi_interrupt_transmit_empty:
            dev->SPI_TNSFR_FRMT_CNTL &= ~MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_TX_IEN;
            break;
#if 0
        case spi_interrupt_receiver_fifo_timeout:
            dev->SPIFIFO &= ~MASK_SPI_SPIFIFO_TIMEOUT;
            break;
#endif
        case spi_interrupt_transmit_1bit_complete:
            dev->SPI_TNSFR_FRMT_CNTL &= ~MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_BISINT_EN;
            break;
        /* Now all the stuff that is set automatically */
        case spi_interrupt_data_ready:
        case spi_interrupt_fault:
            break;
        default:
            iRet = -1;
            break;
    }

    return iRet;
}

/** @brief Disables an interrupt for the QSPI device
 *  @param dev The device to use
 *  @param interrupt The interrupt to check
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 *  @see spi_disable_interrupts_globally
 */
int8_t spi_is_interrupted(ft900_spi_regs_t* dev, spi_interrupt_t interrupt)
{
    int8_t iRet = 0;
#if defined(__FT900__)
	ft900_spi_regs_ex_t *device = (ft900_spi_regs_ex_t *)dev;
#else
	ft900_spi_regs_t *device = dev;
#endif

    if (dev == NULL)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {

        switch(interrupt)
        {
            case spi_interrupt_transmit_empty:
#if defined(__FT900__)
            	if (sys_check_ft900_revB())//if 90x series is rev B
            	{
					if ((dev->SPI_STATUS & (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_TX_EMPTY))
							== (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_TX_EMPTY))
					{
						iRet = 1;
					}
            	}
            	else
            	{
            		//if 90x series is rev C
                    if ((device->SPI_STATUS_2 & (MASK_SPIM_SPIM_STATUS_2_TXFIFO_EMPTY))
                            == (MASK_SPIM_SPIM_STATUS_2_TXFIFO_EMPTY))
                    {
                        iRet = 1;
                    }
            	}
#elif defined(__FT930__)
                if ((device->SPI_STATUS_2 & (MASK_SPIM_SPIM_STATUS_2_TXFIFO_EMPTY))
                        == (MASK_SPIM_SPIM_STATUS_2_TXFIFO_EMPTY))
                {
                    iRet = 1;
                }
#endif
                break;
#if 0
            case spi_interrupt_receiver_fifo_timeout:    
                break;
#endif
            case spi_interrupt_transmit_1bit_complete:
                if ((dev->SPI_STATUS & (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_SPI_BIS))
                        == (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_SPI_BIS))
                {
                    iRet = 1;
                    dev->SPI_CNTL |= 0x00; /* Do nothing but access SPIM_CNTL */
                }
                break;


            case spi_interrupt_data_ready:
#if defined(__FT900__)
            	if (sys_check_ft900_revB())//if 90x series is rev B
            	{
                    if ((dev->SPI_STATUS & (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_SPI_BIS | MASK_SPIM_SPIM_STATUS_MOD_FAULT))
                            == (MASK_SPIM_SPIM_STATUS_SPI_FLAG))
                    {
                        iRet = 1;
                    }
            	}
            	else
            	{
            		//if 90x series is rev C
                    if ((dev->SPI_STATUS & (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_SPI_BIS | MASK_SPIM_SPIM_STATUS_MOD_FAULT))
                            == (MASK_SPIM_SPIM_STATUS_SPI_FLAG))
                    {
                        iRet = 1;
                    }
            	}
#elif defined(__FT930__)
                if ((dev->SPI_STATUS & (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_SPI_BIS | MASK_SPIM_SPIM_STATUS_MOD_FAULT))
                          == (MASK_SPIM_SPIM_STATUS_SPI_FLAG))
                {
                    iRet = 1;
                }
#endif
                break;

            case spi_interrupt_fault:
                if ((dev->SPI_STATUS & (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_MOD_FAULT))
                        == (MASK_SPIM_SPIM_STATUS_SPI_FLAG | MASK_SPIM_SPIM_STATUS_MOD_FAULT))
                {
                    iRet = 1;
                    dev->SPI_CNTL |= 0x00; /* Do nothing but access SPICTRL1 */
                }
                break;
            case spi_interrupt_ex_rx_fifo_full:
                if ((device->SPI_STATUS_2 & (MASK_SPIM_SPIM_STATUS_2_RXFIFO_FULL))
                        == (MASK_SPIM_SPIM_STATUS_2_RXFIFO_FULL))
                {
                    iRet = 1;
                }
                break;
            case spi_interrupt_ex_rx_fifo_overridden:
                if ((device->SPI_STATUS_2 & (MASK_SPIM_SPIM_STATUS_2_RXFULL_OVERRIDE))
                        == (MASK_SPIM_SPIM_STATUS_2_RXFULL_OVERRIDE))
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


/** @brief Return the status register
 *
 *  | Bit  | Name   | When set to 1                          | When set to 0                     |
 *  | :--: | :----- | :------------------------------------- | :-------------------------------- |
 *  | 0    | AUTOSS | Auto SS Assertions Enabled             | SSO always shows contents of SSCR |
 *  | 1    | RXFULL | Receiver FIFO is full                  | Receiver FIFO not is full         |
 *  | 2    | EMPTY  | TX FIFO or TX register empty           | TX FIFO or TX register not empty  |
 *  | 3    | IDLE   | SPI Device Idle                        | Transmission in progress          |
 *  | 4    | FAULT  | Mode Fault (SS Low in Master mode)     | --                                |
 *  | 5    | 1BITTX | End of TX from SPDR BIS Register       | --                                |
 *  | 6    | WCOL   | Data Register Write Collision occurred | --                                |
 *  | 7    | IRQ    | An interrupt occurred                  | --                                |
 *
 *  @param dev The device to use
 *  @returns A copy of the SPISTAT register
 */
uint8_t spi_status(ft900_spi_regs_t* dev)
{
    return dev->SPI_STATUS;
}


/** @brief Control the SPI device
 *
 *  This function will set various options for the driver.
 *
 *  | Option                           | Description                  | Values                                         |
 *  | :------------------------------- | :--------------------------- | :--------------------------------------------- |
 *  | spim_option_fifo                 | Enable or disable the FIFO   | 0 = Disabled (Default) \n 1 = Enabled [Note 1] |
 *  | spim_option_fifo_size            | Set the size of the FIFO     | 16 = 16 Byte FIFO (default) \n 64 = Byte FIFO  |
 *  | spim_option_fifo_receive_trigger | Set the FIFO trigger level   | For 16 Byte FIFOs: 1, 4, 8, 14 \n For 64 Byte FIFOs: 1, 16, 32, 56 |
 *  | spim_option_force_ss_assertions  | Force SS to go low in assert | 0 = Automatic Assertions \n 1 = Force Assertions (Default) |
 *  | spim_option_bus_width            | Set the SPI bus width        | 1 = Single (Default) \n 2 = Dual \n 4 = Quad   |
 *  | spim_option_dual_quad_direction  | Set the multi-bit direction  | 0 = Read \n 1 = Write                          |
 *
 *  Note 1: Enabling the FIFO will cause the driver to clear its contents.
 *
 *  @param dev The device to use
 *  @param opt The option to configure
 *  @param val The value to use
 *
 *  @returns 0 on a success or -1 for a failure
 */
int8_t spi_option(ft900_spi_regs_t* dev, spi_option_t opt, uint8_t val)
{
    int8_t iRet = 0;

    switch (opt)
    {
        case spi_option_fifo:
        {
            if (val == 1)
            {
                if (!(dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_FIFO_EN))
                {
                    dev->SPI_FIFO_CNTL |= MASK_SPIM_SPIM_FIFO_CNTL_FIFO_EN; /* Enable the FIFOs */

                    /* Reset both FIFOs, self clearing */
                    dev->SPI_FIFO_CNTL |= (MASK_SPIM_SPIM_FIFO_CNTL_RCVR_RST | MASK_SPIM_SPIM_FIFO_CNTL_TX_RST);
                    while (dev->SPI_FIFO_CNTL & (MASK_SPIM_SPIM_FIFO_CNTL_RCVR_RST | MASK_SPIM_SPIM_FIFO_CNTL_TX_RST))
                    {
                        /* Wait for these bits to clear */
                    }
                }
            }
            else if (val == 0)
            {
                if ((dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_FIFO_EN))
                {
                    dev->SPI_FIFO_CNTL &= ~MASK_SPIM_SPIM_FIFO_CNTL_FIFO_EN; /* Disable the FIFOs */
                }
            }
            else
            {
                iRet = -1;
            }
        }
        break; /* spim_option_fifo */

        case spi_option_fifo_size:
        {
            if (val == spi_fifo_size_16)
            {
                dev->SPI_FIFO_CNTL &= ~MASK_SPIM_SPIM_FIFO_CNTL_64_BYTE;
            }
            else if (val == spi_fifo_size_64)
            {
                dev->SPI_FIFO_CNTL |= MASK_SPIM_SPIM_FIFO_CNTL_64_BYTE;
            }
            else
            {
                iRet = -1;
            }
        }
        break; /* spi_option_fifo_size */

        /* case spim_option_fifo_word_size: */

        case spi_option_fifo_receive_trigger:
        {
            if (dev->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_64_BYTE)
            {
                /* 64 Byte FIFO in use... */
                if (val == 1)
                {
                    dev->SPI_FIFO_CNTL &= ~(MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                    dev->SPI_FIFO_CNTL |= (0 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                }
                else if (val == 16)
                {
                    dev->SPI_FIFO_CNTL &= ~(MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                    dev->SPI_FIFO_CNTL |= (1 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                }
                else if (val == 32)
                {
                    dev->SPI_FIFO_CNTL &= ~(MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                    dev->SPI_FIFO_CNTL |= (2 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                }
                else if (val == 56)
                {
                    dev->SPI_FIFO_CNTL &= ~(MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                    dev->SPI_FIFO_CNTL |= (3 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                }
                else
                {
                    iRet = -1;
                }
            }
            else
            {
                /* 16 Byte FIFO in use... */
                if (val == 1)
                {
                    dev->SPI_FIFO_CNTL &= ~(MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                    dev->SPI_FIFO_CNTL |= (0 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                }
                else if (val == 4)
                {
                    dev->SPI_FIFO_CNTL &= ~(MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                    dev->SPI_FIFO_CNTL |= (1 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                }
                else if (val == 8)
                {
                    dev->SPI_FIFO_CNTL &= ~(MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                    dev->SPI_FIFO_CNTL |= (2 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                }
                else if (val == 14)
                {
                    dev->SPI_FIFO_CNTL &= ~(MASK_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                    dev->SPI_FIFO_CNTL |= (3 << BIT_SPIM_SPIM_FIFO_CNTL_RCVR_TRIG);
                }
                else
                {
                    iRet = -1;
                }
            }
        }
        break; /* spi_option_fifo_receive_trigger */

        case spi_option_force_ss_assertions:
        {
            if (val == 1)
            {
                dev->SPI_STATUS &= ~MASK_SPIM_SPIM_STATUS_SSC_EN;
            }
            else if (val == 0)
            {
                dev->SPI_STATUS |= MASK_SPIM_SPIM_STATUS_SSC_EN;
            }
            else
            {
                iRet = -1;
            }
        }
        break; /* spi_option_force_ss_assertions */

        case spi_option_bus_width:
        {
            if (val == spi_width_1bit)
            {
            	dev->SPI_TNSFR_FRMT_CNTL &= ~(MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI | MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI);
            }
            else if (val == spi_width_2bit)
            {
                dev->SPI_TNSFR_FRMT_CNTL &= ~(MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI | MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI);
                dev->SPI_TNSFR_FRMT_CNTL |= (1 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI);
            }
            else if (val == spi_width_4bit)
            {
                dev->SPI_TNSFR_FRMT_CNTL &= ~(MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI | MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_QUAD_SPI);
                dev->SPI_TNSFR_FRMT_CNTL |= (2 << BIT_SPIM_SPIM_TNSFR_FRMT_CNTL_DUAL_SPI);
            }
            else
            {
                iRet = -1;
            }
        }
        break; /* spim_option_bus_width */

#if 0
      case spim_option_dual_quad_direction:
      {
          if (SPIM->STFCR & (SPIM_STFCR_DUALSPI | SPIM_STFCR_QUADSPI))
          {
              /* We are in Quad or Dual Mode... */
              if (val == 1)
              {
                  /* Write */
                  SPIM->STFCR &= ~SPIM_STFCR_DIR;
              }
              else if (val == 0)
              {
                  /* Read */
                  SPIM->STFCR |= SPIM_STFCR_DIR;
              }
              else
              {
                  iRet = -1;
              }
          }
          else
          {
              iRet = -1;
          }

      }
      break; /* spim_option_dual_quad_direction */
#endif

      case spi_option_fifo_transmit_trigger:
      {
#if defined(__FT900__)
        	if (sys_check_ft900_revB())
        	{
        		/* Option not applicable for FT900 Rev B */
        		iRet = -1;
        	}
        	else
#endif
        	{
#if defined(__FT900__)
        		ft900_spi_regs_ex_t *device = (ft900_spi_regs_ex_t *)dev;
#else
        		ft900_spi_regs_t *device = dev;
#endif

				  if (device->SPI_FIFO_CNTL & MASK_SPIM_SPIM_FIFO_CNTL_64_BYTE)
				  {
					  /* 64 Byte FIFO in use... */
					  if (val == 1)
					  {
						  device->SPI_FIFO_CNTL_2 &= ~(MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
					  }
					  else if (val == 16)
					  {
						  device->SPI_FIFO_CNTL_2 &= ~(MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
						  device->SPI_FIFO_CNTL_2 |= (1 << BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
					  }
					  else if (val == 32)
					  {
						  device->SPI_FIFO_CNTL_2 &= ~(MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
						  device->SPI_FIFO_CNTL_2 |= (2 << BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
					  }
					  else if (val == 56)
					  {
						  device->SPI_FIFO_CNTL_2 &= ~(MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
						  device->SPI_FIFO_CNTL_2 |= (3 << BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
					  }
					  else
					  {
						  iRet = -1;
					  }
				  }
				  else
				  {
					  /* 16 Byte FIFO in use... */
					  if (val == 1)
					  {
						  device->SPI_FIFO_CNTL_2 &= ~(MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
					  }
					  else if (val == 4)
					  {
						  device->SPI_FIFO_CNTL_2 &= ~(MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
						  device->SPI_FIFO_CNTL_2 |= (1 << BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
					  }
					  else if (val == 8)
					  {
						  device->SPI_FIFO_CNTL_2 &= ~(MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
						  device->SPI_FIFO_CNTL_2 |= (2 << BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
					  }
					  else if (val == 14)
					  {
						  device->SPI_FIFO_CNTL_2 &= ~(MASK_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
						  device->SPI_FIFO_CNTL_2 |= (3 << BIT_SPIM_SPIM_FIFO_CNTL_2_TX_TRIG_LVL);
					  }
					  else
					  {
						  iRet = -1;
					  }
				  }
        	}
      }
      break; /* spi_option_fifo_transmit_trigger */


      	case spi_option_multi_receive:
        {
            if (val == 1)
            {
                dev->SPI_TNSFR_FRMT_CNTL |= MASK_SPIM_SPIM_TNSFR_FMRT_CNTL_MULTI_REC;
            }
            else if (val == 0)
            {
                dev->SPI_TNSFR_FRMT_CNTL &= ~MASK_SPIM_SPIM_TNSFR_FMRT_CNTL_MULTI_REC;
            }
            else
            {
                iRet = -1;
            }
        }
        break; /* spi_option_multi_receive */


        case spi_option_baud_factor:
        {
#if defined(__FT900__)
        	if (sys_check_ft900_revB())
        	{
        		/* Option not applicable for FT900 Rev B */
        		iRet = -1;
        	}
        	else
#endif
        	{
#if defined(__FT900__)
        		ft900_spi_regs_ex_t *device = (ft900_spi_regs_ex_t *)dev;
#else
        		ft900_spi_regs_t *device = dev;
#endif
        		/* Enable baud register */
        		if (val)
                {
                    device->SPI_BAUD |= MASK_SPIM_SPIM_CNTL_2_BAUD_REG_EN;
                }
                else
                {
                    device->SPI_BAUD &= ~MASK_SPIM_SPIM_CNTL_2_BAUD_REG_EN;
                    /*BAUD register is disabled, transmission speed is controlled by
                     * SPR[2:0] of SPCR, CHG_SPR and FAST_SPI.
                     */
                }
                device->SPI_BAUD = val;
        	}
        }
        break;  /* spi_option_baud_factor */

        case spi_option_fast_spi:
        {
#if defined(__FT900__)
        	if (sys_check_ft900_revB())
        	{
        		/* Option not applicable for FT900 Rev B */
        		iRet = -1;
        	}
        	else
        	{
				if (val == 1)
				{
					dev->SPI_TNSFR_FRMT_CNTL |= MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_FAST_SPI;
				}
				else if (val == 0)
				{
					dev->SPI_TNSFR_FRMT_CNTL &= ~MASK_SPIM_SPIM_TNSFR_FRMT_CNTL_FAST_SPI;
				}
				else
				{
					iRet = -1;
				}
        	}
#else
        	iRet = -1;
#endif
        }
        break;    /* spi_option_fast_spi */


        case spi_option_change_spi_rate:
        {
#if defined(__FT900__)
        	if (sys_check_ft900_revB())
        	{
        		/* Option not applicable for FT900 Rev B */
        		iRet = -1;
        	}
        	else
        	{
        		ft900_spi_regs_ex_t *device = (ft900_spi_regs_ex_t *)dev;
				if (val == 1)
				{
					device->SPI_CNTL_2 |= MASK_SPIM_SPIM_CNTL_2_CHG_SPR;
				}
				else if (val == 0)
				{
					device->SPI_CNTL_2 &= ~MASK_SPIM_SPIM_CNTL_2_CHG_SPR;
				}
				else
				{
					iRet = -1;
				}
        	}
#else
        	iRet = -1;
#endif
        }
        break;    /* spi_option_change_spi_rate */

        case spi_option_slave_select_control:
        {
#if defined(__FT900__)
        	ft900_spi_regs_ex_t *device = (ft900_spi_regs_ex_t *)dev;
#else
        	ft900_spi_regs_t *device = dev;
#endif
        	device->SPI_SLV_SEL_CNTL &= (~val);
        }
        break;

        default:
            iRet = -1;
        break; /* default */
    }

    return iRet;
}

