/*
Arvin Aya-ay
sh.c was the skeleton code that has been built upon.
It contains the main loop for our shell as well as a some
built-in command code
*/
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <glob.h>
#include "sh.h"
#include "builtins.h"
#include "history.h"
#include "alias.h"
#define BUFFERSIZE 128

int sh( int argc, char **argv, char **envp )
{
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  char **args = calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist;

  uid = getuid();
  password_entry = getpwuid(uid); /* get passwd info */
  homedir = password_entry->pw_dir; /* Home directory to start out with*/

  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  prompt[0] = ' '; prompt[1] = '\0';

  /* Put PATH into a linked list */
  pathlist = get_path();

  // initialize our variable in builtins.c keeping track of the previous directory
  initPrevDirectory();
  // initialize our variable in builtins.c keeping track of the environment
  initEnvp(envp);

  while ( go )
  {
    // redirect output back to screen in case it was changed
    redirectToScreen();
    /* print your prompt */
    char *cwd = getcwd(NULL, 0);
    printf("%s[%s]%s",prompt,cwd,">");
    free(cwd);
    // end printing prompt

    /* get command line and process */
    if (fgets(commandline, BUFFERSIZE, stdin) != NULL){
      commandline[strlen(commandline) - 1] = '\0';
      char* token = strtok(commandline," ");
      args[0] = token;
      command = args[0];
      int i = 1;
      while (token != NULL){
        token = strtok(NULL," ");
        args[i] = token;
        i++;
      }
    }
    else{
      printf("\n");
      command = NULL;
    }
    // end command line processing

    // if command not null figure out what to do with it
    if (command != NULL){
      // insert command into history list
      insert(command);

      // chcek for redirect
      int redirectAt = redirectPosition(args);
      if (args[redirectAt + 1] != NULL){
        checkRedirect(args[redirectAt], args[redirectAt + 1]);
        // set the argument where the redirect symbol is at to NULL
        args[redirectAt] = NULL;
      }

      // check if command is an alias
      if (aliasHead != NULL){
        AliasList *temp = aliasHead;
        // for each alias struct in the list
        while (temp != NULL){
          // if alias name matches command
          if (strcmp(temp->aliasName, args[0]) == 0){
            // set command first argument in aliasArgs (which is the corresponding command name)
            command = temp->aliasArgs[0];
            // and copy the values of aliasArgs into the local args array
            int i = 0;
            while (temp->aliasArgs[i] != NULL){
              args[i] = temp->aliasArgs[i];
              i++;
            }
          }
          temp = temp->next;
        }
      }
      // end checking alias
      // if an alias was found, the values of command and *args
      // have been modifies to match the alias

      //checkWildcard(args);

      /* now check for each built in command and implement */
      // if exit, free all allocated space
      if (strcmp(command, "exit") == 0){
        struct pathelement *tmp = pathlist;
        struct pathelement *prev = NULL;
        while (tmp != NULL){
          prev = tmp;
          tmp = tmp->next;
          free(prev);
        }
        free(pwd);
        free(prompt);
        free(owd);
        free(args);
        freePrevDirectory();
        freeList(head);
        freeAliasList(aliasHead);
        exit(0);
      }
      // where called seperately because it was defined in sh.c
      // according to skeleton code
      else if (strcmp(command, "where") == 0){
        printf("Executing built-in: where\n");
        where(args[1], pathlist);
      }
      // prompt called seperately because it must modify the prompt local variable
      // in this sh() function
      else if (strcmp(command, "prompt") == 0){
        printf("Executing built-in: prompt\n");
        if (args[1] == NULL){
          char newPrompt[32];
          printf("Enter new prompt prefix: ");
          fgets(newPrompt, BUFFERSIZE, stdin);
          newPrompt[strlen(newPrompt) - 1] = '\0';
          strcpy(prompt, newPrompt);
        }
        else {
          char *newPrompt = args[1];
          strcpy(prompt, newPrompt);
        }
      }
      // list called seperately because it was defined in sh.c
      // according to skeleton code
      else if (strcmp(command, "list") == 0){
        printf("Executing built-in: list\n");
        if (args[1] != NULL){
          int i = 1;
          while (args[i] != NULL){
            list(args[i]);
            i++;
          }
        }
        else {
          list(args[1]);
        }
      }

      // check if one of the other built-ins
      else if (isBuiltIn(command, args)){
        printf("Executing built-in: %s\n", command);
        getBuiltInPtr(command, args);
      }
      // end checking built-ins

      /*  else  program to exec */
      else{
        /* find it */
        // if a command starts with ./ or ../ or / check if it's an absolute path
        if ((command[0] == '.' && command[1] == '/') ||
            (command[0] == '/') ||
            (command[0] == '.' && command[1] == '.' && command[2] == '/'))
          {
          // ithat is executable and not a directory
          DIR *dirLoc = opendir(command);
          if ((access(command, X_OK) == 0) && (dirLoc == NULL)) {
            // if it is set it to commandpath
            commandpath = command;
          }
          // else set commandpath to null
          else {
            commandpath = NULL;
          }
          free(dirLoc);
        }
        // else if it is not, we check if it is a command somewhere that can we find
        // with which()
        else if (command[0] != '.' && command[0] != '/'){
          commandpath = which(command, pathlist);
        }
        /* do fork(), execve() and waitpid() */
        int ampersandAt = -1;
        // if there is no ampersand, execute normally
        if (commandpath != NULL && endsInAmpersand(args) == 0){
          printf("Executing: %s\n", command);
          if ((pid = fork()) < 0) {
            perror("Fork error");
  		    }
          /* child */
          else if (pid == 0) {
            execv(commandpath, args);
            perror("Couldn't execute");
            exit(127);
          }
          /* parent */
  		    if ((pid = waitpid(pid, &status, 0)) < 0){
            perror("Wait error");
          }
          free(commandpath);
        }
        // if there is an ampersand, execute in background
        else if (commandpath != NULL && (ampersandAt=endsInAmpersand(args) != 0)){
          printf("ends in &\n");
          args[ampersandAt] = NULL;
          printf("Executing: %s\n", command);
          if ((pid = fork()) < 0) {
            perror("Fork error");
  		    }
          /* child */
          else if (pid == 0) {
            execv(commandpath, args);
            perror("Couldn't execute");
            kill(pid, SIGCHLD);
            exit(127);
          }
          /* parent */
  		    signal(SIGCHLD, sigchld_handler);
        }
        else {
          fprintf(stderr, "%s: Command not found.\n", args[0]);
        }
      }
      // end finding program to exec
    }

    // if command was null just move back to beginning of loop
    else {
      continue;
    }
  }
  return 0;
} /* sh() */

// which returns path to a command if it exists, else null
char *which(char *command, struct pathelement *pathlist )
{
   /* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
   char *buffer = calloc(1, BUFFERSIZE);
   buffer[BUFFERSIZE - 1] = 0;
   while (pathlist) {
     sprintf(buffer,"%s/%s", pathlist->element, command);
     if (access(buffer, F_OK) == 0){
       return buffer;
     }
     pathlist = pathlist->next;
   }
   free(buffer);
   return NULL;
} /* which() */

// where prints all paths to a command that exist
char* where(char *command, struct pathelement *pathlist)
{
  /* similarly loop through finding all locations of command */
  char cmd[64];
  while (pathlist) {
    sprintf(cmd, "%s/%s", pathlist->element, command);
    if (access(cmd, F_OK) == 0){
      printf("%s/%s\n", pathlist->element, command);
    }
    pathlist = pathlist->next;
  }
} /* where() */

void list (char *dir)
{
  DIR *dirLoc;
  struct dirent *dp;
  if (dir != NULL){ // if called with argument
    dirLoc = opendir(dir); // open and set it as our dir
    printf("\n%s:\n", dir);
    if (!dirLoc){ // only if argument is a valid directory
      printf("Invalid directory\n");
      return;
    }
  }
  else{// else set dir to current working directory
    char *cwd = getcwd(NULL, 0);
    dirLoc = opendir(cwd);
    free(cwd);
  }
  // then print dir
  while ((dp=readdir(dirLoc)) != NULL){
    printf("%s\n", dp->d_name);
  }
  free(dirLoc);
} /* list() */

int endsInAmpersand(char **args){
  int i = 0;
  while (args[i + 1] != NULL){
    i++;
  }
  if (strcmp(args[i], "&") == 0){
    return i;
  }
  else {
    return 0;
  }
}

void sigchld_handler(int sig){
  signal(SIGCHLD, sigchld_handler);
  waitpid(-1, NULL, WNOHANG);
}

void redirectToScreen(){
  int fid = open("/dev/tty", O_WRONLY);
  close(1);
  close(2);
  dup(fid);
  dup(fid);
  close(fid);
}

int redirectPosition(char **args){
  int i = 0;
  while (args[i] != NULL){
    if (args[i][0] == '>' || args[i][0] == '<'){
      return i;
    }
    i++;
  }
}
void checkRedirect(char *redirectSymbol, char *filename){
  int fid;
  if (strcmp(redirectSymbol, ">") == 0){
    fid = open(filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    close(1);
    dup(fid);
    close(fid);
  }
  else if (strcmp(redirectSymbol, ">&") == 0){
    fid = open(filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    close(1);
    close(2);
    dup(fid);
    dup(fid);
    close(fid);
  }
  else if (strcmp(redirectSymbol, ">>") == 0){
    fid = open(filename, O_APPEND|O_WRONLY, S_IRWXU);
    close(1);
    dup(fid);
    close(fid);
  }
  else if (strcmp(redirectSymbol, ">>&") == 0){
    fid = open(filename, O_APPEND|O_WRONLY, S_IRWXU);
    close(1);
    close(2);
    dup(fid);
    dup(fid);
    close(fid);
  }
}
