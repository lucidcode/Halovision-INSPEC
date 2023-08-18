/**
 @file

 @brief
 SD card host controller


 **/
/*
 * ============================================================================
 * History
 * =======
 *
 * Copyright (C) Bridgetek Pte Ltd
 * ============================================================================
 *
 * This source code ("the Software") is provided by Future Technology Devices
 * International Limited ("Bridgetek") subject to the licence terms set out
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
#include <stdbool.h>

#include "ft900_sdhost.h"
#include "ft900_delay.h"
#include "ft900.h"


// Enable to debug. Note that Debugging relies on tinyprintf
#undef SD_HOST_DEBUG
#ifdef SD_HOST_DEBUG
#include "tinyprintf.h"

void AnalyzeR1Status(uint32_t response_r1);
void RegisterDump(void);
#endif // SD_HOST_DEBUG

/* CONSTANTS ***********************************************************************/

/* SD Card commands */

enum sd_command {
	SDCMD_GO_IDLE_STATE = 0,
	SDCMD_ALL_SEND_CID = 2,
	SDCMD_SEND_RELATIVE_ADDR = 3,
	SDCMD_SDIO = 5,
	SDCMD_SET_BUS_WIDTH = 6,
	SDCMD_SEL_DESEL_CARD = 7,
	SDCMD_SEND_IF_COND = 8,
	SDCMD_SEND_CSD = 9,
	SDCMD_STOP_TRANSMISSION = 12,
	SDCMD_READ_STATUS_REG = 13,
	SDCMD_SEND_BLOCKLEN = 16,
	SDCMD_SEND_OP_CON = 41,
	SDCMD_APP_CMD = 55
};

/*
 *  BLOCK sizes
 */
#define SDHOST_BLK_SIZE_POW				9
#define SDCARD_SDSTATUS_BLK_SIZE	   64  // The status register is 512 BITS in size

/*
 *  SD host registers.
 * 
 *  These constants are used to identify the individual sdhost registers, whatever their size.
 *  These are used to symbolically address registers via the functions read_sdhost_reg()
 *  and write_sdhost_reg().
 * 
 *  This is all necessary due to the fact we can only access the reigsters with 32bit values.
 */

/** @brief Auto CMD23 Argument 2 Register */
#define SDH_AUTO_CMD23_ARG2                  1    
/** @brief Block Size Register */
#define SDH_BLK_SIZE                         2    
/** @brief Block Count Register */
#define SDH_BLK_COUNT                        3    
/** @brief Argument 1 Register */
#define SDH_ARG_1                            4    
/** @brief Transfer Mode Register */
#define SDH_TNSFER_MODE                      5    
/** @brief Command Register */
#define SDH_CMD                              6    
/** @brief Response Registers 0 */
#define SDH_RESPONSE0                        7    
/** @brief Response Registers 1 */
#define SDH_RESPONSE1                        8    
/** @brief Response Registers 2 */
#define SDH_RESPONSE2                        9    
/** @brief Response Registers 3 */
#define SDH_RESPONSE3                       10    
/** @brief Buffer Data Port Register */
#define SDH_BUF_DATA                        11    
/** @brief Present State Register */
#define SDH_PRESENT_STATE                   12    
/** @brief Host Control 1 Register */
#define SDH_HST_CNTL_1                      13    
/** @brief Power Control Register */
#define SDH_PWR_CNTL                        14    
/** @brief Block Gap Control Register */
#define SDH_BLK_GAP_CNTL                    15    
/** @brief Clock Control Register */
#define SDH_CLK_CNTL                        16    
/** @brief Timeout Control Register */
#define SDH_TIMEOUT_CNTL                    17    
/** @brief Software Reset Register */
#define SDH_SW_RST                          18    
/** @brief Normal Interrupt Status Register */
#define SDH_NRML_INT_STATUS                 19    
/** @brief Error Interrupt Status Register */
#define SDH_ERR_INT_STATUS                  20    
/** @brief Normal Interrupt Status Enable Register */
#define SDH_NRML_INT_STATUS_ENABLE          21    
/** @brief Error Interrupt Status Enable Register */
#define SDH_ERR_INT_STATUS_ENABLE           22    
/** @brief Normal Interrupt Signal Enable Register */
#define SDH_NRML_INT_SGNL_ENABLE            23    
/** @brief Error Interrupt Signal Enable Register */
#define SDH_ERR_INT_SGNL_ENABLE             24    
/** @brief Auto CMD12 Error Status Register */
#define SDH_AUTO_CMD12_ERR_STATUS           25    
/** @brief Host Control 2 Register */
#define SDH_HST_CNTL_2                      26    
/** @brief Capabilities Register 1 */
#define SDH_CAP_1                           27    
/** @brief Capabilities Register 2 */
#define SDH_CAP_2                           28    
/** @brief Reserved Register 1 */
#define SDH_RSRV_1                          29    
/** @brief Reserved Register 2 */
#define SDH_RSRV_2                          30    
/** @brief Force Event Register for Auto CMD12 Error Status */
#define SDH_FORCE_EVENT_CMD_ERR_STATUS      31    
/** @brief Force Event Register for Error Interrupt Status */
#define SDH_FORCE_EVENT_ERR_INT_STATUS      32    
/** @brief Reserved Register 3 */
#define SDH_RSRV_3                          33    
/** @brief Reserved register 4 */
#define SDH_RSRV_4                          34    
/** @brief Preset Value Register 3 */
#define SDH_PRST_INIT                       35    
/** @brief Preset Value Register 3 */
#define SDH_PRST_DFLT_SPD                   36    
/** @brief Preset Value Register 3 */
#define SDH_PRST_HIGH_SPD                   37    
/** @brief Preset Value Register 3 */
#define SDH_PRST_SDR12                      38    
/** @brief Vendor-defined Register 0 */
#define SDH_VNDR_0                          39    
/** @brief Vendor-defined Register 1 */
#define SDH_VNDR_1                          40    
/** @brief Vendor-defined Register 5 */
#define SDH_VNDR_5                          41    

/*
 *  FT900 32 bit registers
 *  These are used to provide the actual 32bit address of the sdhost registers
 *  These are named arbitrarily
 * 
 */
#if defined(__FT900__)
/* Block Size Register & Block Count Register */
#define SDHOST_BS_BC                          ((volatile uint32_t *)0x10404)
/* Argument 1 Register */
#define SDHOST_ARG1                           ((volatile uint32_t *)0x10408)
/* Transfer Mode Register & Command Register */
#define SDHOST_TM_CMD                         ((volatile uint32_t *)0x1040c)
/* Response Register 0 */
#define SDHOST_RESPONSE0                      ((volatile uint32_t *)0x10410)
/* Response Register 1 */
#define SDHOST_RESPONSE1                      ((volatile uint32_t *)0x10414)
/* Response Register 2 */
#define SDHOST_RESPONSE2                      ((volatile uint32_t *)0x10418)
/* Response Register 3 */
#define SDHOST_RESPONSE3                      ((volatile uint32_t *)0x1041c)
/* Buffer Data Port Register */
#define SDHOST_BUFDATAPORT                    ((volatile uint32_t *)0x10420)
/* Present State Register */
#define SDHOST_PRESENTSTATE                   ((volatile uint32_t *)0x10424)
/* Host Control 1 Register & Power Control Register & Block Gap Control Register */
#define SDHOST_HOSTCTRL1_PWR_BLKGAP           ((volatile uint32_t *)0x10428)
/* Clock Control Register & Timeout Control Register & Software Reset Register */
#define SDHOST_CLK_TIMEOUT_SWRESET            ((volatile uint32_t *)0x1042c)
/* Normal Interrupt Status Register & Error Interrupt Status Register */
#define SDHOST_NORMINTSTATUS_ERRINTSTATUS     ((volatile uint32_t *)0x10430)
/* Normal Interrupt Status Enable Reg. & Error Interrupt Status Enable Reg. */
#define SDHOST_NORMINTSTATUSEN_ERRINTSTATUSEN ((volatile uint32_t *)0x10434)
/* Normal Interrupt Signal Enable Reg. & Error Interrupt Signal Enable Reg. */
#define SDHOST_NORMINTSIGEN_ERRINTSIGEN       ((volatile uint32_t *)0x10438)
/* Auto CMD12 Error Status Register & Host Control 2 Register */
#define SDHOST_AUTOCMD12_HOSTCTRL2            ((volatile uint32_t *)0x1043c)
/* Capabilities Register 0 */
#define SDHOST_CAP0                           ((volatile uint32_t *)0x10440)
/* Capabilities Register 1 */
#define SDHOST_CAP1                           ((volatile uint32_t *)0x10444)
/* Maximum Current Capabilities Register 0 */
#define SDHOST_MAXCURRENTCAP0                 ((volatile uint32_t *)0x10448)
/* Maximum Current Capabilities Register 1 */
#define SDHOST_MAXCURRENTCAP1                 ((volatile uint32_t *)0x1044c)
/* Force Event Reg. for Auto CMD12 Error Status & Force Event Reg. for Error Interrupt Status */
#define SDHOST_FORCEEVENTCMD12_FORCEEVENTERRINT ((volatile uint32_t *)0x10450)
/* ADMA Error Status Register */
#define SDHOST_ADMAERRORSTATUS                ((volatile uint32_t *)0x10454)
/* ADMA System Address Register */
#define SDHOST_ADMASYSADDR                    ((volatile uint32_t *)0x10458)
/* Preset Value Register 0 */
#define SDHOST_PRESETVALUE0                   ((volatile uint32_t *)0x10460)
/* Preset Value Register 1 */
#define SDHOST_PRESETVALUE1                   ((volatile uint32_t *)0x10464)
/* Preset Value Register 2 */
#define SDHOST_PRESETVALUE2                   ((volatile uint32_t *)0x10468)
/* Preset Value Register 3 */
#define SDHOST_PRESETVALUE3                   ((volatile uint32_t *)0x1046c)
/* Vendor-defined Register 0 */
#define SDHOST_VENDOR0                        ((volatile uint32_t *)0x10500)
/* Vendor-defined Register 1 */
#define SDHOST_VENDOR1                        ((volatile uint32_t *)0x10504)
/* Vendor-defined Register 5 */
#define SDHOST_VENDOR5                        ((volatile uint32_t *)0x10514)

#elif defined(__FT930__)
/* Block Size Register & Block Count Register */
#define SDHOST_BS_BC                          ((volatile uint32_t *)0x10604)
/* Argument 1 Register */
#define SDHOST_ARG1                           ((volatile uint32_t *)0x10608)
/* Transfer Mode Register & Command Register */
#define SDHOST_TM_CMD                         ((volatile uint32_t *)0x1060c)
/* Response Register 0 */
#define SDHOST_RESPONSE0                      ((volatile uint32_t *)0x10610)
/* Response Register 1 */
#define SDHOST_RESPONSE1                      ((volatile uint32_t *)0x10614)
/* Response Register 2 */
#define SDHOST_RESPONSE2                      ((volatile uint32_t *)0x10618)
/* Response Register 3 */
#define SDHOST_RESPONSE3                      ((volatile uint32_t *)0x1061c)
/* Buffer Data Port Register */
#define SDHOST_BUFDATAPORT                    ((volatile uint32_t *)0x10620)
/* Present State Register */
#define SDHOST_PRESENTSTATE                   ((volatile uint32_t *)0x10624)
/* Host Control 1 Register & Power Control Register & Block Gap Control Register */
#define SDHOST_HOSTCTRL1_PWR_BLKGAP           ((volatile uint32_t *)0x10628)
/* Clock Control Register & Timeout Control Register & Software Reset Register */
#define SDHOST_CLK_TIMEOUT_SWRESET            ((volatile uint32_t *)0x1062c)
/* Normal Interrupt Status Register & Error Interrupt Status Register */
#define SDHOST_NORMINTSTATUS_ERRINTSTATUS     ((volatile uint32_t *)0x10630)
/* Normal Interrupt Status Enable Reg. & Error Interrupt Status Enable Reg. */
#define SDHOST_NORMINTSTATUSEN_ERRINTSTATUSEN ((volatile uint32_t *)0x10634)
/* Normal Interrupt Signal Enable Reg. & Error Interrupt Signal Enable Reg. */
#define SDHOST_NORMINTSIGEN_ERRINTSIGEN       ((volatile uint32_t *)0x10638)
/* Auto CMD12 Error Status Register & Host Control 2 Register */
#define SDHOST_AUTOCMD12_HOSTCTRL2            ((volatile uint32_t *)0x1063c)
/* Capabilities Register 0 */
#define SDHOST_CAP0                           ((volatile uint32_t *)0x10640)
/* Capabilities Register 1 */
#define SDHOST_CAP1                           ((volatile uint32_t *)0x10644)
/* Maximum Current Capabilities Register 0 */
#define SDHOST_MAXCURRENTCAP0                 ((volatile uint32_t *)0x10648)
/* Maximum Current Capabilities Register 1 */
#define SDHOST_MAXCURRENTCAP1                 ((volatile uint32_t *)0x1064c)
/* Force Event Reg. for Auto CMD12 Error Status & Force Event Reg. for Error Interrupt Status */
#define SDHOST_FORCEEVENTCMD12_FORCEEVENTERRINT ((volatile uint32_t *)0x10650)
/* ADMA Error Status Register */
#define SDHOST_ADMAERRORSTATUS                ((volatile uint32_t *)0x10654)
/* ADMA System Address Register */
#define SDHOST_ADMASYSADDR                    ((volatile uint32_t *)0x10658)
/* Preset Value Register 0 */
#define SDHOST_PRESETVALUE0                   ((volatile uint32_t *)0x10660)
/* Preset Value Register 1 */
#define SDHOST_PRESETVALUE1                   ((volatile uint32_t *)0x10664)
/* Preset Value Register 2 */
#define SDHOST_PRESETVALUE2                   ((volatile uint32_t *)0x10668)
/* Preset Value Register 3 */
#define SDHOST_PRESETVALUE3                   ((volatile uint32_t *)0x1066c)
/* Vendor-defined Register 0 */
#define SDHOST_VENDOR0                        ((volatile uint32_t *)0x10700)
/* Vendor-defined Register 1 */
#define SDHOST_VENDOR1                        ((volatile uint32_t *)0x10704)
/* Vendor-defined Register 5 */
#define SDHOST_VENDOR5                        ((volatile uint32_t *)0x10714)

#else

#error -- processor type not defined

#endif /* __FT900__ */

/*
 *  SDIO special operations, to be used only when CMD52 is sent
 */
#define NOT_IN_USE                          0
#define SDIO_WRITE_CCCR_BUS_SUSPEND         1
#define SDIO_WRITE_CCCR_FUNCTION_SELECT     2
#define SDIO_WRITE_CCCR_IO_ABORT            3

/*
 *  bits defined in individual registers
 */

/* CLK_CTRL bits */
#define CLK_CTRL_CLK_STABLE                 0x0002

/* SW_RESET bits */
#define SW_RESET_DATA_LINE                  0x04
#define SW_RESET_CMD_LINE                   0x02
#define SW_RESET_ALL                        0x01

/* NORM_INT_STATUS bits */
#define NORM_INT_STATUS_CARD_RMV            0x00000080
#define NORM_INT_STATUS_CARD_INS            0x00000040
#define NORM_INT_STATUS_BUF_R_RDY           0x00000020
#define NORM_INT_STATUS_BUF_W_RDY           0x00000010
#define NORM_INT_STATUS_TRANSFER_COMPLETE   0x00000002
#define NORM_INT_STATUS_CMD_COMPLETE        0x00000001

/* ERR_INT_STATUS bits */
#define ERR_INT_STATUS_TUNING_ERR           0x00000400
#define ERR_INT_STATUS_ADMA_ERR             0x00000200
#define ERR_INT_STATUS_AUTO_CMD12_ERR       0x00000100
#define ERR_INT_STATUS_CUR_LIM_ERR          0x00000080
#define ERR_INT_STATUS_DATA_END_BIT_ERR     0x00000040
#define ERR_INT_STATUS_DATA_CRC_ERR         0x00000020
#define ERR_INT_STATUS_DATA_TIMEOUT_ERR     0x00000010
#define ERR_INT_STATUS_CMD_IDX_ERR          0x00000008
#define ERR_INT_STATUS_CMD_END_BIT_ERR      0x00000004
#define ERR_INT_STATUS_CMD_CRC_ERR          0x00000002
#define ERR_INT_STATUS_CMD_TIMEOUT_ERR      0x00000001

/* SDHOST_PRESENTSTATE bits */
#define PRESENT_STATE_SYS_CARD_INST         0x00010000
#define PRESENT_STATE_BUF_RD_EN             0x00000800
#define PRESENT_STATE_BUF_WR_EN             0x00000400
#define PRESENT_STATE_RD_TRAN_ACT           0x00000200
#define PRESENT_STATE_WR_TRAN_ACT           0x00000100
#define PRESENT_STATE_DATA_LIN_ACT          0x00000004
#define PRESENT_STATE_CMD_INHIBIT_D         0x00000002
#define PRESENT_STATE_CMD_INHIBIT_C         0x00000001
#define PRESENT_STATE_SYS_CARD_STABLE       0x00020000

/* The combination of response R1 error bits, used for examining error during CMD transfer */
#define RESPONSE_R1_ERROR_MASK              0xFDF90008
#define RESPONSE_R6_ERROR_MASK              0X0000E008

/* Transfer type (single / multiple-block) */
#define SDHOST_SINGLE_BLK                   0x0000
#define SDHOST_MULTI_BLK                    0x0020

/* DMA enable / disable */
#define SDHOST_DMA_DISABLED                 0x0000
#define SDHOST_DMA_ENABLED                  0x0001

/* BLK_COUNT enable / disable */
#define SDHOST_BLK_COUNT_DISABLED           0x0000
#define SDHOST_BLK_COUNT_ENABLED            0x0002

/* Auto CMD12 enable / disable */
#define SDHOST_AUTO_CMD12_DISABLED          0x0000
#define SDHOST_AUTO_CMD12_ENABLED           0x0004 /* Multiple block transfers for memory only */

/* Command types, specific to the Command Register */
#define CMD_TYPE_NORMAL                     0x00    /* All other commands */
#define CMD_TYPE_SUSPEND                    0x40    /* CMD52 for writing Bus Suspend in CCCR */
#define CMD_TYPE_RESUME                     0x80    /* CMD52 for writing Function Select in CCCR */
#define CMD_TYPE_ABORT                      0xC0    /* CMD12, CMD52 for writing I/O Abort in CCCR */

/* Data present select - bit 5 of the Command Register */
#define CMD_NO_DATA_PRESENT                 0x00
#define CMD_DATA_PRESENT                    0x20

/*
 *  Response types - a combination of Command Index Check Enable bit, Command CRC Check Enable bit
 *  and Response Type Select bit in the Command Register
 * 
 *  Refer to SD Host Controller Simplified Spec document para 2.2.7 response register response */

#define CMD_RESPONSE_NONE                               0x00
#define CMD_RESPONSE_R1                                 0x1A
#define CMD_RESPONSE_R1b                                0x1B
#define CMD_RESPONSE_R2                                 0x09
#define CMD_RESPONSE_R3                                 0x02
#define CMD_RESPONSE_R4                                 0x02    /* SDIO only */
#define CMD_RESPONSE_R5                                 0x1A    /* SDIO only */
#define CMD_RESPONSE_R5b                                0x1B    /* SDIO only */
#define CMD_RESPONSE_R6                                 0x1A
#define CMD_RESPONSE_R7                                 0x1A


/**
 * @brief  Mask for errors Card Status R1 (OCR Register)
 */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR                 ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET              ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS                ((uint32_t)0xFDFFE008)

typedef enum
{
	/**
	 * @brief  SDIO specific error defines
	 */
	SD_CMD_CRC_FAIL = (1), /*!< Command response received (but CRC check failed) */
	SD_DATA_CRC_FAIL = (2), /*!< Data bock sent/received (CRC check Failed) */
	SD_CMD_RSP_TIMEOUT = (3), /*!< Command response timeout */
	SD_DATA_TIMEOUT = (4), /*!< Data time out */
	SD_TX_UNDERRUN = (5), /*!< Transmit FIFO under-run */
	SD_RX_OVERRUN = (6), /*!< Receive FIFO over-run */
	SD_START_BIT_ERR = (7), /*!< Start bit not detected on all data signals in widE bus mode */
	SD_CMD_OUT_OF_RANGE = (8), /*!< CMD's argument was out of range.*/
	SD_ADDR_MISALIGNED = (9), /*!< Misaligned address */
	SD_BLOCK_LEN_ERR = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
	SD_ERASE_SEQ_ERR = (11), /*!< An error in the sequence of erase command occurs.*/
	SD_BAD_ERASE_PARAM = (12), /*!< An Invalid selection for erase groups */
	SD_WRITE_PROT_VIOLATION = (13), /*!< Attempt to program a write protect block */
	SD_LOCK_UNLOCK_FAILED = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
	SD_COM_CRC_FAILED = (15), /*!< CRC check of the previous command failed */
	SD_ILLEGAL_CMD = (16), /*!< Command is not legal for the card state */
	SD_CARD_ECC_FAILED = (17), /*!< Card internal ECC was applied but failed to correct the data */
	SD_CC_ERROR = (18), /*!< Internal card controller error */
	SD_GENERAL_UNKNOWN_ERROR = (19), /*!< General or Unknown error */
	SD_STREAM_READ_UNDERRUN = (20), /*!< The card could not sustain data transfer in stream read operation. */
	SD_STREAM_WRITE_OVERRUN = (21), /*!< The card could not sustain data programming in stream mode */
	SD_CID_CSD_OVERWRITE = (22), /*!< CID/CSD overwrite error */
	SD_WP_ERASE_SKIP = (23), /*!< only partial address space was erased */
	SD_CARD_ECC_DISABLED = (24), /*!< Command has been executed without using internal ECC */
	SD_ERASE_RESET = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
	SD_AKE_SEQ_ERROR = (26), /*!< Error in sequence of authentication. */
	SD_INVALID_VOLTRANGE = (27),
	SD_ADDR_OUT_OF_RANGE = (28),
	SD_SWITCH_ERROR = (29),
	SD_SDIO_DISABLED = (30),
	SD_SDIO_FUNCTION_BUSY = (31),
	SD_SDIO_FUNCTION_FAILED = (32),
	SD_SDIO_UNKNOWN_FUNCTION = (33),

	/**
	 * @brief  Standard error defines
	 */
	SD_INTERNAL_ERROR,
	SD_NOT_CONFIGURED,
	SD_REQUEST_PENDING,
	SD_REQUEST_NOT_APPLICABLE,
	SD_INVALID_PARAMETER,
	SD_UNSUPPORTED_FEATURE,
	SD_UNSUPPORTED_HW,
	SD_ERROR,
	SD_OK = 0
} SD_Error;

/*
 *  sdhost_cmd_t specifies the command type to send to the sdhost hardware
 *
 */

typedef enum {
    SDHOST_BUS_CMD,
    SDHOST_APP_SPECIFIC_CMD
} sdhost_cmd_t;

/*
 *  sdhost_response_t specifies the response from the sdhost hardware
 */

typedef enum {
    SDHOST_RESPONSE_NONE,
    SDHOST_RESPONSE_R1,			/* normal respose */
    SDHOST_RESPONSE_R1b,		/* normal respose */
    SDHOST_RESPONSE_R2,                 /* CID, CSD register */
    SDHOST_RESPONSE_R3,                 /* OCR register (memory) */
    SDHOST_RESPONSE_R4,                 /* OCR register (i/o etc. ) */
    SDHOST_RESPONSE_R5,                 /* SDIO response */
    SDHOST_RESPONSE_R5b,                /* SDIO response */
    SDHOST_RESPONSE_R6,                 /* RCA response */
    SDHOST_RESPONSE_R7			/* SEND_IF_COND response */
} sdhost_response_t;


#define COMM_TIMEOUT_U32		(uint32_t)(10000000) // A simple timeout (at least 100mS @ 100MHz)
/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

static sdhost_context_t sdhost_context = {
		.internalStatus = SDHOST_ERROR // Initialise internalStatus member with error response.
};

static SDHOST_STATUS sdhost_initialisation_status = SDHOST_NOT_INITIALISED;

#ifdef __USE_SDHOST_INTERRUPT_
volatile static int card_status_changed = false;
volatile static int cmd_complete = false;
volatile static int transfer_complete = false;
volatile static int write_buf_ready = false;
volatile static int read_buf_ready = false;
volatile static int errRecoverable;
volatile static uint16_t errStatus;
#endif

/* MACROS **************************************************************************/

/* ft900 gpio registers */

#if defined(__FT930__)
/* function 2 configuration */
#define SYS_REGPAD00             ((volatile uint8_t  *)0x1001C) /*SD CLK*/
#define SYS_REGPAD01             ((volatile uint8_t  *)0x1001D) /*SD CMD*/
#define SYS_REGPAD02             ((volatile uint8_t  *)0x1001E) /*SD CD*/
#define SYS_REGPAD03             ((volatile uint8_t  *)0x1001F) /*SD DAT0*/
#define SYS_REGPAD04             ((volatile uint8_t  *)0x10020) /*SD DAT1*/
#define SYS_REGPAD05             ((volatile uint8_t  *)0x10021) /*SD DAT2*/
#define SYS_REGPAD06             ((volatile uint8_t  *)0x10022) /*SD DAT3*/
#define SYS_REGPAD07             ((volatile uint8_t  *)0x10023) /*SD WP*/
#else
/* function 1 configuration */
#define SYS_REGPAD19             ((volatile uint8_t  *)0x1002f) /*SD CLK*/
#define SYS_REGPAD20             ((volatile uint8_t  *)0x10030) /*SD CMD*/
#define SYS_REGPAD21             ((volatile uint8_t  *)0x10031) /*SD DAT3*/
#define SYS_REGPAD22             ((volatile uint8_t  *)0x10032) /*SD DAT2*/
#define SYS_REGPAD23             ((volatile uint8_t  *)0x10033) /*SD DAT1*/
#define SYS_REGPAD24             ((volatile uint8_t  *)0x10034) /*SD DAT0*/
#define SYS_REGPAD25             ((volatile uint8_t  *)0x10035) /*SD CD*/
#define SYS_REGPAD26             ((volatile uint8_t  *)0x10036) /*SD WP*/
#endif

#define SYS_REGCLKCFG            ((volatile uint32_t *)0x10008)

#ifdef SD_HOST_DEBUG
#define DBG_TIMEOUT()			 if(timeout == 0) {tfp_printf("Timeout ERROR @"); tfp_printf(__FILE__); tfp_printf("%d", __LINE__); RegisterDump();}
#else // SD_HOST_DEBUG

#define DBG_TIMEOUT()
#endif // SD_HOST_DEBUG

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static uint32_t read_sdhost_reg(uint32_t addr);
static void write_sdhost_reg(uint32_t val, uint32_t addr);

#ifdef __USE_SDHOST_INTERRUPT_
/* SD Host ISR */
extern void attach_interrupt(int, void (*func)(void));
void sdhost_isr(void);
#endif

/* FUNCTIONS ***********************************************************************/

sdhost_context_t* sdhost_get_context(void) {
	if (sdhost_initialisation_status != SDHOST_OK)
		return NULL;
	return (&sdhost_context);
}

#ifdef __USE_SDHOST_INTERRUPT_

/** @brief SD Host Interrupt Service Routine
 *  @param none *
 *  @returns none
 */

void sdhost_isr(void)
{
	uint16_t normIntStatus = (uint16_t) read_sdhost_reg(NORM_INT_STATUS);
	uint16_t errIntStatus = (uint16_t) read_sdhost_reg(SDH_ERR_INT_STATUS);

	/* Check if interrupt is due to card detection */
	if (normIntStatus & NORM_INT_STATUS_CARD_INS)
	{
		/* Clear interrupt flag by writing 1 to the flag bit */
		write_sdhost_reg(NORM_INT_STATUS_CARD_INS, NORM_INT_STATUS);

		/* Signal the waiting function */
		card_status_changed = true;
	}

	if (normIntStatus & NORM_INT_STATUS_CARD_RMV)
	{
		/* Clear interrupt flag  by writing 1 to the flag bit */
		write_sdhost_reg(NORM_INT_STATUS_CARD_RMV, NORM_INT_STATUS);

		/* Signal the waiting function */
		card_status_changed = true;
	}

	if (normIntStatus & NORM_INT_STATUS_CMD_COMPLETE)
	{
		/* Clear interrupt flag  by writing 1 to the flag bit */
		write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, NORM_INT_STATUS);

		/* Signal the waiting function */
		cmd_complete = true;
	}

	if (normIntStatus & NORM_INT_STATUS_TRANSFER_COMPLETE)
	{
		/* Clear interrupt flag  by writing 1 to the flag bit */
		write_sdhost_reg(NORM_INT_STATUS_TRANSFER_COMPLETE, NORM_INT_STATUS);

		/* Signal the waiting function */
		transfer_complete = true;
	}

	if (normIntStatus & NORM_INT_STATUS_BUF_W_RDY)
	{
		/* Clear interrupt flag  by writing 1 to the flag bit */
		write_sdhost_reg(NORM_INT_STATUS_BUF_W_RDY, NORM_INT_STATUS);

		/* Signal the waiting function */
		write_buf_ready = true;
	}

	if (normIntStatus & NORM_INT_STATUS_BUF_R_RDY)
	{
		/* Clear interrupt flag  by writing 1 to the flag bit */
		write_sdhost_reg(NORM_INT_STATUS_BUF_R_RDY, NORM_INT_STATUS);

		/* Signal the waiting function */
		read_buf_ready = true;
	}

	/* If an error interrupt happens, start the error recovery sequence */

	if (errIntStatus & 0x07FF)
	{

		/* Disable Error Interrupt Signal */
		write_sdhost_reg(~(errIntStatus), ERR_INT_SIG_EN);

		/*
		 *  Check bits [3:0] of the Error Interrupt Status register
		 *  If one of bits [3:0] are set, namely CMD Line Error occurs,
		 *  set soft_rst_cmd to 1 in the Software Reset register
		 */

		if (errIntStatus & 0x000F)
		{
			do {
				/*
				 *  Check soft_rst_cmd in the Software Reset register.
				 *  If 0, continue.
				 *   If 1, set soft_rst_cmd to 1 in the Software Reset register again.
				 */
				write_sdhost_reg(SW_RESET_CMD_LINE, SDH_SW_RST);
			}while (read_sdhost_reg(SDH_SW_RST) & SW_RESET_CMD_LINE);
		}

		/*
		 *  Check bits [6:4] in the Error Interrupt Status register
		 *  If one of bits[6:4] are set, namely DAT Line Error occurs,
		 *  set soft_rst_dat to 1 in the Software Reset register.
		 */

		if (errIntStatus & 0x0070)
		{
			do {
				write_sdhost_reg(SW_RESET_DATA_LINE, SDH_SW_RST);

				/*
				 *  Check soft_rst_dat in the Software Reset register.
				 *  If 0, continue.
				 *  If 1, set soft_rst_dat to 1 in the Software Reset register again.
				 */
			}while (read_sdhost_reg(SDH_SW_RST) & SW_RESET_DATA_LINE);
		}

		/* Save previous error status. */
		errStatus = errIntStatus;

		/* Clear previous error status. */
		write_sdhost_reg(errIntStatus, SDH_ERR_INT_STATUS);

		/* Enable the Error Interrupt Signal */
		write_sdhost_reg(read_sdhost_reg(ERR_INT_SIG_EN) | errStatus, ERR_INT_SIG_EN);

	}

	return;
}

#else


/** @brief Recover from SD Host errors
 *  @param none
 *  @returns none
 */

static void sdhost_error_recovery(void) {
	uint16_t errIntStatus = (uint16_t) read_sdhost_reg(SDH_ERR_INT_STATUS);
	uint32_t timeout;
	if (errIntStatus & 0x07FF) {
		/* Disable the Error Interrupt Signal */
		write_sdhost_reg(~(errIntStatus), SDH_ERR_INT_SGNL_ENABLE);

		/*
		 *  Check bits [3:0] in the Error Interrupt Status register
		 *  If one of bits [3:0] is set, namely CMD Line Error occurs,
		 *  set soft_rst_cmd to 1 in the Software Reset register
		 */

		if (errIntStatus & 0x000F) {
			/*
			 *  Check soft_rst_cmd in the Software Reset register.
			 *  If 0, continue. If 1, set soft_rst_cmd to 1 in the Software Reset register again.
			 */
			timeout = COMM_TIMEOUT_U32;
			do {
				write_sdhost_reg(SW_RESET_CMD_LINE, SDH_SW_RST);
				// Note we need a delay here else the write and read are too fast for SD Controller to react when compiled with Os flag
				// See redmine bug 402 - http://glared1.ftdi.local/issues/402
				delayus(1);
			} while ((read_sdhost_reg(SDH_SW_RST) & SW_RESET_CMD_LINE)
					&& (--timeout != 0));
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				return;
		}
		}

		/*
		 *  Check bits [6:4] in the Error Interrupt Status register
		 *  If one of bits[6:4] is set, namely DAT Line Error occurs,
		 *  set soft_rst_dat to 1 in the Software Reset register
		 */

		if (errIntStatus & 0x0070) {
			/*
			 *  Check soft_rst_dat in the Software Reset register.
			 *  If 0, continue. If 1, set soft_rst_dat to 1 in the Software Reset register again.
			 */
			timeout = COMM_TIMEOUT_U32;
			do {
				write_sdhost_reg(SW_RESET_DATA_LINE, SDH_SW_RST);
				// Note we need a delay here else the write and read are too fast for SD Controller to react when compiled with Os flag
				// See redmine bug 402 - http://glared1.ftdi.local/issues/402
				delayus(1);
			} while ((read_sdhost_reg(SDH_SW_RST) & SW_RESET_DATA_LINE)
					&& (--timeout != 0));
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				return;
		}
		}

		/* Clear previous error status by setting them to 1 */
		write_sdhost_reg(errIntStatus, SDH_ERR_INT_STATUS);

		/* Enable the Error Interrupt Signal */
		write_sdhost_reg(
				read_sdhost_reg(SDH_ERR_INT_SGNL_ENABLE) | errIntStatus,
				SDH_ERR_INT_SGNL_ENABLE);
	}

	return;
}
#endif

/** @brief Initialise system registers to enable SD Host peripheral
 *  @param none
 *  @returns none
 */

void sdhost_sys_init(void) {
#if defined(__FT930__)
	/* Enable pad for SD Host (bit [7:6]). Must pull up (bit [3]) as stated in specs... */
	*SYS_REGPAD00 = 0x80; /* CLK, none */
	*SYS_REGPAD01 = 0x80; /* CMD, none */
	*SYS_REGPAD02 = 0x80; /* CD, none */
	*SYS_REGPAD03 = 0x80; /* DATA0, none */
	*SYS_REGPAD04 = 0x80; /* DATA1, none */
	*SYS_REGPAD05 = 0x80; /* DATA2, none */
	*SYS_REGPAD06 = 0x80; /* DATA3, none */
	*SYS_REGPAD07 = 0x80; /* WP, none */
#else
	/* Enable pad for SD Host (bit [7:6]). Must pull up (bit [3]) as stated in specs... */
	*SYS_REGPAD19 = 0x40; /* CLK */
	*SYS_REGPAD20 = 0x40; /* CMD, pulled up */
	*SYS_REGPAD21 = 0x40; /* DATA3, pulled up */
	*SYS_REGPAD22 = 0x40; /* DATA2, pulled up */
	*SYS_REGPAD23 = 0x40; /* DATA1, pulled up */
	*SYS_REGPAD24 = 0x40; /* DATA0, pulled up */
	*SYS_REGPAD25 = 0x40; /* CD, pulled up */
	*SYS_REGPAD26 = 0x40; /* WP, pulled up */
#endif

	/* Enable SD Host function (bit 12) */
    sys_enable(sys_device_sd_card);


	return;
}

/**
 *  @brief Read a register from the SD Host peripheral
 *
 *  Access to the SD host operipheral registers can only be per perfornmed on a 32 bit basis.
 *  read_sdhost_rec() takes care of any masking and shifting when accessing a register
 *  less than 32 bits.
 *
 *  @param addr register id.
 *  @returns the contents of the register requested
 */

static uint32_t read_sdhost_reg(uint32_t addr) {
	switch (addr) {
	case SDH_BLK_SIZE:
		return *SDHOST_BS_BC & 0x0000fff;

	case SDH_BLK_COUNT:
		return *SDHOST_BS_BC >> 16;

	case SDH_TNSFER_MODE:
		return *SDHOST_TM_CMD & 0x0000ffff;

	case SDH_CMD:
		return *SDHOST_TM_CMD >> 16;

	case SDH_RESPONSE0:
		return *SDHOST_RESPONSE0;

	case SDH_RESPONSE1:
		return *SDHOST_RESPONSE1;

	case SDH_RESPONSE2:
		return *SDHOST_RESPONSE2;

	case SDH_RESPONSE3:
		return *SDHOST_RESPONSE3;

	case SDH_BUF_DATA:
		return *SDHOST_BUFDATAPORT;

	case SDH_PRESENT_STATE:
		return *SDHOST_PRESENTSTATE;

	case SDH_HST_CNTL_1:
		return *SDHOST_HOSTCTRL1_PWR_BLKGAP & 0xff;

	case SDH_PWR_CNTL:
		return (*SDHOST_HOSTCTRL1_PWR_BLKGAP >> 8) & 0xff;

	case SDH_CLK_CNTL:
		return *SDHOST_CLK_TIMEOUT_SWRESET & 0xffff;

	case SDH_SW_RST:
		return (*SDHOST_CLK_TIMEOUT_SWRESET >> 24) & 0xff;

	case SDH_NRML_INT_STATUS:
		return *SDHOST_NORMINTSTATUS_ERRINTSTATUS & 0xffff;

	case SDH_ERR_INT_STATUS:
		return *SDHOST_NORMINTSTATUS_ERRINTSTATUS >> 16;

	case SDH_NRML_INT_STATUS_ENABLE:
		return *SDHOST_NORMINTSTATUSEN_ERRINTSTATUSEN & 0xffff;

	case SDH_ERR_INT_STATUS_ENABLE:
		return *SDHOST_NORMINTSTATUSEN_ERRINTSTATUSEN >> 16;

	case SDH_NRML_INT_SGNL_ENABLE:
		return *SDHOST_NORMINTSIGEN_ERRINTSIGEN & 0xffff;

	case SDH_ERR_INT_SGNL_ENABLE:
		return *SDHOST_NORMINTSIGEN_ERRINTSIGEN >> 16;

	case SDH_HST_CNTL_2:
		return *SDHOST_AUTOCMD12_HOSTCTRL2 >> 16;

	case SDH_CAP_1:
		return *SDHOST_CAP0;

	case SDH_CAP_2:
		return *SDHOST_CAP1;

	case SDH_VNDR_0:
		return *SDHOST_VENDOR0;

	case SDH_VNDR_1:
		return *SDHOST_VENDOR1;

	case SDH_VNDR_5:
		return *SDHOST_VENDOR5;

	default:
		return 0;
		/* Invalid address */
	}
}

/** @brief Write value to a SD Host peripheral register
 *
 *  This function is used for writing (any) SD Host registers.
 *  All this is absolutely necessary as we can only accees the SDhost hardware in 32 bit chunks.
 *  This function takes care of the necessary and'ing and or'ing of bits.
 *
 *  @param addr register id.
 *  @param val value to write
 *  @returns the contents of the register requested
 */

static void write_sdhost_reg(uint32_t val, uint32_t addr) {
	switch (addr) {
	case SDH_BLK_SIZE:
		*SDHOST_BS_BC = val | (*SDHOST_BS_BC & 0xffff0000);
		break;

	case SDH_BLK_COUNT:
		*SDHOST_BS_BC = (val << 16) | (*SDHOST_TM_CMD & 0xffff);
		break;

	case SDH_TNSFER_MODE:
		*SDHOST_TM_CMD = val | (*SDHOST_TM_CMD & 0xffff0000);
		break;

	case SDH_CMD:
		*SDHOST_TM_CMD = (val << 16) | (*SDHOST_TM_CMD & 0xffff);
		break;

	case SDH_BUF_DATA:
		*SDHOST_BUFDATAPORT = val;
		break;

	case SDH_HST_CNTL_1:
		*SDHOST_HOSTCTRL1_PWR_BLKGAP = val
				| (*SDHOST_HOSTCTRL1_PWR_BLKGAP & 0xffffff00);
		break;

	case SDH_PWR_CNTL:
		*SDHOST_HOSTCTRL1_PWR_BLKGAP = (val << 8)
				| (*SDHOST_HOSTCTRL1_PWR_BLKGAP & 0xffff00ff);
		break;

	case SDH_CLK_CNTL:
		*SDHOST_CLK_TIMEOUT_SWRESET = val
				| (*SDHOST_CLK_TIMEOUT_SWRESET & 0xffff0000);
		break;

	case SDH_SW_RST:
		*SDHOST_CLK_TIMEOUT_SWRESET = (val << 24)
				| (*SDHOST_CLK_TIMEOUT_SWRESET & 0xffffff);
		break;

	case SDH_NRML_INT_STATUS:
		*SDHOST_NORMINTSTATUS_ERRINTSTATUS = val
				| (*SDHOST_NORMINTSTATUS_ERRINTSTATUS & 0xffff0000);
		break;

	case SDH_ERR_INT_STATUS:
		*SDHOST_NORMINTSTATUS_ERRINTSTATUS = (val << 16)
				| (*SDHOST_NORMINTSTATUS_ERRINTSTATUS & 0xffff);
		break;

	case SDH_NRML_INT_STATUS_ENABLE:
		*SDHOST_NORMINTSTATUSEN_ERRINTSTATUSEN = val
				| (*SDHOST_NORMINTSTATUSEN_ERRINTSTATUSEN & 0xffff0000);
		break;

	case SDH_ERR_INT_STATUS_ENABLE:
		*SDHOST_NORMINTSTATUSEN_ERRINTSTATUSEN = (val << 16)
				| (*SDHOST_NORMINTSTATUSEN_ERRINTSTATUSEN & 0xffff);
		break;

	case SDH_NRML_INT_SGNL_ENABLE:
		*SDHOST_NORMINTSIGEN_ERRINTSIGEN = val
				| (*SDHOST_NORMINTSIGEN_ERRINTSIGEN & 0xffff0000);
		break;

	case SDH_ERR_INT_SGNL_ENABLE:
		*SDHOST_NORMINTSIGEN_ERRINTSIGEN = (val << 16)
				| (*SDHOST_NORMINTSIGEN_ERRINTSIGEN & 0xffff);
		break;

	case SDH_HST_CNTL_2:
		*SDHOST_AUTOCMD12_HOSTCTRL2 = (val << 16)
				| (*SDHOST_AUTOCMD12_HOSTCTRL2 & 0xffff);
		break;

	case SDH_VNDR_0:
		*SDHOST_VENDOR0 = val;
		break;

	case SDH_VNDR_1:
		*SDHOST_VENDOR1 = val;
		break;

	case SDH_VNDR_5:
		*SDHOST_VENDOR5 = val;
		break;

	default:
		break;
		/* Invalid address */
	}

	return;
}

/** @brief Initialise the SD Host peripheral
 *  @param none
 *  @returns none
 */

void sdhost_init(void) {
	uint32_t timeout;
	/* Reset all */
	// It's good to wait for the reset to complete
	timeout = COMM_TIMEOUT_U32;
	do {
		write_sdhost_reg(SW_RESET_ALL, SDH_SW_RST);
		// Note we need a delay here else the write and read are too fast for SD Controller to react when compiled with Os flag
		// See redmine bug 402 - http://glared1.ftdi.local/issues/402
		delayus(1);
	} while ((read_sdhost_reg(SDH_SW_RST) & SW_RESET_ALL) && (--timeout != 0));
	DBG_TIMEOUT()

	/*
	 * Set card detection de-bouncing time
	 * vendor vegister 5 sets the card debouncing debug time
	 * where 0 = 2^^9, 1 = 2^^10 ... 10 - 2^^24
	 */
	write_sdhost_reg(0x0B, SDH_VNDR_5);

	// Writing into the debounce register triggers a de-bounce sequence and you have to wait here till the debounce is done before you do anything else!
	// See redmine bug 402 - http://glared1.ftdi.local/issues/402
     timeout = COMM_TIMEOUT_U32;
     // wait for status to be debounced
     while (((read_sdhost_reg(SDH_PRESENT_STATE) & PRESENT_STATE_SYS_CARD_STABLE) == 0) && (--timeout != 0))
     ;
	DBG_TIMEOUT()

	/* reset all bits in host control 2 register */
	write_sdhost_reg(0, SDH_HST_CNTL_2);

	/* Set SD bus voltage after checking SDH_CAP_1 for voltage support */
	if (read_sdhost_reg(SDH_CAP_1) & 0x01000000) /* SD Host supports 3.3 V */
	{
		write_sdhost_reg(0x0E, SDH_PWR_CNTL);
	} else if (read_sdhost_reg(SDH_CAP_1) & 0x02000000) { /* SD Host supports 3.0 V */
		write_sdhost_reg(0x0C, SDH_PWR_CNTL);
	} else if (read_sdhost_reg(SDH_CAP_1) & 0x04000000) { /* SD Host supports 1.8 V */
		write_sdhost_reg(0x0A, SDH_PWR_CNTL);
	}

	/* Enable SD Bus Power */
	write_sdhost_reg(read_sdhost_reg(SDH_PWR_CNTL) | 0x01, SDH_PWR_CNTL);

	/* Enable interrupt signal and status */
	write_sdhost_reg(0x01FFF, SDH_NRML_INT_STATUS_ENABLE);
	write_sdhost_reg(0x07FF, SDH_ERR_INT_STATUS_ENABLE);

#ifdef __USE_SDHOST_INTERRUPT_
	write_sdhost_reg(0x01FFF, SDH_NRML_INT_SGNL_EN);
	write_sdhost_reg(0x07FF, ERR_ERR_INT_SGNL_ENABLE);
#endif

    /* Enable high speed, set bus data width to 1-bit mode */
    write_sdhost_reg(0x05, SDH_HST_CNTL_1);

	/* Configure CRC status wait cycle, pulse latch offset to 0x01 (pulse latch offset MUST be set to 0x01) */
	write_sdhost_reg(0x02000101, SDH_VNDR_0);
	write_sdhost_reg(0, SDH_VNDR_1);

	/* In the card identification mode, the maximum clock frequency is 400 kHz. Have to set the divider to reduce the clock speed */
	/* Enable internal clock */
	write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) | 0x0001, SDH_CLK_CNTL);

	/* Wait for the internal clock to be stable */
	timeout = COMM_TIMEOUT_U32;
	while ((!(read_sdhost_reg(SDH_CLK_CNTL) & CLK_CTRL_CLK_STABLE))
			&& (--timeout != 0))
		;
	DBG_TIMEOUT()
	if (timeout == 0) return;

    /* Enable clock output, choose divider to be 0x40 (i.e. base SD clock divided by 128) */
    write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) | 0x4004, SDH_CLK_CNTL);

#ifdef __USE_SDHOST_INTERRUPT_
	/* Attach SD Host ISR */
	attach_interrupt(4, &sdhost_isr);
#endif

	/* Need card to be initialised next. */
	sdhost_initialisation_status = SDHOST_CARD_NOT_INITIALISED;;

	return;
}

/** @brief This function detects whether an SD card is inserted
 *  @param none
 *  @returns SDHOST_ERROR / SDHOST_CARD_INSERTED / SDHOST_CARD_REMOVED
 */

SDHOST_STATUS sdhost_card_detect(void) {

	SDHOST_STATUS sdhost_status = SDHOST_ERROR;

	if (sdhost_initialisation_status == SDHOST_NOT_INITIALISED)
	{
		return sdhost_initialisation_status;
	}

	/* Check if a card has been inserted */
	if (read_sdhost_reg(SDH_PRESENT_STATE) & PRESENT_STATE_SYS_CARD_INST)
	{
		sdhost_status = SDHOST_CARD_INSERTED;
	} else {

#ifdef __USE_SDHOST_INTERRUPT_
		/* Wait for interrupt to occur */
		while (!card_status_changed)
		;

		/* Interrupt detected! Check SDH_PRESENT_STATE to see if the card has been inserted or removed */

		if (read_sdhost_reg(SDH_PRESENT_STATE) & PRESENT_STATE_SYS_CARD_INST)
		{
			sdhost_status = SDHOST_CARD_INSERTED;
		} else if (!(read_sdhost_reg(SDH_PRESENT_STATE) & PRESENT_STATE_SYS_CARD_INST)) {
			sdhost_status = SDHOST_CARD_REMOVED;
		}

		/* Reset card detection flag */
		card_status_changed = false;
#else
		if ((read_sdhost_reg(SDH_NRML_INT_STATUS) & NORM_INT_STATUS_CARD_INS)
				== 0) {
			sdhost_status = SDHOST_CARD_REMOVED;
		} else {
			/* Clear the flag by writing 1 to the flag bit */
			write_sdhost_reg(NORM_INT_STATUS_CARD_INS, SDH_NRML_INT_STATUS);
			sdhost_status = SDHOST_CARD_INSERTED;
		}
#endif
	}

	return sdhost_status;
}

/**
 *  @brief This function gets a response from a command issued to the SD Host peripheral
 *
 *  This is an internal function that extarts a response from the SD host peripheral in
 *  response to an issued command.
 *
 *  @param response a pointer to the array (R2) / variable (others) that will hold the response
 *  @param response_type the response type
 *  @param is_auto_command_response indicates whether the response is an auto CMD response (auto CMD12/CMD23)
 *  @returns SDHOST_STATUS
 */

static SDHOST_STATUS sdhost_get_response(uint32_t *response,
		sdhost_response_t response_type, bool is_auto_cmd_response) {

	SDHOST_STATUS sdhost_status = SDHOST_OK;

	if (is_auto_cmd_response) {
		/*
		 *    Bits [39:8] (card status) of the response for Auto CMD23 (R1) / Auto CMD12 (R1b) is stored
		 *   in SDH_RESPONSE3 register
		 */

		*response = read_sdhost_reg(SDH_RESPONSE3);

#ifdef SD_HOST_DEBUG
		for (int i = 0; i < 1; i++) {
			tfp_printf("%08X", *((unsigned char*) response + i));
		}
		tfp_printf("\r\n--------------------------------\r\n");
#endif // SD_HOST_DEBUG
	} else {

		if (response_type == SDHOST_RESPONSE_R2) {
			/* For R2, bits [127:8] of the response is stored in all 4 response registers */
			response[0] = read_sdhost_reg(SDH_RESPONSE0);
			response[1] = read_sdhost_reg(SDH_RESPONSE1);
			response[2] = read_sdhost_reg(SDH_RESPONSE2);
			response[3] = read_sdhost_reg(SDH_RESPONSE3);

#ifdef SD_HOST_DEBUG
			for (int i = 0; i < 4*4; i++) {
				tfp_printf("0x%02X,", *((unsigned char*) response + i));
			}
			tfp_printf("\r\n--------------------------------\r\n");
#endif // SD_HOST_DEBUG
		} else {

			*response = read_sdhost_reg(SDH_RESPONSE0);

#ifdef SD_HOST_DEBUG

			for (int i = 0; i < 1; i++) {
				tfp_printf("%08X", *((unsigned char*) response + i));
			}
			tfp_printf("\r\n--------------------------------\r\n");
#endif // SD_HOST_DEBUG
		}

	}

	return sdhost_status;
}

/** @brief This function generates a command for the Command Register.
 *  @param cmd_index command to generate (0-63)
 *  @param cmd_type command type (bus / application-specific)
 *  @param sdio_special_op the SDIO special operation for CMD52, assign NOT_IN_USE
 *          if the operation is not one of the 3 special ones
 *
 *  @param resp_type_ptr pointer to a variable to save the response type for the cmd
 *  @returns Returns the value to be written to Command Register
 */

static uint16_t sdhost_generate_cmd(uint8_t cmd_index, sdhost_cmd_t cmd_type,
		uint8_t sdio_special_op, sdhost_response_t *resp_type_ptr) {
	sdhost_response_t response_type;
	uint16_t cmd_reg_value;

	(void) sdio_special_op;

	/* preset */
	response_type = SDHOST_RESPONSE_NONE;
	cmd_reg_value = 0;

	/* Compute Command Register value based on the info passed into this function */
	/* First, set the cmd_idx bits */
	cmd_reg_value = cmd_index << 8;

	/* Now, compute the other bits */
	if (cmd_type == SDHOST_BUS_CMD) {
		if (cmd_index == SDCMD_STOP_TRANSMISSION) {
			cmd_reg_value |= CMD_TYPE_ABORT;
		} else {
			cmd_reg_value |= CMD_TYPE_NORMAL;
		}

		/* Then set the data_pres_sel bits */
		/*
		 *  Refer to the SD Physical Layer Simplified Spec page 69
		 *  for the list of commands with data transfer
		 */

		if (cmd_index == SDCMD_SET_BUS_WIDTH)
			cmd_reg_value |= CMD_DATA_PRESENT;
		else
			cmd_reg_value |= CMD_NO_DATA_PRESENT;

		/* Last, get the response type and set the bits */

		/*
		 *  Refer to the SD Physical Layer Simplified Spec page 69 for
		 *  the response type for each command
		 */
		switch (cmd_index) {
		case SDCMD_GO_IDLE_STATE:
			cmd_reg_value |= CMD_RESPONSE_NONE;
			response_type = SDHOST_RESPONSE_NONE;
			break;

		case SDCMD_ALL_SEND_CID:
		case SDCMD_SEND_CSD:
			cmd_reg_value |= CMD_RESPONSE_R2;
			response_type = SDHOST_RESPONSE_R2;
			break;

		case SDCMD_SEND_RELATIVE_ADDR:
			cmd_reg_value |= CMD_RESPONSE_R6;
			response_type = SDHOST_RESPONSE_R6;
			break;

		case SDCMD_SDIO:
			cmd_reg_value |= CMD_RESPONSE_R4;
			response_type = SDHOST_RESPONSE_R4;
			break;

		case SDCMD_SEND_IF_COND:
			cmd_reg_value |= CMD_RESPONSE_R7;
			response_type = SDHOST_RESPONSE_R7;
			break;

		case SDCMD_SEL_DESEL_CARD:
		case SDCMD_STOP_TRANSMISSION:
			cmd_reg_value |= CMD_RESPONSE_R1b;
			response_type = SDHOST_RESPONSE_R1b;
			break;

		case SDCMD_READ_STATUS_REG:
			cmd_reg_value |= CMD_RESPONSE_R1;
			response_type = SDHOST_RESPONSE_R1;
			break;

		default:
			cmd_reg_value |= CMD_RESPONSE_R1;
			response_type = SDHOST_RESPONSE_R1;
			break;
		}

		*resp_type_ptr = response_type;

	} else if (cmd_type == SDHOST_APP_SPECIFIC_CMD) {

		/* Set the cmd_type bits in the Command Register */
		cmd_reg_value |= CMD_TYPE_NORMAL;

		/* Other commands are without data transfer */
		cmd_reg_value |= CMD_NO_DATA_PRESENT;

		/* Last, get the response type and set the bits */
		switch (cmd_index) {
		/* Refer to the SD Physical Layer Simplified Spec page 74 for the response type for each command */
		case SDCMD_SEND_OP_CON:
			cmd_reg_value |= CMD_RESPONSE_R3;
			response_type = SDHOST_RESPONSE_R3;
			break;

		case SDCMD_READ_STATUS_REG:
			cmd_reg_value |= CMD_RESPONSE_R2;
			response_type = SDHOST_RESPONSE_R2;
			break;

		default:
			cmd_reg_value |= CMD_RESPONSE_R1;
			response_type = SDHOST_RESPONSE_R1;
			break;
		}
		*resp_type_ptr = response_type;
	}

	return (cmd_reg_value);
}

/** @brief This function sends a command without data transfer.
 *  @param cmd_index command to generate (0-63)
 *  @param cmd_type command type (bus / application-specific)
 *  @param sdio_special_op the SDIO special operation for CMD52, assign NOT_IN_USE
 *         if the operation is not one of the 3 special ones
 *
 *  @param cmd_arg command argument (32 bits)
 *  @param  response pointer to the array (R2) / variable (others) that will hold the response
 *  @returns a SD Host status
 */

static SDHOST_STATUS sdhost_send_command(uint8_t cmd_index, sdhost_cmd_t cmd_type,
		uint8_t sdio_special_op, uint32_t cmd_arg, uint32_t *response) {
#ifdef SD_HOST_DEBUG
	tfp_printf("--------------------------------\r\n");
	tfp_printf("CMD:%d\r\n",cmd_index );
#endif // SD_HOST_DEBUG
	uint32_t timeout;
	SDHOST_STATUS sdhost_status;
	uint16_t cmd_reg_value;
	sdhost_response_t response_type;
	uint32_t present_state;

	/* preset */
	sdhost_status = SDHOST_OK;
	response_type = SDHOST_RESPONSE_NONE;

	/* Compute the value for Command Register */
	cmd_reg_value = sdhost_generate_cmd(cmd_index, cmd_type, sdio_special_op,
			&response_type);

	/*
	 *  Wait until Command Inhibit (CMD) bit in the present state register is 0.
	 *  When Command Inhibit (CMD) is set to 1, the SD command bus is in use and the host driver
	 *  cannot issue a SD command.
	 */
	timeout = COMM_TIMEOUT_U32;
	while (((present_state = read_sdhost_reg(SDH_PRESENT_STATE))
			& PRESENT_STATE_CMD_INHIBIT_C) && (--timeout != 0))
		;
	DBG_TIMEOUT()
	if (timeout == 0)
	{
		sdhost_error_recovery();
		sdhost_status = SDHOST_CMD_TIMEOUT;
		goto __exit;
	}

	/* Check if the previous SD command uses the SD data bus with a busy signal */
	if (present_state & PRESENT_STATE_DATA_LIN_ACT) {
		/*
		 *  If the host driver does not issue an abort command, check cmd_inhibit_d in the present state register.
		 *  Repeat this step until cmd_inhibit_d becomes 0.
		 */
		if ((cmd_reg_value & CMD_TYPE_ABORT) != CMD_TYPE_ABORT) {
			timeout = COMM_TIMEOUT_U32;
			while ((read_sdhost_reg(SDH_PRESENT_STATE)
					& PRESENT_STATE_CMD_INHIBIT_D) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
		}
	}
	}

	/* Set the argument to the argument register */
	*SDHOST_ARG1 = cmd_arg;

	/* Set the command register */
	write_sdhost_reg((uint32_t) cmd_reg_value, SDH_CMD);

#ifdef __USE_SDHOST_INTERRUPT_

	/* Wait for the Command Complete interrupt */
	while (!cmd_complete)
	{
		if (errStatus & ERR_INT_STATUS_CMD_TIMEOUT_ERR)
		{
			/* Clear the error flag */
			errStatus &= ~ERR_INT_STATUS_CMD_TIMEOUT_ERR;

			/* Reset flag */
			cmd_complete = false;
			sdhost_status = SDHOST_CMD_TIMEOUT;
			goto __exit;
		}
	}

	/* Reset flag */
	cmd_complete = false;

#else

	/* Wait for the Command Complete bit in the Normal Status register to be set */
	timeout = COMM_TIMEOUT_U32;
	while (((read_sdhost_reg(SDH_NRML_INT_STATUS) & NORM_INT_STATUS_CMD_COMPLETE)
			== 0) && (--timeout != 0)) {
		/*
		 *  If the Command Timeout bit is set in the Error Status register,
		 *  return an error.
		 */
		if (read_sdhost_reg(SDH_ERR_INT_STATUS) & ERR_INT_STATUS_CMD_TIMEOUT_ERR) {
			/* perform error recovery */
			sdhost_error_recovery();
#ifdef SD_HOST_DEBUG
			tfp_printf("SDHOST_CMD_TIMEOUT");
			#endif
			sdhost_status = SDHOST_CMD_TIMEOUT;
			goto __exit;

		};
	}
	DBG_TIMEOUT()
	if (timeout == 0)
	{
		sdhost_error_recovery();
		sdhost_status = SDHOST_CMD_TIMEOUT;
		goto __exit;
	}

	/* Clear the status flag by writing 1 to it */
	write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif

	/* Get response data */
	sdhost_get_response(response, response_type, false);

#ifdef SD_HOST_DEBUG
	RegisterDump();
#endif // SD_HOST_DEBUG
	/*
	 *  Check whether the command uses the Transfer Complete Interrupt
	 *  by checking cmd_inhibit_d bit in the Present State Register
	 */
	present_state = read_sdhost_reg(SDH_PRESENT_STATE);
	if (present_state & PRESENT_STATE_CMD_INHIBIT_D) {
#ifdef __USE_SDHOST_INTERRUPT_

		/* Wait for the Transfer Complete interrupt */
		//BUGBUG: have a timeout so that won't block forever
		while (!transfer_complete)
		;

		/* Reset flag */
		transfer_complete = false;

#else

		/* Wait for the Transfer Complete bit in the Normal Status register to be set */
		timeout = COMM_TIMEOUT_U32;
		while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
				& NORM_INT_STATUS_TRANSFER_COMPLETE) == 0) && (--timeout != 0))
			;
		DBG_TIMEOUT()
		if (timeout == 0)
		{
			sdhost_error_recovery();
			sdhost_status = SDHOST_CMD_TIMEOUT;
			goto __exit;
		}

		/* Clear the status flag by writing 1 to it */
		write_sdhost_reg(NORM_INT_STATUS_TRANSFER_COMPLETE,
		SDH_NRML_INT_STATUS);
#endif

	}

	/*
	 *  Examine the response data if the response type is R1 or R1b,
	 *  return SDHOST_ERROR if an error occurred
	 */

	if ((response_type == SDHOST_RESPONSE_R1)
			|| (response_type == SDHOST_RESPONSE_R1b)) {
#ifdef SD_HOST_DEBUG
		AnalyzeR1Status(*response);
#endif // SD_HOST_DEBUG
		if ((*response) & RESPONSE_R1_ERROR_MASK)
			sdhost_status = SDHOST_RESPONSE_ERROR;
	} else if (response_type == SDHOST_RESPONSE_R6) {
		if ((*response) & RESPONSE_R6_ERROR_MASK)
			sdhost_status = SDHOST_RESPONSE_ERROR;
	}

	/* Check if any error occurred. Note that SDHOST_CMD_TIMEOUT (if any) has been detected and returned prior to this. */
#ifdef __USE_SDHOST_INTERRUPT_

	if (errStatus & 0x07FF)
	{
		/* Clear error flag */
#ifdef SD_HOST_DEBUG
		tfp_printf("\r\nerrStatus = "); printNum((uint32_t) errStatus, 'h', 1);
#endif // SD_HOST_DEBUG
		errStatus = 0;
		sdhost_status = SDHOST_ERROR;
	}

#else

	if (read_sdhost_reg(SDH_ERR_INT_STATUS) & 0x07FF) {
		/* Recover SD Host from error */
#ifdef SD_HOST_DEBUG
		tfp_printf("SDH_ERR_INT_STATUS");
#endif // SD_HOST_DEBUG
		sdhost_error_recovery();
		sdhost_status = SDHOST_ERROR;
	}
#endif

	__exit: return (sdhost_status);
}

/**
 *  @brief This function peforms data transfers to/fr0m the SD card
 *
 *  This function follows paragraph 3.7.2.1 in the SD Host Controller Simplified Specification V3.00
 *  The flow has been modified so that each transfer type has it's own path through the function, instead
 *  of having common parts of the function as implied in the specification.
 *
 *  @param direction SDHOST_READ / SDHOST_WRITE
 *  @param buf data buffer
 *  @param num_bytes number of bytes to be transferred
 *  @param addr sector address
 *  @returns a SD Host status
 */

SDHOST_STATUS sdhost_transfer_data(uint8_t direction, void *buf,
		uint32_t num_bytes, uint32_t addr) {

	SDHOST_STATUS sdhost_status;
	uint32_t num_blocks;
	uint32_t num_bytes_left;
	uint32_t response;
	uint32_t j;
	uint32_t *buf32;
	uint16_t cmd;
	uint16_t transfer_mode;
	uint32_t nextIdx;
	uint32_t timeout;

	if (sdhost_initialisation_status != SDHOST_OK)
	{
		return sdhost_initialisation_status;
	}

	/* initialise local variables */
	sdhost_status = SDHOST_OK;
	num_blocks = 1;
	num_bytes_left = 0;
	buf32 = (uint32_t *) buf;
	transfer_mode = 0;
	nextIdx = 0;

	/*
	 *  Note: For block CMDs (read/write/lock) SDSC Card uses byte unit address and SDHC/SDXC Cards
	 *        use block unit address (512 Bytes unit); We have to convert from sector address to the
	 *        appropriate address for the CMDs first
	 */

	if (sdhost_context.isSDSCCard == true)
		addr <<= SDHOST_BLK_SIZE_POW;

	/*
	 *  Note: The length of a multiple block transfer needs to be in block size units.
	 *        If the total data length cannot be divided evenly by the block size, there
	 *        are two ways to transfer the data depending on the function and card design:
	 *        Option 1 is for the Card Driver to split the transaction. The remainder of block size
	 *                 data is transferred by using a single block command at the end.
	 *        Option 2 is to add dummy data in the last block to fill the block size. This is efficient but the card must
	 *                 manage or remove the dummy data.
	 */

	/*
	 *  Note: Since Transfer Mode and Command Register has to be accessed at the same time and a
	 *        write to Command Register will send a command, it's better to set both registers at
	 *        the same time to prevent the current command in the Command Register to be sent again.
	 */

	/*
	 *  Note: To improve performance, response data is not be checked.
	 */

	/* Calculate how many "whole" blocks are needed. The remaining byte will be sent in one single-block transfer. */

	num_blocks = num_bytes >> SDHOST_BLK_SIZE_POW;
	num_bytes_left = num_bytes & ((1UL << SDHOST_BLK_SIZE_POW) - 1);

	/* Send CMD16 to set the card's block length */

	sdhost_status = sdhost_send_command(SDCMD_SEND_BLOCKLEN, SDHOST_BUS_CMD, NOT_IN_USE,
	SDHOST_BLK_SIZE, &response);
	if (sdhost_status != SDHOST_OK)
	{
#ifdef SD_HOST_DEBUG
		tfp_printf("ERROR %d\r\n", sdhost_status);
#endif // SD_HOST_DEBUG
		goto __exit;
	}

	/*
	 *  If only one "whole" block is needed, perform single-block transfer for this block.
	 *  Remaining bytes (if any) will be sent in another single-block transfer later.
	 *  This also include the case where less than SDHOST_BLK_SIZE bytes are transferred.
	 */

	if (num_blocks == 1) {
		/* (1) Set Block Size register */
		*SDHOST_BS_BC = (*SDHOST_BS_BC & 0xFFFF0000) | SDHOST_BLK_SIZE;

		/* (2) Single-block transfer, SDH_BLK_COUNT register will be ignored */

		if (direction == SDHOST_WRITE) {
			/* (3) Set Argument 1 Register */
			*SDHOST_ARG1 = addr;

			/* (4) Set Transfer Mode Register and Command Register at the same time */
			cmd = (24 << 8) | CMD_TYPE_NORMAL | CMD_RESPONSE_R1
					| CMD_DATA_PRESENT;
			transfer_mode = SDHOST_DMA_DISABLED | SDHOST_AUTO_CMD12_DISABLED
					| SDHOST_SINGLE_BLK | SDHOST_WRITE;

			*SDHOST_TM_CMD = (cmd << 16) | transfer_mode;

#ifdef __USE_SDHOST_INTERRUPT_
			/*  (6) Wait for Command Complete interrupt */
			while ( (!cmd_complete) && (--timeout != 0))
			;

			/* Reset interrupt flag */
			cmd_complete = false;
#else
			/* (6) Wait for the Command Complete bit in the Normal Status register to be set */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_CMD_COMPLETE) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (7) Clear the status flag by writing 1 to it */
			write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif

			/* (8) Get response data */
			/* sdhost_get_response(&response, SDHOST_RESPONSE_R1, false); -- not checked */

#ifdef __USE_SDHOST_INTERRUPT_
			/* (10) Wait for Buffer Write Ready Int */
			while (!write_buf_ready)
			;

			/* Clear interrupt flag */
			write_buf_ready = false;
#else

			/* (10) wait for buffer write ready */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_BUF_W_RDY) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (11) Clear the flag by writing 1 to the flag bit */
			write_sdhost_reg(NORM_INT_STATUS_BUF_W_RDY, SDH_NRML_INT_STATUS);
#endif

			/*
			 * (12) Write block data to Buffer Data Port register
			 * Note: To improve performance, use inline assembly here.
			 */

			__asm__ ( "streamout.l    %0, %1, %2    \n\t"
					: /* output registers */
					: "r" (&(*SDHOST_BUFDATAPORT)), "r" (buf32), "r" (SDHOST_BLK_SIZE) /* input registers */
			);

		} else if (direction == SDHOST_READ) {
			/* (3) Set Argument 1 Register */
			*SDHOST_ARG1 = addr;

			/* (4) Set Transfer Mode Register and Command Register at the same time */
			cmd = (17 << 8) | CMD_TYPE_NORMAL | CMD_RESPONSE_R1
					| CMD_DATA_PRESENT;
			transfer_mode = SDHOST_DMA_DISABLED | SDHOST_AUTO_CMD12_DISABLED
					| SDHOST_SINGLE_BLK | SDHOST_READ;

			/* (5) Set the command register */
			*SDHOST_TM_CMD = (cmd << 16) | transfer_mode;

#ifdef __USE_SDHOST_INTERRUPT_
			/* (6) Wait for Command Complete interrupt */
			while ((!cmd_complete) && (--timeout != 0))
			;

			/* (7) Reset interrupt flag */
			cmd_complete = false;
#else
			/* (6) Wait for the Command Complete bit in the Normal Status register to be set */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_CMD_COMPLETE) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/*(7) Clear the status flag by writing 1 to it */
			write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif

			/* (8) Get response data */
			/* sdhost_get_response(&response, SDHOST_RESPONSE_R1, false); -- not checked */

#ifdef __USE_SDHOST_INTERRUPT_
			/* (14) Wait for Buffer Read Ready Int */
			while ((!read_buf_ready) && (--timeout != 0))
			;

			/* (15) Clear interrupt flag */
			read_buf_ready = false;
#else
			/* (14) Wait for Buffer Read Ready */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_BUF_R_RDY) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (15) Clear the flag by writing 1 to the flag bit */
			write_sdhost_reg(NORM_INT_STATUS_BUF_R_RDY, SDH_NRML_INT_STATUS);
#endif

			/*
			 * (16) Read block data from Buffer Data Port register
			 * Note: To improve performance, use inline assembly here
			 */

			__asm__ ( "streamin.l        %0, %1, %2    \n\t"
					: /* output registers */
					: "r" (buf32), "r" (&(*SDHOST_BUFDATAPORT)), "r" (SDHOST_BLK_SIZE) /* input registers */
			);
		}

#ifdef __USE_SDHOST_INTERRUPT_
		/* (19) Wait for the Transfer Complete interrupt */
		while (!transfer_complete)
		;

		/* (20)  Reset flag */
		transfer_complete = false;
#else
		/* (19) Wait for the Transfer Complete bit in the Normal Status register to be set */
		if (direction == SDHOST_WRITE) {
			timeout = COMM_TIMEOUT_U32;
		}
		else
		{
		timeout = COMM_TIMEOUT_U32;
		}
		while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
				& NORM_INT_STATUS_TRANSFER_COMPLETE) == 0) && (--timeout != 0))
			;
		DBG_TIMEOUT()
		if (timeout == 0)
		{
			sdhost_error_recovery();
			sdhost_status = SDHOST_CMD_TIMEOUT;
			goto __exit;
		}

		/* (20)  Clear the status flag by writing 1 to it */
		write_sdhost_reg(NORM_INT_STATUS_TRANSFER_COMPLETE,
		SDH_NRML_INT_STATUS);
#endif

		/* Set the next index in the data buffer for the remaining bytes */
		nextIdx = SDHOST_BLK_SIZE >> 2;

	} else if (num_blocks >= 2) {

		/* Else perform multiple-block transfer for the "whole" blocks. */

		nextIdx = 0;

		/*
		 * (1) Set Block Size register
		 * (2) Multiple-block transfer
		 */

		*SDHOST_BS_BC = (num_blocks << 16) | SDHOST_BLK_SIZE;

		if (direction == SDHOST_WRITE) {
			/* (3) Set Argument 1 Register */
			*SDHOST_ARG1 = addr;

			/* (4) Set Transfer Mode Register and (5) Command Register at the same time */
			cmd = (25 << 8) | CMD_TYPE_NORMAL | CMD_RESPONSE_R1
					| CMD_DATA_PRESENT;
			transfer_mode = SDHOST_DMA_DISABLED | SDHOST_BLK_COUNT_ENABLED
					| SDHOST_AUTO_CMD12_ENABLED | SDHOST_MULTI_BLK
					| SDHOST_WRITE;
			*SDHOST_TM_CMD = (cmd << 16) | transfer_mode;

#ifdef __USE_SDHOST_INTERRUPT_
			/* (6) Wait for Command Complete interrupt */
			while (!cmd_complete)
			;

			/* (7) Reset interrupt flag */
			cmd_complete = false;
#else
			/* (6) Wait for the Command Complete bit in the Normal Status register to be set */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_CMD_COMPLETE) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (7) Clear the status flag by writing 1 to it */
			write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif
			/* (8) Get response data */
			/* sdhost_get_response(&response, SDHOST_RESPONSE_R1, false); -- not checked */

			for (j = 0; j < num_blocks; j++) {
#ifdef __USE_SDHOST_INTERRUPT_
				/* (10) Wait for Buffer Write Ready Int */
				while (!write_buf_ready)
				;

				/* (11) Clear interrupt flag */
				write_buf_ready = false;
#else
				/* (10) Wait for Buffer Write Ready bit */
				timeout = COMM_TIMEOUT_U32;
				while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
						& NORM_INT_STATUS_BUF_W_RDY) == 0) && (--timeout != 0))
					;
				DBG_TIMEOUT()
				if (timeout == 0)
				{
					sdhost_error_recovery();
					sdhost_status = SDHOST_CMD_TIMEOUT;
					goto __exit;
				}

				/* (11) Clear the flag by writing 1 to the flag bit */
				write_sdhost_reg(NORM_INT_STATUS_BUF_W_RDY,
				SDH_NRML_INT_STATUS);
#endif
				/*
				 ** (12) Write block data to Buffer Data Port register
				 ** Note: To improve performance, use inline assembly here.
				 */

				__asm__ ( "streamout.l    %0, %1, %2    \n\t"
						: /* output registers */
						: "r" (&(*SDHOST_BUFDATAPORT)), "r" (&buf32[nextIdx]), "r" (SDHOST_BLK_SIZE) /* input registers */
				);

				nextIdx += SDHOST_BLK_SIZE / 4;
			}
		} else if (direction == SDHOST_READ) {
			/* (3) Set Argument 1 Register */
			*SDHOST_ARG1 = addr;

			/* (4) Set Transfer Mode Register and (5) Command Register at the same time */
			cmd = (18 << 8) | CMD_TYPE_NORMAL | CMD_RESPONSE_R1
					| CMD_DATA_PRESENT;
			transfer_mode =
					SDHOST_DMA_DISABLED | SDHOST_BLK_COUNT_ENABLED
							| SDHOST_AUTO_CMD12_ENABLED | SDHOST_MULTI_BLK
							| SDHOST_READ;
			*SDHOST_TM_CMD = (cmd << 16) | transfer_mode;

#ifdef __USE_SDHOST_INTERRUPT_
			/* (6) Wait for Command Complete interrupt */
			while (!cmd_complete)
			;

			/* (7) Reset interrupt flag */
			cmd_complete = false;
#else
			/* (6) Wait for the Command Complete bit in the Normal Status register to be set */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_CMD_COMPLETE) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (7) Clear the status flag by writing 1 to it */
			write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif
			/* (8) Get response data */
			/* sdhost_get_response(&response, SDHOST_RESPONSE_R1, false); -- not checked */

			for (j = 0; j < num_blocks; j++) {
#ifdef __USE_SDHOST_INTERRUPT_
				/* (14) Wait for Buffer Read Ready Int */
				while (!read_buf_ready)
				;

				/* (15) Clear interrupt flag */
				read_buf_ready = false;
#else
				/* (14) Wait for Buffer Read Ready bit */
				timeout = COMM_TIMEOUT_U32;
				while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
						& NORM_INT_STATUS_BUF_R_RDY) == 0) && (--timeout != 0))
					;
				DBG_TIMEOUT()
				if (timeout == 0)
				{
					sdhost_error_recovery();
					sdhost_status = SDHOST_CMD_TIMEOUT;
					goto __exit;
				}

				/* (15) Clear the flag by writing 1 to the flag bit */
				write_sdhost_reg(NORM_INT_STATUS_BUF_R_RDY,
				SDH_NRML_INT_STATUS);
#endif
				/*
				 * (16) Read block data from Buffer Data Port register
				 * Note: To improve performance, use inline assembly here
				 */

				__asm__ ( "streamin.l        %0, %1, %2    \n\t"
						: /* output registers */
						: "r" (&buf32[nextIdx]), "r" (&(*SDHOST_BUFDATAPORT)), "r" (SDHOST_BLK_SIZE) /* input registers */
				);

				nextIdx += SDHOST_BLK_SIZE / 4;
			}
		}

#ifdef __USE_SDHOST_INTERRUPT_
		/* (19) Wait for the Transfer Complete interrupt */
		while (!transfer_complete)
		;

		/* (20) Reset flag */
		transfer_complete = false;
#else
		/* (19) Wait for the Transfer Complete bit in the Normal Status register to be set */
		if (direction == SDHOST_WRITE) {
			timeout = COMM_TIMEOUT_U32;
		}
		else
		{
		timeout = COMM_TIMEOUT_U32;
		}
		while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
				& NORM_INT_STATUS_TRANSFER_COMPLETE) == 0) && (--timeout != 0))
			;
		DBG_TIMEOUT()
		if (timeout == 0)
		{
			sdhost_error_recovery();
			sdhost_status = SDHOST_CMD_TIMEOUT;
			goto __exit;
		}

		/* (20) Clear the status flag by writing 1 to it */
		write_sdhost_reg(NORM_INT_STATUS_TRANSFER_COMPLETE,
		SDH_NRML_INT_STATUS);
#endif

		/* Set the next index in the data buffer for the remaining bytes */
		nextIdx = num_blocks * (SDHOST_BLK_SIZE / 4);
	}

	/* Now take care of the "left-over" bytes */

	if (num_bytes_left > 0) {
		/* (1) Set Block Size register to SDHOST_BLK_SIZE */
		*SDHOST_BS_BC = (*SDHOST_BS_BC & 0xFFFF0000) | SDHOST_BLK_SIZE;

		/* (2) Send CMD16 to set the card's block length */
		sdhost_send_command(SDCMD_SEND_BLOCKLEN, SDHOST_BUS_CMD, NOT_IN_USE,
		SDHOST_BLK_SIZE, &response);

		/* Single-block transfer, SDH_BLK_COUNT register will be ignored */
		if (direction == SDHOST_WRITE) {
			/* (3) Set Argument 1 Register */
			*SDHOST_ARG1 = addr;

			/* (4) Set Transfer Mode Register and Command Register at the same time */
			cmd = (24 << 8) | CMD_TYPE_NORMAL | CMD_RESPONSE_R1
					| CMD_DATA_PRESENT;
			transfer_mode = SDHOST_DMA_DISABLED | SDHOST_AUTO_CMD12_DISABLED
					| SDHOST_SINGLE_BLK | SDHOST_WRITE;

			/* (5) */
			*SDHOST_TM_CMD = (cmd << 16) | transfer_mode;

#ifdef __USE_SDHOST_INTERRUPT_
			/* (6) Wait for Command Complete interrupt */
			while (!cmd_complete)
			;

			/* (7) Reset interrupt flag */
			cmd_complete = false;
#else
			/* (6) Wait for the Command Complete bit in the Normal Status register to be set */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_CMD_COMPLETE) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (7) Clear the status flag by writing 1 to it */
			write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif

			/* (8) Get response data */
			/* sdhost_get_response(&response, SDHOST_RESPONSE_R1, false); -- not checked */

#ifdef __USE_SDHOST_INTERRUPT_
			/* (10) Wait for Buffer Write Ready Int */
			while (!write_buf_ready)
			;

			/* (11) Clear interrupt flag */
			write_buf_ready = false;
#else
			/* (10) Wait for Buffer Write Ready bit */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_BUF_W_RDY) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (11) Clear the flag by writing 1 to the flag bit */
			write_sdhost_reg(NORM_INT_STATUS_BUF_W_RDY, SDH_NRML_INT_STATUS);
#endif

			/*
			 * (12) Write block data to Buffer Data Port register
			 * Note: To improve performance, use inline assembly here
			 */

			__asm__ (
					"streamout.l    %0, %1, %2    \n\t"
					: /* output registers */
					: "r" (&(*SDHOST_BUFDATAPORT)), "r" (&buf32[nextIdx]), "r" (num_bytes_left) /* input registers */
			);

		} else if (direction == SDHOST_READ) {

			/* (3) Set Argument 1 Register */
			*SDHOST_ARG1 = addr;

			/* (4) Set Transfer Mode Register and (5) Command Register at the same time */
			cmd = (17 << 8) | CMD_TYPE_NORMAL | CMD_RESPONSE_R1
					| CMD_DATA_PRESENT;
			transfer_mode = SDHOST_DMA_DISABLED | SDHOST_AUTO_CMD12_DISABLED
					| SDHOST_SINGLE_BLK | SDHOST_READ;

			/* (5) */
			*SDHOST_TM_CMD = (cmd << 16) | transfer_mode;

#ifdef __USE_SDHOST_INTERRUPT_
			/* (6) Wait for Command Complete interrupt */
			while (!cmd_complete)
			;

			/* (7) Reset interrupt flag */
			cmd_complete = false;
#else
			/* (6) Wait for the Command Complete bit in the Normal Status register to be set */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_CMD_COMPLETE) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (7) Clear the status flag by writing 1 to it */
			write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif

			/* (8) Get response data */
			/* sdhost_get_response(&response, SDHOST_RESPONSE_R1, false); -- not checked */

#ifdef __USE_SDHOST_INTERRUPT_
			/* (14) Wait for Buffer Read Ready Int */
			while (!read_buf_ready)
			;

			/* (15) Clear interrupt flag */
			read_buf_ready = false;
#else
			/* (14) Wait for Buffer Read Ready Bit */
			timeout = COMM_TIMEOUT_U32;
			while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
					& NORM_INT_STATUS_BUF_R_RDY) == 0) && (--timeout != 0))
				;
			DBG_TIMEOUT()
			if (timeout == 0)
			{
				sdhost_error_recovery();
				sdhost_status = SDHOST_CMD_TIMEOUT;
				goto __exit;
			}

			/* (15) Clear the flag by writing 1 to the flag bit */
			write_sdhost_reg(NORM_INT_STATUS_BUF_R_RDY, SDH_NRML_INT_STATUS);
#endif
			/*
			 * (16) Read block data from Buffer Data Port register
			 * Note: To improve performance, use inline assembly here.
			 */

			__asm__ (
					"streamin.l        %0, %1, %2    \n\t"
					: /* output registers */
					: "r" (&buf32[nextIdx]), "r" (&(*SDHOST_BUFDATAPORT)), "r" (num_bytes_left) /* input registers */
			);
		}

#ifdef __USE_SDHOST_INTERRUPT_
		/* (18) Wait for the Transfer Complete interrupt */
		while (!transfer_complete)
		;

		/* (19) Reset flag */
		transfer_complete = false;
#else
		/* (18) Wait for the Transfer Complete bit in the Normal Status register to be set */
		timeout = COMM_TIMEOUT_U32;
		while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
				& NORM_INT_STATUS_TRANSFER_COMPLETE) == 0) && (--timeout != 0))
			;
		DBG_TIMEOUT()
		if (timeout == 0)
		{
			sdhost_error_recovery();
			sdhost_status = SDHOST_CMD_TIMEOUT;
			goto __exit;
		}

		/* (19) Clear the status flag by writing 1 to it */
		write_sdhost_reg(NORM_INT_STATUS_TRANSFER_COMPLETE,
		SDH_NRML_INT_STATUS);
#endif

	}

	__exit:
	return (sdhost_status);
}

/** @brief abort current SD controller host operation
 *
 *  Function sends Abort Command following the Asynchronous Abort Sequence
 *  ref: (SD Host Simplified Spec page 101).
 *
 *  @param none
 *  @returns a SD Host Status
 */

SDHOST_STATUS sdhost_abort(void) {
#ifdef SD_HOST_DEBUG
	tfp_printf("sdhost_abort\r\n");
#endif // SD_HOST_DEBUG
	SDHOST_STATUS sdhost_status = SDHOST_OK;
	uint32_t response;
	uint32_t timeout;

	sdhost_status = sdhost_send_command(SDCMD_STOP_TRANSMISSION, SDHOST_BUS_CMD, NOT_IN_USE, 0,
			&response);
	if (sdhost_status != SDHOST_OK)
	{
#ifdef SD_HOST_DEBUG
		tfp_printf("ERROR %d\r\n", sdhost_status);
#endif // SD_HOST_DEBUG
		goto __exit;
	}

	/* Set Software Reset For DAT line (soft_rst_dat = 1) and CMD line (soft_rst_cmd = 1) */
	timeout = COMM_TIMEOUT_U32;
	do {
		write_sdhost_reg(SW_RESET_CMD_LINE | SW_RESET_DATA_LINE, SDH_SW_RST);
	} while (((read_sdhost_reg(SDH_SW_RST) & SW_RESET_CMD_LINE)
			|| (read_sdhost_reg(SDH_SW_RST) & SW_RESET_DATA_LINE))
					&& (--timeout != 0));
	DBG_TIMEOUT()
	if (timeout == 0)
	{
		sdhost_error_recovery();
		sdhost_status = SDHOST_CMD_TIMEOUT;
	}

    __exit:
	return (sdhost_status);
}



#define SET_FUNCTION_HS_MODE        (uint32_t)(0x80FFFFF1)
#define CHECK_FUNCTION_HS_MODE      (uint32_t)(0x00FFFFF1)


SDHOST_STATUS sdhost_send_wide_data_cmd(uint8_t cmd_id, uint32_t arg, uint8_t* pData, uint16_t blk_size)
{
    uint16_t cmd;
    uint16_t transfer_mode;
    uint32_t timeout;
    SDHOST_STATUS ret = SDHOST_OK;

    /*
     *  If only one "whole" block is needed, perform single-block transfer for this block.
     *  Remaining bytes (if any) will be sent in another single-block transfer later.
     *  This also include the case where less than SDHOST_BLK_SIZE bytes are transferred.
     */

    /* (1) Set Block Size register */
    *SDHOST_BS_BC = (*SDHOST_BS_BC & 0xFFFF0000) | blk_size;

    /* (2) Single-block transfer, SDH_BLK_COUNT register will be ignored */

    /* (3) Set Argument 1 Register */
    *SDHOST_ARG1 = arg;

    /* (4) Set Transfer Mode Register and Command Register at the same time */
    cmd = (cmd_id << 8) | CMD_TYPE_NORMAL | CMD_RESPONSE_R1 | CMD_DATA_PRESENT;
    transfer_mode = SDHOST_DMA_DISABLED | SDHOST_AUTO_CMD12_DISABLED | SDHOST_SINGLE_BLK | SDHOST_READ;

    /* (5) Set the command register */
    *SDHOST_TM_CMD = (cmd << 16) | transfer_mode;

#ifdef __USE_SDHOST_INTERRUPT_
    /* (6) Wait for Command Complete interrupt */
    while (!cmd_complete)
    ;

    /* (7) Reset interrupt flag */
    cmd_complete = false;
#else
    /* (6) Wait for the Command Complete bit in the Normal Status register to be set */
    timeout = COMM_TIMEOUT_U32;
    while (((read_sdhost_reg(SDH_NRML_INT_STATUS) & NORM_INT_STATUS_CMD_COMPLETE) == 0) && (--timeout != 0))
        ;
    DBG_TIMEOUT()
    if (timeout == 0)
    {
        sdhost_error_recovery();
        ret = SDHOST_CMD_TIMEOUT;
        goto __exit;
    }

    /*(7) Clear the status flag by writing 1 to it */
    write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif

    /* (8) Get response data */
    //sdhost_get_response(&response, SDHOST_RESPONSE_R1, false);
#ifdef __USE_SDHOST_INTERRUPT_
    /* (14) Wait for Buffer Read Ready Int */
    while (!read_buf_ready)
    ;

    /* (15) Clear interrupt flag */
    read_buf_ready = false;
#else
    /* (14) Wait for Buffer Read Ready */
    timeout = COMM_TIMEOUT_U32;
    while (((read_sdhost_reg(SDH_NRML_INT_STATUS) & NORM_INT_STATUS_BUF_R_RDY) == 0) && (--timeout != 0))
        ;
    DBG_TIMEOUT()
    if (timeout == 0)
    {
        sdhost_error_recovery();
        ret = SDHOST_CMD_TIMEOUT;
        goto __exit;
    }

    /* (15) Clear the flag by writing 1 to the flag bit */
    write_sdhost_reg(NORM_INT_STATUS_BUF_R_RDY, SDH_NRML_INT_STATUS);
#endif

    /*
     * (16) Read block data from Buffer Data Port register
     * Note: To improve performance, use inline assembly here
     */

    __asm__ ( "streamin.l        %0, %1, %2    \n\t"
            : /* output registers */
            : "r" (pData), "r" (&(*SDHOST_BUFDATAPORT)), "r" (SDCARD_SDSTATUS_BLK_SIZE) /* input registers */
    );

#ifdef __USE_SDHOST_INTERRUPT_
    /* (19) Wait for the Transfer Complete interrupt */
    while (!transfer_complete)
    ;

    /* (20)  Reset flag */
    transfer_complete = false;
#else
    /* (19) Wait for the Transfer Complete bit in the Normal Status register to be set */
    timeout = COMM_TIMEOUT_U32;
    while (((read_sdhost_reg(SDH_NRML_INT_STATUS) & NORM_INT_STATUS_TRANSFER_COMPLETE) == 0) && (--timeout != 0))
        ;
    DBG_TIMEOUT()
    if (timeout == 0)
    {
        sdhost_error_recovery();
        ret = SDHOST_CMD_TIMEOUT;
        goto __exit;
    }

    /* (20)  Clear the status flag by writing 1 to it */
    write_sdhost_reg(NORM_INT_STATUS_TRANSFER_COMPLETE, SDH_NRML_INT_STATUS);
#endif

    __exit:
	return ret;

}

bool sdhost_cmd6(void)
{

    uint16_t timeout;
    uint8_t pBuff[512];

    // Checks:
    // 1. CMD6 is only supported in cards with SD Spec v1.10 and above, so we need to check SCR or CSD register [bit 10 of CCC code]
    // 2. CMD6Mode 0 should be sent to check if the card supports the function first, before modifying the function via CMD6Mode 1
    // 3. Additionally the BUSY status in the response specifies whether a switch function can be issued now


    /* Send CMD16 to set the card's block length to 64 bytes, note that this is different from the data transfer block size */


    uint16_t ccc =  (sdhost_context.CSD[2] >> 12) & 0xFFF;
#ifdef SD_HOST_DEBUG
    tfp_printf("CCC = %04X\n", ccc);
#endif
    if((ccc & (1 << 10)) != (1 << 10))
    {
#ifdef SD_HOST_DEBUG
        tfp_printf("CMD6 not supported\n");
#endif
        return false;
    }

    sdhost_send_wide_data_cmd(SDCMD_SET_BUS_WIDTH, CHECK_FUNCTION_HS_MODE, pBuff, SDCARD_SDSTATUS_BLK_SIZE);

    // Bit 1 in the 13th byte should be set, function 1 can be switched and is not busy (busy bit is cleared)
    if( ((pBuff[13] & 0x02) ==  0x02) && ((pBuff[16] & 0x0f) == 0x01) && ((pBuff[30] & 0x02) == 0x00))
    {

        sdhost_send_wide_data_cmd(SDCMD_SET_BUS_WIDTH, SET_FUNCTION_HS_MODE, pBuff, SDCARD_SDSTATUS_BLK_SIZE);
        // read out the bits:
#ifdef SD_HOST_DEBUG
        tfp_printf("SD FUNCTION STATUS = \n");
        for (int i = 0; i < 64; i++)
        {
            tfp_printf("0x%02X,", *((uint8_t*) pBuff + i));
        }
#endif

        // Confirm again that the switch was OK
        if( ((pBuff[13] & 0x02) ==  0x02) && ((pBuff[16] & 0x0f) == 0x01))
        {
            timeout = 1000;
            while(timeout--); // A small delay before changing parametes
            return true;
        }
        else
        {
#ifdef SD_HOST_DEBUG
            tfp_printf("Error while switching to HS mode!");
#endif
        }

    }
    else
    {
#ifdef SD_HOST_DEBUG
        tfp_printf("Couldn't switch to HS mode!");
#endif
    }
    return false;

}

/** @brief Initialise the sdcard host device
 *
 *  Initialises and identifies the SD card that is being connected.
 *  Function sends Abort Command following the Asynchronous Abort Sequence
 *
 *  See SD Host Controller Simplified Specification V3.00 Para. 3.6
 *  The numbers in parantheses indicate the progress of the steps shown in Para 6.1.3
 *  of the simplified specification.
 *
 *  @param none
 *  @returns a SD Host Status
 *
 */

SDHOST_STATUS sdhost_card_init(void) {

	SDHOST_STATUS sdhost_status = SDHOST_OK;
	uint8_t F8 = 0;
	uint8_t SDIO = 0;
	uint32_t response;
	bool initFailed = false;
	uint8_t retry = 0;
	uint32_t timeout;

	if (sdhost_initialisation_status == SDHOST_NOT_INITIALISED)
	{
		// Hardware not initialised yet. That needs to be done first.
		return sdhost_initialisation_status;
	}

	// SD Cards have a defined block size of 512 bytes.
	sdhost_context.block_size = SDHOST_BLK_SIZE;

	do {
		/* Reset init status flag */
		initFailed = false;

		/* (1) SD Bus mode is selected by CMD0, always succeeds */
		sdhost_status = sdhost_send_command(SDCMD_GO_IDLE_STATE, SDHOST_BUS_CMD,
		NOT_IN_USE, 0, &response);

		/* (2) Issue CMD8 to check the high-capacity SD memory card. SD Physical Layer Simplified Spec page 59. */
		sdhost_status = sdhost_send_command(SDCMD_SEND_IF_COND, SDHOST_BUS_CMD,
		NOT_IN_USE, 0x000001AA, &response);

		/* (3) Check if the card is a legacy (non-SD) cards. Legacy cards do not respond to CMD8. */
		if (sdhost_status == SDHOST_CMD_TIMEOUT) {
			/* Legacy card detected, clear F8 flag - SDv1 or MMCv3 */
			F8 = 0;
#ifdef SD_HOST_DEBUG
			tfp_printf("SDv1 or MMCv3 detected.. \r\n");
#endif // SD_HOST_DEBUG
			sdhost_context.cardType = SD_V1;
		} else {
			/*
			 *  The card responded to CMD8. Check validity of the response: CRC, VHS-VCA and check pattern
			 *  CRC:
			 */
			if (sdhost_status == SDHOST_ERROR) {
				initFailed = true;
				retry++;
				continue;
			}

			/* VHS-VCA & check pattern (0xAB) comparison: */
			if ((response & 0x00000FFF) != 0x000001AA) {
				initFailed = true;
				retry++;
				continue;
			}

			/* If we reach here, response for CMD8 is OK, set F8 flag and continue - SDCv2 */
			F8 = 1;
			sdhost_context.cardType = SD_V2;
#ifdef SD_HOST_DEBUG
			tfp_printf("SDCv2 detected..\r\n");
#endif // SD_HOST_DEBUG
		}
	} while (initFailed && (retry < 2)); /* (4) */

	if (initFailed) {
		sdhost_context.internalStatus = SDHOST_CMD8_FAILED;
		sdhost_status = SDHOST_UNUSABLE_CARD;
		goto __exit;
	}


	/* (5) Issue CMD5 to obtain SDIO OCR by setting the voltage window to 0 in the argument */
	sdhost_status = sdhost_send_command(SDCMD_SDIO, SDHOST_BUS_CMD, NOT_IN_USE,
			0, &response);

	if (sdhost_status == SDHOST_CMD_TIMEOUT) {
		/* (6) This card does not support SDIO function */
		sdhost_error_recovery();
		SDIO = 0;

	} else if (sdhost_status == SDHOST_OK) {
		/* (7) TODO: Add support for SDIO cards here */
	} else if (sdhost_status == SDHOST_ERROR) {
		/* This card does not support SDIO function */
		SDIO = 0;

		/* (10) TODO: Check MP (memory present) flag in the response */
	}


	/*
	 *  OCR is available by issuing ACMD41 and setting the voltage window (Bit 23 to bit 0) in the argument to 0.
	 *   CMD55 should be issued before ACMD41.
	 */
	sdhost_status = sdhost_send_command(SDCMD_APP_CMD, SDHOST_BUS_CMD,
	NOT_IN_USE, 0, &response);

	if (sdhost_status != SDHOST_OK) {
		if (sdhost_status != SDHOST_RESPONSE_ERROR) {
			sdhost_context.internalStatus = SDHOST_CMD55_FAILED;
			sdhost_status = SDHOST_ERROR;
			goto __exit;
		}
	}

	/*
	 *  Note: If the voltage window field is set to zero, it is called "inquiry CMD41" that does not start initialization.
	 *        It is used for getting OCR (OCR = response).
	 *        The inquiry ACMD41 shall ignore the other field (bit 31-24) in the argument. So it is safe to use '0' for the argument.
	 */

	sdhost_status = sdhost_send_command(SDCMD_SEND_OP_CON,
			SDHOST_APP_SPECIFIC_CMD, NOT_IN_USE, 0x00000000,
			&(sdhost_context.OCR));


	if (sdhost_status != SDHOST_OK) {
		if ((sdhost_status == SDHOST_CMD_TIMEOUT) && (F8 == 0)) {
			/* This card is not an SD card */
			sdhost_context.internalStatus = SDHOST_ACMD41_FAILED;
			sdhost_status = SDHOST_UNUSABLE_CARD;
		} else {
			if (SDIO) {
				/* Issue CMD3 to get RCA */
				sdhost_status = sdhost_send_command(SDCMD_SEND_RELATIVE_ADDR,
						SDHOST_BUS_CMD, NOT_IN_USE, 0, &response);
				if (sdhost_status == SDHOST_OK) {
					/* RCA is stored in the upper 16 bits of CMD3 response */
					sdhost_context.RCA = response >> 16;
				}
			} else {
				sdhost_context.internalStatus = SDHOST_CMD3_FAILED;
				sdhost_status = SDHOST_UNUSABLE_CARD;
			}
		}
	} else {

		/*
		 *  Issue ACMD41 again to set the voltage window. Check the busy status in the response.
		 *  While the busy is indicated (OCR bit[31] = 0), repeat to issue ACMD41.
		 *
		 *  Note: If the voltage window field (bit 23-0) in the argument is set to non-zero at the first time,
		 *        it is called "first ACMD41" that starts initialization.
		 *
		 *  The other field (bit 31-24) in the argument is effective. Since this host supports high capacity, HCS is set to 1.
		 *  The argument of following ACMD41 shall be the same as that of the first ACMD41.
		 */
		timeout = COMM_TIMEOUT_U32;
		do {

			sdhost_status = sdhost_send_command(SDCMD_APP_CMD, SDHOST_BUS_CMD,
			NOT_IN_USE, 0, &response); /* (23) */

			/* Note: SDSC Card ignores HCS. If HCS is set to 0, SDHC and SDXC Cards never
			 *        return ready status (SD Physical Layer Simplified Spec)
			 */

			/* select 2.7-3.3 voltage window */
			sdhost_status = sdhost_send_command(SDCMD_SEND_OP_CON,
					SDHOST_APP_SPECIFIC_CMD, NOT_IN_USE,
					sdhost_context.OCR | 0x50000000, &(sdhost_context.OCR));

			if (sdhost_status != SDHOST_OK) { /* (24) */
				if (SDIO) {
					/* Issue CMD3 to get RCA */
					sdhost_status = sdhost_send_command(
							SDCMD_SEND_RELATIVE_ADDR, SDHOST_BUS_CMD,
							NOT_IN_USE, 0, &response);
					if (sdhost_status == SDHOST_OK) {
						/* RCA is stored in the upper 16 bits of CMD3 response */
						sdhost_context.RCA = response >> 16;
					}
				} else {
					sdhost_context.internalStatus = SDHOST_ACMD41_FAILED;
					sdhost_status = SDHOST_UNUSABLE_CARD;
				}
				goto __exit;
			}
			delayus(10);
		} while ((!(sdhost_context.OCR & 0x80000000)) && (--timeout != 0));
		DBG_TIMEOUT()
		if (timeout == 0)
		{
			sdhost_error_recovery();
			sdhost_status = SDHOST_CMD_TIMEOUT;
			goto __exit;
		}

#ifdef SD_HOST_DEBUG
		tfp_printf("exit ACMD loop:%08X\r", sdhost_context.OCR);
#endif // SD_HOST_DEBUG
		/*
		 *  (25) If F8 != 0, check CCS bit in the response to determine if the card is a Standard Capacity SD Memory Card
		 *   or High Capacity SD Memory Card
		 */

		if (F8 == 1) {
			if (sdhost_context.OCR & 0x40000000) /* (26) */
			{
				sdhost_context.isSDSCCard = false;
#ifdef SD_HOST_DEBUG
				tfp_printf("Detected SDHC\r\n");
#endif // SD_HOST_DEBUG
			} else {
				sdhost_context.isSDSCCard = true; /* (27) */
#ifdef SD_HOST_DEBUG
				tfp_printf("Detected SDSC\r\n");
#endif // SD_HOST_DEBUG
			}
		} else {
			sdhost_context.isSDSCCard = true;
		}

		/* (32) Issue CMD2 to get CID */
		sdhost_status = sdhost_send_command(SDCMD_ALL_SEND_CID, SDHOST_BUS_CMD,
		NOT_IN_USE, 0, sdhost_context.CID); /* CMD2 responses with R2, which is the 127-bit CID */
		if (sdhost_status != SDHOST_OK) {
			sdhost_context.internalStatus = SDHOST_CMD2_FAILED;
			sdhost_status = SDHOST_ERROR;
			goto __exit;
		}

		/* (33) Issue CMD3 to get RCA */
		sdhost_status = sdhost_send_command(SDCMD_SEND_RELATIVE_ADDR,
				SDHOST_BUS_CMD, NOT_IN_USE, 0, &response);
		if (sdhost_status != SDHOST_OK) {
			sdhost_context.internalStatus = SDHOST_CMD3_FAILED;
			sdhost_status = SDHOST_ERROR;
			goto __exit;
		}

		/* RCA is stored in the upper 16 bits of CMD3 response */
		sdhost_context.RCA = response >> 16;
#ifdef SD_HOST_DEBUG
		tfp_printf("RCA = %08X",sdhost_context.RCA);
#endif // SD_HOST_DEBUG
		/* Issue CMD9 to get CSD */
		sdhost_status = sdhost_send_command(SDCMD_SEND_CSD, SDHOST_BUS_CMD,
		NOT_IN_USE, sdhost_context.RCA << 16, sdhost_context.CSD);
		if (sdhost_status != SDHOST_OK) {
			sdhost_context.internalStatus = SDHOST_CMD9_FAILED;
			sdhost_status = SDHOST_ERROR;
			goto __exit;
		}

#ifdef SD_HOST_DEBUG
		tfp_printf("CSD = ");
		for (int i = 0; i < 16; i++){
			tfp_printf("0x%02X,", *(((uint8_t*) &sdhost_context.CSD[0]) + i));
		}
		tfp_printf("\r\n");
#endif // SD_HOST_DEBUG

		// When interpreting the CSD register note that lowest byte (CRC) of the CSD table is not stored,
		// so all table entries must be right shifted by 8 (bit 127 in the spec is actually bit 119 in the SDHC register and so on)
		// The last byte (bits 127:120) of the CSD.
		if ((sdhost_context.CSD[3] >> 22) == 0)
		{
			// CSD version 1.0: CSD bits 127:126 are 00b
			uint32_t c_size, c_size_mult, read_bl_len;
			uint32_t blen, mult;
			// Bits 83:80 [75:72] in 128 bit wide register.
			// read_bl_len 3:0 from Longword 2 bits 8:11
			read_bl_len = (sdhost_context.CSD[2] >> 8) & 0x0f;
			// Bits 49:47 [41:39] in 128 bit wide register.
			// c_size_mult 2:0 from Longword 1 bits 7:9
			c_size_mult = (sdhost_context.CSD[1] >> 7) & 0x07;
			// Bits 73:62 [65:54] in 128 bit wide register.
			// c_size 9:0 from Longword 1 bits 22:31
			// c_size 11:10 from Longword 2 bits 0:1
			c_size = ((sdhost_context.CSD[2] << 10) | (sdhost_context.CSD[1] >> 22)) & 0xFFF;
			// Start test code for capacity calculation:
			/* 2GB blocks 3970048 - size 2032664576 bytes */
			//c_size = 0xf24; c_size_mult = 7; read_bl_len = 0xa;
			/* 1GB blocks 1984000 - size 1015808000 bytes */
			// c_size = 0xf22; c_size_mult = 7; read_bl_len = 0x9;
			/* 512MB blocks 990976 - size 507379712 bytes */
			// c_size = 0xf1e; c_size_mult = 6; read_bl_len = 0x9;
			/* 256MB blocks 494080 - size 252968960 bytes */
			// c_size = 0xf13; c_size_mult = 5; read_bl_len = 0x9;
			/* 128MB blocks 246016 - size 125960192 bytes */
			// c_size = 0xf03; c_size_mult = 4; read_bl_len = 0x9;
			/* 64MB blocks 121856 - size 62390272 bytes */
			// c_size = 0xedf; c_size_mult = 3; read_bl_len = 0x9;
			/* 32MB blocks 59776 - size 30605312 bytes */
			// c_size = 0x74b; c_size_mult = 3; read_bl_len = 0x9;
			/* 16MB blocks 28800 - size 14745600 bytes */
			// c_size = 0x383; c_size_mult = 3; read_bl_len = 0x9;
			/* 4MB blocks 8192 - size 4194304 bytes */
			// c_size = 0x7ff; c_size_mult = 0; read_bl_len = 0x09;
			// End test code.

			blen = (1 << (read_bl_len));
			mult = (1 << (c_size_mult + 2));

			sdhost_context.capacity = (c_size + 1) * mult * (blen / SDHOST_BLK_SIZE);
#ifdef SD_HOST_DEBUG
			tfp_printf("V1 capacity: C_SIZE = 0x%x, C_SIZE_MULT = 0x%x, READ_BL_LEN = 0x%x\r\n",
					c_size, c_size_mult, read_bl_len);
#endif // SD_HOST_DEBUG
		}
		else
		{
			// CSD version 2.0: CSD bits 127:126 are 01b
			uint32_t c_size;
			//
			// Bits 69:48 [61:40] in 128 bit wide register.
			// c_size 21:0 from Longword 1 bits 29:8
			c_size = ((sdhost_context.CSD[1] >> 8 ) & 0x003FFFFF);

			// Start test code for capacity calculation:
			/* 32GB blocks 61315072 - size 31393316864 bytes */
			// c_size = 0xe9e5;
			/* 8GB blocks 15523840 - size 7948206080 bytes */
			// c_size = 0x3b37;
			// End test code.

			sdhost_context.capacity = (c_size + 1) * ((512 * 1024) / SDHOST_BLK_SIZE);

#ifdef SD_HOST_DEBUG
			tfp_printf("V2 capacity: C_SIZE = 0x%x\r\n", c_size);
#endif // SD_HOST_DEBUG
		}

#ifdef SD_HOST_DEBUG
			tfp_printf("Capacity: 0x%x blocks\r\n", sdhost_context.capacity);
#endif // SD_HOST_DEBUG

		/* Send CMD7 to set the card in Data Transfer mode */
		sdhost_status = sdhost_send_command(SDCMD_SEL_DESEL_CARD,
				SDHOST_BUS_CMD, NOT_IN_USE, sdhost_context.RCA << 16,
				&response);

		if ((sdhost_status != SDHOST_OK) || (response != 0x700)) {
			sdhost_context.internalStatus = SDHOST_CANNOT_ENTER_TRANSFER_STATE;
			sdhost_status = SDHOST_ERROR;
			goto __exit;
		}


		/* Clear transfer complete interrupt flag due to CMD7 */
#ifdef __USE_SDHOST_INTERRUPT_
		transfer_complete = false;
#else
		write_sdhost_reg(NORM_INT_STATUS_TRANSFER_COMPLETE,
		SDH_NRML_INT_STATUS);
#endif

		//sdhost_status = sdhost_send_command(SDCMD_READ_STATUS_REG, SDHOST_BUS_CMD, NOT_IN_USE, sdhost_context.RCA << 16,  &response);

		/* Send ACMD6 to set the card to 4-bit bus mode */
		sdhost_status = sdhost_send_command(SDCMD_APP_CMD, SDHOST_BUS_CMD,
		NOT_IN_USE, sdhost_context.RCA << 16, &response);
		sdhost_status = sdhost_send_command(SDCMD_SET_BUS_WIDTH,
				SDHOST_APP_SPECIFIC_CMD, NOT_IN_USE, 0x2, &response);

		if (sdhost_status != SDHOST_OK) {
			sdhost_context.internalStatus = SDHOST_CANNOT_SET_CARD_BUS_WIDTH;
			sdhost_status = SDHOST_ERROR;
			goto __exit;
		} else {
			/* Set SD host bus width to 4-bit mode */
			write_sdhost_reg(0x03, SDH_HST_CNTL_1);
		}
	}

	/*
	 *  Now set the SD clock to 25 MHz for data transfer
	 *  NOTE: The SDHC "base clock" is 50MHz and NOT 100MHz, that's why we use a divide by 2 factor here.
	 *  See redmine bug 402 - http://glared1.ftdi.local/issues/402
     */
    if(sdhost_cmd6())
    {
#ifdef SD_HOST_DEBUG
        tfp_printf("Try 50MHz\n");
#endif

        /*  Disable SD clock */
        write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) & 0xFFFB, SDH_CLK_CNTL);

        /* Clear the frequency divider and SDHC works at base clock 50 Mhz */
        write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) & 0x003F, SDH_CLK_CNTL);
        //write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) | 0x0000, SDH_CLK_CNTL); // 50 MHz clock

    }
    else
    {
         /*  Disable SD clock */
         write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) & 0xFFFB, SDH_CLK_CNTL);

         /* Clear the frequency divider and load with divide by 2 factor (=1) */
         write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) & 0x003F, SDH_CLK_CNTL);
         write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) | 0x0100, SDH_CLK_CNTL); // 25 MHz clock
    }

    /* Wait for the internal clock to be stable */
	// See redmine bug 402 - http://glared1.ftdi.local/issues/402
    timeout = COMM_TIMEOUT_U32;
    while ((!(read_sdhost_reg(SDH_CLK_CNTL) & CLK_CTRL_CLK_STABLE))
            && (--timeout != 0))
        ;
    DBG_TIMEOUT()

    /* Finally enable SD Clock Output */
    write_sdhost_reg(read_sdhost_reg(SDH_CLK_CNTL) | 0x0004, SDH_CLK_CNTL);

	__exit:

	if (sdhost_status != SDHOST_OK) {
#ifdef SD_HOST_DEBUG
		tfp_printf("---------------- SD HOST CARD INIT ERROR -----------\r\n");
#endif // SD_HOST_DEBUG
	} else {
#ifdef SD_HOST_DEBUG
		tfp_printf("************* SD HOST CARD INIT OK **********\r\n");
		unsigned long __attribute__ ((aligned (4))) data[64 / 4];
		sdhost_get_card_status_reg(data);
#endif // SD_HOST_DEBUG
	}

	/*  Initialisation complete. */
	sdhost_initialisation_status = sdhost_status;

	return (sdhost_status);
}

/** @brief This function returns the contents of SD Status register by sending the ACMD13 command.
 *  @param  response pointer to store the contents of SD Status register (64 bytes). Note that it must be 32 byte aligned!
 *  @returns a SD Host status
 */
void sdhost_get_card_status_reg(uint32_t* pBuff) {

	uint32_t response;
	uint16_t cmd;
	uint16_t transfer_mode;
	uint32_t timeout;

	if (sdhost_initialisation_status == SDHOST_NOT_INITIALISED)
	{
		*pBuff = 0;
		return;
	}

	/* initialise local variables */
	transfer_mode = 0;

	/* Send CMD16 to set the card's block length to 64 bytes, note that this is different from the data transfer block size */

	sdhost_send_command(SDCMD_SEND_BLOCKLEN, SDHOST_BUS_CMD, NOT_IN_USE,
	SDCARD_SDSTATUS_BLK_SIZE, &response);

	// Send CMD55 for ACMD
    sdhost_send_command(SDCMD_APP_CMD, SDHOST_BUS_CMD,
	NOT_IN_USE, sdhost_context.RCA << 16, &response); /* (23) */

	/*
	 *  If only one "whole" block is needed, perform single-block transfer for this block.
	 *  Remaining bytes (if any) will be sent in another single-block transfer later.
	 *  This also include the case where less than SDHOST_BLK_SIZE bytes are transferred.
	 */

	/* (1) Set Block Size register */
	*SDHOST_BS_BC = (*SDHOST_BS_BC & 0xFFFF0000) | SDCARD_SDSTATUS_BLK_SIZE;

	/* (2) Single-block transfer, SDH_BLK_COUNT register will be ignored */

	/* (3) Set Argument 1 Register */
	*SDHOST_ARG1 = 0x00000000;

	/* (4) Set Transfer Mode Register and Command Register at the same time */
	cmd = (SDCMD_READ_STATUS_REG << 8) | CMD_TYPE_NORMAL | CMD_RESPONSE_R1
			| CMD_DATA_PRESENT;
	transfer_mode = SDHOST_DMA_DISABLED | SDHOST_AUTO_CMD12_DISABLED
			| SDHOST_SINGLE_BLK | SDHOST_READ;

	/* (5) Set the command register */
	*SDHOST_TM_CMD = (cmd << 16) | transfer_mode;

#ifdef __USE_SDHOST_INTERRUPT_
	/* (6) Wait for Command Complete interrupt */
	while (!cmd_complete)
	;

	/* (7) Reset interrupt flag */
	cmd_complete = false;
#else
	/* (6) Wait for the Command Complete bit in the Normal Status register to be set */
	timeout = COMM_TIMEOUT_U32;
	while (((read_sdhost_reg(SDH_NRML_INT_STATUS) & NORM_INT_STATUS_CMD_COMPLETE)
			== 0) && (--timeout != 0))
		;
	DBG_TIMEOUT()
	if (timeout == 0)
	{
		sdhost_error_recovery();
		goto __exit;
	}

	/*(7) Clear the status flag by writing 1 to it */
	write_sdhost_reg(NORM_INT_STATUS_CMD_COMPLETE, SDH_NRML_INT_STATUS);
#endif

	/* (8) Get response data */
	//sdhost_get_response(&response, SDHOST_RESPONSE_R1, false);
#ifdef __USE_SDHOST_INTERRUPT_
	/* (14) Wait for Buffer Read Ready Int */
	while (!read_buf_ready)
	;

	/* (15) Clear interrupt flag */
	read_buf_ready = false;
#else
	/* (14) Wait for Buffer Read Ready */
	timeout = COMM_TIMEOUT_U32;
	while (((read_sdhost_reg(SDH_NRML_INT_STATUS) & NORM_INT_STATUS_BUF_R_RDY)
			== 0) && (--timeout != 0))
		;
	DBG_TIMEOUT()
	if (timeout == 0)
	{
		sdhost_error_recovery();
		goto __exit;
	}

	/* (15) Clear the flag by writing 1 to the flag bit */
	write_sdhost_reg(NORM_INT_STATUS_BUF_R_RDY, SDH_NRML_INT_STATUS);
#endif

	/*
	 * (16) Read block data from Buffer Data Port register
	 * Note: To improve performance, use inline assembly here
	 */

	__asm__ ( "streamin.l        %0, %1, %2    \n\t"
			: /* output registers */
			: "r" (pBuff), "r" (&(*SDHOST_BUFDATAPORT)), "r" (SDCARD_SDSTATUS_BLK_SIZE) /* input registers */
	);

#ifdef __USE_SDHOST_INTERRUPT_
	/* (19) Wait for the Transfer Complete interrupt */
	while (!transfer_complete)
	;

	/* (20)  Reset flag */
	transfer_complete = false;
#else
	/* (19) Wait for the Transfer Complete bit in the Normal Status register to be set */
	timeout = COMM_TIMEOUT_U32;
	while (((read_sdhost_reg(SDH_NRML_INT_STATUS)
			& NORM_INT_STATUS_TRANSFER_COMPLETE) == 0) && (--timeout != 0))
		;
	DBG_TIMEOUT()
	if (timeout == 0)
	{
		sdhost_error_recovery();
		goto __exit;
	}

	/* (20)  Clear the status flag by writing 1 to it */
	write_sdhost_reg(NORM_INT_STATUS_TRANSFER_COMPLETE,
	SDH_NRML_INT_STATUS);
#endif
#ifdef SD_HOST_DEBUG
	tfp_printf("SD STATUS = ");
	for (int i = 0; i < 64; i++){
		tfp_printf("0x%02X,", *((uint8_t*) pBuff + i));
	}
#endif // SD_HOST_DEBUG
__exit:
	return;
}

uint32_t sdhost_get_capacity(void)
{
	if (sdhost_initialisation_status != SDHOST_OK)
	{
		return 0;
	}

	return sdhost_context.capacity;
}

/** @brief Get the block size of the attached card.
 *  @returns The number of bytes in each block on the card. For use by erase function.
 */
uint32_t sdhost_get_erase_block_count(void)
{
	if (sdhost_initialisation_status != SDHOST_OK)
	{
		return 0;
	}

	if (sdhost_context.isSDSCCard)
	{
		uint32_t erase_sector_size;
		// Bits 45:39[38:31] in 128 bit wide register.
		erase_sector_size = ((sdhost_context.CSD[1] << 1) | (sdhost_context.CSD[0] >> 31)) & 0x7F;

		// Number of blocks to erase.
		sdhost_context.block_size = (erase_sector_size + 1);

#ifdef SD_HOST_DEBUG
		tfp_printf("V1 erase: erase blocks = 0x%x\r\n", sdhost_context.block_size);
#endif // SD_HOST_DEBUG
	}
	else
	{
		uint32_t cs[16];
		uint32_t au_size;

		/* When interpreting the SD Status response, note that the data is sent to the SDHC in big endian order, with last byte
		 * coming first */
		sdhost_get_card_status_reg(cs);

		// Bits 431:428 in 512 bit wide register, turns out to be bits 84:87 as the data is stored back to front
		// au_size 3:0 from Longword 3 bits 20:23
		au_size = ((cs[2] >> 20) & 0x0f);

		// Start test code for block calculation:
		/* 16kB blocks 32 */
		// au_size = 1;
		/* 32kB blocks 64 */
		// au_size = 2;
		/* 64kB blocks 128 */
		// au_size = 3;
		// End test code.

        // AU SIZE (in bytes) = 8kB << AU_SIZE
        // AU SIZE (in 512b FAT Sectors) = 8kB << AU_SIZE / 512 = 16 << AU_SIZE

		sdhost_context.block_size = (16 << au_size);

		/* Minimum supported size of a block is 16kB */
		/* This is greater then RU and less than or equal to AU - always. */
		//sdhost_context.block_size = 16 * 1024 / SDHOST_BLK_SIZE;

#ifdef SD_HOST_DEBUG
		tfp_printf("V2 erase: erase blocks = 0x%x\r\n", sdhost_context.block_size);
		tfp_printf("V2 erase: AU SIZE = 0x%x\r\n", au_size);
#endif // SD_HOST_DEBUG
	}

	return sdhost_context.block_size;
}

/** @brief Get the sector size of the attached card.
 *  @returns The number of bytes in each block on the card. For use by erase function.
 */
uint32_t sdhost_get_block_size(void)
{
	if (sdhost_initialisation_status != SDHOST_OK)
	{
		return 0;
	}

	return SDHOST_BLK_SIZE;
}


#ifdef SD_HOST_DEBUG

void RegisterDump(void) {

	uint8_t response[4];
	tfp_printf("PRESENT STATE: %08X\r\n", read_sdhost_reg(SDH_PRESENT_STATE));
	tfp_printf("HOST CNTL 1: %08X\r\n", read_sdhost_reg(SDH_HST_CNTL_1));
	tfp_printf("NRM INT STATUS: %08X\r\n", read_sdhost_reg(SDH_NRML_INT_STATUS));
	tfp_printf("ERROR INT STATUS: %08X\r\n", read_sdhost_reg(SDH_ERR_INT_STATUS));


}

void AnalyzeR1Status(uint32_t response_r1) {

	if (response_r1 & SD_OCR_ADDR_OUT_OF_RANGE) {
		tfp_printf("SD_ADDR_OUT_OF_RANGE");
	}

	if (response_r1 & SD_OCR_ADDR_MISALIGNED) {
		tfp_printf("SD_ADDR_MISALIGNED");
	}

	if (response_r1 & SD_OCR_BLOCK_LEN_ERR) {
		tfp_printf("SD_BLOCK_LEN_ERR");
	}

	if (response_r1 & SD_OCR_ERASE_SEQ_ERR) {
		tfp_printf("SD_ERASE_SEQ_ERR");
	}

	if (response_r1 & SD_OCR_BAD_ERASE_PARAM) {
		tfp_printf("SD_BAD_ERASE_PARAM");
	}

	if (response_r1 & SD_OCR_WRITE_PROT_VIOLATION) {
		tfp_printf("SD_WRITE_PROT_VIOLATION");
	}

	if (response_r1 & SD_OCR_LOCK_UNLOCK_FAILED) {
		tfp_printf("SD_LOCK_UNLOCK_FAILED");
	}

	if (response_r1 & SD_OCR_COM_CRC_FAILED) {
		tfp_printf("SD_COM_CRC_FAILED");
	}

	if (response_r1 & SD_OCR_ILLEGAL_CMD) {
		tfp_printf("SD_ILLEGAL_CMD");
	}

	if (response_r1 & SD_OCR_CARD_ECC_FAILED) {
		tfp_printf("SD_CARD_ECC_FAILED");
	}

	if (response_r1 & SD_OCR_CC_ERROR) {
		tfp_printf("SD_CC_ERROR");
	}

	if (response_r1 & SD_OCR_GENERAL_UNKNOWN_ERROR) {
		tfp_printf("SD_GENERAL_UNKNOWN_ERROR");
	}

	if (response_r1 & SD_OCR_STREAM_READ_UNDERRUN) {
		tfp_printf("SD_STREAM_READ_UNDERRUN");
	}

	if (response_r1 & SD_OCR_STREAM_WRITE_OVERRUN) {
		tfp_printf("SD_STREAM_WRITE_OVERRUN");
	}

	if (response_r1 & SD_OCR_CID_CSD_OVERWRIETE) {
		tfp_printf("SD_CID_CSD_OVERWRITE");
	}

	if (response_r1 & SD_OCR_WP_ERASE_SKIP) {
		tfp_printf("SD_WP_ERASE_SKIP");
	}

	if (response_r1 & SD_OCR_CARD_ECC_DISABLED) {
		tfp_printf("SD_CARD_ECC_DISABLED");
	}

	if (response_r1 & SD_OCR_ERASE_RESET) {
		tfp_printf("SD_ERASE_RESET");
	}

	if (response_r1 & SD_OCR_AKE_SEQ_ERROR) {
		tfp_printf("SD_AKE_SEQ_ERROR");
	}

}

#endif // SD_HOST_DEBUG
