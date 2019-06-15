#ifndef TABLE_H
#define TABLE_H
#include "parser.h"

struct table {
	char identif[100];
	int level;
	struct table *next;
};

struct table *table_init(char *identif, int level);
struct table *table_add_elem(struct table *tb_cur, char *identif, int level);
struct table *table_delete_elem(struct table *root, int level);
void table_print(struct table *root);
int cmp_lt(char list[100], char tb[100]);
struct table *table_symbols(struct list *list_head);
#endif