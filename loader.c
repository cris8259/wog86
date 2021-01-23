#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include<sys/mman.h>
#include"loader.h"

#define PAGESIZE getpagesize()

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
        || (header.e_type != ET_EXEC && header.e_type != ET_DYN) || header.e_machine != EM_386)
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
    void *map_addr = NULL;
    unsigned int addr;
    unsigned int len;
    int i, j;
    for (i = 0, j = 0; i < ehdr.e_phnum; i++)
    {
        if (fread(&phdr, sizeof(Elf32_Phdr), 1, f) != 1)
        {
            return 0;
        }
        if (phdr.p_type == PT_LOAD)
        {
            if (j == 0)
            {
                addr = phdr.p_vaddr;
                len = phdr.p_vaddr + phdr.p_memsz - addr;
            }
            if (addr > phdr.p_vaddr)
                addr = phdr.p_vaddr;
            if (len < phdr.p_vaddr + phdr.p_memsz - addr)
                len = phdr.p_vaddr + phdr.p_memsz - addr;
            j++;
        }
    }
    map_addr =
        mmap((void *)addr, len + PAGESIZE - 1, PROT_NONE,
             MAP_PRIVATE | (ehdr.e_type == ET_EXEC ? MAP_FIXED : 0) | MAP_ANONYMOUS | MAP_NORESERVE,
             -1, 0);
    if (map_addr == MAP_FAILED)
        return 0;
    addr = (unsigned int)map_addr;
    fseek(f, ehdr.e_phoff, SEEK_SET);
    for (i = 0; i < ehdr.e_phnum; i++)
    {
        fread(&phdr, sizeof(Elf32_Phdr), 1, f);
        if (phdr.p_type == PT_LOAD)
        {
            int prot = 0;
            if (phdr.p_flags & PF_R)
                prot |= PROT_READ;
            if (phdr.p_flags & PF_W)
                prot |= PROT_WRITE;
            if (phdr.p_flags & PF_X)
                prot |= PROT_EXEC;
            mmap((void *)addr, phdr.p_filesz + PAGESIZE - 1, prot, MAP_FIXED | MAP_PRIVATE, fd,
                 phdr.p_offset);
            addr = addr & ~(PAGESIZE - 1);
            addr = (addr + phdr.p_filesz + PAGESIZE - 1) & ~(PAGESIZE - 1);
        }
    }
    return 1;
}
