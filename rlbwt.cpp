#include "sa_rlbwt.h"
#define ONE ((uint64_t)1)

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

int get_num_blocks(uint64_t len) {
	if (len == 2)
		return 1;
	int num_blocks = round_pow2((len - 1) / 2, false) + 1;
	num_blocks += round_pow2(len / 2, false) + 1;
	return num_blocks;
}

// get block index and also set block len and offset
int get_block_index(uint64_t len, uint64_t off, uint64_t *blen, uint64_t *boff) {
	if (off <= (len - 1) / 2) {
		int bi = round_pow2(off, false);
		*boff = 1 << bi;
		// truncate middle block
		*blen = min(ONE << bi, (len - 1) / 2 - *boff + 1);
		return bi;
	} else {
		int bi = round_pow2(len - off, false);
		*boff = len - (1 << bi); // right end offset
		// truncate middle block
		*blen = min(ONE << bi, *boff - (len - 1) / 2);
		*boff -= *blen - 1; // flip to left end of block
		return get_num_blocks(len) - 1 - bi;
	}
}

int find_run(struct sa_rlbwt *sarl, uint64_t i) {
	int mid, lo = 0, hi = sarl->r - 1;
	while (lo != hi) {
		mid = (lo+hi+1)/2;
		if (i < sarl->runs[mid].i)
			hi = mid - 1;
		else
			lo = mid;
	}
	return lo;
}

// can we do this faster if we compute blocks in a
// smart order to avoid repeat computation?
// start with larger blocks and compute recursively?
// take block with min k of overlapping blocks?
// right now i'm guessing it's O(rnlogn) but not sure
void compute_block(struct sa_rlbwt *sarl, int ri, int bi, uint64_t blen, uint64_t boff) {
	struct sa_run *r = &sarl->runs[ri];
	struct sa_block *b = &r->blocks[bi];
	// compute one hop back at a time
	uint64_t k = 0;
	uint64_t str_index = r->i + boff;
	do {
		k++;
		str_index += r->lf - r->i;
		ri = find_run(sarl, str_index);
		r = &sarl->runs[ri];
		//printf("str_index %llu blen %llu r->i %llu r->len %llu\n", str_index, blen, r->i, r->len);
		if (str_index == r->i || str_index + blen > r->i + r->len)
			break;
	} while (true);
	b->pos = str_index;
	b->k = k;
}

struct sa_rlbwt *build_sa_rlbwt(struct rlbwt_result *res, uint64_t *sa, uint64_t *lf) {
	struct sa_rlbwt *sarl = (struct sa_rlbwt *)malloc(sizeof(struct sa_rlbwt));
	sarl->r = res->r;
	sarl->runs = (struct sa_run *)malloc(res->r * sizeof(struct sa_run));
	struct sa_run *run;
	uint64_t str_index = 0;
	for (int i = 0; i < res->r; i++) {
		run = &sarl->runs[i];
		run->c = res->runs[i].c;
		run->i = str_index;
		run->len = res->runs[i].len;
		run->sa = sa[str_index];
		run->lf = lf[str_index];
		run->nblocks = get_num_blocks(run->len);
		run->blocks = (struct sa_block *)calloc(run->nblocks, sizeof(struct sa_block));
		str_index += run->len;
	}
	for (int i = 0; i < sarl->r; i++) {
		struct sa_run *run = &sarl->runs[i];
		uint64_t off = 1;
		while (off < run->len) {
			uint64_t blen, boff;
			int bi = get_block_index(run->len, off, &blen, &boff);
			compute_block(sarl, i, bi, blen, boff);
			off += blen;
		}
	}
	return sarl;
}

// TODO implement
uint64_t query_sa_rlbwt(struct sa_rlbwt *sarl, uint64_t i) {
	//int ri = find_run(sarl, i);
	//uint64_t k = 0, blen, boff;
	//int bi = get_block_index(len, off, &blen, &boff);

	
	return 0;
}

struct sa_rlbwt *deserialize_sa_rlbwt(ifstream &infile) {
	struct sa_rlbwt *sarl = (struct sa_rlbwt*)malloc(sizeof(struct sa_rlbwt));
	infile.read((char*)&sarl->r, sizeof(int));
	sarl->runs = (struct sa_run*)malloc(sizeof(struct sa_run) * sarl->r);
	for (int ri = 0; ri < sarl->r; ri++) {
		struct sa_run *run = &sarl->runs[ri];
		infile.read((char*)run, sizeof(sa_run) - sizeof(sa_block *));
		run->blocks = (struct sa_block*)malloc(sizeof(struct sa_block) * run->nblocks);
		infile.read((char*)run->blocks, sizeof(sa_block) * run->nblocks);
	}
	return sarl;
}

void serialize_sa_rlbwt(struct sa_rlbwt *sarl, ofstream &outfile) {
	outfile.write((char*)&sarl->r, sizeof(int));
	for (int ri = 0; ri < sarl->r; ri++) {
		struct sa_run *run = &sarl->runs[ri];
		outfile.write((char*)run, sizeof(sa_run) - sizeof(sa_block *));
		outfile.write((char*)run->blocks, sizeof(sa_block) * run->nblocks);
	}
}

void print_sa_rlbwt(struct sa_rlbwt *sarl) {
	printf("sa_rlbwt:\n");
	for (int ri = 0; ri < sarl->r; ri++) {
		struct sa_run *r = &sarl->runs[ri];
		printf("(%c, %llu) ", r->c, r->len);
		printf("i: %llu, sa: %llu, lf: %llu, nblocks: %d\n", r->i, r->sa, r->lf, r->nblocks);
		for (int bi = 0; bi < r->nblocks; bi++) {
			struct sa_block *b = &r->blocks[bi];
			printf("\tblock %d: pos %llu, k %llu\n", bi, b->pos, b->k);
		}
	}
}

void free_sa_rlbwt(struct sa_rlbwt *sarl) {
	for (int i = 0; i < sarl->r; i++)
		free(sarl->runs[i].blocks);
	free(sarl->runs);
}
