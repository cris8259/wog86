#include<sys/stat.h>
#include"loader.h"
int is_exec(const char *filename)
{
    struct stat buf;
    if (stat(filename, &buf) == -1)
        return 0;
    if (!S_ISREG(buf.st_mode) || (buf.st_mode & S_IXUSR) != S_IXUSR)
        return 0;
    return 1;
}
