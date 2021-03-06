/*
* group_isomorphic.c
*
*  Created on: 2016/05/10
*      Author: boonfly
*/

#include <stdio.h>
#include <memory.h>
#include <stdbool.h>
#include "group.h"
#include "group_isomorphic.h"
#include "order.h"
#include "permutation.h"
#include "printtable.h"

#define MAXORDER 100
#define MAXSTOCK 100

static int order;
static int nfactors;
static int factr;

static int group_stock = 0;
static int addstock[MAXSTOCK][MAXORDER][MAXORDER];

static int add[MAXORDER][MAXORDER];

void group_full_initialize() {
	order = get_order();
	nfactors = order - 1;
	factr = factorial(nfactors);
}

int group_full() {
	get_add(add);
	if (if_group()) {
//		printf("群でした。すでに表示したものと同型でないか調べます\n");
		if (group_ifisomorphic_to_stocks() == false) {
//			printf("同型ではありませんでした。\n");
			if (group_to_stock() == false) {
				return 0;
			}
			else {
				printtable('+', add);
			}
			return 0;
		}
		else {
//			printf("同型のものがありました。\n");
			return 0;
		}
	}
	else {
		//			printf("群ではありませんでした。次の仮定へ行きます\n");
		return 0;
	}
}

int if_group() {
	int a, b, c;
	for (a = 1; a <= nfactors; ++a) {
		for (b = 1; b <= nfactors; ++b) {
			for (c = 1; c <= nfactors; ++c) {
				if (get_group_as(a, b, c) == 0) {
					if (add[a][add[b][c]] != add[add[a][b]][c])
						return false;
				}
			}
		}
	}
	return true;
}

int group_ifisomorphic_to_stocks() {//stocksの中に同型なものがあるかどうか
	int i;
	for (i = 0; i < group_stock; i++) {
		if (group_ifisomorphic(i)) {
			return true;
		}
	}
	return false;
}

int group_ifisomorphic(int i) { //i番目のstockと同型かどうか
	int f_[MAXORDER - 1];
	int j;
	for (j = 0; j < nfactors; j++) {
		f_[j] = j + 1;
	}
	int f__[MAXORDER];
	f__[0] = 0;

	for (j = 1; j < factr; j++) {
		nextpermutation(f_, j);
		memcpy(f__ + 1, f_, sizeof(f_));
		if (group_ifisomorphism(i, f__) == true) {
//			printtable('+', addstock[i]);
			return true;
		}
	}
	return false;
}

int group_ifisomorphism(int i, int f__[MAXORDER]) {
	int a, b;
	for (a = 1; a < order; a++) {
		for (b = 1; b < order; b++) {
			if (f__[add[a][b]] != addstock[i][f__[a]][f__[b]])
				return false;
		}
	}
	return true;
}

int group_to_stock() {
	if (group_stock == MAXSTOCK) {
		printf("群のためのストックが足りなかったよ；；\n");
		return false;
	}
//	printf("addstock[%d]にstockしました\n", group_stock);
	memcpy(addstock[group_stock++], add, sizeof(add));
	return true;
}