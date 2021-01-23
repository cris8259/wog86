#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<elf.h>
#include<pthread.h>
#include<sys/mman.h>
#include"loader.h"

#define PAGESIZE 4096

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

unsigned int load_elf(FILE * f)
{
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdr;

    if (!check_elf(f))
        return 0;
    fseek(f, 0, SEEK_SET);
    int fd = fileno(f);
    unsigned int entry;

    if (fread(&ehdr, sizeof(Elf32_Ehdr), 1, f) != 1)
        return 0;
    if (fseek(f, ehdr.e_phoff, SEEK_SET) != 0)
        return 0;
    entry = ehdr.e_entry;
    void *map_addr = NULL;
    unsigned int addr;
    unsigned int len;
    char *interp = NULL;
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
        else if (phdr.p_type == PT_INTERP)
        {
            if (interp != NULL)
                return 0;
            interp = (char *)malloc(phdr.p_filesz);
            pread(fd, interp, phdr.p_filesz, phdr.p_offset);
            if (!load_interp(interp))
                return 0;
            free(interp);
        }
    }
    map_addr =
        mmap((void *)addr, (len + PAGESIZE - 1) & (~(PAGESIZE - 1)), PROT_NONE,
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
            map_addr =
                mmap((void *)((ehdr.e_type == ET_DYN ? addr : phdr.p_vaddr) & (~(PAGESIZE - 1))),
                     (phdr.p_filesz + PAGESIZE - 1) & (~(PAGESIZE - 1)), prot,
                     MAP_FIXED | MAP_PRIVATE, fd,
                     phdr.p_offset - (phdr.p_vaddr - (phdr.p_vaddr & (~(PAGESIZE - 1)))));
            if (map_addr == MAP_FAILED)
                return 0;
            addr += phdr.p_filesz + PAGESIZE - 1;
            addr &= ~(PAGESIZE - 1);
            if (phdr.p_filesz < phdr.p_memsz)
            {
                map_addr =
                    mmap((void
                          *)((ehdr.e_type ==
                              ET_DYN ? addr : (phdr.p_vaddr & (~(PAGESIZE - 1))) + phdr.p_filesz +
                              PAGESIZE - 1) & (~(PAGESIZE - 1))),
                         (phdr.p_memsz - phdr.p_filesz + PAGESIZE - 1) & (~(PAGESIZE - 1)), prot,
                         MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                addr += phdr.p_filesz + PAGESIZE - 1;
                addr &= ~(PAGESIZE - 1);
            }
        }
    }
    return entry;
}

int load_interp(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
        return 0;
    load_elf(f);
    fclose(f);
}
