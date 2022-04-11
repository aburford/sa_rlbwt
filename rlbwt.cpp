#include "sa_rlbwt.h"

struct rlbwt_result *build_rlbwt(char *bwt) {
	printf("%s\n", bwt);
	return NULL;
}

void free_rlbwt(struct rlbwt_result *rlbwt) {
	return;
	//for (int i = 0; i < rlbwt->r; i++)
	//	free(rlbwt->runs[i]);
	free(rlbwt);
}
