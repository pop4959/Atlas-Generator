#ifndef ATLAS_GENERATOR_GENERATORS_H
#define ATLAS_GENERATOR_GENERATORS_H

#include "json.h"

#define BUFFER_SIZE 256

void generate_from_json_file(const char *file_name);

void generate_xnb_for_texturepacker(const char *file_name, json_value *root);

void generate_xnb_for_spritesheetjs(const char *file_name, json_value *root);

void generate_from_config_file();

#endif //ATLAS_GENERATOR_GENERATORS_H
