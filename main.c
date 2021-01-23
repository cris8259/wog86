#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<elf.h>
#include"utils.h"
#include"loader.h"
#include"x86regs.h"
int main(int argc, char **argv, char **envp)
{
    if (argc < 2)
        exit(1);
    char *filename = (char *)malloc(strlen(argv[1]));

    strcpy(filename, argv[1]);
    FILE *f;
    int e_argc;
    char** e_argv;
    uint32_t entry;

    if (!is_exec(filename))
        exit(-1);
    f = fopen(filename, "rb");
    if (!load_elf(f))
        exit(-1);

    Elf32_Ehdr ehdr;
    x86regs regs;

    fread(&ehdr, sizeof(Elf32_Ehdr), 1, f);
    entry=ehdr.e_entry;
    e_argc=argc-1;
    e_argv=argv+1;

    regs.eax.dword[0]=e_argc;
    regs.ebx.dword[0]=(uint32_t)e_argv;
    regs.eip.dword[0]=entry;
}
