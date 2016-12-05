%{
#include "y.tab.h"

void yyerror(char *);
%}


%%
\n					{ return NL; }
[ \t]				{ }
[0-9]+			{ yylval.str = strdup(yytext); return NUM; }
[+/*%\^()-]	{ return yytext[0]; }
.					{ printf("Błąd: undefined char '%s'\n", yytext); }
%%


int yywrap(void) {
	return 1;
}
