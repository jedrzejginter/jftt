%{
#include <stdio.h>
#include <math.h>

int arguments = 0;
int operators = 0;

struct StackItem {
	int value;
};

struct Stack {
	struct StackItem *items[1024];
	int size;
};

struct Stack stack;
char error = '*';
char bad_char;

void new_equation() {
	for (int i = 0; i < stack.size; i++) {
		free(stack.items[i]);
	}

	stack.size = 0;
	error = '*';
}

void push(int num) {
	struct StackItem *item = malloc(sizeof(struct StackItem));
	item->value = num;

	stack.items[stack.size] = item;
	stack.size++;
}

void push_raw(char* val) {
	char *v = strdup(val);
	push(atoi(v));
}

int pop() {
	if (stack.size == 0) {
		error = 'S';
		return 0;

	} else {
		struct StackItem *item = stack.items[stack.size - 1];
		stack.size--;

		free(item);

		return item->value;
	}
}

void calc(char *val) {
	if (stack.size < 2) {
		error = 'A';

	} else {
		int a2 = pop();
		int a1 = pop();
		char* op = strdup(val);

		if (strcmp(op, "+") == 0) {
			push(a1 + a2);
		} else if (strcmp(op, "-") == 0) {
			push(a1 - a2);
		} else if (strcmp(op, "*") == 0) {
			push(a1 * a2);
		} else if (strcmp(op, "/") == 0) {
			if (a2 == 0) {
				push(0);
				error = 'M';
			} else {
				push(a1 / a2);
			}
		} else if (strcmp(op, "%") == 0) {
			if (a2 == 0) {
				push(0);
				error = 'M';
			} else {
				push(a1 % a2);
			}
		} else if (strcmp(op, "^") == 0) {
			push(pow(a1, a2));
		}
	}
}

%}

%%
[\%\*\+\-\/\^]	{ printf("%s", yytext); calc(yytext); operators++; }
\-?[0-9]+	{ printf("%s", yytext); push_raw(yytext); arguments++; }
[ \t]+	{ printf(" "); }
\n	{ return 1; }
.	{ printf("%s", yytext); error = 'C'; bad_char = *yytext; }
%%

int main(int argc, char** argv) {
	new_equation();

	while (yylex()) {
		int result = pop();

		printf("\n");

		if (error == 'C') {
			printf("Błąd: zły symbol \"%c\"\n", bad_char);
		} else if (error == 'S') {
			printf("Błąd: stos jest pusty\n");
		} else if (error == 'M') {
			printf("Błąd: dzielenie przez 0\n");
		} else if (error == 'A') {
			printf("Błąd: za mała liczba argumentów\n");
		} else if (stack.size > 0) {
			printf("Błąd: za mała liczba operatorów\n");
		} else {
			printf("= %d\n", result);
		}

		printf("\n");
		new_equation();
	}

	return 0;
}
