/**
  @file user.c
  @brief Free RTOS with D2XX - User Tasks go here
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
 * This source code ("the Software") is provided by Bridgetek Pte. Ltd.
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

#include <stdint.h>
#include <ft900.h>
#include <ftd2xx_api.h>

#include "FreeRTOS.h"
#include "task.h"
#include "dbg.h"		// for debug print

extern volatile int D2XXTEST__Ready;

/**
 * @brief 	Example User Task to communicate with USB Host
 * 		The task performs data loopback by reading from and writing to the same channel
 * @params	pointer to int that is the interface number to be used for communication with USB Host application
 * @returns 	None
 */
void vD2XX_UserTask (void *param)
{
	uint8_t b[512];			/* USB data buffer */

	int32_t	len = 0;
	int32_t	part = 0;
	int32_t	sum = 0;
	int32_t	cum = 0;
	static int once = 0;		/**< For initial delay */
	int	intf = *(int *)param;	/**< D2XX Interface Number */

	taskENTER_CRITICAL ();
	tfp_printf ("vD2XX_UserTask: (intf=%d) started\n", intf);
	taskEXIT_CRITICAL ();

	for (;;)
	{
		while (!D2XXTEST__Ready)
		{
			vTaskDelay (pdMS_TO_TICKS(1000));	// delay 1 second
		}

		if (!once)	
		{
			// give enough time for enumeration to complete
			once = 1;
			vTaskDelay (pdMS_TO_TICKS(5000));	// delay 5 second
		}


		len = D2XX_Read (intf, b, sizeof (b));

		if (!len)
		{
			//vTaskDelay (pdMS_TO_TICKS(20));		// delay 20ms
			continue;
		}

		if (len < 0)
		{
			dbg ("vD2XX_UserTask: D2XX_Read(%d) failed, error=%ld\n", intf, len);
			goto error;
		}

		cum += len;
		do {
			part = D2XX_Write (intf, b+sum, len);
			if (part >= 0)
			{
				len -= part;
				sum += part;
			}
			else 
			{
				dbg ("vD2XX_UserTask: D2XX_Write(%d) failed, error=%ld\n", intf, part);
				goto error;
	 		}
		} while (len > 0);

		len = 0;
		part = 0;
		sum = 0;

#define KB	(1UL << 10)

		if (cum && !(cum & (256*KB-1)))
		{
			dbg ("D2XX_UserTask(%d): looped back %ld bytes\n", intf, cum);
		}
	}

error:
	for (;;)
		;
}

/* end */
