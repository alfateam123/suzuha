#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

void print_time(){
    struct timeval now;
    gettimeofday(&now, NULL);
    printf("%lu.%lu\n", now.tv_sec, now.tv_usec);
}

int main(int argc, char* argv[]){
    print_time();
    
    {
      const struct timezone tzp{-100, 0};
      settimeofday(NULL, &tzp);
    }

    print_time();

    {
      const struct timezone tzp{100, 0};
      settimeofday(NULL, &tzp);
    }
    
    print_time(); 
}
