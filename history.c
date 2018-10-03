/*
Arvin Aya-ay
history.c contains functions used for keeping track of command history.
Command history is maintained as a linked list.
*/
#include "history.h"

// insert a new history node
void insert(char *cmd){
  HistList *newHist = malloc(sizeof(HistList));
  newHist->data = malloc(128);
  strcpy(newHist->data,cmd);
  newHist->next = NULL;

  // add it to list
  if (head == NULL){
    head = newHist;
  }
  else{
    HistList *temp = head;
    while (temp->next != NULL){ // move to end
      temp = temp->next;
    }
    temp->next = newHist;
  }
}

// free all history nodes
void freeList(HistList *head){
  HistList *tmp;
  while (head != NULL){
    tmp = head;
    head = head->next;
    free(tmp->data);
    free(tmp);
  }
}
