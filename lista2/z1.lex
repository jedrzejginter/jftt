%{
#include<stdio.h>
int blanks = 0;
int lines = 0;
int words = 0;
%}


%%
^\n$	{ printf("\n"); }
^[ \t]*\n$ { printf("+_+"); }
^[[:space:]]+	{  }
[[:space:]]+$	{ }
[ \t]+	{ printf(" "); }
\n	{ lines++; printf("\n"); }
[^ \t\n\r\f\v]+	{ printf("%s", yytext); words++; }
%%


int main(int argc, char** argv) {
  yylex();
  printf("---> Liczba linii: %d\n", lines);
  printf("---> Liczba słów: %d\n", words);
}
