/**
  @file 
  @brief 
  USBH File System Example 1
  
  Read and write some files using FatFS
*/
/*
 * ============================================================================
 * History
 * =======
 * DB 14 Nov 2014 : Created
 *
 * Copyright (C) Bridgetek Pte Ltd
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

#include <stdint.h>
#include <string.h>

#include <ft900.h>
#include <ft900_usb.h>
#include <ft900_usbh_boms.h>
#include <ft900_startup_dfu.h>

/* UART support for printf output. */
#include "tinyprintf.h"

/* Support for FATFS */
#include "ff.h"
#include "diskio.h"

/* CONSTANTS ***********************************************************************/

#define EXAMPLE_FILE "LOREM.TXT"

/* GLOBAL VARIABLES ****************************************************************/

// File system handle for fatfs.
FATFS fs;
// Context structure and handle for BOMS device.
USBH_BOMS_context bomsCtx;
USBH_interface_handle hOpenDisk = 0;

/* LOCAL VARIABLES *****************************************************************/

static const char* LOREM_IPSUM =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi dictum mi "
    "eget malesuada auctor. Cras tellus ligula, feugiat ac ante eu, tincidunt "
    "consectetur mauris. Phasellus in mollis enim, dapibus venenatis est. Sed "
    "urna tellus, varius a dui sed, scelerisque commodo lectus. In pretium "
    "lobortis tortor, semper ultricies odio viverra a. Ut sit amet aliquam "
    "lectus. Phasellus non risus a nisl semper vehicula a vitae lorem. Fusce "
    "suscipit, purus nec facilisis lacinia, lacus massa aliquet augue, in "
    "feugiat neque nibh a lacus. Curabitur pharetra viverra massa quis "
    "efficitur. \r\n"
    "\r\n"
    "Mauris posuere nisl vel aliquam finibus. Aenean ac fringilla justo. Nulla "
    "eu sollicitudin erat. Duis in ligula at quam pretium hendrerit. Fusce quis "
    "egestas metus. In hac habitasse platea dictumst. Fusce tincidunt enim at "
    "tempus ullamcorper. Aenean pellentesque condimentum sapien vel porta. In "
    "sollicitudin tempor pulvinar. Pellentesque aliquet justo lacus, scelerisque "
    "feugiat augue commodo viverra. \r\n"
    "\r\n"
    "Etiam pulvinar quam a pulvinar aliquam. Cras rutrum quis tortor ut ultrices. "
    "Curabitur sit amet odio eros. Mauris auctor erat non risus interdum, at "
    "venenatis urna interdum. Nam eget auctor risus, auctor fringilla leo. "
    "Quisque sit amet ligula mattis, gravida tortor quis, ullamcorper odio. "
    "Nullam semper mauris at leo aliquam, quis mollis tortor iaculis. Mauris ut "
    "tempor elit, sed sodales magna. Donec non eros tortor. Donec lorem justo, "
    "vestibulum vitae sagittis ac, bibendum vitae velit. Integer ante mi, tempus "
    "sodales consectetur vel, porta ac libero. Maecenas dapibus orci at rhoncus "
    "bibendum. Nulla elementum lectus massa, non varius lorem scelerisque sit "
    "amet. \r\n";

/* MACROS **************************************************************************/

#define FDATE_DAY(fdate) ((fdate)&(0x1F))
#define FDATE_MONTH(fdate) (((fdate)>>5)&(0x0F))
#define FDATE_YEAR(fdate) ( (((fdate)>>9)&(0x7F)) + 1980 )

#define FTIME_HOUR(ftime) (((ftime)>>11)&(0x1F))
#define FTIME_MIN(ftime) (((ftime)>>5)&(0x3F))

/* LOCAL FUNCTIONS / INLINES *******************************************************/

FRESULT dir(char* path);
DSTATUS disk_initialize(BYTE pdrv);
DSTATUS disk_status(BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
DWORD get_fattime(void);

/** @name tfp_putc
 *  @details Machine dependent putc function for tfp_printf (tinyprintf) library.
 *  @param p Parameters (machine dependent)
 *  @param c The character to write
 */
void tfp_putc(void* p, char c)
{
    uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}

void ISR_timer(void)
{
    if (timer_is_interrupted(timer_select_a))
    {
    	// Call USB timer handler to action transaction and hub timers.
   		USBH_timer();
    }
}


/* Based on: http://elm-chan.org/fsw/ff/en/readdir.html */
/** List the directory 
 *  @param path The directory to list
 *  @return The status of the operation */
FRESULT dir(char* path)
{
    FRESULT res;
    FILINFO info;
    DIR dir;
    char * fname;
    uint8_t day, month, hour, min;
    uint16_t year;
    uint32_t totalsize = 0, totalfiles = 0;
#if _USE_LFN
    static char longfn[_MAX_LFN+1];
    
    info.lfname = longfn;
    info.lfsize = sizeof(longfn);
#endif
        
    res = f_opendir(&dir, path);
    if (FR_OK == res)
    {
        /* Display a header */
        tfp_printf("ls(path = \"%s\"):\r\n",path);
        tfp_printf("DD/MM/YYYY HH:MM               Size Filename\r\n");
                       
        for(;;)
        {
            res = f_readdir(&dir, &info);
                        
            /* Break if we have finished or an error has occured */
            if ((FR_OK != res) || (info.fname[0] == 0))
                break;
            
            /* Assign fname with the proper filename */
#       if _USE_LFN
            if (*(info.lfname))
            {
                fname = info.lfname;
            }
            else
#       else
            {
                fname = info.fname;
            }
#       endif
            
            /* Work out the date and time the file was last written to */
            day   = FDATE_DAY(info.fdate);
            month = FDATE_MONTH(info.fdate);
            year  = FDATE_YEAR(info.fdate);
            
            hour  = FTIME_HOUR(info.ftime);
            min   = FTIME_MIN(info.ftime);
            
            /* Count the number of files in this folder */
            if (!(info.fattrib & AM_DIR))
                totalfiles++;
            
            /* Count the total size of all files in this folder */
            totalsize += info.fsize;
            
            /* Display the item */
            tfp_printf("%02d/%02d/%04d %02d:%02d   %s %10ld %s\r\n",
                       day, month, year, hour, min, 
                       (info.fattrib & AM_DIR) ? "<DIR>" : "     ",
                       info.fsize,
                       fname);
        }
        
        if (FR_OK == res)
        {
            /* Display the summary if things were ok */
            tfp_printf("  %10ld File(s)     %10ld bytes\r\n",
                       totalfiles, totalsize);
        }
        
        f_closedir(&dir);
    }
    return res;
}


/* FatFS Functions ******************/

/** Initialise a drive
 *  @param pdrv Physical Drive number
 *  @return Disk Status */
DSTATUS disk_initialize(BYTE pdrv)
{
	DSTATUS stat = 0;
	int8_t status;

    status = USBH_BOMS_init(hOpenDisk, 0, &bomsCtx);

    if (status != USBH_BOMS_OK)
    {
    	tfp_printf("BOMS device could not be initialised: %d\r\n", status);
		stat = STA_NOINIT;
		hOpenDisk = 0;
    }

	return stat;
}

/** Disk Status
 *  @param pdrv Physical Drive number
 *  @return Disk Status */
DSTATUS disk_status(BYTE pdrv)
{
	DSTATUS stat = 0;
    
	if (0 == hOpenDisk)
    {
		stat |= STA_NOINIT;
    }
        
	//TODO: perform a GetSense SCSI command to make sure it's there
    if (USBH_BOMS_status(&bomsCtx) != USBH_BOMS_OK)
    {
        stat |= STA_NODISK;
    }

	return stat;
}

/** Read sector(s) from disk
 *  @param pdrv Physical Drive number
 *  @param buff Data buffer to store into
 *  @param sector The logical sector address
 *  @param count The number of sectors to read
 *  @return Disk Status */
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	DRESULT res = RES_OK;

	if (USBH_BOMS_read(&bomsCtx, sector, USBH_BOMS_BLOCK_SIZE * count, buff) != USBH_BOMS_OK)
    {
		res = RES_ERROR;
    }

	return res;
}

#if _USE_WRITE
/** Write sector(s) to the disk
 *  @param pdrv Physical Drive number
 *  @param buff Data buffer to write to the disk
 *  @param sector The logical sector address
 *  @param count The number of sectors to write
 *  @return Disk Status */
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
	DRESULT res = RES_OK;

	if (USBH_BOMS_write(&bomsCtx, sector, USBH_BOMS_BLOCK_SIZE * count, (uint8_t *)buff) != USBH_BOMS_OK)
	{
		res = RES_ERROR;
    }

	return res;
}
#endif

#if _USE_IOCTL
/** Disk IO Control
 *  @param pdrv Physical Drive Number
 *  @param cmd Control Code
 *  @param buff Buffer to send/receive control data 
 *  @return Disk Status */
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
	DRESULT res = RES_OK;

    /* Not Supported */

	return res;
}
#endif

#if _FS_READONLY == 0
/** Get the current time
 *  @return The time in the following format:
 *          bit[31:25] = Year from 1980 (0..127),
 *          bit[24:21] = Month (1..12),
 *          bit[20:16] = Day of the Month (1..31),
 *          bit[15:11] = Hour (0..23),
 *          bit[10:5]  = Minute (0..59),
 *          bit[4..0]  = Second / 2 (0..29) */
DWORD get_fattime(void)
{
    return 0; /* Invalid timestamp */
}
#endif

int8_t fs_testing(USBH_interface_handle hBOMS)
{
    FRESULT res;
    FIL f;
    UINT towrite, written;
    UINT read;
    char* data;
    char* buffer[128];

    /* Initialise FatFS */
    hOpenDisk = hBOMS;
    if ((res = f_mount(&fs, "", 1)) != FR_OK)
    {
    	tfp_printf("Unable to mount File System\r\n");
    	if (res == FR_NO_FILESYSTEM)
    	{
    		tfp_printf("   --> No FAT32 File System in the disk\r\n");
    	}
    	return -1;
    }

    /* List the root directory */
    res = dir("");

    tfp_printf("\r\n\r\n"); delayms(1000);

    /* Check to see if the example file is there */
    res = f_stat(EXAMPLE_FILE, NULL);
    if (FR_OK == res)
    {
        tfp_printf("File " EXAMPLE_FILE " already exists. Deleting\r\n");
        if (FR_OK != f_unlink(EXAMPLE_FILE))
        {
        	tfp_printf("Problem deleting " EXAMPLE_FILE "\r\n");
        }
    }

    /* Write some data to the SD Card */
    tfp_printf( "Opening " EXAMPLE_FILE " for writing\r\n");
    res = f_open(&f, EXAMPLE_FILE, FA_WRITE | FA_CREATE_NEW);
    if (FR_OK != res)
    {
    	tfp_printf("Problem creating file " EXAMPLE_FILE "\r\n");
    }

    data = (char *)LOREM_IPSUM;
    towrite = strlen(data);
    written = 0;

    while(towrite)
    {
        f_write(&f, data, towrite, &written);

        towrite -= written;
        data += written;

        tfp_printf("Wrote %d bytes\r\n", written);
    }

    tfp_printf( "Closing " EXAMPLE_FILE "\r\n");
    if (FR_OK != f_close(&f))
    {
    	tfp_printf("Error closing " EXAMPLE_FILE "\r\n");
    }


    tfp_printf("\r\n\r\n"); delayms(1000);

    /* Open the file and dump out the contents */
    tfp_printf( "Opening " EXAMPLE_FILE " for reading\r\n\r\n");
    if (FR_OK != f_open(&f, EXAMPLE_FILE, FA_READ))
    {
    	tfp_printf("Error opening " EXAMPLE_FILE " for reading\r\n");
    }

    do
    {
        f_read(&f, buffer, 128, &read);
        uart_writen(UART0, (uint8_t *)buffer, read);
    }
    while(read == 128);


    tfp_printf( "\r\n" "Closing " EXAMPLE_FILE "\r\n");
    f_close(&f);


    tfp_printf("\r\n\r\n");

    return 0;
}

void hub_scan_for_boms(USBH_device_handle hDev, int level)
{
    uint8_t status;
    USBH_device_handle hNext;
    USBH_interface_handle hInterface;
	uint8_t usbClass;
    uint8_t usbSubclass;
    uint8_t usbProtocol;

	do {
		// Search all the interfaces on this device for a HID interface
		status = USBH_get_interface_list(hDev, &hInterface);
		while (status == USBH_OK)
		{
       		if (USBH_interface_get_class_info(hInterface, &usbClass, &usbSubclass, &usbProtocol) == USBH_OK)
        	{
       			// If it is a HID interface then use that...
                if ((usbClass == USB_CLASS_MASS_STORAGE) &&
                    (usbSubclass == USB_SUBCLASS_MASS_STORAGE_SCSI) &&
                    (usbProtocol == USB_PROTOCOL_MASS_STORAGE_BOMS))
       			{
       			    tfp_printf("BOMS device found at level %d\r\n", level);
       				fs_testing(hInterface);
       			}
	        }
       		// Next interface on this device
			status = USBH_get_next_interface(hInterface, &hInterface);
		}

		// Get any child devices of this device (i.e. this is a hub).
		status = USBH_get_device_list(hDev, &hNext);
		if (status == USBH_OK)
		{
			// Recursively scan.
			hub_scan_for_boms(hNext, level + 1);
		}

		// Get any other devices on the same device.
		status = USBH_get_next_device(hDev, &hNext);
		if (status == USBH_OK)
		{
			hDev = hNext;
        }

    } while (status == USBH_OK);
}

uint8_t usbh_testing(void)
{
    uint8_t status;
    USBH_device_handle hRootDev;
    USBH_STATE connect;

    USBH_initialise(NULL);
    delayms(1000); 			//A delay to detect USB connect

    while (1)
    {
		USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		if (connect == USBH_STATE_NOTCONNECTED)
		{
			tfp_printf("\r\nPlease plug in a USB Device\r\n");

			// Detect usb device connect
			do
			{
				status = USBH_process();
				USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
			} while (connect == USBH_STATE_NOTCONNECTED);
		}
		tfp_printf("\r\nUSB Device Detected\r\n");

		do{
			status = USBH_process();
			USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		} while (connect != USBH_STATE_ENUMERATED);

		tfp_printf("USB Device Enumerated\r\n");

		// Get the first device (device on root hub)
		status = USBH_get_device_list(USBH_ROOT_HUB_HANDLE, &hRootDev);
		if (status != USBH_OK)
		{
			// Report the error code.
			tfp_printf("%d\r\n", status);
		}
		else
		{
			// Perform the actual testing.
			hub_scan_for_boms(hRootDev, 1);
		}

		tfp_printf("\r\nPlease remove the USB Device\r\n");
		// Detect usb device disconnect
		do
		{
			status = USBH_process();
			USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
		} while (connect != USBH_STATE_NOTCONNECTED);
	}
    return 0;
}

/* FUNCTIONS ***********************************************************************/

int main(int argc, char *argv[])
{
	/* Check for a USB device connection and initiate a DFU firmware download or
	   upload operation. This will timeout and return control here if no host PC
	   program contacts the device's DFU interace. USB device mode is disabled
	   before returning.
	*/
	STARTUP_DFU();

	/* Enable the UART Device... */
    sys_enable(sys_device_uart0);
    /* Make GPIO48 function as UART0_TXD and GPIO49 function as UART0_RXD... */
    gpio_function(48, pad_uart0_txd); /* UART0 TXD */
    gpio_function(49, pad_uart0_rxd); /* UART0 RXD */
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
    	);

    /* Enable tfp_printf() functionality... */
    init_printf(UART0, tfp_putc);

    sys_enable(sys_device_timer_wdt);

    interrupt_attach(interrupt_timers, (int8_t)interrupt_timers, ISR_timer);
    interrupt_enable_globally();

    /* Timer A = 1ms */
	timer_prescaler(timer_select_a, 1000);
    timer_init(timer_select_a, 1000, timer_direction_down, timer_prescaler_select_on, timer_mode_continuous);
    timer_enable_interrupt(timer_select_a);
    timer_start(timer_select_a);

    tfp_printf("Copyright (C) Bridgetek Pte Ltd \r\n"
        "--------------------------------------------------------------------- \r\n"
        "Welcome to USBH File System Tester Example 1... \r\n"
        "\r\n"
        "Find and exercises Flash Disks devices connected to the USB \r\n"
    	"Host Port\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    uart_disable_interrupt(UART0, uart_interrupt_tx);
    uart_disable_interrupt(UART0, uart_interrupt_rx);

    usbh_testing();

    interrupt_detach(interrupt_timers);
    interrupt_disable_globally();

    sys_disable(sys_device_timer_wdt);

    // Wait forever...
    for (;;);

	return 0;
}
