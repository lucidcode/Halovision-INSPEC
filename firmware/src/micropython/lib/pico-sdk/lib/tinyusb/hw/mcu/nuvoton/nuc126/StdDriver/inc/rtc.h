/**************************************************************************//**
 * @file     rtc.h
 * @version  V3.00
 * $Revision: 7 $
 * $Date: 16/10/25 4:25p $
 * @brief    NUC126 series Real Time Clock(RTC) driver header file
 *
 * @note
 * Copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup RTC_Driver RTC Driver
  @{
*/

/** @addtogroup RTC_EXPORTED_CONSTANTS RTC Exported Constants
  @{
*/
/*---------------------------------------------------------------------------------------------------------*/
/*  RTC Initial Keyword Constant Definitions                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define RTC_INIT_KEY            0xA5EB1357UL    /*!< RTC Initiation Key to make RTC leaving reset state */
#define RTC_WRITE_KEY           0x0000A965UL    /*!< RTC Register Access Enable Key to enable RTC read/write accessible and kept 1024 RTC clock */

/*---------------------------------------------------------------------------------------------------------*/
/*  RTC Time Attribute Constant Definitions                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define RTC_CLOCK_12            0               /*!< RTC as 12-hour time scale with AM and PM indication */
#define RTC_CLOCK_24            1               /*!< RTC as 24-hour time scale */
#define RTC_AM                  1               /*!< RTC as AM indication */
#define RTC_PM                  2               /*!< RTC as PM indication */

/*---------------------------------------------------------------------------------------------------------*/
/*  RTC Tick Period Constant Definitions                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define RTC_TICK_1_SEC          0x0UL           /*!< RTC time tick period is 1 second */
#define RTC_TICK_1_2_SEC        0x1UL           /*!< RTC time tick period is 1/2 second */
#define RTC_TICK_1_4_SEC        0x2UL           /*!< RTC time tick period is 1/4 second */
#define RTC_TICK_1_8_SEC        0x3UL           /*!< RTC time tick period is 1/8 second */
#define RTC_TICK_1_16_SEC       0x4UL           /*!< RTC time tick period is 1/16 second */
#define RTC_TICK_1_32_SEC       0x5UL           /*!< RTC time tick period is 1/32 second */
#define RTC_TICK_1_64_SEC       0x6UL           /*!< RTC time tick period is 1/64 second */
#define RTC_TICK_1_128_SEC      0x7UL           /*!< RTC time tick period is 1/128 second */

/*---------------------------------------------------------------------------------------------------------*/
/*  RTC Day of Week Constant Definitions                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define RTC_SUNDAY              0x0UL           /*!< Day of the Week is Sunday */
#define RTC_MONDAY              0x1UL           /*!< Day of the Week is Monday */
#define RTC_TUESDAY             0x2UL           /*!< Day of the Week is Tuesday */
#define RTC_WEDNESDAY           0x3UL           /*!< Day of the Week is Wednesday */
#define RTC_THURSDAY            0x4UL           /*!< Day of the Week is Thursday */
#define RTC_FRIDAY              0x5UL           /*!< Day of the Week is Friday */
#define RTC_SATURDAY            0x6UL           /*!< Day of the Week is Saturday */

/*---------------------------------------------------------------------------------------------------------*/
/*  RTC Miscellaneous Constant Definitions                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define RTC_YEAR2000            2000            /*!< RTC Reference for compute year data */

/*@}*/ /* end of group RTC_EXPORTED_CONSTANTS */


/** @addtogroup RTC_EXPORTED_STRUCTS RTC Exported Structs
  @{
*/
/**
  * @details    RTC define Time Data Struct
  */
typedef struct
{
    uint32_t u32Year;           /*!< Year value */
    uint32_t u32Month;          /*!< Month value */
    uint32_t u32Day;            /*!< Day value */
    uint32_t u32DayOfWeek;      /*!< Day of week value */
    uint32_t u32Hour;           /*!< Hour value */
    uint32_t u32Minute;         /*!< Minute value */
    uint32_t u32Second;         /*!< Second value */
    uint32_t u32TimeScale;      /*!< 12-Hour, 24-Hour */
    uint32_t u32AmPm;           /*!< Only Time Scale select 12-hr used */
} S_RTC_TIME_DATA_T;

/*@}*/ /* end of group RTC_EXPORTED_STRUCTS */


/** @addtogroup RTC_EXPORTED_FUNCTIONS RTC Exported Functions
  @{
*/

/**
  * @brief      Indicate is Leap Year or not
  *
  * @param      None
  *
  * @retval     0   This year is not a leap year
  * @retval     1   This year is a leap year
  *
  * @details    According to current date, return this year is leap year or not.
  */
#define RTC_IS_LEAP_YEAR()              (RTC->LEAPYEAR & RTC_LEAPYEAR_LEAPYEAR_Msk ? 1:0)

/**
  * @brief      Clear RTC Alarm Interrupt Flag
  *
  * @param      None
  *
  * @return     None
  *
  * @details    This macro is used to clear RTC alarm interrupt flag.
  */
#define RTC_CLEAR_ALARM_INT_FLAG()      (RTC->INTSTS = RTC_INTSTS_ALMIF_Msk)

/**
  * @brief      Clear RTC Tick Interrupt Flag
  *
  * @param      None
  *
  * @return     None
  *
  * @details    This macro is used to clear RTC tick interrupt flag.
  */
#define RTC_CLEAR_TICK_INT_FLAG()       (RTC->INTSTS = RTC_INTSTS_TICKIF_Msk)

/**
  * @brief      Get RTC Alarm Interrupt Flag
  *
  * @param      None
  *
  * @retval     0   RTC alarm interrupt did not occur
  * @retval     1   RTC alarm interrupt occurred
  *
  * @details    This macro indicates RTC alarm interrupt occurred or not.
  */
#define RTC_GET_ALARM_INT_FLAG()        ((RTC->INTSTS & RTC_INTSTS_ALMIF_Msk)? 1:0)

/**
  * @brief      Get RTC Time Tick Interrupt Flag
  *
  * @param      None
  *
  * @retval     0   RTC time tick interrupt did not occur
  * @retval     1   RTC time tick interrupt occurred
  *
  * @details    This macro indicates RTC time tick interrupt occurred or not.
  */
#define RTC_GET_TICK_INT_FLAG()         ((RTC->INTSTS & RTC_INTSTS_TICKIF_Msk)? 1:0)

/**
  * @brief      Enable RTC Tick Wake-up Function
  *
  * @param      None
  *
  * @return     None
  *
  * @details    This macro is used to enable RTC tick interrupt wake-up function.
  */
#define RTC_ENABLE_TICK_WAKEUP()        (RTC->INTEN |= RTC_INTEN_TICKIEN_Msk);

/**
  * @brief      Disable RTC Tick Wake-up Function
  *
  * @param      None
  *
  * @return     None
  *
  * @details    This macro is used to disable RTC tick interrupt wake-up function.
  */
#define RTC_DISABLE_TICK_WAKEUP()       (RTC->INTEN &= ~RTC_INTEN_TICKIEN_Msk);

/**
  * @brief      Wait RTC Access Enable
  *
  * @param      None
  *
  * @return     None
  *
  * @details    This function is used to enable the maximum RTC read/write accessible time.
  */
static __INLINE void RTC_WaitAccessEnable(void)
{
    /* To wait RWENF bit is cleared and enable RWENF bit (Access Enable bit) again */
    RTC->RWEN = 0x0; // clear RWENF immediately
    while(RTC->RWEN & RTC_RWEN_RWENF_Msk);

    /* To wait RWENF bit is set and user can access the protected-register of RTC from now on */
    RTC->RWEN = RTC_WRITE_KEY;
    while((RTC->RWEN & RTC_RWEN_RWENF_Msk) == 0x0);
}

void RTC_Open(S_RTC_TIME_DATA_T *sPt);
void RTC_Close(void);
void RTC_32KCalibration(uint32_t u32FrequencyX100);
void RTC_GetDateAndTime(S_RTC_TIME_DATA_T *sPt);
void RTC_GetAlarmDateAndTime(S_RTC_TIME_DATA_T *sPt);
void RTC_SetDateAndTime(S_RTC_TIME_DATA_T *sPt);
void RTC_SetAlarmDateAndTime(S_RTC_TIME_DATA_T *sPt);
void RTC_SetDate(uint32_t u32Year, uint32_t u32Month, uint32_t u32Day, uint32_t u32DayOfWeek);
void RTC_SetTime(uint32_t u32Hour, uint32_t u32Minute, uint32_t u32Second, uint32_t u32TimeMode, uint32_t u32AmPm);
void RTC_SetAlarmDate(uint32_t u32Year, uint32_t u32Month, uint32_t u32Day);
void RTC_SetAlarmTime(uint32_t u32Hour, uint32_t u32Minute, uint32_t u32Second, uint32_t u32TimeMode, uint32_t u32AmPm);
void RTC_SetAlarmDateMask(uint8_t u8IsTenYMsk, uint8_t u8IsYMsk, uint8_t u8IsTenMMsk, uint8_t u8IsMMsk, uint8_t u8IsTenDMsk, uint8_t u8IsDMsk);
void RTC_SetAlarmTimeMask(uint8_t u8IsTenHMsk, uint8_t u8IsHMsk, uint8_t u8IsTenMMsk, uint8_t u8IsMMsk, uint8_t u8IsTenSMsk, uint8_t u8IsSMsk);
uint32_t RTC_GetDayOfWeek(void);
void RTC_SetTickPeriod(uint32_t u32TickSelection);
void RTC_EnableInt(uint32_t u32IntFlagMask);
void RTC_DisableInt(uint32_t u32IntFlagMask);

/*@}*/ /* end of group RTC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group RTC_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif //__RTC_H__

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
