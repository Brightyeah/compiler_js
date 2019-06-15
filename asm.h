#ifndef ASM_H
#define ASM_H
#include "table.h"

struct reg_list {
	int reg;
  	char data[100];
	struct reg_list *ptr;
};
int rbp;

struct reg_list *reg_list_init(int reg, char *data);
struct reg_list *reg_list_addelem(struct reg_list *cur, int reg, char *data);
int reg_list_find(struct reg_list *root, char *data);
int asm_func(struct list *list_head);

#endif