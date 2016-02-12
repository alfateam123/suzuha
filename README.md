Suzuha [was: gtod-shim]
===================================================

Shim library for gettimeofday() for testing and exposing time-related bugs.

How it started
==============

Presently used to set time to a small distance (10 seconds) before
the most recent boundary for overflowing a 32bit representation
of time-since-epoch in milliseconds.


This demonstrates using the shim library to expose
a bug in w3c from libwww 5.4.0:

```sh
LD_PRELOAD=$HOME/src/gtod-shim/gtod.so w3c http://localhost
Looking up localhost
Looking up localhost
Contacting localhost
Segmentation fault
```


How it works now
================

Fortunately, the source code was easy to modify and extend.

Build
-----

```sh
make       #builds gotd.so
make test  #builds the test executable 
```

if you want to read debug lines, open the Makefile and
add `-DGTOD_SHIM_DEBUG` to `gtod.o:` step.


Test
----

This is an example of the log you'll read running the commands shown below.
emphasis (the separation between logs) is added for clarification,
and won't be shown in the actual execution.

```sh
[winter@nest] [/dev/pts/3] [master ⚡] 
└[~/projects/coolprojects/gtod-shim]> ./test; LD_PRELOAD=./gtod.so ./test
############## normal log #################

1442444777.442752
Operation not permitted
waiting 2 seconds...
(after setting the time) 1442444779.442954

############# shim'd log #####################

[GTOD_HOOK] DELTA_BEFORE_BOUNDARY=0
[GTOD_HOOK] delta to 32bit msec boundary=0ms, reporting time as: Thu Sep 17 01:06:19 2015
1442444779.448548
[STOD_HOOK] called custom settimeofday!
[STOD_HOOK] DELTA_BEFORE_BOUNDARY=102
Operation not permitted
waiting 2 seconds...
[GTOD_HOOK] DELTA_BEFORE_BOUNDARY=102
[GTOD_HOOK] delta to 32bit msec boundary=0ms, reporting time as: Thu Sep 17 01:04:39 2015
(after setting the time) 1442444679.448834
```

In the `shim'd log`, expect the difference between the two times to be
100 seconds: we ask the time to be shifted by 102 seconds, but we also wait (spend) 2 seconds,
so 102-2=100 seconds is the value you'll get.

How to use the shim
-------------------

If you read the Test section, you should've understood how you can preload the shim into
your testing application. This section explains how you can leap through time
and know where you landed.

This library mocks some functions.

* `int gettimeofday(struct timevalue*, struct timezone*)`
   this works as explained in `gettimeofday(3P)`.
   if you did not call `settimeofday` before, you'll get the machine time.
   if you _did_ call `settimeofday` before, the time will be shifted as explained in the following doc.
   it may crash if you give a non-null value to the second parameter.

* `int settimeofday(const struct timevalue*, const struct timezone* tzp)`
   the time value is ignored.
   if `tzp->tz_dsttime` is equal to 0 (zero), then the time shift is set
   according to the value of `tzp->tz_minuteswest`.
   Please, please note that the time in `tzp->tz_minuteswest` is assumed to be
   in **seconds**.
   A _positive_ value means a shift in the _past_, a _negative_ one a shift in the _future_.

* `time_t time(time_t*)`
  Simulates `time` from `time.h`, returning the appropriate time value, in seconds.
  **WARNING**: The parameter is ignored, the new value is ALWAYS returned.

Hack it!
--------

Automated testing is here.

To run the tests, just run `make run_tests`. This command will take care of Python testing libraries,
building of the shim library and test app building. Obviously, you'll need CPython to run the tests.
Other Python implementations may be not supported.

You can find the tests in the `tests/` folder.

Troubleshooting
---------------

* _compilation fails with libstdc++.a being a broken symlink_  
Try to replace `libstdc++.a` with `libstdc++.so`. On my machine,
it seemed to work.
