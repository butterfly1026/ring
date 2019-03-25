#include "order.h"


static int order;
static int nfactors;

void order_initialize() {
	order = 4;
	nfactors = order - 1;
}

int get_order() {
	return order;
}

int get_nfactors() {
	return nfactors;
}
