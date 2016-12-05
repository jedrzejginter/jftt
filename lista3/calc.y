%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "stack.h"

int yylex(void);
void yyerror(char *);
void add_to_queue(char *);
%}

%union {
	int num;
	char *str;
}

%token NUM
%token NL OP
%type <num> NL
%type <str> NUM OP exp

%%
line:
	NL		{}
	| exp NL { printf("exp: %s\n", $1); }
	| line exp NL { printf("exp: %s\n", $2); }
;

exp:
	NUM					{ add_to_queue($1); }
	| exp '+' exp		{ add_to_queue("+"); }
	| exp '-' exp		{ add_to_queue("-"); }
	| exp '*' exp		{ add_to_queue("*"); }
	| exp '/' exp		{ add_to_queue("/"); }
	| exp '%' exp		{ add_to_queue("%"); }
	| exp '^' exp		{ add_to_queue("^"); }
	| '(' exp ')'		{ add_to_queue("("); add_to_queue(")"); }
;
%%

void add_to_queue(char *e) {
	printf("Added to queue: %s\n", e);
}

int main (int argc, char **argv) {
	int e;

	push(1);
	pop(&e);

	printf("%d", e);

	//yyparse();
	return 0;
}

void yyerror (char *s) {
	fprintf(stderr, "%s\n", s);
}
