#include "sa_rlbwt.h"

struct kmr_result *build_kmr(string s) {
	uint32_t n = s.size();
	printf("allocating kmr arrays\n");
	struct kmr_result *res = (struct kmr_result *)malloc(sizeof(kmr_result));
	int k = round_pow2(n, true);
	res->n = n;
	uint32_t *kmr = (uint32_t *)malloc(sizeof(uint32_t) * (n+1));
	uint32_t *kmr_prev = (uint32_t *)malloc(sizeof(uint32_t) * (n+1));
	kmr[n] = 0;
	kmr_prev[n] = 0;
	printf("computing first kmr array\n");
	// fill in A_0
	for (uint32_t i = 0; i < n; i++)
		kmr_prev[i] = s[i] - 'A' + 1;
	int alphasort[27];
	for (uint32_t i = 0; i < 27; i++)
		alphasort[i] = 0;
	for (uint32_t i = 0; i < n; i++)
		alphasort[kmr_prev[i]] = 1;
	for (uint32_t i = 1; i < 27; i++)
		alphasort[i] += alphasort[i-1];
	for (uint32_t i = 0; i < n; i++)
		kmr_prev[i] = alphasort[kmr_prev[i]];
	//printf("kmr array 0: ");
	//for (uint32_t i = 0; i <= n; i++)
	//	printf("%d ", kmr_prev[i]);
	//printf("\n");
	uint32_t *swap;
	uint32_t *counts = (uint32_t*)malloc(sizeof(uint32_t) * (n+1));
	uint32_t *counts_copy = (uint32_t*)malloc(sizeof(uint32_t) * (n+1));
	uint32_t *permute = (uint32_t*)malloc(sizeof(uint32_t) * n);
	uint64_t *tmp = (uint64_t*)malloc(sizeof(uint64_t) * n);
	for (uint32_t ai = 1; ai <= k; ai++) {
		printf("computing kmr array %llu of %llu\n", ai, k);
		for (uint32_t i = 0; i <= n; i++)
			counts[i] = 0;
		uint32_t offset = 1 << (ai - 1);
		for (uint32_t i = 0; i < n; i++)
			counts[kmr_prev[min(i+offset, n)]]++;
		for (uint32_t i = 1; i <= n; i++)
			counts[i] += counts[i-1];
		for (uint32_t i = n-1; i < n; i--) {
			permute[counts[kmr_prev[min(i+offset, n)]]-- - 1] = i;
		}
		for (uint32_t i = 0; i <= n; i++)
			counts[i] = 0;
		for (uint32_t i = 0; i < n; i++)
			counts[kmr_prev[permute[i]]]++;
		counts_copy[0] = counts[0];
		for (uint32_t i = 1; i <= n; i++) {
			counts[i] += counts[i-1];
			counts_copy[i] = counts[i];
		}
		for (uint32_t i = n-1; i < n; i--) {
			uint64_t tmpval = kmr_prev[permute[i]] * (n + 1) + kmr_prev[min(permute[i]+offset, n)];
			tmp[counts[kmr_prev[permute[i]]]-- - 1] = tmpval;
		}
		uint32_t prev = 1;
		for (uint32_t i = 1; i < n; i++) {
			if (tmp[i-1] != tmp[i])
				tmp[i-1] = prev++;
			else
				tmp[i-1] = prev;
		}
		tmp[n-1] = prev;
		for (uint32_t i = n-1; i < n; i--)
			kmr[permute[i]] = tmp[counts_copy[kmr_prev[permute[i]]]-- - 1];
		//printf("kmr arr %d: ", ai);
		//for (int i = 0; i <= n; i++)
		//	printf("%d ", kmr[i]);
		//printf("\n");
		swap = kmr;
		kmr = kmr_prev;
		kmr_prev = swap;
	}
	free(counts);
	free(counts_copy);
	free(permute);
	free(tmp);
	free(kmr);
	res->arr = kmr_prev;
	return res;
}

uint32_t round_pow2(uint32_t n, bool up) {
	if (n < 3)
		return n - 1;
	int hi = 31, mid, lo = 1;
	uint32_t base = 1;
	if ((base << hi) == n)
		return hi;
	if (n > (base << hi))
		return up ? (hi + 1) : hi;
	while (hi - lo > 1) {
		mid = (hi+lo)/2;
		if (n < (base << mid))
			hi = mid;
		else if (n > (base << mid))
			lo = mid;
		else
			return mid;
	}
	if (up)
		return hi;
	return lo;
}

uint32_t *deserialize(char *fn, uint32_t *len) {
	ifstream ifs(fn);
	ifs.seekg(0, ifs.end);
	*len = ifs.tellg();
	uint32_t *sa = (uint32_t *)malloc(*len);
	ifs.seekg(0, ifs.beg);
	ifs.read((char*)sa, *len);
	return sa;
}

