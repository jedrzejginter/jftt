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
"!="			{ return(T_NE); }
"<="			{ return(T_LE); }
"<"			{ return(T_LT); }
">="			{ return(T_GE); }
">"			{ return(T_GT); }
"="			{ return(T_EQ); }
"VAR"			{ return(T_VAR); }
"BEGIN"		{ return(T_BEG); }
"THEN"		{ return(T_THEN); }
"ELSE"		{ return(T_ELSE); }
"ENDIF"		{ return(T_ENDIF); }
"IF"			{ return(T_IF); }
"ENDWHILE" 	{ return(T_ENDWHILE); }
"WHILE" 		{ return(T_WHILE); }
"DO" 			{ return(T_DO); }
"FROM" 		{ return(T_FROM); }
"DOWNTO" 	{ return(T_DOWNTO); }
"TO" 			{ return(T_TO); }
"ENDFOR" 	{ return(T_ENDFOR); }
"FOR" 		{ return(T_FOR); }
"END"			{ return(T_END); }
"READ"		{ return(T_READ); }
"WRITE"		{ return(T_WRITE); }
"SKIP"		{ return(T_SKIP); }
{ID}			{ yylval.var = (char *)strdup(yytext); return(T_PIDENTIFIER); }
{NAT_NUM_L}	{ yylval.num = atoi(yytext); return(T_NUM); }
{NAT_NUM}	{ yylval.num = atoi(yytext); return(T_NUM); }
[ \t]+
\n
\{.*\}
.				{ return(yytext[0]); }
%%

int yywrap(void) {
	return 1;
}
