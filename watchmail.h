#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct watchmailElement {
  char* filename;             ///< Path to the file being watched
  pthread_t thread;           ///< ::thread_t structure for ::watchmail_thread
  struct watchmailElement* next; ///< Pointer to next node
}watchmailElement;

watchmailElement *watchmailHead;

void control_watchmail(char *file, int start);

void* watchmail_thread(void *param);
