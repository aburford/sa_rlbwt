#include "../include/sa_rlbwt.h"

// dna alphabet:
// ABCDGHKMNRSTVWY

using namespace std;

void debug() {
	uint32_t blen, boff;
	int bi;
	for (int i = 1; i < 25; i++) {
		printf("len %d has %d blocks\n", i, get_num_blocks(i));
		for (int j = 1; j < i; j++) {
			bi = get_block_index(i, j, &blen, &boff);
			printf("\toff: %d, block index: %d, blen: %lu, boff: %lu\n", j, bi, blen, boff);
		}
	}
}

void test(struct sa_rlbwt *sarl, uint32_t *sa, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		uint32_t res = query_sa_rlbwt(sarl, i);
		printf("sa[%lu]: %lu res: %lu\n", i, sa[i], res);
	}
}

void print_help() {
	printf("Usage: sa_rlbwt [-b in_file [-s sa_file]] | [-q sa_rlbwt_file patterns_file ]\n");
	printf("-b accepts plaintext file with one line of text and write out sa_rlbwt data structure to file. also accepts optional -s option to provide suffix array file\n");
	printf("-q accepts sa_rlbwt data struct file and plaintext file with new line separated patterns to search for\n");
	printf("-r accepts sa_rlbwt data struct file and suffix array file, randomly sample values of suffix array\n");
}

int build_mode(string s, char *safn, string outfn) {
	uint32_t n = s.size();
	printf("read in input string of size %llu\n", n);
	// get SA from last kmr array
	printf("computing sa\n");
	uint32_t *sa, *lf;
	if (safn) {
		uint32_t n2;
		sa = deserialize(safn, &n2);
		printf("computing lf\n");
		uint32_t *kmr = (uint32_t *)malloc(sizeof(uint32_t) * (n + 1));
		for (uint32_t i = 0; i <= n; i++)
			kmr[sa[i]] = i;
		lf = (uint32_t *)malloc(sizeof(uint32_t) * (n + 1));
		for (uint32_t i = 1; i <= n; i++)
			lf[kmr[i]] = kmr[i - 1];
		lf[kmr[0]] = 0;
		free(kmr);
	} else {
		struct kmr_result *kmr = build_kmr(s);
		sa = (uint32_t *)malloc(sizeof(uint32_t) * (n + 1));
		sa[0] = n;
		for (uint32_t i = 0; i <= n; i++)
			sa[kmr->arr[i]] = i;
		// build LF from SA
		printf("computing lf\n");
		lf = (uint32_t *)malloc(sizeof(uint32_t) * (n + 1));
		for (uint32_t i = 1; i <= n; i++)
			lf[kmr->arr[i]] = kmr->arr[i - 1];
		lf[kmr->arr[0]] = 0;
		free(kmr->arr);
		free(kmr);
	}

	printf("computing bwt\n");
	char *bwt = (char *)malloc(s.size() + 2);
	bwt[s.size() + 1] = '\0';
	for (uint32_t i = 0; i <= s.size(); i++) {
		if (sa[i])
			bwt[i] = s[sa[i]-1];
		else
			bwt[i] = '$';
	}
	printf("building rlbwt\n");
	struct rlbwt_result *rlbwt = build_rlbwt(bwt);

	//printf("rlbwt:");
	//for (int i = 0; i < rlbwt->r; i++) {
	//	printf(" (%c, %llu)", rlbwt->runs[i].c, rlbwt->runs[i].len);
	//}
	//printf("\n");

	printf("building sa_rlbwt\n");
	struct sa_rlbwt *sarl = build_sa_rlbwt(rlbwt, sa, lf);
	//print_sa_rlbwt(sarl);
	//test(sarl, sa, n + 1);
	printf("writing out sa_rlbwt\n");
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
	char *sa_file = NULL;
	char *pattern_file;
	while ((ret = getopt(argc, argv, optstr)) > 0) {
		switch (ret) {
		case 'b':
			mode = BUILD_MODE;
			infile = optarg;
			break;
		case 's':
			if (mode != BUILD_MODE) {
				print_help();
				exit(1);
			}
			sa_file = optarg;
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
		pattern_fn = argv[3];
		ifstream ifs(infile);
		struct sa_rlbwt *sarl = deserialize_sa_rlbwt(ifs);
		ifsteram patfile = ifs(pattern_fn);
		string pattern;
		while (patfile >> pattern) {
			
		}
		print_sa_rlbwt(sarl);
	} else if (mode == BUILD_MODE) {
		if (argc != 3 && argc != 5) {
			print_help();
			exit(1);
		}
		ifstream ifs(infile);
		string s;
		ifs >> s;
		string outfn(infile);
		outfn += ".sa_rlbwt";
		return build_mode(s, sa_file, outfn);
	} else if (mode == RAND_MODE) {
		if (argc != 3) {
			print_help();
			exit(1);
		}
		ifstream ifs(infile);
		struct sa_rlbwt *sarl = deserialize_sa_rlbwt(ifs);
		uint32_t len = sarl->runs[sarl->r - 1].i + sarl->runs[sarl->r - 1].len;
		minstd_rand0 gen(0);
		high_resolution_clock::time_point t1, t2;
		for (int i = 0; i < RAND_SAMPLES; i++) {
			uint32_t index = gen() % len;
			t1 = high_resolution_clock::now();
			query_sa_rlbwt(sarl, index);
			t2 = high_resolution_clock::now();
			duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
			// print in microseconds
			printf("%.3f\n", time_span.count() * 1000000);
		}
		ofstream ofs("repdna_x4.sa_rlbwt.smaller");
		serialize_sa_rlbwt(sarl, ofs);
		ofs.close();
		free(sarl);
	}
	return 0;
}
