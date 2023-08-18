/**
    @file

    @brief 
    API for BOMS interface for FT900 using USB Host stack.

    API functions for BOMS interface on top of USB Host stack. These functions
    provide all the functionality required to implement a BOMS application.
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <ft900_usb.h>
#include <ft900_usb_boms.h>
#include <ft900_usbh.h>
#include <ft900_delay.h>
#include <ft900_usbh_boms.h>

#if defined(__GNUC__)
#include <machine/endian.h>
#elif defined(__MIKROC_PRO_FOR_FT90x__)
#define BYTE_ORDER 1234
#define LITTLE_ENDIAN 1234
#endif

// Debugging only
#undef BOMS_DEBUG

#if defined(BOMS_DEBUG)
#endif

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* TYPES ***************************************************************************/

/* MACROS **************************************************************************/

/**
    @name BOMS Direction Codes
    @brief Internally used BOMS Direction Codes.
 */
//@{
#define BOMS_DIR_IN               0
#define BOMS_DIR_OUT              1
//@}

/**
 * \brief Macro to swap CPU endian to BE used on USB.
 */
//@{
#define SWAP_16(x) \
    ((((x) & 0xff00) >> 8) | \
     (((x) & 0x00ff) << 8))
#define SWAP_32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >>  8) | \
     (((x) & 0x0000ff00) <<  8) | \
     (((x) & 0x000000ff) << 24))
//@}
#if BYTE_ORDER == LITTLE_ENDIAN
#define CPU2BOMS_16(A) (A)
#define BOMS2CPU_16(A) (A)
#define CPU2BOMS_32(A) (A)
#define BOMS2CPU_32(A) (A)
#define CPU2SCSI_16(A) SWAP_16(A)
#define SCSI2CPU_16(A) SWAP_16(A)
#define CPU2SCSI_32(A) SWAP_32(A)
#define SCSI2CPU_32(A) SWAP_32(A)
#else
#define CPU2BOMS_16(A) SWAP_16(A)
#define BOMS2CPU_16(A) SWAP_16(A)
#define CPU2BOMS_32(A) SWAP_32(A)
#define BOMS2CPU_32(A) SWAP_32(A)
#define CPU2SCSI_16(A) (A)
#define SCSI2CPU_16(A) (A)
#define CPU2SCSI_32(A) (A)
#define SCSI2CPU_32(A) (A)
#endif

#define BOMS_USBS BOMS2CPU_32(BOMS_SIG_USBS) // Little Endian converted to CPU
#define BOMS_USBC BOMS2CPU_32(BOMS_SIG_USBC) // Little Endian converted to CPU

/* LOCAL FUNCTIONS / INLINES *******************************************************/

static int8_t boms_scsiInquiry(USBH_BOMS_context *ctx);
static int8_t boms_scsiRequestSense(USBH_BOMS_context *ctx);
static int8_t boms_scsiReadCapacity(USBH_BOMS_context *ctx);
static int8_t boms_scsiReadStart(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len);
static int8_t boms_scsiWriteStart(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len);
static int8_t boms_scsiStatus(USBH_BOMS_context *ctx);
static int8_t boms_phaseCommand(USBH_BOMS_context *ctx,
	boms_commandBlockWrapper_t *cbw);
static int8_t boms_phaseData(USBH_BOMS_context *ctx,
	uint8_t *buffer, uint16_t length, uint8_t dir);
static int8_t boms_phaseResponse(USBH_BOMS_context *ctx, boms_commandStatusWrapper_t *csw);

/* FUNCTIONS ***********************************************************************/

int8_t USBH_BOMS_init(USBH_interface_handle hBomsInterface, uint8_t lun, USBH_BOMS_context *ctx)
{
    int8_t status = USBH_BOMS_ERR_CLASS_NOT_SUPPORTED;
    uint8_t maxLun;
    USBH_endpoint_handle hEp1, hEp2;
    USBH_endpoint_info epInfo;
    USBH_interface_info ifInfo;
#ifdef BOMS_DEBUG
    USBH_device *usbDev;
    USBH_endpoint *usbEp;
#endif // BOMS_DEBUG

    // Initialise context for BOMS
    // NOTE: do not check the class/subclass/protocol. This is the application's job.
    ctx->hBomsInterface = hBomsInterface;
    ctx->tag = 0;
    ctx->lun = lun;

    delayms(100);

#ifdef BOMS_DEBUG
    printf("BOMS - Initialising\n");
#endif // BOMS_DEBUG

    if (USBH_interface_get_info(ctx->hBomsInterface, &ifInfo) == USBH_OK)
    {
    	ctx->hBomsDevice = ifInfo.dev;
    	ctx->bomsInterfaceNumber = ifInfo.interface_number;

        if (USBH_get_endpoint_list(ctx->hBomsInterface, &hEp1) == USBH_OK)
        {
            USBH_get_next_endpoint(hEp1, &hEp2);
        }

#ifdef BOMS_DEBUG
        tfp_printf("BOMS - Device 0x%x\r\n", USBH_HANDLE_ADDR(hBomsDev));
        usbDev = (USBH_device *)USBH_HANDLE_ADDR(hBomsDev);
        usbEp = usbDev->endpoint0;
        tfp_printf("BOMS - Interface Number %d\r\n", ctx->bomsInterfaceNumber);
        tfp_printf("BOMS - Finding endpoints in interface 0x%x\r\n", USBH_HANDLE_ADDR(ctx->hBomsInterface));

        tfp_printf("BOMS - Found Endpoints 0x%x and 0x%x\r\n", USBH_HANDLE_ADDR(hEp1), USBH_HANDLE_ADDR(hEp2));
        tfp_printf("BOMS - Getting Endpoint Information for 0x%x\r\n", USBH_HANDLE_ADDR(hEp1));
#endif // BOMS_DEBUG

        if (USBH_endpoint_get_info(hEp1, &epInfo) == USBH_OK)
        {
            if (epInfo.direction == USBH_DIR_IN)
            {
                ctx->hBomsEpIn = hEp1;
                ctx->hBomsEpOut = hEp2;
            }
            else
            {
                ctx->hBomsEpIn = hEp2;
                ctx->hBomsEpOut = hEp1;
            }

#ifdef BOMS_DEBUG
            tfp_printf("BOMS - Getting Max LUN 0x%04x 0x%04x 0x%04x 0x%04x\n", ctx, usbDev, usbEp, usbEp->hc_queue);
#endif // BOMS_DEBUG

            delayms(50);

            if (USBH_BOMS_get_max_lun(ctx, &maxLun) != USBH_BOMS_OK)
            {
                return USBH_BOMS_ERR_CLASS_NOT_SUPPORTED;
            }

            ctx->maxLun = maxLun;
#ifdef BOMS_DEBUG
            tfp_printf("BOMS - Max LUN %d\n", ctx->maxLun);
#endif // BOMS_DEBUG

            if (lun > maxLun)
            {
#ifdef BOMS_DEBUG
            	tfp_printf("BOMS - Getting Max LUN 0x%04x 0x%04x 0x%04x 0x%04x\n", ctx, usbDev, usbEp, usbEp->hc_queue);
#endif // BOMS_DEBUG
            	return USBH_BOMS_ERR_LUN;
            }

#ifdef BOMS_DEBUG
            tfp_printf("BOMS - Inquiry LUN %d\n", cLun);
#endif // BOMS_DEBUG
            status = boms_scsiInquiry(ctx);
            if (status == USBH_BOMS_OK)
            {
#ifdef BOMS_DEBUG
            	tfp_printf("BOMS - Getting Capacity pass %d\n", i);
#endif // BOMS_DEBUG
            	status = boms_scsiReadCapacity(ctx);

#ifdef BOMS_DEBUG
            	if (status == USBH_BOMS_OK)
            	{
            		tfp_printf("BOMS - Capacity 0x%04x%04x LBAs\n", ctx->lba_count >> 16, ctx->lba_count);
            		tfp_printf("BOMS - LBA size 0x%04x%04x\n", ctx->lba_size >> 16, ctx->lba_size);
            	}
#endif // BOMS_DEBUG
            }
        }
    }

#ifdef BOMS_DEBUG
    tfp_printf("BOMS - Initialisation finished %x\n", status);
#endif // BOMS_DEBUG
    return status;
}

int8_t USBH_BOMS_read(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len, uint8_t *buffer)
{
    int8_t status;

    if (len % ctx->lba_size)
    {
        return USBH_BOMS_ERR_PARAMETER;
    }

    status = boms_scsiReadStart(ctx, lba, len);
    if (status == USBH_BOMS_OK)
    {
        boms_phaseData(ctx, buffer, len, BOMS_DIR_IN);
        status = boms_scsiStatus(ctx);
    }

    return status;
}

int8_t USBH_BOMS_write(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len, uint8_t *buffer)
{
    int8_t status;

    if (len % ctx->lba_size)
    {
        return USBH_BOMS_ERR_PARAMETER;
    }

    status = boms_scsiWriteStart(ctx, lba, len);
    if (status == USBH_BOMS_OK)
    {
        boms_phaseData(ctx, buffer, len, BOMS_DIR_OUT);
        status = boms_scsiStatus(ctx);
    }

    return status;
}

int8_t USBH_BOMS_mult_read_start(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len)
{
    int8_t status;

    if (len % ctx->lba_size)
    {
        return USBH_BOMS_ERR_PARAMETER;
    }

    status = boms_scsiReadStart(ctx, lba, len);
    return status;
}

int8_t USBH_BOMS_mult_read_data(USBH_BOMS_context *ctx, uint32_t len, uint8_t *buffer)
{
    int8_t status;

    if (len % ctx->lba_size)
    {
        return USBH_BOMS_ERR_PARAMETER;
    }

    status = boms_phaseData(ctx, buffer, len, BOMS_DIR_IN);
    return status;
}

int8_t USBH_BOMS_mult_write_start(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len)
{
    int8_t status;

    if (len % ctx->lba_size)
    {
        return USBH_BOMS_ERR_PARAMETER;
    }

    status = boms_scsiWriteStart(ctx, lba, len);
    return status;
}

int8_t USBH_BOMS_mult_write_data(USBH_BOMS_context *ctx, uint32_t len, uint8_t *buffer)
{
    int8_t status;

    if (len % ctx->lba_size)
    {
        return USBH_BOMS_ERR_PARAMETER;
    }

    status = boms_phaseData(ctx, buffer, len, BOMS_DIR_OUT);
    return status;
}

int8_t USBH_BOMS_mult_end(USBH_BOMS_context *ctx)
{
    int8_t status;
    status = boms_scsiStatus(ctx);
    return status;
}

// Bulk-Only Mass Storage Get Max LUN
// API
int8_t USBH_BOMS_reset(USBH_BOMS_context *ctx)
{
    int32_t status;
    USB_device_request devReq;

    devReq.bRequest = BOMS_REQUEST_RESET;
    devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
            USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
    devReq.wIndex = ctx->bomsInterfaceNumber;
    devReq.wValue = 0;
    devReq.wLength = 0;

    status = USBH_device_setup_transfer(ctx->hBomsDevice, &devReq, 0, 0);

    //TODO: if status is stall then clear stall
    if (status < 0)
        return USBH_BOMS_ERR_CLASS;
    return USBH_BOMS_OK;
}

// Bulk-Only Mass Storage Get Max LUN
// API
int8_t USBH_BOMS_get_max_lun(USBH_BOMS_context *ctx, uint8_t *maxLun)
{
    int32_t status;
    USB_device_request devReq;
    uint8_t maxLunAlign;

    devReq.bRequest = BOMS_REQUEST_GET_MAX_LUN;
    devReq.bmRequestType = USB_BMREQUESTTYPE_DIR_DEV_TO_HOST |
            USB_BMREQUESTTYPE_CLASS | USB_BMREQUESTTYPE_RECIPIENT_INTERFACE;
    devReq.wIndex = ctx->bomsInterfaceNumber;
    devReq.wValue = 0;
    devReq.wLength = 1;

    status = USBH_device_setup_transfer(ctx->hBomsDevice, &devReq, &maxLunAlign, 500);

    // Copy aligned value to return value.
    *maxLun = maxLunAlign;

    //TODO: if status is stall then clear stall
    if (status < 0)
        return USBH_BOMS_ERR_CLASS;
    return USBH_BOMS_OK;
}

// Get Status
// API
int8_t USBH_BOMS_status(USBH_BOMS_context *ctx)
{
	// Return a success or failure depending on the result of a
	// request sense.
	return boms_scsiRequestSense(ctx);
}

static int8_t boms_scsiInquiry(USBH_BOMS_context *ctx)
{
    boms_commandBlockWrapper_t cbw;
    boms_scsi_CRB_inquiry_t inq;
    boms_commandStatusWrapper_t csw;
    int8_t status;
    unsigned char retry;

    memset(&cbw, 0, sizeof(boms_commandBlockWrapper_t));

    cbw.dCBWSignature = CPU2BOMS_32(BOMS_USBC);
    cbw.dCBWDataTransferLength = CPU2BOMS_32(BOMS_SCSI_INQUIRY_TRANSFER_LEN);
    cbw.bmCBWFlags = CBW_FLAG_DIRECTION_IN;
    cbw.bCBWLUN = ctx->lun;
    cbw.bCBWCBLength = sizeof(boms_scsi_CDB_inquiry_t);
    cbw.CBWCB.inquiry.op_code = BOMS_SCSI_INQUIRY;
    cbw.CBWCB.inquiry.length = BOMS_SCSI_INQUIRY_TRANSFER_LEN;
    cbw.CBWCB.inquiry.lun = ctx->lun;

    retry = 3;
    do
    {
        ctx->tag++;
        cbw.dCBWTag = CPU2BOMS_32(ctx->tag);

        status = boms_phaseCommand(ctx, &cbw);
        if (status == USBH_BOMS_OK)
        {
            status = boms_phaseData(ctx, (uint8_t *)&inq, BOMS_SCSI_INQUIRY_TRANSFER_LEN, BOMS_DIR_IN);
			if ((status == USBH_OK) || (status == USBH_ERR_HALTED) || (status == USBH_ERR_STALLED))
			{
                status = boms_phaseResponse(ctx, &csw);
                if (status != USBH_BOMS_OK)
                {
                    boms_scsiRequestSense(ctx);
                }
            }
        }
        retry--;

        if (!retry)
            break;
    }
    while (status != USBH_OK);

    // add vid pid etc to context
    memcpy(&ctx->vendorId, &inq.vid, 8);
    memcpy(&ctx->productId, &inq.pid, 16);
    memcpy(&ctx->rev, &inq.rev, 4);

    if (status == USBH_OK)
    	return USBH_BOMS_OK;
    return USBH_BOMS_ERR_SCSI;
}

static int8_t boms_scsiRequestSense(USBH_BOMS_context *ctx)
{
    boms_commandBlockWrapper_t cbw;
    boms_scsi_CRB_request_sense_t sense;
    boms_commandStatusWrapper_t csw;
    int8_t status;

    ctx->tag++;
    memset(&cbw, 0, sizeof(boms_commandBlockWrapper_t));

    cbw.dCBWSignature = CPU2BOMS_32(BOMS_USBC);
    cbw.dCBWTag = CPU2BOMS_32(ctx->tag);
    cbw.dCBWDataTransferLength = CPU2BOMS_32(sizeof(boms_scsi_CRB_request_sense_t));
    cbw.bmCBWFlags = CBW_FLAG_DIRECTION_IN;
    cbw.bCBWLUN = ctx->lun;
    cbw.bCBWCBLength = sizeof(boms_scsi_CDB_request_sense_t);
    cbw.CBWCB.request_sense.op_code = BOMS_SCSI_REQUEST_SENSE;
    cbw.CBWCB.request_sense.length = sizeof(boms_scsi_CRB_request_sense_t);
    cbw.CBWCB.request_sense.lun = ctx->lun;

    status = boms_phaseCommand(ctx, &cbw);
    if (status == USBH_OK)
    {
        status = boms_phaseData(ctx, (uint8_t *)&sense, sizeof(boms_scsi_CRB_request_sense_t), BOMS_DIR_IN);
		if ((status == USBH_OK) || (status == USBH_ERR_HALTED))
		{
            status = boms_phaseResponse(ctx, &csw);
            if (csw.bCSWStatus)
            {
            	return USBH_BOMS_ERR_STATUS;
            }
        }
    }

    if (status == USBH_OK)
    	return USBH_BOMS_OK;
    return USBH_BOMS_ERR_SCSI;
}

static int8_t boms_scsiReadCapacity(USBH_BOMS_context *ctx)
{
    boms_commandBlockWrapper_t cbw;
    boms_scsi_CRB_read_capacity_t cap;
    boms_commandStatusWrapper_t csw;
    int8_t status;
    unsigned char retry;

    ctx->lba_count = 0;
    ctx->lba_size = 0;

    memset(&cbw, 0, sizeof(boms_commandBlockWrapper_t));

    cbw.dCBWSignature = CPU2BOMS_32(BOMS_USBC);
    cbw.dCBWDataTransferLength = CPU2BOMS_32(sizeof(boms_scsi_CRB_read_capacity_t));
    cbw.bmCBWFlags = CBW_FLAG_DIRECTION_IN;
    cbw.bCBWLUN = ctx->lun;
    cbw.bCBWCBLength = sizeof(boms_scsi_CDB_read_capacity_t);
    cbw.CBWCB.read_capacity.op_code = BOMS_SCSI_READ_CAPACITY;
    cbw.CBWCB.read_capacity.lun = ctx->lun;

    retry = 20;
    do {
        ctx->tag++;
        cbw.dCBWTag = CPU2BOMS_32(ctx->tag);

        status = boms_phaseCommand(ctx, &cbw);
        if (status == USBH_OK)
        {
            status = boms_phaseData(ctx, (uint8_t *)&cap, sizeof(boms_scsi_CRB_read_capacity_t), BOMS_DIR_IN);

			if ((status == USBH_OK) || (status == USBH_ERR_HALTED))
			{
				status = boms_phaseResponse(ctx, &csw);

				if (csw.bCSWStatus == 0)
				{
					break;
				}

				boms_scsiRequestSense(ctx);
				status = USBH_BOMS_ERR_CAPACITY_TIMEOUT;
			}
        }

        delayms(50);
    }
    while (retry--);

    ctx->lba_count = SCSI2CPU_32(cap.lba);
    ctx->lba_size = SCSI2CPU_32(cap.block_len);

    if (status == USBH_OK)
    	return USBH_BOMS_OK;
    return USBH_BOMS_ERR_SCSI;
}

static int8_t boms_scsiReadStart(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len)
{
    boms_commandBlockWrapper_t cbw;
    int8_t status;

    memset(&cbw, 0, sizeof(boms_commandBlockWrapper_t));

    cbw.dCBWSignature = CPU2BOMS_32(BOMS_USBC);
    cbw.dCBWDataTransferLength = CPU2BOMS_32(len);
    cbw.bmCBWFlags = CBW_FLAG_DIRECTION_IN;
    cbw.bCBWLUN = ctx->lun;
    cbw.bCBWCBLength = sizeof(boms_scsi_CDB_read_t);
    ctx->tag++;
    cbw.dCBWTag = CPU2BOMS_32(ctx->tag);

    len = len / ctx->lba_size;
    cbw.CBWCB.read.op_code = BOMS_SCSI_READ;
    cbw.CBWCB.read.lba = CPU2SCSI_32(lba);
    cbw.CBWCB.read.length = CPU2SCSI_16(len);
    cbw.CBWCB.read.lun = ctx->lun;

    status = boms_phaseCommand(ctx, &cbw);

    if (status == USBH_OK)
    	return USBH_BOMS_OK;
    return USBH_BOMS_ERR_SCSI;
}

static int8_t boms_scsiWriteStart(USBH_BOMS_context *ctx, uint32_t lba, uint32_t len)
{
    boms_commandBlockWrapper_t cbw;
    int8_t status;

    memset(&cbw, 0, sizeof(boms_commandBlockWrapper_t));

    cbw.dCBWSignature = CPU2BOMS_32(BOMS_USBC);
    cbw.dCBWDataTransferLength = CPU2BOMS_32(len);
    cbw.bmCBWFlags = CBW_FLAG_DIRECTION_OUT;
    cbw.bCBWLUN = ctx->lun;
    cbw.bCBWCBLength = sizeof(boms_scsi_CDB_write_t);
    ctx->tag++;
    cbw.dCBWTag = CPU2BOMS_32(ctx->tag);

    len = len / ctx->lba_size;
    cbw.CBWCB.write.op_code = BOMS_SCSI_WRITE;
    cbw.CBWCB.write.lba = CPU2SCSI_32(lba);
    cbw.CBWCB.write.length = CPU2SCSI_16(len);
    cbw.CBWCB.write.lun = ctx->lun;

    status = boms_phaseCommand(ctx, &cbw);

    if (status == USBH_OK)
    	return USBH_BOMS_OK;
    return USBH_BOMS_ERR_SCSI;
}

static int8_t boms_scsiStatus(USBH_BOMS_context *ctx)
{
    boms_commandStatusWrapper_t csw;
    int8_t status;

    status = boms_phaseResponse(ctx, &csw);
    if (csw.bCSWStatus)
    {
        return USBH_BOMS_ERR_STATUS;
    }

    if (status == USBH_OK)
    	return USBH_BOMS_OK;
    return USBH_BOMS_ERR_SCSI;
}

static int8_t boms_phaseCommand(USBH_BOMS_context *ctx,
        boms_commandBlockWrapper_t *cbw)
{
    int32_t result;
    int8_t status = USBH_OK;

    result = USBH_transfer(ctx->hBomsEpOut, (uint8_t *)cbw, sizeof(boms_commandBlockWrapper_t), 0);

    if (result < 0)
    {
        status = (int8_t)result;

        if ((status == USBH_ERR_HALTED) || (status == USBH_ERR_STALLED))
        {
            // Stalled BOMS - reset recovery 5.3.4.
            USBH_BOMS_reset(ctx);
            USBH_endpoint_halt(ctx->hBomsEpIn, USB_REQUEST_CODE_CLEAR_FEATURE);
            USBH_endpoint_halt(ctx->hBomsEpOut, USB_REQUEST_CODE_CLEAR_FEATURE);
        }
    }

    return status;
}

static int8_t boms_phaseData(USBH_BOMS_context *ctx,
        uint8_t *buffer, uint16_t length, uint8_t dir)
{
    int32_t result;
    int8_t status = USBH_OK;
    USBH_endpoint_handle ep;

    if (dir == BOMS_DIR_IN)
    {
        ep = ctx->hBomsEpIn;
    }
    else
    {
        ep = ctx->hBomsEpOut;
    }

    result = USBH_transfer(ep, buffer, length, 0);

    if (result < 0)
    {
        status = (int8_t)result;

        if ((status == USBH_ERR_HALTED) || (status == USBH_ERR_STALLED) || (status == USBH_ERR_USBERR))
        {
            status = USBH_interface_clear_host_halt(ep);
            status = USBH_endpoint_halt(ep, USB_REQUEST_CODE_CLEAR_FEATURE);
        }
        else if (status == USBH_ERR_DATA_BUF)
        {
        	status = USBH_interface_clear_host_halt(ep);
        	status = USBH_clear_endpoint_transfers(ep);
        	status = USBH_endpoint_halt(ep, USB_REQUEST_CODE_CLEAR_FEATURE);
        }
    }

    return status;
}

static int8_t boms_phaseResponse(USBH_BOMS_context *ctx, boms_commandStatusWrapper_t *csw)
{
	int32_t result;
    int8_t status = USBH_OK;

    result = USBH_transfer(ctx->hBomsEpIn, (uint8_t *)csw, sizeof(boms_commandStatusWrapper_t), 0);

    if (result < 0)
    {
    	status = (int8_t)result;

        if ((status == USBH_ERR_HALTED) || (status == USBH_ERR_STALLED))
        {
            USBH_endpoint_halt(ctx->hBomsEpIn, USB_REQUEST_CODE_CLEAR_FEATURE);
            result = USBH_transfer(ctx->hBomsEpIn, (uint8_t *)csw, sizeof(boms_commandStatusWrapper_t), 0);
            if (result < 0)
            {
                status = (int8_t)result;
            }
            else
            {
            	status = USBH_OK;
            }
        }
    }
    else
    {
        if (csw->bCSWStatus == BOMS_STATUS_PHASE_ERROR)
        {
            // Stalled BOMS - reset recovery 5.3.4.
            USBH_BOMS_reset(ctx);
            USBH_endpoint_halt(ctx->hBomsEpIn, USB_REQUEST_CODE_CLEAR_FEATURE);
            USBH_endpoint_halt(ctx->hBomsEpOut, USB_REQUEST_CODE_CLEAR_FEATURE);
        }
    }

    return status;
}

