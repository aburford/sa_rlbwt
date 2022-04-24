#include "../include/sa_rlbwt.h"

// dna alphabet:
// ABCDGHKMNRSTVWY

using namespace std;

void debug() {
	uint64_t blen, boff;
	int bi;
	for (int i = 1; i < 25; i++) {
		printf("len %d has %d blocks\n", i, get_num_blocks(i));
		for (int j = 1; j < i; j++) {
			bi = get_block_index(i, j, &blen, &boff);
			printf("\toff: %d, block index: %d, blen: %lu, boff: %lu\n", j, bi, blen, boff);
		}
	}
}

void test(struct sa_rlbwt *sarl, uint64_t *sa, uint64_t len) {
	for (uint64_t i = 0; i < len; i++) {
		uint64_t res = query_sa_rlbwt(sarl, i);
		printf("sa[%lu]: %lu res: %lu\n", i, sa[i], res);
	}
}

void print_help() {
	printf("Usage: sa_rlbwt [-b in_file] | [-q sa_rlbwt_file patterns_file ]\n");
	printf("-b accepts plaintext file with one line of text and write out sa_rlbwt data structure to file\n");
	printf("-q accepts sa_rlbwt data struct file and plaintext file with new line separated patterns to search for\n");
	printf("-r accepts sa_rlbwt data struct file and suffix array file, randomly sample values of suffix array\n");
}

int build_mode(string s, string outfn) {
	printf("%s\n", s.c_str());
	uint64_t n = s.size();
	struct kmr_result *kmr = build_kmr(s);
	// get SA from last kmr array
	uint64_t *sa = (uint64_t *)malloc(sizeof(uint64_t) * (n + 1));
	sa[0] = n;
	for (uint64_t i = 0; i <= n; i++)
		sa[kmr->arrays[kmr->k][i]] = i;
	// build LF from SA
	uint64_t *lf = (uint64_t *)malloc(sizeof(uint64_t) * (n + 1));
	for (uint64_t i = 1; i <= n; i++)
		lf[kmr->arrays[kmr->k][i]] = kmr->arrays[kmr->k][i - 1];
	lf[kmr->arrays[kmr->k][0]] = 0;
	free_kmr(kmr);

	char *bwt = (char *)malloc(s.size() + 2);
	bwt[s.size() + 1] = '\0';
	for (uint64_t i = 0; i <= s.size(); i++) {
		if (sa[i])
			bwt[i] = s[sa[i]-1];
		else
			bwt[i] = '$';
	}
	struct rlbwt_result *rlbwt = build_rlbwt(bwt);

	printf("rlbwt:");
	for (int i = 0; i < rlbwt->r; i++) {
		printf(" (%c, %llu)", rlbwt->runs[i].c, rlbwt->runs[i].len);
	}
	printf("\n");

	struct sa_rlbwt *sarl = build_sa_rlbwt(rlbwt, sa, lf);
	print_sa_rlbwt(sarl);
	test(sarl, sa, n + 1);
	ofstream outfile(outfn);
	serialize_sa_rlbwt(sarl, outfile);
	outfile.close();

	free(rlbwt);
	free(sa);
	free(bwt);
	free(lf);
	return 0;
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
		ifstream ifs(infile);
		struct sa_rlbwt *sarl = deserialize_sa_rlbwt(ifs);
		// TODO read patterns file and perform queries
		print_sa_rlbwt(sarl);
	} else if (mode == BUILD_MODE) {
		if (argc != 3) {
			print_help();
			exit(1);
		}
		ifstream ifs(infile);
		string s;
		ifs >> s;
		string outfn(infile);
		outfn += ".sa_rlbwt";
		return build_mode(s, outfn);
	} else if (mode == RAND_MODE) {
		if (argc != 3) {
			print_help();
			exit(1);
		}
		ifstream ifs(infile);
		struct sa_rlbwt *sarl = deserialize_sa_rlbwt(ifs);
		uint64_t len = sarl->runs[sarl->r - 1].i + sarl->runs[sarl->r - 1].len;
		minstd_rand0 gen(0);
		// TODO more precise timing
		for (int i = 0; i < RAND_SAMPLES; i++) {
			uint64_t index = gen() % len;
			printf("sa[%llu]: %llu\n", index, query_sa_rlbwt(sarl, index));
		}
	}
	return 0;
}
