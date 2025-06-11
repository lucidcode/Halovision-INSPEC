/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 * @file     MRAM_Baremetal.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     01-June-2023
 * @brief    TestApp to verify MRAM(On-chip NVM (Non-Volatile Memory)) interface
 *            using baremetal, with no operating system.
 *            Verify Read/Write to/from MRAM.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* System Includes */
#include <RTE_Components.h>
#include CMSIS_device_header

#include <stdio.h>
#include "string.h"

/* Project Includes */
/* include for MRAM Driver */
#include "Driver_MRAM.h"
#if defined(RTE_Compiler_IO_STDOUT)
#include "retarget_stdout.h"
#endif  /* RTE_Compiler_IO_STDOUT */


/* for Unused Arguments. */
#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)     ((void)arg)
#endif


/* MRAM Driver */
extern ARM_DRIVER_MRAM Driver_MRAM;
static ARM_DRIVER_MRAM *MRAM_drv = &Driver_MRAM;

void MRAM_Thread_entry();

/* Valid MRAM address-offset. */
#define MRAM_ADDR_OFFSET           (0x100000)

/* Buffer size and value which needs to write to MRAM. */
#define BUFFER_SIZE   0x10000 /* any random size.(for demo purpose size taken as 64KB) */
#define BUFFER_VALUE  0xDF    /* any random value. */

uint8_t buff_TX[BUFFER_SIZE] = {0x00};
uint8_t buff_RX[BUFFER_SIZE] = {0x00};

/**
 * @function    void MRAM_thread_entry()
 * @brief       TestApp to verify MRAM(On-Chip NVM) interface
 *              using baremetal, with no operating system.
 *                 - Verify Read/Write to/from MRAM.
 *                 - write 1MB data to MRAM
 *                 - read back and compare wrote data
 * @note        none
 * @param       none
 * @retval      none
 */
void MRAM_Thread_entry()
{
    int32_t ret;
    ARM_DRIVER_VERSION version;

    uint32_t addr    = MRAM_ADDR_OFFSET;
    int cmp          = 0;
    int err_cnt      = 0;

    /* Fill buffer data which needs to write to MRAM. */
    memset(buff_TX, BUFFER_VALUE, sizeof(buff_TX));

    printf("\r\n >>> MRAM testApp starting up!!!...<<< \r\n");

    /* MRAM driver version. */
    version = MRAM_drv->GetVersion();
    printf("\r\n MRAM version: api:0x%X driver:0x%X...\r\n",version.api, version.drv);
    ARG_UNUSED(version);

    /* Initialize MRAM driver */
    ret = MRAM_drv->Initialize();
    if(ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error in MRAM Initialize.\r\n");
        return;
    }

    /* Power up peripheral */
    ret = MRAM_drv->PowerControl(ARM_POWER_FULL);
    if(ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error in MRAM Power Up.\r\n");
        goto error_uninitialize;
    }

    /* write data to MRAM (for demo purpose write 1MB data (64KB x 16) ) */
    for(int i = 0; i < 16; i++)
    {
        ret = MRAM_drv->ProgramData(addr, buff_TX, BUFFER_SIZE);
        if(ret != BUFFER_SIZE)
        {
            printf("\r\n Error: MRAM ProgramData failed: addr:0x%X data:%0x.\r\n", \
                                    addr, (uint32_t)buff_TX);
            goto error_poweroff;
        }
        addr += BUFFER_SIZE;
    }

    /* read back and compare wrote data from MRAM. */
    addr = MRAM_ADDR_OFFSET;

    for(int i = 0; i < 16; i++)
    {
        ret = MRAM_drv->ReadData(addr, buff_RX, BUFFER_SIZE);
        if(ret != BUFFER_SIZE)
        {
            printf("\r\n Error: MRAM ReadData failed: addr:0x%X data:%0x.\r\n", \
                                    addr, (uint32_t)buff_RX);
            goto error_poweroff;
        }

        /* compare write and read. */
        cmp = memcmp(buff_TX, buff_RX, BUFFER_SIZE);
        if(cmp != 0)
        {
            printf("\r\n Error: MRAM write-read failed: addr:0x%X data:%0x.\r\n", \
                                    addr, (uint32_t)buff_RX);
            err_cnt++;
        }
        addr += BUFFER_SIZE;
    }

    /* check for error. */
    if(err_cnt)
    {
        printf("\r\n waiting in Error. \r\n");
        while(1);
    }

    printf("\r\n MRAM Write-Read test completed!!! \r\n");
    printf("\r\n waiting here forever...\r\n");
    while(1);

error_poweroff:
    /* Received error, Power off MRAM peripheral */
    ret = MRAM_drv->PowerControl(ARM_POWER_OFF);
    if(ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error in MRAM Power OFF.\r\n");
    }

error_uninitialize:
    /* Received error, Un-initialize MRAM driver */
    ret = MRAM_drv->Uninitialize();
    if(ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error in MRAM Uninitialize.\r\n");
    }

    printf("\r\n XXX MRAM demo thread exiting XXX...\r\n");
}

/* Define main entry point */
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
    MRAM_Thread_entry();
}

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
