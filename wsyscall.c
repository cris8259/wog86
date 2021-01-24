#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"wsyscall.h"
#include"x86regs.h"
int w_syscall (int arg1, int arg2, int arg3, int arg4, int arg5)
{
  switch (arg1)
    {
    case 4:
      return write (arg2, (void *) arg3, arg4);
      break;
    case 3:
      return read (arg2, (void *) arg3, arg4);
      break;
    case 1:
      exit (arg2);
      break;
    default:
      printf ("Unsupport syscall %x.", arg1);
    }
}
