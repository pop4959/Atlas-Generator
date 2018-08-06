#ifndef ATLAS_GENERATOR_UTILS_H
#define ATLAS_GENERATOR_UTILS_H

/*
 * Check for a specific file extension.
 */
int has_extension(const char *file_name, const char *extension);

/*
 * Remove the file extension of a given file name.
 */
int remove_extension(char **file_name, const char *extension);

/*
 * Read a file into a string buffer.
 */
size_t read_file_to_string(char **buffer, const char *file_name);

#endif //ATLAS_GENERATOR_UTILS_H
