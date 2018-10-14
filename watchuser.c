#include "watchuser.h"

// thread that will monitor our user linked list
void *watchuser_thread(void *param){
  while(1){
    // we may make changes to the user list so be sure to lock
    pthread_mutex_lock(&lock);
    watchuserElement *temp = watchuserHead;
    while (temp != NULL){
      setutxent(); /* start at beginning of users*/
      while (up = getutxent())	/* get an entry */
      {
        if (up->ut_type == USER_PROCESS)	/* only care about users */
        {
          if (strcmp(up->ut_user, temp->username) == 0 && temp->watched == 0){
            printf("\n%s has logged on %s from %s\n", up->ut_user, up->ut_line, up ->ut_host);
            temp->watched = 1;
          }
        }
      }
      temp = temp->next;
    }
    pthread_mutex_unlock(&lock);
    sleep(10);
  }
}
