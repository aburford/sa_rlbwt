#include "../include/sa_rlbwt.h"

void print_help() {
	printf("Usage: sa_rlbwt [-b in_file] | [-q sa_rlbwt_file patterns_file ]\n");
	printf("-b accepts plaintext file with one line of text and write out suffix array to file\n");
	printf("-q accepts sa_rlbwt data struct file and plaintext file with new line separated patterns to search for\n");
	printf("-r accepts sa_rlbwt data struct file and suffix array file, randomly sample values of suffix array\n");
}

uint32_t query_sa(uint32_t *sa, uint32_t i) {
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
		uint32_t len;
		uint32_t *sa = deserialize(infile, &len);
	} else if (mode == BUILD_MODE) {
		ifstream ifs(infile);
		string s;
		ifs >> s;
		uint32_t n = s.size();
		struct kmr_result *kmr = build_kmr(s);
		// get SA from last kmr array
		uint32_t *sa = (uint32_t *)malloc(sizeof(uint32_t) * (n + 1));
		sa[0] = n;
		for (uint32_t i = 0; i <= n; i++)
			sa[kmr->arr[i]] = i;
		string outfn(infile);
		outfn += ".sa";
		ofstream outfile(outfn);
		outfile.write((char*)sa, sizeof(uint32_t) * (n + 1));
		outfile.close();
	} else if (mode == RAND_MODE) {
		if (argc != 3) {
			print_help();
			exit(1);
		}
		uint32_t len;
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
