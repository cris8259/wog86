#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"utils.h"
#include"loader.h"
int
main (int argc, char **argv, char **envp)
{
  if (argc < 2)
    exit (1);
  char *filename = (char *) malloc (strlen (argv[1]));

  strcpy (filename, argv[1]);
  FILE *f = fopen (filename, "rb");
  int e_argc = argc - 1;
  char **e_argv = argv + 1;

  if (!is_exec (filename))
    exit (1);
  if (!load_elf (f))
    exit (1);
  while (1);
}
