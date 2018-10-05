#include "watchmail.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
 #include <sys/time.h>
  #include <unistd.h>

void control_watchmail(char *file, int start){
  // if disable is false we create a new watchmail and thread
  if (start == 0){
    watchMailList *new_node = (watchMailList*)malloc(sizeof(watchMailList));
    new_node->filename = file;
    new_node->next = watchmailHead;
    watchmailHead = new_node;

    pthread_create(&(new_node->thread), NULL, watchmail_thread, (void*)(new_node->filename));
  }
  // else disable is true so we move thru the list to find the node to disable
  else if (start == 1) {
    watchMailList *prev = NULL;
    watchMailList *temp = watchmailHead;
    // move thru list searching for node
    while (temp != NULL){
      if (strcmp(temp->filename, file) == 0){
        break;
      }
      prev = temp;
      temp = temp->next;
    }
    // if we get to the end of the list and still haven't found the node
    // inform the user and return
    if (temp == NULL){
      printf("No watchmail thread for %s exists!\n", file);
      return;
    }

    // if the node was found, we can cancel it's thread
     pthread_cancel(temp->thread);

     // we also need to remove the node from the list
     if(prev != NULL){
       // if node was not head
       prev->next = temp->next;
     }
     else {
       // if it was head
       watchmailHead = temp->next;
     }

     // free the node
     free(temp->filename);
     free(temp);
   }
 }

void *watchmail_thread(void *param){
  char* filename = (char*)param;
  // will hold our file's info
  struct stat stat_info;
  off_t last_size;

  // get file information and set last size to the file's size
  stat(filename, &stat_info);
  last_size = stat_info.st_size;

  // continuously monitor the file
  while(1){
    // update the file info
    stat(filename, &stat_info);

    // check if size ever becomes larger than previous
    if (stat_info.st_size > last_size){
      struct timeval tp;
      gettimeofday(&tp, NULL);
      printf("\n\aYou've Got Mail in [%s] at [%s]\n", filename, ctime(&(tp.tv_sec)));
    }
    // update last_size
    last_size = stat_info.st_size;
    sleep(1);
  }
  return NULL;
}
