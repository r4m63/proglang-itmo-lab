#include <memory.h>

#include "bmp.h"

#define _BFTYPE 0x4D42
#define _BFRESERVED 0
#define _BOFFBITS 54
#define _BISIZE 40
#define _BIPLANES 1
#define _BIBITCOUNT 24
#define _BICOMPRESSION 0
#define _BIXPELSPERMETER 2835
#define _BIYPERLSPERMENETER 2835
#define _BICLRUSED 0
#define _BICLRIMPORTANT 0

IN_BMP_CODE from_bmp(FILE *in, struct image **img) {
    if (!in || !img)
        return READ_INVALID_ARGUMENT;

    struct bmp header;
    if (fread(&header, sizeof(header), 1, in) != 1 || !check_header(&header))
        return READ_INVALID_HEADER;

    uint32_t padding = calculate_padding(header.biWidth, header.biBitCount);
    *img = allocate_image(header.biWidth, header.biHeight);
    if (!*img)
        return READ_MEMORY_LIMIT_EXCEED;

    size_t row_size = header.biWidth * sizeof(struct pixel) + padding;
    uint8_t *row_buffer = malloc(row_size);
    if (!row_buffer) {
        destroy_image(*img);
        return READ_MEMORY_LIMIT_EXCEED;
    }

    struct pixel *ptr = (*img)->data;

    for (uint32_t i = 0; i < header.biHeight; i++) {
        if (fread(row_buffer, row_size, 1, in) != 1) {
            free(row_buffer);
            destroy_image(*img);
            return READ_IO_ERROR;
        }
        memcpy(ptr, row_buffer, header.biWidth * sizeof(struct pixel));
        ptr += header.biWidth;
    }

    free(row_buffer);
    return READ_CORRECT;
}


OUT_BMP_CODE to_bmp(FILE *out, const struct image *img) {
    if (!out || !img)
        return WRITE_INVALID_ARGUMENT;

    struct bmp header;
    create_header(img->width, img->height, &header);
    uint32_t padding = calculate_padding(header.biWidth, header.biBitCount);

    if (fwrite(&header, sizeof(struct bmp), 1, out) != 1)
        return WRITE_IO_ERROR;

    size_t row_size = img->width * sizeof(struct pixel) + padding;
    uint8_t *row_buffer = malloc(row_size);
    if (!row_buffer)
        return WRITE_MEMORY_ERROR;

    for (uint32_t i = 0; i < header.biHeight; i++) {
        memcpy(row_buffer, img->data + i * img->width, img->width * sizeof(struct pixel));
        if (padding > 0)
            memset(row_buffer + img->width * sizeof(struct pixel), 0, padding);
        if (fwrite(row_buffer, row_size, 1, out) != 1) {
            free(row_buffer);
            return WRITE_IO_ERROR;
        }
    }

    free(row_buffer);
    return WRITE_CORRECT;
}


bool check_header(const struct bmp *header) {
    if (!header) return false;
    return header->bfType == _BFTYPE &&
           header->bfReserved == _BFRESERVED &&
           header->bOffBits == _BOFFBITS &&
           header->biSize == _BISIZE &&
           header->biPlanes == _BIPLANES &&
           header->biBitCount == _BIBITCOUNT &&
           header->biCompression == _BICOMPRESSION &&
           header->biClrUsed == _BICLRUSED &&
           header->biClrImportant == _BICLRIMPORTANT;
}

void create_header(uint32_t width, uint32_t height, struct bmp *header) {
    uint32_t padding = calculate_padding(width, _BIBITCOUNT);
    uint32_t image_size = (width * _BIBITCOUNT / 8 + padding) * height;
    *header = (struct bmp) {
            .bfType = _BFTYPE,
            .bfileSize = _BOFFBITS + image_size,
            .bfReserved = _BFRESERVED,
            .bOffBits = _BOFFBITS,
            .biSize = _BISIZE,
            .biWidth = width,
            .biHeight = height,
            .biPlanes = _BIPLANES,
            .biBitCount = _BIBITCOUNT,
            .biCompression = _BICOMPRESSION,
            .biSizeImage = image_size,
            .biXPelsPerMeter = _BIXPELSPERMETER,
            .biYPelsPerMeter = _BIYPERLSPERMENETER,
            .biClrUsed = _BICLRUSED,
            .biClrImportant = _BICLRIMPORTANT
    };
}

uint32_t calculate_padding(uint32_t width, uint16_t bit_count) {
    return (4 - ((width * bit_count / 8) % 4)) % 4;
}
