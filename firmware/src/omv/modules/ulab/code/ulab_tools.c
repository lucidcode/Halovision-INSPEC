/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2022 Zoltán Vörös
 */


#include <string.h>
#include "py/runtime.h"

#include "ulab.h"
#include "ndarray.h"
#include "ulab_tools.h"

// The following five functions return a float from a void type
// The value in question is supposed to be located at the head of the pointer

mp_float_t ndarray_get_float_uint8(void *data) {
    // Returns a float value from an uint8_t type
    return (mp_float_t)(*(uint8_t *)data);
}

mp_float_t ndarray_get_float_int8(void *data) {
    // Returns a float value from an int8_t type
    return (mp_float_t)(*(int8_t *)data);
}

mp_float_t ndarray_get_float_uint16(void *data) {
    // Returns a float value from an uint16_t type
    return (mp_float_t)(*(uint16_t *)data);
}

mp_float_t ndarray_get_float_int16(void *data) {
    // Returns a float value from an int16_t type
    return (mp_float_t)(*(int16_t *)data);
}


mp_float_t ndarray_get_float_float(void *data) {
    // Returns a float value from an mp_float_t type
    return *((mp_float_t *)data);
}

// returns a single function pointer, depending on the dtype
void *ndarray_get_float_function(uint8_t dtype) {
    if(dtype == NDARRAY_UINT8) {
        return ndarray_get_float_uint8;
    } else if(dtype == NDARRAY_INT8) {
        return ndarray_get_float_int8;
    } else if(dtype == NDARRAY_UINT16) {
        return ndarray_get_float_uint16;
    } else if(dtype == NDARRAY_INT16) {
        return ndarray_get_float_int16;
    } else {
        return ndarray_get_float_float;
    }
}

mp_float_t ndarray_get_float_index(void *data, uint8_t dtype, size_t index) {
    // returns a single float value from an array located at index
    if(dtype == NDARRAY_UINT8) {
        return (mp_float_t)((uint8_t *)data)[index];
    } else if(dtype == NDARRAY_INT8) {
        return (mp_float_t)((int8_t *)data)[index];
    } else if(dtype == NDARRAY_UINT16) {
        return (mp_float_t)((uint16_t *)data)[index];
    } else if(dtype == NDARRAY_INT16) {
        return (mp_float_t)((int16_t *)data)[index];
    } else {
        return (mp_float_t)((mp_float_t *)data)[index];
    }
}

mp_float_t ndarray_get_float_value(void *data, uint8_t dtype) {
    // Returns a float value from an arbitrary data type
    // The value in question is supposed to be located at the head of the pointer
    if(dtype == NDARRAY_UINT8) {
        return (mp_float_t)(*(uint8_t *)data);
    } else if(dtype == NDARRAY_INT8) {
        return (mp_float_t)(*(int8_t *)data);
    } else if(dtype == NDARRAY_UINT16) {
        return (mp_float_t)(*(uint16_t *)data);
    } else if(dtype == NDARRAY_INT16) {
        return (mp_float_t)(*(int16_t *)data);
    } else {
        return *((mp_float_t *)data);
    }
}

#if NDARRAY_BINARY_USES_FUN_POINTER | ULAB_NUMPY_HAS_WHERE
uint8_t ndarray_upcast_dtype(uint8_t ldtype, uint8_t rdtype) {
    // returns a single character that corresponds to the broadcasting rules
    // - if one of the operarands is a float, the result is always float
    // - operation on identical types preserves type
    //
    // uint8 + int8 => int16
    // uint8 + int16 => int16
    // uint8 + uint16 => uint16
    // int8 + int16 => int16
    // int8 + uint16 => uint16
    // uint16 + int16 => float

    if(ldtype == rdtype) {
        // if the two dtypes are equal, the result is also of that type
        return ldtype;
    } else if(((ldtype == NDARRAY_UINT8) && (rdtype == NDARRAY_INT8)) ||
            ((ldtype == NDARRAY_INT8) && (rdtype == NDARRAY_UINT8)) ||
            ((ldtype == NDARRAY_UINT8) && (rdtype == NDARRAY_INT16)) ||
            ((ldtype == NDARRAY_INT16) && (rdtype == NDARRAY_UINT8)) ||
            ((ldtype == NDARRAY_INT8) && (rdtype == NDARRAY_INT16)) ||
            ((ldtype == NDARRAY_INT16) && (rdtype == NDARRAY_INT8))) {
        return NDARRAY_INT16;
    } else if(((ldtype == NDARRAY_UINT8) && (rdtype == NDARRAY_UINT16)) ||
            ((ldtype == NDARRAY_UINT16) && (rdtype == NDARRAY_UINT8)) ||
            ((ldtype == NDARRAY_INT8) && (rdtype == NDARRAY_UINT16)) ||
            ((ldtype == NDARRAY_UINT16) && (rdtype == NDARRAY_INT8))) {
        return NDARRAY_UINT16;
    }
    return NDARRAY_FLOAT;
}

// The following five functions are the inverse of the ndarray_get_... functions,
// and write a floating point datum into a void pointer

void ndarray_set_float_uint8(void *data, mp_float_t datum) {
    *((uint8_t *)data) = (uint8_t)datum;
}

void ndarray_set_float_int8(void *data, mp_float_t datum) {
    *((int8_t *)data) = (int8_t)datum;
}

void ndarray_set_float_uint16(void *data, mp_float_t datum) {
    *((uint16_t *)data) = (uint16_t)datum;
}

void ndarray_set_float_int16(void *data, mp_float_t datum) {
    *((int16_t *)data) = (int16_t)datum;
}

void ndarray_set_float_float(void *data, mp_float_t datum) {
    *((mp_float_t *)data) = datum;
}

// returns a single function pointer, depending on the dtype
void *ndarray_set_float_function(uint8_t dtype) {
    if(dtype == NDARRAY_UINT8) {
        return ndarray_set_float_uint8;
    } else if(dtype == NDARRAY_INT8) {
        return ndarray_set_float_int8;
    } else if(dtype == NDARRAY_UINT16) {
        return ndarray_set_float_uint16;
    } else if(dtype == NDARRAY_INT16) {
        return ndarray_set_float_int16;
    } else {
        return ndarray_set_float_float;
    }
}
#endif /* NDARRAY_BINARY_USES_FUN_POINTER */

int8_t tools_get_axis(mp_obj_t axis, uint8_t ndim) {
    int8_t ax = mp_obj_get_int(axis);
    if(ax < 0) ax += ndim;
    if((ax < 0) || (ax > ndim - 1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("axis is out of bounds"));
    }
    return ax;
}

shape_strides tools_reduce_axes(ndarray_obj_t *ndarray, mp_obj_t axis) {
    // TODO: replace numerical_reduce_axes with this function, wherever applicable
    // This function should be used, whenever a tensor is contracted;
    // The shape and strides at `axis` are moved to the zeroth position,
    // everything else is aligned to the right
    if(!mp_obj_is_int(axis) & (axis != mp_const_none)) {
        mp_raise_TypeError(MP_ERROR_TEXT("axis must be None, or an integer"));
    }
    shape_strides _shape_strides;

    _shape_strides.increment = 0;
    // this is the contracted dimension (won't be overwritten for axis == None)
    _shape_strides.ndim = 0;

    if(axis == mp_const_none) {
        _shape_strides.shape = ndarray->shape;
        _shape_strides.strides = ndarray->strides;
        return _shape_strides;
    }

    size_t *shape = m_new(size_t, ULAB_MAX_DIMS + 1);
    _shape_strides.shape = shape;
    int32_t *strides = m_new(int32_t, ULAB_MAX_DIMS + 1);
    _shape_strides.strides = strides;

    memcpy(_shape_strides.shape, ndarray->shape, sizeof(size_t) * ULAB_MAX_DIMS);
    memcpy(_shape_strides.strides, ndarray->strides, sizeof(int32_t) * ULAB_MAX_DIMS);

    _shape_strides.axis = ULAB_MAX_DIMS - 1; // value of index for axis == mp_const_none (won't be overwritten)

    if(axis != mp_const_none) { // i.e., axis is an integer
        int8_t ax = tools_get_axis(axis, ndarray->ndim);
        _shape_strides.axis = ULAB_MAX_DIMS - ndarray->ndim + ax;
        _shape_strides.ndim = ndarray->ndim - 1;
    }

    // move the value stored at index to the leftmost position, and align everything else to the right
    _shape_strides.shape[0] = ndarray->shape[_shape_strides.axis];
    _shape_strides.strides[0] = ndarray->strides[_shape_strides.axis];
    for(uint8_t i = 0; i < _shape_strides.axis; i++) {
        // entries to the right of index must be shifted by one position to the left
        _shape_strides.shape[i + 1] = ndarray->shape[i];
        _shape_strides.strides[i + 1] = ndarray->strides[i];
    }

    if(_shape_strides.ndim != 0) {
        _shape_strides.increment = 1;
    }

    if(_shape_strides.ndim == 0) {
        _shape_strides.ndim = 1;
        _shape_strides.shape[ULAB_MAX_DIMS - 1] = 1;
        _shape_strides.strides[ULAB_MAX_DIMS - 1] = ndarray->itemsize;
    }

    return _shape_strides;
}

mp_obj_t ulab_tools_restore_dims(ndarray_obj_t *ndarray, ndarray_obj_t *results, mp_obj_t keepdims, shape_strides _shape_strides) {
    // restores the contracted dimension, if keepdims is True
    if((ndarray->ndim == 1) && (keepdims != mp_const_true)) {
        // since the original array has already been contracted and 
        // we don't want to keep the dimensions here, we have to return a scalar
        return mp_binary_get_val_array(results->dtype, results->array, 0);
    }

    if(keepdims == mp_const_true) {
        results->ndim += 1;
        for(int8_t i = 0; i < ULAB_MAX_DIMS; i++) {
            results->shape[i] = ndarray->shape[i];
        }
        results->shape[_shape_strides.axis] = 1;

        results->strides[ULAB_MAX_DIMS - 1] = ndarray->itemsize;
        for(uint8_t i = ULAB_MAX_DIMS; i > 1; i--) {
            results->strides[i - 2] = results->strides[i - 1] * results->shape[i - 1];
        }
    }

    return MP_OBJ_FROM_PTR(results);
}

#if ULAB_MAX_DIMS > 1
ndarray_obj_t *tools_object_is_square(mp_obj_t obj) {
    // Returns an ndarray, if the object is a square ndarray,
    // raises the appropriate exception otherwise
    if(!mp_obj_is_type(obj, &ulab_ndarray_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("size is defined for ndarrays only"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(obj);
    if((ndarray->shape[ULAB_MAX_DIMS - 1] != ndarray->shape[ULAB_MAX_DIMS - 2]) || (ndarray->ndim != 2)) {
        mp_raise_ValueError(MP_ERROR_TEXT("input must be square matrix"));
    }
    return ndarray;
}
#endif

uint8_t ulab_binary_get_size(uint8_t dtype) {
    #if ULAB_SUPPORTS_COMPLEX
    if(dtype == NDARRAY_COMPLEX) {
        return 2 * (uint8_t)sizeof(mp_float_t);
    }
    #endif
    return dtype == NDARRAY_BOOL ? 1 : mp_binary_get_size('@', dtype, NULL);
}

#if ULAB_SUPPORTS_COMPLEX
void ulab_rescale_float_strides(int32_t *strides) {
    // re-scale the strides, so that we can work with floats, when iterating
    uint8_t sz = sizeof(mp_float_t);
    for(uint8_t i = 0; i < ULAB_MAX_DIMS; i++) {
        strides[i] /= sz;
    }
}
#endif

bool ulab_tools_mp_obj_is_scalar(mp_obj_t obj) {
    #if ULAB_SUPPORTS_COMPLEX
    if(mp_obj_is_int(obj) || mp_obj_is_float(obj) || mp_obj_is_type(obj, &mp_type_complex)) {
        return true;
    } else {
        return false;
    }
    #else
    if(mp_obj_is_int(obj) || mp_obj_is_float(obj)) {
        return true;
    } else {
        return false;
    }
    #endif
}

ndarray_obj_t *ulab_tools_inspect_out(mp_obj_t out, uint8_t dtype, uint8_t ndim, size_t *shape, bool dense_only) {
    if(!mp_obj_is_type(out, &ulab_ndarray_type)) {
        mp_raise_TypeError(MP_ERROR_TEXT("out has wrong type"));
    }
    ndarray_obj_t *ndarray = MP_OBJ_TO_PTR(out);
    
    if(ndarray->dtype != dtype) {
        mp_raise_ValueError(MP_ERROR_TEXT("out array has wrong dtype"));
    }

    if(ndarray->ndim != ndim) {
        mp_raise_ValueError(MP_ERROR_TEXT("out array has wrong dimension"));
    }

    for(uint8_t i = 0; i < ULAB_MAX_DIMS; i++) {
        if(ndarray->shape[i] != shape[i]) {
            mp_raise_ValueError(MP_ERROR_TEXT("out array has wrong shape"));
        }
    }

    if(dense_only) {
        if(!ndarray_is_dense(ndarray)) {
            mp_raise_ValueError(MP_ERROR_TEXT("output array must be contiguous"));
        }
    }
    return ndarray;
}