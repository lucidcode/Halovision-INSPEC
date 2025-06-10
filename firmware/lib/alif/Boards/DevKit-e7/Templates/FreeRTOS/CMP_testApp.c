/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     CMP_testapp.c
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     18-June-2022
 * @brief    TestApp to verify analog Comparator interface using FreeRTOS as
 *           an operating system..
 *              - CMP0 instance is used - in RTE_Device.h we set the input muxes
 *              - Input A is set by RTE_CMP0_SEL_POSITIVE
 *              - Input A is set to RTE_CMP0_POSITIVE_PIN_PO_00 (analog pin P0_0)
 *              - Input B is set by RTE_CMP0_SEL_NEGATIVE
 *              - Input B is set to RTE_CMP_NEGATIVE_DAC6(which provide 0.9v)
 *              Hardware setup (1 wires needed):
 *              - Connect P0_0(+ve pin) to P12_3(GPIO output) and DAC6 is set as negative
 *                pin,check CMP0 output in the pin P14_7 using saleae logic analyzer.
 *              - If +ve input is greater than -ve input, interrupt will be generated,
 *                and the output will be high.
 *              - If -ve input is greater than +ve input, interrupt will be generated,
 *                and the output will be low.
 *              For window control feature:
 *              - As glb_events/utimer events are active for few clocks, set prescalar
 *                value to 0. These interrupt will occur continuously because Utimer
 *                is running continuously.
 ******************************************************************************/

/* System Includes */
#include "Driver_GPIO.h"
#include <stdio.h>
#include "system_utils.h"
#include "pinconf.h"
#include "Driver_UTIMER.h"

/* include for Comparator Driver */
#include "Driver_CMP.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "RTE_Components.h"
#if defined(RTE_Compiler_IO_STDOUT)
#include "retarget_stdout.h"
#endif  /* RTE_Compiler_IO_STDOUT */

/* LED configurations */
#define GPIO12_PORT                     12     /* Use LED0_R,LED0_B GPIO port */
#define LED0_R                          PIN_3  /* LED0_R gpio pin             */

/* To read the HSCMP0 output status set CMP_OUTPIN as 7, for HSCMP1 set CMP_OUTPIN as 6,
 * for set CMP_OUTPIN as HSCMP2 5, and for HSCMP3 set CMP_OUTPIN as 4 */
#define CMP14_PORT                      14
#define CMP_OUTPIN                      7

#define NUM_TAPS                        5      /* Number of filter taps     */

#define LPCMP                0
#define HSCMP                1

/* To configure for HSCMP, use CMP_INSTANCE HSCMP */
/* To configure for LPCMP, use CMP_INSTANCE LPCMP */
#define CMP_INSTANCE         HSCMP

/* To enable comparator window control, change the macro value from 0 to 1
 * The glb_events/utimer events define the window where to look at the cmp_input.
 * As GLB_events/Utimer_events are active for few clocks, there is no reason to set
 * prescaler value, so set Prescaler value to 0 when using window control.
 * As Utimer is running continuously, the HSCMP interrupts will occur continuously. */
#define CMP_WINDOW_CONTROL   0

#if CMP_WINDOW_CONTROL
#define SAMPLING_RATE        0  /* Set the prescaler values as 0 for windowing function */
#else
#define SAMPLING_RATE        8  /* Set the prescaler values from 0 to 31 */
#endif

#define ERROR    -1
#define SUCCESS   0

extern  ARM_DRIVER_GPIO ARM_Driver_GPIO_(GPIO12_PORT);
ARM_DRIVER_GPIO *ledDrv = &ARM_Driver_GPIO_(GPIO12_PORT);

extern  ARM_DRIVER_GPIO ARM_Driver_GPIO_(CMP14_PORT);
ARM_DRIVER_GPIO *CMPout = &ARM_Driver_GPIO_(CMP14_PORT);

#if(CMP_INSTANCE == LPCMP)
#if !defined(M55_HE)
#error "This Demo application works only on RTSS_HE"
#endif
extern ARM_DRIVER_CMP Driver_LPCMP;
static ARM_DRIVER_CMP *CMPdrv = &Driver_LPCMP;
#else
extern ARM_DRIVER_CMP Driver_CMP0;
static ARM_DRIVER_CMP *CMPdrv = &Driver_CMP0;
#endif

volatile uint32_t call_back_counter = 0;
uint32_t value =0;

#define CMP_CALLBACK_EVENT_SUCCESS  1

/*Define for FreeRTOS*/
#define TIMER_SERVICE_TASK_STACK_SIZE   configTIMER_TASK_STACK_DEPTH
#define IDLE_TASK_STACK_SIZE            configMINIMAL_STACK_SIZE

StackType_t IdleStack[2 * IDLE_TASK_STACK_SIZE];
StaticTask_t IdleTcb;
StackType_t TimerStack[2 * TIMER_SERVICE_TASK_STACK_SIZE];
StaticTask_t TimerTcb;

TaskHandle_t cmp_xHandle;

/****************************** FreeRTOS functions **********************/

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
      StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
   *ppxIdleTaskTCBBuffer = &IdleTcb;
   *ppxIdleTaskStackBuffer = IdleStack;
   *pulIdleTaskStackSize = IDLE_TASK_STACK_SIZE;
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
   (void) pxTask;

   for (;;);
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
      StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
   *ppxTimerTaskTCBBuffer = &TimerTcb;
   *ppxTimerTaskStackBuffer = TimerStack;
   *pulTimerTaskStackSize = TIMER_SERVICE_TASK_STACK_SIZE;
}

void vApplicationIdleHook(void)
{
   for (;;);
}

/* Use window control(External trigger using UTIMER or QEC) to trigger the comparator comparison */
#if(CMP_INSTANCE == HSCMP)
#if CMP_WINDOW_CONTROL

/* UTIMER0 Driver instance */
extern ARM_DRIVER_UTIMER DRIVER_UTIMER0;
ARM_DRIVER_UTIMER *ptrUTIMER = &DRIVER_UTIMER0;

TaskHandle_t  utimer_compare_xHandle;

#define UTIMER_TASK_START               0X04
#define UTIMER_COMPARE_A_CB_EVENT      (1U << 2U)

/**
 * @function    void utimer_compare_mode_cb_func(uint8_t event)
 * @brief       utimer compare mode callback function
 * @note        none
 * @param       event
 * @retval      none
 */
static void utimer_compare_mode_cb_func(uint8_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdFALSE;
    if(event == ARM_UTIMER_EVENT_COMPARE_A) {
        xResult = xTaskNotifyFromISR(utimer_compare_xHandle, UTIMER_COMPARE_A_CB_EVENT,
                                     eSetBits, &xHigherPriorityTaskWoken);
        if(xResult == pdTRUE) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

/**
 * @function    void utimer_compare_mode_app(void)
 * @brief       utimer compare mode application
 * @note        none
 * @param       none
 * @retval      none
 */
static void utimer_compare_mode_app(void *pvParameters)
{
    int32_t slRet;
    uint8_t ucChannel = 0;
    uint32_t ulCount_array[3];
    BaseType_t xReturned;

    pinconf_set(0, 0, PINMUX_ALTERNATE_FUNCTION_4, 0);

    /*
     * utimer channel 0 is configured for utimer compare mode (driver A).
     * observe driver A output signal from P1_2.
     */
    /*
     * System CLOCK frequency (F)= 400Mhz
     *
     * Time for 1 count T = 1/F = 1/(400*10^6) = 0.0025 * 10^-6
     *
     * To Increment or Decrement Timer by 1 count, takes 0.0025 micro sec
     *
     * So count for 1 sec = 1/(0.0025*(10^-6)) = 400000000
     * DEC = 400000000
     * HEX = 0x17D78400
     *
     * So count for 500ms = (500*(10^-3)/(0.0025*(10^-6)) = 200000000
     * DEC = 200000000
     * HEX = 0xBEBC200
     */
    ulCount_array[0] =  0x000000000;       /*< initial counter value >*/
    ulCount_array[1] =  0x17D78400;        /*< over flow count value > */
    ulCount_array[2] =  0xBEBC200;         /*< compare a/b value> */

    xReturned = xTaskNotifyWait(NULL,UTIMER_TASK_START,
                                NULL, portMAX_DELAY);
    if(xReturned != pdTRUE )
    {
        printf("\n\r Task Wait Time out expired \n\r");
        while(1);
    }

    slRet = ptrUTIMER->Initialize (ucChannel, utimer_compare_mode_cb_func);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d failed initialize \n", ucChannel);
        return;
    }

    slRet = ptrUTIMER->PowerControl (ucChannel, ARM_POWER_FULL);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d failed power up \n", ucChannel);
        goto error_compare_mode_uninstall;
    }

    slRet = ptrUTIMER->ConfigCounter (ucChannel, ARM_UTIMER_MODE_COMPARING, ARM_UTIMER_COUNTER_UP);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d mode configuration failed \n", ucChannel);
        goto error_compare_mode_poweroff;
    }

    slRet = ptrUTIMER->SetCount (ucChannel, ARM_UTIMER_CNTR, ulCount_array[0]);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d set count failed \n", ucChannel);
        goto error_compare_mode_poweroff;
    }

    slRet = ptrUTIMER->SetCount (ucChannel, ARM_UTIMER_CNTR_PTR, ulCount_array[1]);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d set count failed \n", ucChannel);
        goto error_compare_mode_poweroff;
    }

    slRet = ptrUTIMER->SetCount (ucChannel, ARM_UTIMER_COMPARE_A, ulCount_array[2]);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d set count failed \n", ucChannel);
        goto error_compare_mode_poweroff;
    }

    slRet = ptrUTIMER->Start(ucChannel);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d failed to start \n", ucChannel);
        goto error_compare_mode_poweroff;
    }

    while(1)
    {
        /* Waiting for the callback */
        xReturned = xTaskNotifyWait(NULL,UTIMER_COMPARE_A_CB_EVENT,
                                    NULL,  portMAX_DELAY);
        if(xReturned != pdTRUE )
        {
            printf("\n\r Task Wait Time out expired \n\r");
            goto error_compare_mode_poweroff;
        }
    }
    return;

    slRet = ptrUTIMER->Stop (ucChannel, ARM_UTIMER_COUNTER_CLEAR);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d failed to stop \n", ucChannel);
    }

error_compare_mode_poweroff:

    slRet = ptrUTIMER->PowerControl (ucChannel, ARM_POWER_OFF);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d failed power off \n", ucChannel);
    }

error_compare_mode_uninstall:

    slRet = ptrUTIMER->Uninitialize (ucChannel);
    if(slRet != ARM_DRIVER_OK) {
        printf("utimer channel %d failed to un-initialize \n", ucChannel);
    }
}

#endif
#endif

/**
 * @fn          void cmp_pinmux_config(void)
 * @brief       Initialize the pinmux for CMP output
 * @return      status
*/
int32_t cmp_pinmux_config(void)
{
    int32_t ucStatus;

    /* Configure HSCMP0 output */
    ucStatus = pinconf_set(PORT_14, PIN_7, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_READ_ENABLE);
    if(ucStatus)
        return ERROR;

    /* Configure HSCMP1 output */
    ucStatus = pinconf_set(PORT_14, PIN_6, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_READ_ENABLE);
    if(ucStatus)
        return ERROR;

    /* Configure HSCMP2 output */
    ucStatus = pinconf_set(PORT_14, PIN_5, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_READ_ENABLE);
    if(ucStatus)
        return ERROR;

    /* Configure HSCMP3 output */
    ucStatus = pinconf_set(PORT_14, PIN_4, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_READ_ENABLE);
    if(ucStatus)
        return ERROR;

    return SUCCESS;
}

/**
 * @fn        led_init(void)
 * @brief     - Initialize the LED0_R
 *            - Enable the power for LED0_R
 *            - Set direction for LED0_R
 *            - Set value for LED0_R
 * @param[in]  None
 * return      status
 */
static int32_t led_init(void)
{
    int32_t slRet = 0;

    /* gpio12 pin3 can be used as Red LED of LED0 */
    pinconf_set(GPIO12_PORT, LED0_R, PINMUX_ALTERNATE_FUNCTION_0, 0);

    /* Initialize the LED0_R */
    slRet = ledDrv->Initialize(LED0_R, NULL);
    if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to initialize\n");
        return ERROR;
    }

    slRet = CMPout->Initialize(CMP_OUTPIN, NULL);
    if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to initialize\n");
        return ERROR;
    }

    /* Enable the power for LED0_R */
    slRet = ledDrv->PowerControl(LED0_R, ARM_POWER_FULL);
    if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to powered full\n");
        goto error_uninitialize_LED;
    }

    /* Enable the power for LED0_R */
    slRet = CMPout->PowerControl(CMP_OUTPIN, ARM_POWER_FULL);
    if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to powered full\n");
        goto error_uninitialize_LED;
    }

   slRet = ledDrv->SetDirection(LED0_R, GPIO_PIN_DIRECTION_OUTPUT);
   if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to configure\n");
        goto error_power_off_LED;
    }

   slRet = CMPout->SetDirection(CMP_OUTPIN, GPIO_PIN_DIRECTION_OUTPUT);
   if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to configure\n");
        goto error_power_off_LED;
    }

    slRet = ledDrv->SetValue(LED0_R, GPIO_PIN_OUTPUT_STATE_HIGH);
    if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to configure\n");
        goto error_power_off_LED;
    }

    return SUCCESS;

error_power_off_LED:
    /* Power-off the LED0_R */
    slRet = ledDrv->PowerControl(LED0_R, ARM_POWER_OFF);
    if(slRet != ARM_DRIVER_OK)  {
        printf("ERROR: Failed to power off \n");
    }

error_uninitialize_LED:
    /* Uninitialize the LED0_R */
    slRet = ledDrv->Uninitialize(LED0_R);
    if(slRet != ARM_DRIVER_OK)  {
        printf("Failed to Un-initialize \n");
    }
    return ERROR;
}

/**
 * @fn        cmp_get_status(void)
 * @brief     - Get the Status of CMP output pin.
 * @param[in]  None
 * return      status
 */
static int32_t cmp_get_status(void)
{
    int32_t slRet = 0;

    slRet = CMPout->GetValue(CMP_OUTPIN, &value);
    if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to toggle LEDs\n");
        goto error_power_off_LED;
    }
    return value;

    error_power_off_LED:
    /* Power-off the CMP_OUTPIN */
    slRet = CMPout->PowerControl(CMP_OUTPIN, ARM_POWER_OFF);
    if(slRet != ARM_DRIVER_OK)  {
        printf("ERROR: Failed to power off \n");
    }

    /* Uninitialize the CMP_OUTPIN */
    slRet = CMPout->Uninitialize(CMP_OUTPIN);
    if(slRet != ARM_DRIVER_OK)  {
        printf("Failed to Un-initialize \n");
    }
    return ERROR;
}

/**
 * @fn         led_toggle(void)
 * @brief      - set LED0_R for toggle
 * @param[in]  None
 * return      status
 */
int32_t led_toggle(void)
{
    int32_t slRet = 0;

    slRet = ledDrv->SetValue(LED0_R, GPIO_PIN_OUTPUT_STATE_TOGGLE);
    if(slRet != ARM_DRIVER_OK) {
        printf("ERROR: Failed to toggle LEDs\n");
        goto error_power_off_LED;
    }
    return SUCCESS;

error_power_off_LED:
    /* Power-off the LED0_R */
    slRet = ledDrv->PowerControl(LED0_R, ARM_POWER_OFF);
    if(slRet != ARM_DRIVER_OK)  {
        printf("ERROR: Failed to power off \n");
    }

    /* Uninitialize the LED0_R */
    slRet = ledDrv->Uninitialize(LED0_R);
    if(slRet != ARM_DRIVER_OK)  {
        printf("Failed to Un-initialize \n");
    }
    return ERROR;
}

/**
 * @fn       void CMP_filter_callback(uint32_t event)
 * @brief    - This code expects the LED Blinky application to be running
 *             and the pin P12_3 should toggle every 1 second.
 *           - The comparator compares the voltage of P12_3 which is connected
 *             to positive comparator input which is compared to the 0.9v DAC6.
 *           - When the comparator input changes from HIGH to LOW or from LOW to HIGH,
 *             interrupt will be generated and it will set the call back event
 *           - According to the interrupt generation the call_back_counter will be incremented.
 * @return   None
 */
static void CMP_filter_callback(uint32_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, notificationResult = pdFALSE;

    if(event & ARM_CMP_FILTER_EVENT_OCCURRED)
    {
        /* Received Comparator filter event */
        notificationResult = xTaskNotifyFromISR(cmp_xHandle, CMP_CALLBACK_EVENT_SUCCESS,
                                                eSetBits, &xHigherPriorityTaskWoken);

        if(notificationResult == pdTRUE)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    call_back_counter++;
}

static void CMP_demo_Thread_entry(void *pvParameters)
{
    int32_t slRet = 0;
    uint8_t ucLoop_count = 10;
    BaseType_t xReturned;
    uint8_t ucStatus = 0;
    ARM_DRIVER_VERSION version;
    const TickType_t xDelay = (1000/portTICK_PERIOD_MS);

    printf("\r\n >>> Comparator demo FreeRTOS starting up!!! <<< \r\n");

    /* Configure the CMP output pins */
    if(cmp_pinmux_config())
    {
        printf("CMP pinmux failed\n");
    }

    version = CMPdrv->GetVersion();
    printf("\r\n Comparator version api:%X driver:%X...\r\n", version.api, version.drv);

    /* Initialize the configurations for LED0_R */
    if(led_init())
    {
        printf("Error: LED initialization failed\n");
        return;
    }

    /* Initialize the Comparator driver */
    slRet = CMPdrv->Initialize(CMP_filter_callback);
    if(slRet != ARM_DRIVER_OK){
        printf("\r\n Error: Comparator init failed\n");
        return;
    }

    /* Enable the power for Comparator */
    slRet = CMPdrv->PowerControl(ARM_POWER_FULL);
    if(slRet != ARM_DRIVER_OK){
        printf("\r\n Error: Comparator Power up failed\n");
        goto error_uninitialize;
    }

#if(CMP_INSTANCE == HSCMP)

#if CMP_WINDOW_CONTROL
    /* Start CMP using window control */
    slRet = CMPdrv->Control(ARM_CMP_WINDOW_CONTROL_ENABLE, ARM_CMP_WINDOW_CONTROL_SRC_0);
    if(slRet != ARM_DRIVER_OK) {
        printf("\r\n Error: CMP External trigger enable failed\n");
        goto error_poweroff;
    }
#endif

    /* Filter function for analog comparator*/
    slRet = CMPdrv->Control(ARM_CMP_FILTER_CONTROL, NUM_TAPS);
    if(slRet != ARM_DRIVER_OK){
        printf("\r\n Error: Comparator Filter control failed\n");
        goto error_poweroff;
    }

    /* Prescaler function for the comparator */
    slRet = CMPdrv->Control(ARM_CMP_PRESCALER_CONTROL, SAMPLING_RATE);
    if(slRet != ARM_DRIVER_OK){
        printf("\r\n Error: Comparator Prescaler control failed\n");
        goto error_poweroff;
    }
#endif

    /* To Start the Comparator module */
    slRet = CMPdrv->Start();
    if(slRet != ARM_DRIVER_OK){
        printf("\r\n Error: Comparator Start failed\n");
        goto error_poweroff;
    }

#if(CMP_INSTANCE == HSCMP)
#if CMP_WINDOW_CONTROL
    /* Set flag to wake up utimer task */
    xReturned = xTaskNotify(utimer_compare_xHandle,UTIMER_TASK_START,
                            eSetBits);
    if(xReturned == pdTRUE)
    {
        portYIELD();
    }
    else
    {
        printf("Error in invoking Utimer Task \r\n");
    }
#endif
#endif

    while(ucLoop_count --)
    {
        /* Toggle the LED0_R */
        if(led_toggle())
        {
            printf("ERROR: Failed to toggle LEDs\n");
            goto error_poweroff;
        }

        /* wait for CMP callback */
        xReturned = xTaskNotifyWait(NULL, CMP_CALLBACK_EVENT_SUCCESS, NULL, portMAX_DELAY);
        if (xReturned != pdTRUE) {
            printf("Error: CMP tx_event_flags_get\n");
            goto error_poweroff;
        }

        vTaskDelay(xDelay);

        /* Check the status of the CMP output pin */
        ucStatus = cmp_get_status();

        /* If user give +ve input voltage more than -ve input voltage, status will be set to 1*/
        if(ucStatus == 1)
        {
            printf("\n CMP positive input voltage is greater than negative input voltage\n");
        }
        /* If user give -ve input voltage more than +ve input voltage, status will be set to 0*/
        else if(ucStatus == 0)
        {
            printf("\n CMP negative input voltage is greater than the positive input voltage\n");
        }
        else
        {
            printf("ERROR: Status detection is failed\n");
            goto error_poweroff;
        }
    }

#if(CMP_INSTANCE == HSCMP)
#if CMP_WINDOW_CONTROL
    /* Disable CMP window control */
    slRet = CMPdrv->Control(ARM_CMP_WINDOW_CONTROL_DISABLE, ARM_CMP_WINDOW_CONTROL_SRC_0);
    if(slRet != ARM_DRIVER_OK) {
        printf("\r\n Error: CMP External trigger enable failed\n");
        goto error_poweroff;
    }
#endif
#endif

    /* To Stop the comparator module */
    slRet = CMPdrv->Stop();
    if(slRet != ARM_DRIVER_OK){
        printf("\r\n Error: Comparator Stop failed\n");
        goto error_poweroff;
    }

    printf("\n Comparator Filter event completed and the call_back_counter value is %d\n",call_back_counter );

error_poweroff:
    /* Power off Comparator peripheral */
    slRet = CMPdrv->PowerControl(ARM_POWER_OFF);
    if(slRet != ARM_DRIVER_OK){
        printf("\r\n Error: Comparator Power OFF failed.\r\n");
    }

error_uninitialize:
    /* UnInitialize Comparator driver */
    slRet = CMPdrv->Uninitialize();
    if(slRet != ARM_DRIVER_OK){
        printf("\r\n Error: Comparator Uninitialize failed.\r\n");
    }
}

int main()
{
    #if defined(RTE_Compiler_IO_STDOUT_User)
    int32_t ret;
    ret = stdout_init();
    if(ret != ARM_DRIVER_OK)
    {
        while(1)
        {
        }
    }
    #endif

    /* System Initialization */
    SystemCoreClockUpdate();

    /* Create application main thread */
    BaseType_t xReturned = xTaskCreate(CMP_demo_Thread_entry, "CMPFreertos", 256,
                                       NULL, configMAX_PRIORITIES-1, &cmp_xHandle);
    if(xReturned != pdPASS)
    {
       vTaskDelete(cmp_xHandle);
       return -1;
    }

#if(CMP_INSTANCE == HSCMP)
#if CMP_WINDOW_CONTROL
    xReturned = xTaskCreate(utimer_compare_mode_app, "utimer_compare_mode_demo", 256,
                            NULL, configMAX_PRIORITIES-2, &utimer_compare_xHandle);
    if(xReturned != pdPASS)
    {
        vTaskDelete(utimer_compare_xHandle);
        return -1;
    }
#endif
#endif

    /* Start thread execution */
    vTaskStartScheduler();
}
