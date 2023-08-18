/**
 * @file ftd2xx_api.h
 * 
 * @brief D2XX Solution API.
 *        This contains D2XX SOlution API function definitions, constants and
 *        structures which are exposed in the API.
 **/
/*
 * ============================================================================
 * History
 * =======
 *
 * (C) Copyright 2014, Future Technology Devices International Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Future Technology Devices
 * International Limited ("FTDI") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef FTD2XX_API_H_
//@{
#define FTD2XX_API_H_
//@}

/* INCLUDES ******************************************************************/
#include <stdint.h>

/* MACROS ********************************************************************/

//@{
/* Pack structures to align on byte boundaries. Required for USB data structures.*/
#define D2XX_PACK __attribute__ ((__packed__))

/*
 * The number of strings to be provided for the USB string descriptor.
 */
#define D2XX_NUM_DESCRIPTOR_STRINGS   		(4)
//@}


#if defined(__FT930__)
/**
 * @brief The maximum number of D2XX interfaces the D2XX solution can support.
 */
#define D2XX_MAX_INTERFACES   				(7)
#else
/**
 * @brief The maximum number of D2XX interfaces the D2XX solution can support.
 */
#define D2XX_MAX_INTERFACES   				(3)
#endif


/**
 * @brief The maximum total length of all the four strings used in the string descriptors.
 * Refer TD2XX_DeviceConfiguration for the details on the strings used.
 */
#define D2XX_MAX_DESCRIPTOR_STRING_SIZE   	(128)

/**
 * @brief A non zero, negative, number base to start the error coding for the
 * Enum ED2XX_ErrorCode.
 */
#define	D2XX_API_ERR_BASE					(-1)

/**
 * @brief Double Null terminated ascii string for Unique device interface GUID in registry format (including Braces and hyphen)
 * for. e.g.: {2C69C451-55E9-46f0-8E4E-1F30D1E148EE}
 */
#define D2XX_DEVICEGUID_STRING_SIZE   	(40)

/**
 * @brief Maximum transfer size on an D2XX interface that the user can configure
 */
#define D2XX_MAX_USERCONFIG_XFER_SIZE   	D2XX_XFER_SIZE_1024

/**
 * @name D2XX Line Control from USB Host
 */
//@{
#define D2XX_SETDATA_7_BIT  	7
#define D2XX_SETDATA_8_BIT  	8

#define D2XX_SETDATA_NOPAR  	0
#define D2XX_SETDATA_ODDPAR  	1
#define D2XX_SETDATA_EVENPAR  	2
#define D2XX_SETDATA_MARKPAR  	3
#define D2XX_SETDATA_SPACEPAR  	4

#define D2XX_SETDATA_1_STOP  	0
#define D2XX_SETDATA_2_STOP  	2

#define D2XX_SETDATA_BREAK_OFF 	0
#define D2XX_SETDATA_BREAK_ON 	1
//@}

/**
 * @name D2XX Interface reset to purge Tx and Rx buffers
 */
//@{
#define D2XX_INTF_PURGE_TX_RX 	0
#define D2XX_INTF_PURGE_RX 		1
#define D2XX_INTF_PURGE_TX 		2
//@}
/* TYPES *********************************************************************/

/**
    @enum ED2XX_ErrorCode
    @brief The return values returned by the API functions
 **/
typedef enum
{
	D2XX_ERR_NONE = 0,					/**< returns Success */
	D2XX_ERR_IO = D2XX_API_ERR_BASE,	/**< in case of IO operation error */
	D2XX_ERR_MULTI = D2XX_API_ERR_BASE-1,/**< in case of multiple call of the init or exit API */
	D2XX_ERR_DEVICE = D2XX_API_ERR_BASE-2,					/**< in case of Device error */
	D2XX_ERR_INSUFFICIENT_RESOURCES = D2XX_API_ERR_BASE-3,	/**< in case of insufficient memory or resources */
	D2XX_ERR_INVALID_PARAMETER = D2XX_API_ERR_BASE-4,			/**< Invalid Parameter supplied to API function */
	D2XX_ERR_NOT_SUPPORTED = D2XX_API_ERR_BASE-5			/**< Operation not supported */

}ED2XX_ErrorCode;

/**
    @enum ED2XX_EventCode
    @brief The events for which the D2XX API provides callback
 **/
typedef enum
{
	D2XX_EVT_SUSPEND,    	/**< SUSPEND EVENT from USB Host */
	D2XX_EVT_RESUME,	 	/**< RESUME EVENT from USB Host */
	D2XX_EVT_BUS_RESET,		/**< BUS RESET EVENT from USB Host */
	D2XX_EVT_READY,	 		/**< D2XX enters Ready state where READ/WRITE requests are processed*/
	D2XX_EVT_UNREADY,	 	/**< D2XX exits Ready state (USB disconnected)*/
	D2XX_EVT_DFU_DETACH,	/**< DFU DETACH Class command from DFU application */
	D2XX_EVT_TESTMODE,	 	/**< D2XX enters Test Mode. Exit is via power cycle*/
	D2XX_EVT_INTF_RESET,	/**< Interface RESET Vendor Command from D2XX Application */

	/* D2XX Set commands from host to be notified to the application */
	D2XX_EVT_SET_DTRSTATUS,				/**< DTR status in case of DTR_DSR flow control is enabled */
	D2XX_EVT_SET_RTSSTATUS,				/**< RTS status in case of RTS_CTS flow control is enabled */
	D2XX_EVT_SET_BAUDRATE,				/**< Set Baud rate command */
	D2XX_EVT_SET_FLOWCTRL_NONE,			/**< No flow control */
	D2XX_EVT_SET_FLOWCTRL_RTS_CTS,		/**< Flow control type is RTS_CTS*/
	D2XX_EVT_SET_FLOWCTRL_DTR_DSR,		/**< Flow control type is DTR_DSR*/
	D2XX_EVT_SET_FLOWCTRL_XON_XOFF,		/**< Flow control type is XON_XOFF*/
	D2XX_EVT_SET_DATACHARACTERISTICS,   /**< Set Data format */
	D2XX_EVT_SET_EVENTCHAR,				/**< Set Event character */
	D2XX_EVT_SET_ERRORCHAR,				/**< Set Error(parity) characters */
	D2XX_EVT_SET_BITMODE,				/**< Set bit mode to enable different chip mode */

	/* D2XX Get commands from host to be notified to the application */
	D2XX_EVT_GET_MODEMSTATUS, 			/**< Get Modem Status received */
	D2XX_EVT_GET_BITMODE,				/**< Get bit mode received */

	D2XX_EVT_MAX_CODE

}ED2XX_EventCode;

/**
    @enum ED2XX_IoctlID
    @brief The ioctl id is the unique identifier used by the D2XX IOCTL
    	   API to process the ioctl request
 **/
typedef enum
{
	D2XX_IOCTL_SYS_REMOTE_WAKEUP,	/**< REMOTE WAKEUP to the USB Host */
	D2XX_IOCTL_INTERFACE_WAKEUP,	/**< D2XX interface wakeup to be sent for a D2XX interface */
	D2XX_IOCTL_MAX_ID				/**< End of IOCTL ID List */

}ED2XX_IoctlID;

/**
    @enum ED2XX_TransferSize
    @brief The maximum packet size for which transfer happens on the D2XX interface
 **/
typedef enum
{
	/** 32 Bytes **/
	D2XX_XFER_SIZE_32 = 32,
	/** 64 Bytes **/
	D2XX_XFER_SIZE_64 = 64,
	/** 128 Bytes **/
	D2XX_XFER_SIZE_128 = 128,
	/** 256 Bytes **/
	D2XX_XFER_SIZE_256 = 256,
	/** 512 Bytes **/
	D2XX_XFER_SIZE_512 = 512,
	/** 1024 Bytes **/
	D2XX_XFER_SIZE_1024 = 1024

}ED2XX_TransferSize;

/**
    @struct TProductDescriptors
    @brief Struct to provide the product specific information about D2XX USB device
 **/
typedef struct
{
	/** Vendor ID (assigned by the USB-IF)
	 **/
	uint16_t  VendorID;
	/** Product ID (assigned by the manufacturer)
	 **/
	uint16_t  ProductID;

}TProductDescriptors;

/**
    @struct TConfigDescriptors
    @brief Struct to provide the configuration descriptor information about D2XX USB device
 **/
typedef struct
{

	/** Battery Charge Detection to be enabled or not.
	 * 0=disable, 1=enable
	 **/
	uint8_t BCDEnable;
	/** DFU support.
	 * 0=disable, 1=enable
	 **/
	uint8_t DFUCapable;
	/** Bus or Self Powered Device.
	 * 0=disable, 1=enable
	 **/
	uint8_t SelfPowered;
	/** Maximum power consumption of the USB device from the bus in this
	 * specific configuration when the device is fully operational. Expressed in
	 * 2 mA units (i.e., 50 = 100 mA).
	 **/
	uint8_t MaxPower;
	/** Number of D2XX interfaces supported by this USB configuration. Range: 1 to D2XX_MAX_INTERFACES
	 **/
	uint8_t NumOfD2XXInterfaces;
	/** Remote Wakeup capable or not.
	 * 0=disable, 1=enable
	 **/
	uint8_t RMWKUPEnable;
	/** The maximum packet size for which transfer happens on each of the D2XX
	 * interfaces.
	 * Value: 0 or enum values defined in ED2XX_TransferSize.
	 * Fill the value to 0 if a d2xx interface is not used (i.e. Indexes >= NumOfD2XXInterfaces)
	 **/
	uint16_t MaxTransferSize[7];
}TConfigDescriptors;

/**
    @struct TD2XX_DeviceConfiguration
    @brief Struct to provide all the custom information for configuring the
    D2XX device
    @details  Structure provided during initialization of D2XX solution library. \n
			  Comprises of structures to provide data related to Product,
			  Configuration and String descriptors.
 **/
typedef struct D2XX_PACK TD2XX_DeviceConfiguration
{
	//! @cond PRIVATE
	/* Signature of the D2XX configuration */
	uint32_t  Signature;
	/* Version number of this configuration structure
	 */
	uint16_t  ConfigVersion;
	/* Delay after USB PHY is enabled
	 */
	uint16_t  DelayAfterPHYEn;
	//! @endcond
	/** Struct to provide the product specific information about D2XX USB device.
	 **/
	TProductDescriptors ProductDesc;
	/** Struct to provide the configuration descriptor information about
	    D2XX USB device.
	 **/
	TConfigDescriptors ConfigDesc;
	/** String configuration section.
	 * \par
     * String 1 - ASCII string detailing the manufacturer.
	 * \par
	 * String 2 - ASCII string detailing the Product.
	 * \par
	 * String 3 - ASCII string for the Serial Number (max 14 characters).
	 * \par
	 * String 4 - ASCII string for the DFU Runtime Interface Name.
	 * \par
     * Note:
     * 1. All the strings should be preceded with the data length of the string.
     * for e.g.
	 * \par
     * 0x04,'F','T','D','I',
	 * \par
	 * 0x0A,'F','T','9','0','0',' ','D','2','X','X',
	 * \par
	 * 0x0E,'F','T','9','0','0','S','e','r','i','a','l','#','0','3',
	 * \par
	 * 0x15,'D','F','U',' ','R','u','n','t','i','m','e',' ','I','n','t','e','r','f','a','c','e'
	 * \par
	 * 2. No string should be left blank
	 * 3. The Serial number should be limited to a maximum of 14 characters ONLY by the user.
	 *    D2XX on the PC host(windows/linux) adds an additional letter to the serial number to identify the port number. 
	 *    This brings the length of Serial number to 16 characters including null termination as is the requirement from 
	 *    D2XX library(FT_DEVICE_LIST_INFO_NODE of D2XX_Programmer's_Guide.pdf). 
	 **/
	uint8_t Strings[D2XX_MAX_DESCRIPTOR_STRING_SIZE];
	/** Double Null terminated ascii string for Unique device interface GUID in registry format (including Braces and hyphen).
		for. e.g.: {2C69C451-55E9-46f0-8E4E-1F30D1E148EE}
	 **/
	uint8_t DfuDeviceInterfaceGUID[D2XX_DEVICEGUID_STRING_SIZE];

	/** XOR Checksum
	 **/
	uint16_t  Checksum;
}TD2XX_DeviceConfiguration;

/**
    @typedef   FD2XX_Callback
    @brief     Callback declaration for user callback functions invoked
    		   from D2XX solution.
    @param[in] eventID - The events for which the D2XX provides callback
	@param[in] ref - User application context which is stored and given back
					 during invocation of callback functions on events.
    @param[in] param1 - Note 1: In case of D2XX_EVT_SUSPEND, this param gives whether
    					the RemoteWakeup is enabled or not.
						1 - Enabled, 0 - Disabled
						Based on which the user application can issue a Remote
						Wakeup to the host device in Suspend mode.

						Note 2: In case of D2XX_EVT_SET_XXXXXX events, this param gives the D2XX Interface Number
								Range: 1..n, n -> Number of D2XX Interfaces configured by the application

	@param[in] param2 - This parameter is any one of the below according to the eventID
						1. can be any one of D2XX_INTF_PURGE_TX_RX, D2XX_INTF_PURGE_RX, D2XX_INTF_PURGE_TX in case of D2XX_EVT_INTF_RESET
						2. DTR bit with bit status as 0 or 1 (active) in case of D2XX_EVT_SET_DTRSTATUS
						3. RTS bit with bit status as 0 or 1 (active) in case of D2XX_EVT_SET_RTSSTATUS
						4. struct TD2XX_EventSetBaudrate in case of D2XX_EVT_SET_BAUDRATE
						5. Ignore (or unused) in case of D2XX_EVT_SET_FLOWCTRL_NONE, D2XX_EVT_SET_FLOWCTRL_RTS_CTS and D2XX_EVT_SET_FLOWCTRL_DTR_DSR
						6. struct TD2XX_EventXONXOFF in case of D2XX_EVT_SET_FLOWCTRL_XON_XOFF
						7. struct TD2XX_EventSetDataCharacteristics in case of D2XX_EVT_SET_DATACHARACTERISTICS
						8. struct TD2XX_SetSpecialChar in case of D2XX_EVT_SET_EVENTCHAR and D2XX_EVT_SET_ERRORCHAR
						9. struct TD2XX_EventSetBitMode in case of D2XX_EVT_SET_BITMODE
    @return    void
 **/
typedef void (*FD2XX_Callback)(ED2XX_EventCode  eventID, void *ref, void* param1, void* param2);

/**
    @struct TD2XX_EventSetDataCharacteristics
    @brief Struct to provide the data format - the number of data bits, stop bits and
    			parity mode set by the D2XX USB Host.
    @details  Refer 'D2XX Line Control from USB Host' macros in this header
 **/
typedef struct TD2XX_EventSetDataCharacteristics
{
	/** Can be either D2XX_SETDATA_7_BIT or
	    				D2XX_SETDATA_8_BIT
	 **/
	uint8_t  data_size;
	/** Can be one of D2XX_SETDATA_NOPAR,
						D2XX_SETDATA_ODDPAR, D2XX_SETDATA_EVENPAR,
						D2XX_SETDATA_MARKPAR, D2XX_SETDATA_SPACEPAR.
	**/
	uint8_t  parity;
	/** Number of stop bits. Can be one of
	    				D2XX_SETDATA_1_STOP or D2XX_SETDATA_2_STOP.
	**/
	uint8_t  stop_bits;
	/** Number of stop bits. Can be one of
	    				D2XX_SETDATA_BREAK_OFF or D2XX_SETDATA_BREAK_ON.
	**/
	uint8_t  break_condition;
}TD2XX_EventSetDataCharacteristics;

/**
    @struct TD2XX_EventSetBaudrate
    @brief Struct to provide the baud rate information in terms of divisor, set by the D2XX USB Host.
    @details  Refer to: http://www.ftdichip.com/Support/Documents/AppNotes/AN232B-05_BaudRates.pdf
     	 	 	It doesn't check if the baud rate can be calculated within the
     	 	 	+/- 3% required to ensure a stable link.
 **/
typedef struct TD2XX_EventSetBaudrate
{
	uint16_t integer_divisor;
	uint16_t sub_integer_divisor;
}TD2XX_EventSetBaudrate;

/**
    @struct TD2XX_EventXONXOFF
    @brief Struct to provide the XON and XOFF characters for XON XON flow control, set by the D2XX USB Host.
    @details  Struct to provide the XON and XOFF characters for XON XON flow control, set by the D2XX USB Host.
 **/
typedef struct TD2XX_EventXONXOFF
{
	uint8_t XON_char;
	uint8_t XOFF_char;
}TD2XX_EventXONXOFF;

/**
    @struct TD2XX_SetSpecialChar
    @brief Struct to provide the special character, set by the D2XX USB Host.
    @details   Useful in inserting specified characters in the data stream to represent events firing or errors occurring.
 **/
typedef struct TD2XX_SetSpecialChar
{
	/** Event or error character **/
	uint8_t event_char;
	/** For event char: 0 if no trigger, 1 if Trigger IN on Event Char
	    For error char: 0 if no error replacement, 1 if error replacement is on
	 **/
	uint8_t trigger_stat;
}TD2XX_SetSpecialChar;

/**
    @struct TD2XX_EventSetBitMode
    @brief Struct to provide the XON and XOFF characters for XON XON flow control, set by the D2XX USB Host.
    @details  Struct to provide the XON and XOFF characters for XON XON flow control, set by the D2XX USB Host.
 **/
typedef struct TD2XX_EventSetBitMode
{
	/** Required value for bit mode mask. This sets up which bits are input and outputs
                       A bit value of 0 sets the corresponding pin to an input, a bit value of 1 for an output.
                       In the case of CBUS Bit Bag, the upper nibble of this value controls which pins are
                       inputs and outputs, while the lower nibble controls which of the outputs are high and
                       low.
	**/
	uint8_t mask;
	/** Can be one of the following:
    					0x0 - Reset
    					0x1 - Asynchronous Bit Bang
    					0x4 - Synchronous Bit Bang (FT232R, FT245R, FT2232, FT2232H, FT4232H and FT232H devices only)
    					0x20 - CBUS Bit Bang Mode (FT232R and FT232H devices only)
    **/
	uint8_t mode;
}TD2XX_EventSetBitMode;

/* CONSTANTS *****************************************************************/

/* GLOBAL VARIABLES **********************************************************/

/* FUNCTION PROTOTYPES *******************************************************/
/**
    @brief     Initialise D2XX solution.
    @details   Initialises D2XX solution library. \n
               This function MUST be called prior to any further call to the
               USB functions.
    @param[in] d2xxDeviceConfig - User application custom specific information
    			about the D2XX USB device and its interfaces. This data is
    			used in the construction of device, config and string
    			descriptors.
	@param[in] callbackFn - The user application registers its callback
							function through this param.
	@param[in] ref - The user application registers its callback
							context through this param.
	@return     D2XX_ERR_NONE if successful.
                D2XX_ERR_INVALID_PARAMETER if invalid values or ranges provided
                 through the d2xxDeviceConfig param.

 **/
ED2XX_ErrorCode D2XX_Init(TD2XX_DeviceConfiguration *d2xxDeviceConfig, FD2XX_Callback callbackFn, void *ref);

/**
    @brief     Performs Read on a D2XX interface.
    @details   Performs Read on a D2XX interface.
    @param[in] interfaceNum - D2XX Interface Number
				Range: 1..n, n -> Number of D2XX Interfaces configured by the
								  application
	@param[in]  readBuffer - A pointer to the buffer which the stream data is
							 read into.
	@param[in]  length - The number of bytes of data to read from D2XX
						 interface buffer.

	@return     Returns the actual number of bytes read from the D2XX
				interface.
				Zero bytes is returned if the internal D2XX buffer is empty.
                D2XX_ERR_INVALID_PARAMETER if invalid values or ranges
                provided through the interfaceNum or readBuffer params.
                D2XX_ERR_IO if unsuccessful.
                D2XX_ERR_DEVICE if D2XX is not in the state of processing the
                request

 **/
int32_t D2XX_Read(int32_t interfaceNum, uint8_t  *readBuffer, const int32_t length);

/**
    @brief     Performs Write on a D2XX interface.
    @details   Performs Write on a D2XX interface.
	@param[in] interfaceNum - D2XX Interface Number
				Range: 1..n, n -> Number of D2XX Interfaces configured by the
								  application
	@param[inout]  writeBuffer - A pointer to the buffer to which the stream data
							  is written to.
	@param[in]  length - The number of bytes of data to write from user buffer
						 to the D2XX interface.
	@return     Returns the actual number of bytes written to the D2XX
				interface.
				Zero bytes is returned if the internal D2XX buffer is full.
                D2XX_ERR_INVALID_PARAMETER if invalid values or ranges provided
                 through the interfaceNum or writeBuffer params.
                D2XX_ERR_IO if unsuccessful.
                D2XX_ERR_DEVICE if D2XX is not in the state of processing the
                request

 **/
int32_t D2XX_Write(int32_t interfaceNum, uint8_t  *writeBuffer, const int32_t length);

/**
    @brief     IOCTL API to D2XX library.
    @details   The ioctl API is a catch-all that can handle transactions where
    			read and write are not suitable. Typically, this means control
    			data for a D2XX interface or system control of USB device.
    @param[in] interfaceNum - D2XX Interface Number
    			Value: 0 - System Purpose (e.g. Remote Wakeup)
    			       1..n, n -> Number of D2XX Interfaces configured by the
    			       	   	      application
	@param[in] ioctlID - D2XX IOCTL ID
    		   Refer to ED2XX_IoctlID documentation
	@param[in] param1 - Additional Parameter that application passes
			   for D2XX_IOCTL_INTERFACE_WAKEUP.
			   It is for set or clear
			   1 -> Set Wakeup
			   0 -> Clear Wakeup
	@param[in] param2 - Currently unused.

	@return     D2XX_ERR_NONE if successful.
                D2XX_ERR_INVALID_PARAMETER if invalid values or ranges provided
                 through the interfaceNum or ioctlID params.
                D2XX_ERR_DEVICE if D2XX is not in the state of processing the
                request
                D2XX_ERR_NOT_SUPPORTED if any unsupported IOCTL request is made.

 **/
ED2XX_ErrorCode  D2XX_IOCTL(int32_t interfaceNum, int ioctlID, void *param1, void *param2);

/**
    @brief     Cleanup D2XX solution when exiting D2XX mode.
    @details   Application to call this function to exit D2XX mode. \n
               This function cleans up D2XX solution and USB Driver.

 **/
void D2XX_Exit(void);

/**
    @brief     D2XX Timer function that runs the D2XX internal process.
    @details   To be called every millisecond from a timer interrupt handler to
	            to support for D2XX USB transactions.
	            Note: !!! To be called ONLY after D2XX_Init is successful !!!

 **/
void D2XX_Timer(void);

#if defined(__FT900__)
/**
    @brief     Main foreground D2XX process
    @details   Performs callbacks to pass USBD events to D2XX user. To be called in a main loop of the application
	            Note: !!! To be called ONLY after D2XX_Init is successful !!!

 */
void D2XX_Process(void);
#endif // defined(__FT900__)

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FTD2XX_API_H_ */
