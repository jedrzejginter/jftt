%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "symtab.h"
#define YYDEBUG 1
int yylex(void);
void yyerror(char *);

%}
%union {
	int num;
	char *id;
	struct Foobar *foobar;
}

%start program
%token T_VAR T_BEG T_END
%token T_IF T_THEN T_ELSE T_ENDIF T_WHILE T_DO T_ENDWHILE
%token T_FOR T_FROM T_TO T_ENDFOR T_DOWNTO T_READ T_WRITE T_SKIP
%token T_EQ T_NE T_LT T_GT T_LE T_GE
%token T_ASSGNOP
%token T_NEWLINE
%token <num> T_NUM
%token <id> T_PIDENTIFIER
%type <foobar> identifier
%type <foobar> value
%type <foobar> expression
%type <foobar> condition
%type <foobar> command commands
%left '-' '+'
%left '*' '/' '%'

%%
program : T_VAR vdeclarations T_BEG commands T_END {
	print_cmd_tree($4, -1);
}
;

vdeclarations :
	| vdeclarations T_PIDENTIFIER { install_sym($2, 1, "number"); }
	| vdeclarations T_PIDENTIFIER '[' T_NUM ']' { install_sym($2, $4, "array"); }
;

commands : commands command {
		struct Foobar *f = $1;
		f = merge(f, $2);

		$$ = f;
	}
	| command {
		$$ = $1;
	}
;

command :
	identifier T_ASSGNOP expression ';' {
		struct Foobar *f = $3;

		f = merge(f, $1);
		f = insert(f, cmd("STORE", 1));

		$$ = f;
	}
	| T_IF condition T_THEN commands T_ELSE commands T_ENDIF {
		struct Foobar *f = $2;
		struct Foobar *b1, *b2;

		b1 = $4;
		b2 = $6;

		if (f->swap_blocks == 1) {
			b1 = $6;
			b2 = $4;
		}

		int raw_ln = b1->cmd_tree_size + 2;
		int ln = raw_ln + $2->add_cmd_tree_size + 1;

		f = insert(f, jcmd("JZERO", 1, ln));

		if (f->add_cmd_tree_size > 0) {
			f = merge_add_tree(f, f);
			f = insert(f, jscmd("JUMP", raw_ln));
		}

		f = merge(f, b1);

		f = insert(f, jscmd("JUMP", b2->cmd_tree_size + 1));
		f = merge(f, b2);

		f->swap_blocks = 0;
		$$ = f;
	}
	| T_WHILE condition T_DO commands T_ENDWHILE {
		struct Foobar *f = reg_overwrite($2, 1, 3);
		struct Foobar *a1, *a2;

		int cond_size = $2->cmd_tree_size;
		int ln = $4->cmd_tree_size + 2;

		if ($2->swap_blocks == 1) {
			f = insert(f, jcmd("JZERO", 3, 2));
			f = insert(f, jscmd("JUMP", ln));

		} else {
			f = insert(f, jcmd("JZERO", 3, ln));
		}

		f = merge(f, $4);

		f = insert(f, jscmd("JUMP", -(ln + cond_size) + 1));

		$$ = f;
	}
	| T_FOR T_PIDENTIFIER T_FROM value T_TO value T_DO commands T_ENDFOR {

	}
	| T_FOR T_PIDENTIFIER T_FROM value T_DOWNTO value T_DO commands T_ENDFOR {
		struct Foobar *f, *g;

		install_sym($2, 1, "num");

		if (strcmp($6->type, "num") == 0 && strcmp($4->type, "num") == 0) {
			int endval = $4->value - $6->value + 1;

			if (endval < 0) {
				endval = 0;
			}

			f = load_num_to_register(endval, 2);

		} else if (strcmp($4->type, "num") == 0 && strcmp($6->type, "id") == 0) {
			f = load_num_to_register($4->value + 1, 2);
			f = merge(f, load_sym_to_register($6->name, 0));
			f = insert(f, cmd("SUB", 2));

		} else if (strcmp($4->type, "id") == 0 && strcmp($6->type, "id") == 0) {
			f = load_sym_to_register($4->name, 0);
			f = insert(f, cmd("LOAD", 2));
			f = insert(f, cmd("INC", 2));
			f = merge(f, load_sym_to_register($6->name, 0));
			f = insert(f, cmd("SUB", 2));

		} else {
			// TODO: from ID to NUM

		}

		g = load_sym_to_register($2, 0);
		g = insert(g, cmd("LOAD", 1));
		g = insert(g, cmd("DEC", 1));
		g = insert(g, cmd("STORE", 1));

		f = insert(f, jcmd("JZERO", 2, g->cmd_tree_size + $8->cmd_tree_size + 3));
		f = merge(f, $8);
		f = insert(f, cmd("DEC", 2));
		f = merge(f, g);
		f = insert(f, jscmd("JUMP", - (g->cmd_tree_size + $8->cmd_tree_size + 2)));

		uninstall_sym($2);

		// A: JZERO 2 X
		// 	{ load r2 value to memory if inner for loops }
		// 	commands here
		// 	{ load from memory if inner loops }
		// 	DEC 2
		// X: JUMP A
		$$ = f;
	}
	| T_READ identifier ';' {
		struct Foobar *f = $2;

		f = insert(f, cmd("GET", 1));
		f = insert(f, cmd("STORE", 1));

		$$ = f;
	}
	| T_WRITE value ';' {
		struct Foobar *f = $2;

		if (strcmp($2->type, "id") == 0) {
			f = insert(f, cmd("LOAD", 1));
		}

		f = insert(f, cmd("PUT", 1));

		$$ = f;
	}
	| T_SKIP ';' { }
;

expression : value { $$ = $1; }
	| value '+' value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			$$ = load_num_to_register($1->value + $3->value, 1);

		} else if (strcmp($1->type, "id") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, $3);
			f = insert(f, cmd("ADD", 1));
			$$ = f;

		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;

			f = merge(f, $3);
			f = insert(f, cmd("ADD", 1));
			$$ = f;

		} else {
			struct Foobar *f = $3;

			f = merge(f, $1);
			f = insert(f, cmd("ADD", 1));
			$$ = f;

		}
	}
	| value '-' value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val = $1->value - $3->value;
			if (val < 0) {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);

		} else if (strcmp($1->type, "id") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, $3);
			f = insert(f, cmd("SUB", 1));
			$$ = f;

		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;

			f = merge(f, $3);
			f = insert(f, cmd("SUB", 1));
			$$ = f;

		} else {
			struct Foobar *f = $1;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, load_num_to_register($3->value, 0));

			f = insert(f, jcmd("JZERO", 0, 4));
			f = insert(f, cmd("DEC", 0));
			f = insert(f, cmd("DEC", 1));
			f = insert(f, jscmd("JUMP", -3));
			$$ = f;
		}
	}
	| value '*' value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val = $1->value * $3->value;
			if (val < 0) {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);
		}
	}
	| value '/' value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val;
			if ($3->value == 0) {
				val = 0;
			} else {
				val = ($1->value - $1->value % $3->value) / $3->value;
			}

			if (val < 0) {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);
		}
	}
	| value '%' value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			$$ = load_num_to_register($1->value % $3->value, 1);
		}
	}
;

condition : value T_EQ value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val;

			if ($1->value == $3->value) {
				val = 1;
			} else {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);
		} else if (strcmp($1->type, "id") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			f = insert_add(f, cmd("DEC", 1));
			f = insert_add(f, jcmd("JZERO", 1, 2));

			$$ = f;
		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;

			f = merge(f, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			f = insert_add(f, cmd("DEC", 1));
			f = insert_add(f, jcmd("JZERO", 1, 2));

			$$ = f;
		} else {
			struct Foobar *f = $3;

			f = merge(f, $1);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			f = insert_add(f, cmd("DEC", 1));
			f = insert_add(f, jcmd("JZERO", 1, 2));

			$$ = f;
		}
	}
	| value T_NE value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val;

			if ($1->value != $3->value) {
				val = 1;
			} else {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);
		} else if (strcmp($1->type, "id") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			f = insert_add(f, cmd("DEC", 1));
			f = insert_add(f, jcmd("JZERO", 1, 2));

			f->swap_blocks = 1;

			$$ = f;
		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;

			f = merge(f, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			f = insert_add(f, cmd("DEC", 1));
			f = insert_add(f, jcmd("JZERO", 1, 2));

			f->swap_blocks = 1;

			$$ = f;
		} else {
			struct Foobar *f = $3;

			f = merge(f, $1);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			f = insert_add(f, cmd("DEC", 1));
			f = insert_add(f, jcmd("JZERO", 1, 2));

			f->swap_blocks = 1;

			$$ = f;
		}
	}
	| value T_LT value {
		if (strcmp($1->type, "id") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $3;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, $1);
			f = insert(f, cmd("SUB", 1));
			$$ = f;

		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val;

			if ($1->value < $3->value) {
				val = 1;
			} else {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);

		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = merge($1, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			f->swap_blocks = 1;

			$$ = f;

		} else {
			struct Foobar *f = merge($1, $3);
			f = insert(f, cmd("SUB", 1));
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("DEC", 1));

			$$ = f;
		}
	}
	| value T_GT value {
		if (strcmp($1->type, "id") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, $3);
			f = insert(f, cmd("SUB", 1));
			$$ = f;

		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val;

			if ($1->value > $3->value) {
				val = 1;
			} else {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);

		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = merge($1, $3);
			f = insert(f, cmd("SUB", 1));
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("DEC", 1));

			$$ = f;

		} else {
			struct Foobar *f = merge($1, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			f->swap_blocks = 1;

			$$ = f;
		}
	}
	| value T_LE value {
		if (strcmp($1->type, "id") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $3;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, $1);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));
			$$ = f;
		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val;

			if ($1->value <= $3->value) {
				val = 1;
			} else {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);
		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = merge($1, $3);
			f = insert(f, cmd("SUB", 1));

			f->swap_blocks = 1;

			$$ = f;
		} else {
			struct Foobar *f = merge($1, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			$$ = f;
		}
	}
	| value T_GE value {
		if (strcmp($1->type, "id") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = $1;
			f = insert(f, cmd("LOAD", 1));

			f = merge(f, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));
			$$ = f;
		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			int val;

			if ($1->value >= $3->value) {
				val = 1;
			} else {
				val = 0;
			}

			$$ = load_num_to_register(val, 1);
		} else if (strcmp($1->type, "num") == 0 && strcmp($3->type, "id") == 0) {
			struct Foobar *f = merge($1, $3);
			f = insert(f, cmd("INC", 1));
			f = insert(f, cmd("SUB", 1));

			$$ = f;
		} else {
			struct Foobar *f = merge($1, $3);
			f = insert(f, cmd("SUB", 1));

			f->swap_blocks = 1;

			$$ = f;
		}
	}
;

value: T_NUM 		{ $$ = load_num_to_register($1, 1); }
	| identifier 	{ $$ = $1; }
;

identifier : T_PIDENTIFIER 					{ $$ = load_sym_to_register($1, 0); }
	| T_PIDENTIFIER '[' T_PIDENTIFIER ']' 	{ $$ = load_tab_id_index_to_register($1, $3, 0); }
	| T_PIDENTIFIER '[' T_NUM ']' 			{ $$ = load_tab_sym_to_register($1, $3, 0); }
;

%%
int main(int argc, char *argv[]) {
	yyparse ();

	print_line("HALT\n");
	print_mem();

	return 0;
}

void yyerror(char *s) {
	printf("Error: %s\n", s);
}
