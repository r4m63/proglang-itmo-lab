#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "codes.h"
#include "image.h"

struct __attribute__((packed)) bmp {
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};

IN_BMP_CODE from_bmp(FILE *in, struct image **img);

OUT_BMP_CODE to_bmp(FILE *out, struct image const *img);

bool check_header(const struct bmp *header);

void create_header(uint32_t width, uint32_t height, struct bmp *header);

uint32_t calculate_padding(uint32_t width, uint16_t bit_count);
