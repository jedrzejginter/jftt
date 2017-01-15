%{
#include "y.tab.h"

int yywrap(void);
void yyerror(char *);
void register_new_line(void);

%}

NAT_NUM_L [1-9][0-9]*
NAT_NUM [0-9]
ID [_a-z]+

%%
":="			{ return(T_ASSGNOP); }
"<>"			{ return(T_NE); }
"<="			{ return(T_LE); }
"<"			{ return(T_LT); }
">="			{ return(T_GE); }
">"			{ return(T_GT); }
"="			{ return(T_EQ); }
"VAR"			{ return(T_VAR); }
"BEGIN"		{ return(T_BEG); }
"END"			{ return(T_END); }
"IF"			{ return(T_IF); }
"THEN"		{ return(T_THEN); }
"ELSE"		{ return(T_ELSE); }
"ENDIF"		{ return(T_ENDIF); }
"WHILE" 		{ return(T_WHILE); }
"DO" 			{ return(T_DO); }
"ENDWHILE" 	{ return(T_ENDWHILE); }
"FOR" 		{ return(T_FOR); }
"FROM" 		{ return(T_FROM); }
"TO" 			{ return(T_TO); }
"DOWNTO" 	{ return(T_DOWNTO); }
"ENDFOR" 	{ return(T_ENDWHILE); }
"READ"		{ return(T_READ); }
"WRITE"		{ return(T_WRITE); }
"SKIP"		{ return(T_SKIP); }
{ID}			{ yylval.id = (char *)strdup(yytext); return(T_PIDENTIFIER); }
{NAT_NUM_L}	{ yylval.num = atoi(yytext); return(T_NUM); }
{NAT_NUM}	{ yylval.num = atoi(yytext); return(T_NUM); }
[ \t]+
\n				{ register_new_line(); }
\{.*\}		{ }
.				{ return(yytext[0]); }
%%

int yywrap(void) {
	return 1;
}
