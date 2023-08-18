#pragma once

#include <stdbool.h>

#include "broadcom/gen/vcmailbox.h"

bool vcmailbox_get_power_state(vcmailbox_device_t device);
bool vcmailbox_set_power_state(vcmailbox_device_t device, bool on);

uint32_t* vcmailbox_get_framebuffer(uint32_t* virtual_width, uint32_t* virtual_height,
                                    uint32_t* physical_width, uint32_t* physical_height,
                                    uint32_t* pitch, uint32_t* bits_per_pixel);
bool vcmailbox_release_framebuffer(void);
uint64_t vcmailbox_get_serial_number(void);
uint32_t vcmailbox_get_board_model(void);
uint32_t vcmailbox_get_board_revision(void);
uint32_t vcmailbox_get_firmware_revision(void);
uint32_t vcmailbox_get_temperature(void);
uint32_t vcmailbox_get_clock_rate_measured(vcmailbox_clock_id_t clock_id);
