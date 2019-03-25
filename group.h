#ifndef GROUP_H_
#define GROUP_H_

void rowcolrenew();

int make_divisors();

void group_initialize();

void get_add(int add_[100][100]);

int get_group_as(int a, int b, int c);

int add_is_full();

void make_group();

int group_backtrackfc(int depth);

void group_clean(int depth);

void group_factordelete(struct pair newpair, int depth);

int group_lf(struct pair pair, int depth);

int group_nrest(struct pair pair);

struct pair group_nextpair();

int group_printtable(char mark, int table[100][100]);

void group_decide(struct pair pair, int factor, int depth);

#endif /* GROUP_H_ */
