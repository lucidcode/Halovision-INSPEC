/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2023 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2023 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * MJPEG Python module.
 */
#include "imlib_config.h"
#if defined(IMLIB_ENABLE_IMAGE_FILE_IO)

#include "py/obj.h"
#include "py/nlr.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "py_assert.h"
#include "py_helper.h"
#include "py_image.h"

#include "file_utils.h"
#include "framebuffer.h"
#include "omv_boardconfig.h"

static const mp_obj_type_t py_mjpeg_type;

typedef struct py_mjpeg_obj {
    mp_obj_base_t base;
    uint32_t frames;
    uint32_t bytes;
    uint32_t us_old;
    uint32_t us_avg;
    uint32_t width;
    uint32_t height;
    bool closed;
    FIL fp;
} py_mjpeg_obj_t;

STATIC void py_mjpeg_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "{\"closed\":%s, \"width\":%u, \"height\":%u, \"count\":%u, \"size\":%u}",
              self->closed ? "\"true\"" : "\"false\"",
              self->width,
              self->height,
              self->frames,
              f_size(&self->fp));
}

STATIC mp_obj_t py_mjpeg_is_closed(mp_obj_t self_in) {
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->closed);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_is_closed_obj, py_mjpeg_is_closed);

STATIC mp_obj_t py_mjpeg_width(mp_obj_t self_in) {
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->width);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_width_obj, py_mjpeg_width);

STATIC mp_obj_t py_mjpeg_height(mp_obj_t self_in) {
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->height);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_height_obj, py_mjpeg_height);

STATIC mp_obj_t py_mjpeg_count(mp_obj_t self_in) {
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->frames);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_count_obj, py_mjpeg_count);

STATIC mp_obj_t py_mjpeg_size(mp_obj_t self_in) {
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(f_size(&self->fp));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_size_obj, py_mjpeg_size);

STATIC mp_obj_t py_mjpeg_write(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_roi, ARG_channel, ARG_alpha, ARG_color_palette, ARG_alpha_palette, ARG_hint, ARG_quality };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_roi, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rgb_channel, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = -1 } },
        { MP_QSTR_alpha, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 256 } },
        { MP_QSTR_color_palette, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_alpha_palette, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_hint, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 0 } },
        { MP_QSTR_quality, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 90 } },
    };

    // Parse args.
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Sanity checks
    if (self->closed) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("MJPEG stream is closed"));
    }

    if (args[ARG_channel].u_int < -1 || args[ARG_channel].u_int > 2) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("RGB channel can be 0, 1, or 2"));
    }

    if (args[ARG_alpha].u_int < 0 || args[ARG_alpha].u_int > 256) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Alpha ranges between 0 and 256"));
    }

    if (args[ARG_quality].u_int < 0 || args[ARG_quality].u_int > 100) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Quality ranges between 0 and 100"));
    }

    image_t *image = py_helper_arg_to_image(pos_args[1], ARG_IMAGE_ANY);
    rectangle_t roi = py_helper_arg_to_roi(args[ARG_roi].u_obj, image);

    const uint16_t *color_palette = py_helper_arg_to_palette(args[ARG_color_palette].u_obj, PIXFORMAT_RGB565);
    const uint8_t *alpha_palette = py_helper_arg_to_palette(args[ARG_alpha_palette].u_obj, PIXFORMAT_GRAYSCALE);

    mjpeg_write(&self->fp, self->width, self->height, &self->frames, &self->bytes,
                image, args[ARG_quality].u_int, &roi, args[ARG_channel].u_int,
                args[ARG_alpha].u_int, color_palette, alpha_palette, args[ARG_hint].u_int);

    uint32_t ticks = mp_hal_ticks_us();

    if (self->frames > 1) {
        uint32_t ticks_diff = mp_hal_ticks_us() - self->us_old;

        if (self->frames <= 2) {
            self->us_avg = ticks_diff;
        } else {
            uint64_t cumulative_average_n = ((uint64_t) self->us_avg) * (self->frames - 1);
            self->us_avg = (cumulative_average_n + ticks_diff) / self->frames;
        }
    }

    self->us_old = ticks;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_mjpeg_write_obj, 2, py_mjpeg_write);

STATIC mp_obj_t py_mjpeg_sync(mp_obj_t self_in) {
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->closed) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("MJPEG stream is closed"));
    }
    mjpeg_sync(&self->fp, self->frames, self->bytes, self->us_avg);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_sync_obj, py_mjpeg_sync);

STATIC mp_obj_t py_mjpeg_close(mp_obj_t self_in) {
    py_mjpeg_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->closed) {
        mjpeg_close(&self->fp, self->frames, self->bytes, self->us_avg);
    }
    self->closed = true;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(py_mjpeg_close_obj, py_mjpeg_close);

STATIC mp_obj_t py_mjpeg_open(uint n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_width, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_INT,  {.u_int = -1 } },
        { MP_QSTR_height, MP_ARG_INT,  {.u_int = -1 } },
    };

    // Parse args.
    const char *path = mp_obj_str_get_str(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    py_mjpeg_obj_t *mjpeg = m_new_obj_with_finaliser(py_mjpeg_obj_t);
    memset(mjpeg, 0, sizeof(py_mjpeg_obj_t));
    mjpeg->base.type = &py_mjpeg_type;
    mjpeg->width = (args[ARG_width].u_int == -1) ? framebuffer_get_width() : args[ARG_width].u_int;
    mjpeg->height = (args[ARG_height].u_int == -1) ? framebuffer_get_height() : args[ARG_height].u_int;

    file_open(&mjpeg->fp, path, false, FA_WRITE | FA_CREATE_ALWAYS);
    mjpeg_open(&mjpeg->fp, mjpeg->width, mjpeg->height);
    return mjpeg;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_mjpeg_open_obj, 1, py_mjpeg_open);

STATIC const mp_rom_map_elem_t py_mjpeg_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_Mjpeg)          },
    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&py_mjpeg_close_obj)     },
    { MP_ROM_QSTR(MP_QSTR_is_closed),   MP_ROM_PTR(&py_mjpeg_is_closed_obj) },
    { MP_ROM_QSTR(MP_QSTR_width),       MP_ROM_PTR(&py_mjpeg_width_obj)     },
    { MP_ROM_QSTR(MP_QSTR_height),      MP_ROM_PTR(&py_mjpeg_height_obj)    },
    { MP_ROM_QSTR(MP_QSTR_count),       MP_ROM_PTR(&py_mjpeg_count_obj)     },
    { MP_ROM_QSTR(MP_QSTR_size),        MP_ROM_PTR(&py_mjpeg_size_obj)      },
    { MP_ROM_QSTR(MP_QSTR_add_frame),   MP_ROM_PTR(&py_mjpeg_write_obj)     },
    { MP_ROM_QSTR(MP_QSTR_write),       MP_ROM_PTR(&py_mjpeg_write_obj)     },
    { MP_ROM_QSTR(MP_QSTR_sync),        MP_ROM_PTR(&py_mjpeg_sync_obj)      },
    { MP_ROM_QSTR(MP_QSTR_close),       MP_ROM_PTR(&py_mjpeg_close_obj)     },
};

STATIC MP_DEFINE_CONST_DICT(py_mjpeg_locals_dict, py_mjpeg_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    py_mjpeg_type,
    MP_QSTR_Mjpeg,
    MP_TYPE_FLAG_NONE,
    print, py_mjpeg_print,
    locals_dict, &py_mjpeg_locals_dict
    );

STATIC const mp_rom_map_elem_t globals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_mjpeg)      },
    { MP_ROM_QSTR(MP_QSTR_Mjpeg),       MP_ROM_PTR(&py_mjpeg_open_obj)  },
};

STATIC MP_DEFINE_CONST_DICT(globals_dict, globals_dict_table);

const mp_obj_module_t mjpeg_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_t) &globals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_mjpeg, mjpeg_module);
#endif // IMLIB_ENABLE_IMAGE_FILE_IO
