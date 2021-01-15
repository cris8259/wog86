#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"utils.h"
#include"loader.h"
int main(int argc, char **argv, char **envp)
{
    if (argc < 2)
        exit(1);
    char *filename = (char *)malloc(strlen(argv[1]));
    strcpy(filename, argv[1]);
    int e_argc = argc - 1;
    char **e_argv = argv + 1;
    if (!is_exec(filename) || !check_elf(filename))
        exit(0);
}
