#include <memory.h>
#include "permutation.h"
#include "order.h"

#define MAXORDER 100

static int nfactors;

void permu_initialize() {
	nfactors = get_nfactors();
}

int factorial(int n) {
	int i;
	int m = 1;
	for (i = 1; i <= n; ++i) {
		m *= i;
	}
	return m;
}

int ascending_sort(int array[MAXORDER - 2], int total) {
	int i, j;
	int tmp;
	for (i = 0; i < total; i++) {
		for (j = i + 1; j < total; j++) {
			if (array[i] > array[j]) {
				tmp = array[i];
				array[i] = array[j];
				array[j] = tmp;
			}
		}
	}
	return 0;
}

int nextpermutation(int f_[MAXORDER - 1], int q) {
	int nextf_[MAXORDER - 1];
	memcpy(nextf_, f_, sizeof(int) * nfactors);

	int n;
	int k;
	int min, kofmin;
	int number;
	int array[MAXORDER - 2];
	int m;
	for (n = 2; n <= nfactors; n++) {
		if (q % n == 0) {
			q /= n;
		}
		else { //f_‚©‚çnextf_‚ğì‚é
			   /*f_[nfactors-n+1]`f_[nfactors-1]‚©‚ç
			   f_[nfactors-n]‚æ‚è‚à‘å‚«‚¢”š‚Ì’†‚Åˆê”Ô¬‚³‚¢”š‚ğ‘I‚ñ‚Ånextf_[order-n]‚É“ü‚ê‚é*/
			min = nfactors + 1; //min © ‡
			for (k = nfactors - (n - 1); k < nfactors; k++) {
				number = f_[k];
				if (number > f_[nfactors - n])
					if (min > number) {
						min = number;
						kofmin = k;
					}
			}
			nextf_[nfactors - n] = min;

			/*f_[nfactors-n]~f_[nfactors-1]‚Ì‚»‚êˆÈŠO‚Ì”š‚ğ¸‡‚É•À‚×‚é*/
			m = 0;
			for (k = nfactors - n; k < nfactors; k++) {
				if (k != kofmin)
					array[m++] = f_[k];
			}
			ascending_sort(array, m);
			memcpy(nextf_ + nfactors - (n - 1), array, sizeof(int) * m);
			break;
		}
	}

	memcpy(f_, nextf_, sizeof(int) * nfactors);

	return 0;
}
