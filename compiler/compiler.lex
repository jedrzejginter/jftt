%{
#include "y.tab.h"

int yywrap(void);
void yyerror(char *);
void register_new_line(void);

int cmt = 0;

%}

NAT_NUM_L [1-9][0-9]*
NAT_NUM [0-9]
ID [_a-z]+

%%
"{"			{ cmt = 1; }
";"			{ if (cmt == 0) { return(T_SEMICOLON); } }
"["			{ if (cmt == 0) { return(T_OP_BRACKET); } }
"]"			{ if (cmt == 0) { return(T_CL_BRACKET); } }
":="			{ if (cmt == 0) { return(T_ASSGNOP); } }
"<>"			{ if (cmt == 0) { return(T_NE); } }
"!="			{ if (cmt == 0) { return(T_NE); } }
"<="			{ if (cmt == 0) { return(T_LE); } }
"<"			{ if (cmt == 0) { return(T_LT); } }
">="			{ if (cmt == 0) { return(T_GE); } }
">"			{ if (cmt == 0) { return(T_GT); } }
"="			{ if (cmt == 0) { return(T_EQ); } }
"VAR"			{ if (cmt == 0) { return(T_VAR); } }
"BEGIN"		{ if (cmt == 0) { return(T_BEG); } }
"THEN"		{ if (cmt == 0) { return(T_THEN); } }
"ELSE"		{ if (cmt == 0) { return(T_ELSE); } }
"ENDIF"		{ if (cmt == 0) { return(T_ENDIF); } }
"IF"			{ if (cmt == 0) { return(T_IF); } }
"ENDWHILE" 	{ if (cmt == 0) { return(T_ENDWHILE); } }
"WHILE" 		{ if (cmt == 0) { return(T_WHILE); } }
"DO" 			{ if (cmt == 0) { return(T_DO); } }
"FROM" 		{ if (cmt == 0) { return(T_FROM); } }
"DOWNTO" 	{ if (cmt == 0) { return(T_DOWNTO); } }
"TO" 			{ if (cmt == 0) { return(T_TO); } }
"ENDFOR" 	{ if (cmt == 0) { return(T_ENDFOR); } }
"FOR" 		{ if (cmt == 0) { return(T_FOR); } }
"END"			{ if (cmt == 0) { return(T_END); } }
"READ"		{ if (cmt == 0) { return(T_READ); } }
"WRITE"		{ if (cmt == 0) { return(T_WRITE); } }
"SKIP"		{ if (cmt == 0) { return(T_SKIP); } }
{ID}			{ if (cmt == 0) { yylval.var = (char *)strdup(yytext); return(T_PIDENTIFIER); } }
{NAT_NUM_L}	{ if (cmt == 0) { yylval.num = atoi(yytext); return(T_NUM); } }
{NAT_NUM}	{ if (cmt == 0) { yylval.num = atoi(yytext); return(T_NUM); } }
\n				{ register_new_line(); }
[ \t]+		{ }
"}"			{ if (cmt == 1) { cmt = 0; } }
.				{ if (cmt == 0) { return(yytext[0]); } }
%%

int yywrap(void) {
	return 1;
}
