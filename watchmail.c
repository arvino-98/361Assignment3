#include "watchmail.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
 #include <sys/time.h>

int control_watchmail(char *file, int disable){
  if (disable == 1){
    watchMailList *new_node = (watchMailList*)malloc(sizeof(watchMailList));
    new_node->filename = file;
    new_node->next = watchmails;
    watchmails = new_node;

    pthread_create(&(new_node->thread), NULL, watchmail_thread, (void*)(new_node->filename));
  }
  else {
    watchMailList *prev = NULL;
    watchMailList *temp = watchmails;

    while (temp != NULL){
      if (strcmp(temp->filename, file) == 1){
        break;
      }
      prev = temp;
      temp = temp->next;
    }
    if (temp == NULL){
      printf("No watchmail thread for %s exists!\n", file);
      return -1;
    }

     pthread_cancel(temp->thread);

     if(prev != NULL){
       prev->next = temp->next;
     }
     else {
       watchmails = temp->next;
     }

     free(temp->filename);
     free(temp);
   }
   return 0;
 }

void *watchmail_thread(void *param){
  char* filename = (char*)param;
  struct stat stat_info;
  off_t last_size;

  stat(filename, &stat_info);
  last_size = stat_info.st_size;

  while(1){
    stat(filename, &stat_info);

    if (stat_info.st_size > last_size){
      struct timeval tp;
      gettimeofday(&tp, NULL);
      printf("\n\aYou have new mail in %s at %s\n", filename, ctime(&(tp.tv_sec)));
    }
    last_size = stat_info.st_size;
  }
  return NULL;
}
