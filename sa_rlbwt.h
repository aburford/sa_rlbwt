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

struct kmr_result *build_kmr(string s);
void free_kmr(struct kmr_result *);

struct rlbwt_result *build_rlbwt(char *bwt);
struct run *get_run(struct rlbwt_result *, uint64_t);
