all:
	g++ -std=c++17 -O0 -Wall -Wno-unused-variable -Wno-format kmr.cpp rlbwt.cpp main.cpp -o sa_rlbwt
	g++ -std=c++17 -O0 -Wall -Wno-unused-variable -Wno-format kmr.cpp rlbwt.cpp sa.cpp -o sa -ldivsufsort
