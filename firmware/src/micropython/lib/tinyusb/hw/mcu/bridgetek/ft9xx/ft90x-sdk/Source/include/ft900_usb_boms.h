/**
    @file ft900_usb_boms.h

    @brief 
    USB BOMS class definitions.
  
    This contains USB definitions of constants and structures which are used in
    USB BOMS implementations.

    Definitions for USB BOMS Devices based on USB Mass Storage Bulk Only
    Specification Revision 1.0 from http://www.usb.org/developers/docs/

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

#ifndef FT900_USB_BOMS_H_
#define FT900_USB_BOMS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* INCLUDES ************************************************************************/

#include <ft900_usb.h>

/* CONSTANTS ***********************************************************************/

/**
    @name USB BOMS Request Codes
    @brief BOMS Request Codes.
        Used as bRequest in USB_device_request and bDescriptorType
        in USB_device_descriptor.
 */
//@{
#define BOMS_REQUEST_RESET        0xff
#define BOMS_REQUEST_GET_MAX_LUN  0xfe
//@}

/**
    @name USB BOMS Command Block Status Values
    @brief Table 5.3 - Command Block Status Values. bCSWStatus.
 */
//@{
#define BOMS_STATUS_OK             0x00 /// Command Passed ("good status")
#define BOMS_STATUS_COMMAND_FAILED 0x01 /// Command Failed
#define BOMS_STATUS_PHASE_ERROR    0x02 /// Phase Error
/// 0x03 and 0x04 Reserved (Obsolete)
/// 0x05 to 0xFF Reserved
//@}

/**
    @name USB BOMS SCSI Command Codes
    @brief From SCSI command code spec.
 */
//@{
/** @brief SCSI Test Unit Ready (6)
 * Query if the device is ready to respond to requests.
 */
#define BOMS_SCSI_TEST_UNIT_READY 	0x00
/** @brief SCSI Request Sense (12)
 * Return status sense data of last operation.
 */
#define BOMS_SCSI_REQUEST_SENSE 	0x03
/** @brief SCSI Inquiry (6)
 * Get device information.
 */
#define BOMS_SCSI_INQUIRY 			0x12
/** @brief SCSI Read Format Capacity (10)
 * Return media capacity and supported formatable capacities.
 */
#define BOMS_SCSI_READ_FORMAT_CAPACITY 	0x23
/** @brief SCSI Read Capacity (10)
 * Return capacity of media.
 */
#define BOMS_SCSI_READ_CAPACITY 	0x25
/** @brief SCSI Read Capacity (16)
 * Return capacity of media.
 */
#define BOMS_SCSI_READ_CAPACITY_16 	0x9e
/** @brief SCSI Mode Sense (6)
 * Return parameters.
 */
#define BOMS_SCSI_MODE_SENSE        0x1a
/** @brief SCSI Prevent Allow Medium Removal (6)
 * Not applicable for SD cards.
 */
#define BOMS_SCSI_PREVENT_ALLOW_REMOVAL 0x1e
/** @brief SCSI Read (10)
 * Transfer data. Up to 65535 byte max transfer.
 */
#define BOMS_SCSI_READ 				0x28
/** @brief SCSI Write (10)
 * Transfer data. Up to 65535 byte max transfer.
 */
#define BOMS_SCSI_WRITE 			0x2A
//@}

/**
    @name USB BOMS SCSI Command Block Direction Codes
    @brief From SCSI command code spec.
 */
//@{
#define CBW_FLAG_DIRECTION_IN  		0x80
#define CBW_FLAG_DIRECTION_OUT		0x00
//@}

/**
    @name USB BOMS Signatures
    @brief For BOMS command and status wrappers.
 */
//@{
#define BOMS_SIG_USBS 0x53425355
#define BOMS_SIG_USBC 0x43425355
//@}

/**
 @brief Sense Key responses for REQUEST SENSE SCSI requests.
 */
//@{
#define SCSI_SENSE_KEY_NO_SENSE 0x00
#define SCSI_SENSE_KEY_RECOVERED_ERROR 0x01
#define SCSI_SENSE_KEY_NOT_READY 0x02
#define SCSI_SENSE_KEY_MEDIUM_ERROR 0x03
#define SCSI_SENSE_KEY_HARDWARE_ERROR 0x04
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST 0x05
#define SCSI_SENSE_KEY_UNIT_ATTENTION 0x6
#define SCSI_SENSE_KEY_DATA_PROTECT 0x07
#define SCSI_SENSE_KEY_BLANK_CHECK 0x08
#define SCSI_SENSE_KEY_COPY_ABORTED 0x0a
#define SCSI_SENSE_KEY_ABORTED_COMMAND 0x0b
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW 0x0d
#define SCSI_SENSE_KEY_MISCOMPARE 0x0e
//@}

/**
 @brief Additional Sense responses and Additional Sense Qualifier responses
 	 for REQUEST SENSE SCSI requests.
 */
//@{
#define SCSI_SENSE_KEY_ADDN_NO_ERROR 0x0000
#define SCSI_SENSE_KEY_ADDN_NOT_READY_CAUSE_NOT_KNOWN 0x0400
#define SCSI_SENSE_KEY_ADDN_NOT_READY_BECOMING_READY 0x0401
#define SCSI_SENSE_KEY_ADDN_NOT_READY_NEED_INIT_COMMAND 0x0402
#define SCSI_SENSE_KEY_ADDN_NOT_READY_MANUAL_INTERVENTION_REQUIRED 0x0403
#define SCSI_SENSE_KEY_ADDN_NOT_READY_MEDIUM_NOT_PRESENT 0x3A00
#define SCSI_SENSE_KEY_ADDN_NOT_READY_MEDIUM_NOT_PRESENT_TRAY_CLOSED 0x3A01
#define SCSI_SENSE_KEY_ADDN_NOT_READY_MEDIUM_NOT_PRESENT_TRAY_OPEN 0x3A02
#define SCSI_SENSE_KEY_ADDN_MEDIUM_ERROR_WRITE_FAULT 0x0300
#define SCSI_SENSE_KEY_ADDN_MEDIUM_ERROR_UNRECOVERED_READ_ERROR 0x1100
#define SCSI_SENSE_KEY_ADDN_HARDWARE_ERROR_NO_INDEX_OR_SECTOR 0x0100
#define SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_COMMAND 0x2000
#define SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_LBA_OUT_OF_RANGE 0x2100
#define SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_FIELD_IN_CDB 0x2400
#define SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_LUN 0x2500
#define SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_FIELDS_IN_PARM 0x2600
#define SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_PARAMETER_NOT_SUPPORTED 0x2601
#define SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_INVALID_PARM_VALUE 0x2602
#define SCSI_SENSE_KEY_ADDN_ILLEGAL_REQUEST_COMMAND_SEQUENCE_ERROR 0x2C00
#define SCSI_SENSE_KEY_ADDN_WRITE_PROTECT_COMMAND_NOT_ALLOWED 0x2700
#define SCSI_SENSE_KEY_ADDN_ABORTED_COMMAND_NO_ADDITIONAL_CODE 0x0000
//@}

/* TYPES ***************************************************************************/

/**
    @name SCSI Command Descriptor Blocks (CDBs)
    @brief Command block definitions for SCSI commands.
 */
//@{
typedef struct PACK boms_scsi_CDB_test_unit_ready_t
{
    unsigned char op_code;             /// Operation code = 00h
    unsigned char R1;                  /// Reserved
    unsigned char R2;                  /// Reserved
    unsigned char R3;                  /// Reserved
    unsigned char R4;                  /// Reserved
    unsigned char control;             /// Control
} boms_scsi_CDB_test_unit_ready_t;

typedef struct PACK boms_scsi_CDB_request_sense_t
{
    unsigned char op_code;             /// Operation code = 03h
    unsigned char R1 : 5;              /// Reserved
    unsigned char lun : 3;             /// LUN
    unsigned char R2;                  /// Reserved
    unsigned char R3;                  /// Reserved
    unsigned char length;              /// Allocation length
    unsigned char control;             /// Control
} boms_scsi_CDB_request_sense_t;

typedef struct PACK boms_scsi_CDB_mode_sense_t
{
    unsigned char op_code;             /// Operation code = 03h
    unsigned char R1 : 3;              /// Reserved
    unsigned char dbd : 1;             /// DBD
    unsigned char R2 : 4;              /// Reserved
    unsigned char page_code : 6;       /// Page Code
    unsigned char pc : 2;              /// Page Control
    unsigned char sub_page_code;       /// Sub Page Code
    unsigned char length;              /// Allocation length
    unsigned char control;             /// Control
} boms_scsi_CDB_mode_sense_t;

typedef struct PACK boms_scsi_CDB_inquiry_t
{
    unsigned char op_code;             /// Operation code = 12h
    unsigned char evpd : 1;            /// EVPD
    unsigned char R1 : 4;              /// Reserved
    unsigned char lun : 3;             /// LUN
    unsigned char page_code;           /// Page Code
    unsigned char R2;                  /// Reserved
    unsigned char length;              /// Allocation length
    unsigned char control;             /// Control
} boms_scsi_CDB_inquiry_t;

typedef struct PACK boms_scsi_CDB_removal_t
{
    unsigned char op_code;             /// Operation code = 12h
    unsigned char R1 : 5;              /// Reserved
    unsigned char lun : 3;             /// LUN
    unsigned char R2[2];               /// Reserved
    unsigned char prevent : 1;         /// Prevent removal
    unsigned char R3 : 7;              /// Reserved
    unsigned char R4[7];               /// Reserved
} boms_scsi_CDB_removal_t;


typedef struct PACK boms_scsi_CDB_read_capacity_t
{
    unsigned char op_code;             /// Operation code = 25h
    unsigned char rel_adr : 1;         /// RelAdr
    unsigned char R1 : 4;              /// Reserved
    unsigned char lun : 3;             /// LUN
    unsigned long lba;                 /// Logical Block Address
    unsigned char R2;                  /// Reserved
    unsigned char R3;                  /// Reserved
    unsigned char pmi : 1;             /// Partial Medium Indicator
    unsigned char R4 : 7;              /// Reserved
    unsigned char control;             /// Control
} boms_scsi_CDB_read_capacity_t;

typedef struct PACK boms_scsi_CDB_read_format_capacity_t
{
    unsigned char op_code;             /// Operation code = 23h
    unsigned char R1 : 5;              /// Reserved
    unsigned char lun : 3;             /// LUN
    unsigned char R2[5];               /// Reserved
    unsigned short alloc_length;       /// Allocation Length
    unsigned char R3[3];               /// Reserved
} boms_scsi_CDB_read_format_capacity_t;

typedef struct PACK boms_scsi_CDB_read_t
{
    // byte 0
    unsigned char  op_code;            /// Operation code = 28h
    // byte 1
    unsigned char  rel_adr : 1;        /// RelAdr
    unsigned char  R1 : 2;             /// Reserved
    unsigned char  fua : 1;            /// FUA
    unsigned char  dpo : 1;            /// DPO
    unsigned char  lun : 3;            /// LUN
    // bytes 2..5
    unsigned long  lba;                /// Logical Block Address
    // byte 6
    unsigned char  R2;                 /// Reserved
    // bytes 7..8
    unsigned short length;             /// Transfer Length
    // byte 9
    unsigned char  control;            /// Control
} boms_scsi_CDB_read_t;

typedef struct PACK boms_scsi_CDB_write_t
{
    // byte 0
    unsigned char  op_code;            /// Operation code = 2Ah
    // byte 1
    unsigned char  rel_adr : 1;        /// RelAdr
    unsigned char  R1 : 1;             /// Reserved
    unsigned char  ebp : 1;            /// EBP
    unsigned char  fua : 1;            /// FUA
    unsigned char  dpo : 1;            /// DPO
    unsigned char  lun : 3;            /// LUN
    // bytes 2..5
    unsigned long  lba;                /// Logical Block Address
    // byte 6
    unsigned char  R2;                 /// Reserved
    // bytes 7..8
    unsigned short length;             /// Transfer Length
    // byte 9
    unsigned char  control;            /// Control
} boms_scsi_CDB_write_t;
//@}

/**
    @name SCSI Command Block Wrapper (CDBs)
    @brief Wrapper for SCSI command blocks. Table 5.1.
 */
typedef struct PACK boms_commandBlockWrapper_t
{
    /// Signature that helps identify this data packet as a CBW.
    /// The signature field shall contain the value 43425355h
    /// (little endian), indicating a CBW.
    unsigned int dCBWSignature;        /// 0x43425355 (LE)
    /// A Command Block Tag sent by the host. The device shall
    /// echo the contents of this field back to the host in the
    /// dCSWTag field of the associated CSW. The dCSWTag positively
    /// associates a CSW with the corresponding CBW.
    unsigned int dCBWTag;
    /// The number of bytes of data that the host expects to
    /// transfer on the Bulk-In or Bulk-Out endpoint (as indicated
    /// by the Direction bit) during the execution of this command.
    /// If this field is zero, the device and the host shall transfer
    /// no data between the CBW and the associated CSW, and the device
    /// shall ignore the value of the Direction bit in bmCBWFlags.
    unsigned int dCBWDataTransferLength;
    /// Bit 7 Direction - the device shall ignore this bit if the
    ///                   dCBWDataTransferLength field is
    ///                   zero, otherwise:
    ///       0 = Data-Out from host to the device,
    ///       1 = Data-In from the device to the host.
    /// Bit 6 Obsolete. The host shall set this bit to zero.
    /// Bits 5..0 Reserved - the host shall set these bits to zero.
    unsigned char bmCBWFlags;
    /// The device Logical Unit Number (LUN) to which the command
    /// block is being sent. For devices that support multiple LUNs,
    /// the host shall place into this field the LUN to which this
    /// command block is addressed. Otherwise, the host shall set
    /// this field to zero.
    unsigned char bCBWLUN;             /// bits 0..3 only
    /// The valid length of the CBWCB in bytes. This defines the
    /// valid length of the command block. The only legal values
    /// are 1 through 16 (01h through 10h). All other values are
    /// reserved.
    unsigned char bCBWCBLength;        /// bits 0..4 only
    /// The command block to be executed by the device. The device
    /// shall interpret the first bCBWCBLength bytes in this field as
    /// a command block as defined by the command set identified by
    /// bInterfaceSubClass. If the command set supported by the device
    /// uses command blocks of fewer than 16 (10h) bytes in length, the
    /// significant bytes shall be transferred first, beginning with
    /// the byte at offset 15 (Fh). The device shall ignore the content
    /// of the CBWCB field past the byte at offset (15 + bCBWCBLength - 1).
    union
    {
        unsigned char                           buffer[16];
        struct boms_scsi_CDB_test_unit_ready_t unit_ready;
        struct boms_scsi_CDB_request_sense_t   request_sense;
        struct boms_scsi_CDB_mode_sense_t      mode_sense;
        struct boms_scsi_CDB_inquiry_t         inquiry;
        struct boms_scsi_CDB_removal_t         removal;
        struct boms_scsi_CDB_read_capacity_t   read_capacity;
        struct boms_scsi_CDB_read_format_capacity_t   read_fcapacity;
        struct boms_scsi_CDB_read_t            read;
        struct boms_scsi_CDB_write_t           write;
    } CBWCB;
} boms_commandBlockWrapper_t;

/**
    @name SCSI Response Blocks (CRBs)
    @brief Response block definitions for SCSI commands.
 */
//@{
typedef struct PACK boms_scsi_CRB_request_sense_t
{
    /// Response Code
    /// 70h - current errors, Fixed Format
    /// 71h - deferred errors, Fixed Format
    /// 72h - Current error, Descriptor Format
    /// 73h - Deferred error, Descriptor Format
    unsigned char rsp_code : 7;
    /// Valid
    /// 0 - sense data are not SCSI compliant
    /// 1 - sense data are SCSI compliant
    unsigned char valid : 1;
    /// Segment number - valid only if the current command is
    /// COPY, COMPARE, or COPY AND VERIFY
    unsigned char segment;
    /// Sense Key
    unsigned char sense_key : 4;
    unsigned char R1 : 1;
    /// ILI - incorrect length indicator - 1 = requested logical block length did not match the logical block length of
    /// the data on the medium
    unsigned char ili : 1;
    /// EOM - 1 = end-of-medium (sequential-access devices) OR out-of-paper (printers)
    unsigned char eom : 1;
    /// Filemark - 1 = the current command has reached a filemark or setmark (sequential-access devices only)
    unsigned char filemark : 1;
    unsigned long information;
    unsigned char adnl_len;
    unsigned long cmd_info;
    unsigned char adnl_sense_code;
    unsigned char adnl_sense_code_qualifier;
    unsigned char fru;
    /// SKSV - Sense Key Specific Valid -
    /// 0 - sense key specific data are not SCSI compliant
    /// 1 - sense key specific data are SCSI compliant
    unsigned char sksv1 : 7;
    unsigned char sksv : 1;
    unsigned char sksv2;
    unsigned char sksv3;
} boms_scsi_CRB_request_sense_t;

typedef struct PACK boms_scsi_CRB_inquiry_t
{
    // byte 0
    /// Peripheral Device Type
    unsigned char peripheral_dev_type : 5;
    /// Peripheral Device Qualifier
    unsigned char peripheral_qual : 3;
    // byte 1
    unsigned char device_type_modifier : 7;
    /// RMB - removable medium bit
    unsigned char rmb : 1;
    // byte 2
    unsigned char ansi : 3;
    unsigned char ecma : 3;
    unsigned char iso : 2;
    // byte 3
    /// Response data format - 000=SCSI compliant, other values = legacy devices
    unsigned char resp_data_format : 4;
    unsigned char R1 : 2;
    /// TrmIOP - 1 = supports Terminate I/O Process messages
    unsigned char trm_iop : 1;
    /// AEN - 1 = supports Asynchronous Event Notifications - (processor devices only)
    unsigned char aen : 1;
    // byte 4
    /// Additional length (n-4)
    unsigned char adnl_len;
    // byte 5
    unsigned char R2;
    // byte 6
    unsigned char R3;
    // byte 7
    /// SftRe - 1 = performs soft resets
    unsigned char sft_re : 1;
    /// CmdQue - 1 = supports Tagged Command Queuing
    unsigned char cmd_que : 1;
    unsigned char R4 : 1;
    /// Linked - 1 = this LUN supports linked commands
    unsigned char linked : 1;
    /// Sync - 1 = supports synchronous transfers
    unsigned char sync : 1;
    /// WBus32+WBus16 - support for wide data buses:
    /// 00 - 8-bit only
    /// 01 - 8 or 16-bit
    /// 11 - 8, 16 or 32 bit
    unsigned char wbus16 : 1;
    unsigned char wbus32 : 1;
    /// RelAdr - 1= this LUN supports Relative Addressing Mode (linked commands only)
    unsigned char rel_adr : 1;
    // byte 8
    unsigned char vid[8];
    // byte 16
    unsigned char pid[16];
    // byte 32
    unsigned char rev[4];
    // byte 36
    unsigned char vendor[20];
    // byte 56
    unsigned char R5[40];
    // byte 96
} boms_scsi_CRB_inquiry_t;

/// Size of useful data in boms_scsi_CRB_inquiry_t response.
#define BOMS_SCSI_INQUIRY_TRANSFER_LEN 36

typedef struct PACK boms_scsi_CRB_read_capacity_t
{
    unsigned long lba;                 /// Logical Block Address
    unsigned long block_len;           /// Block Length
} boms_scsi_CRB_read_capacity_t;

typedef struct PACK boms_scsi_CRB_read_capacity_16_t
{
	// bytes 0-7
    unsigned long lba_h;                 /// Logical Block Address
    unsigned long lba_l;                 /// Logical Block Address
    // bytes 7-15
    unsigned long block_len_h;           /// Block Length
    unsigned long block_len_l;           /// Block Length
    // byte 16
    unsigned char prot_en : 1;
    unsigned char p_type : 3;
    unsigned char reserved_1 : 4;
    // bytes 17-31
    unsigned char reserved_2[15];
} boms_scsi_CRB_read_capacity_16_t;

typedef struct PACK boms_scsi_CRB_read_format_capacity_t
{
	// bytes 0-3
    unsigned long cap_list;             /// Capacity List Header
	// bytes 4-7
    unsigned long blocks;     			/// Current/Maximum Capacity Blocks
	// bytes 8-11
    unsigned long code : 2;     		/// Descriptor Code
    unsigned long R1 : 6;     			/// Reserved
    unsigned long block_len : 24;     	/// Block Length
} boms_scsi_CRB_read_format_capacity_t;

typedef struct PACK boms_scsi_CRB_mode_sense_t
{
	// Mode Parameter List
    // byte 0
    unsigned char mode_data_length;
    // byte 1
    unsigned char medium_type_code;
    // byte 2
    unsigned char device_specific_parameters;
    // byte 3
    unsigned char block_descriptor_length;
} boms_scsi_CRB_mode_sense_t;

typedef struct PACK boms_scsi_CRB_mode_sense_page_read_write_error_recovery_t
{
    // Read-Write Error Recovery Page
    // byte 0
    unsigned char page_code : 6; // Always 0x01
    unsigned char reserved : 1;
    unsigned char parameters_savable : 1;
    // byte 1
    unsigned char page_length; // Always 0x0a
    // byte 2
    unsigned char params;
    // byte 3
    unsigned char read_retry_count;
    // byte 4
    unsigned char correction_span;
    // byte 5
    unsigned char head_offset_count;
    // byte 6
    unsigned char data_strobe_offset_count;
    // byte 7
    unsigned char reserved_1;
    // byte 8
    unsigned char write_retry_count;
    // byte 9
    unsigned char reserved_2;
    // byte 10
    unsigned short recovery_limit;
} boms_scsi_CRB_mode_sense_page_read_write_error_recovery_t;

typedef struct PACK boms_scsi_CRB_mode_sense_page_flexible_disk_t
{
    // Flexible Disk Page
    // byte 0
    unsigned char page_code : 6; // Always 0x05
    unsigned char reserved : 1;
    unsigned char parameters_savable : 1;
    // byte 1
    unsigned char page_length; // Always 0x1e
    // byte 2
    unsigned short transfer_rate;
    // byte 3,4
    unsigned char number_of_heads;
    // byte 5
    unsigned char sectors_per_track;
    // byte 6,7
    unsigned short bytes_per_sector;
    // byte 8,9
    unsigned short number_of_cylinders;
    // byte 10-18
    unsigned char reserved_1[9];
    // byte 19
    unsigned char motor_on_delay;
    // byte 20
    unsigned char motor_off_delay;
    // byte 21-27
    unsigned char reserved_2[7];
    // byte 28,29
    unsigned short rotation_rate;
    // byte 30,31
    unsigned char reserved_3[2];
} boms_scsi_CRB_mode_sense_page_flexible_disk_t;

typedef struct PACK boms_scsi_CRB_mode_sense_page_removable_block_access_capabilities_t
{
    // Removable Block Access Capabilities Page
    // byte 0
    unsigned char page_code : 6; // Always 0x1b
    unsigned char reserved : 1;
    unsigned char parameters_savable : 1;
    // byte 1
    unsigned char page_length; // Always 0x0a
    // byte 2
    unsigned char reserved_1 : 6;
    unsigned char support_reporting_format_progress : 1;
    unsigned char system_floppy_type : 1;
    // byte 3
    unsigned char total_luns : 3;
    unsigned char reserved_2 : 3;
    unsigned char multiple_luns : 1;
    unsigned char non_cd_optical_drive : 1;
    // byte 4-10
    unsigned char reserved_3[8];
} boms_scsi_CRB_mode_sense_page_removable_block_access_capabilities_t;

typedef struct PACK boms_scsi_CRB_mode_sense_page_fault_failure_reporting_control_t
{
    //Fault/Failure Reporting Control Page
    // byte 0
    unsigned char page_code : 6; // Always 0x1c
    unsigned char reserved : 1;
    unsigned char parameters_savable : 1;
    // byte 1
    unsigned char page_length; // Always 0x06
    // byte 2
    unsigned char logerr : 1;
    unsigned char reserved_1 : 1;
    unsigned char test : 1;
    unsigned char disable_exception : 1;
    unsigned char reserved_2 : 3;
    unsigned char performance : 1;
    // byte 3
    unsigned char mrie : 4;
    unsigned char reserved_3 : 4;
    // bytes 4-7
    unsigned long interval_timer;
} boms_scsi_CRB_mode_sense_page_fault_failure_reporting_control_t;

//@}

/**
    @name SCSI Command Status Wrapper (CDBs)
    @brief Wrapper for SCSI status response. Table 5.2.
 */
typedef struct PACK boms_commandStatusWrapper_t
{
    /// Signature that helps identify this data packet as a CSW. The
    /// signature field shall contain the value 53425355h (little endian),
    /// indicating CSW.
    unsigned int dCSWSignature;
    /// The device shall set this field to the value received in the
    /// dCBWTag of the associated CBW.
    unsigned int dCSWTag;
    /// For Data-Out the device shall report in the dCSWDataResidue the
    /// difference between the amount of data expected as stated in the
    /// dCBWDataTransferLength, and the actual amount of data processed by
    /// the device. For Data-In the device shall report in the
    /// dCSWDataResidue the difference between the amount of data expected
    /// as stated in the dCBWDataTransferLength and the actual amount of
    /// relevant data sent by the device. The dCSWDataResidue shall not
    /// exceed the value sent in the dCBWDataTransferLength.
    unsigned int dCSWDataResidue;
    /// bCSWStatus indicates the success or failure of the command. The
    /// device shall set this byte to zero if the command completed
    /// successfully. A non-zero value shall indicate a failure during command
    /// execution.
    unsigned char bCSWStatus;
} boms_commandStatusWrapper_t;

/* End of USB BOMS Specification */
//@}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // FT900_USB_BOMS_H_
