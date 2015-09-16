#include <stdio.h>
/*#include <stdlib.h>*/
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(){
  struct timeval now;
  gettimeofday(&now, NULL);
  printf("%ld.%ld\n", now.tv_sec, now.tv_usec);

  now.tv_sec += 1000;
  struct timezone set_shift;
  set_shift.tz_dsttime = 0;
  set_shift.tz_minuteswest = 0; //10000000;
  int ret = settimeofday(&now, &set_shift);
  if(ret == -1) printf("%s\n", strerror(errno)); 

  printf("waiting...\n");
  sleep(2);

  {
    struct timeval now;
    gettimeofday(&now, NULL);
    printf("(after setting the time) %ld.%ld\n", now.tv_sec, now.tv_usec);
  }

  return 0;
}
