#include <stdbool.h>
#include <stdio.h>
#include <memory.h>
#include "group_isomorphic.h"
#include "permutation.h"
#include "printtable.h"
#include "group.h"
#include "order.h"

#define MAXORDER 100
#define MAXNF 100
#define MAXSTOCK 100
#define MAXNDIVISORS 20

static int add[MAXORDER][MAXORDER] = {
	{0,1,2,3},
	{1,0,3,2},
	{2,3,0,1},
	{3,2,1,0}
};
static int addfactor[MAXORDER][MAXORDER][MAXORDER] = { 0 };
static int addrowfactor[MAXORDER][MAXORDER] = { 0 };
static int addcolfactor[MAXORDER][MAXORDER] = { 0 };
static int adddepth[MAXORDER][MAXORDER] = { 0 };
static int group_as[MAXORDER][MAXORDER][MAXORDER] = { 0 };
static int divisors[MAXNDIVISORS];

static int order;
static int nfactors; 
static int ndivisors;

void rowcolrenew() {
	int i, j, f;
	for (i = 1; i <= nfactors; i++) {
		for (f = 0; f <= nfactors; f++) {
			addrowfactor[i][f] = 0;
			addcolfactor[i][f] = 0;
		}
	}
	for (i = 0; i <= nfactors; i++) {
		for (j = 0; j <= nfactors; j++) {
			if (adddepth[i][j] == 0) {
				for (f = 0; f <= nfactors; f++) {
					if (addfactor[i][j][f] == 0) {
						addrowfactor[i][f]++;
						addcolfactor[j][f]++;
					}
				}
			} else {
				f = add[i][j];
				addrowfactor[i][f] = -1;
				addcolfactor[j][f] = -1;
			}
		}
	}
}

int make_divisors() {
	int i;
	int n = 0;

	for (i = 1; i <= order ; i++){
		if (order%i == 0) {
			divisors[n++] = i;
		}
	}

	return n;
}

void group_initialize() {

	order = get_order();
	nfactors = order - 1;
	ndivisors = make_divisors();

	int i, j;
	for (j = 0; j < order; j++) {
		add[0][j] = j;
		adddepth[0][j] = -1;
		if (j != 0) {
			for (i = 1; i < order; ++i) {
				addfactor[i][j][j] = -1;
			}
		}
	}
	for (i = 1; i < order; i++) {
		add[i][0] = i;
		adddepth[i][0] = -1;
		for (j = 1; j < order; ++j) {
			addfactor[i][j][i] = -1;
		}
	}
	int a, b;
	for (a = 0; a < order; ++a) {
		for (b = 0; b < order; b++) {
			group_as[0][a][b] = -1;
			group_as[a][0][b] = -1;
			group_as[a][b][0] = -1;
		}
	}

	/*
	struct pair pair;
	pair.left = 1;
	int divisor = 9;
	int q = order / divisor;
	for (int j = 0; j < q; j++) {
		for (int k = 0; k < divisor; k++) {
			pair.right = j*divisor + k;
			group_decide(pair, j*divisor + (k + 1) % divisor, -1);
		}
	}
	*/
}

void get_add(int add_[MAXORDER][MAXORDER]) {
	memcpy(add_, add, sizeof(add));
}

int get_group_as(int a, int b, int c){
	return group_as[a][b][c];
}

void make_group() {
	order_initialize();
	group_initialize();
	group_full_initialize();

	struct pair pair;
	pair.left = 1;
	int divisor,q;
	for (int i = ndivisors-1; i > 0; i--) {
		divisor = divisors[i];
		q = order / divisor;
		for (int j = 0; j < q; j++) {
			for (int k = 0; k < divisor; k++) {
				pair.right = j*divisor + k;
				group_decide(pair, j*divisor + (k + 1) % divisor, 1);
			}
		}
		group_backtrackfc(2);
		group_clean(1);
	}
}

void group_decide(struct pair pair,int factor,int depth) {
	add[pair.left][pair.right] = factor;
	adddepth[pair.left][pair.right] = depth;
	group_factordelete(pair, depth);
}

int group_backtrackfc(int depth) {
//	printf("depth=%d\n", depth);
//	group_printtable('+', add);

	if (table_is_full(adddepth)) {
		//		printf("表が埋まりました。群であるか調べます\n");
		group_full();
		return 0;
	}

	struct pair pair;
	pair = group_nextpair();
	//	printf("pair=(%d,%d)\n", pair.left, pair.right);

	for (int d = 0; d < order; ++d) {
		if (addfactor[pair.left][pair.right][d] != 0) continue;
		adddepth[pair.left][pair.right] = depth;
		add[pair.left][pair.right] = d;
		group_factordelete(pair, depth);
		//		printf("depth=%d d=%d\n", depth, d);
		//		group_printtable('+', add);
				/*確定、確認*/
		if (group_lf(pair, depth) == true) {
			//		printf("矛盾が出ませんでした さらに仮定します\n\n");
			//		printf("depth = %d\n", depth);
			//		printf("pair=(%d,%d)\n", pair.left, pair.right);
			//		printf("d=%d\n", d);
			group_backtrackfc(depth + 1);
		}
		else {
			//			printf("矛盾が出ました 次の仮定へ行きます\n\n");
		}

		group_clean(depth);
	}

	return 0;
}

int group_printtable(char mark, int table[MAXORDER][MAXORDER]) {
	int x, y;
	order = get_order();

	printf("%c_", mark);
	for (y = 0; y < order; y++)
		printf("_%d", y);
	printf("\n");
	for (x = 0; x < order; x++) {
		printf("%d|", x);
		for (y = 0; y < order; y++) {
			if (adddepth[x][y] == 0) {
				printf("  ");
			} else {
				printf(" %d", table[x][y]);
			}
		}
		printf("\n");
	}
	printf("\n");
	return 0;
}

void group_clean(int depth) {
	int a, b, c;
	for (a = 1; a <= nfactors; ++a) {
		for (b = 1; b <= nfactors; ++b) {
			if (adddepth[a][b] == depth)
				adddepth[a][b] = 0;
			for (c = 0; c <= nfactors; ++c) {
				if (addfactor[a][b][c] == depth)
					addfactor[a][b][c] = 0;
			}
		}
	}
	for (a = 1; a <= nfactors; ++a) {
		for (b = 1; b <= nfactors; ++b) {
			for (c = 1; c <= nfactors; ++c) {
				if (group_as[a][b][c] == depth)
					group_as[a][b][c] = 0;
			}
		}
	}
}

void group_factordelete(struct pair newpair, int depth) {
	int left = newpair.left, right = newpair.right;
	int factor = add[left][right];
	for (int j = 1; j < order; ++j) {
		if (j != right) {
			if (addfactor[left][j][factor] == 0) {
				addfactor[left][j][factor] = depth;
			}
		}
	}
	for (int i = 1; i < order; ++i) {
		if (i != left) {
			if (addfactor[i][right][factor] == 0) {
				addfactor[i][right][factor] = depth;
			}
		}
	}
}

int group_lf(struct pair pair, int depth) {
//	printf("lf pair=(%d,%d)\n", pair.left, pair.right);
//	group_printtable('+', add);

	int change;
	struct pair newpair;
	int x, a, b, c, abc;
	while (true) {
		change = false;

		rowcolrenew();
//		printtable('r', addrowfactor);
//		printtable('c', addcolfactor);
		int i, j;
		int f = add[pair.left][pair.right];
		for (i = 1; i <= nfactors; i++){
			if (addrowfactor[i][f] == 0) {
//				printf("%d行目に%dが入りません\n", i, f);
				return false;
			}
			if (addrowfactor[i][f] == 1) {
				for (j = 1; j <= nfactors; j++){
					if (adddepth[i][j] == 0) {
						if (addfactor[i][j][f] == 0) {
							newpair.left = i;
							newpair.right = j;
							break;
						}
					}
				}
				if (change == false) change = true;
				adddepth[newpair.left][newpair.right] = depth;
				add[newpair.left][newpair.right] = f;
				group_factordelete(newpair, depth);
				if (group_lf(newpair, depth) == false)
					return false;
			}
		}
		for (j = 1; j <= nfactors; j++) {
			if (addcolfactor[j][f] == 0) {
//				printf("%d列目に%dが入りません\n", j, f);
				return false;
			}
			if (addcolfactor[j][f] == 1) {
				for (i = 1; i <= nfactors; i++) {
					if (adddepth[i][j] == 0) {
						if (addfactor[i][j][f] == 0) {
							newpair.left = i;
							newpair.right = j;
						}
					}
				}
				if (change == false) change = true;
				adddepth[newpair.left][newpair.right] = depth;
				add[newpair.left][newpair.right] = f;
				group_factordelete(newpair, depth);
				if (group_lf(newpair, depth) == false)
					return false;
			}
		}

		for (x = 1; x <= nfactors; ++x) {
			/*associative (ab)c=a(bc) */
			a = pair.left;
			b = pair.right;
			c = x;
			if (adddepth[b][c] != 0) {
				if (adddepth[a][add[b][c]] != 0) {
					newpair.left = add[a][b];
					newpair.right = c;
					abc = add[a][add[b][c]];
					if (adddepth[newpair.left][newpair.right] == 0) {
						if (addfactor[newpair.left][newpair.right][abc] == 0) {
							if (change == false) change = true;
							adddepth[newpair.left][newpair.right] = depth;
							add[newpair.left][newpair.right] = abc;
							group_factordelete(newpair, depth);
							group_as[a][b][c] = depth;
							if (group_lf(newpair, depth) == false)
								return false;
						}
						else {
	//						printf("a結合律(%d%d)%d=%d(%d%d)で%d%dを求めましたが、候補にありません\n", a, b, c, a, b, c, newpair.left, newpair.right);
							return false;
						}
					}
					else {
						if (add[newpair.left][newpair.right] == abc) {
							group_as[a][b][c] = depth;
						}
						else {
//							printf("結合律(%d%d)%d=%d(%d%d)が成り立ちません\n", a, b, c, a, b, c);
							return false;
						}
					}
				}
				if (adddepth[add[a][b]][c] != 0) {
					newpair.left = a;
					newpair.right = add[b][c];
					abc = add[add[a][b]][c];
					if (adddepth[newpair.left][newpair.right] == 0) {
						if (addfactor[newpair.left][newpair.right][abc] == 0) {
							if (change == false) change = true;
							adddepth[newpair.left][newpair.right] = depth;
							add[newpair.left][newpair.right] = abc;
							group_factordelete(newpair, depth);
							group_as[a][b][c] = depth;
							if (group_lf(newpair, depth) == false) return false;
						}
						else {
//							printf("b結合律(%d%d)%d=%d(%d%d)で%d%dを求めましたが、候補にありません\n", a, b, c, a, b, c, newpair.left, newpair.right);
							return false;
						}
					}
				}
			}
			a = x;
			b = pair.left;
			c = pair.right;
			if (adddepth[a][b] != 0) {
				if (adddepth[add[a][b]][c] != 0) {
					newpair.left = a;
					newpair.right = add[b][c];
					abc = add[add[a][b]][c];
					if (adddepth[newpair.left][newpair.right] == 0) {
						if (addfactor[newpair.left][newpair.right][abc] == 0) {
							if (change == false) change = true;
							adddepth[newpair.left][newpair.right] = depth;
							add[newpair.left][newpair.right] = abc;
							group_factordelete(newpair, depth);
							group_as[a][b][c] = depth;
							if (group_lf(newpair, depth) == false) return false;
						}
						else {
//							printf("結合律(%d%d)%d=%d(%d%d)で%d%dを求めましたが、候補にありません\n", a, b, c, a, b, c, newpair.left, newpair.right);
							return false;
						}
					} else {
						if (add[newpair.left][newpair.right] == abc) {
							group_as[a][b][c] = depth;
						}
						else {
//							printf("結合律(%d%d)%d=%d(%d%d)が成り立ちません\n", a, b, c, a, b, c);
							return false;
						}
					}
				}
				if (adddepth[a][add[b][c]] != 0) {
					newpair.left = add[a][b];
					newpair.right = c;
					abc = add[a][add[b][c]];
					if (adddepth[newpair.left][newpair.right] == 0) {
						if (addfactor[newpair.left][newpair.right][abc] == 0) {
							adddepth[newpair.left][newpair.right] = depth;
							add[newpair.left][newpair.right] = abc;
							group_factordelete(newpair, depth);
							group_as[a][b][c] = depth;
							if (group_lf(newpair, depth) == false)
								return false;
						}
						else {
							return false;
						}
					}
				}
			}

		}
		if (change == false) break;
	}
	return true;
}

int group_nrest(struct pair pair) {
	int count = 0;
	for (int i = 0; i < order; ++i) {
		if (addfactor[pair.left][pair.right][i] == 0) {
			count++;
		}
	}
	return count;
}

struct pair group_nextpair() {
	int left, right;
	struct pair pair;
	int min = order;
	struct pair minpair;
	for (left = 1; left <= nfactors; ++left) {
		for (right = 1; right <= nfactors; ++right) {
			if (adddepth[left][right] == 0) {
				pair.left = left;
				pair.right = right;
				if (group_nrest(pair) < min) {
					min = group_nrest(pair);
					minpair = pair;
				}
			}
		}
	}
	return minpair;
}