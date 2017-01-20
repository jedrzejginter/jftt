%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errors.h"
#include "types.h"
#include "code.h"
#include "memory.h"
#include "type-generators.h"
#include "code-generator.h"
#define YYDEBUG 1
int yylex(void);
void yyerror(char *);

%}
%union {
	int num;
	char *var;
	struct Declarations *vdec;
	struct Id *id;
	struct Value *val;
	struct Condition *cond;
	struct Expression *expr;
	struct Command *cmd;
	struct Commands *cmds;
}

%start program
%token T_VAR T_BEG T_END
%token T_IF T_THEN T_ELSE T_ENDIF T_WHILE T_DO T_ENDWHILE
%token T_FOR T_FROM T_TO T_ENDFOR T_DOWNTO T_READ T_WRITE T_SKIP
%token T_EQ T_NE T_LT T_GT T_LE T_GE
%token T_ASSGNOP
%token T_NEWLINE T_SEMICOLON T_OP_BRACKET T_CL_BRACKET
%token <num> T_NUM
%token <var> T_PIDENTIFIER
%type <id> identifier
%type <val> value
%type <expr> expression
%type <cond> condition
%type <cmd> command
%type <cmds> commands
%left '-' '+'
%left '*' '/' '%'

%%
program : T_VAR vdeclarations T_BEG commands T_END {

	if (ERRORS > 0) {
		exit(0);
	} else {
		struct OutputCode *oc = code_gen($4);

		if (ERRORS > 0) {
			exit(0);
		} else {
			print_cmd_tree(oc, 0); }
		}
	}
;

vdeclarations :
	| vdeclarations T_PIDENTIFIER 											{ __declare_var($2, 0, 1, "num"); }
	| vdeclarations T_PIDENTIFIER T_OP_BRACKET T_NUM T_CL_BRACKET 	{ __declare_var($2, $4, 1, "arr"); }
;

commands : commands command	{ $$ = __insert_command($1, $2); }
	| command 						{ $$ = __insert_command(NULL, $1); }
;

command : identifier T_ASSGNOP expression T_SEMICOLON {
		$$ = __Command("assign");
		$$->id = $1;
		$$->expr = $3;
	}
	| T_IF condition T_THEN commands T_ELSE commands T_ENDIF {
		$$ = __Command("if");
		$$->cond = $2;
		$$->cmd1 = $4;
		$$->cmd2 = $6;
	}
	| T_WHILE condition T_DO commands T_ENDWHILE {
		$$ = __Command("while");
		$$->cond = $2;
		$$->cmd1 = $4;
	}
	| T_FOR T_PIDENTIFIER T_FROM value T_TO value T_DO commands T_ENDFOR {
		$$ = __Command("for");
		$$->pid = $2;
		$$->val1 = $4;
		$$->val2 = $6;
		$$->cmd1 = $8;
	}
	| T_FOR T_PIDENTIFIER T_FROM value T_DOWNTO value T_DO commands T_ENDFOR {
		$$ = __Command("for-downto");
		$$->pid = $2;
		$$->val1 = $4;
		$$->val2 = $6;
		$$->cmd1 = $8;
	}
	| T_READ identifier T_SEMICOLON {
		$$ = __Command("read");
		$$->id = $2;
	}
	| T_WRITE value T_SEMICOLON {
		$$ = __Command("write");
		$$->val1 = $2;
	}
	| T_SKIP T_SEMICOLON {
		$$ = __Command("skip");
	}
;

expression : value 		{ $$ = __Expression($1, NULL, NULL); }
	| value '+' value		{ $$ = __Expression($1, $3, "+"); }
	| value '-' value		{ $$ = __Expression($1, $3, "-"); }
	| value '*' value		{ $$ = __Expression($1, $3, "*"); }
	| value '/' value		{ $$ = __Expression($1, $3, "/"); }
	| value '%' value		{ $$ = __Expression($1, $3, "%"); }
;

condition : value T_EQ value 	{ $$ = __Condition($1, $3, "="); }
	| value T_NE value 			{ $$ = __Condition($1, $3, "<>"); }
	| value T_LT value 			{ $$ = __Condition($1, $3, "<"); }
	| value T_GT value 			{ $$ = __Condition($1, $3, ">"); }
	| value T_LE value 			{ $$ = __Condition($1, $3, "<="); }
	| value T_GE value 			{ $$ = __Condition($1, $3, ">="); }
;

value: T_NUM 		{ $$ = __Value("num", $1, NULL); $$->ln = PR_LINE; }
	| identifier 	{ $$ = __Value("id", 0, $1); $$->ln = PR_LINE; }
;

identifier : T_PIDENTIFIER 											{ $$ = __Id($1, 0, NULL);	$$->ln = PR_LINE; }
	| T_PIDENTIFIER T_OP_BRACKET T_PIDENTIFIER T_CL_BRACKET 	{ $$ = __Id($1, 0, $3);		$$->ln = PR_LINE; }
	| T_PIDENTIFIER T_OP_BRACKET T_NUM T_CL_BRACKET 			{ $$ = __Id($1, $3, NULL);	$$->ln = PR_LINE; }
;

%%
int main(int argc, char *argv[]) {
	yyparse ();
	return 0;
}

void yyerror(char *s) {
	__err_bad_syntax();
}
