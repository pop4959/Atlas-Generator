#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "xnb.h"

xnb *xnb_init(object *primary_asset_data)
{
	xnb *x = malloc(sizeof(xnb));
	strcpy(x->format_identifier, "XNB");
	x->target_platform = 'w';
	x->xnb_format_version = 5;
	x->flag_bits = 0;
	x->compressed_file_size = 0;
	x->decompressed_data_size = 0;
	x->type_reader_count = 0;
	x->type_readers = NULL;
	x->type_readers_end = NULL;
	x->shared_resource_count = 0;
	x->primary_asset_data = *primary_asset_data;
	x->shared_resource_data = NULL;
	x->shared_resource_data_end = NULL;
	return x;
}

void xnb_add_type_reader(xnb *x, char *reader_name)
{
	type_reader *tr = malloc(sizeof(uint7be_t) + sizeof(reader_name) + sizeof(int32_t) + sizeof(type_reader *));
	tr->type_reader_name_length = strlen(reader_name);
	tr->type_reader_name = reader_name;
	tr->reader_version_number = 0;
	tr->next = NULL;
	if (x->type_readers)
	{
		x->type_readers_end->next = tr;
		x->type_readers_end = tr;
	} else
	{
		x->type_readers = tr;
		x->type_readers_end = tr;
	}
	++x->type_reader_count;
}

void xnb_add_shared_resource_data(xnb *x, object *shared_resource)
{
	object *o = malloc(sizeof(uint7be_t) + sizeof(uint32_t) + sizeof(shared_resource->payload_size) + sizeof(object *));
	o->type_reader_id = shared_resource->type_reader_id;
	o->payload_size = shared_resource->payload_size;
	o->payload = shared_resource->payload;
	o->next = NULL;
	if (x->shared_resource_data)
	{
		x->shared_resource_data_end->next = o;
		x->shared_resource_data_end = o;
	} else
	{
		x->shared_resource_data = o;
		x->shared_resource_data_end = o;
	}
	++x->shared_resource_count;
}

void xnb_free(xnb *x)
{
	if (x->type_readers)
	{
		for (type_reader *tr = x->type_readers; tr != NULL;)
		{
			type_reader *current = tr;
			tr = tr->next;
			free(current);
		}
	}
	if (x->shared_resource_data)
	{
		for (object *o = x->shared_resource_data; o != NULL;)
		{
			object *current = o;
			o = o->next;
			free(current);
		}
	}
	free(x);
}

uint32_t xnb_file_size(xnb *x)
{
	size_t file_size = 0;
	file_size += sizeof(x->format_identifier);
	file_size += sizeof(x->target_platform);
	file_size += sizeof(x->xnb_format_version);
	file_size += sizeof(x->flag_bits);
	file_size += sizeof(x->compressed_file_size);
	if (xnb_is_compressed(x->flag_bits))
	{
		file_size += sizeof(x->decompressed_data_size);
	}
	byte *encoded_number;
	file_size += encode_7_bit_encoded_int(&encoded_number, x->type_reader_count);
	free(encoded_number);
	if (x->type_readers)
	{
		for (type_reader *tr = x->type_readers; tr != NULL; tr = tr->next)
		{
			file_size += encode_7_bit_encoded_int(&encoded_number, tr->type_reader_name_length);
			free(encoded_number);
			file_size += tr->type_reader_name_length;
			file_size += sizeof(tr->reader_version_number);
		}
	}
	file_size += encode_7_bit_encoded_int(&encoded_number, x->shared_resource_count);
	free(encoded_number);
	file_size += encode_7_bit_encoded_int(&encoded_number, x->primary_asset_data.type_reader_id);
	free(encoded_number);
	file_size += x->primary_asset_data.payload_size;
	if (x->shared_resource_data)
	{
		for (object *o = x->shared_resource_data; o != NULL; o = o->next)
		{
			file_size += encode_7_bit_encoded_int(&encoded_number, o->type_reader_id);
			free(encoded_number);
			file_size += o->payload_size;
		}
	}
	return file_size;
}

void xnb_write(xnb *x, const char *file_name)
{
	FILE *xnb_file = fopen(file_name, "wb");
	fwrite(x->format_identifier, sizeof(x->format_identifier), 1, xnb_file);
	fwrite(&x->target_platform, sizeof(x->target_platform), 1, xnb_file);
	fwrite(&x->xnb_format_version, sizeof(x->xnb_format_version), 1, xnb_file);
	fwrite(&x->flag_bits, sizeof(x->flag_bits), 1, xnb_file);
	uint32_t file_size = xnb_file_size(x);
	fwrite(&file_size, sizeof(file_size), 1, xnb_file);
	if (xnb_is_compressed(x->flag_bits))
	{
		fwrite(&x->decompressed_data_size, sizeof(x->decompressed_data_size), 1, xnb_file);
	}
	byte *encoded_type_reader_count;
	size_t encoded_type_reader_count_size = encode_7_bit_encoded_int(&encoded_type_reader_count, x->type_reader_count);
	fwrite(encoded_type_reader_count, encoded_type_reader_count_size, 1, xnb_file);
	free(encoded_type_reader_count);
	if (x->type_readers)
	{
		for (type_reader *tr = x->type_readers; tr != NULL; tr = tr->next)
		{
			byte *encoded_type_reader_name_length;
			size_t encoded_type_reader_name_length_size = encode_7_bit_encoded_int(&encoded_type_reader_name_length,
																				   tr->type_reader_name_length);
			fwrite(encoded_type_reader_name_length, encoded_type_reader_name_length_size, 1, xnb_file);
			free(encoded_type_reader_name_length);
			fwrite(tr->type_reader_name, tr->type_reader_name_length, 1, xnb_file);
			fwrite(&tr->reader_version_number, sizeof(tr->reader_version_number), 1, xnb_file);
		}
	}
	byte *encoded_shared_resource_count;
	size_t encoded_shared_resource_count_size = encode_7_bit_encoded_int(&encoded_shared_resource_count,
																		 x->shared_resource_count);
	fwrite(encoded_shared_resource_count, encoded_shared_resource_count_size, 1, xnb_file);
	free(encoded_shared_resource_count);
	byte *encoded_type_reader_id;
	size_t encoded_type_reader_id_size = encode_7_bit_encoded_int(&encoded_type_reader_id,
																  x->primary_asset_data.type_reader_id);
	fwrite(encoded_type_reader_id, encoded_type_reader_id_size, 1, xnb_file);
	free(encoded_type_reader_id);
	fwrite(x->primary_asset_data.payload, x->primary_asset_data.payload_size, 1, xnb_file);
	if (x->shared_resource_data)
	{
		for (object *o = x->shared_resource_data; o != NULL; o = o->next)
		{
			byte *encoded_object_type_reader_id;
			size_t encoded_object_type_reader_id_size = encode_7_bit_encoded_int(&encoded_object_type_reader_id,
																				 o->type_reader_id);
			fwrite(encoded_object_type_reader_id, encoded_object_type_reader_id_size, 1, xnb_file);
			free(encoded_object_type_reader_id);
			fwrite(o->payload, o->payload_size, 1, xnb_file);
		}
	}
	fclose(xnb_file);
}

int xnb_is_compressed(uint8_t flag_bits)
{
	return flag_bits & 0x80;
}

int xnb_is_hidef(uint8_t flag_bits)
{
	return flag_bits & 0x01;
}

size_t encode_7_bit_encoded_int(byte **buffer, uint7be_t number)
{
	size_t num_bytes = 1;
	if (number > 1)
	{
		num_bytes = (size_t) ceil((ceil(log2(number)) / 8));
	}
	byte *encoded = malloc(num_bytes);
	int32_t value = number;
	int i = 0;
	do
	{
		byte b = (byte) (value & 0x7F);
		value >>= 7;
		if (value != 0)
			b |= 0x80;
		encoded[i++] = b;
	} while (value != 0);
	*buffer = encoded;
	return num_bytes;
}

texture_atlas *xnb_atlas_init(unsigned int frame_count)
{
	texture_atlas *atlas = malloc(sizeof(texture_atlas));
	atlas->type_reader_id = READER_TYPE_TEXTURE_ATLAS;
	atlas->sprite_frame_count = frame_count;
	atlas->texture_regions = NULL;
	atlas->texture_regions_end = NULL;
	atlas->width = 0x706f705f;
	atlas->height = 0x39353934;
	return atlas;
}

void xnb_atlas_add_region(texture_atlas *ta, char *texture_region_name, int32_t x, int32_t y, int32_t width,
						  int32_t height, float offset_x, float offset_y, float size_x, float size_y)
{
	texture_region *tr = malloc(sizeof(texture_region));
	tr->type_reader_id = READER_TYPE_TEXTURE_REGION;
	tr->texture_region_name_length = strlen(texture_region_name);
	tr->texture_region_name = malloc(tr->texture_region_name_length);
	tr->texture_region_name = texture_region_name;
	tr->rect.type_reader_id = READER_TYPE_RECTANGLE;
	tr->rect.x = x;
	tr->rect.y = y;
	tr->rect.width = width;
	tr->rect.height = height;
	tr->offset_x = offset_x;
	tr->offset_y = offset_y;
	tr->size_x = size_x;
	tr->size_y = size_y;
	tr->next = NULL;
	if (ta->texture_regions)
	{
		ta->texture_regions_end->next = tr;
		ta->texture_regions_end = tr;
	} else
	{
		ta->texture_regions = tr;
		ta->texture_regions_end = tr;
	}
}

object *xnb_atlas_flatten_to_object(texture_atlas *ta)
{
	object *o = malloc(sizeof(object));
	o->type_reader_id = ta->type_reader_id;
	o->payload_size =
			sizeof(ta->type_reader_id) + sizeof(ta->sprite_frame_count) +
			ta->sprite_frame_count * sizeof(texture_region) + sizeof(ta->width) + sizeof(ta->height);
	o->payload = malloc(o->payload_size);
	memset(o->payload, 0, o->payload_size);
	size_t offset = 0;
	memcpy(o->payload + offset, &ta->sprite_frame_count, sizeof(ta->sprite_frame_count));
	offset += sizeof(ta->sprite_frame_count);
	for (texture_region *tr = ta->texture_regions; tr != NULL; tr = tr->next)
	{
		byte *encoded_type_reader_id;
		size_t encoded_type_reader_id_size = encode_7_bit_encoded_int(&encoded_type_reader_id, tr->type_reader_id);
		memcpy(o->payload + offset, encoded_type_reader_id, encoded_type_reader_id_size);
		offset += encoded_type_reader_id_size;
		free(encoded_type_reader_id);
		byte *encoded_texture_region_name_length;
		size_t encoded_texture_region_name_length_size = encode_7_bit_encoded_int(&encoded_texture_region_name_length,
																				  tr->texture_region_name_length);
		memcpy(o->payload + offset, encoded_texture_region_name_length, encoded_texture_region_name_length_size);
		offset += encoded_texture_region_name_length_size;
		free(encoded_texture_region_name_length);
		memcpy(o->payload + offset, tr->texture_region_name, tr->texture_region_name_length);
		offset += tr->texture_region_name_length;
		byte *encoded_type_reader_id_2;
		size_t encoded_type_reader_id_2_size = encode_7_bit_encoded_int(&encoded_type_reader_id_2,
																		tr->rect.type_reader_id);
		memcpy(o->payload + offset, encoded_type_reader_id_2, encoded_type_reader_id_2_size);
		offset += encoded_type_reader_id_2_size;
		free(encoded_type_reader_id_2);
		memcpy(o->payload + offset, &tr->rect.x, sizeof(tr->rect.x));
		offset += sizeof(tr->rect.x);
		memcpy(o->payload + offset, &tr->rect.y, sizeof(tr->rect.y));
		offset += sizeof(tr->rect.y);
		memcpy(o->payload + offset, &tr->rect.width, sizeof(tr->rect.width));
		offset += sizeof(tr->rect.width);
		memcpy(o->payload + offset, &tr->rect.height, sizeof(tr->rect.height));
		offset += sizeof(tr->rect.height);
		memcpy(o->payload + offset, &tr->offset_x, sizeof(tr->offset_x));
		offset += sizeof(tr->offset_x);
		memcpy(o->payload + offset, &tr->offset_y, sizeof(tr->offset_y));
		offset += sizeof(tr->offset_y);
		memcpy(o->payload + offset, &tr->size_x, sizeof(tr->size_x));
		offset += sizeof(tr->size_x);
		memcpy(o->payload + offset, &tr->size_y, sizeof(tr->size_y));
		offset += sizeof(tr->size_y);
	}
	memcpy(o->payload + offset, &ta->width, sizeof(ta->width));
	offset += sizeof(ta->width);
	memcpy(o->payload + offset, &ta->height, sizeof(ta->height));
	offset += sizeof(ta->height);
	o->payload_size = offset;
	o->next = NULL;
	return o;
}
