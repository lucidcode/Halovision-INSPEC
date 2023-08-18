/**
    @file

    @brief
    Additional functions for API for USB Host stack on FT900.

    API functions for USB Host stack. These functions provide additional
    functionality useful to implement a USB Host application.
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
#include <limits.h>

#include <ft900_usb.h>
#include <ft900_usbh.h>

/* CONSTANTS ***********************************************************************/

/* GLOBAL VARIABLES ****************************************************************/

/* LOCAL VARIABLES *****************************************************************/

/* MACROS **************************************************************************/

/* FUNCTIONS ***********************************************************************/

int8_t USBHX_find_by_vid_pid(USBH_device_handle *phDev,
		uint16_t usbVid, uint16_t usbPid)
{
    uint8_t status = USBH_OK;
    USBH_device_handle hDev;
    USBH_device_handle hChildDev;
	uint16_t usbThisVid;
    uint16_t usbThisPid;

    // Must specify pointers to the return handles.
    if (phDev == NULL)
    {
    	return USBH_ERR_PARAMETER;
    }

    // Copy the values of the handles passed in pointers. If the
    // function fails then the handles will be unaffected on return.
    hDev = *phDev;

    // If we have not been supplied with a device then start at the root
    // device and search for a match.
    if (hDev == 0)
    {
    	status = USBH_get_device_list(USBH_ROOT_HUB_HANDLE, &hDev);
    }

    if (status == USBH_OK)
    {
    	do {
			// Get the class information for this interface.
			if (USBH_device_get_vid_pid(hDev, &usbThisVid, &usbThisPid) == USBH_OK)
			{
				// If it is a matching interface then use that...
				if ((usbVid == usbThisVid) || (usbVid == 0xFFFF))
				{
					if ((usbPid == usbThisPid) || (usbPid == 0xFFFF))
					{
						// Match achieved. Return handles to device and interface.
						*phDev = hDev;
						return USBH_OK;
					}
				}
			}

			// Get any child devices of this device (i.e. this is a hub).
			status = USBH_get_device_list(hDev, &hChildDev);
			if (status == USBH_OK)
			{
				// Recursively scan child devices.
				status = USBHX_find_by_vid_pid(&hChildDev, usbVid, usbPid);
				if (status == USBH_OK)
				{
					*phDev = hChildDev;
					return USBH_OK;
				}
			}

			// Get any other devices on the same device.
			status = USBH_get_next_device(hDev, &hDev);

		} while (status == USBH_OK);
    }

    return status;
}

int8_t USBHX_find_by_class(USBH_device_handle *phDev, USBH_interface_handle *phInterface,
		uint8_t usbClass, uint8_t usbSubclass, uint8_t usbProtocol)
{
    uint8_t status = USBH_OK;
    USBH_device_handle hDev;
    USBH_device_handle hChildDev;
    USBH_interface_handle hInterface;
    USBH_interface_handle hChildInterface;
	uint8_t usbThisClass;
    uint8_t usbThisSubclass;
    uint8_t usbThisProtocol;

    // Must specify pointers to the return handles.
    if ((phDev == NULL) || (phInterface == NULL))
    {
    	return USBH_ERR_PARAMETER;
    }

    // Copy the values of the handles passed in pointers. If the
    // function fails then the handles will be unaffected on return.
    hDev = *phDev;
    hInterface = *phInterface;

    // If we have not been supplied with a device then start at the root
    // device and search for a match.
    if (hDev == 0)
    {
    	status = USBH_get_device_list(USBH_ROOT_HUB_HANDLE, &hDev);
    }

    if (status == USBH_OK)
    {
    	do {
			status = USBH_OK;
			// If we have been supplied a device and no interface then find the
			// first interface on the device.
        	if (hInterface == 0)
			{
				status = USBH_get_interface_list(hDev, &hInterface);
			}

			while (status == USBH_OK)
			{
				// Get the class information for this interface.
				if (USBH_interface_get_class_info(hInterface, &usbThisClass, &usbThisSubclass, &usbThisProtocol) == USBH_OK)
				{
					// If it is a matching interface then use that...
					if ((usbClass == usbThisClass) || (usbClass == 0xFF))
					{
						if ((usbSubclass == usbThisSubclass) || (usbSubclass == 0xFF))
						{
							if ((usbProtocol == usbThisProtocol) || (usbProtocol == 0xFF))
							{
								// Match achieved. Return handles to device and interface.
								*phDev = hDev;
								*phInterface = hInterface;
								return USBH_OK;
							}
						}
					}
				}
				// Next interface on this device
				status = USBH_get_next_interface(hInterface, &hInterface);
			}

			// Get any child devices of this device (i.e. this is a hub).
			status = USBH_get_device_list(hDev, &hChildDev);
			if (status == USBH_OK)
			{
				hChildInterface = 0;
				// Recursively scan child devices.
				status = USBHX_find_by_class(&hChildDev, &hChildInterface,
						usbClass, usbSubclass, usbProtocol);
				if (status == USBH_OK)
				{
					*phDev = hChildDev;
					*phInterface = hChildInterface;
					return USBH_OK;
				}
			}

			// Get any other devices on the same device.
			status = USBH_get_next_device(hDev, &hDev);
			hInterface = 0;

		} while (status == USBH_OK);
    }

    return status;
}

USBH_STATE USBHX_root_enumerate_wait(void)
{
    USBH_STATE connect;

    // Check first for an already connected device
	do
	{
		USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);
	} while (connect == USBH_STATE_NOTCONNECTED);

	// Start enumeration at root hub
	USBH_enumerate(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT);

	// Update connect status - enumeration may have failed
	USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);

	return connect;
}

int8_t USBHX_root_connected(void)
{
    USBH_STATE connect;

	USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);

	if (connect < USBH_STATE_CONNECTED)
		return 0;
	return 1;
}

int8_t USBHX_root_enumerated(void)
{
    USBH_STATE connect;

	USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);

	if (connect < USBH_STATE_ENUMERATED)
		return 0;
	return 1;
}

int8_t USBHX_root_enumeration_failed(void)
{
    USBH_STATE connect;

	USBH_get_connect_state(USBH_ROOT_HUB_HANDLE, USBH_ROOT_HUB_PORT, &connect);

	if (connect == USBH_STATE_ENUMERATED)
		return 0;
	return 1;
}

static int8_t usbhx_enumerate_parse_config_desc(USBH_device_handle hDev, uint8_t type, uint8_t index,
		uint16_t offset, uint16_t len, uint8_t *buf, uint8_t *buf0)
{
	int8_t status;

	status = USBH_device_get_descriptor(hDev, type,	index, offset + len, (uint8_t *)buf0);
	memcpy(buf, &buf0[offset], len);

	return status;
}

static int8_t usbhx_enumerate_parse_config_desc_64(USBH_device_handle hDev, uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf)
{
	uint8_t buf0[64];
	return usbhx_enumerate_parse_config_desc(hDev, type, index, offset, len, buf, buf0);
}

int8_t usbhx_enumerate_parse_config_desc_128(USBH_device_handle hDev, uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf)
{
	uint8_t buf0[128];
	return usbhx_enumerate_parse_config_desc(hDev, type, index, offset, len, buf, buf0);
}

int8_t usbhx_enumerate_parse_config_desc_256(USBH_device_handle hDev, uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf)
{
	uint8_t buf0[256];
	return usbhx_enumerate_parse_config_desc(hDev, type, index, offset, len, buf, buf0);
}

int8_t usbhx_enumerate_parse_config_desc_512(USBH_device_handle hDev, uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf)
{
	uint8_t buf0[512];
	return usbhx_enumerate_parse_config_desc(hDev, type, index, offset, len, buf, buf0);
}

int8_t usbhx_enumerate_parse_config_desc_1024(USBH_device_handle hDev, uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf)
{
	uint8_t buf0[1024];
	return usbhx_enumerate_parse_config_desc(hDev, type, index, offset, len, buf, buf0);
}

int8_t usbhx_enumerate_parse_config_desc_2048(USBH_device_handle hDev, uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf)
{
	uint8_t buf0[2048];
	return usbhx_enumerate_parse_config_desc(hDev, type, index, offset, len, buf, buf0);
}

int8_t usbhx_enumerate_parse_config_desc_4096(USBH_device_handle hDev, uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf)
{
	uint8_t buf0[4096];
	return usbhx_enumerate_parse_config_desc(hDev, type, index, offset, len, buf, buf0);
}

int8_t USBHX_get_config_descriptor(USBH_device_handle hDev, uint8_t type, uint8_t index, uint16_t offset, uint16_t len, uint8_t *buf)
{
	int8_t status = USBH_ERR_NOT_FOUND;;
	uint16_t confDescLen;

	confDescLen = len + offset;

	if (confDescLen <= 64)
	{
		status = usbhx_enumerate_parse_config_desc_64(hDev, type, index, offset, len, buf);
	}
	else if (confDescLen <= 128)
	{
		status = usbhx_enumerate_parse_config_desc_128(hDev, type, index, offset, len, buf);
	}
	else if (confDescLen <= 256)
	{
		status = usbhx_enumerate_parse_config_desc_256(hDev, type, index, offset, len, buf);
	}
	else if (confDescLen <= 512)
	{
		status = usbhx_enumerate_parse_config_desc_512(hDev, type, index, offset, len, buf);
	}
	else if (confDescLen <= 1024)
	{
		status = usbhx_enumerate_parse_config_desc_1024(hDev, type, index, offset, len, buf);
	}
	else if (confDescLen <= 2048)
	{
		status = usbhx_enumerate_parse_config_desc_2048(hDev, type, index, offset, len, buf);
	}
	else if (confDescLen <= 4096)
	{
		status = usbhx_enumerate_parse_config_desc_4096(hDev, type, index, offset, len, buf);
	}

	return status;
}

