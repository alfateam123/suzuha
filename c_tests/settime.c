#include <stdio.h>
/*#include <stdlib.h>*/
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

int main(){
  //get the time now. the time has not been shim'd.
  struct timeval now;
  gettimeofday(&now, NULL);
  printf("%ld.%ld\n", now.tv_sec, now.tv_usec);

  //now, we set the shift. WE SHIM NOW!
  now.tv_sec -= 100;
  settimeofday(&now, NULL);

  //we'll read the time now. it should be 100 seconds in the past:
  //you can check it by comparing now.tv_sec and later_now.tv_sec.
  struct timeval later_now;
  gettimeofday(&later_now, NULL);
  printf("%ld.%ld\n", later_now.tv_sec, later_now.tv_usec);

  return 0;
}
