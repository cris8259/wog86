#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<elf.h>
#include"utils.h"
#include"loader.h"
#include"x86regs.h"
#include"x86exec.h"
int
main (int argc, char **argv, char **envp)
{
  if (argc < 2)
    exit (1);
  char *filename = (char *) malloc (strlen (argv[1]));

  strcpy (filename, argv[1]);
  FILE *f;
  int e_argc;
  char **e_argv;
  uint32_t entry;

  f=fopen(filename,"rb");
  if(!f)
  {
	  perror(filename);
	  exit(-1);
  }
  if (!is_exec (filename))
    {
      printf ("Not an executable.");
      exit (-1);
    }
  if (!(entry = load_elf (f)))
    exit (-1);
  fclose (f);
  Elf32_Ehdr ehdr;
  x86regs x86regs;

  e_argc = argc - 1;
  e_argv = argv + 1;

  R_EAX = e_argc;
  R_EBX = (uint32_t) e_argv;
  R_EIP = entry;
  x86_exec (x86regs);
}
