#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include "permutation.h"
#include "printtable.h"
#include "group.h"
#include "np.h"
#include "order.h"

#define MAXORDER 100
#define MAXNF 100
#define MAXSTOCK 100

static int order;
static int nfactors;

int add[MAXORDER][MAXORDER];
int mul[MAXORDER][MAXORDER] = { { 0 } };
int muldepth[MAXORDER][MAXORDER] = { { 0 } };
int ld[MAXORDER][MAXORDER][MAXORDER] = { { { 0 } } };
int rd[MAXORDER][MAXORDER][MAXORDER] = { { { 0 } } };
int as[MAXORDER][MAXORDER][MAXORDER] = { { { 0 } } };

int f[MAXNF][MAXORDER];
int nf;
int stock = 0;
int mulstock[MAXSTOCK][MAXORDER][MAXORDER];

int printtable(char mark, int table[MAXORDER][MAXORDER]);

int makef(int f[MAXNF][MAXORDER]);
int printarray(int array[MAXORDER]);

int backtrackfc(int);
int mul_is_full();
int if_ring();
int ifisomorphic_to_stocks();
struct pair nextpair();
int lf(struct pair, int);
int clean(int);

int initialize();

int main(void) {
	/*
	order_initialize(&order, &nfactors);
	print_initialize();
	permu_initialize();

	make_np();
	*/
	/*
	make_group();
	return 0;
	*/
	get_add(add);
	printf("用意された可換群をひょうじしまーす\n");
	printtable('+', add);
	/*
	if(if_group){
	printf("まさしく群である\n");
	} else {
	printf("実はこれ群じゃないんですわ\n");
	}
	*/
	nf = makef(f);
	printf("可換群の自己同型写像をひょうじしまーす\n");
	int i;
	for(i = 0; i < nf; i++){
	printarray(f[i]);
	}
	printf("\n");

	printf("環をひょうじしまーす\n");
	initialize();
	backtrackfc(1);
	printf("環は%d個ありました\n",stock);
	return 0;
	
}

int backtrackfc(int depth) {
	if (mul_is_full()) {
		if (if_ring())
			if (ifisomorphic_to_stocks() == false) {
				if (stock == MAXSTOCK) {
					printf("環のためのストックが足りなかったよ；；\n");
					return 0;
				}
				memcpy(mulstock[stock++], mul, sizeof(mul));
				printtable('*', mul);
			}
		return 0;
	}

	struct pair pair;
	pair = nextpair();

	int d;
	for (d = 0; d <= nfactors; ++d) {
		muldepth[pair.left][pair.right] = depth;
		mul[pair.left][pair.right] = d;
		/*確定*/
		if (lf(pair, depth) == false) {
//			printf("矛盾が出ました 次の仮定へ行きます\n\n");
			clean(depth);
			continue;
		}
//		printf("矛盾が出ませんでした さらに仮定します\n\n");
//		if(/*確認をして制約違反がない*/)
		backtrackfc(depth + 1);
		clean(depth);
	}
	return 0;
}

struct pair nextpair() {
	int left, right;
	struct pair next_pair;
	for (left = 1; left <= nfactors; ++left) {
		for (right = 1; right <= nfactors; ++right) {
			if (muldepth[left][right] == 0) {
				next_pair.left = left;
				next_pair.right = right;
				return next_pair;
			}
		}
	}
	printf("エラー＞＜\n");
	next_pair.left = 0;
	next_pair.right = 0;
	return next_pair;
}

int lf(struct pair pair, int depth) {

	int change;
	struct pair newpair;
	int x, a, b, c;

	while (true) {
		change = false;

		for (x = 1; x <= nfactors; ++x) {
			/*left distributive a(b+c)=ab+ac */
			a = pair.left;
			if (pair.right <= x) {
				b = pair.right;
				c = x;
			}
			else {
				b = x;
				c = pair.right;
			}
			if (muldepth[a][x] != 0) {
				newpair.left = a;
				newpair.right = add[b][c];
				if (muldepth[newpair.left][newpair.right] == 0) {
					if (change == false)
						change = true;
					muldepth[newpair.left][newpair.right] = depth;
					mul[newpair.left][newpair.right] = add[mul[a][b]][mul[a][c]];
					ld[a][b][c] = depth;
//					printf("%d*(%d+%d)=%d*%d+%d*%dより確定 mul[%d][%d]←%d\n", a, b, c, a, b, a, c, newpair.left, newpair.right, mul[newpair.left][newpair.right]);
					if (lf(newpair, depth) == false)
						return false;
				}
				else {
					if (mul[newpair.left][newpair.right] == add[mul[a][b]][mul[a][c]]) {
						ld[a][b][c] = depth;
					}
					else {
						return false;
					}
				}
			}

			/*right distributive (a+b)c=ac+bc */
			if (pair.left <= x) {
				a = pair.left;
				b = x;
			}
			else {
				a = x;
				b = pair.left;
			}
			c = pair.right;
			if (muldepth[x][c] != 0) {
				newpair.left = add[a][b];
				newpair.right = c;
				if (muldepth[newpair.left][newpair.right] == 0) {
					if (change == false)
						change = true;
					muldepth[newpair.left][newpair.right] = depth;
					mul[newpair.left][newpair.right] = add[mul[a][c]][mul[b][c]];
					rd[a][b][c] = depth;
					if (lf(newpair, depth) == false)
						return false;
				}
				else {
					if (mul[newpair.left][newpair.right] == add[mul[a][c]][mul[b][c]]) {
						rd[a][b][c] = depth;
					}
					else {
						return false;
					}
				}
			}

			/*associative (ab)c=a(bc) */
			a = pair.left;
			b = pair.right;
			c = x;
			if (muldepth[b][c] != 0) {
				if (muldepth[a][mul[b][c]] != 0) {
					newpair.left = mul[a][b];
					newpair.right = c;
					if (muldepth[newpair.left][newpair.right] == 0) {
						if (change == false) change = true;
						muldepth[newpair.left][newpair.right] = depth;
						mul[newpair.left][newpair.right] = mul[a][mul[b][c]];
						as[a][b][c] = depth;
						if (lf(newpair, depth) == false)
							return false;
					}
					else {
						if (mul[newpair.left][newpair.right] == mul[a][mul[b][c]]) {
							as[a][b][c] = depth;
						}
						else {
							return false;
						}
					}
				}
				if (muldepth[mul[a][b]][c] != 0) {
					newpair.left = a;
					newpair.right = mul[b][c];
					if (muldepth[newpair.left][newpair.right] == 0) {
						if (change == false) change = true;
						muldepth[newpair.left][newpair.right] = depth;
						mul[newpair.left][newpair.right] = mul[mul[a][b]][c];
						as[a][b][c] = depth;
						if (lf(newpair, depth) == false)
							return false;
					}
				}
			}
			a = x;
			b = pair.left;
			c = pair.right;
			if (muldepth[a][b] != 0) {
				if (muldepth[mul[a][b]][c] != 0) {
					newpair.left = a;
					newpair.right = mul[b][c];
					if (muldepth[newpair.left][newpair.right] == 0) {
						if (change == false)
							change = true;
						muldepth[newpair.left][newpair.right] = depth;
						mul[newpair.left][newpair.right] = mul[mul[a][b]][c];
						as[a][b][c] = depth;
						if (lf(newpair, depth) == false)
							return false;
					}
					else {
						if (mul[newpair.left][newpair.right] == mul[mul[a][b]][c]) {
							as[a][b][c] = depth;
						}
						else {
							return false;
						}
					}
				}
				if (muldepth[a][mul[b][c]] != 0) {
					newpair.left = mul[a][b];
					newpair.right = c;
					if (muldepth[newpair.left][newpair.right] == 0) {
						muldepth[newpair.left][newpair.right] = depth;
						mul[newpair.left][newpair.right] = mul[a][mul[b][c]];
						as[a][b][c] = depth;
						if (lf(newpair, depth) == false)
							return false;
					}
				}
			}
		}

		if (change == false)
			break;
	}
	return true;
}


int ifisomorphism_on_add(int f__[MAXORDER]) {

	int a, b;
	for (a = 1; a < order; a++) {
		for (b = 1; b < order; b++) {
			if (f__[add[a][b]] != add[f__[a]][f__[b]])
				return false;
		}
	}
	return true;
}

int makef(int f[MAXNF][MAXORDER]) {

	int f_[MAXORDER - 1];
	int i;
	for (i = 0; i < nfactors; i++) {
		f_[i] = i + 1;
	}
	int f__[MAXORDER];
	f__[0] = 0;

	int nf = 0;

	for (i = 1; i < factorial(nfactors); i++) {
		nextpermutation(f_, i);
		memcpy(f__ + 1, f_, sizeof(f_));
		if (ifisomorphism_on_add(f__) == true)
			memcpy(f[nf++], f__, sizeof(f__));
	}

	return nf;
}



int initialize() {
	int a,b;
	for (a = 0; a <= nfactors; ++a) {
		muldepth[0][a] = -1;
		muldepth[a][0] = -1;
		ld[0][0][a] = -1;
		ld[a][0][0] = -1;
		rd[0][0][a] = -1;
		rd[0][a][0] = -1;
		for (b = 0; b <= nfactors; b++)
		{
			as[0][a][b] = -1;
			as[a][0][b] = -1;
			as[a][b][0] = -1;
		}
	}
	return 0;
}



int mul_is_full() {
	int first, second;
	for (first = 1; first <= nfactors; ++first)
		for (second = 1; second <= nfactors; ++second)
			if (muldepth[first][second] == 0)
				return false;
	return true;
}

int if_ring() {

	int a, b, c;
	for (a = 1; a <= nfactors; a++)
		for (b = 1; b <= nfactors; b++)
			for (c = 1; c <= nfactors; c++) {
				if (b <= c)
					if (ld[a][b][c] == 0)
						if (mul[a][add[b][c]] != add[mul[a][b]][mul[a][c]]) // left distributive
							return false;
				if (a <= b)
					if (rd[a][b][c] == 0)
						if (mul[add[a][b]][c] != add[mul[a][c]][mul[b][c]]) // right distributive
							return false;
				if (as[a][b][c] == 0)
					if (mul[a][mul[b][c]] != mul[mul[a][b]][c]) // associative
						return false;
			}
	return true;
}

int ifisomorphism(int i, int j) {
	int a, b;
	for (a = 1; a < order; a++) {
		for (b = 1; b < order; b++) {
			if (f[j][mul[a][b]] != mulstock[i][f[j][a]][f[j][b]])
				return false;
		}
	}
	return true;
}

int ifisomorphic(int i) {
	int j;
	for (j = 0; j < nf; j++) {
		if (ifisomorphism(i, j))
			return true;
	}
	return false;
}

int ifisomorphic_to_stocks() {
	int i;
	for (i = 0; i < stock; i++) {
		if (ifisomorphic(i)) {
			return true;
		}
	}
	return false;
}

int clean(int depth) {
	int a, b, c;
	for (a = 1; a <= nfactors; ++a) {
		for (b = 1; b <= nfactors; ++b) {
			if (muldepth[a][b] == depth)
				muldepth[a][b] = 0;
		}
	}
	for (a = 1; a <= nfactors; ++a) {
		for (b = 1; b <= nfactors; ++b) {
			for (c = 1; c <= nfactors; ++c) {
				if (ld[a][b][c] == depth)
					ld[a][b][c] = 0;
				if (rd[a][b][c] == depth)
					rd[a][b][c] = 0;
				if (as[a][b][c] == depth)
					as[a][b][c] = 0;
			}
		}
	}
	return 0;
}