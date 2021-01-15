#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include"loader.h"
int check_elf(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;
    Elf32_Ehdr header;
    if (fread(&header, sizeof(Elf32_Ehdr), 1, f) != 1)
    {
        fclose(f);
        return 0;
    }
    fclose(f);
    if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0 || header.e_ident[EI_CLASS] != ELFCLASS32
        || header.e_ident[EI_DATA] != ELFDATA2LSB || header.e_ident[EI_VERSION] != EV_CURRENT
        || (header.e_ident[EI_OSABI] != ELFOSABI_LINUX && header.e_ident[EI_OSABI] != ELFOSABI_NONE
            && header.e_ident[EI_OSABI] != ELFOSABI_SYSV) || header.e_ehsize != sizeof(Elf32_Ehdr)
        || header.e_phentsize != sizeof(Elf32_Phdr) || (header.e_type != ET_EXEC
                                                      && header.e_type != ET_DYN)
        || header.e_machine != EM_386)
        return 0;
    return 1;
}
