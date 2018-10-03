/*
Arvin Aya-ay
builtins.h
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

void initPrevDirectory();
void freePrevDirectory();
void initEnvp(char **envp);
int isBuiltIn(char *command, char **args);
int builtInSize();
void getBuiltInPtr(char *command,char **args);

void bic_pwd();
void bic_cd(char **args);
void bic_history(char **args);
void bic_pid();
void bic_kill(char **args);
void bic_printenv(char **args);
void bic_setenv(char **args);
void prompt(char **args);
void bic_alias(char **args);
