%{
#include<stdio.h>

%}


%%
\".*\"																{ printf("%s", yytext); }
\/\/\/(.*\\\n)*.*													{ printf("%s", yytext); }
\/\/(.*\\\n)*.*													{}
\/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+\/[ \t]*\n?	{}
.																		{ printf("%s", yytext); }
%%


int main(int argc, char** argv) {
  yylex();
}
