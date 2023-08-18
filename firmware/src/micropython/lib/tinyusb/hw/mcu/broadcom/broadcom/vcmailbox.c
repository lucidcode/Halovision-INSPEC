#include <stdint.h>
#include <string.h>

#include "broadcom/caches.h"
#include "broadcom/cpu.h"
#include "broadcom/defines.h"
#include "broadcom/gen/vcmailbox.h"

STRICT_ALIGN bool vcmailbox_request(volatile vcmailbox_buffer_t* buffer) {
    size_t buffer_size = buffer->buffer_size;
    buffer->code = VCMAILBOX_CODE_PROCESS_REQUEST;
    COMPLETE_MEMORY_READS;
    while (VCMAILBOX->STATUS0_b.FULL) {}
    while (!VCMAILBOX->STATUS0_b.EMPTY) {
        VCMAILBOX->READ;
    }
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    uint32_t buffer_address = 0x00000000 | (uint32_t) buffer | 8;
    #pragma GCC diagnostic pop
    data_clean(buffer, buffer_size);
    
    VCMAILBOX->WRITE = buffer_address;
    size_t count = 0;
    while (VCMAILBOX->STATUS0_b.EMPTY || VCMAILBOX->READ != buffer_address) {
        count++;
        if (count > 10000000) {
            return false;
        }
    }
    COMPLETE_MEMORY_READS;
    data_invalidate(buffer, buffer_size);
    return buffer->code == VCMAILBOX_CODE_REQUEST_SUCCESSFUL;
}

STRICT_ALIGN uint32_t compute_size(uint32_t tag_size) {
    uint32_t size = VCMAILBOX_HEADER_SIZE + tag_size + sizeof(uint32_t);
    if (size % 16 != 0) {
        size += 16 - (size % 16);
    }
    return size;
}

bool vcmailbox_get_power_state(vcmailbox_device_t device) {
    int size = compute_size(sizeof(vcmailbox_get_power_state_t));
    vcmailbox_buffer_t* buf = (vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset(buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_get_power_state_t* tag = (vcmailbox_get_power_state_t*) &buf->data;
    tag->tag = VCMAILBOX_TAG_GET_POWER_STATE;
    tag->is_response = false;
    tag->value_size = sizeof(vcmailbox_get_power_state_request_t);
    tag->request.device_id = (uint32_t) device;
    vcmailbox_request(buf);
    return tag->response.state == 1;
}

bool vcmailbox_set_power_state(vcmailbox_device_t device, bool on) {
    int size = compute_size(sizeof(vcmailbox_set_power_state_t));
    vcmailbox_buffer_t* buf = (vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset(buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_set_power_state_t* tag = (vcmailbox_set_power_state_t*) &buf->data;
    tag->tag = VCMAILBOX_TAG_SET_POWER_STATE;
    tag->is_response = false;
    tag->request.device_id = (uint32_t) device;
    tag->value_size = sizeof(vcmailbox_set_power_state_request_t);
    // Always wait for power stability.
    tag->request.state = on ? 0x3 : 0x2;
    vcmailbox_request(buf);
    return tag->response.state == 1;
}

// Private struct for laying out tag sequence.
typedef struct __attribute__((packed)) {
    uint32_t buffer_size;
    uint32_t code;
    // Series of tagged data where the final tag is 0x0.
    vcmailbox_set_physical_display_width_height_t physical_size;
    vcmailbox_set_virtual_buffer_width_height_t virtual_size;
    vcmailbox_set_virtual_offset_t virtual_offset;
    vcmailbox_set_depth_t depth;
    vcmailbox_set_pixel_order_t pixel_order;
    vcmailbox_allocate_buffer_t buf;
    vcmailbox_get_pitch_t pitch;
    uint32_t final_tag;
} vc_framebuffer_request_t;

uint32_t* vcmailbox_get_framebuffer(uint32_t* virtual_width, uint32_t* virtual_height,
                               uint32_t* physical_width, uint32_t* physical_height,
                               uint32_t* pitch, uint32_t* bits_per_pixel) {
    int size = compute_size(sizeof(vc_framebuffer_request_t));
    vc_framebuffer_request_t* buf = (vc_framebuffer_request_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset(buf, 0, size);
    buf->buffer_size = size;

    buf->physical_size = VCMAILBOX_SET_PHYSICAL_DISPLAY_WIDTH_HEIGHT_DEFAULTS;
    buf->physical_size.request.width = *physical_width;
    buf->physical_size.request.height = *physical_height;
    buf->virtual_size = VCMAILBOX_SET_VIRTUAL_BUFFER_WIDTH_HEIGHT_DEFAULTS;
    buf->virtual_size.request.width = *virtual_width;
    buf->virtual_size.request.height = *virtual_height;
    buf->virtual_offset = VCMAILBOX_SET_VIRTUAL_OFFSET_DEFAULTS;
    buf->virtual_offset.request.x = 0;
    buf->virtual_offset.request.y = 0;
    buf->depth = VCMAILBOX_SET_DEPTH_DEFAULTS;
    buf->depth.request.bits_per_pixel = 32;
    buf->pixel_order = VCMAILBOX_SET_PIXEL_ORDER_DEFAULTS;
    buf->pixel_order.request.state = 1;
    buf->buf = VCMAILBOX_ALLOCATE_BUFFER_DEFAULTS;
    buf->buf.request.alignment = 4096;
    buf->pitch = VCMAILBOX_GET_PITCH_DEFAULTS;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Waddress-of-packed-member"
    #pragma GCC diagnostic ignored "-Wcast-align"
    bool ok = vcmailbox_request((vcmailbox_buffer_t*) buf);
    #pragma GCC diagnostic pop
    if (!ok) {
        *virtual_width = 0;
        *virtual_height = 0;
        *physical_width = 0;
        *physical_height = 0;
        return NULL;
    }
    *virtual_width = buf->virtual_size.response.width;
    *virtual_height = buf->virtual_size.response.height;
    *physical_width = buf->physical_size.response.width;
    *physical_height = buf->physical_size.response.height;
    *pitch = buf->pitch.response.bytes_per_line;
    *bits_per_pixel = buf->depth.response.bits_per_pixel;
    return (uint32_t*) (size_t) (buf->buf.response.frame_buffer_base_address & 0x3fffffff);
}

bool vcmailbox_release_framebuffer(void) {
    int size = compute_size(sizeof(vcmailbox_release_buffer_t));
    vcmailbox_buffer_t* buf = (vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset(buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_release_buffer_t* tag = (vcmailbox_release_buffer_t*) &buf->data;
    *tag = VCMAILBOX_RELEASE_BUFFER_DEFAULTS;
    return vcmailbox_request(buf);
}

uint64_t vcmailbox_get_serial_number(void) {
    int size = compute_size(sizeof(vcmailbox_get_board_serial_t));
    vcmailbox_buffer_t* buf = (vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset(buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_get_board_serial_t* tag = (vcmailbox_get_board_serial_t*) &buf->data;
    *tag = VCMAILBOX_GET_BOARD_SERIAL_DEFAULTS;
    vcmailbox_request(buf);
    return tag->response.board_serial;
}

uint32_t vcmailbox_get_board_model(void) {
    int size = compute_size(sizeof(vcmailbox_get_board_model_t));
    vcmailbox_buffer_t* buf = (vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset(buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_get_board_model_t* tag = (vcmailbox_get_board_model_t*) &buf->data;
    *tag = VCMAILBOX_GET_BOARD_MODEL_DEFAULTS;
    vcmailbox_request(buf);
    return tag->response.board_model;
}

STRICT_ALIGN uint32_t vcmailbox_get_board_revision(void) {
    int size = compute_size(sizeof(vcmailbox_get_board_revision_t));
    vcmailbox_buffer_t* buf = (vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset(buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_get_board_revision_t* tag = (vcmailbox_get_board_revision_t*) &buf->data;
    *tag = VCMAILBOX_GET_BOARD_REVISION_DEFAULTS;
    vcmailbox_request(buf);
    return tag->response.board_revision;
}

uint32_t vcmailbox_get_firmware_revision(void) {
    int size = compute_size(sizeof(vcmailbox_get_board_model_t));
    volatile vcmailbox_buffer_t* buf = (volatile vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset((uint32_t*) buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_get_firmware_revision_t* tag = (vcmailbox_get_firmware_revision_t*) &buf->data;
    *tag = VCMAILBOX_GET_FIRMWARE_REVISION_DEFAULTS;
    vcmailbox_request(buf);
    return tag->response.firmware_revision;
}

uint32_t vcmailbox_get_temperature(void) {
    int size = compute_size(sizeof(vcmailbox_get_temperature_t));
    volatile vcmailbox_buffer_t* buf = (volatile vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset((uint32_t*) buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_get_temperature_t* tag = (vcmailbox_get_temperature_t*) &buf->data;
    *tag = VCMAILBOX_GET_TEMPERATURE_DEFAULTS;
    tag->request.temperature_id = 0; // always 0
    if (!vcmailbox_request(buf)) {
        return 0;
    }
    return tag->response.value;
}

uint32_t vcmailbox_get_clock_rate_measured(vcmailbox_clock_id_t clock_id) {
    int size = compute_size(sizeof(vcmailbox_get_clock_rate_measured_t));
    volatile vcmailbox_buffer_t* buf = (volatile vcmailbox_buffer_t*) __builtin_alloca_with_align(size, 16 * 8);
    memset((uint32_t*) buf, 0, size);
    buf->buffer_size = size;
    vcmailbox_get_clock_rate_measured_t* tag = (vcmailbox_get_clock_rate_measured_t*) &buf->data;
    *tag = VCMAILBOX_GET_CLOCK_RATE_MEASURED_DEFAULTS;
    tag->request.clock_id = clock_id;
    vcmailbox_request(buf);
    return tag->response.rate_in_hz;
}
