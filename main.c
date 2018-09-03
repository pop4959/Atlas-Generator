#include <string.h>
#include <dirent.h>
#include "generators.h"
#include "utils.h"

int main(int argc, char **argv)
{
	int from_config = 0;
	int custom_path = 0;
	int character = 34;
	char *target_path = NULL;

	int i = 0;
	for (char *arg = *argv; arg != NULL; arg = argv[++i])
	{
		if (strncmp(arg, "-", 1) == 0)
		{
			switch (arg[1])
			{
				case 'c':
					character = atoi(argv[i + 1]);
					break;
				case 'l':
					from_config = 1;
					break;
				case 'o':
					custom_path = 1;
					target_path = malloc(sizeof(char) * (strlen(argv[i + 1]) + 3));
					strncpy(target_path, "./", 2);
					strncpy(target_path + 2, argv[i + 1], strlen(argv[i + 1]));
					strncpy(target_path + 2 + strlen(argv[i + 1]), "\0", 1);
					break;
				default:
					break;
			}
		}
	}
	if (from_config)
	{
		generate_from_config_file();
	} else
	{
		DIR *dirp = opendir(custom_path ? target_path : ".");
		struct dirent *fp;
		while ((fp = readdir(dirp)) != NULL)
		{
			if (has_extension(fp->d_name, "json"))
			{
				char *buffer = malloc(strlen(target_path) + strlen(fp->d_name));
				strncpy(buffer, target_path, strlen(target_path));
				strncpy(buffer + strlen(target_path), fp->d_name, strlen(fp->d_name));
				strncpy(buffer + strlen(target_path) + strlen(fp->d_name), "\0", 1);
				generate_from_json_file(buffer, character);
				free(buffer);
			}
		}
		closedir(dirp);
	}
	free(target_path);
	return 0;
}
