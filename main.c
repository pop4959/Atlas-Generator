#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 256

int main(int argc, char **argv) {
    char buffer[BUFFER_SIZE];
    FILE *file_config = fopen("config.txt", "r");
    char xnb_tag[] = {'X', 'N', 'B'};
    char platform = 'w';
    int8_t flag_bits = 5;
    int8_t xnb_format_version = 0;
    int32_t compressed_file_size = 0;
    int8_t block[] = {0x03, 0x74, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x41, 0x74, 0x6C, 0x61, 0x73, 0x43, 0x6F,
                      0x6E, 0x74, 0x65, 0x6E, 0x74, 0x2E, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x41, 0x74, 0x6C,
                      0x61, 0x73, 0x52, 0x65, 0x61, 0x64, 0x65, 0x72, 0x2C, 0x20, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72,
                      0x65, 0x41, 0x74, 0x6C, 0x61, 0x73, 0x43, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x50, 0x43, 0x2C,
                      0x20, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x3D, 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x30,
                      0x2C, 0x20, 0x43, 0x75, 0x6C, 0x74, 0x75, 0x72, 0x65, 0x3D, 0x6E, 0x65, 0x75, 0x74, 0x72, 0x61,
                      0x6C, 0x2C, 0x20, 0x50, 0x75, 0x62, 0x6C, 0x69, 0x63, 0x4B, 0x65, 0x79, 0x54, 0x6F, 0x6B, 0x65,
                      0x6E, 0x3D, 0x6E, 0x75, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x75, 0x54, 0x65, 0x78, 0x74, 0x75,
                      0x72, 0x65, 0x41, 0x74, 0x6C, 0x61, 0x73, 0x43, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x2E, 0x54,
                      0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x52, 0x65, 0x67, 0x69, 0x6F, 0x6E, 0x52, 0x65, 0x61, 0x64,
                      0x65, 0x72, 0x2C, 0x20, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x41, 0x74, 0x6C, 0x61, 0x73,
                      0x43, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x50, 0x43, 0x2C, 0x20, 0x56, 0x65, 0x72, 0x73, 0x69,
                      0x6F, 0x6E, 0x3D, 0x31, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x30, 0x2C, 0x20, 0x43, 0x75, 0x6C, 0x74,
                      0x75, 0x72, 0x65, 0x3D, 0x6E, 0x65, 0x75, 0x74, 0x72, 0x61, 0x6C, 0x2C, 0x20, 0x50, 0x75, 0x62,
                      0x6C, 0x69, 0x63, 0x4B, 0x65, 0x79, 0x54, 0x6F, 0x6B, 0x65, 0x6E, 0x3D, 0x6E, 0x75, 0x6C, 0x6C,
                      0x00, 0x00, 0x00, 0x00, 0x2F, 0x4D, 0x69, 0x63, 0x72, 0x6F, 0x73, 0x6F, 0x66, 0x74, 0x2E, 0x58,
                      0x6E, 0x61, 0x2E, 0x46, 0x72, 0x61, 0x6D, 0x65, 0x77, 0x6F, 0x72, 0x6B, 0x2E, 0x43, 0x6F, 0x6E,
                      0x74, 0x65, 0x6E, 0x74, 0x2E, 0x52, 0x65, 0x63, 0x74, 0x61, 0x6E, 0x67, 0x6C, 0x65, 0x52, 0x65,
                      0x61, 0x64, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    int32_t sprite_frame_count = atoi(fgets(buffer, BUFFER_SIZE - 1, file_config));
    int32_t max_width = atoi(fgets(buffer, BUFFER_SIZE - 1, file_config));
    int32_t max_height = atoi(fgets(buffer, BUFFER_SIZE - 1, file_config));
    int column_size = atoi(fgets(buffer, BUFFER_SIZE - 1, file_config));
    int shift_x = atoi(fgets(buffer, BUFFER_SIZE - 1, file_config));
    int shift_y = atoi(fgets(buffer, BUFFER_SIZE - 1, file_config));
    int8_t reader_type_1 = 2;
    int8_t reader_type_2 = 3;
    FILE *file_atlas = fopen("animation_atlas_variant00.xnb", "wb");
    fwrite(xnb_tag, sizeof(char), sizeof(xnb_tag), file_atlas);
    fputc(platform, file_atlas);
    fputc(flag_bits, file_atlas);
    fputc(xnb_format_version, file_atlas);
    fwrite(&compressed_file_size, sizeof(int32_t), 1, file_atlas);
    fwrite(block, sizeof(int8_t), sizeof(block), file_atlas);
    fwrite(&sprite_frame_count, sizeof(int32_t), 1, file_atlas);
    int32_t bounds_x, bounds_y, bounds_w, bounds_h;
    float origin_x, origin_y, orig_size_x, orig_size_y;
    for (int i = 0; i < sprite_frame_count; ++i) {
        fputc(reader_type_1, file_atlas);
        fgets(buffer, BUFFER_SIZE - 1, file_config);
        buffer[strcspn(buffer, "\r\n")] = 0;
        fputc(strlen(buffer), file_atlas);
        fwrite(buffer, sizeof(char), strlen(buffer), file_atlas);
        fputc(reader_type_2, file_atlas);
        bounds_x = max_width * (i / column_size);
        bounds_y = max_height * (i % column_size);
        bounds_w = max_width;
        bounds_h = max_height;
        origin_x = shift_x;
        origin_y = shift_y;
        orig_size_x = max_width;
        orig_size_y = max_height;
        fwrite(&bounds_x, sizeof(int32_t), 1, file_atlas);
        fwrite(&bounds_y, sizeof(int32_t), 1, file_atlas);
        fwrite(&bounds_w, sizeof(int32_t), 1, file_atlas);
        fwrite(&bounds_h, sizeof(int32_t), 1, file_atlas);
        fwrite(&origin_x, sizeof(float), 1, file_atlas);
        fwrite(&origin_y, sizeof(float), 1, file_atlas);
        fwrite(&orig_size_x, sizeof(float), 1, file_atlas);
        fwrite(&orig_size_y, sizeof(float), 1, file_atlas);
    }
    fwrite(&max_width, sizeof(int32_t), 1, file_atlas);
    fwrite(&max_height, sizeof(int32_t), 1, file_atlas);
    fclose(file_config);
    fclose(file_atlas);
    return 0;
}
