all:
	mkdir -p bin
	g++ -std=c++17 -O3 -Wall -Wno-unused-variable -Wno-format src/kmr.cpp src/rlbwt.cpp src/main.cpp -o bin/sa_rlbwt
	g++ -std=c++17 -O3 -Wall -Wno-unused-variable -Wno-format src/kmr.cpp src/rlbwt.cpp src/sa.cpp -o bin/sa -ldivsufsort
	g++ -g -Werror src/kmr.cpp src/rlbwt.cpp test/correctness_tests.cpp -o bin/test
