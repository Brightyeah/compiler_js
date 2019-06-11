#include "parser.h"

//PARSER
int parser(struct list *list_head) {
	struct list *list_parser;
	list_parser = list_head;
	global_parcer_error = 0;
	parser_prev = 0;
	parser_str_prev[0] = '\0';
	for (int flag_undef = 0; list_parser != NULL;) {
		flag_undef = parser_body(list_parser);
		list_parser = list_parser->ptr;
	}
	return 0;
}

int parser_body(struct list *list_parser) {
	int pnum = list_parser->tok_num;
	int parser_col = list_parser->col;
	int parser_row = list_parser->row;
	int error_parser = 0;

	if (pnum == tok_var || pnum == tok_const) {
		if (parser_prev == 40 || parser_prev == 41 || parser_prev == 59 
			|| parser_prev == 123 || parser_prev == 125 || parser_prev == 0) {}// ( ) ; { }
		else error_parser = 1;

	} else if (pnum == tok_id) {
		if (parser_prev == tok_var || parser_prev == tok_const || parser_prev == 40
			|| parser_prev == 43 || parser_prev == 45 || parser_prev == 59 || parser_prev == 60
			|| parser_prev == 61 || parser_prev == 62 || parser_prev == 123) {}// + ( ; < = > {
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