/**
  @file
  @brief
  SD Host Example 1

  Read and write some files using FatFS
*/
/*
 * ============================================================================
 * History
 * =======
 * 14 Nov 2014 : Created
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

#include <stdint.h>
#include <string.h>
#include <ft900.h>
#include "ff.h"
#include "diskio.h"
#include "assert.h"
#include "tinyprintf.h"

#if defined(__FT900__)
#define GPIO_SD_CLK  (19)
#define GPIO_SD_CMD  (20)
#define GPIO_SD_DAT3 (21)
#define GPIO_SD_DAT2 (22)
#define GPIO_SD_DAT1 (23)
#define GPIO_SD_DAT0 (24)
#define GPIO_SD_CD   (25)
#define GPIO_SD_WP   (26)
#elif defined(__FT930__)
// SDHC to GPIO mapping for FT93x
#define GPIO_SD_CLK  (0)
#define GPIO_SD_CMD  (1)
#define GPIO_SD_DAT3 (6)
#define GPIO_SD_DAT2 (5)
#define GPIO_SD_DAT1 (4)
#define GPIO_SD_DAT0 (3)
#define GPIO_SD_CD   (2)
#define GPIO_SD_WP   (7)
#endif

#define EXAMPLE_FILE "LOREM.TXT"

#define FDATE_DAY(fdate) ((fdate)&(0x1F))
#define FDATE_MONTH(fdate) (((fdate)>>5)&(0x0F))
#define FDATE_YEAR(fdate) ( (((fdate)>>9)&(0x7F)) + 1980 )

#define FTIME_HOUR(ftime) (((ftime)>>11)&(0x1F))
#define FTIME_MIN(ftime) (((ftime)>>5)&(0x3F))


void setup(void);
void loop(void);
FRESULT dir(char* path);
DSTATUS disk_initialize(BYTE pdrv);
DSTATUS disk_status(BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
DWORD get_fattime(void);
void myputc(void* p, char c);


static int sd_ready = 0;
FATFS fs;

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


/** Main program */
int main(void)
{
    setup();
    for(;;) loop();
    return 0;
}

/** Setup */
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
        "Welcome to SD Host Example 1... \r\n"
        "\r\n"
        "Read and write some files using FatFS\r\n"
        "--------------------------------------------------------------------- \r\n"
        );

    /* Enable tfp_printf() functionality... */
    init_printf(UART0,myputc);

    /* All SD Host pins except CLK need a pull-up to work. The MM900EV*A module does not have external pull-up, so enable internal one */
    gpio_function(GPIO_SD_CLK, pad_sd_clk); gpio_pull(GPIO_SD_CLK, pad_pull_none);
    gpio_function(GPIO_SD_CMD, pad_sd_cmd); gpio_pull(GPIO_SD_CMD, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT3, pad_sd_data3); gpio_pull(GPIO_SD_DAT3, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT2, pad_sd_data2); gpio_pull(GPIO_SD_DAT2, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT1, pad_sd_data1); gpio_pull(GPIO_SD_DAT1, pad_pull_pullup);
    gpio_function(GPIO_SD_DAT0, pad_sd_data0); gpio_pull(GPIO_SD_DAT0, pad_pull_pullup);
    gpio_function(GPIO_SD_CD, pad_sd_cd); gpio_pull(GPIO_SD_CD, pad_pull_pullup);
    gpio_function(GPIO_SD_WP, pad_sd_wp); gpio_pull(GPIO_SD_WP, pad_pull_pullup);


    /* Start up the SD Card */
    sys_enable(sys_device_sd_card);

    sdhost_init();  
    
    /* Check to see if a card is inserted */
    uart_puts(UART0, "Please Insert SD Card\r\n");
    while (sdhost_card_detect() != SDHOST_CARD_INSERTED);
    uart_puts(UART0, "SD Card Inserted\r\n");

    /* Initialise FatFS */
    ASSERT_P(FR_OK, f_mount(&fs, "", 0), "Unable to mount File System");

#   if _USE_LABEL == 1
    {
        char label[64];

        if (FR_OK == f_getlabel("", label, NULL))
            tfp_printf("Mounted \"%s\"\r\n",label);
        else
            tfp_printf("Mounted");
    }
#   else
    tfp_printf("Mounted\r\n");
#   endif  /* _USE_LABEL == 1 */

}

/** Loop */
void loop(void)
{
    FRESULT res;
    FIL f;
    UINT towrite, written;
    UINT read;
    char* testdata;
    char buffer[128];

    /* List the root directory */
    res = dir("");

    uart_puts(UART0,"\r\n\r\n"); delayms(1000);

    /* Check to see if the example file is there */
    res = f_stat(EXAMPLE_FILE, NULL);
    if (FR_OK == res)
    {
        uart_puts(UART0, EXAMPLE_FILE " already exists. Deleting\r\n");
        ASSERT_P(FR_OK, f_unlink(EXAMPLE_FILE), "Problem deleting " EXAMPLE_FILE);
    }

    /* Write some data to the SD Card */
    uart_puts(UART0, "Opening " EXAMPLE_FILE " for writing\r\n");
    res = f_open(&f, EXAMPLE_FILE, FA_WRITE | FA_CREATE_NEW);
    ASSERT_P(FR_OK, res, "Problem creating " EXAMPLE_FILE);

    testdata = (char *) LOREM_IPSUM;
    towrite = strlen(testdata);
    written = 0;

    while(towrite)
    {
        f_write(&f, testdata, towrite, &written);

        towrite -= written;
        testdata += written;

        tfp_printf("Wrote %d bytes\r\n", written);
    }

    uart_puts(UART0, "Closing " EXAMPLE_FILE "\r\n");
    ASSERT_P(FR_OK, f_close(&f), "Error closing " EXAMPLE_FILE);


    uart_puts(UART0,"\r\n\r\n"); delayms(1000);

    /* Open the file and dump out the contents */
    uart_puts(UART0, "Opening " EXAMPLE_FILE " for reading\r\n\r\n");
    ASSERT(FR_OK, f_open(&f, EXAMPLE_FILE, FA_READ));

    do
    {
        f_read(&f, buffer, 128, &read);
        uart_writen(UART0, (uint8_t *) buffer, read);
    }
    while(read == 128);


    uart_puts(UART0, "\r\n" "Closing " EXAMPLE_FILE "\r\n");
    f_close(&f);


    uart_puts(UART0,"\r\n\r\n");

    for(;;);
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

	if(SDHOST_OK != sdhost_card_init())
    {
		stat = STA_NOINIT;
    }
    else
    {
        sd_ready = 1;
    }

	return stat;
}

/** Disk Status
 *  @param pdrv Physical Drive number
 *  @return Disk Status */
DSTATUS disk_status(BYTE pdrv)
{
	DSTATUS stat = 0;

	if (0 == sd_ready)
    {
		stat |= STA_NOINIT;
    }

    if (sdhost_card_detect() == SDHOST_CARD_REMOVED)
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

	if (SDHOST_OK !=
        sdhost_transfer_data(SDHOST_READ, (void*)buff, SDHOST_BLK_SIZE * count, sector))
    {
		res = RES_ERROR;
    }

	return res;
}


/** Write sector(s) to the disk
 *  @param pdrv Physical Drive number
 *  @param buff Data buffer to write to the disk
 *  @param sector The logical sector address
 *  @param count The number of sectors to write
 *  @return Disk Status */
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
	DRESULT res = RES_OK;

	if (SDHOST_OK !=
        sdhost_transfer_data(SDHOST_WRITE, (void*)buff, SDHOST_BLK_SIZE * count, sector))
	{
		res = RES_ERROR;
    }

	return res;
}



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



/** Printf putc
 *  @param p Parameters
 *  @param c The character to write */
void myputc(void* p, char c)
{
    uart_write((ft900_uart_regs_t*)p, (uint8_t)c);
}
