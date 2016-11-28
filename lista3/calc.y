%{
#include <stdlib.h>

extern "C" int yylex();
extern "C" int yyparse();

void yyerror(const char *m);
%}

%union {
	int ival;
	float fval;
	char *sval;
}

%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING

%%

snazzle:
	INT snazzle      { printf("bison found an int: %d\n", $1); }
	| FLOAT snazzle  { printf("bison found a float: %f\n", $1); }
	| STRING snazzle { printf("bison found a string: %s\n", $1); }
	| INT            { printf("bison found an int: %d\n", $1); }
	| FLOAT          { printf("bison found a float: %f\n", $1); }
	| STRING         { printf("bison found a string: %s\n", $1); }
	;

%%

int main(void) {
	yylex();
}

void yyerror(const char *m) {
	printf("Error: %s\n", m);
	exit(-1);
}
