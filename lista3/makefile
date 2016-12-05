
CC = gcc
LEX = flex
YACC = bison
OBJ = lex.yy.o y.tab.o stack.o

default: calc

stack.o: stack.c stack.h
	$(CC) -c stack.c -o stack.o

y.tab.c y.tab.h: calc.y
	$(YACC) -y -d calc.y

lex.yy.c: calc.l
	$(LEX) calc.l

y.tab.o: y.tab.c
	$(CC) -c y.tab.c -o y.tab.o

lex.yy.o: lex.yy.c y.tab.h
	$(CC) -c lex.yy.c -o lex.yy.o

calc: $(OBJ) y.tab.h stack.h
	$(CC) $(OBJ) -lm -o calc

clean:
	rm y.tab.c lex.yy.c y.tab.h $(OBJ) calc
