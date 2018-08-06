#include <string.h>
#include <dirent.h>
#include "generators.h"
#include "utils.h"

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "--old") == 0)
	{
		generate_from_config_file();
	} else
	{
		DIR *dirp = opendir(".");
		struct dirent *fp;
		while ((fp = readdir(dirp)) != NULL)
		{
			if (has_extension(fp->d_name, "json"))
			{
				generate_from_json_file(fp->d_name);
			}
		}
		closedir(dirp);
	}
	return 0;
}
