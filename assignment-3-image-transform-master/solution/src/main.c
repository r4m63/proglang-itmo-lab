#include <stdio.h>

#include "bmp.h"
#include "codes.h"
#include "file.h"
#include "transform.h"

#define ARGS_COUNT 4

int main(int argc, char *argv[]) {

    if (argc != ARGS_COUNT) {
        return WRONG_ARGS_COUNT;
    }

    const char *input_file_name = argv[1];
    const char *output_file_name = argv[2];
    const char *command = argv[3];

    FILE *input_file = ((void *) 0);
    FILE_CODE input_file_res = open_file(input_file_name, READ_BINARY, &input_file);
    if (input_file_res != FILE_OPEN_CORRECT)
        return input_file_res;

    struct image *input_image = ((void *) 0);
    IN_BMP_CODE input_image_res = from_bmp(input_file, &input_image);
    if (input_image_res != READ_CORRECT)
        return input_image_res;

    FILE *output_file = ((void *) 0);
    FILE_CODE output_file_res = open_file(output_file_name, WRITE_BINARY, &output_file);
    if (output_file_res != FILE_OPEN_CORRECT)
        return output_file_res;

    struct image *output_image = ((void *) 0);
    TRANSFORMATION_CODE output_image_res = transform(input_image, &output_image, command);
    if (output_image_res != TRANSFORM_CORRECT)
        return output_image_res;

    OUT_BMP_CODE written_result = to_bmp(output_file, output_image);
    if (written_result != WRITE_CORRECT)
        return written_result;

    close_file(input_file);
    close_file(output_file);
    destroy_image(input_image);
    if (output_image != input_image)
        destroy_image(output_image);

    return 0;
}
