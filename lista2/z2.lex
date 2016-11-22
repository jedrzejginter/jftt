%{

%}


%%
\".*\"																{ printf("%s", yytext); }
\/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+\/[ \t]*\n?	{ }
%%


int main(int argc, char** argv) {
  yylex();
}
