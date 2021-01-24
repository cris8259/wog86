#include<stdio.h>
#include<stdlib.h>
#include"wsyscall.h"
#include"x86regs.h"
#include"x86exec.h"
int
x86_exec (x86regs x86regs)
{
  for (;;)
    {
      switch (*(uint8_t *) (R_EIP))
	{
	case 0xB8:
	  R_EAX = *(uint32_t *) (R_EIP + 1);
	  R_EIP += 5;
	  break;
	case 0xBB:
	  R_EBX = *(uint32_t *) (R_EIP + 1);
	  R_EIP += 5;
	  break;
	case 0xB9:
	  R_ECX = *(uint32_t *) (R_EIP + 1);
	  R_EIP += 5;
	  break;
	case 0xBA:
	  R_EDX = *(uint32_t *) (R_EIP + 1);
	  R_EIP += 5;
	  break;
	case 0xCD:
	  R_EIP += 1;
	  if (*(uint8_t *) (R_EIP) == 0x80)
	    {
	      R_EAX = w_syscall (R_EAX, R_EBX, R_ECX, R_EDX, R_EDI);
	      R_EIP += 1;
	    }
	  else
	    printf ("Unsupport INT %x.", *(uint8_t *) (R_EIP));
	  break;
	default:
	  printf ("Unsuppport opcode %x.", *(uint8_t *) R_EIP);
	  exit (-1);
	}
    }
}
