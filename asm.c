#include "asm.h"

//LIST ASM REG
struct reg_list *reg_list_init(int reg, char *data) {
  	struct reg_list *cur;
  	cur = (struct reg_list*)malloc(sizeof(struct reg_list));
  	cur->reg = reg;
  	strcpy(cur->data, data);
  	cur->ptr = NULL;
  	return (cur);
}

struct reg_list *reg_list_addelem(struct reg_list *cur, int reg, char *data) {
	struct reg_list *temp, *p;
  	temp = (struct reg_list*)malloc(sizeof(struct reg_list));
  	p = cur->ptr;
  	cur->ptr = temp;
  	temp->reg = reg;
  	strcpy(temp->data, data);
  	temp->ptr = p;
  	return (temp);
}

int reg_list_find(struct reg_list *root, char *data) {
  	struct reg_list *cur;
  	cur = root;
  	for (;cur != NULL; cur = cur->ptr)
  		if (strcmp(cur->data, data) == 0) return cur->reg;
  	return 0;
}

//ASSEMBLER translate
int asm_func(struct list *list_head) {
  	FILE *fasm, *fasmlc;
	if ((fasm = fopen("_asm.s", "w+")) == NULL) {
    	printf("Not open file _asm.s!\n"); return 0;
  	}
  	if ((fasmlc = fopen("_asmlc.s", "w+")) == NULL) {
    	printf("Not open file _asmlc.s!\n"); return 0;
  	}

  	fprintf(fasm, ".data\n");
  	fprintf(fasm, ".text\n");
  	fprintf(fasm, ".global main\n");
  	fprintf(fasm, "main:\n");
  	fprintf(fasm, "\t\tpushq\t%%rbp\n");
  	fprintf(fasm, "\t\tmovq\t%%rsp, %%rbp\n");
  	fprintf(fasm, "\t\tsubq\t$16, %%rsp\n");

  	struct reg_list *rlst, *rlst_root;
  	char asm_str_prev[100];
  	asm_str_prev[0] = '\0';
	struct list *lst;
	lst = list_head;
	int reg_find = 0, flag_lbrace = 0;
	rbp = 0;
	for (int lc = 0, l = 0, init = 0; lst != NULL; lst = lst->ptr) {
// TOK VAR
		if (lst->tok_num == tok_var) { //var a = 2|"a";
			lst = lst->ptr->ptr->ptr;
			if (lst->tok_num == tok_num_int) {
				rbp += 4;
				fprintf(fasm, "\t\tmovl\t$%s, -%d(%rbp)\n", lst->data, rbp);
			} else if (lst->tok_num == tok_string) {
				fprintf(fasmlc, ".LC%d:\n", lc);
				fprintf(fasmlc, "\t\tstring %s\n", lst->data);
				if (rbp % 8 == 0) rbp += 8;
				else rbp += 12;
				fprintf(fasm, "\t\tmovq\t$.LC%d, -%d(%rbp)\n", lc++, rbp);
			}
			if (init == 0) {
  				rlst = reg_list_init(rbp, lst->data);
  				rlst_root = rlst;
  				init = 1;
			} else rlst = reg_list_addelem(rlst, rbp, lst->data);
			lst = lst->ptr;
// TOK EQUAL
		} else if (lst->tok_num == tok_id) {
			reg_find = reg_list_find(rlst_root, lst->data);
			if (strcmp(lst->ptr->data, "(\0") == 0) {
				//это вызов
				int reg_find_2 = reg_list_find(rlst_root, lst->ptr->ptr->data);
				fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
				fprintf(fasm, "\t\tmovl\t%%eax, %%edi\n");
				fprintf(fasm, "\t\tcall\t%s\n", lst->data);
				lst = lst->ptr->ptr->ptr->ptr;
			} else {
				lst = lst->ptr;
				if (lst->tok_num == tok_num_int) {
					int ff = atoi(lst->data);
					fprintf(fasm, "\t\tmovl\t$%d, -%d(%%rbp)\n", ff, reg_find);
				} else if (lst->tok_num == tok_id) { 
					if (strcmp(lst->ptr->data, ";\0") == 0) {
						int reg_find_2 = reg_list_find(rlst_root, lst->data);
						fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
						fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
					} else if (strcmp(lst->ptr->data, "+\0") == 0) {
						if (lst->ptr->ptr->tok_num == tok_num_int) {
							int reg_find_2 = reg_list_find(rlst_root, lst->data);
							lst = lst->ptr->ptr;
							int fff = atoi(lst->data);
							if (reg_find != reg_find_2) {
								fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
								fprintf(fasm, "\t\taddl\t$%d, %%eax\n", fff);
								fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
							} else {
								fprintf(fasm, "\t\taddl\t$%d, -%d(%%rbp)\n", fff, reg_find);
							}
						}
					} else if (strcmp(lst->ptr->data, "-\0") == 0) {
						if (lst->ptr->ptr->tok_num == tok_num_int) {
							int reg_find_2 = reg_list_find(rlst_root, lst->data);
							lst = lst->ptr->ptr;
							int fff = atoi(lst->data);
							if (reg_find != reg_find_2) {
								fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
								fprintf(fasm, "\t\tsubl\t$%d, %%eax\n", fff);
								fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
							} else {
								fprintf(fasm, "\t\tsubl\t$%d, -%d(%%rbp)\n", fff, reg_find);
							}
						}
					} else if (strcmp(lst->ptr->data, "*\0") == 0) {
					} else if (strcmp(lst->ptr->data, "/\0") == 0) {}

				} else if (lst->tok_num == tok_string) {
					fprintf(fasmlc, ".LC%d:\n", lc);
					fprintf(fasmlc, "\t\tstring %s\n", lst->data);
					fprintf(fasm, "\t\tmovq\t$.LC%d, -%d(%%rbp)\n", lc++, reg_find);
				}
				lst = lst->ptr;
			}
// TOK ALERT
		} else if (lst->tok_num == tok_alert) {
			lst = lst->ptr->ptr;
			if (lst->tok_num == tok_string) {
				fprintf(fasmlc, ".LC%d:\n", lc);
				fprintf(fasmlc, "\t\t.string %s\n", lst->data);
				fprintf(fasm, "\t\tmovl\t$.LC%d, %%edi\n", lc++);
				fprintf(fasm, "\t\tmovl\t$0, %%edi\n");
				fprintf(fasm, "\t\tcall\tputs\n");
			} else if (lst->tok_num == tok_id) {
				reg_find = reg_list_find(rlst_root, lst->data);
				fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find);
				fprintf(fasm, "\t\tmovl\t%%eax, %%esi\n");
				fprintf(fasm, "\t\tmovl\t$.LC0, %%edi\n");//xx
				fprintf(fasm, "\t\tcall\tprintf\n");
			}
			lst = lst->ptr->ptr;
// TOK IF
		} else if (lst->tok_num == tok_if) {//if (a <|> 2) {}
			struct list *lst_2;
			lst_2 = lst->ptr->ptr;
			int less_more = 0;
			if (lst_2->tok_num == tok_id) {
				reg_find = reg_list_find(rlst_root, lst_2->data);
				lst_2 = lst_2->ptr;
				if (strcmp(lst_2->data, "<\0") == 0) less_more = 1;
				else if (strcmp(lst_2->data, ">\0") == 0) less_more = 2;
				lst_2 = lst_2->ptr;
				if (lst_2->tok_num == tok_num_int) {
					int ff = atoi(lst_2->data);
					if (less_more == 1) {
						fprintf(fasm, "\t\tcmpl\t$%d, -%d(%%rbp)\n", ff-1, reg_find); //%d для даты
						fprintf(fasm, "\t\tjg\t\t.L%d\n", l);
					} else if (less_more == 2) {
						fprintf(fasm, "\t\tcmpl\t$%d, -%d(%%rbp)\n", ff, reg_find); //%d для даты
						fprintf(fasm, "\t\tjle\t.L%d\n", l);
					}
				} else if (lst_2->tok_num == tok_id) {
					int reg_find_2 = reg_list_find(rlst_root, lst_2->data);
					fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find);
					fprintf(fasm, "\t\tcmpl\t-%d(%%rbp), %%eax\n", reg_find_2);
					if (less_more == 1) 
						fprintf(fasm, "\t\tjge\t.L%d\n", l);
					else if (less_more == 2) 
						fprintf(fasm, "\t\tjle\t.L%d\n", l);
				}
				lst_2 = lst_2->ptr->ptr;
				if (strcmp(lst_2->data, "{\0") == 0) {
					for (;strcmp(lst_2->data, "}\0") != 0;lst_2 = lst_2->ptr) {
						// TOK VAR
						if (lst_2->tok_num == tok_var) { //var a = 2|"a";
							// lst_2 = lst_2->ptr->ptr;
							if (lst_2->tok_num == tok_num_int) {
								rbp += 4;
								fprintf(fasm, "\t\tmovl\t$%d, -%d(%rbp)\n", lst_2->data, rbp);
							} else if (lst_2->tok_num == tok_string) {
								fprintf(fasmlc, ".LC%d:\n", lc);
								fprintf(fasmlc, "\t\tstring %s\n", lst_2->data);
								if (rbp % 8 == 0) rbp += 8;
								else rbp += 12;
								fprintf(fasm, "\t\tmovq\t$.LC%d, -%d(%rbp)\n", lc++, rbp);
							}
							if (init == 0) {
				  				rlst = reg_list_init(rbp, lst_2->data);
				  				rlst_root = rlst;
				  				init = 1;
							} else rlst = reg_list_addelem(rlst, rbp, lst_2->data);
							lst_2 = lst_2->ptr;
						// TOK EQUAL
						} else if (lst_2->tok_num == tok_id) {
							reg_find = reg_list_find(rlst_root, lst_2->data);
							if (strcmp(lst_2->ptr->data, "(\0") == 0) {
								//это вызов
								int reg_find_2 = reg_list_find(rlst_root, lst_2->ptr->ptr->data);
								fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
								fprintf(fasm, "\t\tmovl\t%%eax, %%edi\n");
								fprintf(fasm, "\t\tcall\t%s\n", lst_2->data);
								lst_2 = lst_2->ptr->ptr->ptr->ptr;
							} else {
								lst_2 = lst_2->ptr;
								if (lst_2->tok_num == tok_num_int) {
									int ff = atoi(lst_2->data);
									fprintf(fasm, "\t\tmovl\t$%d, -%d(%%rbp)\n", ff, reg_find);
								} else if (lst_2->tok_num == tok_id) { 
									if (strcmp(lst_2->ptr->data, ";\0") == 0) {
										int reg_find_2 = reg_list_find(rlst_root, lst_2->data);
										fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
										fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
									} else if (strcmp(lst_2->ptr->data, "+\0") == 0) {
										if (lst_2->ptr->ptr->tok_num == tok_num_int) {
											int reg_find_2 = reg_list_find(rlst_root, lst_2->data);
											lst_2 = lst_2->ptr->ptr;
											int fff = atoi(lst_2->data);
											if (reg_find != reg_find_2) {
												fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
												fprintf(fasm, "\t\taddl\t$%d, %%eax\n", fff);
												fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
											} else {
												fprintf(fasm, "\t\taddl\t$%d, -%d(%%rbp)\n", fff, reg_find);
											}
										}
									} else if (strcmp(lst_2->ptr->data, "-\0") == 0) {
										if (lst_2->ptr->ptr->tok_num == tok_num_int) {
											int reg_find_2 = reg_list_find(rlst_root, lst_2->data);
											lst_2 = lst_2->ptr->ptr;
											int fff = atoi(lst_2->data);
											if (reg_find != reg_find_2) {
												fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
												fprintf(fasm, "\t\tsubl\t$%d, %%eax\n", fff);
												fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
											} else {
												fprintf(fasm, "\t\tsubl\t$%d, -%d(%%rbp)\n", fff, reg_find);
											}
										}
									} else if (strcmp(lst_2->ptr->data, "*\0") == 0) {
									} else if (strcmp(lst_2->ptr->data, "/\0") == 0) {}

								} else if (lst_2->tok_num == tok_string) {
									fprintf(fasmlc, ".LC%d:\n", lc);
									fprintf(fasmlc, "\t\tstring %s\n", lst_2->data);
									fprintf(fasm, "\t\tmovq\t$.LC%d, -%d(%%rbp)\n", lc++, reg_find);
								}
								lst_2 = lst_2->ptr;
							}
						// TOK ALERT
						} else if (lst_2->tok_num == tok_alert) {
							lst_2 = lst_2->ptr->ptr;
							fprintf(fasmlc, ".LC%d:\n", lc);
							fprintf(fasmlc, "\t\t.string %s\n", lst_2->data);
							if (lst_2->tok_num == tok_string) {
								fprintf(fasm, "\t\tmovl\t$.LC%d, %%edi\n", lc++);
								fprintf(fasm, "\t\tmovl\t$0, %%edi\n");
								fprintf(fasm, "\t\tcall\tputs\n");
							} else if (lst_2->tok_num == tok_id) {
								reg_find = reg_list_find(rlst_root, lst_2->data);
								fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find);
								fprintf(fasm, "\t\tmovl\t%%eax, %%esi\n");
								fprintf(fasm, "\t\tmovl\t$.LC%d, %%edi\n", lc++);
								fprintf(fasm, "\t\tcall\tprintf\n");
							}
							lst_2 = lst_2->ptr->ptr;
						}
					}
				}
				fprintf(fasm, ".L%d:\n", ++l);

				// lst_2 = lst_2->ptr->ptr;
				flag_lbrace++;
				//не продумано для выхода L3...L2 по убыванию c lbrace
				lst = lst_2;
			}
// TOK WHILE
		} else if (lst->tok_num == tok_while) {
			lst = lst->ptr->ptr;
			int less_more = 0;
			fprintf(fasm, "\t\tjmp\t\t.L%d:\n", l);
			if (lst->tok_num == tok_id) {
				reg_find = reg_list_find(rlst_root, lst->data);
				// lst = lst->ptr;
				if (strcmp(lst->ptr->data, "<\0") == 0) less_more = 1;
				else if (strcmp(lst->ptr->data, ">\0") == 0) less_more = 2;
	
			}
			// lst = lst->ptr->ptr->ptr->ptr->ptr;
			struct list *lst_2;
			lst_2 = lst->ptr->ptr->ptr->ptr->ptr;
			flag_lbrace++;
			fprintf(fasm, ".L%d\n", ++l);
			for (;strcmp(lst_2->data, "}\0") != 0; lst_2 = lst_2->ptr) {
				// TOK VAR
				if (lst_2->tok_num == tok_var) { //var a = 2|"a";
					// lst_2 = lst_2->ptr->ptr;
					if (lst_2->tok_num == tok_num_int) {
						rbp += 4;
						fprintf(fasm, "\t\tmovl\t$%d, -%d(%rbp)\n", lst_2->data, rbp);
					} else if (lst_2->tok_num == tok_string) {
						fprintf(fasmlc, ".LC%d:\n", lc);
						fprintf(fasmlc, "\t\tstring %s\n", lst_2->data);
						if (rbp % 8 == 0) rbp += 8;
						else rbp += 12;
						fprintf(fasm, "\t\tmovq\t$.LC%d, -%d(%rbp)\n", lc++, rbp);
					}
					if (init == 0) {
		  				rlst = reg_list_init(rbp, lst_2->data);
		  				rlst_root = rlst;
		  				init = 1;
					} else rlst = reg_list_addelem(rlst, rbp, lst_2->data);
					lst_2 = lst_2->ptr;
				// TOK EQUAL
				} else if (lst_2->tok_num == tok_id) {
					reg_find = reg_list_find(rlst_root, lst_2->data);
					if (strcmp(lst_2->ptr->data, "(\0") == 0) {
						//это вызов
						int reg_find_2 = reg_list_find(rlst_root, lst_2->ptr->ptr->data);
						fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
						fprintf(fasm, "\t\tmovl\t%%eax, %%edi\n");
						fprintf(fasm, "\t\tcall\t%s\n", lst_2->data);
						lst_2 = lst_2->ptr->ptr->ptr->ptr;
					} else {
						lst_2 = lst_2->ptr;
						if (lst_2->tok_num == tok_num_int) {
							int ff = atoi(lst_2->data);
							fprintf(fasm, "\t\tmovl\t$%d, -%d(%%rbp)\n", ff, reg_find);
						} else if (lst_2->tok_num == tok_id) { 
							if (strcmp(lst_2->ptr->data, ";\0") == 0) {
								int reg_find_2 = reg_list_find(rlst_root, lst_2->data);
								fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
								fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
							} else if (strcmp(lst_2->ptr->data, "+\0") == 0) {
								if (lst_2->ptr->ptr->tok_num == tok_num_int) {
									int reg_find_2 = reg_list_find(rlst_root, lst_2->data);
									lst_2 = lst_2->ptr->ptr;
									int fff = atoi(lst_2->data);
									if (reg_find != reg_find_2) {
										fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
										fprintf(fasm, "\t\taddl\t$%d, %%eax\n", fff);
										fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
									} else {
										fprintf(fasm, "\t\taddl\t$%d, -%d(%%rbp)\n", fff, reg_find);
									}
								}
							} else if (strcmp(lst_2->ptr->data, "-\0") == 0) {
								if (lst_2->ptr->ptr->tok_num == tok_num_int) {
									int reg_find_2 = reg_list_find(rlst_root, lst_2->data);
									lst_2 = lst_2->ptr->ptr;
									int fff = atoi(lst_2->data);
									if (reg_find != reg_find_2) {
										fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find_2);
										fprintf(fasm, "\t\tsubl\t$%d, %%eax\n", fff);
										fprintf(fasm, "\t\tmovl\t %%eax, -%d(%%rbp)\n", reg_find);
									} else {
										fprintf(fasm, "\t\tsubl\t$%d, -%d(%%rbp)\n", fff, reg_find);
									}
								}
							} else if (strcmp(lst_2->ptr->data, "*\0") == 0) {
							} else if (strcmp(lst_2->ptr->data, "/\0") == 0) {}

						} else if (lst_2->tok_num == tok_string) {
							fprintf(fasmlc, ".LC%d:\n", lc);
							fprintf(fasmlc, "\t\tstring %s\n", lst_2->data);
							fprintf(fasm, "\t\tmovq\t$.LC%d, -%d(%%rbp)\n", lc++, reg_find);
						}
						lst_2 = lst_2->ptr;
					}
				// TOK ALERT
				} else if (lst_2->tok_num == tok_alert) {
					lst_2 = lst_2->ptr->ptr;
					fprintf(fasmlc, ".LC%d:\n", lc);
					fprintf(fasmlc, "\t\t.string %s\n", lst_2->data);
					if (lst_2->tok_num == tok_string) {
						fprintf(fasm, "\t\tmovl\t$.LC%d, %%edi\n", lc++);
						fprintf(fasm, "\t\tmovl\t$0, %%edi\n");
						fprintf(fasm, "\t\tcall\tputs\n");
					} else if (lst_2->tok_num == tok_id) {
						reg_find = reg_list_find(rlst_root, lst_2->data);
						fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find);
						fprintf(fasm, "\t\tmovl\t%%eax, %%esi\n");
						fprintf(fasm, "\t\tmovl\t$.LC%d, %%edi\n", lc++);
						fprintf(fasm, "\t\tcall\tprintf\n");
					}
					lst_2 = lst_2->ptr->ptr;
				}

			}
			// lst = lst->ptr->ptr; потом
			fprintf(fasm, ".L%d:\n", l-1);
			if (lst->ptr->ptr->ptr->tok_num == tok_num_int) {
				int ff = atoi(lst->ptr->ptr->ptr->data);
				if (less_more == 1) {
					fprintf(fasm, "\t\tcmpl\t$%d, -%d(%%rbp)\n", ff, reg_find); //%d для даты
					fprintf(fasm, "\t\tjle\t\t.L%d\n", l-1);
				} else if (less_more == 2) {
					fprintf(fasm, "\t\tcmpl\t$%d, -%d(%%rbp)\n", ff, reg_find); //%d для даты
					fprintf(fasm, "\t\tjg\t\t.L%d\n", ++l);
				}
			} else if (lst->ptr->ptr->ptr->tok_num == tok_id) {
				int reg_find_2 = reg_list_find(rlst_root, lst->ptr->ptr->ptr->data);
				fprintf(fasm, "\t\tmovl\t-%d(%%rbp), %%eax\n", reg_find);
				fprintf(fasm, "\t\tcmpl\t-%d(%%rbp), %%eax\n", reg_find_2);
				if (less_more == 1) {
					fprintf(fasm, "\t\tjl\t\t.L%d\n", l-1);
				} else if (less_more == 2) {
					fprintf(fasm, "\t\tjg\t\t.L%d\n", l-1);
				}
			}
			lst = lst_2->ptr;
// TOK FUNCTION
		} else if (lst->tok_num == tok_function) {
	
		}
	}
	//в конце flag_equals
	fprintf(fasm, "\t\tmovl\t$0, %%eax\n");
	//в конце самом
	fprintf(fasm, "\t\tleave\n");
	fprintf(fasm, "\t\tret\n");
	fclose(fasm); fclose(fasmlc);
	return 1;
}