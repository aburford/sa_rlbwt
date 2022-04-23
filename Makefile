all:
	g++ -g -Wall -Wno-format kmr.cpp rlbwt.cpp main.cpp -o sa_rlbwt
	g++ -g -Wall -Wno-format kmr.cpp rlbwt.cpp sa.cpp -o sa
