#include "sa_rlbwt.h"

struct kmr_result *build_kmr(string s) {
	uint64_t n = s.size();
	struct kmr_result *res = (struct kmr_result *)malloc(sizeof(kmr_result));
	res->k = round_pow2(n, true);
	res->n = n;
	uint64_t **kmr = (uint64_t **)malloc(sizeof(uint64_t *) * (res->k+1));
	for (uint64_t i = 0; i <= res->k; i++) {
		kmr[i] = (uint64_t *)malloc(sizeof(uint64_t) * (n+1));
		kmr[i][n] = 0;
	}
	// fill in A_0
	for (uint64_t i = 0; i < n; i++)
		kmr[0][i] = s[i] - 'A' + 1;
	int alphasort[27];
	for (uint64_t i = 0; i < 27; i++)
		alphasort[i] = 0;
	for (uint64_t i = 0; i < n; i++)
		alphasort[kmr[0][i]] = 1;
	for (uint64_t i = 1; i < 27; i++)
		alphasort[i] += alphasort[i-1];
	for (uint64_t i = 0; i < n; i++)
		kmr[0][i] = alphasort[kmr[0][i]];

	uint64_t *counts = (uint64_t*)malloc(sizeof(uint64_t) * (n+1));
	uint64_t *counts_copy = (uint64_t*)malloc(sizeof(uint64_t) * (n+1));
	uint64_t *permute = (uint64_t*)malloc(sizeof(uint64_t) * n);
	uint64_t *tmp = (uint64_t*)malloc(sizeof(uint64_t) * n);
	for (uint64_t ai = 1; ai <= res->k; ai++) {
		for (uint64_t i = 0; i <= n; i++)
			counts[i] = 0;
		uint64_t offset = 1 << (ai - 1);
		for (uint64_t i = 0; i < n; i++)
			counts[kmr[ai-1][min(i+offset, n)]]++;
		for (uint64_t i = 1; i <= n; i++)
			counts[i] += counts[i-1];
		for (uint64_t i = n-1; i < n; i--) {
			permute[counts[kmr[ai-1][min(i+offset, n)]]-- - 1] = i;
		}
		for (uint64_t i = 0; i <= n; i++)
			counts[i] = 0;
		for (uint64_t i = 0; i < n; i++)
			counts[kmr[ai-1][permute[i]]]++;
		counts_copy[0] = counts[0];
		for (uint64_t i = 1; i <= n; i++) {
			counts[i] += counts[i-1];
			counts_copy[i] = counts[i];
		}
		for (uint64_t i = n-1; i < n; i--) {
			uint64_t tmpval = kmr[ai-1][permute[i]] * (n + 1) + kmr[ai-1][min(permute[i]+offset, n)];
			tmp[counts[kmr[ai-1][permute[i]]]-- - 1] = tmpval;
		}
		uint64_t prev = 1;
		for (uint64_t i = 1; i < n; i++) {
			if (tmp[i-1] != tmp[i])
				tmp[i-1] = prev++;
			else
				tmp[i-1] = prev;
		}
		tmp[n-1] = prev;
		for (uint64_t i = n-1; i < n; i--)
			kmr[ai][permute[i]] = tmp[counts_copy[kmr[ai-1][permute[i]]]-- - 1];
	}
	free(counts);
	free(counts_copy);
	free(permute);
	free(tmp);
	res->arrays = kmr;
	return res;
}

void free_kmr(struct kmr_result *kmr) {
	for (uint64_t i = 0; i <= kmr->k; i++)
		free(kmr->arrays[i]);
	free(kmr->arrays);
	free(kmr);
}

uint64_t round_pow2(uint64_t n, bool up) {
	if (n < 3)
		return n - 1;
	int hi = 63, mid, lo = 1;
	uint64_t base = 1;
	if (n > (base << hi))
		return hi;
	if ((base << hi) == n)
		return hi;
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
