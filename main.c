#include "lexer.h"

int main() {
	//OPEN FILE
	FILE *file_js;
	char file_char_js[] = "main.js";
	printf("\n\tReading file %s ...\n", file_char_js);
	if ((file_js = fopen(file_char_js, "r")) == NULL) {
		printf("\tFile is NOT open.\n");
		return 0;
	} else printf("\tFile is open.\n\n"); 

	//LEXER
	struct list *lexer_list, *lexer_head;
	lexer_list = 0; lexer_head = 0;
	int lexer_flag_eof = 0, flag_undef = 0, init_list = 0;
	counter_col = 0; current_col = 0;
	counter_row = 1; current_row = 1;
	while (lexer_flag_eof != tok_eof) {
		lexer_flag_eof = lexer(file_js);
		if (lexer_flag_eof != tok_comment) {
			if (init_list == 0) {
				lexer_list = list_init(what_str, id_str);
				lexer_head = lexer_list;
				init_list = 1; 
			} else lexer_list = list_addelem(lexer_list, what_str, id_str);
			free(id_str);
		}
		if (lexer_flag_eof == undef_str) flag_undef = 1;
	}
	fclose(file_js);
	if (flag_undef == 1) {
		printf("\tLexer is NOT okey.\n\n");
		list_print(lexer_head, file_char_js);
	} else printf("\tLexer is okey.\n\n");

	return 0;
}