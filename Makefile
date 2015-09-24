
CXX := g++

all: gtod.so test.c

#gtod.o: gtod.cpp
#	$(CXX) -DGTOD_SHIM_DEBUG  -fPIC $^ -c -o $@ -Wall -Werror

gtod.o: gtod.cpp
	$(CXX) -fPIC $^ -c -o $@ -Wall -Werror

gtod.so: gtod.o libstdc++.a
	$(CXX) -shared $^ -o $@ -ldl 

libstdc++.a:
	ln -s $(shell $(CXX) -print-file-name=libstdc++.a)

test.c:
	$(CXX) $@ -Wall -Werror

clean:
	-rm gtod.o gtod.so libstdc++.a test

run_tests:
	bash run_tests.sh
