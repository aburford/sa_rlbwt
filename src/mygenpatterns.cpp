#include <fstream>
#include <iostream>
#include <random>

using namespace std;
int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("./mygenpatterns [text_file]\n");
		exit(1);
	}
	ifstream ifs(argv[1]);
	string fn(argv[1]);
	ofstream ofs(fn + ".patterns");
	string t;
	ifs >> t;
	ifs.close();
	minstd_rand0 gen(0);
	uint32_t len = t.size();
	uint32_t index;
	for (int i = 0; i < 1000; i++) {
		int patlen = gen() % 10 + 10;
		if (patlen >= len)
			continue;
		index = gen() % (len + 1 - patlen);
		ofs << t.substr(index, patlen) << "\n";
	}
	ofs.close();
}
