#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include "../include/sa_rlbwt.h"

bool compare_first(std::pair<char, int> a, std::pair<char, int> b){
	return a.first < b.first;
}

// Generate de Bruijn sequences
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

// Generate prefixes of the Thu-Morse sequence
std::string tm_sequence(int repeats) {
	std::string sequence("A");
	for (int i = 0; i < repeats; i++) {
		int n = sequence.size();
		for (int j = 0; j < n; j++) {
			sequence.push_back('A' + ('B' - sequence[j]));
		}
	}
	return sequence;
}

std::string random_string(int n) {
	std::string s;
	for (int i = 0; i < n; i++) {
		char c = 'A';
		// Characters are geometric to get nontrivial repetition
		while (rand() % 2 != 0 and c != 'Z') {
			c++;
		}
		s.push_back(c);
	}
	return s;
}

int compare_kmr(string s) {
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
	// Exhaustively check suffix array queries against ground truth
	// from kmr
	for (uint64_t i = 0; i <= n; i++) {
		assert(sa[i] == query_sa_rlbwt(sarl, i));
	}
	return 0;
}

int main(int argc, char *argv[]) {
	for (int k = 1; k < 8; k++) {
		for (int n = 1; n < 6; n++) {
			compare_kmr(db_sequence(k, n));
		}
	}
	for (int n = 1; n < 12; n++) {
		compare_kmr(tm_sequence(n));
	}
	int seed = time(NULL);
	std::cout << "seed: " << seed << std::endl;
	srand(seed);
	for (int n = 1; n <= 1 << 16; n *= 2) {
		compare_kmr(random_string(n));
	}
	return 0;
}
