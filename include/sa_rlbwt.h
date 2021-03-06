#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <random>
#include <cstring>
#include <divsufsort.h>
#include <chrono>
#include <thread>

#define BUILD_MODE 1
#define QUERY_MODE 2
#define RAND_MODE 4

//#define RAND_SAMPLES 1000000
#define RAND_SAMPLES 1000000

using namespace std;
using namespace chrono;

struct kmr_result {
	uint32_t n;
	uint32_t *arr;
};

struct run {
	char c;
	uint32_t len;
};

struct rlbwt_result {
	int r;
	struct run *runs;
};

struct sa_block {
	uint32_t pos;
	uint32_t k;
	// cached run index
	uint32_t run_lo;
	uint32_t run_hi;
};

struct sa_run {
	char c;
	uint32_t i;
	uint32_t len;
	uint32_t sa;
	uint32_t lf; // only used for construction
	int nblocks; // could also be computed on the fly
	sa_block *blocks;
};

struct sa_rlbwt {
	int r;
	struct sa_run *runs;
};

struct kmr_result *build_kmr(string s);
uint32_t *deserialize(char *fn, uint64_t *len);

struct rlbwt_result *build_rlbwt(char *bwt);
struct sa_rlbwt *build_sa_rlbwt(struct rlbwt_result *res, uint32_t *sa, uint32_t *lf);
uint32_t query_sa_rlbwt(struct sa_rlbwt *sarl, uint32_t i);
void serialize_sa_rlbwt(struct sa_rlbwt *sarl, ofstream &outfile);
struct sa_rlbwt *deserialize_sa_rlbwt(ifstream &infile);
struct sa_rlbwt *deserialize_sa_rlbwt_old(ifstream &infile);
void print_sa_rlbwt(struct sa_rlbwt *sarl);
void free_sa_rlbwt(struct sa_rlbwt *sarl);
uint32_t count_occ(struct sa_rlbwt *sarl, const char *text, const char *pattern);

extern uint32_t round_pow2(uint32_t n, bool up);

// debug
int get_block_index(uint32_t len, uint32_t off, uint32_t *blen, uint32_t *boff);
int get_num_blocks(uint32_t len);


#define UDBG printf("DBG:%s:%s:%d\n", __FILE__, __func__, __LINE__)
