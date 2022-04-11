#include "sa_rlbwt.h"

// dna alphabet:
// ABCDGHKMNRSTVWY

using namespace std;

int main(int argc, char *argv[]) {
	if (argc != 2 || argv[1][0] == '-') {
		cout << "Usage: sa_rlbwt FILE" << endl;
		exit(0);
	}
	ifstream ifs(argv[1]);
	string s;
	while (ifs >> s) {
		printf("read string of length %lu\n", s.size());
		printf("%s\n", s.c_str());
		struct kmr_result *kmr = build_kmr(s);
		printf("k:%llu\n", kmr->k);
		printf("kmr:");
		for (uint64_t i = 0; i <= kmr->n; i++)
			printf(" %llu", kmr->arrays[0][i]);
		printf("\n");
		printf("kmr:");
		for (uint64_t i = 0; i <= kmr->n; i++)
			printf(" %llu", kmr->arrays[1][i]);
		printf("\n");
		printf("kmr:");
		for (uint64_t i = 0; i <= kmr->n; i++)
			printf(" %llu", kmr->arrays[2][i]);
		printf("\n");
		// get SA from last kmr array
		uint64_t *sa = (uint64_t *)malloc(sizeof(uint64_t) * (kmr->n + 1));
		for (uint64_t i = 0; i <= kmr->n; i++)
			sa[kmr->arrays[kmr->k][i] - 1] = i;
		printf("sa:");
		for (uint64_t i = 0; i <= kmr->n; i++)
			printf(" %llu", sa[i]);
		printf("\n");
		free_kmr(kmr);
		char *bwt = (char *)malloc(s.size() + 1);
		for (uint64_t i = 0; i <= s.size(); i++) {
			if (sa[i])
				bwt[i] = s[sa[i]-1];
			else
				bwt[i] = '$';
		}
		struct rlbwt_result *rlbwt = build_rlbwt(bwt);
		
		free_rlbwt(rlbwt);
		free(sa);
		free(bwt);
	}
	return 0;
}
