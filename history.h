/*
Arvin Aya-ay
history.h
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HistList{
  char *data;
  struct HistList *next;
} HistList;
HistList *head;

void insert(char *cmd);
void freeList(HistList *head);
void printAllHistory();
