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
	long long int num;
	char *id;
}

%start program
%token T_VAR T_BEG T_END
%token T_IF T_THEN T_ELSE T_ENDIF T_WHILE T_DO T_ENDWHILE
%token T_FOR T_FROM T_TO T_ENDFOR T_DOWNTO T_READ T_WRITE T_SKIP
%token T_EQ T_NE T_LT T_GT T_LE T_GE
%token T_NUM T_ASSGNOP
%token T_NEWLINE
%token <id> T_PIDENTIFIER
%left '-' '+'
%left '*' '/' '%'

%%
program : T_VAR vdeclarations T_BEG commands T_END
;

vdeclarations :
	| vdeclarations T_PIDENTIFIER { install($2); }
	| vdeclarations T_PIDENTIFIER '[' T_NUM ']' { install($2); }
;

commands : commands command
	| command
;

command :
	identifier T_ASSGNOP expression ';'	{ sym_set_init($1); }
	| T_IF condition T_THEN commands T_ELSE commands T_ENDIF
	| T_WHILE condition T_DO commands T_ENDWHILE
	| T_FOR T_PIDENTIFIER T_FROM value T_TO value T_DO commands T_ENDFOR { context_check($2); }
	| T_FOR T_PIDENTIFIER T_FROM value T_DOWNTO value T_DO commands T_ENDFOR { context_check($2); }
	| T_READ identifier ';'	{ sym_set_init($1); }
	| T_WRITE value ';'
	| T_SKIP ';'
;

expression : value
	| value '+' value
	| value '−' value
	| value '*' value
	| value '/' value
	| value '%' value
;

condition : value T_EQ value
	| value T_NE value
	| value T_LT value
	| value T_GT value
	| value T_LE value
	| value T_GE value
;

value: T_NUM
	| identifier
;

identifier : T_PIDENTIFIER { context_check($1); }
	| T_PIDENTIFIER '[' T_PIDENTIFIER ']' { context_check($1); context_check($3); }
	| T_PIDENTIFIER '[' T_NUM ']' { context_check($1); }
;

%%
int main(int argc, char *argv[]) {
	yyparse ();
	if (errors > 0) {
		printf("---\nErrors generated: %d\n", errors);
	} else {
		printf("Compiled successfully :)\n");
	}
	return 0;
}

void yyerror(char *s) {
	printf("Error: %s\n", s);
}
