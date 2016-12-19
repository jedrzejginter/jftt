%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "stack.h"

int yylex(void);
void yyerror(char *);
int calculate(void);
void psput(char *, int, int);

struct Token {
	char* value;
	int tid;
	int prec;
};

struct Token psstack[1024];
int psstack_pos = 0;

struct Token opstack[1024];
int opstack_pos = 0;

struct Token rpnstack[1024];
int rpnstack_pos = 0;

void rpnstack_push(struct Token);
struct Token rpnstack_pop();

%}

%union {
	char *string;
}

%token T_NL T_LP T_RP T_PLUS T_MIN T_MUL T_DIV T_MOD T_POW
%token <string> T_NUM
%type <string> expr
%type <string> term
%type <string> efact
%type <string> fact

%%
program:
	| program line
;

line:
	T_NL
	| expr T_NL {
		printf("\nWynik: %d\n", calculate());
		psstack_pos = 0;
		opstack_pos = 0;
		rpnstack_pos = 0;
	}
;

expr:
	term
	| expr T_PLUS term
	| expr T_MIN term
;

term:
	efact
	| term T_MUL efact
	| term T_DIV efact
	| term T_MOD efact
;

efact:
	fact
	| efact T_POW fact
;

fact:
	T_NUM
	| T_LP expr T_RP
;
%%

int main(int argc, char **argv) {
	yyparse();
	return 0;
}

void yyerror (char *s) {
	printf("Błąd.\n");
}

void psput(char *s, int tid, int prec) {
	struct Token tok;
	tok.value = s;
	tok.tid = tid;
	tok.prec = prec;
	psstack[psstack_pos++] = tok;
}

void rpnstack_push(struct Token t) {
	rpnstack[rpnstack_pos++] = t;
}

struct Token rpnstack_pop() {
	struct Token rettok = rpnstack[rpnstack_pos - 1];
	rpnstack_pos--;

	return rettok;
}

int calculate() {
	struct Token tok;

	for (int i = 0; i < psstack_pos; i++) {
		tok = psstack[i];

		switch (tok.tid) {
			case 0: {
				rpnstack_push(tok);
				break;
			}
			case T_LP: {
				opstack[opstack_pos] = tok;
				opstack_pos++;
				break;
			}
			case T_RP: {
				int lp_found = 0;

				for (int i = opstack_pos - 1; i >= 0; --i) {
					if (opstack[i].tid != T_LP) {
						rpnstack_push(opstack[i]);
						opstack_pos--;
						lp_found = 1;
					} else {
						break;
					}
				}

				if (lp_found == 0) {
					printf("Błąd nawiasów\n");
					return -1;
				} else if (opstack_pos > 0) {
					opstack_pos--;
				}

				break;
			}
			default: {
				for (int i = opstack_pos - 1; i >= 0; i--) {
					int should_pop = 0;

					if (opstack[i].tid == T_POW) {
						if (opstack[i].prec > tok.prec) {
							should_pop = 1;
						}
					} else {
						if (opstack[i].prec >= tok.prec) {
							should_pop = 1;
						}
					}

					if (should_pop == 1) {
						rpnstack_push(opstack[i]);
						opstack_pos--;
					} else {
						break;
					}
				}

				opstack[opstack_pos] = tok;
				opstack_pos++;

				break;
			}
		}
	}

	if (opstack[opstack_pos-1].tid == T_LP || opstack[opstack_pos-1].tid == T_RP) {
		printf("Błąd nawiasów");
		return -1;
	}

	for (int i = opstack_pos - 1; i >= 0; i--) {
		rpnstack_push(opstack[i]);
	}

	struct Token tmpstack[1024];
	int tmpstack_pos = 0;

	for (int i = 0; i < rpnstack_pos; i++) {
		struct Token t = rpnstack[i];

		printf("%s ", t.value);

		if (t.tid == 0) {
			tmpstack[tmpstack_pos] = t;
			tmpstack_pos++;
		} else {
			if (tmpstack_pos >= 2) {
				int a1 = atoi(tmpstack[tmpstack_pos-2].value);
				int a2 = atoi(tmpstack[tmpstack_pos-1].value);

				tmpstack_pos -= 2;

				int pushval = 0;

				switch (t.tid) {
					case T_PLUS: { pushval = a1+a2; break; }
					case T_MIN: { pushval = a1-a2; break; }
					case T_DIV:
					case T_MOD: {
						if (a2 == 0) {
							yyerror("Błąd - dzielenie przez 0!\n");
						} else {
							if (t.tid == T_DIV) {
								pushval = a1/a2;
							} else {
								pushval = a1%a2;
							}
						}

						break;
					}
					case T_MUL: { pushval = a1*a2; break; }
					case T_POW: { pushval = pow(a1, a2); break; }
					default: { break; }
				}

				struct Token nt;
				char tmp[1024];

				sprintf(tmp, "%d", pushval);

				nt.value = strdup(tmp);
				nt.tid = 0;
				nt.prec = 0;

				tmpstack[tmpstack_pos] = nt;
				tmpstack_pos++;
			} else {
				yyerror("Błąd na stosie\n");
			}
		}
	}

	return atoi(tmpstack[0].value);
}
