/**
    @file

    @brief
    Parallel Camera Interface

    
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
#include <ft900_cam.h>
#include <ft900_asm.h>
#include <registers/ft900_registers.h>
#include <private/ft900_internals.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* LOCAL FUNCTIONS / INLINES *******************************************************/

/* FUNCTIONS ***********************************************************************/

/** @brief Initialise the Camera interface
 *  @param triggers
 *  @param clkpol
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_init(cam_trigger_mode_t triggers, cam_clock_pol_t clkpol)
{
    int8_t iRet = 0;

    if (triggers > cam_trigger_mode_3 || clkpol > cam_clock_pol_raising)
    {
        iRet = -1; /* Out of range */
    }

    if (iRet == 0)
    {
        CAM->CAM_REG1 &= ~(MASK_CAM_REG1_TRIGPAT0 | MASK_CAM_REG1_TRIGPAT1
                       | MASK_CAM_REG1_TRIGPAT2 | MASK_CAM_REG1_TRIGPAT3);

        switch(triggers)
        {
            case cam_trigger_mode_0: CAM->CAM_REG1 |= MASK_CAM_REG1_TRIGPAT0; break;
            case cam_trigger_mode_1: CAM->CAM_REG1 |= MASK_CAM_REG1_TRIGPAT1; break;
            case cam_trigger_mode_2: CAM->CAM_REG1 |= MASK_CAM_REG1_TRIGPAT2; break;
            case cam_trigger_mode_3: CAM->CAM_REG1 |= MASK_CAM_REG1_TRIGPAT3; break;
            default: break;
        }

        switch(clkpol)
        {
            case cam_clock_pol_falling: CAM->CAM_REG4 &= ~MASK_CAM_REG4_CLKSENSE; break;
            case cam_clock_pol_raising: CAM->CAM_REG4 |= MASK_CAM_REG4_CLKSENSE; break;
            default: break;
        }
    }

    return iRet;
}


/** @brief Start capturing data
 *  @param bytes The number of bytes to capture
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_start(uint16_t bytes)
{
    int8_t iRet = 0;

    if ((bytes % 4) != 0)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        CAM->CAM_REG1 = (CAM->CAM_REG1 & ~MASK_CAM_REG1_COUNT) | (bytes << BIT_CAM_REG1_COUNT);
    }

    return iRet;
}


/** @brief Stop capturing data
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_stop(void)
{
    CAM->CAM_REG1 &= ~MASK_CAM_REG1_COUNT;
    return 0;
}


/** @brief Set the threshold for when the camera interrupt fires
 *  @param n The number of bytes to fill the FIFO with before the interrupt fires
 *           (this must be a multiple of 4)
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_set_threshold(uint16_t n)
{
    int8_t iRet = 0;

    if ((n % 4) != 0 || n > 2048)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        CAM->CAM_REG1 = (CAM->CAM_REG1 & ~MASK_CAM_REG1_THRESHOLD) | (n << BIT_CAM_REG1_THRESHOLD);
    }

    return iRet;
}

/** @brief Empty out the camera buffer
 */
void cam_flush(void)
{
    FT900_ATTR_UNUSED uint32_t dummy;
    while(cam_available())
        dummy = CAM->CAM_REG3;
}

/** @brief Check how many bytes are available on the FIFO
 *  @returns The number of bytes available
 */
uint16_t cam_available(void)
{
    return (CAM->CAM_REG2 & MASK_CAM_REG2_FULLNESS);
}


/** @brief Check how many bytes have been read by the Camera Interface
 *  @returns The number of bytes read
 */
uint16_t cam_total_read()
{
    return (CAM->CAM_REG1 & MASK_CAM_REG1_COUNT) >> BIT_CAM_REG1_COUNT;
}


/** @brief Read a number of bytes from the FIFO
 *  @param b A pointer to read the data into
 *  @param len The number of bytes to read from the FIFO (this must be a multiple of 4)
 *  @returns The number of bytes read, 0 otherwise
 */
uint16_t cam_readn(uint8_t *b, size_t len)
{
    uint16_t iRet = 0;

    if ((len % 4) != 0)
    {
        iRet = -1;
    }

    if (iRet == 0)
    {
        /* TODO: There may be an endianness issues with this */
        asm_streamin32(&(CAM->CAM_REG3), b, len);
        iRet = len;
    }

    return iRet;
}


/** @brief Enable the threshold interrupt
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_enable_interrupt(void)
{
    CAM->CAM_REG4 |= MASK_CAM_REG4_INTEN;
    return 0;
}


/** @brief Disable the threshold interrupt
 *  @returns 0 on success, -1 otherwise
 */
int8_t cam_disable_interrupt(void)
{
    CAM->CAM_REG4 &= ~MASK_CAM_REG4_INTEN;
    return 0;
}

/** @brief Check that an interrupt has occurred
 *  @returns 0 when the interrupt hasn't been fired, 1 when the interrupt has fired and -1 otherwise
 */
int8_t cam_is_interrupted(void)
{
    return (CAM->CAM_REG4 & MASK_CAM_REG4_HASDATA) ? 1 : 0;
}
