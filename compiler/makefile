CC = gcc
LEX = flex
YACC = bison
OBJ = lex.yy.o y.tab.o

default: compiler

y.tab.c y.tab.h: compiler.y
	$(YACC) -y -d compiler.y

lex.yy.c: compiler.lex
	$(LEX) compiler.lex

y.tab.o: y.tab.c
	$(CC) -c y.tab.c -o y.tab.o

lex.yy.o: lex.yy.c y.tab.h
	$(CC) -c lex.yy.c -o lex.yy.o

compile: $(OBJ) y.tab.h
	$(CC) $(OBJ) -lm -o compiler

clean:
	rm y.tab.c lex.yy.c y.tab.h $(OBJ) compiler
