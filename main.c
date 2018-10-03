/*
Arvin Ay-ay

This entire program is a simple shell with functions modeled after tcsh.
I was not able to get the * or ? wildcards working.

main.c calls sh.c to start the program.
Also contains signal handling code
*/
#include "sh.h"
#include "builtins.h"
#include <signal.h>
#include <stdio.h>

void sig_handler(int sig);

int main( int argc, char **argv, char **envp )
{
  /* put signal set up stuff here */
  signal(SIGINT, sig_handler);
  signal(SIGTSTP, sig_handler);
  signal(SIGTERM, sig_handler);

  return sh(argc, argv, envp);
}

void sig_handler(int sig)
{
  /* define your signal handler */
  if (sig == SIGINT){
    signal(SIGINT, sig_handler);
  }
  if (sig == SIGTSTP || sig == SIGTERM){
    signal(SIGTSTP, sig_handler);
    signal(SIGTERM, sig_handler);
  }
  char *cwd = getcwd(NULL, 0);
  printf("\n[%s]>", cwd);
  fflush(stdout);
  free(cwd);
}
