#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utmpx.h>
#include <pthread.h>
#include <unistd.h>

typedef struct watchuserElement {
  char* username;
  int watched;
  struct watchuserElement* next; ///< Pointer to next node
}watchuserElement;

int threadRunning;
watchuserElement *watchuserHead;

pthread_t watchuserThread;
struct utmpx *up;
pthread_mutex_t lock;

void *watchuser_thread(void *param);
