#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <malloc.h>

enum TOKENS {
	tok_comment = 0,
	tok_eof = -1,
	tok_var = -2,
	tok_while = -4,
	tok_function = -5,
	tok_alert = -6,
	tok_if = -7,
	tok_else = -8,
	tok_return = -9,
	//tok_const = -10,
	tok_true = -12,
	tok_false = -13,

	tok_div = -20,
	tok_id = -100,// ID, str and NUMBER
	tok_string = -101,
	tok_num_double = -102,
	tok_num_int = -103,
	undef_str = -200,
};

int global_lexer_undef;
int init;
int counter_col, current_col, counter_row, current_row;
int lexer_print; //для вывода
char last_char;	//для считывания
char *id_str, what_str[30]; //строка токена, какой токен
int what_str_num;
int id_numeric_int, id_numeric_double;
int flag_equals;

//LIST
struct list {
	int col;
	int row;
	int tok_num;
	char tok[30];  //what_str
  	char data[100]; //id_str
	struct list *ptr;
} ;
struct list *list_init(char *tok, int tok_is_num, char *data);
struct list *list_addelem(struct list *cur_list, char *tok, int tok_is_num, char *data);
void list_print(struct list *cur_list, char *file_char_js);

//LEXER
void lexer_fgetc(FILE*file_js);
void lexer_isspace(FILE *file_js);
int lexer_check_eof();
int lexer_check_id(FILE *file_js);
int lexer_check_num(FILE *file_js);
int lexer_check_comments(FILE *file_js);
int lexer_check_str_two(FILE *file_js);
int lexer_check_str_one(FILE *file_js);
int lexer_check_ascii(FILE *file_js);
int lexer(FILE *file_js);
#endif