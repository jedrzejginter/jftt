%{
#include "y.tab.h"

void yyerror(char *);
void psput(char *, int, int);

char *s;
%}


%%
^\#.*\n		{ }
\\\n			{ }
[ \t]			{ }
[\n]			{ return T_NL; }
\-?[0-9]+	{ s = strdup(yytext); psput(s, 0, 0); return T_NUM; }
[(]			{ s = strdup(yytext); psput(s, T_LP, 1); return T_LP; }
[)]			{ s = strdup(yytext); psput(s, T_RP, 1); return T_RP; }
[+]			{ s = strdup(yytext); psput(s, T_PLUS, 2); return T_PLUS; }
[-]			{ s = strdup(yytext); psput(s, T_MIN, 2); return T_MIN; }
[*]			{ s = strdup(yytext); psput(s, T_MUL, 3); return T_MUL; }
[/]			{ s = strdup(yytext); psput(s, T_DIV, 3); return T_DIV; }
[%]			{ s = strdup(yytext); psput(s, T_MOD, 3); return T_MOD; }
[\^]			{ s = strdup(yytext); psput(s, T_POW, 4); return T_POW; }
.				{ yyerror("Błąd."); return -1; }
%%


int yywrap(void) {
	return 1;
}
