%{
#include<stdio.h>
int lines = 0;
int words = 0;
%}


%%
^[[:space:]]+		{}
[ \t]+$				{}
[ \t]+				{ printf(" "); }
\n						{ lines++; printf("\n"); }
[^ \t\n]+			{ words++; printf("%s", yytext); }
%%


int main(int argc, char** argv) {
	yylex();

	printf(">\n> Liczba linii: %d\n", lines);
	printf("> Liczba słów: %d\n>\n", words);
}
