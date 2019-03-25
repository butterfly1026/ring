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
//		printf("�Q�ł����B���łɕ\���������̂Ɠ��^�łȂ������ׂ܂�\n");
		if (group_ifisomorphic_to_stocks() == false) {
//			printf("���^�ł͂���܂���ł����B\n");
			if (group_to_stock() == false) {
				return 0;
			}
			else {
				printtable('+', add);
			}
			return 0;
		}
		else {
//			printf("���^�̂��̂�����܂����B\n");
			return 0;
		}
	}
	else {
		//			printf("�Q�ł͂���܂���ł����B���̉���֍s���܂�\n");
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

int group_ifisomorphic_to_stocks() {//stocks�̒��ɓ��^�Ȃ��̂����邩�ǂ���
	int i;
	for (i = 0; i < group_stock; i++) {
		if (group_ifisomorphic(i)) {
			return true;
		}
	}
	return false;
}

int group_ifisomorphic(int i) { //i�Ԗڂ�stock�Ɠ��^���ǂ���
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
		printf("�Q�̂��߂̃X�g�b�N������Ȃ�������G�G\n");
		return false;
	}
//	printf("addstock[%d]��stock���܂���\n", group_stock);
	memcpy(addstock[group_stock++], add, sizeof(add));
	return true;
}