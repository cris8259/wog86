#include<stdio.h>
#include<sys/stat.h>
#include"loader.h"
int is_exec(const char *filename)
{
    struct stat buf;
    FILE *f;
    if (stat(filename, &buf) == -1)
        return 0;
    if (!S_ISREG(buf.st_mode) || (buf.st_mode & S_IXUSR) != S_IXUSR)
        return 0;
    if (!(f = fopen(filename, "rb")))
        return 0;
    if (!check_elf(f))
        return 0;
    return 1;
}
