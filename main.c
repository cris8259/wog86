#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<elf.h>
#include"utils.h"
#include"loader.h"
int main(int argc, char **argv, char **envp)
{
    if (argc < 2)
        exit(1);
    char *filename = (char *)malloc(strlen(argv[1]));

    strcpy(filename, argv[1]);
    FILE *f;
    void *entry = NULL;
    int e_argc = argc - 1;
    char **e_argv = argv + 1;

    if (!is_exec(filename))
        exit(1);
    f = fopen(filename, "rb");
    if (!load_elf(f))
        exit(1);
    Elf32_Ehdr ehdr;

    fread(&ehdr, sizeof(Elf32_Ehdr), 1, f);
    entry = ehdr.e_entry;
}
