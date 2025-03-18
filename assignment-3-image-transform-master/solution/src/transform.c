#include <string.h>

#include "transform.h"
#include "image.h"

static TRANSFORM_TYPE get_transform_type(const char *transform) {
    if (strcmp(transform, "none") == 0) return TRANSFORM_NONE;
    if (strcmp(transform, "cw90") == 0) return TRANSFORM_CW90;
    if (strcmp(transform, "ccw90") == 0) return TRANSFORM_CCW90;
    if (strcmp(transform, "fliph") == 0) return TRANSFORM_FLIPH;
    if (strcmp(transform, "flipv") == 0) return TRANSFORM_FLIPV;
    return TRANSFORM_UNKNOWN;
}

static TRANSFORMATION_CODE do_cw90(const struct image *src, struct image **res) {
    *res = allocate_image(src->height, src->width);
    if (*res == NULL) return TRANSFORM_MEMORY_LIMIT_EXCEED;
    struct pixel *src_data = src->data;
    struct pixel *res_data = (*res)->data;
    for (uint32_t y = 0; y < src->height; y++) {
        for (uint32_t x = 0; x < src->width; x++) {
            res_data[y + (src->width - x - 1) * src->height] = src_data[x + y * src->width];
        }
    }
    return TRANSFORM_CORRECT;
}

static TRANSFORMATION_CODE do_ccw90(const struct image *src, struct image **res) {
    *res = allocate_image(src->height, src->width);
    if (*res == NULL) return TRANSFORM_MEMORY_LIMIT_EXCEED;
    struct pixel *src_data = src->data;
    struct pixel *res_data = (*res)->data;
    for (uint32_t y = 0; y < src->height; y++) {
        for (uint32_t x = 0; x < src->width; x++) {
            res_data[(src->height - y - 1) + x * src->height] = src_data[x + y * src->width];
        }
    }
    return TRANSFORM_CORRECT;
}

static TRANSFORMATION_CODE do_fliph(const struct image *src, struct image **res) {
    *res = allocate_image(src->width, src->height);
    if (*res == NULL) return TRANSFORM_MEMORY_LIMIT_EXCEED;
    struct pixel *src_data = src->data;
    struct pixel *res_data = (*res)->data;
    for (uint32_t y = 0; y < src->height; y++) {
        for (uint32_t x = 0; x < src->width; x++) {
            res_data[(src->width - x - 1) + y * src->width] = src_data[x + y * src->width];
        }
    }
    return TRANSFORM_CORRECT;
}

static TRANSFORMATION_CODE do_flipv(const struct image *src, struct image **res) {
    *res = allocate_image(src->width, src->height);
    if (*res == NULL) return TRANSFORM_MEMORY_LIMIT_EXCEED;
    struct pixel *src_data = src->data;
    struct pixel *res_data = (*res)->data;
    for (uint32_t y = 0; y < src->height; y++) {
        for (uint32_t x = 0; x < src->width; x++) {
            res_data[x + (src->height - y - 1) * src->width] = src_data[x + y * src->width];
        }
    }
    return TRANSFORM_CORRECT;
}

TRANSFORMATION_CODE transform(struct image *src, struct image **res, const char *transform) {
    TRANSFORM_TYPE type = get_transform_type(transform);
    switch (type) {
        case TRANSFORM_NONE:
            *res = src;
            return TRANSFORM_CORRECT;
        case TRANSFORM_CW90:
            return do_cw90(src, res);
        case TRANSFORM_CCW90:
            return do_ccw90(src, res);
        case TRANSFORM_FLIPH:
            return do_fliph(src, res);
        case TRANSFORM_FLIPV:
            return do_flipv(src, res);
        default:
            return TRANSFORM_COMMAND_NOT_EXISTS;
    }
}
