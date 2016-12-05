#include "stack.h"

int stack[STACK_SIZE];
int sptr = 0;

int push(int element) {
	if (sptr == STACK_SIZE) {
		return -1;

	} else {
		stack[sptr] = element;
		sptr++;

		return 0;
	}
}

int pop(int *element) {
	if (sptr == 0) {
		return -1;

	} else {
		*element = stack[sptr-1];
		sptr--;

		return 0;
	}
}
