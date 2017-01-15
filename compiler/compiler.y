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

		if (f->swap_blocks == 1) {
			b1 = $6;
			b2 = $4;
		} else {
			b1 = $4;
			b2 = $6;
		}

		f = insert(f, jcmd("JZERO", 1, b1->cmd_tree_size + 2));
		f = merge(f, b1);

		f = insert(f, jscmd("JUMP", b2->cmd_tree_size + 1));
		f = merge(f, b2);

		f->swap_blocks = 0;
		$$ = f;
	}
	| T_WHILE condition T_DO commands T_ENDWHILE { }
	| T_FOR T_PIDENTIFIER T_FROM value T_TO value T_DO commands T_ENDFOR { }
	| T_FOR T_PIDENTIFIER T_FROM value T_DOWNTO value T_DO commands T_ENDFOR { }
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
			f = insert(f, cmd("PUT", 1));
		} else {
			f = insert(f, cmd("LOAD", 0));
			f = insert(f, cmd("PUT", 0));
		}

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
			$$ = load_num_to_register($1->value - $3->value, 1);

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
			struct Foobar *f = $3;

			f = merge(f, $1);
			f = insert(f, cmd("SUB", 1));
			$$ = f;
		}
	}
	| value '*' value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			$$ = load_num_to_register($1->value * $3->value, 1);

		}
	}
	| value '/' value {
		if (strcmp($1->type, "num") == 0 && strcmp($3->type, "num") == 0) {
			$$ = load_num_to_register(($1->value - $1->value % $3->value) / $3->value, 1);
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

	return 0;
}

void yyerror(char *s) {
	printf("Error: %s\n", s);
}
