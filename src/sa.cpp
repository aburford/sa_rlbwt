#include "../include/sa_rlbwt.h"

void print_help() {
	printf("Usage: sa_rlbwt [-b in_file] [-r sa_file]\n");
	printf("-b accepts plaintext file with one line of text and write out suffix array to file\n");
	printf("-r accepts sa_rlbwt data struct file and suffix array file, randomly sample values of suffix array\n");
}

uint32_t __attribute__((optimize("O0"))) query_sa(uint32_t *sa, uint32_t i) {
	return sa[i];
}

int main(int argc, char *argv[]) {
	int ret = 0;
	const char *optstr = "hb:q:s:r:";
	int mode = 0;
	char *infile;
	char *pattern_file;
	while ((ret = getopt(argc, argv, optstr)) > 0) {
		switch (ret) {
		case 'b':
			mode = BUILD_MODE;
			infile = optarg;
			break;
		case 'q':
			mode = QUERY_MODE;
			infile = optarg;
			break;
		case 'r':
			mode = RAND_MODE;
			infile = optarg;
			break;
		case 'h':
			print_help();
			exit(0);
			break;
		}
	}
	if (!mode) {
		print_help();
		exit(1);
	}
	if (mode == QUERY_MODE) {
		if (argc != 4) {
			print_help();
			exit(1);
		}
		printf("query mode\n");
		pattern_file = argv[3];
		uint64_t len;
		uint32_t *sa = deserialize(infile, &len);
		// we never ended up implementing this part
	} else if (mode == BUILD_MODE) {
		ifstream ifs(infile);
		string s;
		ifs >> s;
		int64_t n = s.size();
		//struct kmr_result *kmr = build_kmr(s);
		int64_t *sa = (int64_t *)malloc(sizeof(int64_t) * n);
		printf("calling divsufsort\n");
		divsufsort((const unsigned char*)s.c_str(), (saidx_t *)sa, n);
		printf("copying to uint32\n");
		uint32_t *sa32 = (uint32_t *)malloc(sizeof(uint32_t) * (n+1));
		sa32[0] = n;
		for (uint32_t i = 1; i <= n; i++)
			sa32[i] = sa[i-1];
		string outfn(infile);
		outfn += ".sa";
		ofstream outfile(outfn);
		printf("writing output file\n");
		outfile.write((char*)sa32, sizeof(uint32_t) * (n + 1));
		outfile.close();
	} else if (mode == RAND_MODE) {
		if (argc != 3) {
			print_help();
			exit(1);
		}
		uint64_t len;
		uint32_t *sa = deserialize(infile, &len);
		len /= sizeof(uint32_t);
		minstd_rand0 gen(0);
		high_resolution_clock::time_point t1, t2;
		for (int i = 0; i < RAND_SAMPLES; i++) {
			uint32_t index = gen() % len;
			t1 = high_resolution_clock::now();
			query_sa(sa, index);
			t2 = high_resolution_clock::now();
			duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
			// print in microseconds
			printf("%.3f\n", time_span.count() * 1000000);
		}
	}
	return 0;
}
