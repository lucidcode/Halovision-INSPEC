/**
  @file main.c
  @brief Free RTOS with D2XX
  An example of thread-safe D2XX library usage with FreeRTOS
*/
/*
 * ============================================================================
 * History
 * =======
 * 11 Sep 2016 : Created
 *
 * (C) Copyright,  Bridgetek Pte. Ltd.
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
 * appliance. There are exclusions of FTDI liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on FTDI's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, FTDI
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <ft900.h>
#include <ft900_startup_dfu.h>
#include <ftd2xx_api.h>

#include "FreeRTOS.h"
#include "task.h"
#include "dbg.h"		// for debug print

#define GPIO_REMOTE_WAKEUP

#ifdef GPIO_REMOTE_WAKEUP
#if defined(__FT930__)
//GPIO12 in FT930Q mini module board
#define GPIO_PIN  12
#else
#define GPIO_PIN  18
#endif
#endif

#define USER_TASK_STACK_SIZE	256	/**< 32-bit words */
#define SYS_TASK_STACK_SIZE	    256	/**< 32-bit words */
#define MAX_READBUFFER_SIZE  1024

#define TEST_WAKEUP_SOURCE_RESET 				(1 << 0)
#define TEST_WAKEUP_SOURCE_RESUME 				(1 << 1)
#define TEST_WAKEUP_SOURCE_GPIO 				(1 << 2)
#define TEST_WAKEUP_SOURCE_READY				(1 << 3)
#define TEST_WAKEUP_SOURCE_CONNECT				(1 << 4)
#define TEST_WAKEUP_SOURCE_DISCONNECT			(1 << 5)
#define TEST_WAKEUP_SOURCE_OTHERS				(1 << 6)
#define TEST_WAKEUP_SOURCE_SLAVE				(1 << 7)

#define DEVICE_INACTIVITY				10000 //10 second timeout
/*Minimum 10 seconds needed or would interfere in hibernate/wakeup process in bus powered mode */


/* GLOBAL VARIABLES ****************************************************************/

extern TD2XX_DeviceConfiguration __pD2XXDefaultConfiguration[]; // pointer to D2XX Config in flash
// The test config (in Flash and then PM) is copied to the RAM structure D2XXTEST_FlashConfig on startup
__attribute__ ((aligned (4))) TD2XX_DeviceConfiguration D2XXTEST_UserD2xxConfig = {0};

#ifdef DEBUG_EVENT
static char *D2XXTest_EventStrings[D2XX_EVT_MAX_CODE] = {	"SUSPEND",    	/**< SUSPEND EVENT from USB Host */
																	"RESUME",	 	/**< RESUME EVENT from USB Host */
																	"BUS_RESET",	 		/**< USB Bus Reset */
																	"READY",	 		/**< D2XX enters Ready state where READ/WRITE requests are processed*/
																	"UNREADY",	 	/**< D2XX exits Ready state*/
																	"DETACH",		/**< DFU DETACH Class command from DFU application */
																	"TESTMODE",	 	/**< D2XX enters Test Mode. Exit is via power cycle*/
																	"INTF_RESET"	/**< Interface RESET Vendor Command from D2XX Application */
																};
#endif
uint8_t D2XXTEST__DfuDetach = 0;
uint8_t D2XXTEST__Ready = 0;
volatile uint8_t SetRemoteWakeup = 0;
#if defined(__FT930__)
extern volatile uint8_t RemoteWakeupEnable;  // used in d2xx_api_master.c
#else
volatile uint8_t RemoteWakeupEnable = 0;
#endif
volatile uint8_t D2XXTEST__Suspend = 0;	/**< set when USB is in suspend */
volatile uint8_t D2XXTEST__Sleep = 0;	/**< set when system is entering sleep */
volatile uint8_t D2XXTEST__Wakeup = 0;	/**< set when system is wakeup from sleep - through a resume or local wake-up */
volatile uint8_t wkupSource = 0;		/**< set to identify wakeup source */
volatile uint8_t D2XXTEST__Testmode = 0;

#if 0
volatile int device_inactive_counter = 0;
volatile uint8_t check_device_status = 0;
#endif

TaskHandle_t	hTask_SystemTask;			/**< handle to D2XX System Task */
void 		vD2XX_SystemTask (void *unused);	/**< system task to handle DFU and SUSPEND/RESUME events */

TaskHandle_t	hTask_UserTask[D2XX_MAX_INTERFACES];		/**< handles to user tasks */
void 		vD2XX_UserTask (void *intf);		/**< user task */

int 		xD2XX_UserTaskParam[D2XX_MAX_INTERFACES];	/**< to hold parameters for user task */

TaskHandle_t	hTask_TimerTask;			/**< handle to D2XX Timer Task */
void 		vD2XX_1msTimerTask(void *unused);	/**< system task to handle USBD_Timer and D2XX_Timer calls */

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);
//void loop (void);
void debug_uart_init(void);
void tfp_putc(void* p, char c);
void d2xx_callback(ED2XX_EventCode  eventID, void *ref, void* param1, void* param2);
void power_mgmt_ISR(void);
#ifdef GPIO_REMOTE_WAKEUP
void gpio_ISR();
#endif

#if 0
uint16_t xorChecksum(const char str[], uint16_t length);
#endif

#undef BDFU  /* Undefine for C-DFU */

#ifdef BDFU
extern char __PMSIZE;
typedef void (*__dfumain_t)(unsigned int, unsigned int) __attribute__((noreturn));
#define GO_BDFU(VID, PID, REL) ((__dfumain_t)(&__PMSIZE - 12))( (PID) << 16 | (VID), REL);
#endif

/* FUNCTIONS ***********************************************************************/
int main(void)
{
	int i;

	/* Setup UART and D2XX */
    setup();

	// create the necessary user tasks
	// call d2xx init
	// start the task scheduler AFTER calling d2xx_init

	for (i = 0; i < D2XXTEST_UserD2xxConfig.ConfigDesc.NumOfD2XXInterfaces; i++)
	{
		xD2XX_UserTaskParam[i] = i+1;
		xTaskCreate(
			vD2XX_UserTask, 		/* Task */
			"D2XX Task",               	/* Name for the task */
			USER_TASK_STACK_SIZE,		/* Stack depth  */
			(void *) &xD2XX_UserTaskParam[i],			/* Task parameters */
			tskIDLE_PRIORITY,		/* Priority */
			&hTask_UserTask[i]		/* Task Handles for each interface */
			);
	}
	dbg ("D2XX tasks created\n");

	xTaskCreate(
		vD2XX_SystemTask, 		/* Task */
		"D2XX System Task",             /* Name for the task */
		SYS_TASK_STACK_SIZE,		/* Stack depth  */
		0,				/* Unused */
		tskIDLE_PRIORITY,		/* Priority */
		&hTask_SystemTask		/* Task Handle */
		);


	xTaskCreate(
			vD2XX_1msTimerTask, 		/* Task */
		"D2XX 1ms Timer Task",             /* Name for the task */
		SYS_TASK_STACK_SIZE,		/* Stack depth  */
		0,				/* Unused */
		tskIDLE_PRIORITY,		/* Priority */
		&hTask_TimerTask		/* Task Handle */
		);

	/**< vTaskStartScheduler should be called AFTER D2XX_Init and not before */
	dbg ("TaskScheduler started\n");

	vTaskStartScheduler();
	// function never returns 
	for (;;) ;
}
void setup(void)
{

	ED2XX_ErrorCode retVal;


	D2XXTEST__Suspend = 0;
	D2XXTEST__Sleep = 0;
	D2XXTEST__Wakeup = 0;

	#ifdef BDFU
	timer_prescaler(1000); /* to ward off linkage issue */
#endif
#if defined(__FT900__)
	interrupt_attach(interrupt_0, (int8_t)interrupt_0, power_mgmt_ISR);
#endif

	// UART initialisation
	debug_uart_init();

    /* Print out a welcome message... */
	dbg("(C) Copyright, Bridgetek Pte. Ltd. \r\n"
			"--------------------------------------------------------------------- \r\n"
			"Welcome to D2XX Example with FreeRTOS... \r\n"
			"\r\n"
			"Enter any text on the D2XX port, the same is echoed back on the same port... \r\n"
			"--------------------------------------------------------------------- \r\n"
	);

#if defined(__FT930__)
	retVal =
			D2XX_Init(&D2XXTEST_UserD2xxConfig, d2xx_callback, NULL);


    /*slave sub-system control register setup*/
    *(SLAVECPU) |= (MASK_SLAVE_CPU_CTRL_SLV_RESET);  // assert bit to keep slave CPU in reset
    *(SLAVECPU) |= (MASK_SLAVE_CPU_CTRL_D2XX_MODE);    // turn-on D2XX_mode
    *(SLAVECPU) &= ~(MASK_SLAVE_CPU_CTRL_SLV_RESET); // de-assert bit to allow slave CPU to start
#else

	retVal =

			D2XX_Init(__pD2XXDefaultConfiguration, d2xx_callback, NULL);
	memcpy_pm2dat(&D2XXTEST_UserD2xxConfig.ConfigDesc, (__flash__ void *)(uint32_t)&__pD2XXDefaultConfiguration->ConfigDesc, sizeof(TConfigDescriptors));
#endif

	interrupt_enable_globally(); //needed for interrupts
	//dbg("Configuration:- Signature: %08X, Actual Checksum: %04X, Expected Checksum : %04X\r\n", D2XXTEST_UserD2xxConfig.Signature,
	//		D2XXTEST_UserD2xxConfig.Checksum, xorChecksum((char *)&D2XXTEST_UserD2xxConfig, sizeof(TD2XX_DeviceConfiguration) - 2));
	dbg("D2XX_Init() returned: %d, No of Interfaces: %d\r\n", retVal, D2XXTEST_UserD2xxConfig.ConfigDesc.NumOfD2XXInterfaces);

#if 0
	if (retVal == D2XX_ERR_NONE)
	{
		check_device_status = 1;
	}
#endif
}

/** @name tfp_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
void tfp_putc(void* p, char c)
{
	uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}

/** @name d2xx_callback
 *  @details Callback function to the D2XX library.
 *  @param
 *  @param
 */
void d2xx_callback (ED2XX_EventCode ec, void *ref, void *param1, void *param2)
{
	/* 0 => Device, 1-3 => Interface*/
	/* or */
	/* 0 => Remote Wakeup disabled, 1 => Remote wakeup enabled */
	uint8_t param = 0;

	if (param1)
	{
		param = (*(uint8_t *)param1);
	}
#ifdef DEBUG_EVENT
	dbg("~%d",eventID);
	dbg("%s\n", D2XXTest_EventStrings[eventID]);
#endif // DEBUG_EVENTS
	switch (ec)
	{
	case D2XX_EVT_SUSPEND:
		if (!D2XXTEST__Sleep)
		{
			D2XXTEST__Suspend = 1;
			D2XXTEST__Sleep = 0;
			RemoteWakeupEnable = param;
#ifdef DEBUG_EVENT
			dbg("RemoteWakeup Enabled :%d \r\n", RemoteWakeupEnable);
#endif // DEBUG_EVENTS
		}
		break;
	case D2XX_EVT_RESUME:
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_RESUME;
			D2XXTEST__Wakeup = 1;
		}
		else
		{
			D2XXTEST__Suspend = 0;
		}
		break;
	case D2XX_EVT_BUS_RESET:
#if 0
		check_device_status = 0;
#endif
		break;
	case D2XX_EVT_READY: /*Connected */
		D2XXTEST__Ready = 1;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_READY;
			D2XXTEST__Wakeup = 1;
		}
		else
		{
			D2XXTEST__Suspend = 0;
		}
		break;
	case D2XX_EVT_UNREADY: /*Detached */
		D2XXTEST__Ready = 0;
		D2XXTEST__Suspend = 1;
		break;
	case D2XX_EVT_DFU_DETACH: /* DFU Detach from DFU application */
		D2XXTEST__DfuDetach = 1;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_OTHERS;
			D2XXTEST__Wakeup = 1;
		}
		else
		{
			D2XXTEST__Suspend = 0;
		}
		break;
	case D2XX_EVT_TESTMODE: /* Testmode during electrical tests */
		D2XXTEST__Testmode = 1;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_OTHERS;
			D2XXTEST__Wakeup = 1;
		}
		else
		{
			D2XXTEST__Suspend = 0;
		}
		break;
	default:
		break;
	}
}

/* Initializes the UART for the testing */
void debug_uart_init(void)
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

	// Open the UART using the coding required.
	uart_open(UART0,                    /* Device */
			1,                        /* Prescaler = 1 */
			UART_DIVIDER_19200_BAUD,  /* Divider = 1302 */
			uart_data_bits_8,         /* No. buffer Bits */
			uart_parity_none,         /* Parity */
			uart_stop_bits_1);        /* No. Stop Bits */

	/* Print out a welcome message... */
	uart_puts(UART0,
			"\x1B[2J" /* ANSI/VT100 - Clear the Screen */
			"\x1B[H"  /* ANSI/VT100 - Move Cursor to Home */
	);

#ifdef DEBUG
	/* Enable tfp_printf() functionality... */
	init_printf(UART0, tfp_putc);
#endif

}
#if 0
/* function to calculate the expected XOR checksum for the d2xx configuration */
uint16_t xorChecksum(const char str[], uint16_t length) {
    uint16_t sum = 0;
    uint16_t *s = (uint16_t *)str;

    length >>= 1;
    while (length) {
        sum ^= *s++;
        length--;
    }
    return sum;
}
#endif


/* Power management ISR */
void power_mgmt_ISR(void)
{

#if defined(__FT930__)
	if (SYS->PMCFG_H & MASK_SYS_PMCFG_SLAVE_PERI_IRQ_PEND)
	{
		// Clear d2xx hw engine wakeup
		SYS->PMCFG_H = MASK_SYS_PMCFG_SLAVE_PERI_IRQ_PEND;

		//D2XXTEST__Suspend = 0;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_SLAVE;
			D2XXTEST__Wakeup = 1;
		}
	}
#endif
	if (SYS->PMCFG_H & MASK_SYS_PMCFG_PM_GPIO_IRQ_PEND)
	{
		// Clear GPIO wakeup pending
		SYS->PMCFG_H = MASK_SYS_PMCFG_PM_GPIO_IRQ_PEND;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_GPIO;
			D2XXTEST__Wakeup = 1;
		}
	}
#if defined(__FT900__)
	if (SYS->PMCFG_H & MASK_SYS_PMCFG_DEV_CONN_DEV)
	{
		// Clear connection interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_CONN_DEV;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_CONNECT;
			D2XXTEST__Wakeup = 1;
		}

	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_DEV_DIS_DEV)
	{
		// Clear disconnection interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_DEV_DIS_DEV;
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_DISCONNECT;
			D2XXTEST__Wakeup = 1;
		}
	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_HOST_RST_DEV)
	{
		// Clear Host Reset interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_HOST_RST_DEV;
		USBD_resume();
		if (D2XXTEST__Sleep)
		{
			wkupSource |= TEST_WAKEUP_SOURCE_RESET;
			D2XXTEST__Wakeup = 1;
		}
	}

	if (SYS->PMCFG_H & MASK_SYS_PMCFG_HOST_RESUME_DEV)
	{
		// Clear Host Resume interrupt
		SYS->PMCFG_H = MASK_SYS_PMCFG_HOST_RESUME_DEV;
		if(! (SYS->MSC0CFG & MASK_SYS_MSC0CFG_DEV_RMWAKEUP))
		{
			// If we are driving K-state on Device USB port;
			// We must maintain the 1ms requirement before resuming the phy
			USBD_resume();
		}
	}
#endif
}

void vD2XX_SystemTask (void *unused)
{
	int i;

	dbg ("vD2XX_System Task started\n");

	while (1)
	{
#if defined(__FT900__)
    	D2XX_Process();
#endif
		/* Event handling */
		if (D2XXTEST__DfuDetach)
		{
			dbg ("vD2XX_SystemTask: Entering DFU Mode \n");

			taskENTER_CRITICAL ();		/**< we will not leave the CRITICAL SECTION */
			vTaskSuspendAll ();		/**< Suspend all tasks - they will be deleted */

			/* DFU detach request has come. Exit D2XX */
			for (i = 0; i < D2XXTEST_UserD2xxConfig.ConfigDesc.NumOfD2XXInterfaces; i++)
				vTaskDelete (hTask_UserTask[i]);

			D2XX_Exit();

			/* Take the system to DFU Mode. Provide timeout in ms to
			 * wait for re-enumeration with the host
			 * */
			taskEXIT_CRITICAL ();
			dbg ("Switching to DFU");
			taskENTER_CRITICAL ();		/**< we will not leave the CRITICAL SECTION */
#if defined(__FT930__)
		    *(SLAVECPU) &= ~(MASK_SLAVE_CPU_CTRL_D2XX_MODE);    // turn-off D2XX_mode
		    *(SLAVECPU) |= (MASK_SLAVE_CPU_CTRL_SLV_RESET);  // assert bit to keep slave CPU in reset
#endif
			STARTUP_DFU(0);
			return;		/**< does not return */ 
		} 
		
		/* Suspend mode handling */ 
		if (D2XXTEST__Suspend)
		{
			dbg ("vD2XX_SystemTask: Entering Suspend Mode\n");

			taskENTER_CRITICAL();
			{
				wkupSource = 0;
#ifdef GPIO_REMOTE_WAKEUP
				SetRemoteWakeup = 0;
				if (RemoteWakeupEnable)
				{
					dbg("vD2XX_SystemTask: GPIO pin setup for interrupt\n");
					/* Set up the pin */
					gpio_dir(GPIO_PIN, pad_dir_input);
					gpio_pull(GPIO_PIN, pad_pull_pullup);

					/* Attach an interrupt */
					interrupt_attach(interrupt_gpio, (uint8_t)interrupt_gpio, gpio_ISR);
					gpio_interrupt_enable(GPIO_PIN, gpio_int_edge_falling);
				}
#endif
				D2XXTEST__Sleep = 1;
				D2XXTEST__Wakeup = 0;
			}
			taskEXIT_CRITICAL();

			/* suspend all user tasks */
			for(int i = 0; i < D2XX_MAX_INTERFACES; i++)
			      vTaskSuspend(hTask_UserTask[i]);


			/* Power down Mode */
			SYS->PMCFG_L |= MASK_SYS_PMCFG_PM_PWRDN_MODE;
			SYS->PMCFG_L |= MASK_SYS_PMCFG_PM_PWRDN;
			asm volatile ("nop");
			asm volatile ("nop");
			asm volatile ("nop");
			asm volatile ("nop");
			asm volatile ("nop");

			//wait for wakeup
			while(!D2XXTEST__Wakeup);

			taskENTER_CRITICAL ();
			{
				SYS->PMCFG_L &= ~MASK_SYS_PMCFG_PM_PWRDN_MODE;
				SYS->PMCFG_L &= ~MASK_SYS_PMCFG_PM_PWRDN;
#ifdef GPIO_REMOTE_WAKEUP
				if (RemoteWakeupEnable)
				{
					gpio_interrupt_disable(GPIO_PIN);
					interrupt_detach(interrupt_gpio);
					gpio_is_interrupted(GPIO_PIN);
				}
#endif
				D2XXTEST__Sleep = 0;
				D2XXTEST__Suspend = 0;
				D2XXTEST__Wakeup = 0;
			}
			taskEXIT_CRITICAL ();

			for(int i = 0; i < D2XX_MAX_INTERFACES; i++)
			      vTaskResume(hTask_UserTask[i]);

			dbg("vD2XX_SystemTask: Exiting Suspend Mode at Master: %02X \r\n", wkupSource);

			if (SetRemoteWakeup)
			{
				D2XX_IOCTL(0, D2XX_IOCTL_SYS_REMOTE_WAKEUP, (void *)&SetRemoteWakeup, NULL);
				SetRemoteWakeup = 0;
				dbg("vD2XX_SystemTask: Sending remote wakeup from Application \r\n");
			}
		}
	}
}


#ifdef GPIO_REMOTE_WAKEUP
void gpio_ISR()
{
    if (gpio_is_interrupted(GPIO_PIN))
    {
		if (D2XXTEST__Sleep)
		{
			//Need to perform remote wakeup procedure
			D2XXTEST__Wakeup = 1;
			SetRemoteWakeup = 1;
		}
    }
}
#endif

void vD2XX_1msTimerTask (void *unused)
{
	taskENTER_CRITICAL ();
	tfp_printf ("vD2XX_TimerTask: started\n");
	taskEXIT_CRITICAL ();

	for (;;)
	{
		D2XX_Timer();
#if 0
		if (check_device_status)
		{
			device_inactive_counter++;

			if (device_inactive_counter >= DEVICE_INACTIVITY)
			{
				dbg("Device inactive !!!\n");
				USBD_suspend_device();
				device_inactive_counter = 0;
				check_device_status = 0;
			}
		}
#endif
		vTaskDelay (pdMS_TO_TICKS(1));	// delay 1 ms
	}
}
