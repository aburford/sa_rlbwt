#include <iostream>
#include <random>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("./genstring [text_file]\n");
		exit(1);
	}
	ifstream ifs(argv[1]);
	string fn(argv[1]);
	string s;
	string sigma("ATGC");
	ifs >> s;
	s += s;
	s += s;
	minstd_rand0 gen(0);
	for (uint32_t i = 0; i < s.size() / 1000; i++) {
		int x = gen() % s.size();
		s[x] = sigma[gen() % 4];
	}
	ofstream ofs(fn + "_x4");
	ofs << s;
	ifs.close();
	ofs.close();
}
