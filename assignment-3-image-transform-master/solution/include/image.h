#pragma once

#include <malloc.h>
#include <stdint.h>

struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct image {
    uint64_t width;
    uint64_t height;
    struct pixel *data;
};

struct image *allocate_image(uint64_t width, uint64_t height);

void destroy_image(struct image *image);
