all:
	mkdir -p bin
	g++ -g -Werror src/kmr.cpp src/rlbwt.cpp src/main.cpp -o bin/sa_rlbwt
	g++ -g -Werror src/kmr.cpp src/rlbwt.cpp src/sa.cpp -o bin/sa
	g++ -g -Werror src/kmr.cpp src/rlbwt.cpp test/correctness_tests.cpp -o bin/test
