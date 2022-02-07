/**************************************************************************//**
 * @file     i2c.c
 * @version  V3.00
 * @brief    NUC121 series I2C driver source file
 *
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup I2C_Driver I2C Driver
  @{
*/


/** @addtogroup I2C_EXPORTED_FUNCTIONS I2C Exported Functions
  @{
*/

/**
  * @brief      Enable specify I2C Controller and set Clock Divider
  *
  * @param[in]  i2c         Specify I2C port
  * @param[in]  u32BusClock The target I2C Bus clock in Hz
  *
  * @return     Actual I2C Bus Clock frequency
  *
  * @details    The function enable the specify I2C Controller and set proper Clock Divider
  *             in I2C CLOCK DIVIDED REGISTER (I2CLK) according to the target I2C Bus clock.
  *             I2C Bus clock = PCLK / (4*(divider+1).
  *
  */
uint32_t I2C_Open(I2C_T *i2c, uint32_t u32BusClock)
{
    uint32_t u32Div;

    u32Div = (uint32_t)(((SystemCoreClock * 10) / (u32BusClock * 4) + 5) / 10 - 1); /* Compute proper divider for I2C clock */
    i2c->CLKDIV = u32Div;

    /* Enable I2C */
    i2c->CTL |= I2C_CTL_I2CEN_Msk;

    return (SystemCoreClock / ((u32Div + 1) << 2));
}

/**
  * @brief      Disable specify I2C Controller
  *
  * @param[in]  i2c         Specify I2C port
    *
  * @return     None
  *
  * @details    Reset I2C Controller and disable specify I2C port.
    *
  */

void I2C_Close(I2C_T *i2c)
{
    /* Reset I2C controller */
    if ((uint32_t)i2c == I2C0_BASE)
    {
        SYS->IPRST1 |= SYS_IPRST1_I2C0RST_Msk;
        SYS->IPRST1 &= ~SYS_IPRST1_I2C0RST_Msk;
    }
    else if ((uint32_t)i2c == I2C1_BASE)
    {
        SYS->IPRST1 |= SYS_IPRST1_I2C1RST_Msk;
        SYS->IPRST1 &= ~SYS_IPRST1_I2C1RST_Msk;
    }

    /* Disable I2C */
    i2c->CTL &= ~I2C_CTL_I2CEN_Msk;
}

/**
  * @brief      Clear Time-out Counter flag
  *
  * @param[in]  i2c         Specify I2C port
    *
  * @return     None
  *
  * @details    When Time-out flag will be set, use this function to clear I2C Bus Time-out Counter flag .
    *
  */
void I2C_ClearTimeoutFlag(I2C_T *i2c)
{
    i2c->TOCTL |= I2C_TOCTL_TOIF_Msk;
}

/**
  * @brief      Set control bit of I2C Controller
  *
  * @param[in]  i2c         Specify I2C port
  * @param[in]  u8Start     Set I2C START condition
  * @param[in]  u8Stop      Set I2C STOP condition
  * @param[in]  u8Si        Clear SI flag
  * @param[in]  u8Ack       Set I2C ACK bit
  *
  * @return     None
  *
  * @details    The function set I2C control bit of I2C Bus protocol.
  *
  */
void I2C_Trigger(I2C_T *i2c, uint8_t u8Start, uint8_t u8Stop, uint8_t u8Si, uint8_t u8Ack)
{
    uint32_t u32Reg = 0;

    if (u8Start)
        u32Reg |= I2C_CTL_STA;

    if (u8Stop)
        u32Reg |= I2C_CTL_STO;

    if (u8Si)
        u32Reg |= I2C_CTL_SI;

    if (u8Ack)
        u32Reg |= I2C_CTL_AA;

    i2c->CTL = (i2c->CTL & ~0x3C) | u32Reg;
}

/**
  * @brief      Disable Interrupt of I2C Controller
  *
  * @param[in]  i2c         Specify I2C port
  *
  * @return     None
  *
  * @details    The function is used for disable I2C interrupt
  *
  */
void I2C_DisableInt(I2C_T *i2c)
{
    i2c->CTL &= ~I2C_CTL_INTEN_Msk;
}

/**
  * @brief      Enable Interrupt of I2C Controller
  *
  * @param[in]  i2c         Specify I2C port
  *
  * @return     None
  *
  * @details    The function is used for enable I2C interrupt
  *
  */
void I2C_EnableInt(I2C_T *i2c)
{
    i2c->CTL |= I2C_CTL_INTEN_Msk;
}

/**
 * @brief      Get I2C Bus clock
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     The actual I2C Bus clock in Hz
 *
 * @details    To get the actual I2C Bus clock frequency.
 */
uint32_t I2C_GetBusClockFreq(I2C_T *i2c)
{
    uint32_t u32Divider = i2c->CLKDIV;

    return (SystemCoreClock / ((u32Divider + 1) << 2));
}

/**
 * @brief      Set I2C Bus clock
 *
 * @param[in]  i2c          Specify I2C port
 * @param[in]  u32BusClock  The target I2C Bus clock in Hz
 *
 * @return     The actual I2C Bus clock in Hz
 *
 * @details    To set the actual I2C Bus clock frequency.
 */
uint32_t I2C_SetBusClockFreq(I2C_T *i2c, uint32_t u32BusClock)
{
    uint32_t u32Div;

    u32Div = (uint32_t)(((SystemCoreClock * 10) / (u32BusClock * 4) + 5) / 10 - 1); /* Compute proper divider for I2C clock */
    i2c->CLKDIV = u32Div;

    return (SystemCoreClock / ((u32Div + 1) << 2));
}

/**
 * @brief      Get Interrupt Flag
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     I2C interrupt flag status
 *
 * @details    To get I2C Bus interrupt flag.
 */
uint32_t I2C_GetIntFlag(I2C_T *i2c)
{
    return ((i2c->CTL & I2C_CTL_SI_Msk) == I2C_CTL_SI_Msk ? 1 : 0);
}

/**
 * @brief      Get I2C bus Status Code
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     I2C status code
 *
 * @details    To get I2C Bus Status Code.
 */
uint32_t I2C_GetStatus(I2C_T *i2c)
{
    return (i2c->STATUS);
}

/**
 * @brief      Read a Byte from I2C Bus
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     I2C Data
 *
 * @details    To read a bytes data from specify I2C port.
 */
uint8_t I2C_GetData(I2C_T *i2c)
{
    return (i2c->DAT);
}

/**
 * @brief      Send a byte to I2C bus
 *
 * @param[in]  i2c          I2C port
 * @param[in]  u8Data       The data to send to I2C bus
 *
 * @return     None
 *
 * @details    This function is used to write a byte to specified I2C port
 */
void I2C_SetData(I2C_T *i2c, uint8_t u8Data)
{
    i2c->DAT = u8Data;
}

/**
 * @brief      Set 7-bit Slave Address and GC Mode
 *
 * @param[in]  i2c          I2C port
 * @param[in]  u8SlaveNo    Set the number of I2C address register (0~3)
 * @param[in]  u8SlaveAddr  7-bit slave address
 * @param[in]  u8GCMode     Enable/Disable GC Mode (I2C_GCMODE_ENABLE / I2C_GCMODE_DISABLE)
 *
 * @return     None
 *
 * @details    This function is used to set 7-bit slave addresses in I2C SLAVE ADDRESS REGISTER (I2CADDR0~3)
 *             and enable GC Mode.
 *
 */
void I2C_SetSlaveAddr(I2C_T *i2c, uint8_t u8SlaveNo, uint8_t u8SlaveAddr, uint8_t u8GCMode)
{
    switch (u8SlaveNo)
    {
    case 1:
        i2c->ADDR1  = (u8SlaveAddr << 1) | u8GCMode;
        break;

    case 2:
        i2c->ADDR2  = (u8SlaveAddr << 1) | u8GCMode;
        break;

    case 3:
        i2c->ADDR3  = (u8SlaveAddr << 1) | u8GCMode;
        break;

    case 0:
    default:
        i2c->ADDR0  = (u8SlaveAddr << 1) | u8GCMode;
        break;
    }
}

/**
 * @brief      Configure the mask bits of 7-bit Slave Address
 *
 * @param[in]  i2c              I2C port
 * @param[in]  u8SlaveNo        Set the number of I2C address mask register (0~3)
 * @param[in]  u8SlaveAddrMask  A byte for slave address mask
 *
 * @return     None
 *
 * @details    This function is used to set 7-bit slave addresses.
 *
 */
void I2C_SetSlaveAddrMask(I2C_T *i2c, uint8_t u8SlaveNo, uint8_t u8SlaveAddrMask)
{
    switch (u8SlaveNo)
    {
    case 1:
        i2c->ADDRMSK1  = u8SlaveAddrMask << 1;
        break;

    case 2:
        i2c->ADDRMSK2  = u8SlaveAddrMask << 1;
        break;

    case 3:
        i2c->ADDRMSK3  = u8SlaveAddrMask << 1;
        break;

    case 0:
    default:
        i2c->ADDRMSK0  = u8SlaveAddrMask << 1;
        break;
    }
}

/**
 * @brief      Enable Time-out Counter Function and support Long Time-out
 *
 * @param[in]  i2c              I2C port
 * @param[in]  u8LongTimeout    Configure DIV4 to enable Long Time-out (0/1)
 *
 * @return     None
 *
 * @details    This function enable Time-out counter function and configure DIV4 to support Long
 *             Time-out.
 *
 */
void I2C_EnableTimeout(I2C_T *i2c, uint8_t u8LongTimeout)
{
    if (u8LongTimeout)
        i2c->TOCTL |= I2C_TOCTL_TOCDIV4_Msk;
    else
        i2c->TOCTL &= ~I2C_TOCTL_TOCDIV4_Msk;

    i2c->TOCTL |= I2C_TOCTL_TOCEN_Msk;
}

/**
 * @brief      Disable Time-out Counter Function
 *
 * @param[in]  i2c          I2C port
 *
 * @return     None
 *
 * @details    To disable Time-out counter function in I2CTOC register.
 *
 */
void I2C_DisableTimeout(I2C_T *i2c)
{
    i2c->TOCTL &= ~I2C_TOCTL_TOCEN_Msk;
}

/**
 * @brief      Enable I2C Wake-up Function
 *
 * @param[in]  i2c          I2C port
 *
 * @return     None
 *
 * @details    To enable Wake-up function of I2C Wake-up control register.
 *
 */
void I2C_EnableWakeup(I2C_T *i2c)
{
    i2c->WKCTL |= I2C_WKCTL_WKEN_Msk;
}

/**
 * @brief      Disable I2C Wake-up Function
 *
 * @param[in]  i2c          I2C port
 *
 * @return     None
 *
 * @details    To disable Wake-up function of I2C Wake-up control register.
 *
 */
void I2C_DisableWakeup(I2C_T *i2c)
{
    i2c->WKCTL &= ~I2C_WKCTL_WKEN_Msk;
}


/**
  * @brief      Write a byte to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8Data          Write a byte data to Slave
  *
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I2C Master write a byte data to Slave.
  *
  */

uint8_t I2C_WriteByte(I2C_T *i2c, uint8_t u8SlaveAddr, const uint8_t u8Data)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;

    I2C_START(i2c);

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));    /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                           /* Clear SI */
            break;

        case 0x18:                                           /* Slave Address ACK */
            I2C_SET_DATA(i2c, u8Data);                         /* Write data to I2CDAT */
            break;

        case 0x20:                                           /* Slave Address NACK */
        case 0x30:                                           /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                       /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            u8Ctrl = I2C_CTL_STO_SI;                       /* Clear SI and send STOP */
            u8Xfering = 0;
            break;

        case 0x38:                                           /* Arbitration Lost */
        default:                                             /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                       /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                        /* Write controlbit to I2C_CTL register */
    }

    return (u8Err | u8Xfering);                                  /* return (Success)/(Fail) status */
}

/**
  * @brief      Write multi bytes to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  *pu8Data        Pointer to array to write data to Slave
  * @param[in]  u32wLen         How many bytes need to write to Slave
  *
  * @return     A length of how many bytes have been transmitted.
  *
  * @details    The function is used for I2C Master write multi bytes data to Slave.
  *
  */

uint32_t I2C_WriteMultiBytes(I2C_T *i2c, uint8_t u8SlaveAddr, const uint8_t *pu8Data, uint32_t u32wLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
    uint32_t u32txLen = 0;

    I2C_START(i2c);                                              /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));    /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                           /* Clear SI */
            break;

        case 0x18:                                           /* Slave Address ACK */
        case 0x28:
            if (u32txLen < u32wLen)
                I2C_SET_DATA(i2c, pu8Data[u32txLen++]);                /* Write Data to I2CDAT */
            else
            {
                u8Ctrl = I2C_CTL_STO_SI;                   /* Clear SI and send STOP */
                u8Xfering = 0;
            }

            break;

        case 0x20:                                           /* Slave Address NACK */
        case 0x30:                                           /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                       /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x38:                                           /* Arbitration Lost */
        default:                                             /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                       /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                        /* Write controlbit to I2C_CTL register */
    }

    return u32txLen;                                             /* Return bytes length that have been transmitted */
}

/**
  * @brief      Specify a byte register address and write a byte to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address (1 byte) of data write to
  * @param[in]  u8Data          A byte data to write it to Slave
  *
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I2C Master specify a address that data write to in Slave.
  *
  */

uint8_t I2C_WriteByteOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr, const uint8_t u8Data)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
    uint32_t u32txLen = 0;

    I2C_START(i2c);                                              /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));    /* Send Slave address with write bit */
            u8Ctrl = I2C_CTL_SI;                           /* Clear SI */
            break;

        case 0x18:                                           /* Slave Address ACK */
            I2C_SET_DATA(i2c, u8DataAddr);                   /* Write Lo byte address of register */
            break;

        case 0x20:                                           /* Slave Address NACK */
        case 0x30:                                           /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                       /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            if (u32txLen < 1)
            {
                I2C_SET_DATA(i2c, u8Data);
                u32txLen++;
            }
            else
            {
                u8Ctrl = I2C_CTL_STO_SI;                   /* Clear SI and send STOP */
                u8Xfering = 0;
            }

            break;

        case 0x38:                                           /* Arbitration Lost */
        default:                                             /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                       /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                        /* Write controlbit to I2C_CTL register */
    }

    return (u8Err | u8Xfering);                                  /* return (Success)/(Fail) status */
}


/**
  * @brief      Specify a byte register address and write multi bytes to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address (1 byte) of data write to
  * @param[in]  *pu8Data        Pointer to array to write data to Slave
  * @param[in]  u32wLen         How many bytes need to write to Slave
  *
  * @return     A length of how many bytes have been transmitted.
  *
  * @details    The function is used for I2C Master specify a byte address that multi data bytes write to in Slave.
  *
  */

uint32_t I2C_WriteMultiBytesOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr, const uint8_t *pu8Data, uint32_t u32wLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
    uint32_t u32txLen = 0;

    I2C_START(i2c);                                              /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));    /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;
            break;

        case 0x18:                                           /* Slave Address ACK */
            I2C_SET_DATA(i2c, u8DataAddr);                   /* Write Lo byte address of register */
            break;

        case 0x20:                                           /* Slave Address NACK */
        case 0x30:                                           /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                       /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            if (u32txLen < u32wLen)
                I2C_SET_DATA(i2c, pu8Data[u32txLen++]);
            else
            {
                u8Ctrl = I2C_CTL_STO_SI;                   /* Clear SI and send STOP */
                u8Xfering = 0;
            }

            break;

        case 0x38:                                           /* Arbitration Lost */
        default:                                             /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                       /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                        /* Write controlbit to I2C_CTL register */
    }

    return u32txLen;                                             /* Return bytes length that have been transmitted */
}

/**
  * @brief      Specify two bytes register address and Write a byte to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u16DataAddr     Specify a address (2 byte) of data write to
  * @param[in]  u8Data          Write a byte data to Slave
  *
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I2C Master specify two bytes address that data write to in Slave.
  *
  */

uint8_t I2C_WriteByteTwoRegs(I2C_T *i2c, uint8_t u8SlaveAddr, uint16_t u16DataAddr, const uint8_t u8Data)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Addr = 1, u8Ctrl = 0;
    uint32_t u32txLen = 0;

    I2C_START(i2c);                                                         /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));               /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                                        /* Clear SI */
            break;

        case 0x18:                                                      /* Slave Address ACK */
            I2C_SET_DATA(i2c, (uint8_t)((u16DataAddr & 0xFF00) >> 8));    /* Write Hi byte address of register */
            break;

        case 0x20:                                                      /* Slave Address NACK */
        case 0x30:                                                      /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                                    /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            if (u8Addr)
            {
                I2C_SET_DATA(i2c, (uint8_t)(u16DataAddr & 0xFF));       /* Write Lo byte address of register */
                u8Addr = 0;
            }
            else if ((u32txLen < 1) && (u8Addr == 0))
            {
                I2C_SET_DATA(i2c, u8Data);
                u32txLen++;
            }
            else
            {
                u8Ctrl = I2C_CTL_STO_SI;                              /* Clear SI and send STOP */
                u8Xfering = 0;
            }

            break;

        case 0x38:                                                      /* Arbitration Lost */
        default:                                                        /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                   /* Write controlbit to I2C_CTL register */
    }

    return (u8Err | u8Xfering);                                             /* return (Success)/(Fail) status */
}


/**
  * @brief      Specify two bytes register address and write multi bytes to Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u16DataAddr     Specify a address (2 bytes) of data write to
  * @param[in]  *pu8Data        Pointer to array to write data to Slave
  * @param[in]  u32wLen         How many bytes need to write to Slave
  *
  * @return     A length of how many bytes have been transmitted.
  *
  * @details    The function is used for I2C Master specify a byte address that multi data write to in Slave.
  *
  */

uint32_t I2C_WriteMultiBytesTwoRegs(I2C_T *i2c, uint8_t u8SlaveAddr, uint16_t u16DataAddr, const uint8_t *pu8Data, uint32_t u32wLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Addr = 1, u8Ctrl = 0;
    uint32_t u32txLen = 0;

    I2C_START(i2c);                                                         /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));               /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */
            break;

        case 0x18:                                                      /* Slave Address ACK */
            I2C_SET_DATA(i2c, (uint8_t)((u16DataAddr & 0xFF00) >> 8));    /* Write Hi byte address of register */
            break;

        case 0x20:                                                      /* Slave Address NACK */
        case 0x30:                                                      /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            if (u8Addr)
            {
                I2C_SET_DATA(i2c, (uint8_t)(u16DataAddr & 0xFF));       /* Write Lo byte address of register */
                u8Addr = 0;
            }
            else if ((u32txLen < u32wLen) && (u8Addr == 0))
                I2C_SET_DATA(i2c, pu8Data[u32txLen++]);                           /* Write data to Register I2CDAT*/
            else
            {
                u8Ctrl = I2C_CTL_STO_SI;                              /* Clear SI and send STOP */
                u8Xfering = 0;
            }

            break;

        case 0x38:                                                      /* Arbitration Lost */
        default:                                                        /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                   /* Write controlbit to I2C_CTL register */
    }

    return u32txLen;                                                        /* Return bytes length that have been transmitted */
}

/**
  * @brief      Read a byte from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  *
  * @return     Read a byte data from Slave
  *
  * @details    The function is used for I2C Master to read a byte data from Slave.
  *
  */
uint8_t I2C_ReadByte(I2C_T *i2c, uint8_t u8SlaveAddr)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8rData = 0, u8Ctrl = 0;

    I2C_START(i2c);                                                /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));    /* Write SLA+R to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x40:                                             /* Slave Address ACK */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x48:                                             /* Slave Address NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x58:
            u8rData = (unsigned char) I2C_GET_DATA(i2c);         /* Receive Data */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Xfering = 0;
            break;

        case 0x38:                                             /* Arbitration Lost */
        default:                                               /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                          /* Write controlbit to I2C_CTL register */
    }

    if (u8Err)
        u8rData = 0;                                                 /* If occurs error, return 0 */

    return u8rData;                                                  /* Return read data */
}


/**
  * @brief      Read multi bytes from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[out] *pu8rData       Point to array to store data from Slave
  * @param[in]  u32rLen         How many bytes need to read from Slave
  *
  * @return     A length of how many bytes have been received
  *
  * @details    The function is used for I2C Master to read multi data bytes from Slave.
  *
  *
  */
uint32_t I2C_ReadMultiBytes(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t *pu8rData, uint32_t u32rLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
    uint32_t u32rxLen = 0;

    I2C_START(i2c);                                                /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));    /* Write SLA+R to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x40:                                             /* Slave Address ACK */
            u8Ctrl = I2C_CTL_SI_AA;                          /* Clear SI and set ACK */
            break;

        case 0x48:                                             /* Slave Address NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x50:
            pu8rData[u32rxLen++] = (unsigned char) I2C_GET_DATA(i2c);    /* Receive Data */

            if (u32rxLen < (u32rLen - 1))
            {
                u8Ctrl = I2C_CTL_SI_AA;                             /* Clear SI and set ACK */
            }
            else
            {
                u8Ctrl = I2C_CTL_SI;                                /* Clear SI */
            }

            break;

        case 0x58:
            pu8rData[u32rxLen++] = (unsigned char) I2C_GET_DATA(i2c);    /* Receive Data */
            u8Ctrl = I2C_CTL_STO_SI;                                /* Clear SI and send STOP */
            u8Xfering = 0;
            break;

        case 0x38:                                                    /* Arbitration Lost */
        default:                                                      /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                                /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                 /* Write controlbit to I2C_CTL register */
    }

    return u32rxLen;                                                      /* Return bytes length that have been received */
}


/**
  * @brief      Specify a byte register address and read a byte from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address(1 byte) of data read from
  *
  * @return     Read a byte data from Slave
  *
  * @details    The function is used for I2C Master specify a byte address that a data byte read from Slave.
  *
  *
  */
uint8_t I2C_ReadByteOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8rData = 0, u8Ctrl = 0;

    I2C_START(i2c);                                                /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));      /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x18:                                             /* Slave Address ACK */
            I2C_SET_DATA(i2c, u8DataAddr);                     /* Write Lo byte address of register */
            break;

        case 0x20:                                             /* Slave Address NACK */
        case 0x30:                                             /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            u8Ctrl = I2C_CTL_STA_SI;                         /* Send repeat START */
            break;

        case 0x10:
            I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));    /* Write SLA+R to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                               /* Clear SI */
            break;

        case 0x40:                                             /* Slave Address ACK */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x48:                                             /* Slave Address NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x58:
            u8rData = (uint8_t) I2C_GET_DATA(i2c);               /* Receive Data */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Xfering = 0;
            break;

        case 0x38:                                             /* Arbitration Lost */
        default:                                               /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                          /* Write controlbit to I2C_CTL register */
    }

    if (u8Err)
        u8rData = 0;                                                 /* If occurs error, return 0 */

    return u8rData;                                                  /* Return read data */
}

/**
  * @brief      Specify a byte register address and read multi bytes from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address (1 bytes) of data read from
  * @param[out] *pu8rData       Point to array to store data from Slave
  * @param[in]  u32rLen         How many bytes need to read from Slave
  *
  * @return     A length of how many bytes have been received
  *
  * @details    The function is used for I2C Master specify a byte address that multi data bytes read from Slave.
  *
  *
  */
uint32_t I2C_ReadMultiBytesOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr, uint8_t *pu8rData, uint32_t u32rLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Ctrl = 0;
    uint32_t u32rxLen = 0;

    I2C_START(i2c);                                                /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));      /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x18:                                             /* Slave Address ACK */
            I2C_SET_DATA(i2c, u8DataAddr);                     /* Write Lo byte address of register */
            break;

        case 0x20:                                             /* Slave Address NACK */
        case 0x30:                                             /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            u8Ctrl = I2C_CTL_STA_SI;                         /* Send repeat START */
            break;

        case 0x10:
            I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));    /* Write SLA+R to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x40:                                             /* Slave Address ACK */
            u8Ctrl = I2C_CTL_SI_AA;                          /* Clear SI and set ACK */
            break;

        case 0x48:                                             /* Slave Address NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x50:
            pu8rData[u32rxLen++] = (uint8_t) I2C_GET_DATA(i2c);   /* Receive Data */

            if (u32rxLen < (u32rLen - 1))
                u8Ctrl = I2C_CTL_SI_AA;                      /* Clear SI and set ACK */
            else
                u8Ctrl = I2C_CTL_SI;                         /* Clear SI */

            break;

        case 0x58:
            pu8rData[u32rxLen++] = (uint8_t) I2C_GET_DATA(i2c);   /* Receive Data */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Xfering = 0;
            break;

        case 0x38:                                             /* Arbitration Lost */
        default:                                               /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                         /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                          /* Write controlbit to I2C_CTL register */
    }

    return u32rxLen;                                               /* Return bytes length that have been received */
}

/**
  * @brief      Specify two bytes register address and read a byte from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u16DataAddr     Specify a address(2 byte) of data read from
  *
  * @return     Read a byte data from Slave
  *
  * @details    The function is used for I2C Master specify two bytes address that a data byte read from Slave.
  *
  *
  */
uint8_t I2C_ReadByteTwoRegs(I2C_T *i2c, uint8_t u8SlaveAddr, uint16_t u16DataAddr)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8rData = 0, u8Addr = 1, u8Ctrl = 0;

    I2C_START(i2c);                                                         /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));               /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */
            break;

        case 0x18:                                                      /* Slave Address ACK */
            I2C_SET_DATA(i2c, (uint8_t)((u16DataAddr & 0xFF00) >> 8));    /* Write Hi byte address of register */
            break;

        case 0x20:                                                      /* Slave Address NACK */
        case 0x30:                                                      /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            if (u8Addr)
            {
                I2C_SET_DATA(i2c, (uint8_t)(u16DataAddr & 0xFF));       /* Write Lo byte address of register */
                u8Addr = 0;
            }
            else
                u8Ctrl = I2C_CTL_STA_SI;                              /* Clear SI and send repeat START */

            break;

        case 0x10:
            I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));             /* Write SLA+R to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */
            break;

        case 0x40:                                                      /* Slave Address ACK */
            u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */
            break;

        case 0x48:                                                      /* Slave Address NACK */
            u8Ctrl = I2C_CTL_STO_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x58:
            u8rData = (unsigned char) I2C_GET_DATA(i2c);                  /* Receive Data */
            u8Ctrl = I2C_CTL_STO_SI;                                  /* Clear SI and send STOP */
            u8Xfering = 0;
            break;

        case 0x38:                                                      /* Arbitration Lost */
        default:                                                        /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                   /* Write controlbit to I2C_CTL register */
    }

    if (u8Err)
        u8rData = 0;                                                          /* If occurs error, return 0 */

    return u8rData;                                                           /* Return read data */
}

/**
  * @brief      Specify two bytes register address and read multi bytes from Slave
  *
  * @param[in]  *i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u16DataAddr     Specify a address (2 bytes) of data read from
  * @param[out] *pu8rData       Point to array to store data from Slave
  * @param[in]  u32rLen         How many bytes need to read from Slave
  *
  * @return     A length of how many bytes have been received
  *
  * @details    The function is used for I2C Master specify two bytes address that multi data bytes read from Slave.
  *
  *
  */
uint32_t I2C_ReadMultiBytesTwoRegs(I2C_T *i2c, uint8_t u8SlaveAddr, uint16_t u16DataAddr, uint8_t *pu8rData, uint32_t u32rLen)
{
    uint8_t u8Xfering = 1, u8Err = 0, u8Addr = 1, u8Ctrl = 0;
    uint32_t u32rxLen = 0;

    I2C_START(i2c);                                                         /* Send START */

    while (u8Xfering && (u8Err == 0))
    {
        I2C_WAIT_READY(i2c);

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (u8SlaveAddr << 1 | 0x00));               /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */
            break;

        case 0x18:                                                      /* Slave Address ACK */
            I2C_SET_DATA(i2c, (uint8_t)((u16DataAddr & 0xFF00) >> 8));    /* Write Hi byte address of register */
            break;

        case 0x20:                                                      /* Slave Address NACK */
        case 0x30:                                                      /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x28:
            if (u8Addr)
            {
                I2C_SET_DATA(i2c, (uint8_t)(u16DataAddr & 0xFF));       /* Write Lo byte address of register */
                u8Addr = 0;
            }
            else
                u8Ctrl = I2C_CTL_STA_SI;                              /* Clear SI and send repeat START */

            break;

        case 0x10:
            I2C_SET_DATA(i2c, ((u8SlaveAddr << 1) | 0x01));             /* Write SLA+R to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                                      /* Clear SI */
            break;

        case 0x40:                                                      /* Slave Address ACK */
            u8Ctrl = I2C_CTL_SI_AA;                                   /* Clear SI and set ACK */
            break;

        case 0x48:                                                      /* Slave Address NACK */
            u8Ctrl = I2C_CTL_STO_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;

        case 0x50:
            pu8rData[u32rxLen++] = (unsigned char) I2C_GET_DATA(i2c);      /* Receive Data */

            if (u32rxLen < (u32rLen - 1))
                u8Ctrl = I2C_CTL_SI_AA;                               /* Clear SI and set ACK */
            else
                u8Ctrl = I2C_CTL_SI;                                  /* Clear SI */

            break;

        case 0x58:
            pu8rData[u32rxLen++] = (unsigned char) I2C_GET_DATA(i2c);      /* Receive Data */
            u8Ctrl = I2C_CTL_STO_SI;                                  /* Clear SI and send STOP */
            u8Xfering = 0;
            break;

        case 0x38:                                                      /* Arbitration Lost */
        default:                                                        /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;                                  /* Clear SI and send STOP */
            u8Err = 1;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                                   /* Write controlbit to I2C_CTL register */
    }

    return u32rxLen;                                                        /* Return bytes length that have been received */
}

/*@}*/ /* end of group I2C_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group I2C_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
