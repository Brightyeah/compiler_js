#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

//Tree
struct node {
 	int num;
	char data[100];
  	struct node *left;
  	struct node *right;
};
 
struct tree {
  struct node *root;
  int count;
};

struct tree *tree_create(void);
int insert(struct tree *search_tree, char *item, int number);
int bin_search(const struct tree *search_tree, char *item, int number);
static void walk(const struct node *search_node);
void walk_all(const struct tree *my_tree);

//PARSER
int global_parcer_error;
int parser_prev;
char parser_str_prev[100];

int parser(struct list *list_head, struct tree *tree_parcer);
int parser_body(struct list *list_parser);

#endif