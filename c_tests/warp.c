#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

struct timeval print_time(){
    struct timeval now;
    gettimeofday(&now, NULL);
    printf("%lu.%lu\n", now.tv_sec, now.tv_usec);
    return now;
}

int main(int argc, char* argv[]){
    auto now = print_time();
    
    {
      const struct timezone tzp{-100, 0};
      settimeofday(&now, &tzp);
    }

    now = print_time();

    {
      const struct timezone tzp{100, 0};
      settimeofday(&now, &tzp);
    }
    
    print_time(); 
}
