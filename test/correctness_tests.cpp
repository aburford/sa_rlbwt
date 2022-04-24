#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include "../include/sa_rlbwt.h"

bool compare_first(std::pair<char, int> a, std::pair<char, int> b){
	return a.first < b.first;
}

std::string db_sequence(int k, int n) {
	std::vector<std::pair<char, int>> perm;
	std::string sequence;
	int repeats = std::round(std::pow(k, n - 1));
	for (int i = 0; i < repeats; i++)
	{
		for (int j = 0; j < k; j++)
		{
			perm.emplace_back(j + 'A', i * k + j);
		}
	}
	std::stable_sort(perm.begin(), perm.end(), compare_first);
	bool *visited = (bool *)calloc(perm.size(), sizeof(bool));
	for (int i = 0; i < perm.size(); i++) {
		if (visited[i])
			continue;
		int perm_idx = i;
		do {
			visited[perm_idx] = true;
			sequence.push_back((perm_idx / repeats) + 'A');
			perm_idx = perm[perm_idx].second;
		} while (perm_idx != i);
	}
	return sequence;
}

int all_queries(string s) {
	uint32_t n = s.size();
	uint32_t *sa, *lf;
	struct kmr_result *kmr = build_kmr(s);
	sa = (uint32_t *)malloc(sizeof(uint32_t) * (n + 1));
	sa[0] = n;
	for (uint32_t i = 0; i <= n; i++)
		sa[kmr->arr[i]] = i;
	// build LF from SA
	lf = (uint32_t *)malloc(sizeof(uint32_t) * (n + 1));
	for (uint32_t i = 1; i <= n; i++)
		lf[kmr->arr[i]] = kmr->arr[i - 1];
	lf[kmr->arr[0]] = 0;
	free(kmr->arr);
	free(kmr);
	char *bwt = (char *)malloc(s.size() + 2);
	bwt[s.size() + 1] = '\0';
	for (uint32_t i = 0; i <= s.size(); i++) {
		if (sa[i])
			bwt[i] = s[sa[i]-1];
		else
			bwt[i] = '$';
	}
	struct rlbwt_result *rlbwt = build_rlbwt(bwt);
	struct sa_rlbwt *sarl = build_sa_rlbwt(rlbwt, sa, lf);
	for (uint64_t i = 0; i <= n; i++) {
		assert(sa[i] == query_sa_rlbwt(sarl, i));
	}
	return 0;
}

int main(int argc, char *argv[]) {
	std::string seq = db_sequence(3, 4);
	all_queries(seq);
	return 0;
}
