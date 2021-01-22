#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include<sys/mman.h>
#include<sys/user.h>
#include"loader.h"

int check_elf(FILE * f)
{
    Elf32_Ehdr header;

    fseek(f, 0, SEEK_SET);
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
        || (header.e_type != ET_EXEC && header.e_type != ET_DYN)
        || header.e_machine != EM_386)
        return 0;
    return 1;
}

int load_elf(FILE * f)
{
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdr;

    if (!check_elf(f))
        return 0;
    fseek(f, 0, SEEK_SET);
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
                ((void *)(phdr.p_vaddr & ~(PAGE_SIZE - 1)),
                 phdr.p_filesz + PAGE_SIZE - 1, prot,
                 MAP_PRIVATE | (ehdr.e_type == ET_EXEC ? MAP_FIXED : 0), fd,
                 phdr.p_offset - (phdr.p_vaddr & (PAGE_SIZE - 1))) ==
                MAP_FAILED)
                return 0;
            if (phdr.p_filesz < phdr.p_memsz)
            {
                if (mmap
                    ((void *)((phdr.p_vaddr + phdr.p_filesz + PAGE_SIZE - 1) &
                              ~(PAGE_SIZE - 1)), phdr.p_memsz - phdr.p_filesz,
                     prot, MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1,
                     0) == MAP_FAILED)
                    return 0;
            }
        }
    }
    return 1;
}
