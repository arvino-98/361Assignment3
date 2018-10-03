/*
Arvin Aya-ay
alias.c contains code to maintain our alias list.
The list is a linked list of AliasList structs.
Each struct contains the alias's name and associated command and arguments

USAGE:
'alias aliasName commandName argument1 argument2 ...'
aliasName - name we want for the new alias
commandName - command the alias will be associated with
argument1 argument2 ... - arguments for the command

For example if we call 'alias listLong ls -l --author',
calling 'listLong" will execute 'ls -l --author'
*/
#include "alias.h"

// insert an alias into the list according to given **args
void insertAlias(char **args){
  AliasList *newAlias = malloc(sizeof(AliasList));

  newAlias->aliasName = malloc(48);
  // copy args[1] to aliasName
  // since alias should be called like "alias aliasName command arg1 arg2 .."
  // we want the second value
  strcpy(newAlias->aliasName,args[1]);

  newAlias->aliasArgs = calloc(48, sizeof(char*));
  int i = 0;
  while (args[i + 2] != NULL){
    newAlias->aliasArgs[i] = malloc(64);
    // copy each argument into aliasArgs
    // again, since alias should be called like "alias aliasName command arg1 arg2 .."
    // we want everything after the 2nd value.
    // so below we copy from args[i + 2]
    strcpy(newAlias->aliasArgs[i], args[i + 2]);
    i++;
  }

  newAlias->next = NULL;

  // add it to list
  if (aliasHead == NULL){
    aliasHead = newAlias;
  }
  else{
    AliasList *temp = aliasHead;
    while (temp->next != NULL){
      temp = temp->next;
    }
    temp->next = newAlias;
  }
}

// prints aliases in the firm "alias name: name | command: arg0 arg1 ..."
void printAllAlias(){
  AliasList *temp = aliasHead;
  while (temp != NULL){
    printf("alias name: %s | command: ", temp->aliasName);
    int j = 0;
    while (temp->aliasArgs[j] != NULL){
      printf("%s ",temp->aliasArgs[j]);
      j++;
    }
    printf("\n");
    temp = temp->next;
  }
}

// free all alias nodes
void freeAliasList(AliasList *head){
  AliasList *tmp;
  while (head != NULL){
    tmp = head;
    head = head->next;
    free(tmp->aliasName);
    int i = 0;
    while (tmp->aliasArgs[i] != NULL){
      free(tmp->aliasArgs[i]);
      i++;
    }
    free(tmp->aliasArgs);
    free(tmp);
  }
}
