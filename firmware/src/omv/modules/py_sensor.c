/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Sensor Python module.
 */
#include <stdarg.h>
#include <stdio.h>
#include "py/mphal.h"
#include "py/runtime.h"

#if MICROPY_PY_SENSOR

#include "sensor.h"
#include "imlib.h"
#include "xalloc.h"
#include "py_assert.h"
#include "py_image.h"
#if MICROPY_PY_IMU
#include "py_imu.h"
#endif
#include "omv_boardconfig.h"
#include "omv_i2c.h"
#include "py_helper.h"
#include "framebuffer.h"

extern sensor_t sensor;
static mp_obj_t vsync_callback = mp_const_none;
static mp_obj_t frame_callback = mp_const_none;

#define sensor_raise_error(err) mp_raise_msg(&mp_type_RuntimeError, (mp_rom_error_text_t) sensor_strerror(err))
#define sensor_print_error(op)  printf("\x1B[31mWARNING: %s control is not supported by this image sensor.\x1B[0m\n", op);

#if MICROPY_PY_IMU
static void do_auto_rotation(int pitch_deadzone, int roll_activezone) {
    if (sensor_get_auto_rotation()) {
        float pitch = py_imu_pitch_rotation();
        if (((pitch <= (90 - pitch_deadzone)) || ((90 + pitch_deadzone) < pitch))
            && ((pitch <= (270 - pitch_deadzone)) || ((270 + pitch_deadzone) < pitch))) {
            // disable when 90 or 270
            float roll = py_imu_roll_rotation();
            if (((360 - roll_activezone) <= roll) || (roll < (0 + roll_activezone)) ) {
                // center is 0/360, upright
                sensor_set_hmirror(false);
                sensor_set_vflip(false);
                sensor_set_transpose(false);
            } else if (((270 - roll_activezone) <= roll) && (roll < (270 + roll_activezone))) {
                // center is 270, rotated right
                sensor_set_hmirror(true);
                sensor_set_vflip(false);
                sensor_set_transpose(true);
            } else if (((180 - roll_activezone) <= roll) && (roll < (180 + roll_activezone))) {
                // center is 180, upside down
                sensor_set_hmirror(true);
                sensor_set_vflip(true);
                sensor_set_transpose(false);
            } else if (((90 - roll_activezone) <= roll) && (roll < (90 + roll_activezone))) {
                // center is 90, rotated left
                sensor_set_hmirror(false);
                sensor_set_vflip(true);
                sensor_set_transpose(true);
            }
        }
    }
}
#endif // MICROPY_PY_IMU

static mp_obj_t py_sensor__init__() {
    // This is the module init function, not the sensor init function,
    // it gets called when the module is imported. This is good
    // place to check if the sensor was detected or not.
    if (sensor_is_detected() == false) {
        sensor_raise_error(SENSOR_ERROR_ISC_UNDETECTED);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor__init__obj, py_sensor__init__);

static mp_obj_t py_sensor_reset() {
    int error = sensor_reset();
    if (error != 0) {
        sensor_raise_error(error);
    }
    #if MICROPY_PY_IMU
    // +-10 degree dead-zone around pitch 90/270.
    // +-45 degree active-zone around roll 0/90/180/270/360.
    do_auto_rotation(10, 45);
    // We're setting the dead-zone on pitch because roll readings are invalid there.
    // We're setting the full range on roll to set the initial state.
    #endif // MICROPY_PY_IMU
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_reset_obj, py_sensor_reset);

static mp_obj_t py_sensor_sleep(mp_obj_t enable) {
    PY_ASSERT_FALSE_MSG(sensor_sleep(mp_obj_is_true(enable)) != 0, "Sleep Failed");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_sleep_obj, py_sensor_sleep);

static mp_obj_t py_sensor_shutdown(mp_obj_t enable) {
    PY_ASSERT_FALSE_MSG(sensor_shutdown(mp_obj_is_true(enable)) != 0, "Shutdown Failed");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_shutdown_obj, py_sensor_shutdown);

static mp_obj_t py_sensor_flush() {
    framebuffer_update_jpeg_buffer();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_flush_obj, py_sensor_flush);

static mp_obj_t py_sensor_snapshot(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    #if MICROPY_PY_IMU
    // +-10 degree dead-zone around pitch 90/270.
    // +-35 degree active-zone around roll 0/90/180/270/360.
    do_auto_rotation(10, 35);
    // We're setting the dead-zone on pitch because roll readings are invalid there.
    // We're not setting the full range on roll to prevent oscillation.
    #endif // MICROPY_PY_IMU

    mp_obj_t image = py_image(0, 0, 0, 0, 0);
    int error = sensor.snapshot(&sensor, (image_t *) py_image_cobj(image), 0);
    if (error != 0) {
        sensor_raise_error(error);
    }
    return image;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_sensor_snapshot_obj, 0, py_sensor_snapshot);

static mp_obj_t py_sensor_skip_frames(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_map_elem_t *kw_arg = mp_map_lookup(kw_args, MP_ROM_QSTR(MP_QSTR_time), MP_MAP_LOOKUP);
    mp_int_t time = 300; // OV Recommended.

    if (kw_arg != NULL) {
        time = mp_obj_get_int(kw_arg->value);
    }

    uint32_t millis = mp_hal_ticks_ms();

    if (!n_args) {
        while ((mp_hal_ticks_ms() - millis) < time) {
            // 32-bit math handles wrap around...
            py_sensor_snapshot(0, NULL, NULL);
        }
    } else {
        for (int i = 0, j = mp_obj_get_int(args[0]); i < j; i++) {
            if ((kw_arg != NULL) && ((mp_hal_ticks_ms() - millis) >= time)) {
                break;
            }

            py_sensor_snapshot(0, NULL, NULL);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_sensor_skip_frames_obj, 0, py_sensor_skip_frames);

static mp_obj_t py_sensor_width() {
    return mp_obj_new_int(resolution[sensor.framesize][0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_width_obj, py_sensor_width);

static mp_obj_t py_sensor_height() {
    return mp_obj_new_int(resolution[sensor.framesize][1]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_height_obj, py_sensor_height);

static mp_obj_t py_sensor_get_fb() {
    if (framebuffer_get_depth() < 0) {
        return mp_const_none;
    }

    image_t image;
    framebuffer_init_image(&image);
    return py_image_from_struct(&image);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_fb_obj, py_sensor_get_fb);

static mp_obj_t py_sensor_get_id() {
    return mp_obj_new_int(sensor_get_id());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_id_obj, py_sensor_get_id);

static mp_obj_t py_sensor_get_frame_available() {
    return mp_obj_new_bool(framebuffer->tail != framebuffer->head);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_frame_available_obj, py_sensor_get_frame_available);

static mp_obj_t py_sensor_alloc_extra_fb(mp_obj_t w_obj, mp_obj_t h_obj, mp_obj_t pixfmt_obj) {
    int w = mp_obj_get_int(w_obj);
    PY_ASSERT_TRUE_MSG(w > 0, "Width must be > 0");

    int h = mp_obj_get_int(h_obj);
    PY_ASSERT_TRUE_MSG(h > 0, "Height must be > 0");

    pixformat_t pixfmt = mp_obj_get_int(pixfmt_obj);
    PY_ASSERT_TRUE_MSG(IMLIB_PIXFORMAT_IS_VALID(pixfmt), "Invalid Pixel Format");

    image_t img = {.w = w, .h = h, .pixfmt = pixfmt, .size = 0, .pixels = 0};

    // Alloc image first (could fail) then alloc RAM so that there's no leak on failure.
    mp_obj_t r = py_image_from_struct(&img);

    fb_alloc_mark();
    ((image_t *) py_image_cobj(r))->pixels = fb_alloc0(image_size(&img), FB_ALLOC_NO_HINT);
    fb_alloc_mark_permanent(); // pixels will not be popped on exception
    return r;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(py_sensor_alloc_extra_fb_obj, py_sensor_alloc_extra_fb);

static mp_obj_t py_sensor_dealloc_extra_fb() {
    fb_alloc_free_till_mark_past_mark_permanent();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_dealloc_extra_fb_obj, py_sensor_dealloc_extra_fb);

static mp_obj_t py_sensor_set_pixformat(mp_obj_t pixformat) {
    int error = sensor_set_pixformat(mp_obj_get_int(pixformat));
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_pixformat_obj, py_sensor_set_pixformat);

static mp_obj_t py_sensor_get_pixformat() {
    if (sensor.pixformat == PIXFORMAT_INVALID) {
        sensor_raise_error(SENSOR_ERROR_INVALID_PIXFORMAT);
    }
    return mp_obj_new_int(sensor.pixformat);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_pixformat_obj, py_sensor_get_pixformat);

static mp_obj_t py_sensor_set_framesize(mp_obj_t framesize) {
    int error = sensor_set_framesize(mp_obj_get_int(framesize));
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_framesize_obj, py_sensor_set_framesize);

static mp_obj_t py_sensor_get_framesize() {
    if (sensor.framesize == FRAMESIZE_INVALID) {
        sensor_raise_error(SENSOR_ERROR_INVALID_FRAMESIZE);
    }
    return mp_obj_new_int(sensor.framesize);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_framesize_obj, py_sensor_get_framesize);

static mp_obj_t py_sensor_set_framerate(mp_obj_t framerate) {
    int error = sensor_set_framerate(mp_obj_get_int(framerate));
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_framerate_obj, py_sensor_set_framerate);

static mp_obj_t py_sensor_get_framerate() {
    if (sensor.framerate == 0) {
        sensor_raise_error(SENSOR_ERROR_INVALID_FRAMERATE);
    }
    return mp_obj_new_int(sensor.framerate);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_framerate_obj, py_sensor_get_framerate);

static mp_obj_t py_sensor_set_windowing(uint n_args, const mp_obj_t *args) {
    if (sensor.framesize == FRAMESIZE_INVALID) {
        sensor_raise_error(SENSOR_ERROR_INVALID_FRAMESIZE);
    }

    rectangle_t temp;
    temp.x = 0;
    temp.y = 0;
    temp.w = resolution[sensor.framesize][0];
    temp.h = resolution[sensor.framesize][1];

    mp_obj_t *array = (mp_obj_t *) args;
    mp_uint_t array_len = n_args;

    if (n_args == 1) {
        mp_obj_get_array(args[0], &array_len, &array);
    }

    rectangle_t r;

    if (array_len == 2) {
        r.w = mp_obj_get_int(array[0]);
        r.h = mp_obj_get_int(array[1]);
        r.x = (temp.w / 2) - (r.w / 2);
        r.y = (temp.h / 2) - (r.h / 2);
    } else if (array_len == 4) {
        r.x = mp_obj_get_int(array[0]);
        r.y = mp_obj_get_int(array[1]);
        r.w = mp_obj_get_int(array[2]);
        r.h = mp_obj_get_int(array[3]);
    } else {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("The tuple/list must either be (x, y, w, h) or (w, h)"));
    }

    if ((r.w < 1) || (r.h < 1)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid ROI dimensions!"));
    }

    if (!rectangle_overlap(&r, &temp)) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("ROI does not overlap on the image!"));
    }

    rectangle_intersected(&r, &temp);

    int error = sensor_set_windowing(r.x, r.y, r.w, r.h);
    if (error != 0) {
        sensor_raise_error(error);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_sensor_set_windowing_obj, 1, 4, py_sensor_set_windowing);

static mp_obj_t py_sensor_get_windowing() {
    if (sensor.framesize == FRAMESIZE_INVALID) {
        sensor_raise_error(SENSOR_ERROR_INVALID_FRAMESIZE);
    }

    return mp_obj_new_tuple(4, (mp_obj_t []) {mp_obj_new_int(framebuffer_get_x()),
                                              mp_obj_new_int(framebuffer_get_y()),
                                              mp_obj_new_int(framebuffer_get_u()),
                                              mp_obj_new_int(framebuffer_get_v())});
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_windowing_obj, py_sensor_get_windowing);

static mp_obj_t py_sensor_set_gainceiling(mp_obj_t gainceiling) {
    gainceiling_t gain;
    switch (mp_obj_get_int(gainceiling)) {
        case 2:
            gain = GAINCEILING_2X;
            break;
        case 4:
            gain = GAINCEILING_4X;
            break;
        case 8:
            gain = GAINCEILING_8X;
            break;
        case 16:
            gain = GAINCEILING_16X;
            break;
        case 32:
            gain = GAINCEILING_32X;
            break;
        case 64:
            gain = GAINCEILING_64X;
            break;
        case 128:
            gain = GAINCEILING_128X;
            break;
        default:
            sensor_raise_error(SENSOR_ERROR_INVALID_ARGUMENT);
            break;
    }

    if (sensor_set_gainceiling(gain) != 0) {
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_gainceiling_obj, py_sensor_set_gainceiling);

static mp_obj_t py_sensor_set_brightness(mp_obj_t brightness) {
    if (sensor_set_brightness(mp_obj_get_int(brightness)) != 0) {
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_brightness_obj, py_sensor_set_brightness);

static mp_obj_t py_sensor_set_contrast(mp_obj_t contrast) {
    if (sensor_set_contrast(mp_obj_get_int(contrast)) != 0) {
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_contrast_obj, py_sensor_set_contrast);

static mp_obj_t py_sensor_set_saturation(mp_obj_t saturation) {
    if (sensor_set_saturation(mp_obj_get_int(saturation)) != 0) {
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_saturation_obj, py_sensor_set_saturation);

static mp_obj_t py_sensor_set_quality(mp_obj_t qs) {
    int q = mp_obj_get_int(qs);
    PY_ASSERT_TRUE((q >= 0 && q <= 100));

    q = 100 - q; //invert quality
    q = 255 * q / 100; //map to 0->255
    if (sensor_set_quality(q) != 0) {
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_quality_obj, py_sensor_set_quality);

static mp_obj_t py_sensor_set_colorbar(mp_obj_t enable) {
    if (sensor_set_colorbar(mp_obj_is_true(enable)) != 0) {
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_colorbar_obj, py_sensor_set_colorbar);

static mp_obj_t py_sensor_set_auto_gain(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_gain_db, ARG_gain_db_ceiling };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_gain_db, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_gain_db_ceiling, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args.
    int enable = mp_obj_get_int(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    float gain_db = py_helper_arg_to_float(args[ARG_gain_db].u_obj, NAN);
    float gain_db_ceiling = py_helper_arg_to_float(args[ARG_gain_db_ceiling].u_obj, NAN);

    int error = sensor_set_auto_gain(enable, gain_db, gain_db_ceiling);
    if (error != 0) {
        if (error != SENSOR_ERROR_CTL_UNSUPPORTED) {
            sensor_raise_error(error);
        }
        sensor_print_error("Auto Gain");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_sensor_set_auto_gain_obj, 1, py_sensor_set_auto_gain);

static mp_obj_t py_sensor_get_gain_db() {
    float gain_db;
    int error = sensor_get_gain_db(&gain_db);
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_obj_new_float(gain_db);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_gain_db_obj, py_sensor_get_gain_db);

static mp_obj_t py_sensor_set_auto_exposure(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_exposure_us };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_exposure_us, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1} },
    };

    // Parse args.
    int enable = mp_obj_get_int(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int error = sensor_set_auto_exposure(enable, args[ARG_exposure_us].u_int);
    if (error != 0) {
        if (error != SENSOR_ERROR_CTL_UNSUPPORTED) {
            sensor_raise_error(error);
        }
        sensor_print_error("Auto Exposure");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_sensor_set_auto_exposure_obj, 1, py_sensor_set_auto_exposure);

static mp_obj_t py_sensor_get_exposure_us() {
    int exposure_us;
    int error = sensor_get_exposure_us(&exposure_us);
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_obj_new_int(exposure_us);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_exposure_us_obj, py_sensor_get_exposure_us);

static mp_obj_t py_sensor_set_auto_whitebal(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_rgb_gain_db };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_rgb_gain_db, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args.
    int enable = mp_obj_get_int(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    float rgb_gain_db[3] = {NAN, NAN, NAN};
    py_helper_arg_to_float_array(args[ARG_rgb_gain_db].u_obj, rgb_gain_db, 3);

    int error = sensor_set_auto_whitebal(enable, rgb_gain_db[0], rgb_gain_db[1], rgb_gain_db[2]);
    if (error != 0) {
        if (error != SENSOR_ERROR_CTL_UNSUPPORTED) {
            sensor_raise_error(error);
        }
        sensor_print_error("Auto White Balance");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_sensor_set_auto_whitebal_obj, 1, py_sensor_set_auto_whitebal);

static mp_obj_t py_sensor_get_rgb_gain_db() {
    float r_gain_db = 0.0, g_gain_db = 0.0, b_gain_db = 0.0;
    int error = sensor_get_rgb_gain_db(&r_gain_db, &g_gain_db, &b_gain_db);
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_obj_new_tuple(3, (mp_obj_t []) {
        mp_obj_new_float(r_gain_db),
        mp_obj_new_float(g_gain_db),
        mp_obj_new_float(b_gain_db)
    });
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_rgb_gain_db_obj, py_sensor_get_rgb_gain_db);

static mp_obj_t py_sensor_set_auto_blc(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_enable, ARG_regs };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_enable, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_regs,   MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int enable = args[ARG_enable].u_int;
    int regs[sensor.hw_flags.blc_size];
    bool regs_present = args[ARG_regs].u_obj != mp_const_none;
    if (regs_present) {
        mp_obj_t *arg_array;
        mp_obj_get_array_fixed_n(args[ARG_regs].u_obj, sensor.hw_flags.blc_size, &arg_array);
        for (uint32_t i = 0; i < sensor.hw_flags.blc_size; i++) {
            regs[i] = mp_obj_get_int(arg_array[i]);
        }
    }

    int error = sensor_set_auto_blc(enable, regs_present ? regs : NULL);
    if (error != 0) {
        if (error != SENSOR_ERROR_CTL_UNSUPPORTED) {
            sensor_raise_error(error);
        }
        sensor_print_error("Auto BLC");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_sensor_set_auto_blc_obj, 1,  py_sensor_set_auto_blc);

static mp_obj_t py_sensor_get_blc_regs() {
    int regs[sensor.hw_flags.blc_size];
    int error = sensor_get_blc_regs(regs);
    if (error != 0) {
        sensor_raise_error(error);
    }

    mp_obj_list_t *l = mp_obj_new_list(sensor.hw_flags.blc_size, NULL);
    for (uint32_t i = 0; i < sensor.hw_flags.blc_size; i++) {
        l->items[i] = mp_obj_new_int(regs[i]);
    }
    return l;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_blc_regs_obj, py_sensor_get_blc_regs);

static mp_obj_t py_sensor_set_hmirror(mp_obj_t enable) {
    int error = sensor_set_hmirror(mp_obj_is_true(enable));
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_hmirror_obj, py_sensor_set_hmirror);

static mp_obj_t py_sensor_get_hmirror() {
    return mp_obj_new_bool(sensor_get_hmirror());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_hmirror_obj, py_sensor_get_hmirror);

static mp_obj_t py_sensor_set_vflip(mp_obj_t enable) {
    int error = sensor_set_vflip(mp_obj_is_true(enable));
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_vflip_obj, py_sensor_set_vflip);

static mp_obj_t py_sensor_get_vflip() {
    return mp_obj_new_bool(sensor_get_vflip());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_vflip_obj, py_sensor_get_vflip);

static mp_obj_t py_sensor_set_transpose(mp_obj_t enable) {
    int error = sensor_set_transpose(mp_obj_is_true(enable));
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_transpose_obj, py_sensor_set_transpose);

static mp_obj_t py_sensor_get_transpose() {
    return mp_obj_new_bool(sensor_get_transpose());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_transpose_obj, py_sensor_get_transpose);

static mp_obj_t py_sensor_set_auto_rotation(mp_obj_t enable) {
    int error = sensor_set_auto_rotation(mp_obj_is_true(enable));
    if (error != 0) {
        sensor_raise_error(error);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_auto_rotation_obj, py_sensor_set_auto_rotation);

static mp_obj_t py_sensor_get_auto_rotation() {
    return mp_obj_new_bool(sensor_get_auto_rotation());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_auto_rotation_obj, py_sensor_get_auto_rotation);

static mp_obj_t py_sensor_set_framebuffers(mp_obj_t count) {
    mp_int_t c = mp_obj_get_int(count);

    if (framebuffer->n_buffers == c) {
        return mp_const_none;
    }

    if (c < 1) {
        sensor_raise_error(SENSOR_ERROR_INVALID_ARGUMENT);
    }

    int error = sensor_set_framebuffers(c);
    if (error != 0) {
        sensor_raise_error(error);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_framebuffers_obj, py_sensor_set_framebuffers);

static mp_obj_t py_sensor_get_framebuffers() {
    return mp_obj_new_int(framebuffer->n_buffers);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_framebuffers_obj, py_sensor_get_framebuffers);

static mp_obj_t py_sensor_disable_delays(uint n_args, const mp_obj_t *args) {
    if (!n_args) {
        return mp_obj_new_bool(sensor.disable_delays);
    }

    sensor.disable_delays = mp_obj_get_int(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_sensor_disable_delays_obj, 0, 1, py_sensor_disable_delays);

static mp_obj_t py_sensor_disable_full_flush(uint n_args, const mp_obj_t *args) {
    if (!n_args) {
        return mp_obj_new_bool(sensor.disable_full_flush);
    }

    sensor.disable_full_flush = mp_obj_get_int(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_sensor_disable_full_flush_obj, 0, 1, py_sensor_disable_full_flush);

static mp_obj_t py_sensor_set_special_effect(mp_obj_t sde) {
    if (sensor_set_special_effect(mp_obj_get_int(sde)) != 0) {
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_special_effect_obj, py_sensor_set_special_effect);

static mp_obj_t py_sensor_set_lens_correction(mp_obj_t enable, mp_obj_t radi, mp_obj_t coef) {
    if (sensor_set_lens_correction(mp_obj_is_true(enable),
                                   mp_obj_get_int(radi), mp_obj_get_int(coef)) != 0) {
        return mp_const_false;
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(py_sensor_set_lens_correction_obj, py_sensor_set_lens_correction);

static void sensor_vsync_callback(uint32_t vsync) {
    if (mp_obj_is_callable(vsync_callback)) {
        mp_call_function_1(vsync_callback, mp_obj_new_int(vsync));
    }
}

static mp_obj_t py_sensor_set_vsync_callback(mp_obj_t vsync_callback_obj) {
    if (!mp_obj_is_callable(vsync_callback_obj)) {
        vsync_callback = mp_const_none;
        sensor_set_vsync_callback(NULL);
    } else {
        vsync_callback = vsync_callback_obj;
        sensor_set_vsync_callback(sensor_vsync_callback);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_vsync_callback_obj, py_sensor_set_vsync_callback);

static void sensor_frame_callback() {
    if (mp_obj_is_callable(frame_callback)) {
        mp_call_function_0(frame_callback);
    }
}

static mp_obj_t py_sensor_set_frame_callback(mp_obj_t frame_callback_obj) {
    if (!mp_obj_is_callable(frame_callback_obj)) {
        frame_callback = mp_const_none;
        sensor_set_frame_callback(NULL);
    } else {
        frame_callback = frame_callback_obj;
        sensor_set_frame_callback(sensor_frame_callback);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_frame_callback_obj, py_sensor_set_frame_callback);

static mp_obj_t py_sensor_ioctl(uint n_args, const mp_obj_t *args) {
    mp_obj_t ret_obj = mp_const_none;
    int request = mp_obj_get_int(args[0]);
    int error = SENSOR_ERROR_INVALID_ARGUMENT;

    switch (request) {
        case IOCTL_SET_READOUT_WINDOW: {
            if (n_args >= 2) {
                int x, y, w, h;
                mp_obj_t *array;
                mp_uint_t array_len;
                mp_obj_get_array(args[1], &array_len, &array);

                if (array_len == 4) {
                    x = mp_obj_get_int(array[0]);
                    y = mp_obj_get_int(array[1]);
                    w = mp_obj_get_int(array[2]);
                    h = mp_obj_get_int(array[3]);
                } else if (array_len == 2) {
                    w = mp_obj_get_int(array[0]);
                    h = mp_obj_get_int(array[1]);
                    x = 0;
                    y = 0;
                } else {
                    mp_raise_msg(&mp_type_ValueError,
                                 MP_ERROR_TEXT("The tuple/list must either be (x, y, w, h) or (w, h)"));
                }

                error = sensor_ioctl(request, x, y, w, h);
            }
            break;
        }

        case IOCTL_GET_READOUT_WINDOW: {
            int x, y, w, h;
            error = sensor_ioctl(request, &x, &y, &w, &h);
            if (error == 0) {
                ret_obj = mp_obj_new_tuple(4, (mp_obj_t []) {mp_obj_new_int(x),
                                                             mp_obj_new_int(y),
                                                             mp_obj_new_int(w),
                                                             mp_obj_new_int(h)});
            }
            break;
        }

        case IOCTL_SET_TRIGGERED_MODE:
        case IOCTL_SET_FOV_WIDE:
        case IOCTL_SET_NIGHT_MODE: {
            if (n_args >= 2) {
                error = sensor_ioctl(request, mp_obj_get_int(args[1]));
            }
            break;
        }

        case IOCTL_GET_TRIGGERED_MODE:
        case IOCTL_GET_FOV_WIDE:
        case IOCTL_GET_NIGHT_MODE: {
            int enabled;
            error = sensor_ioctl(request, &enabled);
            if (error == 0) {
                ret_obj = mp_obj_new_bool(enabled);
            }
            break;
        }

        #if (OMV_OV5640_AF_ENABLE == 1)
        case IOCTL_TRIGGER_AUTO_FOCUS:
        case IOCTL_PAUSE_AUTO_FOCUS:
        case IOCTL_RESET_AUTO_FOCUS: {
            error = sensor_ioctl(request);
            break;
        }
        case IOCTL_WAIT_ON_AUTO_FOCUS: {
            error = sensor_ioctl(request, (n_args < 2) ? 5000 : mp_obj_get_int(args[1]));
            break;
        }
        #endif

        case IOCTL_LEPTON_GET_WIDTH: {
            int width;
            error = sensor_ioctl(request, &width);
            if (error == 0) {
                ret_obj = mp_obj_new_int(width);
            }
            break;
        }

        case IOCTL_LEPTON_GET_HEIGHT: {
            int height;
            error = sensor_ioctl(request, &height);
            if (error == 0) {
                ret_obj = mp_obj_new_int(height);
            }
            break;
        }

        case IOCTL_LEPTON_GET_RADIOMETRY: {
            int radiometry;
            error = sensor_ioctl(request, &radiometry);
            if (error == 0) {
                ret_obj = mp_obj_new_int(radiometry);
            }
            break;
        }

        case IOCTL_LEPTON_GET_REFRESH: {
            int refresh;
            error = sensor_ioctl(request, &refresh);
            if (error == 0) {
                ret_obj = mp_obj_new_int(refresh);
            }
            break;
        }

        case IOCTL_LEPTON_GET_RESOLUTION: {
            int resolution;
            error = sensor_ioctl(request, &resolution);
            if (error == 0) {
                ret_obj = mp_obj_new_int(resolution);
            }
            break;
        }

        case IOCTL_LEPTON_RUN_COMMAND: {
            if (n_args >= 2) {
                error = sensor_ioctl(request, mp_obj_get_int(args[1]));
            }
            break;
        }

        case IOCTL_LEPTON_SET_ATTRIBUTE: {
            if (n_args >= 3) {
                size_t data_len;
                int command = mp_obj_get_int(args[1]);
                uint16_t *data = (uint16_t *) mp_obj_str_get_data(args[2], &data_len);
                PY_ASSERT_TRUE_MSG(data_len > 0, "0 bytes transferred!");
                error = sensor_ioctl(request, command, data, data_len / sizeof(uint16_t));
            }
            break;
        }

        case IOCTL_LEPTON_GET_ATTRIBUTE: {
            if (n_args >= 3) {
                int command = mp_obj_get_int(args[1]);
                size_t data_len = mp_obj_get_int(args[2]);
                PY_ASSERT_TRUE_MSG(data_len > 0, "0 bytes transferred!");
                uint16_t *data = xalloc(data_len * sizeof(uint16_t));
                error = sensor_ioctl(request, command, data, data_len);
                if (error == 0) {
                    ret_obj = mp_obj_new_bytearray_by_ref(data_len * sizeof(uint16_t), data);
                }
            }
            break;
        }

        case IOCTL_LEPTON_GET_FPA_TEMPERATURE:
        case IOCTL_LEPTON_GET_AUX_TEMPERATURE: {
            int temp;
            error = sensor_ioctl(request, &temp);
            if (error == 0) {
                ret_obj = mp_obj_new_float((((float) temp) / 100) - 273.15f);
            }
            break;
        }

        case IOCTL_LEPTON_SET_MEASUREMENT_MODE:
            if (n_args >= 2) {
                int high_temp = (n_args == 2) ? false : mp_obj_get_int(args[2]);
                error = sensor_ioctl(request, mp_obj_get_int(args[1]), high_temp);
            }
            break;

        case IOCTL_LEPTON_GET_MEASUREMENT_MODE: {
            int enabled, high_temp;
            error = sensor_ioctl(request, &enabled, &high_temp);
            if (error == 0) {
                ret_obj = mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_bool(enabled), mp_obj_new_bool(high_temp)});
            }
            break;
        }

        case IOCTL_LEPTON_SET_MEASUREMENT_RANGE:
            if (n_args >= 3) {
                // GCC will not let us pass floats to ... so we have to pass float pointers instead.
                float min = mp_obj_get_float(args[1]);
                float max = mp_obj_get_float(args[2]);
                error = sensor_ioctl(request, &min, &max);
            }
            break;

        case IOCTL_LEPTON_GET_MEASUREMENT_RANGE: {
            float min, max;
            error = sensor_ioctl(request, &min, &max);
            if (error == 0) {
                ret_obj = mp_obj_new_tuple(2, (mp_obj_t []) {mp_obj_new_float(min), mp_obj_new_float(max)});
            }
            break;
        }

        #if (OMV_HM01B0_ENABLE == 1)
        case IOCTL_HIMAX_MD_ENABLE: {
            if (n_args >= 2) {
                error = sensor_ioctl(request, mp_obj_get_int(args[1]));
            }
            break;
        }

        case IOCTL_HIMAX_MD_WINDOW: {
            if (n_args >= 2) {
                int x, y, w, h;
                mp_obj_t *array;
                mp_uint_t array_len;
                mp_obj_get_array(args[1], &array_len, &array);

                if (array_len == 4) {
                    x = mp_obj_get_int(array[0]);
                    y = mp_obj_get_int(array[1]);
                    w = mp_obj_get_int(array[2]);
                    h = mp_obj_get_int(array[3]);
                } else if (array_len == 2) {
                    w = mp_obj_get_int(array[0]);
                    h = mp_obj_get_int(array[1]);
                    x = 0;
                    y = 0;
                } else {
                    mp_raise_msg(&mp_type_ValueError,
                                 MP_ERROR_TEXT("The tuple/list must either be (x, y, w, h) or (w, h)"));
                }

                error = sensor_ioctl(request, x, y, w, h);
            }
            break;
        }

        case IOCTL_HIMAX_MD_THRESHOLD: {
            if (n_args >= 2) {
                error = sensor_ioctl(request, mp_obj_get_int(args[1]));
            }
            break;
        }

        case IOCTL_HIMAX_MD_CLEAR: {
            error = sensor_ioctl(request);
            break;
        }

        case IOCTL_HIMAX_OSC_ENABLE: {
            if (n_args >= 2) {
                error = sensor_ioctl(request, mp_obj_get_int(args[1]));
            }
            break;
        }
        #endif // (OMV_HM01B0_ENABLE == 1)

        default: {
            sensor_raise_error(SENSOR_ERROR_CTL_UNSUPPORTED);
            break;
        }
    }

    if (error != 0) {
        sensor_raise_error(error);
    }

    return ret_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(py_sensor_ioctl_obj, 1, 5, py_sensor_ioctl);

static mp_obj_t py_sensor_set_color_palette(mp_obj_t palette_obj) {
    int palette = mp_obj_get_int(palette_obj);
    switch (palette) {
        case COLOR_PALETTE_RAINBOW:
            sensor_set_color_palette(rainbow_table);
            break;
        case COLOR_PALETTE_IRONBOW:
            sensor_set_color_palette(ironbow_table);
            break;
        default:
            sensor_raise_error(SENSOR_ERROR_INVALID_ARGUMENT);
            break;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_set_color_palette_obj, py_sensor_set_color_palette);

static mp_obj_t py_sensor_get_color_palette() {
    const uint16_t *palette = sensor_get_color_palette();
    if (palette == rainbow_table) {
        return mp_obj_new_int(COLOR_PALETTE_RAINBOW);
    } else if (palette == ironbow_table) {
        return mp_obj_new_int(COLOR_PALETTE_IRONBOW);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(py_sensor_get_color_palette_obj, py_sensor_get_color_palette);

static mp_obj_t py_sensor_write_reg(mp_obj_t addr, mp_obj_t val) {
    sensor_write_reg(mp_obj_get_int(addr), mp_obj_get_int(val));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(py_sensor_write_reg_obj, py_sensor_write_reg);

static mp_obj_t py_sensor_read_reg(mp_obj_t addr) {
    return mp_obj_new_int(sensor_read_reg(mp_obj_get_int(addr)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_sensor_read_reg_obj, py_sensor_read_reg);

STATIC const mp_rom_map_elem_t globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_sensor)},

    // Pixel Formats
    { MP_ROM_QSTR(MP_QSTR_BINARY),              MP_ROM_INT(PIXFORMAT_BINARY)},      /* 1BPP/BINARY*/
    { MP_ROM_QSTR(MP_QSTR_GRAYSCALE),           MP_ROM_INT(PIXFORMAT_GRAYSCALE)},   /* 1BPP/GRAYSCALE*/
    { MP_ROM_QSTR(MP_QSTR_RGB565),              MP_ROM_INT(PIXFORMAT_RGB565)},      /* 2BPP/RGB565*/
    { MP_ROM_QSTR(MP_QSTR_BAYER),               MP_ROM_INT(PIXFORMAT_BAYER)},       /* 1BPP/RAW*/
    { MP_ROM_QSTR(MP_QSTR_YUV422),              MP_ROM_INT(PIXFORMAT_YUV422)},      /* 2BPP/YUV422*/
    { MP_ROM_QSTR(MP_QSTR_JPEG),                MP_ROM_INT(PIXFORMAT_JPEG)},        /* JPEG/COMPRESSED*/

    // Image Sensors
    { MP_ROM_QSTR(MP_QSTR_OV2640),              MP_ROM_INT(OV2640_ID)},
    { MP_ROM_QSTR(MP_QSTR_OV5640),              MP_ROM_INT(OV5640_ID)},
    { MP_ROM_QSTR(MP_QSTR_OV7670),              MP_ROM_INT(OV7670_ID)},
    { MP_ROM_QSTR(MP_QSTR_OV7690),              MP_ROM_INT(OV7690_ID)},
    { MP_ROM_QSTR(MP_QSTR_OV7725),              MP_ROM_INT(OV7725_ID)},
    { MP_ROM_QSTR(MP_QSTR_OV9650),              MP_ROM_INT(OV9650_ID)},
    { MP_ROM_QSTR(MP_QSTR_MT9V022),             MP_ROM_INT(MT9V0X2_ID)},
    { MP_ROM_QSTR(MP_QSTR_MT9V024),             MP_ROM_INT(MT9V0X4_ID)},
    { MP_ROM_QSTR(MP_QSTR_MT9V032),             MP_ROM_INT(MT9V0X2_ID)},
    { MP_ROM_QSTR(MP_QSTR_MT9V034),             MP_ROM_INT(MT9V0X4_ID)},
    { MP_ROM_QSTR(MP_QSTR_MT9M114),             MP_ROM_INT(MT9M114_ID)},
    { MP_ROM_QSTR(MP_QSTR_LEPTON),              MP_ROM_INT(LEPTON_ID)},
    { MP_ROM_QSTR(MP_QSTR_HM01B0),              MP_ROM_INT(HM01B0_ID)},
    { MP_ROM_QSTR(MP_QSTR_GC2145),              MP_ROM_INT(GC2145_ID)},
    { MP_ROM_QSTR(MP_QSTR_PAJ6100),             MP_ROM_INT(PAJ6100_ID)},
    { MP_ROM_QSTR(MP_QSTR_FROGEYE2020),         MP_ROM_INT(FROGEYE2020_ID)},

    // Special effects
    { MP_ROM_QSTR(MP_QSTR_NORMAL),              MP_ROM_INT(SDE_NORMAL)},          /* Normal/No SDE */
    { MP_ROM_QSTR(MP_QSTR_NEGATIVE),            MP_ROM_INT(SDE_NEGATIVE)},        /* Negative image */

    // C/SIF Resolutions
    { MP_ROM_QSTR(MP_QSTR_QQCIF),               MP_ROM_INT(FRAMESIZE_QQCIF)},    /* 88x72     */
    { MP_ROM_QSTR(MP_QSTR_QCIF),                MP_ROM_INT(FRAMESIZE_QCIF)},     /* 176x144   */
    { MP_ROM_QSTR(MP_QSTR_CIF),                 MP_ROM_INT(FRAMESIZE_CIF)},      /* 352x288   */
    { MP_ROM_QSTR(MP_QSTR_QQSIF),               MP_ROM_INT(FRAMESIZE_QQSIF)},    /* 88x60     */
    { MP_ROM_QSTR(MP_QSTR_QSIF),                MP_ROM_INT(FRAMESIZE_QSIF)},     /* 176x120   */
    { MP_ROM_QSTR(MP_QSTR_SIF),                 MP_ROM_INT(FRAMESIZE_SIF)},      /* 352x240   */
    // VGA Resolutions
    { MP_ROM_QSTR(MP_QSTR_QQQQVGA),             MP_ROM_INT(FRAMESIZE_QQQQVGA)},  /* 40x30     */
    { MP_ROM_QSTR(MP_QSTR_QQQVGA),              MP_ROM_INT(FRAMESIZE_QQQVGA)},   /* 80x60     */
    { MP_ROM_QSTR(MP_QSTR_QQVGA),               MP_ROM_INT(FRAMESIZE_QQVGA)},    /* 160x120   */
    { MP_ROM_QSTR(MP_QSTR_QVGA),                MP_ROM_INT(FRAMESIZE_QVGA)},     /* 320x240   */
    { MP_ROM_QSTR(MP_QSTR_VGA),                 MP_ROM_INT(FRAMESIZE_VGA)},      /* 640x480   */
    { MP_ROM_QSTR(MP_QSTR_HQQQQVGA),            MP_ROM_INT(FRAMESIZE_HQQQQVGA)}, /* 40x20     */
    { MP_ROM_QSTR(MP_QSTR_HQQQVGA),             MP_ROM_INT(FRAMESIZE_HQQQVGA)},  /* 80x40     */
    { MP_ROM_QSTR(MP_QSTR_HQQVGA),              MP_ROM_INT(FRAMESIZE_HQQVGA)},   /* 160x80    */
    { MP_ROM_QSTR(MP_QSTR_HQVGA),               MP_ROM_INT(FRAMESIZE_HQVGA)},    /* 240x160   */
    { MP_ROM_QSTR(MP_QSTR_HVGA),                MP_ROM_INT(FRAMESIZE_HVGA)},     /* 480x320   */
    // FFT Resolutions
    { MP_ROM_QSTR(MP_QSTR_B64X32),              MP_ROM_INT(FRAMESIZE_64X32)},    /* 64x32     */
    { MP_ROM_QSTR(MP_QSTR_B64X64),              MP_ROM_INT(FRAMESIZE_64X64)},    /* 64x64     */
    { MP_ROM_QSTR(MP_QSTR_B128X64),             MP_ROM_INT(FRAMESIZE_128X64)},   /* 128x64    */
    { MP_ROM_QSTR(MP_QSTR_B128X128),            MP_ROM_INT(FRAMESIZE_128X128)},  /* 128x128   */
    // Himax Resolutions
    { MP_ROM_QSTR(MP_QSTR_B160X160),            MP_ROM_INT(FRAMESIZE_160X160)},  /* 160x160   */
    { MP_ROM_QSTR(MP_QSTR_B320X320),            MP_ROM_INT(FRAMESIZE_320X320)},  /* 320x320   */
    // Other Resolutions
    { MP_ROM_QSTR(MP_QSTR_LCD),                 MP_ROM_INT(FRAMESIZE_LCD)},      /* 128x160   */
    { MP_ROM_QSTR(MP_QSTR_QQVGA2),              MP_ROM_INT(FRAMESIZE_QQVGA2)},   /* 128x160   */
    { MP_ROM_QSTR(MP_QSTR_WVGA),                MP_ROM_INT(FRAMESIZE_WVGA)},     /* 720x480   */
    { MP_ROM_QSTR(MP_QSTR_WVGA2),               MP_ROM_INT(FRAMESIZE_WVGA2)},    /* 752x480   */
    { MP_ROM_QSTR(MP_QSTR_SVGA),                MP_ROM_INT(FRAMESIZE_SVGA)},     /* 800x600   */
    { MP_ROM_QSTR(MP_QSTR_XGA),                 MP_ROM_INT(FRAMESIZE_XGA)},      /* 1024x768  */
    { MP_ROM_QSTR(MP_QSTR_WXGA),                MP_ROM_INT(FRAMESIZE_WXGA)},     /* 1280x768  */
    { MP_ROM_QSTR(MP_QSTR_SXGA),                MP_ROM_INT(FRAMESIZE_SXGA)},     /* 1280x1024 */
    { MP_ROM_QSTR(MP_QSTR_SXGAM),               MP_ROM_INT(FRAMESIZE_SXGAM)},    /* 1280x960  */
    { MP_ROM_QSTR(MP_QSTR_UXGA),                MP_ROM_INT(FRAMESIZE_UXGA)},     /* 1600x1200 */
    { MP_ROM_QSTR(MP_QSTR_HD),                  MP_ROM_INT(FRAMESIZE_HD)},       /* 1280x720  */
    { MP_ROM_QSTR(MP_QSTR_FHD),                 MP_ROM_INT(FRAMESIZE_FHD)},      /* 1920x1080 */
    { MP_ROM_QSTR(MP_QSTR_QHD),                 MP_ROM_INT(FRAMESIZE_QHD)},      /* 2560x1440 */
    { MP_ROM_QSTR(MP_QSTR_QXGA),                MP_ROM_INT(FRAMESIZE_QXGA)},     /* 2048x1536 */
    { MP_ROM_QSTR(MP_QSTR_WQXGA),               MP_ROM_INT(FRAMESIZE_WQXGA)},    /* 2560x1600 */
    { MP_ROM_QSTR(MP_QSTR_WQXGA2),              MP_ROM_INT(FRAMESIZE_WQXGA2)},   /* 2592x1944 */

    // Framebuffer Sizes
    { MP_ROM_QSTR(MP_QSTR_SINGLE_BUFFER),       MP_ROM_INT(1)},
    { MP_ROM_QSTR(MP_QSTR_DOUBLE_BUFFER),       MP_ROM_INT(2)},
    { MP_ROM_QSTR(MP_QSTR_TRIPLE_BUFFER),       MP_ROM_INT(3)},
    { MP_ROM_QSTR(MP_QSTR_VIDEO_FIFO),          MP_ROM_INT(4)},

    // IOCTLs
    { MP_ROM_QSTR(MP_QSTR_IOCTL_SET_READOUT_WINDOW),            MP_ROM_INT(IOCTL_SET_READOUT_WINDOW)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_GET_READOUT_WINDOW),            MP_ROM_INT(IOCTL_GET_READOUT_WINDOW)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_SET_TRIGGERED_MODE),            MP_ROM_INT(IOCTL_SET_TRIGGERED_MODE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_GET_TRIGGERED_MODE),            MP_ROM_INT(IOCTL_GET_TRIGGERED_MODE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_SET_FOV_WIDE),                  MP_ROM_INT(IOCTL_SET_FOV_WIDE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_GET_FOV_WIDE),                  MP_ROM_INT(IOCTL_GET_FOV_WIDE)},
    #if (OMV_OV5640_AF_ENABLE == 1)
    { MP_ROM_QSTR(MP_QSTR_IOCTL_TRIGGER_AUTO_FOCUS),            MP_ROM_INT(IOCTL_TRIGGER_AUTO_FOCUS)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_PAUSE_AUTO_FOCUS),              MP_ROM_INT(IOCTL_PAUSE_AUTO_FOCUS)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_RESET_AUTO_FOCUS),              MP_ROM_INT(IOCTL_RESET_AUTO_FOCUS)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_WAIT_ON_AUTO_FOCUS),            MP_ROM_INT(IOCTL_WAIT_ON_AUTO_FOCUS)},
    #endif
    { MP_ROM_QSTR(MP_QSTR_IOCTL_SET_NIGHT_MODE),                MP_ROM_INT(IOCTL_SET_NIGHT_MODE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_GET_NIGHT_MODE),                MP_ROM_INT(IOCTL_GET_NIGHT_MODE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_WIDTH),              MP_ROM_INT(IOCTL_LEPTON_GET_WIDTH)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_HEIGHT),             MP_ROM_INT(IOCTL_LEPTON_GET_HEIGHT)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_RADIOMETRY),         MP_ROM_INT(IOCTL_LEPTON_GET_RADIOMETRY)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_REFRESH),            MP_ROM_INT(IOCTL_LEPTON_GET_REFRESH)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_RESOLUTION),         MP_ROM_INT(IOCTL_LEPTON_GET_RESOLUTION)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_RUN_COMMAND),            MP_ROM_INT(IOCTL_LEPTON_RUN_COMMAND)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_SET_ATTRIBUTE),          MP_ROM_INT(IOCTL_LEPTON_SET_ATTRIBUTE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_ATTRIBUTE),          MP_ROM_INT(IOCTL_LEPTON_GET_ATTRIBUTE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_FPA_TEMPERATURE),    MP_ROM_INT(IOCTL_LEPTON_GET_FPA_TEMPERATURE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_AUX_TEMPERATURE),    MP_ROM_INT(IOCTL_LEPTON_GET_AUX_TEMPERATURE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_SET_MEASUREMENT_MODE),   MP_ROM_INT(IOCTL_LEPTON_SET_MEASUREMENT_MODE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_MEASUREMENT_MODE),   MP_ROM_INT(IOCTL_LEPTON_GET_MEASUREMENT_MODE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_SET_MEASUREMENT_RANGE),  MP_ROM_INT(IOCTL_LEPTON_SET_MEASUREMENT_RANGE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_LEPTON_GET_MEASUREMENT_RANGE),  MP_ROM_INT(IOCTL_LEPTON_GET_MEASUREMENT_RANGE)},
    #if (OMV_HM01B0_ENABLE == 1)
    { MP_ROM_QSTR(MP_QSTR_IOCTL_HIMAX_MD_ENABLE),       MP_ROM_INT(IOCTL_HIMAX_MD_ENABLE)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_HIMAX_MD_WINDOW),       MP_ROM_INT(IOCTL_HIMAX_MD_WINDOW)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_HIMAX_MD_THRESHOLD),    MP_ROM_INT(IOCTL_HIMAX_MD_THRESHOLD)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_HIMAX_MD_CLEAR),        MP_ROM_INT(IOCTL_HIMAX_MD_CLEAR)},
    { MP_ROM_QSTR(MP_QSTR_IOCTL_HIMAX_OSC_ENABLE),      MP_ROM_INT(IOCTL_HIMAX_OSC_ENABLE)},
    #endif

    // Sensor functions
    { MP_ROM_QSTR(MP_QSTR___init__),            MP_ROM_PTR(&py_sensor__init__obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&py_sensor_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep),               MP_ROM_PTR(&py_sensor_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_shutdown),            MP_ROM_PTR(&py_sensor_shutdown_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush),               MP_ROM_PTR(&py_sensor_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_snapshot),            MP_ROM_PTR(&py_sensor_snapshot_obj) },
    { MP_ROM_QSTR(MP_QSTR_skip_frames),         MP_ROM_PTR(&py_sensor_skip_frames_obj) },
    { MP_ROM_QSTR(MP_QSTR_width),               MP_ROM_PTR(&py_sensor_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height),              MP_ROM_PTR(&py_sensor_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_fb),              MP_ROM_PTR(&py_sensor_get_fb_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_id),              MP_ROM_PTR(&py_sensor_get_id_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_frame_available), MP_ROM_PTR(&py_sensor_get_frame_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_alloc_extra_fb),      MP_ROM_PTR(&py_sensor_alloc_extra_fb_obj) },
    { MP_ROM_QSTR(MP_QSTR_dealloc_extra_fb),    MP_ROM_PTR(&py_sensor_dealloc_extra_fb_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixformat),       MP_ROM_PTR(&py_sensor_set_pixformat_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_pixformat),       MP_ROM_PTR(&py_sensor_get_pixformat_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_framesize),       MP_ROM_PTR(&py_sensor_set_framesize_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_framesize),       MP_ROM_PTR(&py_sensor_get_framesize_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_framerate),       MP_ROM_PTR(&py_sensor_set_framerate_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_framerate),       MP_ROM_PTR(&py_sensor_get_framerate_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_windowing),       MP_ROM_PTR(&py_sensor_set_windowing_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_windowing),       MP_ROM_PTR(&py_sensor_get_windowing_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_gainceiling),     MP_ROM_PTR(&py_sensor_set_gainceiling_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_contrast),        MP_ROM_PTR(&py_sensor_set_contrast_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_brightness),      MP_ROM_PTR(&py_sensor_set_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_saturation),      MP_ROM_PTR(&py_sensor_set_saturation_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_quality),         MP_ROM_PTR(&py_sensor_set_quality_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_colorbar),        MP_ROM_PTR(&py_sensor_set_colorbar_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_auto_gain),       MP_ROM_PTR(&py_sensor_set_auto_gain_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_gain_db),         MP_ROM_PTR(&py_sensor_get_gain_db_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_auto_exposure),   MP_ROM_PTR(&py_sensor_set_auto_exposure_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_exposure_us),     MP_ROM_PTR(&py_sensor_get_exposure_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_auto_whitebal),   MP_ROM_PTR(&py_sensor_set_auto_whitebal_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_rgb_gain_db),     MP_ROM_PTR(&py_sensor_get_rgb_gain_db_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_auto_blc),        MP_ROM_PTR(&py_sensor_set_auto_blc_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_blc_regs),        MP_ROM_PTR(&py_sensor_get_blc_regs_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_hmirror),         MP_ROM_PTR(&py_sensor_set_hmirror_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_hmirror),         MP_ROM_PTR(&py_sensor_get_hmirror_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_vflip),           MP_ROM_PTR(&py_sensor_set_vflip_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_vflip),           MP_ROM_PTR(&py_sensor_get_vflip_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_transpose),       MP_ROM_PTR(&py_sensor_set_transpose_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_transpose),       MP_ROM_PTR(&py_sensor_get_transpose_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_auto_rotation),   MP_ROM_PTR(&py_sensor_set_auto_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_auto_rotation),   MP_ROM_PTR(&py_sensor_get_auto_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_framebuffers),    MP_ROM_PTR(&py_sensor_set_framebuffers_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_framebuffers),    MP_ROM_PTR(&py_sensor_get_framebuffers_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_delays),      MP_ROM_PTR(&py_sensor_disable_delays_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_full_flush),  MP_ROM_PTR(&py_sensor_disable_full_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_special_effect),  MP_ROM_PTR(&py_sensor_set_special_effect_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_lens_correction), MP_ROM_PTR(&py_sensor_set_lens_correction_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_vsync_callback),  MP_ROM_PTR(&py_sensor_set_vsync_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_frame_callback),  MP_ROM_PTR(&py_sensor_set_frame_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl),               MP_ROM_PTR(&py_sensor_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_color_palette),   MP_ROM_PTR(&py_sensor_set_color_palette_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_color_palette),   MP_ROM_PTR(&py_sensor_get_color_palette_obj) },
    { MP_ROM_QSTR(MP_QSTR___write_reg),         MP_ROM_PTR(&py_sensor_write_reg_obj) },
    { MP_ROM_QSTR(MP_QSTR___read_reg),          MP_ROM_PTR(&py_sensor_read_reg_obj) },
};
STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t sensor_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t) &globals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_sensor, sensor_module);
#endif // MICROPY_PY_SENSOR
