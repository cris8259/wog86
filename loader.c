#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include<sys/mman.h>
#include"loader.h"

#define PAGESIZE 4096

int check_elf(FILE * f)
{
    Elf32_Ehdr header;
    if (fread(&header, sizeof(Elf32_Ehdr), 1, f) != 1)
        return 0;
    if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0
        || header.e_ident[EI_CLASS] != ELFCLASS32
        || header.e_ident[EI_DATA] != ELFDATA2LSB
        || header.e_ident[EI_VERSION] != EV_CURRENT
        || (header.e_ident[EI_OSABI] != ELFOSABI_LINUX
            && header.e_ident[EI_OSABI] != ELFOSABI_NONE
            && header.e_ident[EI_OSABI] != ELFOSABI_SYSV)
        || header.e_ehsize != sizeof(Elf32_Ehdr)
        || header.e_phentsize != sizeof(Elf32_Phdr)
        || (header.e_type != ET_EXEC && header.e_type != ET_DYN) || header.e_machine != EM_386)
        return 0;
    return 1;
}

int load_elf(FILE * f)
{
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdr;
    int fd = fileno(f);
    if (fread(&ehdr, sizeof(Elf32_Ehdr), 1, f) != 1)
        return 0;
    if (fseek(f, ehdr.e_phoff, SEEK_SET) != 0)
        return 0;
    int i;
    for (i = 0; i < ehdr.e_phnum; i++)
    {
        if (fread(&phdr, sizeof(Elf32_Phdr), 1, f) != 1)
        {
            return 0;
        }
        if (phdr.p_type == PT_LOAD)
        {
            int prot;
            if (phdr.p_flags & PF_R)
                prot = PROT_READ;
            if (phdr.p_flags & PF_W)
                if (!prot)
                    prot = PROT_WRITE;
                else
                    prot |= PROT_WRITE;
            if (phdr.p_flags & PF_X)
                if (!prot)
                    prot = PROT_EXEC;
                else
                    prot |= PROT_EXEC;
            if (mmap
                ((void *)(phdr.p_vaddr & ~(PAGESIZE - 1)),
                 phdr.p_filesz + phdr.p_vaddr & (PAGESIZE - 1), prot,
                 MAP_PRIVATE | MAP_DENYWRITE|(ehdr.e_type==ET_EXEC?MAP_FIXED:0), fd,
                 phdr.p_offset - (phdr.p_vaddr & (PAGESIZE - 1))) == MAP_FAILED)
                return 0;
        }
    }
    return 1;
}
