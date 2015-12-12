#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

const int shift = 1;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("You've forgotten the file you want to be shifted\n");
        exit(-1);
    }
    else if (argc > 2)
    {
        printf("Too many arguments\n");
        exit(-1);
    }

    struct stat file_info;
    char *ptr = NULL;
    int fd = open((char *)argv[1], O_RDWR, 0666);
    if (fd == -1)
    {
        perror(NULL);
        exit(-1);
    }

    stat((char *)argv[1], &file_info);
    size_t file_size = file_info.st_size;
    ptr = (char *)mmap(NULL, file_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Mapping failed\n");
        exit(-1);
    }
    close(fd);

    int i;
    for (i = 0; i < file_size; ++i)
    {
        ptr[i] += shift;
    }
    if (munmap((void *)ptr, file_size) == -1)
    {
        printf("Failed to unmap\n");
        exit(-1);
    }
    return 0;
}
