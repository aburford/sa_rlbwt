all:
	mkdir -p bin
	g++ -std=c++17 -O3 -Wno-unused-variable -Wno-format src/kmr.cpp src/rlbwt.cpp src/main.cpp -o bin/sa_rlbwt
	g++ -std=c++17 -O3 -Wno-unused-variable -Wno-format src/kmr.cpp src/rlbwt.cpp src/sa.cpp -o bin/sa -ldivsufsort
	g++-6 -std=c++17 src/kmr.cpp src/rlbwt.cpp test/correctness_tests.cpp -o bin/test

lib:
	mkdir -p lib
	g++ -fPIC -c -o lib/rlbwt.o -std=c++17 -O3 -Wno-unused-variable -Wno-format src/rlbwt.cpp
	g++ -fPIC -c -o lib/kmr.o -std=c++17 -O3 -Wno-unused-variable -Wno-format src/kmr.cpp
	gcc -shared -o lib/libsa_rlbwt.so lib/rlbwt.o lib/kmr.o

genpatterns:
	g++ mygenpatterns.cpp -o mygenpatterns

genstring:
	g++ genstring.cpp -o genstring
