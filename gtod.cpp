//===-- gtod.cpp ----------------------------------------------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License.
//
//===----------------------------------------------------------------------===//
//
// gettimeofday() replacement
// Used for POC demonstrating invalid behavior due to timer wraparound.
//
//===----------------------------------------------------------------------===//

#include <assert.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

const int64_t USECS_IN_SEC = 1000000;
const int64_t USECS_IN_MSEC = 1000;

/*const*/ int64_t DELTA_BEFORE_BOUNDARY = 0; //10 * USECS_IN_SEC; // 10s

typedef int (*GTOD_t)(struct timeval *, struct timezone *);
typedef int (*STOD_t)(const struct timeval*, const struct timezone *);

// Get usec since epoch using actual gettimeofday().
uint64_t get_usec_since_epoch() {
  static GTOD_t gtod_orig = (GTOD_t) dlsym(RTLD_NEXT, "gettimeofday");

  assert(gtod_orig && "Failed to find original gettimeofday()!");
  assert(gtod_orig != gettimeofday);

  struct timeval tv;
  gtod_orig(&tv, NULL);

  uint64_t sec = tv.tv_sec;
  uint64_t usec = tv.tv_usec;
  uint64_t usec_since_epoch = (sec * USECS_IN_SEC) + usec;

  assert(((usec_since_epoch - usec) / USECS_IN_SEC) == sec);

  return usec_since_epoch;
}

int gettimeofday(struct timeval *tp, struct timezone *tzp) {
  assert(!tzp && "non-null timezone pointer? you're using gtod-shim, so a timezone would be ignored.");

#ifdef GTOD_SHIM_DEBUG
   printf("[GTOD_HOOK] DELTA_BEFORE_BOUNDARY=%ld\n", DELTA_BEFORE_BOUNDARY);
#endif

  uint64_t now = get_usec_since_epoch();

  //// Store time at first invocation of gtod
  //uint64_t usec_since_epoch_base = now;
  

  // Desired time is nearest 32bit boundary of msec
  // Adjust by least-significant 32bits in addition
  // to the padding defined by DELTA_BEFORE_BOUNDARY
  //uint64_t mask = (((uint64_t) 1) << 32) - 1;
  uint64_t adjust = DELTA_BEFORE_BOUNDARY * USECS_IN_SEC;
      //(msec_since_epoch_base & mask) * USECS_IN_MSEC + DELTA_BEFORE_BOUNDARY;

  // Compute adjusted time and populate timeval struct
  uint64_t now_adjust = now - adjust;
  tp->tv_sec = now_adjust / USECS_IN_SEC;
  tp->tv_usec = now_adjust % USECS_IN_SEC;

  // For debugging, log the time and delta to the 32bit msec boundary
#ifdef GTOD_SHIM_DEBUG
  if (true) {
    // Store time at first invocation of gtod
    uint64_t usec_since_epoch_base = now;

    int64_t usec_before_boundary =
        (int64_t)(now - usec_since_epoch_base) - DELTA_BEFORE_BOUNDARY;
    printf(
        "[GTOD_HOOK] delta to 32bit msec boundary=%jdms, reporting time as: %s",
        usec_before_boundary / USECS_IN_MSEC, ctime(&tp->tv_sec));
  }
#endif

  return 0;
}


int use_real_settimeofday(const struct timeval *tp,const struct timezone *ztp){
  static STOD_t stod_orig = (STOD_t) dlsym(RTLD_NEXT, "settimeofday");

  assert(stod_orig && "Failed to find original settimeofday()!");
  assert(stod_orig != settimeofday);

  return stod_orig(tp, ztp);
}

int settimeofday(const struct timeval *tp, const struct timezone *ztp){

#ifdef GTOD_SHIM_DEBUG
   printf("[STOD_HOOK] called custom settimeofday!\n");
#endif

  if(ztp != NULL){
    if(ztp->tz_dsttime == 0){
      DELTA_BEFORE_BOUNDARY += ztp->tz_minuteswest;
    }
  }

#ifdef GTOD_SHIM_DEBUG
   printf("[STOD_HOOK] DELTA_BEFORE_BOUNDARY=%ld\n", DELTA_BEFORE_BOUNDARY);
#endif

  return use_real_settimeofday(tp, ztp);
}
