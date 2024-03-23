#include "../Includes/ft900_rtc.h"

typedef struct
{
	uint8_t RTCsec;
	uint8_t RTCmin;
	uint8_t RTChour;
	uint8_t RTCday;
	uint8_t RTCdate;
	uint8_t RTCmth;
	uint8_t RTCyear;
	uint8_t RTCcontrol;
	uint8_t RTCosctrim;
	uint8_t reserved0;
	uint8_t alm0sec;
	uint8_t alm0min;
	uint8_t alm0hour;
	uint8_t alm0wkday;
	uint8_t alm0date;
	uint8_t alm0mnth;
	uint8_t reserved1;
	uint8_t alm1sec;
	uint8_t alm1min;
	uint8_t alm1hour;
	uint8_t alm1wkday;
	uint8_t alm1date;
	uint8_t alm1mnth;
}ext_rtc_reg_dump_t;

rtc_result ext_rtc_reg_dump()
{
	rtc_result iRet = RTC_OK;
	ext_rtc_reg_dump_t regs;

	iRet = i2cm_read(ADDR_RTCC,ADDR_SEC,(uint8_t*)&regs,0x17);checkStatus(iRet);

	tfp_printf("RTCsec 		= %x\r\n",regs.RTCsec);
	tfp_printf("RTCmin 		= %x\r\n",regs.RTCmin);
	tfp_printf("RTChour 	= %x\r\n",regs.RTChour);
	tfp_printf("RTCday 		= %x\r\n",regs.RTCday);
	tfp_printf("RTCdate 	= %x\r\n",regs.RTCdate);
	tfp_printf("RTCmth 		= %x\r\n",regs.RTCmth);
	tfp_printf("RTCyear 	= %x\r\n",regs.RTCyear);
	tfp_printf("RTCcontrol 	= %x\r\n",regs.RTCcontrol);
	tfp_printf("RTCosctrim 	= %x\r\n",regs.RTCosctrim);
	tfp_printf("reserved0 	= %x\r\n",regs.reserved0);
	tfp_printf("alm0sec 	= %x\r\n",regs.alm0sec);
	tfp_printf("alm0min 	= %x\r\n",regs.alm0min);
	tfp_printf("alm0hour 	= %x\r\n",regs.alm0hour);
	tfp_printf("alm0wkday 	= %x\r\n",regs.alm0wkday);
	tfp_printf("alm0date 	= %x\r\n",regs.alm0date);
	tfp_printf("alm0mnth 	= %x\r\n",regs.alm0mnth);
	tfp_printf("reserved1 	= %x\r\n",regs.reserved1);
	tfp_printf("alm1sec 	= %x\r\n",regs.alm1sec);
	tfp_printf("alm1min 	= %x\r\n",regs.alm1min);
	tfp_printf("alm1hour 	= %x\r\n",regs.alm1hour);
	tfp_printf("alm1wkday 	= %x\r\n",regs.alm1wkday);
    tfp_printf("alm1date 	= %x\r\n",regs.alm1date);
	tfp_printf("alm1mnth 	= %x\r\n",regs.alm1mnth);

__exit:
	return iRet;
}

/*
 *  @brief attach RTC interrupt with a interrupt callback function
 *  @cb call back function for the interrupt
 *  @return RTC_OK on success or RTC_ERROR on failure
 */
rtc_result ext_rtc_attach_interrupt(register_callback cb)
{
	rtc_result iRet = RTC_OK;

	interrupt_disable_globally();
	//interrupt_detach(interrupt_gpio);
	/* Set up the pin */
	gpio_function(RTC_INT_PIN,pad_gpio40);
	gpio_dir(RTC_INT_PIN, pad_dir_input);
	gpio_pull(RTC_INT_PIN, pad_pull_pullup);

	/* Attach an interrupt */
	iRet = interrupt_attach(interrupt_gpio, (uint8_t)(interrupt_gpio),cb);
	checkStatus(iRet);
	iRet = gpio_interrupt_enable(RTC_INT_PIN, gpio_int_edge_raising);
	checkStatus(iRet);
	interrupt_enable_globally();

__exit:
	return iRet;
}

/*
 *
 */
rtc_result ext_rtc_detach_interrupt()
{
	rtc_result iRet;
	iRet = gpio_interrupt_disable(RTC_INT_PIN);
	checkStatus(iRet);
	iRet = interrupt_detach(interrupt_gpio);
	checkStatus(iRet);

__exit:
	return iRet;
}

/**
 * @brief enables RTC oscillator
 */
static rtc_result enable_oscillator(void)
{
	rtc_result iRet;
	uint8_t txByte = 0;
	
	iRet = i2cm_read(ADDR_RTCC,ADDR_SEC,&txByte,1);
	checkStatus(iRet);
	txByte |= START_32KHZ;
	iRet = i2cm_write(ADDR_RTCC,ADDR_SEC,&txByte,1);
	checkStatus(iRet);	// init SEC register and start the 32khz oscillator
	//iRet = i2cm_read(ADDR_RTCC,ADDR_CTRL,&txByte,1);
	//checkStatus(iRet);
	//txByte |= EXTOSC;
	//iRet = i2cm_write(ADDR_RTCC,ADDR_SEC,&txByte,1);
	//checkStatus(iRet);  // Enable external clock source

__exit:
	return iRet;
}

/**
 * @brief disables RTC oscillator
 */
static rtc_result disable_oscillator(void)
{
	rtc_result iRet;
	uint8_t txByte = 0;

	iRet = i2cm_read(ADDR_RTCC,ADDR_SEC,&txByte,1);
	checkStatus(iRet);
	txByte &= (~START_32KHZ);
	iRet = i2cm_write(ADDR_RTCC,ADDR_SEC,&txByte,1);
	checkStatus(iRet);	// init SEC register and start the 32khz oscillator
	//iRet = i2cm_read(ADDR_RTCC,ADDR_CTRL,&txByte,1);
	//checkStatus(iRet);
	//txByte &= (~EXTOSC);
	//iRet = i2cm_write(ADDR_RTCC,ADDR_SEC,&txByte,1);
	//checkStatus(iRet);  // Enable external clock source
	
__exit:
	return iRet;
	
}

/**
 *
 *  @brief
 *  @param data Parameter_Description
 *  @return Return_Description
 *
 */
static uint8_t bcd2dec(uint8_t data)
{
	uint8_t msb = data / 16;
	data = (data & 0x0f) + (msb*10);
	return data;
}
/**
 *
 *  @brief
 *  @param data Parameter_Description
 *  @return Return_Description
 *
 */
static uint8_t dec2bcd(uint8_t data)
{
	uint8_t msb = data / 10;
	uint8_t lsb = data % 10;
	data = (msb * 16) + lsb;
	return data;
}

/**
 *
 *  @brief Initialize Real time clock
 *  @param register_callback callback function to register
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_init(register_callback cb)
{
	rtc_result iRet;
	uint8_t txByte = 0;
	ext_rtc_time_t time;
	iRet = i2cm_read(ADDR_RTCC,ADDR_DAY,&txByte,1);
	checkStatus(iRet);
	if((txByte&OSCON) != OSCON)             // if oscillator = not running, set time/date(arbitrary) else do nothing.
	{
		time.sec = 0;
		time.min = 10;
		time.hour = 10;
		time.day = 1;
		time.fmt_12_24 = HOUR_FORMAT_12;
		time.AM_PM = AM_HOUR_FORMAT;
		time.date = 01;
		time.month = 01;
		time.year = 17;
		ext_rtc_write(time);
		iRet = enable_oscillator();
		checkStatus(iRet);
	}

	if(cb != NULL)
		iRet = ext_rtc_attach_interrupt(cb);

__exit:
	return iRet;
}

/**
 *
 *  @brief Enable the Square Wave.
 *  @param ext_rtc_square_wave_t type of square wave to enable
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_enable_squarewave(ext_rtc_square_wave_t option)
{
	rtc_result iRet = RTC_OK;
	uint8_t txByte = 0;
	uint8_t address = ADDR_CTRL;

	assert(option,0x00,0x03);
	iRet = i2cm_read(ADDR_RTCC,ADDR_CTRL,&txByte,1);
	checkStatus(iRet);
	txByte &= 0xFC;
	txByte |= (option | SQWE); //select frequency and enable square wave
	iRet = i2cm_write(ADDR_RTCC,address,&txByte,1);
	checkStatus(iRet);

__exit:
	return iRet;
}
/**
 *
 *  @brief Disable the Square wave
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_disable_squarewave(void)
{
	rtc_result iRet = RTC_OK;
	uint8_t txByte = 0;
	iRet = i2cm_read(ADDR_RTCC,ADDR_CTRL,&txByte,1);
	checkStatus(iRet);
	txByte &= (~SQWE);
	iRet = i2cm_write(ADDR_RTCC,ADDR_CTRL,&txByte,1);
	checkStatus(iRet);

__exit:
	return iRet;
}

/**
 *
 *  @brief Enable the interrupt
 *  @param ext_rtc_interrupt_t type of interrupt to enable[AL_INT_0,AL_INT_1, SQWE_INT]
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_enable_interrupt(ext_rtc_interrupt_t _interrupt)
{
	rtc_result iRet = RTC_OK;
	uint8_t txbuf = 0;

	if((_interrupt != AL_INT_0) && (_interrupt != AL_INT_1) && (_interrupt != SQWE_INT) && (_interrupt != ALL_INT))
	{
		iRet = RTC_INVALID_PARAMETERS;
		checkStatus(iRet);
	}

	iRet = i2cm_read(ADDR_RTCC,ADDR_CTRL,&txbuf,1);
	checkStatus(iRet);
	txbuf |= _interrupt;
	iRet = i2cm_write(ADDR_RTCC,ADDR_CTRL,&txbuf,1);
	checkStatus(iRet);
	delayms(100);
__exit:
	return iRet;
}

/**
 *
 *  @brief Disable the interrupt
 *  @param ext_rtc_interrupt_t type of interrupt to disable[AL_INT_0,AL_INT_1, SQWE_INT]
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_disable_interrupt(ext_rtc_interrupt_t _interrupt)
{
	rtc_result iRet = RTC_OK;
	uint8_t txbuf = 0;

	if((_interrupt != AL_INT_0) && (_interrupt != AL_INT_1) && (_interrupt != SQWE_INT) && (_interrupt != ALL_INT) )
	{
		iRet = RTC_INVALID_PARAMETERS;
		checkStatus(iRet);
	}

	iRet = i2cm_read(ADDR_RTCC,ADDR_CTRL,&txbuf,1);
	checkStatus(iRet);
	txbuf &= ~_interrupt;
	iRet = i2cm_write(ADDR_RTCC,ADDR_CTRL,&txbuf,1);
	checkStatus(iRet);
	delayms(100);
	/*clear alarm flags*/
	if((_interrupt == AL_INT_0) || (_interrupt == ALL_INT))
		ext_rtc_clear_alarm_interrupt(ALARM_0);

	if((_interrupt == AL_INT_1) || (_interrupt == ALL_INT))
		ext_rtc_clear_alarm_interrupt(ALARM_1);

__exit:
	return iRet;
}

/**
 * @brief Check if an interrupt has been triggered
 *  @param interrupt The interrupt to check
 *  @warning This function clears the current interrupt status bit
 *  @warning This function does not clear nor return status for square wave
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 */
int8_t ext_rtc_is_interrupted(ext_rtc_interrupt_t interrupt)
{
	int8_t iRet = 0;
	ext_rtc_alarm_stat stat = 0;

	stat = ext_rtc_alarm_status(); //get alarm status
	if (gpio_is_interrupted(RTC_INT_PIN))
	{
		if((interrupt == AL_INT_0) && (stat & AL_0))
		{
			ext_rtc_clear_alarm_interrupt(ALARM_0);
			iRet = 1;
		}
		else if((interrupt == AL_INT_1) && (stat & AL_1))
		{
			ext_rtc_clear_alarm_interrupt(ALARM_1);
			iRet = 1;
		}
	}
	return iRet;
}

/**
 *
 *  @brief Set the Time and Date
 *  @param ext_rtc_time_t to set the real time clock
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_write(ext_rtc_time_t time)
{
	rtc_result iRet = RTC_OK;
	uint8_t txBuffer[7] = {0};
	uint8_t time_fmt;
	disable_oscillator();
	txBuffer[0] = dec2bcd(time.sec) /*| osc_st*/;
	txBuffer[1] = dec2bcd(time.min);
	time_fmt 	=  ((time.fmt_12_24 << MASK_HOUR_12) & HOUR_12) |\
				   ((time.AM_PM << MASK_HOUR_AM_PM) & HOUR_AM_PM);
	txBuffer[2] = dec2bcd(time.hour) | time_fmt;
	txBuffer[3] = time.day;
	txBuffer[4] = dec2bcd(time.date);
	txBuffer[5] = dec2bcd(time.month);
	txBuffer[6] = dec2bcd(time.year);
	iRet = i2cm_write(ADDR_RTCC,ADDR_SEC,txBuffer,7);
	checkStatus(iRet);
	enable_oscillator();

__exit:
	return iRet;
}

/**
 *
 *  @brief get the current value of Real time clock
 *  @param ext_rtc_time_t	 pointer to store the real time clock values
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_read(ext_rtc_time_t *time)
{
	rtc_result iRet = RTC_OK;
	uint8_t rxBuffer[7] = {0};

	iRet = i2cm_read(ADDR_RTCC,ADDR_SEC,rxBuffer,7);
	checkStatus(iRet);
	time->sec = bcd2dec((rxBuffer[0]) & 0x7f);
	time->min = bcd2dec((rxBuffer[1]) & 0x7f);
	time->fmt_12_24 = (rxBuffer[2] & HOUR_12) >> MASK_HOUR_12;
	if(time->fmt_12_24)
		time->hour = bcd2dec((rxBuffer[2]) & 0x1f);
	else
		time->hour = bcd2dec((rxBuffer[2]) & 0x3f);
	time->AM_PM = (rxBuffer[2] & HOUR_AM_PM) >> MASK_HOUR_AM_PM;
	time->day = bcd2dec((rxBuffer[3]) & 0x07);
	time->date = bcd2dec((rxBuffer[4]) & 0x3f);
	time->month = bcd2dec((rxBuffer[5]) & 0x1f);
	time->year =  bcd2dec(rxBuffer[6]);

__exit:
	return iRet;
}

/**
 *
 *  @brief set and configure alarm of RTC
 *  @param ext_rtc_alarm_num 		alarm number to set alarm ALARM_0 or ALARM_1
 *  @param ext_rtc_alarm_type_t    	parameter to match alarm sec, min ...
 *  @param ext_rtc_alarm_time_t   		values to match alarm
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_set_alarm(ext_rtc_alarm_num alrm_num, ext_rtc_alarm_time_t al_time, ext_rtc_alarm_type_t alrm_type)
{
	rtc_result iRet = RTC_OK;
	uint8_t time_fmt,alarm_addr,temp = 0,tmp;
	uint8_t txBuffer[6] = {0};

	assert(alrm_num,0,1);
	assert(alrm_type,0,7);
	if ((alrm_type == 5) ||(alrm_type == 6))
	{
		iRet = RTC_ERROR;
		checkStatus(iRet);
	}

	txBuffer[0] = dec2bcd(al_time.sec);
	txBuffer[1] = dec2bcd(al_time.min);
	time_fmt 	=  ((al_time.fmt_12_24 << MASK_HOUR_12) & HOUR_12) |\
				   ((al_time.AM_PM << MASK_HOUR_AM_PM) & HOUR_AM_PM);
	txBuffer[2] = dec2bcd(al_time.hour) | time_fmt;
	txBuffer[3] = al_time.day;
	txBuffer[4] = dec2bcd(al_time.date);
	txBuffer[5] = dec2bcd(al_time.month);

	if(alrm_num == ALARM_0)
	{
		alarm_addr = ALARM_0_REG;
	}
	else
	{
		alarm_addr = ALARM_1_REG;
	}

	//iRet = enable_oscillator();checkStatus(iRet);
	//iRet = i2cm_read(ADDR_RTCC,alarm_addr + 3,&temp,1);
	//checkStatus(iRet);
	iRet = i2cm_read(ADDR_RTCC,alarm_addr + 3,&tmp,1);
	checkStatus(iRet);

	temp = 0;
	temp |= ALMx_POL;
	if(tmp & ALMx_IF)
		temp |= ALMx_IF;
	temp |= (alrm_type << 4) | ALMx_IF; 	//select alarm mask and clear ALMx_IF by writing 1 to it
	iRet = i2cm_write(ADDR_RTCC,alarm_addr + 3,&temp,1);
	checkStatus(iRet);

	switch(alrm_type) {
		case seconds:
			iRet = i2cm_write(ADDR_RTCC,alarm_addr + 0,&txBuffer[0],1);
			checkStatus(iRet);
			break;

		case minutes:
			iRet = i2cm_write(ADDR_RTCC,alarm_addr + 1,&txBuffer[1],1);
			checkStatus(iRet);
			break;

		case hours:
			iRet = i2cm_write(ADDR_RTCC,alarm_addr + 2,&txBuffer[2],1);
			checkStatus(iRet);
			break;

		case day_of_week:
			txBuffer[3] |= temp;
			iRet = i2cm_write(ADDR_RTCC,alarm_addr + 3,&txBuffer[3],1);
			checkStatus(iRet);
			break;

		case date:
			iRet = i2cm_write(ADDR_RTCC,alarm_addr + 4,&txBuffer[4],1);
			checkStatus(iRet);
			break;

		case sec_min_hr_day_date_month:
			iRet = i2cm_write(ADDR_RTCC,alarm_addr,txBuffer,6);
			checkStatus(iRet);
			break;

		default:
			iRet = RTC_ERROR;
			checkStatus(iRet);
	}

	temp = (alrm_num ==ALARM_0)?AL_INT_0:AL_INT_1;
	iRet = ext_rtc_enable_interrupt(temp);
	checkStatus(iRet);

__exit:
	return iRet;
}


/**
 *
 *  @brief clear specified  alarm interrupt
 *  @param ext_rtc_alarm_num 		alarm number to disable ALARM_0 or ALARM_1
 *  @return					RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_clear_alarm_interrupt(ext_rtc_alarm_num alrm_num)
{
	rtc_result iRet = RTC_OK;
	uint8_t txbuf,alarm_addr;

	assert(alrm_num,0,1);
	if(alrm_num == ALARM_0)
	{
		alarm_addr = ALARM_0_REG;
	}
	else
	{
		alarm_addr = ALARM_1_REG;
	}

	iRet = i2cm_read(ADDR_RTCC, alarm_addr + 3 , &txbuf, 1);
	checkStatus(iRet);

	txbuf |= ALMx_IF;
	iRet = i2cm_write(ADDR_RTCC, alarm_addr + 3, &txbuf, 1);
	checkStatus(iRet);

__exit:
	return iRet;
}

/**
 *
 *  @brief get alarm interrupt status
 *  @return	 status of alarm interrupt[0- NONE, 1- AL-1, 2- AL-2, 3- both] or RTC_ERROR on failure
 *
 */
ext_rtc_alarm_stat ext_rtc_alarm_status(void)
{
	uint8_t txbuf, iRet, alarm_status = 0 ;

	iRet = i2cm_read(ADDR_RTCC, ADDR_ALM0CTL , &txbuf, 1);
	checkStatus(iRet);
	if(txbuf & ALMx_IF)
		alarm_status = AL_0;
	iRet = i2cm_read(ADDR_RTCC, ADDR_ALM1CTL, &txbuf, 1);
	checkStatus(iRet);
	if(txbuf & ALMx_IF)
		alarm_status |= AL_1;

__exit:
	return alarm_status;
}

/**
 *
 *  @brief Write to on-board SRAM on RTC(64 bytes)
 *  @param address  ram location to write
 *  @param buffer	pointer to the array to write
 *  @param len 		The number of bytes to write
 *  @return			RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_write_ram(uint8_t address,uint8_t *buffer,uint8_t len)
{
	rtc_result iRet = RTC_OK;
	assert(address, 0, 0x3F);
	address += SRAM_PTR;
	iRet = i2cm_write(ADDR_RTCC, address,buffer,len);
	checkStatus(iRet);

__exit:
	return iRet;
}

/**
 *
 *  @brief Read to on-board SRAM on RTC(64 bytes)
 *  @param address ram location to read
 *  @param buffer  pointer to the array to read into
 *  @param len     The number of bytes to write
 *   @return	   RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_read_ram(uint8_t address,uint8_t *buffer,uint8_t len)
{
	rtc_result iRet = RTC_OK;
	assert(address, 0, 0x3F);
	address += SRAM_PTR;
	iRet = i2cm_read(ADDR_RTCC,address,buffer,len);
	checkStatus(iRet);

__exit:
	return iRet;
}
