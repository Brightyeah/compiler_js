#include "lexer.h"

//LIST
struct list *list_init(char *tok, int tok_is_num, char *data) {
  	struct list *cur_list;
  	cur_list = (struct list*)malloc(sizeof(struct list));
  	cur_list->col = current_col;
  	cur_list->row = current_row;
  	cur_list->tok_num = tok_is_num;
  	strcpy(cur_list->tok, tok);
  	strcpy(cur_list->data, data);
  	cur_list->ptr = NULL;
  	return (cur_list);
}

struct list *list_addelem(struct list *cur_list, char *tok, int tok_is_num, char *data) {
	struct list *temp, *p;
  	temp = (struct list*)malloc(sizeof(struct list));
  	p = cur_list->ptr;
  	cur_list->ptr = temp;
  	temp->col = current_col;
  	temp->row = current_row;
  	temp->tok_num = tok_is_num;
  	strcpy(temp->tok, tok);
  	strcpy(temp->data, data);
  	temp->ptr = p;
  	return (temp);
}

void list_print(struct list *cur_list, char *file_char_js) {
  	struct list *p;
  	p = cur_list;
  	while (p != NULL) {
  		if (!strcmp(p->tok, "UNDEF\0"))
	   		printf("%s `%s`\t\tLoc=<%s:%d:%d>\n", p->tok, p->data, file_char_js, p->row, p->col);
	    p = p->ptr;
  	}
}

//LEXER
void lexer_fgetc(FILE*file_js) {
	last_char = fgetc(file_js);
	counter_col++;
}

void lexer_isspace(FILE *file_js) {
	while (isspace(last_char)) {
		if (last_char == 10) {
			counter_col = 1;
			counter_row++;
		}
		if (last_char == 9) counter_col += 4;
		if (last_char == 32) counter_col++;
		last_char = fgetc(file_js);
	}
}

int lexer_check_eof() {
	strcpy(what_str, "EOF\0");
	id_str = malloc(sizeof(char));
	id_str[0] = '\0';
	return tok_eof;
}

int lexer_check_id(FILE *file_js) {
	int i = 0;
	id_str = malloc(sizeof(char));
	id_str[i] = last_char;
	lexer_fgetc(file_js);
	for (i = 1; isalpha(last_char) || last_char == 95 || last_char == 36 || isdigit(last_char); i++) {
		id_str = realloc(id_str, (i+1) * sizeof(char));
		id_str[i] = last_char;
		lexer_fgetc(file_js);
	}
	id_str = realloc(id_str, (i+1) * sizeof(char));
	id_str[i] = '\0';
	strcpy(what_str, "ID\0");
	if (!strcmp(id_str, "var\0"))				return tok_var;
	else if (!strcmp(id_str, "const\0"))		return tok_const;
	else if (!strcmp(id_str, "function\0")) 	return tok_function;
	else if (!strcmp(id_str, "if\0")) 			return tok_if;
	else if (!strcmp(id_str, "else\0")) 		return tok_else;
	else if (!strcmp(id_str, "while\0")) 		return tok_while;
	else if (!strcmp(id_str, "return\0")) 		return tok_return;
	else if (!strcmp(id_str, "true\0")) 		return tok_true;
	else if (!strcmp(id_str, "false\0")) 		return tok_false;
	else if (!strcmp(id_str, "alert\0")) 		return tok_alert;
	else 										return tok_id;
}

int lexer_check_num(FILE *file_js) {
	int i = 0, flag_for_dot = 0, flag_undef = 0;
	if (last_char == 46) flag_for_dot = 1;
	id_str = malloc(sizeof(char));
	id_str[i] = last_char;
	lexer_fgetc(file_js);
	for (i = 1; isdigit(last_char) || last_char == 46 || isalpha(last_char); i++) {
		if (isalpha(last_char) || (last_char == 46 && flag_for_dot)) {
			while (isalpha(last_char) || isdigit(last_char) || last_char == 46) {
				id_str = realloc(id_str, (i+1) * sizeof(char));
				id_str[i++] = last_char;
				lexer_fgetc(file_js);
			}
			flag_undef = 1;
			break;
		}
		if (last_char == 46) flag_for_dot = 1;
		if (last_char == 10) break;
		id_str = realloc(id_str, (i+1) * sizeof(char));
		id_str[i] = last_char;
		lexer_fgetc(file_js);
	}
	id_str = realloc(id_str, (i+1) * sizeof(char));
	id_str[i] = '\0';
	if (flag_undef) {
		strcpy(what_str, "UNDEF\0");
		return undef_str;
	}
	strcpy(what_str, "numeric\0");
	if (flag_for_dot == 1) {
		id_numeric_double = strtod(id_str, 0);
		return tok_num_double;
	} else {
		id_numeric_int = strtod(id_str, 0);
		return tok_num_int;
	}
}

int lexer_check_comments(FILE *file_js) {
	lexer_fgetc(file_js);
	if (last_char == 47) {
		do {
			lexer_fgetc(file_js);
		} while (last_char != EOF && last_char != 10); // 10=\n
		if (last_char == 10) lexer_isspace(file_js);
		if (last_char == EOF) return lexer_check_eof();
	} else if (last_char == 42) { // 42=*
		while (1) {
			lexer_fgetc(file_js);
			lexer_isspace(file_js);
			if (last_char == EOF) return lexer_check_eof();
			if (last_char == 42) {
				lexer_fgetc(file_js);
				if (last_char == EOF) return lexer_check_eof();
				else if (last_char == 47) {
					lexer_fgetc(file_js);
					break;
				}
			}
		}
	} else {
		strcpy(what_str, "div\0");
		id_str = malloc(2*sizeof(char));
		id_str[0] = '/';
		id_str[1] = '\0';
		lexer_fgetc(file_js);
		return tok_div;
	}
	return tok_comment;
}

int lexer_check_str_two(FILE *file_js) {
	int i = 0;
	id_str = malloc(sizeof(char));
	id_str[i++] = last_char;
	lexer_fgetc(file_js);
	if (last_char == 34) {
		id_str = realloc(id_str, 2 * sizeof(char));
		id_str[i++] = last_char;
		lexer_fgetc(file_js);
	} else {
		for (i = 1; last_char != 34; i++) {
			id_str = realloc(id_str, (i+1) * sizeof(char));
			id_str[i] = last_char;
			lexer_fgetc(file_js);
		}
		id_str = realloc(id_str, (i+1) * sizeof(char));
		id_str[i++] = last_char;
		lexer_fgetc(file_js);
	}
	id_str = realloc(id_str, (i+1) * sizeof(char));
	id_str[i] = '\0';
	strcpy(what_str, "string\0");
	return tok_string;
}

int lexer_check_str_one(FILE *file_js) {
	int i = 0;
	id_str = malloc(sizeof(char));
	id_str[i++] = last_char;
	lexer_fgetc(file_js);
	if (last_char == 39) {
		id_str = realloc(id_str, (i+1) * sizeof(char));
		id_str[i++] = last_char;
		lexer_fgetc(file_js);
	} else {
		for (i = 1; last_char != 39; i++) {
			id_str = realloc(id_str, (i+1) * sizeof(char));
			id_str[i] = last_char;
			lexer_fgetc(file_js);
		}
		id_str = realloc(id_str, (i+1) * sizeof(char));
		id_str[i++] = last_char;
		lexer_fgetc(file_js);
	}
	id_str = realloc(id_str, (i+1) * sizeof(char));
	id_str[i] = '\0';
	strcpy(what_str, "string\0");
	return tok_string;
}

int lexer_check_ascii(FILE *file_js) {
	id_str = malloc(2*sizeof(char));
	id_str[0] = last_char;
	id_str[1] = '\0';
	if (last_char == 40) strcpy(what_str, "l_parent\0");
	else if (last_char == 41) strcpy(what_str, "r_parent\0");
	else if (last_char == 42) strcpy(what_str, "mul\0");
	else if (last_char == 43) strcpy(what_str, "plus\0");
	else if (last_char == 45) strcpy(what_str, "minus\0");
	else if (last_char == 47) strcpy(what_str, "div\0");
	else if (last_char == 59) strcpy(what_str, "semi\0");
	else if (last_char == 60) strcpy(what_str, "less\0");
	else if (last_char == 61) strcpy(what_str, "equal\0");
	else if (last_char == 62) strcpy(what_str, "more\0");
	else if (last_char == 91) strcpy(what_str, "l_brackets\0");
	else if (last_char == 93) strcpy(what_str, "r_brackets\0");
	else if (last_char == 123) strcpy(what_str, "l_brace\0");
	else if (last_char == 125) strcpy(what_str, "l_brace\0");
	else {
		strcpy(what_str, "UNDEF\0");
		lexer_fgetc(file_js);
		return undef_str;
	}
	int cur_char = last_char;
	lexer_fgetc(file_js);
	return cur_char;
}

int lexer(FILE *file_js) {
	if (init == 0) {
		lexer_fgetc(file_js);
		init = 1;
	}
	lexer_isspace(file_js);
	current_row = counter_row;
	current_col = counter_col;
	if (isalpha(last_char) || last_char == 36 || last_char == 95)   lexer_print = lexer_check_id(file_js);
	else if (isdigit(last_char) || last_char == 46)  				lexer_print = lexer_check_num(file_js);
	else if (last_char == 47)										lexer_print = lexer_check_comments(file_js);
	else if (last_char == 34) 										lexer_print = lexer_check_str_two(file_js);
	else if (last_char == 39)										lexer_print = lexer_check_str_one(file_js);
	else if (last_char == EOF) 										lexer_print = lexer_check_eof();
	else 															lexer_print = lexer_check_ascii(file_js);
	what_str_num = lexer_print;
	return lexer_print;
}