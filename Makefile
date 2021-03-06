
CXX := g++

all: gtod.so test

gtod.o: gtod.cpp
	$(CXX) -fPIC $^ -c -o $@ -Wall -Werror #-DGTOD_SHIM_DEBUG

gtod.so: gtod.o libstdc++.a
	$(CXX) -shared $^ -o $@ -ldl 

libstdc++.a:
	ln -s $(shell $(CXX) -print-file-name=libstdc++.a)

test:
	$(CXX) c_tests/test.c -o test -Wall -Werror && \
	$(CXX) c_tests/time.c -o time -Wall -Werror && \
	$(CXX) c_tests/warp.c -o warp -Wall -Werror -std=c++0x &&\
	$(CXX) c_tests/settime.c -o settime -Wall -Werror

clean:
	rm gtod.o gtod.so libstdc++.a test warp settime time

run_tests:
	bash run_tests.sh
