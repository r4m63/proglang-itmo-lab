#pragma once

#include "codes.h"
#include "image.h"

TRANSFORMATION_CODE transform(struct image *src, struct image **res, const char *transform);
