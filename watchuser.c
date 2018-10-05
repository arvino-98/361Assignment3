#include "watchuser.h"

void *watchuser_thread(void *param){
  while(1){
    pthread_mutex_lock(&lock);
    watchuserElement *temp = watchuserHead;
    while (temp != NULL){
      setutxent();
      while (up = getutxent())	/* get an entry */
      {
        if (up->ut_type == USER_PROCESS)	/* only care about users */
        {
          if (strcmp(up->ut_user, temp->username) == 0){
            printf("\n%s has logged on %s from %s\n", up->ut_user, up->ut_line, up ->ut_host);
          }
        }
      }
      temp = temp->next;
    }
    pthread_mutex_unlock(&lock);
    sleep(10);
  }
}
