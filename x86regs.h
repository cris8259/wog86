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

#define R_EAX x86regs.eax.dword[0]
#define R_EBX x86regs.ebx.dword[0]
#define R_ECX x86regs.ecx.dword[0]
#define R_EDX x86regs.edx.dword[0]
#define R_EDI x86regs.edi.dword[0]
#define R_ESI x86regs.esi.dword[0]
#define R_ESP x86regs.esp.dword[0]
#define R_EBP x86regs.ebp.dword[0]
#define R_EIP x86regs.eip.dword[0]
#define R_AX x86regs.eax.word[0]
#define R_DX x86regs.edx.word[0]
#define R_AL x86regs.eax.byte[0]
#define R_AH x86regs.eax.byte[1]
#define R_CL x86regs.ecx.byte[0]
#define R_CS x86regs.cs.dword[0]
#define R_DS x86regs.ds.dword[0]
#define R_SS x86regs.ss.dword[0]
#define R_ES x86regs.es.dword[0]
#define R_FS x86regs.fs.dword[0]
#define R_GS x86regs.gs.dword[0]
