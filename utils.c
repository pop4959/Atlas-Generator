#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int has_extension(const char *file_name, const char *extension)
{
	size_t file_name_len = strlen(file_name), extension_len = strlen(extension);
	if (file_name_len < extension_len)
		return 0;
	return (strcmp(&file_name[file_name_len - extension_len], extension) == 0);
}

int remove_extension(char **file_name, const char *extension)
{
	size_t file_name_len = strlen(*file_name), extension_len = strlen(extension);
	if (file_name_len - 1 <= extension_len)
		return 0;
	(*file_name)[file_name_len - extension_len - 1] = '\0';
	return 1;
}

size_t read_file_to_string(char **buffer, const char *file_name)
{
	size_t file_length = 0;
	FILE *f = fopen(file_name, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		file_length = (size_t) ftell(f);
		fseek(f, 0, SEEK_SET);
		*buffer = malloc(file_length + 1);
		if (*buffer)
		{
			fread(*buffer, 1, file_length, f);
			(*buffer)[file_length] = '\0';
		}
		fclose(f);
	}
	return file_length + 1;
}
