#include "sa_rlbwt.h"

struct rlbwt_result *build_rlbwt(char *bwt) {
	printf("%s\n", bwt);
	uint64_t n = strlen(bwt);
	struct rlbwt_result *res = (struct rlbwt_result *)malloc(sizeof(struct rlbwt_result));
	struct run *run;
	res->r = 1;
	res->runs = (struct run *)malloc(sizeof(struct run) * n);
	run = &res->runs[0];
	run->c = bwt[0];
	run->len = 1;
	for (uint64_t i = 1; i < n; i++) {
		if (bwt[i] == bwt[i - 1]) {
			res->runs[res->r - 1].len++;
		} else {
			run = &res->runs[res->r];
			run->c = bwt[i];
			run->len = 1;
			res->r++;
		}
	}
	return res;
}

