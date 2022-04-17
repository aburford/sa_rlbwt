#include <iostream>
#include <fstream>

using namespace std;

struct kmr_result {
	// arrays is (k+1) x (n+1) 2d array
	uint64_t n;
	uint64_t k;
	uint64_t **arrays;
};

struct run {
	char c;
	uint64_t len;
};

struct rlbwt_result {
	int r;
	struct run *runs;
};

struct sa_block {
	uint64_t pos;
	uint64_t k;
	// cached run index
	uint64_t run_i;
};

struct sa_run {
	char c;
	uint64_t i;
	uint64_t len;
	uint64_t sa;
	uint64_t lf; // only used for construction
	int nblocks; // could also be computed on the fly
	sa_block *blocks;
};

struct sa_rlbwt {
	int r;
	struct sa_run *runs;
};

struct kmr_result *build_kmr(string s);
void free_kmr(struct kmr_result *);

struct rlbwt_result *build_rlbwt(char *bwt);
struct sa_rlbwt *build_sa_rlbwt(struct rlbwt_result *res, uint64_t *sa, uint64_t *lf);
uint64_t query_sa_rlbwt(struct sa_rlbwt *sarl, uint64_t i);

extern uint64_t round_pow2(uint64_t n, bool up);

// debug
int get_block_index(uint64_t len, uint64_t off, uint64_t *blen, uint64_t *boff);
int get_num_blocks(uint64_t len);
