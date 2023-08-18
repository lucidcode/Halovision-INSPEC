/**
    @file

    @brief
    Internal header file for USB Host device.

    This contains function prototypes and externals used within the USB Host
    API. These functions perform the background tasks and are not exposed
    in the API documentation.
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef FT900_USBH_INTERNAL_H_
#define FT900_USBH_INTERNAL_H_

// Pack structures to align on byte boundaries. Required for USB data structures.
#define PACK __attribute__ ((__packed__))

/* INCLUDES ************************************************************************/

#include "ft900_usb.h"
#include "ft900_usbh.h"

/* CONSTANTS ***********************************************************************/

/** @brief Indicate internally that a transfer is yet to complete (not an error condition).
 */
#define USBH_NOT_COMPLETE 0x01
/** @brief Enumeration has turned on hub port power
 */
#define USBH_ENUM_HUB_POWER 3

/* TYPES ***************************************************************************/

/**
    @name Device, Endpoint and Interface Handles.
    @details Handles are used to pass devices, interfaces and endpoints to the
        application.
        (Pointers to the USBH_device, USBH_interface and USBH_endpoint
        structures are not allowed as these are only used internally.)
        The handles allow embedding an enumeration value to detect stale
        handles which have been retained by the application. Enumeration is
        a dynamic operation and devices may appear and disappear without
        warning. This makes sure that new devices cannot be mistakenly used
        by an old handle.
 */
//@{
/**
    @typedef USBH_device_handle_t
    @brief Structure that is used to pass a handle to a device to the
        application. It is made up of a pointer to the device structure
        and a fairly unique value to detect enumeration changes and hence
        stale handles.
 **/
typedef struct USBH_device_handle_s
{
    /**
        Low 20 bits are the pointer to the USBH_device structure.
    **/
    uint32_t pDevice:20;
    /**
        High 12 bits are the enumeration value which must match the enumeration
        value in the USBH_device structure for this handle to be valid.
    **/
    uint32_t enumValue:12;
} USBH_device_handle_t;

/**
    @typedef USBH_interface_handle_t
    @brief Structure that is used to pass a handle to an interface to the
        application. It is made up of a pointer to the interface structure
        and a fairly unique value to detect enumeration changes and hence
        stale handles.
 **/
typedef struct USBH_interface_handle_s
{
    /**
        Low 20 bits are the pointer to the USBH_interface structure.
    **/
    uint32_t pInterface:20;
    /**
        High 12 bits are the enumeration value which must match the enumeration
        value in the USBH_interface structure for this handle to be valid.
    **/
    uint32_t enumValue:12;
} USBH_interface_handle_t;

/**
    @typedef USBH_endpoint_handle_t
    @brief Structure that is used to pass a handle to an endpoint to the
        application. It is made up of a pointer to the endpoint structure
        and a fairly unique value to detect enumeration changes and hence
        stale handles.
 **/
typedef struct USBH_endpoint_handle_s
{
    /**
        Low 20 bits are the pointer to the USBH_endpoint structure.
    **/
    uint32_t pEndpoint:20;
    /**
        High 12 bits are the enumeration value which must match the enumeration
        value in the USBH_endpoint structure for this handle to be valid.
    **/
    uint32_t enumValue:12;
} USBH_endpoint_handle_t;
//@}

/**
    @struct USBH_itd
    @brief Structure that represents an Isochronous Transfer Descriptor.
        This data structure is used for one or more USB transactions.
        It is intended for High Speed isochronous endpoints only.
		NOTE: This structure is an EHCI defined structure and must be
		located in the USBH memory space.
		Size is 64 bytes - 16 longwords.
 **/
typedef struct PACK USBH_itd
{
    /** Physical address of next transfer descriptor in list.
        Bits 31-5: The memory address of the next pointer in the list. The
            lower 5 bits are zero giving an alignment of 0x20 bytes.
			NOTE: This specifies an address in the USBH memory space.
        Bits 4-3: Reserved. Must be zero.
        Bits 2-1: Next item type. Whether the next item in the list is another
            QH, iTD, siTD or FSTN.
            0x0 - iTD (isochronous transfer descriptor)
            0x1 - QH
            0x2 - siTD (split transaction isochronous transfer descriptor)
            0x3 - FSTN (Frame Span Traversal Node)
        Bit 0: Terminate. A value if 1 will tell the host that there are no
            more valid queue headers in the list. This is the last entry in
            the list.
    **/
	volatile uint32_t next;
    /** 8 Transaction Status and Control descriptions.
     	Contains status results fields, transaction lengths and buffer offsets
     	for 8 isochronous transactions (slots). The 8 slots can be used for
     	up to 8 transfers. The Transaction Offset and Buffer Page Pointers are
     	used to calculate the start of buffers for each transaction.
     	Bits 31-28: Status of the transaction for this slot.
     		Bit 31: Active.
     		Bit 30: Data Buffer Error.
     		Bit 29: Babble Detected.
     		Bit 28: Transaction Error.
     	Bits 27-16: Transaction X Length. For OUT endpoints this is the number
     		of bytes to send. For IN endpoints this is initially set to the
     		number of bytes expected to be received. When the transaction is
     		completed it is set to the number of bytes actually received.
     	Bit 15: Interrupt on Complete.
     	Bits 14-12: Page Select. Indicates which offset pointer (from Buffer
     		Page Pointer List) to which to add the Transaction Offset to for
     		calculating the start address (in host controller memory) of this
     		transaction. Any of the 7 Buffer Page Pointers can be used.
     	Bits 11:0: Transaction X Offset. Offset in memory of the starting
     		address of the transaction. From start of memory address pointed
     		to by the Buffer Page Pointer List slot selected in the
     		Page Select value.
    **/
	volatile uint32_t status_control_list[8];
    /** 7 Buffer Page Pointers.
     	Contains up to 7 pointers to buffers for isochronous transactions.
     	All entries in list:
     	Bits 31-12: Buffer Pointer. 4 kB aligned pointer which is combined with
     		a Transaction Offset to form the address of buffer.
		List entry 0 only:
     	Bits 11-8: Endpoint Number. Target endpoint number.
     	Bit 7: Reserved.
     	Bits 6-0: Device Address. USB Address of target.
     	List entry 1 only:
     	Bit 11: I/O. Endpoint direction, IN (1) or OUT (0).
     	Bits 10-0: Maximum Packet Size.
     	Bit 15: Interrupt on Complete.
     	Bits 14-12: Page Select. Indicates which offset pointer (from Buffer
     		Page Pointer List) to which to add the Transaction Offset to for
     		calculating the start address (in host controller memory) of this
     		transaction.
     	Bits 11:0: Transaction X Offset. Offset in memory of the starting
     		address of the transaction. From start of memory address pointed
     		to by the Buffer Page Pointer List slot selected in the
     		Page Select value.
    **/
	volatile uint32_t pointer_list[7];

} USBH_itd;

/**
    @struct USBH_sitd
    @brief Structure that represents a Split Isochronous Transfer Descriptor.
        This data structure is used for one or more USB transactions.
        It is intended for Full Speed isochronous endpoints only.
		NOTE: This structure is an EHCI defined structure and must be
		located in the USBH memory space.
		Size is 48 bytes - 7 longwords.
 **/
typedef struct PACK USBH_sitd
{
    /** Physical address of next transfer descriptor in list.
        Bits 31-5: The memory address of the next pointer in the list. The
            lower 5 bits are zero giving an alignment of 0x20 bytes.
			NOTE: This specifies an address in the USBH memory space.
        Bits 4-3: Reserved. Must be zero.
        Bits 2-1: Next item type. Whether the next item in the list is another
            QH, iTD, siTD or FSTN.
            0x0 - iTD (isochronous transfer descriptor)
            0x1 - QH
            0x2 - siTD (split transaction isochronous transfer descriptor)
            0x3 - FSTN (Frame Span Traversal Node)
        Bit 0: Terminate. A value if 1 will tell the host that there are no
            more valid queue headers in the list. This is the last entry in
            the list.
    **/
	volatile uint32_t next;
    /** Endpoint Characteristics.
    **/
    volatile uint32_t ep_char_1;
    /** Endpoint Capabilities
    **/
    volatile uint32_t ep_capab_2;
    /** Transfer State
    **/
    volatile uint32_t transfer_state;
    /** Buffer Pointer List
    **/
    volatile uint32_t pointer_list[2];
    /** Back Pointer
    **/
	volatile uint32_t back;

} USBH_sitd;

/**
    @struct USBH_qtd
    @brief Structure that represents a a Queue Element Transfer Descriptor.
        This data structure is used for one or more USB transactions.
		NOTE: This structure is an EHCI defined structure and must be
		located in the USBH memory space.
		Size is 32 bytes - 8 longwords.
 **/
typedef struct PACK USBH_qtd
{
    /** Physical address of next transfer descriptor in list.
        Bits 31-5: The memory address of the next pointer in the list. The
            lower 5 bits are zero giving an alignment of 0x20 bytes.
			NOTE: This specifies an address in the USBH memory space.
        Bits 4-1: Reserved.
        Bit 0: Terminate. A value if 1 will tell the host that there are no
            more valid entries in the list. This is the last entry in the list.
    **/
	volatile uint32_t next;
    /** Physical address of next transfer descriptor in list should the the
        current transfer descriptor completion result in a short packet.
        Bit map as qtd_next.
    **/
	volatile uint32_t alt_next;
    /** Information host controller requires to execute USB transaction.
        Bit 31: Data Toggle.
        Bits 30-16: Total bytes to transfer.
        Bit 15: Interrupt on complete.
        Bits 14-12: Current page.
        Bits 11-10: Error Counter.
        Bits 9-8: PID code.
        Bits 7-0: Status. Refer to the descritions
        	Bit 6: Halted.
        	Bit 5: Data buffer error.
        	Bit 4: Babble detected.
        	Bit 3: Transaction error.
        	Bit 2: Missed micro-frame.
        	Bit 1: Split transaction.
        	Bit 0: Ping state.
    **/
	volatile uint32_t token;
    /** Physical address of buffer pages which form part of this transaction.
        Each page is 4kB in size. The first buffer pointer can start at any
        offset in the page but subsequent buffers must be aligned on 4kB
        boundaries.
        First pointer
            Bits 31-0: Address of start of memory buffer.
        Pointers 1 to 4
            Bits 31-12: Address of start of memory page. Lower 12 bits are zero.
            Bit 11-0: Current offset. Used by host controller and therefore
            reserved.
		NOTE: This specified an address in the USBH memory space.
    **/
	volatile uint32_t buf[5];

} USBH_qtd;

/**
    @struct USBH_qh
    @brief Structure that represents a Queue Head Descriptor.
        This data structure is used for the start of a queue of Queue Element
        Transfer Descriptors. This typically represents an endpoint in the
        transfer queue list.
		NOTE: This structure is an EHCI defined structure and must be
		located in the USBH memory space.
 **/
typedef struct PACK USBH_qh
{
    /** Queue Head Horizontal Link pointer.
    Physical address of next queue header descriptor in list.
        Bits 31-5: The memory address of the next pointer in the list. The
            lower 5 bits are zero giving an alignment of 0x20 bytes.
			NOTE: This specifies an address in the USBH memory space.
        Bits 4-3: Reserved. Must be zero.
        Bits 2-1: Next item type. Whether the next item in the list is another
            QH, iTD, siTD or FSTN.
            0x0 - iTD (isochronous transfer descriptor)
            0x1 - QH
            0x2 - siTD (split transaction isochronous transfer descriptor)
            0x3 - FSTN (Frame Span Traversal Node)
        Bit 0: Terminate. A value if 1 will tell the host that there are no
            more valid queue headers in the list. This is the last entry in
            the list.
        Size is 48 bytes - 12 long words.
    **/
    volatile uint32_t next;
    /** Endpoint Characteristics.
    **/
    volatile uint32_t ep_char_1;
    /** Endpoint Capabilities
    **/
    volatile uint32_t ep_capab_2;
    /** Pointer to current qTD.
    **/
    volatile uint32_t qtd_current;
    /** Transfer overlay. Used by host controller for processing current qTD.
    **/
    struct USBH_qtd transfer_overlay;
} USBH_qh;
//@}

/**
 	@enum USBH_list_entry_type
    @details Type of list entry. Can be one of the following:
				0: iTD - High Speed ISOCHONOUS.
				1: qH - Low-, Full-, or High-speed BULK, INTERRUPT or CONTROL.
				2: siTD - Full-speed ISOCHONOUS.
				3: FSTN - Periodic Frame Span Traversal Node.
 */
enum USBH_list_entry_type {
	USBH_list_entry_type_iTD,
	USBH_list_entry_type_qH,
	USBH_list_entry_type_siTD,
	USBH_list_entry_type_FSTN,
};

/**
    @struct USBH_list_entry
    @brief Structure that represents a Queue Entry.
        This data structure is used for the start of a queue of Queue Element
        Transfer Descriptors. This typically represents an endpoint in the
        transfer queue list.
		NOTE: This structure is an EHCI defined structure and must be
		located in the USBH memory space.
 **/
typedef struct USBH_list_entry
{
    /**
     	 Type of the list entry. From enum USBH_list_entry_type.
     */
    uint8_t type;

	union { // Anonymous.
		/**
			Pointer to the queue head for this endpoint.
			Interrupt, bulk and control endpoints.
		 **/
		USBH_qh *qh;
		/**
			Pointer to the isochronous transfer descriptor.
			High speed isochronous endpoints.
		 **/
		USBH_itd *iTD;
		/**
			Pointer to the split isochronous transfer descriptor.
			Full speed isochronous endpoints.
		 **/
		USBH_sitd *siTD;
	};
} USBH_list_entry;

/**
    @name Forward declarations for Device, Endpoint, Interface and
        Transfer structures. These are internal to the USBH library.
        Handles to devices, endpoints, interfaces and transfers must be used instead.
 */
//@{
/**
    @typedef USBH_device
    @brief Structure that represents a USB device.
 **/
typedef struct USBH_device USBH_device;

/**
    @typedef USBH_interface
    @brief Structure that represents a USB interface.
 **/
typedef struct USBH_interface USBH_interface;

/**
    @typedef USBH_endpoint
    @brief Structure that represents a USB endpoint.
 **/
typedef struct USBH_endpoint USBH_endpoint;

/**
    @typedef USBH_xfer
    @brief Structure that represents an asynchronous USB transfer.
 **/
typedef struct USBH_xfer USBH_xfer;
//@}

/**
    @name Device, Endpoint, Interface and Transfer Structures.
    @details Used internally in the library to keep information about devices,
        interfaces, endpoints and transfers.

        These structures are NOT part of the API and will be removed from
        the API specifications. It is up to the API to keep track of devices,
        interfaces, endpoints and transfers internally and provide a handle
        for external use.

        The following structure will describe a single device with 2 interfaces
        and 2 endpoints on interface 0, 1 endpoint on interface 1.

            <Root Hub>
                |d
            <Device 1>
             |a     |b
            <EP0>  <Interface 0>---<Interface 1>
                         |               |
                       <EP1>---<EP2>   <EP3>

        This example is a hub with 2 devices attached.

            <Root Hub>
                |d
            <Hub Device...............>
              |a     |b            |c
            <EP0>  <Interface 0>   |
                         |         |d
                       <EP1>     <Device Port 1> --- <Device Port 2>
                                   |a        |b        |a        |b
                                 <EP0> <Interface 0> <EP0> <Interface 0>
                                             |                   |
                                           <EP1>               <EP1>

            Link (a) is a pointer to an endpoint using the endpoint0 member.
            Link (b) is a pointer to an interface list using the interfaces
                member.
            Link (c) is a pointer to devices connected to a hub using the
                children member.
            Link (d) is a pointer to the parent hub using the parent member.
**/
//@{
/**
    @struct USBH_device
    @brief Structure that represents a USB device internally to the USBH
           library.
           The application will reference this structure as a handle.
 **/
struct USBH_device
{
    /**
        Marker to say that this device has been enumerated.
        This forms a fairly unique number which increments at
        each pass of enumeration. A zero value is not enumerated and non-zero
        value can be used to detect changes in enumeration and invalid handles.
        Size is 12 bits.
    **/
    uint16_t enumValue;
    /**
        Device information. Copy of USB Device Descriptor.
    **/
    /* May not be necessary, could just keep VID/PID and Class info. */
    USB_device_descriptor descriptor;

    /**
        Pointer to the next USBH_device. A NULL pointer indicates the end
        of the device list. This is a horizontal link to peers.
     **/
    USBH_device *next;

    /**
        Pointer to the parent USBH_device. A NULL pointer indicates that this
        device is connected to the root hub. This is an upwards link to a
        parent hub.
     **/
    USBH_device *parent;
    /**
        The port number which this device is connected to on the parent hub.
        For devices on the root hub this will be zero.
     **/
    uint8_t parentPort;

    /**
        Pointer to a structure of USB_endpoint for the control endpoints for
        this device.  This is common to all interfaces of this device.
     **/
    USBH_endpoint *endpoint0;
    /**
        Pointer to a list of USBH_interface structures for interfaces of this
        device. A NULL pointer indicates the end of the interface list.
        This is a downward link to children.
     **/
    USBH_interface *interfaces;
    /**
        Pointer to a list of child USBH_device structures. This is used when
        this device is a hub and there are devices connected to the ports.
        This is an upwards link to children.
     **/
    USBH_device *children;

    /**
        The bus address which is assigned to this device. Values are 0-127.
     **/
    uint8_t address;
    /**
        The bus speed which this device is configured to run at.
     **/
    uint8_t speed;
    /** Configuration number for this device set with SET_CONFIGURATION.
        A non-zero value means that it is operational.
    **/
    uint8_t configuration;

    /**
        HUB: The number of downstream ports on this device. This is used when
        this device is a hub and there are devices connected to the ports.
     **/
    uint8_t portCount;
    /**
        HUB: Poll interval timeout. This is approximated in milliseconds, but the
        bInterval field in the interface descriptor depends on the device
        speed. It is not used in the library - just calculated and kept for
        reference.
     **/
    uint16_t hubPollTimeout;
    /**
        HUB: Status change bitmap. This is the last value read from the
        hub's Status Change Endpoint. There can be (in theory) 256 ports on
        a hub (although only 127 devices).
     **/
    uint8_t hubStatusChange[16];
};

/**
    @struct USBH_interface
    @brief Structure that represents a USB interface internally within the
           USBH library.
           The application will reference this structure as a handle.
 **/
struct USBH_interface
{
    /**
        Marker to say that this interface has been enumerated.
        This forms a fairly unique number which increments at
        each pass of enumeration. A zero value is not enumerated and non-zero
        value can be used to detect changes in enumeration and invalid handles.
        Size is 12 bits.
    **/
    uint16_t enumValue;
    /**
        Back pointer to the parent USBH_device. This is an upwards link to the
        parent device.
     **/
    USBH_device *device;
    /**
        Pointer to the next USBH_interface. A NULL pointer indicates the end
        of the interface list. This is a horizontal link to peers.
     **/
    USBH_interface *next;
    /** Number of endpoints in this interface.
     **/
    uint8_t ep_count;
    /**
        Pointer to the first USB_endpoint of this interface. The first pointer
        will be the control endpoint for this interface. This may be common to
        other interfaces.
     **/
     /* TODO: Split into separate lists for control, bulk, and periodic? */
     /* TODO: Have considered using an endpoint number instead of a handle
        here but there is an issue in that each time it is referenced then the
        library will have to find said endpoint. Can do this by traversing a
        list (potentially slow) or keeping an array (could potentially waste
        memory). So use this method for now. */
    USBH_endpoint *endpoints;

    /** Interface Number setting for this interface to allow selection on
        standard requests. **/
    uint8_t number;
    /** Interface Class value.
     **/
    uint8_t usb_class;
    /** Interface Subclass value.
     **/
    uint8_t usb_subclass;
    /** Interface Protocol value.
     **/
    uint8_t usb_protocol;
    /** Alternate setting for this interface currently set with
        SET_INTERFACE. **/
    uint8_t alt;
    /** This interface is currently set with SET_INTERFACE. **/
    uint8_t set;
};

/**
    @struct USBH_endpoint
    @brief Structure that represents a USB endpoint. This structure is used
           to hold information about an endpoint internally in the USBH
           library.
           The actual data is stored within the EHCI Queue Head Descriptor.
           This structure will be populated by enumeration and a pointer
           returned to the application from the API when the endpoint is
           'discovered'.
           The application will reference this structure as a handle.
 **/
struct USBH_endpoint
{
    /**
        Marker to say that this endpoint has been enumerated.
        This forms a fairly unique number which increments at
        each pass of enumeration. A zero value is not enumerated and non-zero
        value can be used to detect changes in enumeration and invalid handles.
        Size is 12 bits.
    **/
    uint16_t enumValue;
    /*!*
        Interface information. Copy of USB Configuration Descriptor for
        interface.
    **/
    /* Remove: Unlikely to be useful in endpoint structure. */
    /*USB_endpoint_descriptor descriptor;*/
        /**
        Back pointer to the interface for this endpoint.
     **/
    USBH_interface *interface;
    /**
        Pointer to the next USB_endpoint within this interface. A NULL pointer
        indicates the end of the endpoint list.
     **/
    USBH_endpoint *next;

    /* Here's the relevant information syphoned off from the descriptor */
    /** Encodes USB endpoint number (0-15). **/
    USBH_ENDPOINT_NUMBER index;
    /** IN or OUT endpoint **/
    USBH_ENDPOINT_DIR direction;
    /** Endpoint max packet size **/
    USBH_ENDPOINT_SIZE max_packet_size;
    /** BULK, ISO, INT or CTRL endpoint **/
    USBH_ENDPOINT_TYPE type;
    /**
     	 Interval (for Interrupt and Isochronous endpoints).
     	 This is the number of microframes between polls of the endpoint.
    **/
    uint16_t interval;
    /**
      	 Additional transaction opportunities.
    **/
    uint8_t additional;

    /**
        Current known status. Stalled, etc.
     **/
    int8_t status;
    /**
        Last frame used.
        For isochronous endpoints this is the microframe that the
        endpoint last used for a transaction. This information plus
        the interval value (in microframes) can be used to make sure
        that the endpoint is not polled too often.
     **/
    int16_t last_microframe;
    /**
		Pointer to the queue head, isochronous transfer descriptor,
		or split isochronous transfer descriptor for this endpoint.
	 **/
    USBH_list_entry hc_entry;
    /**
        Pointer to the first qTD for this endpoint. When a qH is used.
     **/
    USBH_qtd *hc_head_qtd;
    /**
        Pointer to a dummy qTD for this endpoint. When a qH is used.
     **/
    USBH_qtd *hc_dummy_qtd;
    /**
        Pointer to the next endpoint as they appear in the periodic or
        asynchronous list. This is used to order endpoints by bInterval
        for interrupt and bulk endpoints in the periodic list. For the
        asynchronous list the endpoint order can be modified.
        This is not a per-interface list but a global list.
     **/
    USBH_endpoint *list_next;
};

/**
    @struct USBH_xfer
    @brief Structure that represents a USB transfer. This structure is used
           to hold information about a pending periodic or asynchronous transfer.
           The actual data is stored within the EHCI Transfer Descriptor (qTD).
           This structure will be populated by a call to perform a transfer.
           It is used internally in the USBH library to keep track of current
           transfers and any information pertinent to the transfer (e.g. for
           callback functions when completed).
 **/
struct USBH_xfer
{
    /**
        Endpoint reference. The target or source endpoint for data for this
		transaction.
		If this is NULL then the qh member is used.
    **/
    USBH_endpoint *endpoint;
    /**
        Timeout. Initially set with the timout period required. Decremented for
		each millisecond timer tick (call to USBH_timer).
		When this reaches zero the transfer is marked as timed-out and it's
		qTD removed from the queue.
    **/
	volatile uint16_t timeout;
    /**
        Length of data actually transferred.
     **/
    uint8_t length;
    /**
        Status. Stalled, completed, overrun, underrun etc.
     **/
    uint8_t status;
    /**
        Pointer to the queue header for this transfer in USBH memory (CPU address).
     **/
    USBH_list_entry hc_entry;
    /**
        Pointer to the transfer descriptor for this transfer in USBH memory (CPU address).
     **/
    USBH_qtd *hc_qtd;
    /**
        Pointer to the next USBH_xfer structure in a list of active transfers.
		A NULL pointer indicates the end of the transfer list.
		This list is separately maintained by the library to facilitate
		timeout processing. It is not related to the pointers in the qTD or
		QH structures.
     **/
    USBH_xfer *next;
    /**
         Asynchronous only: Identifier to pass to the Callback function to
         uniquely identify this transfer.
     */
    uint32_t id;
    /**
         Asynchronous only: Callback function to execute when transfer is
         completed.
     */
    USBH_callback callback;
    /**
        Asynchronous only: Pointer to data buffer in EHCI RAM memory to
        receive response from device or to hold data to transmit.
     */
    uint8_t *hc_buf;
    /**
        Size of EHCI RAM memory allocated for hc_buf.
     */
    size_t hc_len;
    /**
        Asynchronous only: Pointer to main memory data buffer where data
        received from the device are copied to.
     */
    uint8_t *dest_buf;
    /**
        Asynchronous only: Actual length of data placed in dest_buf buffer
        from EHCI RAM memory.
     */
    size_t dest_len;
    /**
        Asynchronous only: Microframe number of periodic schedule where this
        transfer has been added. For High-speed isochronous (iTD) this will
        identify the time slot in the iTD where the status can be found.
        Shift right 3 bits to obtain the frame number in the periodic list.
     */
    uint16_t microframe;
};
//@}

/* GLOBAL VARIABLES ****************************************************************/

/* MACROS **************************************************************************/

/**
    @name Handle macros
    @details Facilitates adding/removing addresses and enumeration values from
        Device, Endpoint and Interface handles.
 */
#define USBH_HANDLE_ADDR(A) ((uint32_t)A & (uint32_t)0xFFFFF)
#define USBH_HANDLE_ENUM_VAL(A) ((uint32_t)A >> 20)
#define USBH_HANDLE(A, B) (((uint32_t)A & (uint32_t)0xFFFFF) | ((uint32_t)B << 20))

/* FUNCTION PROTOTYPES *************************************************************/

// Hardware register functions
static void usbh_hw_init(void);
static void usbh_hw_enable_int(void);
#if 0
//TODO: currently not used
static void usbh_hw_disable_int(void)
static void usbh_bcdh_init(uint8_t mode);
#endif // 0
static void usbh_hw_pad_setup(void);
static void usbh_hw_run(void);
static void usbh_hw_stop(void);
static void usbh_hw_clrsts(void);
static void usbh_hw_reset (void);
static uint8_t usbh_hw_root_connected(void);
static void usbh_hw_root_c_connect_change(void);
static void usbh_hw_root_c_enable_change(void);
#if 0
//TODO: currently not used
static void usbh_hw_wait_for_connect(void);
#endif // 0
static uint8_t usbh_hw_get_device_speed(void);
static void usbh_hw_update_root_hub_status(void);
static void usbh_hw_update_root_hub_port_status(void);
void usbh_hw_wait_for_doorbell(void);

static uint32_t usbh_get_periodic_size(void);
static uint32_t usbh_get_periodic_addr(void);
static void usbh_write_periodic_list(uint32_t offset, uint32_t pointer);
#ifdef USBH_DEBUG_PERIODIC_LIST
static uint32_t usbh_read_periodic_list(uint32_t offset);
#endif // USBH_DEBUG_PERIODIC_LIST

// Reserved memory for management of host controller lists
static void usbh_alloc_init(void);
static USBH_device *usbh_alloc_device(void);
static USBH_interface *usbh_alloc_interface(void);
static USBH_endpoint *usbh_alloc_endpoint(void);
static USBH_xfer *usbh_alloc_transfer(void);
static void usbh_free_endpoint(USBH_endpoint *ep);
static void usbh_free_device(USBH_device *dev);
static void usbh_free_interface(USBH_interface *iface);
static void usbh_free_transfer(USBH_xfer *xfer);

// EHCI memory management functions for HC shared memory
static void usbh_alloc_hc_block(uint32_t start, uint32_t size);
static void *usbh_alloc_hc_2_blocks(void);
static void usbh_free_hc_2_blocks(void *hc);
static void *usbh_alloc_hc_1_block(void);
static void usbh_free_hc_1_block(void *hc);
static void *usbh_alloc_hc_buffer(uint32_t size);
static void usbh_free_hc_buffer(void *buf, uint32_t size);
static void usbh_hc_memclear(void);
// EHCI memory copy/set functions
void usbh_init_qtd(USBH_qtd *hc);
// Guaranteed atomic and of size 32 bit in HC shared memory
static inline void usbh_memcpy32_mem2hc(void *dst0, void *src0, uint32_t len);
static inline void usbh_memcpy8_hc2mem(void *dst0, void *src0, uint32_t len);
static inline void usbh_memcpy8_mem2hc(void *dst0, void *src0, uint32_t len);
static void usbh_memset32_hc(void *dst0, uint32_t val, uint32_t len);

// Asynchronous and Periodic List Functions
static void usbh_periodic_init(void);
static void usbh_asynch_init(void);
static int8_t usbh_ep_list_add(USBH_endpoint *this_ep, USBH_endpoint *list_ep, uint32_t listType);
static int8_t usbh_ep_list_remove(USBH_endpoint *this_ep, USBH_endpoint *list_ep);
static USBH_endpoint *usbh_ep_list_find_periodic(uint16_t interval);

// Endpoint initialisation
static int8_t usbh_init_ep_qh(USBH_list_entry *this_entry, USBH_qh *ctrl_qh, USBH_endpoint *ep);
static int8_t usbh_init_ctrlep(USBH_endpoint *ctrl_ep, USBH_qh *ctrl_qh,
		uint8_t speed, uint8_t newaddr,
		uint8_t hub, uint8_t port);

// Transfer List Functions
static void usbh_xfer_init(USBH_xfer *xfer);
static void usbh_xfer_add(USBH_xfer *xfer, USBH_xfer **list);
static void usbh_xfer_remove(USBH_xfer *xfer, USBH_xfer **list);
static int8_t usbh_xfer_complete(USBH_xfer *xfer);

// Generate transactions on the USB
static int32_t usbh_setup_transfer(USBH_endpoint *ep0, USB_device_request *devReq,
		uint8_t* buf, size_t len, uint16_t timeout);
static int32_t usbh_transfer_queued_worker(USBH_endpoint *ep, uint8_t *buffer, const size_t length,
		uint16_t timeout, uint32_t id, USBH_callback cb);
static int32_t usbh_transfer_iso_worker(USBH_endpoint *ep, uint8_t *buffer, const size_t length,
		uint16_t timeout, uint32_t id, USBH_callback cb);

// Read and filter interrupt status
static uint32_t usbh_interrupt(void);

// Wait for transaction completion
static int8_t usbh_wait_complete(USBH_xfer *xfer);

// Status management
static void usbh_update_xfer_timeouts(USBH_xfer *list);

// Enumeration and enumeration support functions
static int8_t usbh_hub_enumerate(USBH_device *usbHub, uint8_t configurationNum);
static void usbh_hub_poll_start(USBH_device *hubDev);
static int8_t usbh_hub_change(uint32_t id, int8_t status, size_t len, uint8_t *change);
static int8_t usbh_hub_port_update(USBH_device *usbHub, uint8_t port,
		uint8_t configurationNum, USBH_device **devNew);
static int8_t usbh_hub_port_enumerate(USBH_device *usbHub, uint8_t port,
		uint8_t configurationNum, USBH_device **newDev);
static int8_t usbh_hub_port_remove(USBH_device *usbHub, uint8_t port);

static int8_t usbh_req_get_descriptor(USBH_endpoint *ep0, uint8_t type,
		uint8_t index, size_t len, uint8_t *buf);
static int8_t usbh_req_set_address(USBH_endpoint *ep0, uint8_t addr);
static int8_t usbh_req_set_configuration(USBH_endpoint *ep0, uint8_t num);
static uint8_t usbh_get_hub_status_worker(USBH_device *usbHub, uint8_t index, USB_hub_port_status *buf);
static uint8_t usbh_get_hub_descriptor(USBH_device *usbHub, USB_hub_descriptor *buf);
#if 0
//TODO: currently not used
static uint8_t usbh_get_hub_port_speed(USBH_device *usbHub, uint8_t port);
#endif // 0
static uint8_t usbh_set_hub_port_feature(USBH_device *usbHub, uint8_t port, uint16_t feature);
static uint8_t usbh_clear_hub_port_feature(USBH_device *usbHub, uint8_t port, uint16_t feature);

static void usbh_dev_disconnect(USBH_device *usbDev);
static void usbh_dev_disconnect_config(USBH_device *devRemove, uint8_t keepDev);
static int8_t usbh_hub_enumerate(USBH_device *hubDev, uint8_t configurationNum);


/**
    @brief      Set the host controller to invalidate a transfer (DEPRECATED).
    @details    Remove the ECHI transfer descriptor for an asynchronous
				transfer.
                Transfer will be removed from the queue and invalidated.
                Deprecated as USBH_clear_endpoint_transfers() will remove
                active transactions without exposing the internal API.
    @param[in]  xfer Handle to an transfer.
    @returns    USBH_OK if successful.
                USBH_ERR_* if not successful.
 **/
int8_t USBH_clear_transfer(USBH_xfer *xfer); /* DEPRECATED */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FT900_USBH_INTERNAL_H_ */
