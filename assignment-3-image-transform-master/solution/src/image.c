#include "image.h"

struct image *allocate_image(uint64_t width, uint64_t height) {
    struct image *image = malloc(sizeof(struct image));
    if (image == NULL)
        return NULL;

    image->width = width;
    image->height = height;
    image->data = malloc(sizeof(struct pixel) * width * height);

    if (image->data == NULL) {
        free(image);
        return NULL;
    }

    return image;
}

void destroy_image(struct image *image) {
    if (image == NULL)
        return;
    free(image->data);
    free(image);
    image = NULL;
}

