#include "lexer.h"
#include "parser.h"
#include "table.h"
#include "asm.h"

int main(int argc, char *argv) {
//OPEN FILE
	FILE *file_js;
	char file_char_js[] = "main.js";
	printf("\tReading file %s ...\n", file_char_js);
	if ((file_js = fopen(file_char_js, "r")) == NULL) {
		printf("\tFile is NOT open.\n");
		return 0;
	} else printf("\tFile is open.\n"); 

//LEXER with list
	struct list *list_lexer, *list_head;
	list_lexer = 0; list_head = 0;
	int lexer_flag_eof = 0, flag_first_list = 0;
	global_lexer_undef = 0; flag_equals = 0;
	counter_col = 0; current_col = 0;
	counter_row = 1; current_row = 1;
	while (lexer_flag_eof != tok_eof) {
		lexer_flag_eof = lexer(file_js);
		if (lexer_flag_eof != tok_comment) {
			if (flag_first_list == 0) {
				list_lexer = list_init(what_str, what_str_num, id_str);
				list_head = list_lexer;
				flag_first_list = 1; 
			} else list_lexer = list_addelem(list_lexer, what_str, what_str_num, id_str);
			free(id_str);
		}
		if (lexer_flag_eof == undef_str) global_lexer_undef = 1;
	}
	fclose(file_js);

	if (global_lexer_undef == 1) {
		list_print(list_head, file_char_js);
		printf("\nLexer is NOT okey.\n\n");
	} else {
		list_print(list_head, file_char_js);
		printf("\n\tLexer is okey.\n");
    	
//PARSER with tree
		struct tree *tree_parser;
		tree_parser = tree_create();
		parser(list_head, tree_parser);
		if (global_parcer_error == 1) {
			printf("\nParser is NOT okey.\n\n");
		} else {
			//walk_all(tree_parser);
			printf("\n\tParser is okey.\n\n");

//TABLE of symbols
			struct table *tb_root;
			tb_root = table_symbols(list_head);
			if (tb_root == NULL) {
				printf("\nTable is NOT okey.\n\n");
			} else {
			 	table_print(tb_root);
				printf("\n\tTable is okey.\n\n");
//ASSEMBLER
				if (asm_func(list_head) == 1) {
					printf("\tAssembler is okey.\n");
				} else{
					printf("Assembler is NOT okey.\n");
				}

			}
		}

	}
	// getchar();getchar();
	return 0;
}