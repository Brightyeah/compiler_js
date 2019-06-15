#include "table.h"

struct table *table_init(char *identif, int level) {
  	struct table *tb_cur;
  	tb_cur = (struct table*)malloc(sizeof(struct table));
  	tb_cur->level = level;
  	strcpy(tb_cur->identif, identif);
  	tb_cur->next = NULL;
  	return (tb_cur);
}
struct table *table_add_elem(struct table *tb_cur, char *identif, int level) {
	struct table *temp, *p;
  	temp = (struct table*)malloc(sizeof(struct table));
  	p = tb_cur->next;
  	tb_cur->next = temp;
  	temp->level = level;
  	strcpy(temp->identif, identif);
  	temp->next = p;
  	return (temp);
}
struct table *table_delete_elem(struct table *root, int level) {
  	struct table *temp, *temp_root;
  	temp_root = root; temp = root;
  	for (;temp_root->level == level; temp_root = temp) { //delete root
  		temp = temp_root->next;
  		free(temp_root);
  	}
  	for (;temp->next != NULL; temp = temp->next) {
  		if (temp->next->level == level) {
	   		temp->next = temp->next->next;
	   		//free(temp->next);
	   	}
  	}
  	return (temp);
}
void table_print(struct table *root) {
  	struct table *p;
  	p = root;
  	for (; p != NULL; p = p->next)
	   	printf("l%d `%s`\n", p->level, p->identif);
}

int cmp_lt(char list[100], char tb[100]) {
	for (int i = 0; i < 100; i++) {
		if (list[i] == tb[i]) {
			if (list[i] == '\0' && tb[i] == '\0') return 0;
		} else return 1;
	}
	return 1;
}

//TABLE
struct table *table_symbols(struct list *list_head) {
	struct list *list_parser;
	list_parser = list_head; 
	struct table *tb = NULL, *tb_root = NULL;
	int level = 0, tok_var_prev = 0, global_flag_have = 0;
	for (int init = 0; list_parser != NULL; list_parser = list_parser->ptr) {
		int tb_tok_num = list_parser->tok_num;
		if (tb_tok_num == tok_id) {
			if (tok_var_prev == tok_var) {
				if (init == 0) {
					tb = table_init(list_parser->data, level);
					tb_root = tb;
					init = 1;
				} else tb = table_add_elem(tb, list_parser->data, level);
			} else if (tok_var_prev == tok_function) {
			} else  {	//first meeting
				int flag_have = 0;
				struct table *tb_have;
				tb_have = tb_root;
				for (; tb_have != NULL; tb_have = tb_have->next) {//ищу токен var
					if ((cmp_lt(list_parser->data, tb_have->identif) == 0) && tb_have->level <= level) {
						flag_have = global_flag_have = 1;
						break;
					}
				}
				if (flag_have == 0)
					printf("1st meet expr: 'var %s'\tLoc=<%d:%d>\n", list_parser->data, list_parser->row, list_parser->col);
			}

		} else if (tb_tok_num == '{') {//добавить сколько именно вхождений
			level++;

		} else if (tb_tok_num == '}') 	{
			if (level == 0) {
				printf("Too many '}' Loc=<%d:%d>\n", list_parser->row, list_parser->col);
				return NULL;
			}
			level--;
		}
		tok_var_prev = tb_tok_num;
	}
	if (level != 0) {
		printf("Too many '{'\n");
		return NULL;
	}
	if (global_flag_have == 0) {
		return NULL;
	}
	return tb_root;
}