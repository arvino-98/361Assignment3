#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void sig_handler(int sig)
{
  printf("\n\nCHILD caught signal [%d]\n\n", sig);
  signal(SIGINT, sig_handler); // no need in Linux...
}

int main(int argc, char **argv)
{
  int i = 0;

  // signal(SIGINT, sig_handler);

  // printf("CHILD: pid [%d] group ID (%d)\n", getpid(), getpgrp());

  while (i < 20) {
    //printf("child is asleep...(%d)\n", i++);
    sleep(10);
    i++;
  }
  exit(0);
}
