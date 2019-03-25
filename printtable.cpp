#include <stdio.h>
#include "printtable.h"
#include "order.h"

#define MAXORDER 100

static int order;
static int nfactors;

void print_initialize() {
	order = get_order();
	nfactors = order - 1;
}

int printtable(char mark, int table[MAXORDER][MAXORDER]) {
	int x, y;

	printf("%c_", mark);
	for (y = 0; y < order; y++)
		printf("_%d", y);
	printf("\n");
	for (x = 0; x < order; x++) {
		printf("%d|", x);
		for (y = 0; y < order; y++) {
			printf(" %d", table[x][y]);
		}
		printf("\n");
	}
	printf("\n");
	return 0;
}

int table_is_full(int table[MAXORDER][MAXORDER]) {
	int first, second;
	for (first = 1; first <= nfactors; ++first)
		for (second = 1; second <= nfactors; ++second)
			if (table[first][second] == 0)
				return false;
	return true;
}

int printarray(int array[MAXORDER]) {
	int j;
	for (j = 0; j < order; j++) {
		printf(" %d", array[j]);
	}
	printf("\n");
	return 0;
}