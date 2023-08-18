#ifndef FT900_RTC_H_
#define FT900_RTC_H_

#include <stdio.h>
#include <stdint.h>
#include <ft900.h>
#include <tinyprintf.h>

 #define  ADDR_EEPROM_WRITE 0xae       //  DEVICE ADDR for EEPROM (writes)
 #define  ADDR_EEPROM_READ  0xaf       //  DEVICE ADDR for EEPROM (reads)
 #define  ADDR_RTCC_WRITE   0xde       //  DEVICE ADDR for RTCC MCHP  (writes)
 #define  ADDR_RTCC		    0xde       //  DEVICE ADDR for RTCC MCHP
 #define  ADDR_RTCC_READ    0xdf       //  DEVICE ADDR for RTCC MCHP  (reads)
//.............................................................................................
 #define  SRAM_PTR          0x20       //  pointer of the SRAM area (RTCC)
 #define  ADDR_EEPROM_SR    0xff       //  STATUS REGISTER in the  EEPROM
//.............................................................................................
 #define  ADDR_SEC          0x00       //  address of SECONDS      register
 #define  ADDR_MIN          0x01       //  address of MINUTES      register
 #define  ADDR_HOUR         0x02       //  address of HOURS        register
 #define  ADDR_DAY          0x03       //  address of DAY OF WK    register
 #define  ADDR_STAT         0x03       //  address of STATUS       register
 #define  ADDR_DATE         0x04       //  address of DATE         register
 #define  ADDR_MNTH         0x05       //  address of MONTH        register
 #define  ADDR_YEAR         0x06       //  address of YEAR         register
 #define  ADDR_CTRL         0x07       //  address of CONTROL      register
 #define  ADDR_CAL          0x08       //  address of CALIB        register
 #define  ADDR_ULID         0x09       //  address of UNLOCK ID    register
//............................................................................................
 #define  ADDR_ALM0SEC      0x0a       //  address of ALARM0 SEC   register
 #define  ADDR_ALM0MIN      0x0b       //  address of ALARM0 MIN   register
 #define  ADDR_ALM0HR       0x0c       //  address of ALARM0 HOUR  register
 #define  ADDR_ALM0CTL      0x0d       //  address of ALARM0 CONTR register
 #define  ADDR_ALM0DAT      0x0e       //  address of ALARM0 DATE  register
 #define  ADDR_ALM0MTH      0x0f       //  address of ALARM0 MONTH register
//.............................................................................................
 #define  ADDR_ALM1SEC      0x11       //  address of ALARM1 SEC   register
 #define  ADDR_ALM1MIN      0x12       //  address of ALARM1 MIN   register
 #define  ADDR_ALM1HR       0x13       //  address of ALARM1 HOUR  register
 #define  ADDR_ALM1CTL      0x14       //  address of ALARM1 CONTR register
 #define  ADDR_ALM1DAT      0x15       //  address of ALARM1 DATE  register
 #define  ADDR_ALM1MTH      0x16       //  address of ALARM1 MONTH register
//.............................................................................................
 #define  ADDR_SAVtoBAT_MIN 0x18       //  address of T_SAVER MIN(VDD->BAT)
 #define  ADDR_SAVtoBAT_HR  0x19       //  address of T_SAVER HR (VDD->BAT)
 #define  ADDR_SAVtoBAT_DAT 0x1a       //  address of T_SAVER DAT(VDD->BAT)
 #define  ADDR_SAVtoBAT_MTH 0x1b       //  address of T_SAVER MTH(VDD->BAT)
//.............................................................................................
 #define  ADDR_SAVtoVDD_MIN 0x1c       //  address of T_SAVER MIN(BAT->VDD)
 #define  ADDR_SAVtoVDD_HR  0x1d       //  address of T_SAVER HR (BAT->VDD)
 #define  ADDR_SAVtoVDD_DAT 0x1e       //  address of T_SAVER DAT(BAT->VDD)
 #define  ADDR_SAVtoVDD_MTH 0x1f       //  address of T_SAVER MTH(BAT->VDD)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                  GLOBAL CONSTANTS RTCC - INITIALIZATION
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 #define  PM                0x20       				//  post-meridian bit (ADDR_HOUR)
 #define  OUT_PIN           0x80       				//  = b7 (ADDR_CTRL)
 #define  SQWE              0x40       				//  SQWE = b6 (ADDR_CTRL)
 
 #define  ALM_NO            0x00       				//  no alarm activated        (ADDR_CTRL)
 #define  ALM_0             0x10       				//  ALARM0 is       activated (ADDR_CTRL)
 #define  ALM_1             0x20       				//  ALARM1 is       activated (ADDR_CTRL)
 #define  ALM_01            0x30       				//  both alarms are activated (ADDR_CTRL)
 #define  MFP_64H           0x04       				//  MFP = SQVAW(64 HERZ)      (ADDR_CTRL)
 #define  ALMx_POL          0x80       				//  polarity of MFP on alarm  (ADDR_ALMxCTL)
 #define  ALMx_IF           0x08       				//  MASK of the ALARM_IF      (ADDR_ALMxCTL)
 
 #define  ALARM_0_REG		0x0A					//  Register set of alarm 0
 #define  ALARM_1_REG		0x11					//  Register set of alarm 1
 #define  MASK_OSCON        5       				
 #define  OSCON             (1 << MASK_OSCON)       //  state of the oscillator(running or not)
 #define  VBATEN            0x08       				//  enable battery for back-up
 #define  EXTOSC    		0x08       				//  external oscillator input bit
 #define  START_32KHZ       0x80       				//  start crystal: ST = b7 (ADDR_SEC)
 #define  LP                0x20       				//  mask for the leap year bit(MONTH REG)
 #define  MASK_HOUR_12		6
 #define  HOUR_12           (1 << MASK_HOUR_12) 	//  12 hours format   (ADDR_HOUR)
 #define  MASK_HOUR_AM_PM	5
 #define  HOUR_AM_PM        (1 << MASK_HOUR_AM_PM)	//  AM/PM hours format (ADDR_HOUR)


/**
 * RTC Pin configuration
 */
#define RTC_INT_PIN					40

typedef enum{
	AM_HOUR_FORMAT,
	PM_HOUR_FORMAT,
}hour_format_indecator;

typedef enum{
	HOUR_FORMAT_24,
	HOUR_FORMAT_12,
}hour_format;

/**
 *  @brief structure for RTC time-keeping registers
 */
typedef struct
{
	uint8_t sec;		//seconds 0-59
	uint8_t min;		//minutes 0-59
	uint8_t hour;		//hours 0-11, 0-23
	uint8_t day;		//day 1-7
	uint8_t date;		//date 1 - 28,29,30,31
	uint8_t month;		//month 1-12
	uint8_t year;		//year 0-99
	uint8_t AM_PM;		// am/pm indicator AM = 0, PM = 1
	uint8_t fmt_12_24;	//hour format 0 = 12-hour, 1 = 24-hour
}ext_rtc_time_t;

/**
 *  @brief structure for RTC alarm time-keeping registers
 */
typedef struct
{
	uint8_t sec;		//seconds 0-59
	uint8_t min;		//minutes 0-59
	uint8_t hour;		//hours 0-11, 0-23
	uint8_t day;		//day 1-7
	uint8_t date;		//date 1 - 28,29,30,31
	uint8_t month;		//month 1-12
	uint8_t AM_PM;		// am/pm indicator AM = 0, PM = 1
	uint8_t fmt_12_24;	//hour format 0 = 12-hour, 1 = 24-hour
}ext_rtc_alarm_time_t;

/**
 * @brief days
 */
typedef enum
{
	Monday = 1,
	Tuesday,
	Wednesday,
	Thurdsay,
	Friday,
	Saturday,
	Sunday
}ext_rtc_days_t;

/**
 * @brief months
 */
typedef enum
{
	January = 1,
	February,
	March,
	April,
	May,
	June,
	July,
	August,
	September,
	October,
	November,
	December
}ext_rtc_months_t;

/**
 * @supported square wave frequencies
 */
typedef enum
{
	 square_wave_01Hz	= 0x00,       //  MFP = SQVAW(01 HERZ)      (ADDR_CTRL)
     square_wave_4KHz	= 0x01,       //  MFP = SQVAW(04 KHZ)       (ADDR_CTRL)
     square_wave_8KHz	= 0x02,       //  MFP = SQVAW(08 KHZ)       (ADDR_CTRL)
     square_wave_32KHz	= 0x03,       //  MFP = SQVAW(32 KHZ)       (ADDR_CTRL)
}ext_rtc_square_wave_t;

/**
 * @alarm number
 */
typedef enum
{
	ALARM_0,
	ALARM_1,
}ext_rtc_alarm_num;

/**
 * @alarm status
 */
typedef enum
{
	ERR			= -1,
	NONE		=  0,
	AL_0		=  1,
	AL_1		=  2,
	BOTH		=  3,
}ext_rtc_alarm_stat;

/**
 * @supported alarm types
 */
typedef enum
{
	seconds,
	minutes,
	hours,
	day_of_week,
	date,
	sec_min_hr_day_date_month = 0x07,
}ext_rtc_alarm_type_t;

/**
 * @
 */
typedef enum
{
	RTC_ERROR 				= -1,
	RTC_OK					=  0,
	RTC_INVALID_PARAMETERS	=  1,
}rtc_result;

/**
 * @ interrupt types
 */
typedef enum
{
	AL_INT_0 = ALM_0,
	AL_INT_1 = ALM_1,
	SQWE_INT = SQWE,
	ALL_INT	 = (ALM_0 | ALM_1 | SQWE), //alaram 0 + alaram 1 + square wave
}ext_rtc_interrupt_t;


#define checkStatus(iRet)								\
	if(iRet != RTC_OK)									\
	{													\
		goto __exit;									\
	}

#define assert(val, min, max)							\
	if((val < min) || (val > max))						\
	{													\
		iRet = RTC_INVALID_PARAMETERS;					\
		goto __exit;									\
	}

#define checkStat(iRet, message)						\
	if(iRet != RTC_OK)									\
	{													\
		printf("Error in %s \r\n",message);				\
		exit(1);										\
	}


typedef void (*register_callback)(void);

rtc_result ext_rtc_attach_interrupt(register_callback cb);
rtc_result ext_rtc_detach_interrupt();

/**
 *
 *  @brief Initialize Real time clock
 *  @param cb callback function to register
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_init(register_callback cb);

/**
 *
 *  @brief Enable the Square Wave.
 *  @param option type of square wave to enable
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_enable_squarewave(ext_rtc_square_wave_t option);

/**
 *
 *  @brief Disable the Square wave
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_disable_squarewave(void);

/**
 *
 *  @brief Enable the interrupt
 *  @param _interrupt type of interrupt to enable[AL_INT_0,AL_INT_1, SQWE_INT]
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_enable_interrupt(ext_rtc_interrupt_t _interrupt);

/**
 *
 *  @brief Disable the interrupt
 *  @param _interrupt type of interrupt to disable[AL_INT_0,AL_INT_1, SQWE_INT]
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_disable_interrupt(ext_rtc_interrupt_t _interrupt);

/**
 * @brief Check if an interrupt has been triggered
 *  @param _interrupt The interrupt to check
 *  @warning This function clears the current interrupt status bit
 *  @warning This function does not clear nor return status for square wave
 *  @returns 1 when interrupted, 0 when not interrupted, -1 otherwise
 */
int8_t ext_rtc_is_interrupted(ext_rtc_interrupt_t _interrupt);

/**
 *
 *  @brief Set the Time and Date
 *  @param t to set the real time clock
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_write(ext_rtc_time_t time);

/**
 *
 *  @brief get the current value of Real time clock
 *  @param t pointer to store the real time clock values
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_read(ext_rtc_time_t *time);

/**
 *
 *  @brief set and configure alarm of RTC
 *  @param alrm_mum 				alarm number to set alarm ALARM_0 or ALARM_1
 *  @param alrm_type    			parameter to match alarm sec, min ...
 *  @param ext_rtc_alarm_time_t   	values to match alarm
 *  @return RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_set_alarm(ext_rtc_alarm_num alrm_num, ext_rtc_alarm_time_t time, ext_rtc_alarm_type_t alrm_type);

/**
 *
 *  @brief clear specified  alarm interrupt
 *  @param alrm_mum 		alarm number to disable ALARM_0 or ALARM_1
 *  @return					RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_clear_alarm_interrupt(ext_rtc_alarm_num alrm_num);

/**
 *
 *  @brief get alarm interrupt status
 *  @return	 status of alarm interrupt[0- NONE, 1- AL-1, 2- AL-2, 3- both] or RTC_ERROR on failure
 *
 */
ext_rtc_alarm_stat ext_rtc_alarm_status(void);

/**
 *
 *  @brief Write to on-board SRAM on RTC(64 bytes)
 *  @param address  ram location to write
 *  @param buffer	pointer to the array to write
 *  @param len 		The number of bytes to write
 *  @return			RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_write_ram(uint8_t address,uint8_t *buffer,uint8_t len);

/**
 *
 *  @brief Read to on-board SRAM on RTC(64 bytes)
 *  @param address ram location to read
 *  @param buffer  pointer to the array to read into
 *  @param len     The number of bytes to write
 *  @return	   RTC_OK on success or RTC_ERROR on failure
 *
 */
rtc_result ext_rtc_read_ram(uint8_t address,uint8_t *buffer,uint8_t len);

rtc_result ext_rtc_reg_dump();
#endif /* FT900_RTC_H_ */
