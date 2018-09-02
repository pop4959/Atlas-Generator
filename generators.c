#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generators.h"
#include "json-builder.h"
#include "utils.h"
#include "xnb.h"
#include "offsets.h"
#include "json.h"

void generate_from_json_file(const char *file_name)
{
	char buffer[BUFFER_SIZE];
	FILE *file_config = fopen("config.txt", "r");
	int character = atoi(fgets(buffer, BUFFER_SIZE - 1, file_config));
	fclose(file_config);
	char *json;
	size_t json_length = read_file_to_string(&json, file_name);
	json_settings settings = {};
	settings.value_extra = json_builder_extra;
	char err[json_error_max];
	json_value *root = json_parse_ex(&settings, json, json_length, err);
	if (root)
	{
		json_object_entry first_element = root->u.object.values[0];
		if (strcmp(first_element.name, "frames") == 0)
		{
			generate_xnb_for_texturepacker(file_name, root, character);
		} else if (strcmp(first_element.name, "meta") == 0)
		{
			generate_xnb_for_spritesheetjs(file_name, root, character);
		}
	}
	json_value_free(root);
}

void generate_xnb_for_texturepacker(const char *file_name, json_value *root, int character)
{
	json_object_entry frame_object = root->u.object.values[0];
	json_value **frames = frame_object.value->u.array.values;
	unsigned int frames_length = frame_object.value->u.array.length;
	texture_atlas *atlas = xnb_atlas_init(frames_length);
	for (int i = 0; i < frames_length; ++i)
	{
		json_object_entry *frame_data = frames[i]->u.object.values;
		json_object_entry *frame = frame_data[1].value->u.object.values;
		json_object_entry *spriteSourceSize = frame_data[4].value->u.object.values;
		json_object_entry *sourceSize = frame_data[5].value->u.object.values;
		remove_extension(&frame_data[0].value->u.string.ptr, "png");
		xnb_atlas_add_region(atlas, frame_data[0].value->u.string.ptr, (int32_t) frame[0].value->u.integer,
							 (int32_t) frame[1].value->u.integer, (int32_t) frame[2].value->u.integer,
							 (int32_t) frame[3].value->u.integer,
							 spriteSourceSize[0].value->u.integer + character_offset_x(character),
							 spriteSourceSize[1].value->u.integer + character_offset_y(character),
							 spriteSourceSize[2].value->u.integer, spriteSourceSize[3].value->u.integer);
	}
	object *o = xnb_atlas_flatten_to_object(atlas);
	xnb *atlas_xnb = xnb_init(o);
	xnb_add_type_reader(atlas_xnb, READER_NAME_TEXTURE_ATLAS);
	xnb_add_type_reader(atlas_xnb, READER_NAME_TEXTURE_REGION);
	xnb_add_type_reader(atlas_xnb, READER_NAME_RECTANGLE);
	char xnb_extension[5] = ".xnb";
	char *xnb_file_name = malloc(strlen(file_name) + strlen(xnb_extension));
	strcpy(xnb_file_name, file_name);
	remove_extension(&xnb_file_name, "json");
	strcat(xnb_file_name, xnb_extension);
	xnb_write(atlas_xnb, xnb_file_name);
	free(xnb_file_name);
	free(atlas);
	free(o);
	xnb_free(atlas_xnb);
}

void generate_xnb_for_spritesheetjs(const char *file_name, json_value *root, int character)
{
	json_object_entry frame_object = root->u.object.values[1];
	json_object_entry *frames = frame_object.value->u.object.values;
	unsigned int frames_length = frame_object.value->u.object.length;
	texture_atlas *atlas = xnb_atlas_init(frames_length);
	for (int i = 0; i < frames_length; ++i)
	{
		json_object_entry *frame_data = frames[i].value->u.object.values;
		json_object_entry *frame = frame_data[0].value->u.object.values;
		json_object_entry *spriteSourceSize = frame_data[3].value->u.object.values;
		json_object_entry *sourceSize = frame_data[4].value->u.object.values;
		remove_extension(&frames[i].name, "png");
		xnb_atlas_add_region(atlas, frames[i].name, (int32_t) frame[0].value->u.integer,
							 (int32_t) frame[1].value->u.integer,
							 (int32_t) frame[2].value->u.integer, (int32_t) frame[3].value->u.integer,
							 spriteSourceSize[0].value->u.integer + character_offset_x(character),
							 spriteSourceSize[1].value->u.integer + character_offset_y(character),
							 sourceSize[0].value->u.integer, sourceSize[1].value->u.integer);
	}
	object *o = xnb_atlas_flatten_to_object(atlas);
	xnb *atlas_xnb = xnb_init(o);
	xnb_add_type_reader(atlas_xnb, READER_NAME_TEXTURE_ATLAS);
	xnb_add_type_reader(atlas_xnb, READER_NAME_TEXTURE_REGION);
	xnb_add_type_reader(atlas_xnb, READER_NAME_RECTANGLE);
	char xnb_extension[5] = ".xnb";
	char *xnb_file_name = malloc(strlen(file_name) + strlen(xnb_extension));
	strcpy(xnb_file_name, file_name);
	remove_extension(&xnb_file_name, "json");
	strcat(xnb_file_name, xnb_extension);
	xnb_write(atlas_xnb, xnb_file_name);
	free(xnb_file_name);
	free(atlas);
	free(o);
	xnb_free(atlas_xnb);
}

void generate_from_config_file()
{
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
	for (int i = 0; i < sprite_frame_count; ++i)
	{
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
}
