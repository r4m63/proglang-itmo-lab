#pragma once

#include <stdio.h>

#include "codes.h"

#define READ_BINARY "rb"
#define WRITE_BINARY "wb"

FILE_CODE open_file(const char *file_name, const char *mode, FILE **file) {
    *file = fopen(file_name, mode);
    if (*file == NULL)
        return FILE_OPEN_ERROR;
    return FILE_OPEN_CORRECT;
}

void close_file(FILE *file) {
    if (file != NULL)
        fclose(file);
}
