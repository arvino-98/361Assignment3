/*
Arvin Ay-ay
builtins.c contains the built in commands that were
not already defined in sh.c
*/

#include "builtins.h"
#include "history.h"
#include "alias.h"
#include "sh.h"
#include "watchmail.h"
#include <signal.h>
int histToPrint = 10;
char* prevDirectory;
char **bic_envp;

// array of char* that keep track of names of our buil-in commands
// mapped 1 to 1 to function pointers in below array BUILT_IN_COMMANDS_PTR
const char* BUILT_IN_COMMANDS[] = {
  "pwd",
  "cd",
  "history",
  "pid",
  "kill",
  "printenv",
  "setenv",
  "alias",
  "watchuser",
  "watchmail"
};
// array of char* that keep track of function pointers of our buil-in commands
// mapped 1 to 1 to function pointers in above array BUILT_IN_COMMANDS
void (*BUILT_IN_COMMANDS_PTR[])(char** args) = {
  bic_pwd,
  bic_cd,
  bic_history,
  bic_pid,
  bic_kill,
  bic_printenv,
  bic_setenv,
  bic_alias,
  bic_watchuser,
  bic_watchmail
};

// initialize previous directory to the current directory
void initPrevDirectory(){
  prevDirectory = getcwd(NULL, 0);
}
void freePrevDirectory(){
  free(prevDirectory);
}
// initialize our local copy of the environment variable
void initEnvp(char **envp){
  bic_envp = envp;
}

// returns the size of our BUILT_IN_COMMANDS array
int builtInSize(){
  return sizeof(BUILT_IN_COMMANDS) / sizeof(char *);
}

// returns 1 if command is a built-in, else 0
int isBuiltIn(char *command, char **args){
  for (int i = 0; i < builtInSize(); i++){
    if (strcmp(command, BUILT_IN_COMMANDS[i]) == 0){
      return 1;
    }
  }
  return 0;
}

// iterates through the BUILT_IN_COMMANDS_PTR array a returns the proper function pointer
// according to the given command
void getBuiltInPtr(char *command, char **args){
  for (int i = 0; i < builtInSize(); i++){
    if (strcmp(command, BUILT_IN_COMMANDS[i]) == 0){
      (*BUILT_IN_COMMANDS_PTR[i])(args);
    }
  }
}

// prints the current working directory
void bic_pwd(){
  char *tmp;
  tmp = getcwd(NULL, 0);
  printf("%s\n", tmp);
  free(tmp);
}

// changes directory according to given argument
void bic_cd(char **args){
  char *prevDirectoryLoc = prevDirectory; // local copy of prevDirectory
  prevDirectory = getcwd(NULL, 0); // change global prevDirectory
  if (args[1] == NULL){ // if called with no arguments
    char *home = getenv("HOME");
    if (chdir(home) != 0){
      perror("Error moving to home directory");
    }
  }
  else if (args[1] != NULL && strcmp(args[1], "-") == 0){ // if called with 1 argument: "-"
    if (chdir(prevDirectoryLoc) != 0){
      perror("Not a valid directory");
    }
  }
  else if (args[1] != NULL){ // for any other argument given
    if (chdir(args[1]) != 0){
      perror("Not a valid directory");
    }
  }
  free(prevDirectoryLoc);
}

// prints history or changes amount to print based on given argument
void bic_history(char **args){
  if(args[1] == NULL){ // if called with no arguments, print history
    int i = 0;
    HistList *temp = head;
    while (temp != NULL && i < histToPrint){
      printf("%s\n", temp->data);
      temp = temp->next;
      i++;
    }
  }
  else if (args[1] != NULL){ // if called with 1 argument, change number to print
    int isDigit = 1;
    // if any character in given string is a digit set isDigit to false
    for (int i = 0; i < strlen(args[1]); i++){
      if (isdigit(args[1][i]) == 0){
        isDigit = 0;
      }
    }
    if (isDigit){
      histToPrint = atoi(args[1]);
    }
  }
}

// prints PID of shell
void bic_pid(){
  printf("PID: %d\n", getpid());
}

// kills the procss of a give pid
void bic_kill(char **args){
  // if called with no argument, return
  if (args[1] == NULL){
    return;
  }
  // else if called with a single argument that doesn't start
  // with '-', attempt to kill it
  else if (args[1][0] !=  '-') {
    if (kill(atoi(args[1]), SIGTERM) != 0){
      perror("Not a valid signal");
    }
  }
  // else if called with a two arguments and the first starts
  // with '-', attempt to send the specified signal
  else if (args[1][0] ==  '-' && args[2] != NULL){
    if (kill(atoi(args[2]), args[1][1]) != 0){
      perror("Not a valid signal");
    }
  }
  // else there were too many arguments
  else {
    printf("Invalid argument");
  }
}

// prints enviroment or single variable depending on argument
void bic_printenv(char **args){
  if(args[1] == NULL){ // if called with no arguments
    char **envpLoc = bic_envp; // local variable to hold environment list
    while(*envpLoc){ // move thru environment list and print each variable
      char *thisEnv = *envpLoc;
      printf("%s\n", thisEnv);
      envpLoc++;
    }
  }
  else if(args[1] != NULL){ //if called with 1 argument
    char *tmp;
    if ((tmp=getenv(args[1])) != NULL){ // set temp to the variable we want and print
      printf("%s\n", tmp);
    }
  }
  else if (args[2] != NULL){ // if called with >1 arguments
    printf("Too many arguments\n");
    return;
  }
}

// sets environment variables according to arguments
// if no argument given, prints entire environment
void bic_setenv(char **args){
  if (args[1] == NULL){ // if called with no arguments
    bic_printenv(args);
  }
  else if(args[3] != NULL){ // if called with >3 arguments
    printf("Too many arguments\n");
    return;
  }
  else if(args[2] != NULL){ // if called with 2 arguments
    if (setenv(args[1], args[2], 1) != 0){
      perror("Error setting variable and value");
    }
  }
  else if(args[1] != NULL){ // if called with 1 argument
    if (setenv(args[1], "", 0) != 0){
      perror("Error setting variable");
    }
  }
}

// prints known alias with no arguments, else inserts it into alias list
void bic_alias(char **args){
  if(args[1] == NULL){ // if called with no arguments, print history
    printAllAlias();
  }
  else{
    insertAlias(args);
  }
}

void bic_watchuser(char **args){
  struct utmpx *up;
  setutxent();			/* start at beginning */
  while (up = getutxent())	/* get an entry */
  {
    if ( up->ut_type == USER_PROCESS )	/* only care about users */
    {
      printf("%s has logged on %s from %s\n", up->ut_user, up->ut_line, up ->ut_host);
    }
  }
}

void bic_watchmail(char **args){
  int start;
  char *file;

  // if not enough or too many arguments
  if (args[1] == NULL || args[3] != NULL){
    printf("Invalid arguments.");
    return;
  }

  if (access(args[1], F_OK) == -1){
    return;
  }

  file = (char*)malloc(strlen(args[1]) + 1);
  strcpy(file, args[1]);

  // if called with just 1 argument, we can set start to true
  if (args[2] == NULL){
    start = 0;
  }
  // if called with more than one
  else if (args[2] != NULL){
    // we check if it specifies 'on' or 'off' and set start accordingly
    if(strcmp(args[2], "on") == 0){
      start = 0;
    }
    else if(strcmp(args[2], "off") == 0){
      start = 1;
    }
    else {
      fprintf(stderr, "Invalid command. Third arguments specifies 'on' or 'off'\n");
      return;
    }
  }

  // if start is true, we call control_watchmail to start a new thread
  if (start == 0){
    printf("Starting watchmail for %s\n", file);
    control_watchmail(file,start);
  }
  // if start is false, we call control_watchmail to stop a thread
  else if (start == 1){
    printf("Stopping watchmail for %s\n", file);
    control_watchmail(file,start);
  }



}
