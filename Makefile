all: test lib genpatterns genstring
	mkdir -p bin
	g++ -std=c++17 -O3 -Wno-unused-variable -Wno-format src/kmr.cpp src/rlbwt.cpp src/main.cpp -o bin/sa_rlbwt
	g++ -std=c++17 -O3 -Wno-unused-variable -Wno-format src/kmr.cpp src/rlbwt.cpp src/sa.cpp -o bin/sa -ldivsufsort

test: bin/test
bin/test:
	mkdir -p bin
	g++ -std=c++17 src/kmr.cpp src/rlbwt.cpp test/correctness_tests.cpp -o bin/test

lib:
	mkdir -p lib
	g++ -fPIC -c -o lib/rlbwt.o -std=c++17 -O3 -Wno-unused-variable -Wno-format src/rlbwt.cpp
	g++ -fPIC -c -o lib/kmr.o -std=c++17 -O3 -Wno-unused-variable -Wno-format src/kmr.cpp
	gcc -shared -o lib/libsa_rlbwt.so lib/rlbwt.o lib/kmr.o

mygenpatterns: bin/mygenpatterns
bin/mygenpatterns:
	mkdir -p bin
	g++ src/mygenpatterns.cpp -o bin/mygenpatterns

genstring: bin/genstring
bin/genstring:
	mkdir -p bin
	g++ src/genstring.cpp -o bin/genstring
