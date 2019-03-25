#define _CRT_SECURE_NO_WARNINGS

#include <stdbool.h>
#include <stdio.h>
#include <memory.h>
#include "np.h"
#include "permutation.h"
#include "printtable.h"
#include "order.h"

#define MAXORDER 100
#define MAXSTOCK 100



static int np[MAXORDER][MAXORDER] = { 0 };
static int npfactor[MAXORDER][MAXORDER][MAXORDER] = { 0 };
static int nprowfactor[MAXORDER][MAXORDER] = { 0 };
static int npcolfactor[MAXORDER][MAXORDER] = { 0 };
static int npsqrfactor[MAXORDER][MAXORDER] = { 0 };
static int npdepth[MAXORDER][MAXORDER] = { 0 };

static int order;
static int nfactors;
static int ndivisors;

void rowcolsqrrenew() {
	int i, j, f;
	pair pair;


	for (i = 1; i <= nfactors; i++) {
		for (f = 1; f <= nfactors; f++) {
			nprowfactor[i][f] = 0;
			npcolfactor[i][f] = 0;
			npsqrfactor[i][f] = 0;
		}
	}

	for (i = 1; i <= nfactors; i++) {
		for (j = 1; j <= nfactors; j++) {
			pair.left = i;
			pair.right = j;
			if (npdepth[i][j] == 0) {
				for (f = 1; f <= nfactors; f++) {
					if (npfactor[i][j][f] == 0) {
						nprowfactor[i][f]++;
						npcolfactor[j][f]++;
						npsqrfactor[np_pair_to_sqr(pair)][f]++;
					}
				}
			}
			else {
				f = np[i][j];
				nprowfactor[i][f] = -1;
				npcolfactor[j][f] = -1;
				npsqrfactor[np_pair_to_sqr(pair)][f] = -1;
			}
		}
	}

}

void np_initialize() {

	order = 10;
	nfactors = order - 1;

}

void get_np(int np_[MAXORDER][MAXORDER]) {
	memcpy(np_, np, sizeof(np));
}

int np_input() {
	int i, j, k;
	struct pair pair;
	char c;
	for (i = 1; i <= nfactors; i++) {
		for (j = 1; j <= nfactors; j++) {
			scanf("%c", &c);
			for (k = 1; k <= nfactors; k++) {
				if (c == '0' + k) {
					np[i][j] = k;
					npdepth[i][j] = -1;
					pair.left = i;
					pair.right = j;
					np_decide(pair, k , -1);
				}
			}
		}
		scanf("%c", &c);
	}
	return 0;
}

void make_np() {
	np_initialize();
	np_input();
	np_printtable();

	np_backtrackfc(1);
}

void np_decide(struct pair pair, int factor, int depth) {
	np[pair.left][pair.right] = factor;
	npdepth[pair.left][pair.right] = depth;
	np_factordelete(pair, depth);
}

int np_backtrackfc(int depth) {
	//	printf("depth=%d\n", depth);
	//	np_printtable(' ', add);

	if (table_is_full(npdepth)) {
//		printf("表が埋まりました。\n");
		printtable(' ',np);
		return 0;
	}

	struct pair pair;
	pair = np_nextpair();
	//	printf("pair=(%d,%d)\n", pair.left, pair.right);

	for (int d = 1; d <= nfactors; ++d) {
		if (npfactor[pair.left][pair.right][d] != 0) continue;
		npdepth[pair.left][pair.right] = depth;
		np[pair.left][pair.right] = d;
		np_factordelete(pair, depth);

		//		printf("depth=%d d=%d\n", depth, d);
		//		group_printtable('+', add);
		/*確定、確認*/
		if (np_lf(pair, depth) == true) {
			//		printf("矛盾が出ませんでした さらに仮定します\n\n");
			//		printf("depth = %d\n", depth);
			//		printf("pair=(%d,%d)\n", pair.left, pair.right);
			//		printf("d=%d\n", d);
			np_backtrackfc(depth + 1);
		}
		else {
			//			printf("矛盾が出ました 次の仮定へ行きます\n\n");
		}

		np_clean(depth);
	}

	return 0;
}

int np_printtable() {
	int x, y;

	printf(" ");
	for (y = 1; y < order; y++)
		printf("_%d", y);
	printf("\n");
	for (x = 1; x < order; x++) {
		printf("%d|", x);
		for (y = 1; y < order; y++) {
			if (npdepth[x][y] == 0) {
				printf("  ");
			}
			else {
				printf(" %d", np[x][y]);
			}
		}
		printf("\n");
	}
	printf("\n");
	return 0;
}

void np_clean(int depth) {
	int a, b, c;
	for (a = 1; a <= nfactors; ++a) {
		for (b = 1; b <= nfactors; ++b) {
			if (npdepth[a][b] == depth)
				npdepth[a][b] = 0;
			for (c = 1; c <= nfactors; ++c) {
				if (npfactor[a][b][c] == depth)
					npfactor[a][b][c] = 0;
			}
		}
	}
}

void np_factordelete(struct pair newpair, int depth) {
	int left = newpair.left, right = newpair.right;
	int factor = np[left][right];
	for (int j = 1; j < order; ++j) {
		if (npfactor[left][j][factor] == 0) {
			npfactor[left][j][factor] = depth;
		}
	}
	for (int i = 1; i < order; ++i) {
		if (npfactor[i][right][factor] == 0) {
			npfactor[i][right][factor] = depth;
		}
	}
	struct pair pairs[9];
	np_pair_to_pairs(newpair, pairs);
	for (int k = 0; k < nfactors; k++) {
		if (npfactor[pairs[k].left][pairs[k].right][factor] == 0) {
			npfactor[pairs[k].left][pairs[k].right][factor] = depth;
		}
	}
}

int np_pair_to_sqr(pair pair) {
	int i,j;
	i = (pair.left - 1) / 3;
	j = (pair.right - 1) / 3;
	return i * 3 + j + 1;
}
void np_sqr_to_pairs(int sqr, pair pairs[9]){
	int i, j;
	i = (sqr - 1) / 3;
	j = (sqr - 1) % 3;
	pair start;
	start.left = i * 3 + 1;
	start.right = j * 3 + 1;
	pair pair;
	for (int k = 1; k <= 9; k++){
		i = (k - 1) / 3;
		j = (k - 1) % 3;
		pair.left = start.left + i;
		pair.right = start.right + j;
		pairs[k-1] = pair;
	}
}
void np_pair_to_pairs(pair pair, struct pair pairs[9]) {
	int sqr = np_pair_to_sqr(pair);
	np_sqr_to_pairs(sqr, pairs);
}

int np_lf(struct pair pair, int depth) {
	np_printtable();
//	printf("lf pair=(%d,%d)\n", pair.left, pair.right);

	int change;
	struct pair newpair;
	int i, j, sqr,k;
	struct pair pairs[9];

	while (true) {
		change = false;

		rowcolsqrrenew();
//		printtable('d', npdepth);
//		printtable('r', nprowfactor);
//		printtable('c', npcolfactor);
//		printtable('s', npsqrfactor);

		int f = np[pair.left][pair.right];

		for (i = 1; i <= nfactors; i++) {
			if (nprowfactor[i][f] == 0) {
//				printf("%d行目に%dが入りません\n", i, f);
				return false;
			}
			if (nprowfactor[i][f] == 1) {
				for (j = 1; j <= nfactors; j++) {
					if (npdepth[i][j] == 0) {
						if (npfactor[i][j][f] == 0) {
							newpair.left = i;
							newpair.right = j;
						}
					}
				}
				if (change == false) change = true;
				npdepth[newpair.left][newpair.right] = depth;
				np[newpair.left][newpair.right] = f;
				np_factordelete(newpair, depth);
				if (np_lf(newpair, depth) == false)
					return false;
			}
		}
		for (j = 1; j <= nfactors; j++) {
			if (npcolfactor[j][f] == 0) {
				//				printf("%d列目に%dが入りません\n", j, f);
				return false;
			}
			if (npcolfactor[j][f] == 1) {
				for (i = 1; i <= nfactors; i++) {
					if (npdepth[i][j] == 0) {
						if (npfactor[i][j][f] == 0) {
							newpair.left = i;
							newpair.right = j;
							break;
						}
					}
				}
				if (change == false) change = true;
				npdepth[newpair.left][newpair.right] = depth;
				np[newpair.left][newpair.right] = f;
				np_factordelete(newpair, depth);
				if (np_lf(newpair, depth) == false)
					return false;
			}
		}
		for (sqr = 1; sqr <= nfactors; sqr++) {
			if (npsqrfactor[sqr][f] == 0) {
				printf("%d番目の正方形に%dが入りません\n", sqr, f);
				return false;
			}
			if (npsqrfactor[sqr][f] == 1) {
				np_sqr_to_pairs(sqr, pairs);
				for (k = 0; k < nfactors; k++) {
					if (npdepth[pairs[k].left][pairs[k].right] == 0) {
						if (npfactor[pairs[k].left][pairs[k].right][f] == 0) {
							newpair.left = pairs[k].left;
							newpair.right = pairs[k].right;
							break;
						}
					}
				}
				if (change == false) change = true;
				npdepth[newpair.left][newpair.right] = depth;
				np[newpair.left][newpair.right] = f;
				np_factordelete(newpair, depth);
				if (np_lf(newpair, depth) == false)
					return false;
			}
		}
		if (change == false) break;
	}
	return true;
}

int np_nrest(struct pair pair) {
	int count = 0;
	for (int i = 1; i < order; ++i) {
		if (npfactor[pair.left][pair.right][i] == 0) {
			count++;
		}
	}
	return count;
}

struct pair np_nextpair() {
	int left, right;
	struct pair pair;
	int min = order;
	struct pair minpair;
	for (left = 1; left <= nfactors; ++left) {
		for (right = 1; right <= nfactors; ++right) {
			if (npdepth[left][right] == 0) {
				pair.left = left;
				pair.right = right;
				if (np_nrest(pair) < min) {
					min = np_nrest(pair);
					minpair = pair;
				}
			}
		}
	}
	return minpair;
}