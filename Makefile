all:
	g++ -g -Werror kmr.cpp rlbwt.cpp main.cpp -o sa_rlbwt
	g++ -g -Werror kmr.cpp rlbwt.cpp sa.cpp -o sa
