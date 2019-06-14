#include "parser.h"

//Tree
struct tree *tree_create(void) {
	struct tree *new_tree = malloc(sizeof *new_tree);
    if (new_tree == NULL) return NULL;
    new_tree->root = NULL;
    new_tree->count = 0;
    return new_tree;
}
int insert(struct tree *search_tree, char *item, int number) {
    struct node *search_node, **new;
    new = &search_tree->root;
    search_node = search_tree->root;
    for (;;) {
        if (search_node == NULL) {
            search_node = *new = malloc(sizeof * search_node);
            if (search_node != NULL) {
            	strcpy(search_node->data, item);
            	search_node->num = number;
                search_node->left = search_node->right = NULL;
                search_tree->count++;
                return 1;
            } else return 0;
        } 
        else if (search_node->num == number) return 2;
		else if (number > search_node->num) {
			new = &search_node->right;
            search_node = search_node->right;
        } else {
            new = &search_node->left;
            search_node = search_node->left;
        }
    }
}
int bin_search(const struct tree *search_tree, char *item, int number) {
  	const struct node *search_node;
 	search_node = search_tree->root;
    for(;;) {
        if (search_node == NULL) return 0;
        else if (search_node->num == number) return 1;
        else if (number > search_node->num) search_node = search_node->right;
        else search_node = search_node->left;  
    }
}
static void walk(const struct node *search_node) {
    if(search_node == NULL) return;
    walk(search_node->left);
    printf("%s \n", search_node->data);
    walk(search_node->right);
}
void walk_all(const struct tree *my_tree) {
    walk(my_tree->root);
}

//PARSER
int parser(struct list *list_head, struct tree *tree_parser) {
	struct list *list_parser;
	list_parser = list_head;
	global_parcer_error = 0;
	parser_prev = 0;
	parser_str_prev[0] = '\0';
	for (int flag_undef = 0, i = 0; list_parser != NULL; i++) {
		flag_undef = parser_body(list_parser);
		insert(tree_parser, list_parser->data, i);
		list_parser = list_parser->ptr;
	}
	//если нет ошибок пройтись еще круг для списка с var this;
	return 0;
}
int id_this_var, id_this_const;
int parser_body(struct list *list_parser) {
	int pnum = list_parser->tok_num;
	int parser_col = list_parser->col;
	int parser_row = list_parser->row;
	int error_parser = 0;

	if (pnum == tok_var) {
		if (parser_prev == 40 || parser_prev == 41 || parser_prev == 44 || parser_prev == 59 
			|| parser_prev == 123 || parser_prev == 125 || parser_prev == 0) {}// ( ) ; { } ,
		else error_parser = 1;

	} else if (pnum == tok_id) {
		if (parser_prev == tok_var || parser_prev == 40 || parser_prev == 43 || parser_prev == 44 || parser_prev == 45 
			  || parser_prev == 59 || parser_prev == 60 || parser_prev == 61 
			  || parser_prev == 62 || parser_prev == 123|| parser_prev == 125) {}// + ( ; < = > { ,
		else error_parser = 1;

	} else if (pnum == tok_if || pnum == tok_while || pnum == tok_alert || pnum == tok_return) {
		if (parser_prev == 41 || parser_prev == 59 || parser_prev == 123 
			|| parser_prev == 125 || parser_prev == 0) {}// ) ; {}
		else error_parser = 1;

	} else if (pnum == tok_function) {
		if (parser_prev == 41 || parser_prev == 59 || parser_prev == 125 || parser_prev == 0) {}// ) ; }
		else error_parser = 1;

	} else if (pnum == tok_else) {
		if (parser_prev == tok_if || parser_prev == 59 || parser_prev == 125) {}
		else error_parser = 1;

	} else if (pnum == tok_string) {
		if (parser_prev == 40 || parser_prev == 61) {}// ( =
		else error_parser = 1;
		
	} else if (pnum == tok_num_int || pnum == tok_num_double) {
		if (parser_prev == 40 || parser_prev == 42 || parser_prev == 43
			|| parser_prev == 45 || parser_prev == 47 || parser_prev == 60
			|| parser_prev == 61 || parser_prev == 62) {}
		else error_parser = 1;

	} else if (pnum == 40) { // (
		if (parser_prev == tok_if || parser_prev == tok_else || parser_prev == tok_while
			|| parser_prev == tok_alert || parser_prev == tok_function || parser_prev == tok_return) {}
		else error_parser = 1;

	} else if (pnum == 41) { // )
		if (parser_prev == tok_id || parser_prev == tok_string || parser_prev == tok_num_int
			|| parser_prev == tok_num_double || parser_prev == tok_true || parser_prev == tok_false 
			|| parser_prev == 40 || parser_prev == 41 || parser_prev == 93) {}// ( ) ]
		else error_parser = 1;

	} else if (pnum == 42 || pnum == 43 || pnum == 45 || pnum == 47 
		|| pnum == 59 || pnum == 60 || pnum == 62) {// * + - / ; < >
		if (parser_prev == tok_id || parser_prev == tok_num_int || parser_prev == tok_num_double
			|| parser_prev == 41 || parser_prev == 93) {}// ) ]
		else error_parser = 1;

	} else if (pnum == 44) { // ,
		if (parser_prev == tok_id || parser_prev == tok_num_int || parser_prev == tok_num_double) {}
		else error_parser = 1;

	} else if (pnum == 61) { // =
		if (parser_prev == tok_id || parser_prev == 60 || parser_prev == 62) {}// < >
		else error_parser = 1;

	} else if (pnum == 91) { // [
		if (parser_prev == tok_id) {}
		else error_parser = 1;
		
	} else if (pnum == 93) { // ]
		if (parser_prev == tok_id || parser_prev == tok_num_int) {}
		else error_parser = 1;
		
	} else if (pnum == 123) { // {
		if (parser_prev == 41) {}
		else error_parser = 1;

	} else if (pnum == 125) { // }
		if (parser_prev == 59 || parser_prev == 123) {}
		else error_parser = 1;
	
	} else {}

	if (error_parser == 1) {
		global_parcer_error = 1;
		// printf("\nERR `%d %d`\tLoc=<%d:%d>", parser_prev, pnum, parser_row, parser_col);
		printf("\nERR `%s %s`\tLoc=<%d:%d>", parser_str_prev, list_parser->data, parser_row, parser_col);
	}
	parser_prev = pnum;
	strcpy(parser_str_prev, list_parser->data);
	return pnum;
}