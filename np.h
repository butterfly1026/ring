
#ifndef NP_H_
#define NP_H_

void rowcolsqrrenew();

void np_initialize();

void get_np(int np_[100][100]);

int np_input();

void make_np();

void np_decide(struct pair pair, int factor, int depth);

int np_backtrackfc(int depth);

int np_printtable();

int np_pair_to_sqr(pair pair);
void np_sqr_to_pairs(int sqr, pair pairs[9]);
void np_pair_to_pairs(pair pair, struct pair pairs[9]);

void np_clean(int depth);

void np_factordelete(struct pair newpair, int depth);

int np_lf(struct pair pair, int depth);

int np_nrest(struct pair pair);

struct pair np_nextpair();

#endif