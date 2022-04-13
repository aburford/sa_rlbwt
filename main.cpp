#include "sa_rlbwt.h"

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
			printf("\toff: %d, block index: %d, blen: %llu, boff: %llu\n", j, bi, blen, boff);
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2 || argv[1][0] == '-') {
		cout << "Usage: sa_rlbwt FILE" << endl;
		exit(0);
	}
	ifstream ifs(argv[1]);
	string s;
	while (ifs >> s) {
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
		printf("sa_rlbwt:\n");
		for (int ri = 0; ri < sarl->r; ri++) {
			struct sa_run *r = &sarl->runs[ri];
			printf("(%c, %llu)", r->c, r->len);
			printf("\ti: %llu, sa: %llu, lf: %llu, nblocks: %d\n", r->i, r->sa, r->lf, r->nblocks);
			for (int bi = 0; bi < r->nblocks; r++) {
				struct sa_block *b = &r->blocks[bi];
				printf("\t\tblock %d: pos %llu, k %llu\n", bi, b->pos, b->k);
			}
		}

		free(rlbwt);
		free(sa);
		free(bwt);
		free(lf);

	}
	return 0;
}
