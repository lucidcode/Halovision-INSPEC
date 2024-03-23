/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image math operations.
 */
#include "imlib.h"

#ifdef IMLIB_ENABLE_MATH_OPS
typedef struct imlib_replace_line_op_state {
    bool hmirror, vflip, transpose;
    image_t *mask;
} imlib_replace_line_op_state_t;

static void imlib_replace_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    bool hmirror = ((imlib_replace_line_op_state_t *) data)->hmirror;
    bool vflip = ((imlib_replace_line_op_state_t *) data)->vflip;
    bool transpose = ((imlib_replace_line_op_state_t *) data)->transpose;
    image_t *mask = ((imlib_replace_line_op_state_t *) data)->mask;

    image_t target;
    memcpy(&target, img, sizeof(image_t));

    if (transpose) {
        int w = target.w;
        int h = target.h;
        target.w = h;
        target.h = w;
    }

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            int v_line = vflip ? (img->h - line - 1) : line;
            for (int i = 0, j = img->w; i < j; i++) {
                int h_i = hmirror ? (img->w - i - 1) : i;

                if ((!mask) || image_get_mask_pixel(mask, h_i, v_line)) {
                    int pixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), h_i);
                    IMAGE_PUT_BINARY_PIXEL(&target, transpose ? v_line : i, transpose ? i : v_line, pixel);
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            int v_line = vflip ? (img->h - line - 1) : line;
            for (int i = 0, j = img->w; i < j; i++) {
                int h_i = hmirror ? (img->w - i - 1) : i;

                if ((!mask) || image_get_mask_pixel(mask, h_i, v_line)) {
                    int pixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), h_i);
                    IMAGE_PUT_GRAYSCALE_PIXEL(&target, transpose ? v_line : i, transpose ? i : v_line, pixel);
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            int v_line = vflip ? (img->h - line - 1) : line;
            for (int i = 0, j = img->w; i < j; i++) {
                int h_i = hmirror ? (img->w - i - 1) : i;

                if ((!mask) || image_get_mask_pixel(mask, h_i, v_line)) {
                    int pixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), h_i);
                    IMAGE_PUT_RGB565_PIXEL(&target, transpose ? v_line : i, transpose ? i : v_line, pixel);
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_replace(image_t *img,
                   const char *path,
                   image_t *other,
                   int scalar,
                   bool hmirror,
                   bool vflip,
                   bool transpose,
                   image_t *mask) {
    bool in_place = img->data == other->data;
    image_t temp;

    if (in_place) {
        memcpy(&temp, other, sizeof(image_t));
        temp.data = fb_alloc(image_size(&temp), FB_ALLOC_NO_HINT);
        memcpy(temp.data, other->data, image_size(&temp));
        other = &temp;
    }

    // To improve transpose performance we will split the operation up into chunks that fit in
    // onchip RAM. These chunks will then be copied to the target buffer in an efficent manner.
    if (path == NULL && other && transpose) {
        uint32_t size;
        void *data = fb_alloc_all(&size, FB_ALLOC_PREFER_SPEED);
        // line_num stores how many lines we can do at a time with on-chip RAM.
        int line_num = size / image_line_size(other);
        // Transposed chunks will be copied to the output image...
        uint8_t *img_data = img->data;
        int t_line_size = (image_line_size(img) * img->h) / img->w;
        // Work top to bottom transposing as many lines at a time in a chunk of the image.
        for (int i = 0, ii = other->h; i < ii; i += line_num) {
            line_num = IM_MIN(line_num, (ii - i));
            // Make an image that is a slice of the input image.
            image_t in = {.w = other->w, .h = line_num, .pixfmt = other->pixfmt};
            in.data = other->data + (image_line_size(other) * i);
            // Make an image that will hold the transposed output.
            image_t out = in;
            out.data = data;
            // Transpose the slice of the input image.
            imlib_replace_line_op_state_t state;
            state.hmirror = hmirror;
            state.vflip = vflip;
            state.mask = mask;
            state.transpose = true;
            imlib_image_operation(&out, NULL, &in, 0, imlib_replace_line_op, &state);
            out.w = line_num;
            out.h = other->w;
            // Copy lines of the chunk to the target image.
            int out_line_size = image_line_size(&out);
            for (int j = 0, jj = out.h; j < jj; j++) {
                memcpy(img_data + (t_line_size * j), out.data + (out_line_size * j), out_line_size);
            }
            // Slide the offset for the first line over by the size of the slice we transposed.
            img_data += out_line_size;
        }
        fb_free(); // fb_alloc_all
    } else {
        imlib_replace_line_op_state_t state;
        state.hmirror = hmirror;
        state.vflip = vflip;
        state.mask = mask;
        state.transpose = transpose;
        imlib_image_operation(img, path, other, scalar, imlib_replace_line_op, &state);
    }

    if (in_place) {
        fb_free();
    }

    if (transpose) {
        int w = img->w;
        int h = img->h;
        img->w = h;
        img->h = w;
    }
}

static void imlib_add_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            imlib_b_or_line_op(img, line, other, data, vflipped);
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = __UQADD8(p0, p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = __USAT(p0 + p1, 8);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = __USAT(p0 + p1, 8);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    uint32_t r = __USAT16(((p0 >> 11) & 0x1f001f) + ((p1 >> 11) & 0x1f001f), 5);
                    uint32_t g = __USAT16(((p0 >> 5) & 0x3f003f) + ((p1 >> 5) & 0x3f003f), 6);
                    uint32_t b = __USAT16((p0 & 0x1f001f) + (p1 & 0x1f001f), 5);
                    *((uint32_t *) (row0 + x)) = COLOR_R5_G6_B5_TO_RGB565(r, g, b);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t r = __USAT(COLOR_RGB565_TO_R5(p0) + COLOR_RGB565_TO_R5(p1), 5);
                    uint32_t g = __USAT(COLOR_RGB565_TO_G6(p0) + COLOR_RGB565_TO_G6(p1), 6);
                    uint32_t b = __USAT(COLOR_RGB565_TO_B5(p0) + COLOR_RGB565_TO_B5(p1), 5);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t r = __USAT(COLOR_RGB565_TO_R5(p0) + COLOR_RGB565_TO_R5(p1), 5);
                        uint32_t g = __USAT(COLOR_RGB565_TO_G6(p0) + COLOR_RGB565_TO_G6(p1), 6);
                        uint32_t b = __USAT(COLOR_RGB565_TO_B5(p0) + COLOR_RGB565_TO_B5(p1), 5);
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_add(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_add_line_op, mask);
}

static void imlib_sub_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            uint32_t *row1 = (uint32_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 32; x += 32) {
                    uint32_t p0 = row0[x / 32];
                    uint32_t p1 = row1[x / 32];
                    row0[x / 32] = p0 & (~p1); // the same as p0 > p1 for all bits
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                    uint32_t p = p0 > p1;
                    IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                        uint32_t p = p0 > p1;
                        IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = __UQSUB8(p0, p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = __USAT((int32_t) (p0 - p1), 8);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = __USAT((int32_t) (p0 - p1), 8);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    uint32_t r = __USAT16(__SSUB16((p0 >> 11) & 0x1f001f, (p1 >> 11) & 0x1f001f), 5);
                    uint32_t g = __USAT16(__SSUB16((p0 >> 5) & 0x3f003f, (p1 >> 5) & 0x3f003f), 6);
                    uint32_t b = __USAT16(__SSUB16(p0 & 0x1f001f, p1 & 0x1f001f), 5);
                    *((uint32_t *) (row0 + x)) = COLOR_R5_G6_B5_TO_RGB565(r, g, b);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t r = __USAT((int32_t) (COLOR_RGB565_TO_R5(p0) - COLOR_RGB565_TO_R5(p1)), 5);
                    uint32_t g = __USAT((int32_t) (COLOR_RGB565_TO_G6(p0) - COLOR_RGB565_TO_G6(p1)), 6);
                    uint32_t b = __USAT((int32_t) (COLOR_RGB565_TO_B5(p0) - COLOR_RGB565_TO_B5(p1)), 5);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t r = __USAT((int32_t) (COLOR_RGB565_TO_R5(p0) - COLOR_RGB565_TO_R5(p1)), 5);
                        uint32_t g = __USAT((int32_t) (COLOR_RGB565_TO_G6(p0) - COLOR_RGB565_TO_G6(p1)), 6);
                        uint32_t b = __USAT((int32_t) (COLOR_RGB565_TO_B5(p0) - COLOR_RGB565_TO_B5(p1)), 5);
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

static void imlib_rsub_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *row0 = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            uint32_t *row1 = (uint32_t *) other;

            if (!mask) {
                size_t x = 0;
                #if (__ARM_ARCH > 6)
                for (; (img->w - x) >= 32; x += 32) {
                    uint32_t p0 = row0[x / 32];
                    uint32_t p1 = row1[x / 32];
                    row0[x / 32] = p1 & (~p0); // the same as p1 > p0 for all bits
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                    uint32_t p = p1 > p0;
                    IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_BINARY_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_BINARY_PIXEL_FAST(row1, x);
                        uint32_t p = p1 > p0;
                        IMAGE_PUT_BINARY_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    *((uint32_t *) (row0 + x)) = __UQSUB8(p1, p0);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = __USAT((int32_t) (p1 - p0), 8);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = __USAT((int32_t) (p1 - p0), 8);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    uint32_t r = __USAT16(__SSUB16((p1 >> 11) & 0x1f001f, (p0 >> 11) & 0x1f001f), 5);
                    uint32_t g = __USAT16(__SSUB16((p1 >> 5) & 0x3f003f, (p0 >> 5) & 0x3f003f), 6);
                    uint32_t b = __USAT16(__SSUB16(p1 & 0x1f001f, p0 & 0x1f001f), 5);
                    *((uint32_t *) (row0 + x)) = COLOR_R5_G6_B5_TO_RGB565(r, g, b);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t r = __USAT((int32_t) (COLOR_RGB565_TO_R5(p1) - COLOR_RGB565_TO_R5(p0)), 5);
                    uint32_t g = __USAT((int32_t) (COLOR_RGB565_TO_G6(p1) - COLOR_RGB565_TO_G6(p0)), 6);
                    uint32_t b = __USAT((int32_t) (COLOR_RGB565_TO_B5(p1) - COLOR_RGB565_TO_B5(p0)), 5);
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t r = __USAT((int32_t) (COLOR_RGB565_TO_R5(p1) - COLOR_RGB565_TO_R5(p0)), 5);
                        uint32_t g = __USAT((int32_t) (COLOR_RGB565_TO_G6(p1) - COLOR_RGB565_TO_G6(p0)), 6);
                        uint32_t b = __USAT((int32_t) (COLOR_RGB565_TO_B5(p1) - COLOR_RGB565_TO_B5(p0)), 5);
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_sub(image_t *img, const char *path, image_t *other, int scalar, bool reverse, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, reverse ? imlib_rsub_line_op : imlib_sub_line_op, mask);
}

static void imlib_min_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            imlib_b_and_line_op(img, line, other, data, vflipped);
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    __USUB8(p0, p1);
                    *((uint32_t *) (row0 + x)) = __SEL(p1, p0);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = IM_MIN(p0, p1);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = IM_MIN(p0, p1);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    uint32_t p0_rb = p0 & 0xf81ff81f, p1_rb = p1 & 0xf81ff81f;
                    __USUB8(p0_rb, p1_rb);
                    uint32_t rb = __SEL(p1_rb, p0_rb);
                    uint32_t p0_g = p0 & 0x07e007e0, p1_g = p1 & 0x07e007e0;
                    __USUB16(p0_g, p1_g);
                    uint32_t g = __SEL(p1_g, p0_g);
                    *((uint32_t *) (row0 + x)) = rb | g;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t r = IM_MIN(COLOR_RGB565_TO_R5(p0), COLOR_RGB565_TO_R5(p1));
                    uint32_t g = IM_MIN(COLOR_RGB565_TO_G6(p0), COLOR_RGB565_TO_G6(p1));
                    uint32_t b = IM_MIN(COLOR_RGB565_TO_B5(p0), COLOR_RGB565_TO_B5(p1));
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t r = IM_MIN(COLOR_RGB565_TO_R5(p0), COLOR_RGB565_TO_R5(p1));
                        uint32_t g = IM_MIN(COLOR_RGB565_TO_G6(p0), COLOR_RGB565_TO_G6(p1));
                        uint32_t b = IM_MIN(COLOR_RGB565_TO_B5(p0), COLOR_RGB565_TO_B5(p1));
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_min(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_min_line_op, mask);
}

static void imlib_max_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            imlib_b_or_line_op(img, line, other, data, vflipped);
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    __USUB8(p0, p1);
                    *((uint32_t *) (row0 + x)) = __SEL(p0, p1);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = IM_MAX(p0, p1);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = IM_MAX(p0, p1);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    uint32_t p0_rb = p0 & 0xf81ff81f, p1_rb = p1 & 0xf81ff81f;
                    __USUB8(p0_rb, p1_rb);
                    uint32_t rb = __SEL(p0_rb, p1_rb);
                    uint32_t p0_g = p0 & 0x07e007e0, p1_g = p1 & 0x07e007e0;
                    __USUB16(p0_g, p1_g);
                    uint32_t g = __SEL(p0_g, p1_g);
                    *((uint32_t *) (row0 + x)) = rb | g;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t r = IM_MAX(COLOR_RGB565_TO_R5(p0), COLOR_RGB565_TO_R5(p1));
                    uint32_t g = IM_MAX(COLOR_RGB565_TO_G6(p0), COLOR_RGB565_TO_G6(p1));
                    uint32_t b = IM_MAX(COLOR_RGB565_TO_B5(p0), COLOR_RGB565_TO_B5(p1));
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t r = IM_MAX(COLOR_RGB565_TO_R5(p0), COLOR_RGB565_TO_R5(p1));
                        uint32_t g = IM_MAX(COLOR_RGB565_TO_G6(p0), COLOR_RGB565_TO_G6(p1));
                        uint32_t b = IM_MAX(COLOR_RGB565_TO_B5(p0), COLOR_RGB565_TO_B5(p1));
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_max(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_max_line_op, mask);
}

static void imlib_difference_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    image_t *mask = (image_t *) data;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            imlib_b_xor_line_op(img, line, other, data, vflipped);
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *row0 = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            uint8_t *row1 = (uint8_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 4; x += 4) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    uint32_t sub0 = __USUB8(p0, p1);
                    uint32_t sub1 = __USUB8(p1, p0);
                    *((uint32_t *) (row0 + x)) = __SEL(sub1, sub0);
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                    uint32_t p = __USAD8(p0, p1);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_GRAYSCALE_PIXEL_FAST(row1, x);
                        uint32_t p = __USAD8(p0, p1);
                        IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row0, x, p);
                    }
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *row0 = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            uint16_t *row1 = (uint16_t *) other;

            if (!mask) {
                size_t x = 0;
                #if defined(ARM_MATH_DSP)
                for (; (img->w - x) >= 2; x += 2) {
                    uint32_t p0 = *((uint32_t *) (row0 + x));
                    uint32_t p1 = *((uint32_t *) (row1 + x));
                    uint32_t p0_rb = p0 & 0xf81ff81f, p1_rb = p1 & 0xf81ff81f;
                    uint32_t sub0 = __USUB8(p0_rb, p1_rb);
                    uint32_t sub1 = __USUB8(p1_rb, p0_rb);
                    uint32_t rb = __SEL(sub1, sub0);
                    uint32_t p0_g = p0 & 0x07e007e0, p1_g = p1 & 0x07e007e0;
                    sub0 = __USUB16(p0_g, p1_g);
                    sub1 = __USUB16(p1_g, p0_g);
                    uint32_t g = __SEL(sub1, sub0);
                    *((uint32_t *) (row0 + x)) = rb | g;
                }
                #endif

                for (; x < img->w; x++) {
                    uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                    uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                    uint32_t r = __USAD8(COLOR_RGB565_TO_R5(p0), COLOR_RGB565_TO_R5(p1));
                    uint32_t g = __USAD8(COLOR_RGB565_TO_G6(p0), COLOR_RGB565_TO_G6(p1));
                    uint32_t b = __USAD8(COLOR_RGB565_TO_B5(p0), COLOR_RGB565_TO_B5(p1));
                    IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            } else {
                for (size_t x = 0; x < img->w; x++) {
                    if (image_get_mask_pixel(mask, x, line)) {
                        uint32_t p0 = IMAGE_GET_RGB565_PIXEL_FAST(row0, x);
                        uint32_t p1 = IMAGE_GET_RGB565_PIXEL_FAST(row1, x);
                        uint32_t r = __USAD8(COLOR_RGB565_TO_R5(p0), COLOR_RGB565_TO_R5(p1));
                        uint32_t g = __USAD8(COLOR_RGB565_TO_G6(p0), COLOR_RGB565_TO_G6(p1));
                        uint32_t b = __USAD8(COLOR_RGB565_TO_B5(p0), COLOR_RGB565_TO_B5(p1));
                        IMAGE_PUT_RGB565_PIXEL_FAST(row0, x, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_difference(image_t *img, const char *path, image_t *other, int scalar, image_t *mask) {
    imlib_image_operation(img, path, other, scalar, imlib_difference_line_op,  mask);
}

typedef struct imlib_blend_line_op_state {
    float alpha;
    image_t *mask;
} imlib_blend_line_op_t;

static void imlib_blend_line_op(image_t *img, int line, void *other, void *data, bool vflipped) {
    float alpha = ((imlib_blend_line_op_t *) data)->alpha, beta = 1 - alpha;
    image_t *mask = ((imlib_blend_line_op_t *) data)->mask;

    switch (img->pixfmt) {
        case PIXFORMAT_BINARY: {
            uint32_t *data = IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_BINARY_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_BINARY_PIXEL_FAST(((uint32_t *) other), i);
                    int p = (dataPixel * alpha) + (otherPixel * beta);
                    IMAGE_PUT_BINARY_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case PIXFORMAT_GRAYSCALE: {
            uint8_t *data = IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_GRAYSCALE_PIXEL_FAST(((uint8_t *) other), i);
                    int p = (dataPixel * alpha) + (otherPixel * beta);
                    IMAGE_PUT_GRAYSCALE_PIXEL_FAST(data, i, p);
                }
            }
            break;
        }
        case PIXFORMAT_RGB565: {
            uint16_t *data = IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(img, line);
            for (int i = 0, j = img->w; i < j; i++) {
                if ((!mask) || image_get_mask_pixel(mask, i, line)) {
                    int dataPixel = IMAGE_GET_RGB565_PIXEL_FAST(data, i);
                    int otherPixel = IMAGE_GET_RGB565_PIXEL_FAST(((uint16_t *) other), i);
                    int r = (COLOR_RGB565_TO_R5(dataPixel) * alpha) + (COLOR_RGB565_TO_R5(otherPixel) * beta);
                    int g = (COLOR_RGB565_TO_G6(dataPixel) * alpha) + (COLOR_RGB565_TO_G6(otherPixel) * beta);
                    int b = (COLOR_RGB565_TO_B5(dataPixel) * alpha) + (COLOR_RGB565_TO_B5(otherPixel) * beta);
                    IMAGE_PUT_RGB565_PIXEL_FAST(data, i, COLOR_R5_G6_B5_TO_RGB565(r, g, b));
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}

void imlib_blend(image_t *img, const char *path, image_t *other, int scalar, float alpha, image_t *mask) {
    imlib_blend_line_op_t state;
    state.alpha = alpha;
    state.mask = mask;
    imlib_image_operation(img, path, other, scalar, imlib_blend_line_op, &state);
}
#endif //IMLIB_ENABLE_MATH_OPS
