#include<stdint.h>
typedef union
{
    uint32_t dword[1];
    uint16_t word[2];
    uint8_t byte[4];
} x86reg;
typedef struct
{
    x86reg eax;
    x86reg ecx;
    x86reg edx;
    x86reg ebx;
    x86reg esp;
    x86reg ebp;
    x86reg esi;
    x86reg edi;
    x86reg eip;
    x86reg cs;
    x86reg ds;
    x86reg ss;
    x86reg es;
    x86reg fs;
    x86reg gs;
} x86regs;
