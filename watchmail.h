#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct watchmailElement {
  char* filename;             ///< Path to the file being watched
  pthread_t thread;           ///< ::thread_t structure for ::watchmail_thread
  struct watchmailElement* next; ///< Pointer to next node
}watchmailElement;

typedef struct watchmailElement watchMailList;

watchMailList *watchmails;

int control_watchmail();

void* watchmail_thread(void*);
