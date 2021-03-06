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
  struct timezone set_shift;
  set_shift.tz_dsttime = 0;
  set_shift.tz_minuteswest = 102; //102 seconds in the PAST.
  int ret = settimeofday(&now, &set_shift);
  if(ret == -1) printf("%s\n", strerror(errno)); 

  printf("waiting 2 seconds...\n");
  sleep(2);

  //we'll read the time now. it should be 100 seconds in the past:
  //you can check it by comparing now.tv_sec and later_now.tv_sec.
  struct timeval later_now;
  gettimeofday(&later_now, NULL);
  printf("(after setting the time) %ld.%ld\n", later_now.tv_sec, later_now.tv_usec);

  return 0;
}
