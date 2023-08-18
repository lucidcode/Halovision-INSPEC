/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2022 Blake W. Felt & Angus Gratton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mpconfig.h"

#if MICROPY_HW_ENABLE_USBDEV

#include "tusb.h"
#include "mp_usbd.h"
#include "mp_usbd_internal.h"

#define USBD_CDC_CMD_MAX_SIZE (8)
#define USBD_CDC_IN_OUT_MAX_SIZE (64)

const tusb_desc_device_t mp_usbd_desc_device_static = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = MICROPY_HW_USB_VID,
    .idProduct = MICROPY_HW_USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = USBD_STR_MANUF,
    .iProduct = USBD_STR_PRODUCT,
    .iSerialNumber = USBD_STR_SERIAL,
    .bNumConfigurations = 1,
};

const uint8_t mp_usbd_desc_cfg_static[USBD_STATIC_DESC_LEN] = {
    TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_STATIC_MAX, USBD_STR_0, USBD_STATIC_DESC_LEN,
        0, USBD_MAX_POWER_MA),

    #if CFG_TUD_CDC
    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
    #endif
    #if CFG_TUD_MSC
    TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, 5, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
    #endif
};

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    char serial_buf[MICROPY_HW_USB_DESC_STR_MAX + 1]; // Includes terminating NUL byte
    static uint16_t desc_wstr[MICROPY_HW_USB_DESC_STR_MAX + 1]; // Includes prefix uint16_t
    const char *desc_str;
    uint16_t desc_len;

    switch (index) {
        case 0:
            desc_wstr[1] = 0x0409; // supported language is English
            desc_len = 4;
            break;
        case USBD_STR_SERIAL:
            // TODO: make a port-specific serial number callback
            mp_usbd_port_get_serial_number(serial_buf);
            desc_str = serial_buf;
            break;
        case USBD_STR_MANUF:
            desc_str = MICROPY_HW_USB_MANUFACTURER_STRING;
            break;
        case USBD_STR_PRODUCT:
            desc_str = MICROPY_HW_USB_PRODUCT_FS_STRING;
            break;
        #if CFG_TUD_CDC
        case USBD_STR_CDC:
            desc_str = MICROPY_HW_USB_CDC_INTERFACE_STRING;
            break;
        #endif
        #if CFG_TUD_MSC
        case USBD_STR_MSC:
            desc_str = MICROPY_HW_USB_MSC_INTERFACE_STRING;
            break;
        #endif
        default:
            desc_str = NULL;
    }

    if (index != 0) {
        if (desc_str == NULL) {
            return NULL; // Will STALL the endpoint
        }

        // Convert from narrow string to wide string
        desc_len = 2;
        for (int i = 0; i < MICROPY_HW_USB_DESC_STR_MAX && desc_str[i] != 0; i++) {
            desc_wstr[1 + i] = desc_str[i];
            desc_len += 2;
        }
    }
    // first byte is length (including header), second byte is string type
    desc_wstr[0] = (TUSB_DESC_STRING << 8) | desc_len;

    return desc_wstr;
}


const uint8_t *tud_descriptor_device_cb(void) {
    return (const void *)&mp_usbd_desc_device_static;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return mp_usbd_desc_cfg_static;
}

#endif
