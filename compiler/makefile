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

build:
	rm -rf 204420 && mkdir 204420 && cp ./*.* 204420 && cp ./make* 204420 && rm -rf 204420/*.txt 204420/*.mr 204420/*.imp 204420/*.zip 204420/.git* 204420/*.sh && zip -r 204420.zip 204420* && rm -rf 204420
