#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

void depth_search(char *direc_name, char *file, int depth)
{
	struct stat object;
	struct dirent *buf;
	DIR *direc = opendir(direc_name);
	char *path = (char *)malloc(sizeof(char));
	if (direc == NULL)
	{
		perror(NULL);
		exit(-1);
	}
	while ((buf = readdir(direc)) != NULL)
	{
		if (strcmp(buf->d_name, ".") != 0 && strcmp(buf->d_name, "..") != 0)
		{
			path = realloc(path, sizeof(char) * (strlen(direc_name) + strlen(buf->d_name) + 5));
			strcpy(path, direc_name);
			strcat(path, "/");
			strcat(path, buf->d_name);
			stat(path, &object);
			if (S_ISDIR(object.st_mode) && depth > 0)
				depth_search(path, file, depth - 1);
			else if (S_ISREG(object.st_mode) && strcmp(file, buf->d_name) == 0)
			{
				printf("Your file is found at %s\n", path);
				exit(0);
			}
		}
	}
	free(path);
	closedir(direc);
}

int main(int argc, char *argv[])
{
	int max_depth = atoi(argv[2]);
	depth_search((char *)argv[1], (char *)argv[3], max_depth);
	printf("Can't find the file :(\n");
	return 0;
}
