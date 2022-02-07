
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *               2020 Scott Shawcroft for Adafruit Industries
 *               2020-2021 Zoltán Vörös
 *               2020 Taku Fukada
*/

#include <math.h>
#include <string.h>
#include "py/runtime.h"

#include "numpy.h"
#include "../ulab_create.h"
#include "approx/approx.h"
#include "compare/compare.h"
#include "fft/fft.h"
#include "filter/filter.h"
#include "linalg/linalg.h"
#include "numerical/numerical.h"
#include "poly/poly.h"
#include "vector/vector.h"

// math constants
#if ULAB_NUMPY_HAS_E
mp_obj_float_t ulab_const_float_e_obj = {{&mp_type_float}, MP_E};
#endif

#if ULAB_NUMPY_HAS_INF
mp_obj_float_t numpy_const_float_inf_obj = {{&mp_type_float}, (mp_float_t)INFINITY};
#endif

#if ULAB_NUMPY_HAS_NAN
mp_obj_float_t numpy_const_float_nan_obj = {{&mp_type_float}, (mp_float_t)NAN};
#endif

#if ULAB_NUMPY_HAS_PI
mp_obj_float_t ulab_const_float_pi_obj = {{&mp_type_float}, MP_PI};
#endif

static const mp_rom_map_elem_t ulab_numpy_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_numpy) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ndarray), (mp_obj_t)&ulab_ndarray_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_array), MP_ROM_PTR(&ndarray_array_constructor_obj) },
    #if ULAB_NUMPY_HAS_FROMBUFFER
        { MP_ROM_QSTR(MP_QSTR_frombuffer), MP_ROM_PTR(&create_frombuffer_obj) },
    #endif
    // math constants
    #if ULAB_NUMPY_HAS_E
        { MP_ROM_QSTR(MP_QSTR_e), MP_ROM_PTR(&ulab_const_float_e_obj) },
    #endif
    #if ULAB_NUMPY_HAS_INF
        { MP_ROM_QSTR(MP_QSTR_inf), MP_ROM_PTR(&numpy_const_float_inf_obj) },
    #endif
    #if ULAB_NUMPY_HAS_NAN
        { MP_ROM_QSTR(MP_QSTR_nan), MP_ROM_PTR(&numpy_const_float_nan_obj) },
    #endif
    #if ULAB_NUMPY_HAS_PI
        { MP_ROM_QSTR(MP_QSTR_pi), MP_ROM_PTR(&ulab_const_float_pi_obj) },
    #endif
    // class constants, always included
    { MP_ROM_QSTR(MP_QSTR_bool), MP_ROM_INT(NDARRAY_BOOL) },
    { MP_ROM_QSTR(MP_QSTR_uint8), MP_ROM_INT(NDARRAY_UINT8) },
    { MP_ROM_QSTR(MP_QSTR_int8), MP_ROM_INT(NDARRAY_INT8) },
    { MP_ROM_QSTR(MP_QSTR_uint16), MP_ROM_INT(NDARRAY_UINT16) },
    { MP_ROM_QSTR(MP_QSTR_int16), MP_ROM_INT(NDARRAY_INT16) },
    { MP_ROM_QSTR(MP_QSTR_float), MP_ROM_INT(NDARRAY_FLOAT) },
    // modules of numpy
    #if ULAB_NUMPY_HAS_FFT_MODULE
        { MP_ROM_QSTR(MP_QSTR_fft), MP_ROM_PTR(&ulab_fft_module) },
    #endif
    #if ULAB_NUMPY_HAS_LINALG_MODULE
        { MP_ROM_QSTR(MP_QSTR_linalg), MP_ROM_PTR(&ulab_linalg_module) },
    #endif
    #if ULAB_HAS_PRINTOPTIONS
        { MP_ROM_QSTR(MP_QSTR_set_printoptions), (mp_obj_t)&ndarray_set_printoptions_obj },
        { MP_ROM_QSTR(MP_QSTR_get_printoptions), (mp_obj_t)&ndarray_get_printoptions_obj },
    #endif
    #if ULAB_NUMPY_HAS_NDINFO
        { MP_ROM_QSTR(MP_QSTR_ndinfo), (mp_obj_t)&ndarray_info_obj },
    #endif
    #if ULAB_NUMPY_HAS_ARANGE
        { MP_ROM_QSTR(MP_QSTR_arange), (mp_obj_t)&create_arange_obj },
    #endif
    #if ULAB_NUMPY_HAS_CONCATENATE
        { MP_ROM_QSTR(MP_QSTR_concatenate), (mp_obj_t)&create_concatenate_obj },
    #endif
    #if ULAB_NUMPY_HAS_DIAG
        { MP_ROM_QSTR(MP_QSTR_diag), (mp_obj_t)&create_diag_obj },
    #endif
    #if ULAB_MAX_DIMS > 1
        #if ULAB_NUMPY_HAS_EYE
            { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t)&create_eye_obj },
        #endif
    #endif /* ULAB_MAX_DIMS */
    // functions of the approx sub-module
    #if ULAB_NUMPY_HAS_INTERP
        { MP_OBJ_NEW_QSTR(MP_QSTR_interp), (mp_obj_t)&approx_interp_obj },
    #endif
    #if ULAB_NUMPY_HAS_TRAPZ
        { MP_OBJ_NEW_QSTR(MP_QSTR_trapz), (mp_obj_t)&approx_trapz_obj },
    #endif
    // functions of the create sub-module
    #if ULAB_NUMPY_HAS_FULL
        { MP_ROM_QSTR(MP_QSTR_full), (mp_obj_t)&create_full_obj },
    #endif
    #if ULAB_NUMPY_HAS_LINSPACE
        { MP_ROM_QSTR(MP_QSTR_linspace), (mp_obj_t)&create_linspace_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOGSPACE
        { MP_ROM_QSTR(MP_QSTR_logspace), (mp_obj_t)&create_logspace_obj },
    #endif
    #if ULAB_NUMPY_HAS_ONES
        { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t)&create_ones_obj },
    #endif
    #if ULAB_NUMPY_HAS_ZEROS
        { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t)&create_zeros_obj },
    #endif
    // functions of the compare sub-module
    #if ULAB_NUMPY_HAS_CLIP
        { MP_OBJ_NEW_QSTR(MP_QSTR_clip), (mp_obj_t)&compare_clip_obj },
    #endif
    #if ULAB_NUMPY_HAS_EQUAL
        { MP_OBJ_NEW_QSTR(MP_QSTR_equal), (mp_obj_t)&compare_equal_obj },
    #endif
    #if ULAB_NUMPY_HAS_NOTEQUAL
        { MP_OBJ_NEW_QSTR(MP_QSTR_not_equal), (mp_obj_t)&compare_not_equal_obj },
    #endif
    #if ULAB_NUMPY_HAS_MAXIMUM
        { MP_OBJ_NEW_QSTR(MP_QSTR_maximum), (mp_obj_t)&compare_maximum_obj },
    #endif
    #if ULAB_NUMPY_HAS_MINIMUM
        { MP_OBJ_NEW_QSTR(MP_QSTR_minimum), (mp_obj_t)&compare_minimum_obj },
    #endif
    // functions of the filter sub-module
    #if ULAB_NUMPY_HAS_CONVOLVE
        { MP_OBJ_NEW_QSTR(MP_QSTR_convolve), (mp_obj_t)&filter_convolve_obj },
    #endif
    // functions of the numerical sub-module
    #if ULAB_NUMPY_HAS_ARGMINMAX
        { MP_OBJ_NEW_QSTR(MP_QSTR_argmax), (mp_obj_t)&numerical_argmax_obj },
        { MP_OBJ_NEW_QSTR(MP_QSTR_argmin), (mp_obj_t)&numerical_argmin_obj },
    #endif
    #if ULAB_NUMPY_HAS_ARGSORT
        { MP_OBJ_NEW_QSTR(MP_QSTR_argsort), (mp_obj_t)&numerical_argsort_obj },
    #endif
    #if ULAB_NUMPY_HAS_CROSS
        { MP_OBJ_NEW_QSTR(MP_QSTR_cross), (mp_obj_t)&numerical_cross_obj },
    #endif
    #if ULAB_NUMPY_HAS_DIFF
        { MP_OBJ_NEW_QSTR(MP_QSTR_diff), (mp_obj_t)&numerical_diff_obj },
    #endif
    #if ULAB_NUMPY_HAS_FLIP
        { MP_OBJ_NEW_QSTR(MP_QSTR_flip), (mp_obj_t)&numerical_flip_obj },
    #endif
    #if ULAB_NUMPY_HAS_MINMAX
        { MP_OBJ_NEW_QSTR(MP_QSTR_max), (mp_obj_t)&numerical_max_obj },
    #endif
    #if ULAB_NUMPY_HAS_MEAN
        { MP_OBJ_NEW_QSTR(MP_QSTR_mean), (mp_obj_t)&numerical_mean_obj },
    #endif
    #if ULAB_NUMPY_HAS_MEDIAN
        { MP_OBJ_NEW_QSTR(MP_QSTR_median), (mp_obj_t)&numerical_median_obj },
    #endif
    #if ULAB_NUMPY_HAS_MINMAX
        { MP_OBJ_NEW_QSTR(MP_QSTR_min), (mp_obj_t)&numerical_min_obj },
    #endif
    #if ULAB_NUMPY_HAS_ROLL
        { MP_OBJ_NEW_QSTR(MP_QSTR_roll), (mp_obj_t)&numerical_roll_obj },
    #endif
    #if ULAB_NUMPY_HAS_SORT
        { MP_OBJ_NEW_QSTR(MP_QSTR_sort), (mp_obj_t)&numerical_sort_obj },
    #endif
    #if ULAB_NUMPY_HAS_STD
        { MP_OBJ_NEW_QSTR(MP_QSTR_std), (mp_obj_t)&numerical_std_obj },
    #endif
    #if ULAB_NUMPY_HAS_SUM
        { MP_OBJ_NEW_QSTR(MP_QSTR_sum), (mp_obj_t)&numerical_sum_obj },
    #endif
    // functions of the poly sub-module
    #if ULAB_NUMPY_HAS_POLYFIT
        { MP_OBJ_NEW_QSTR(MP_QSTR_polyfit), (mp_obj_t)&poly_polyfit_obj },
    #endif
    #if ULAB_NUMPY_HAS_POLYVAL
        { MP_OBJ_NEW_QSTR(MP_QSTR_polyval), (mp_obj_t)&poly_polyval_obj },
    #endif
    // functions of the vector sub-module
    #if ULAB_NUMPY_HAS_ACOS
    { MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&vectorise_acos_obj },
    #endif
    #if ULAB_NUMPY_HAS_ACOSH
    { MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&vectorise_acosh_obj },
    #endif
    #if ULAB_NUMPY_HAS_ARCTAN2
    { MP_OBJ_NEW_QSTR(MP_QSTR_arctan2), (mp_obj_t)&vectorise_arctan2_obj },
    #endif
    #if ULAB_NUMPY_HAS_AROUND
    { MP_OBJ_NEW_QSTR(MP_QSTR_around), (mp_obj_t)&vectorise_around_obj },
    #endif
    #if ULAB_NUMPY_HAS_ASIN
    { MP_OBJ_NEW_QSTR(MP_QSTR_asin), (mp_obj_t)&vectorise_asin_obj },
    #endif
    #if ULAB_NUMPY_HAS_ASINH
    { MP_OBJ_NEW_QSTR(MP_QSTR_asinh), (mp_obj_t)&vectorise_asinh_obj },
    #endif
    #if ULAB_NUMPY_HAS_ATAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_atan), (mp_obj_t)&vectorise_atan_obj },
    #endif
    #if ULAB_NUMPY_HAS_ATANH
    { MP_OBJ_NEW_QSTR(MP_QSTR_atanh), (mp_obj_t)&vectorise_atanh_obj },
    #endif
    #if ULAB_NUMPY_HAS_CEIL
    { MP_OBJ_NEW_QSTR(MP_QSTR_ceil), (mp_obj_t)&vectorise_ceil_obj },
    #endif
    #if ULAB_NUMPY_HAS_COS
    { MP_OBJ_NEW_QSTR(MP_QSTR_cos), (mp_obj_t)&vectorise_cos_obj },
    #endif
    #if ULAB_NUMPY_HAS_COSH
    { MP_OBJ_NEW_QSTR(MP_QSTR_cosh), (mp_obj_t)&vectorise_cosh_obj },
    #endif
    #if ULAB_NUMPY_HAS_DEGREES
    { MP_OBJ_NEW_QSTR(MP_QSTR_degrees), (mp_obj_t)&vectorise_degrees_obj },
    #endif
    #if ULAB_NUMPY_HAS_EXP
    { MP_OBJ_NEW_QSTR(MP_QSTR_exp), (mp_obj_t)&vectorise_exp_obj },
    #endif
    #if ULAB_NUMPY_HAS_EXPM1
    { MP_OBJ_NEW_QSTR(MP_QSTR_expm1), (mp_obj_t)&vectorise_expm1_obj },
    #endif
    #if ULAB_NUMPY_HAS_FLOOR
    { MP_OBJ_NEW_QSTR(MP_QSTR_floor), (mp_obj_t)&vectorise_floor_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOG
    { MP_OBJ_NEW_QSTR(MP_QSTR_log), (mp_obj_t)&vectorise_log_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOG10
    { MP_OBJ_NEW_QSTR(MP_QSTR_log10), (mp_obj_t)&vectorise_log10_obj },
    #endif
    #if ULAB_NUMPY_HAS_LOG2
    { MP_OBJ_NEW_QSTR(MP_QSTR_log2), (mp_obj_t)&vectorise_log2_obj },
    #endif
    #if ULAB_NUMPY_HAS_RADIANS
    { MP_OBJ_NEW_QSTR(MP_QSTR_radians), (mp_obj_t)&vectorise_radians_obj },
    #endif
    #if ULAB_NUMPY_HAS_SIN
    { MP_OBJ_NEW_QSTR(MP_QSTR_sin), (mp_obj_t)&vectorise_sin_obj },
    #endif
    #if ULAB_NUMPY_HAS_SINH
    { MP_OBJ_NEW_QSTR(MP_QSTR_sinh), (mp_obj_t)&vectorise_sinh_obj },
    #endif
    #if ULAB_NUMPY_HAS_SQRT
    { MP_OBJ_NEW_QSTR(MP_QSTR_sqrt), (mp_obj_t)&vectorise_sqrt_obj },
    #endif
    #if ULAB_NUMPY_HAS_TAN
    { MP_OBJ_NEW_QSTR(MP_QSTR_tan), (mp_obj_t)&vectorise_tan_obj },
    #endif
    #if ULAB_NUMPY_HAS_TANH
    { MP_OBJ_NEW_QSTR(MP_QSTR_tanh), (mp_obj_t)&vectorise_tanh_obj },
    #endif
    #if ULAB_NUMPY_HAS_VECTORIZE
    { MP_OBJ_NEW_QSTR(MP_QSTR_vectorize), (mp_obj_t)&vectorise_vectorize_obj },
    #endif

};

static MP_DEFINE_CONST_DICT(mp_module_ulab_numpy_globals, ulab_numpy_globals_table);

mp_obj_module_t ulab_numpy_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_numpy_globals,
};
