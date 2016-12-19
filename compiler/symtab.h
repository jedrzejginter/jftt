struct symrec {
	char *name; /* name of symbol */
	struct symrec *next; /* link field */
	int is_initialized;
};

typedef struct symrec symrec;
symrec *sym_table = (symrec *)0;

symrec *putsym(char *sym_name) {
	symrec *ptr;

	ptr = (symrec *)malloc(sizeof(symrec));
	ptr->name = (char *)malloc(strlen(sym_name) + 1);
	strcpy(ptr->name, sym_name);
	ptr->next = (struct symrec *)sym_table;
	ptr->is_initialized = 0;

	sym_table = ptr;

	return ptr;
}

symrec *getsym(char *sym_name) {
	symrec *ptr;

	for (ptr = sym_table; ptr != (symrec *)0; ptr = (symrec *)ptr->next) {
		if (strcmp(ptr->name, sym_name) == 0) {
			return ptr;
		}
	}

	return 0;
}

void install(char *sym_name) {
	symrec *s;
	s = getsym(sym_name);

	if (s == 0) {
		s = putsym(sym_name);
	} else {
		register_error();
		printf("%d: `%s` is already defined.\n", line_number, sym_name);
	}
}

void context_check(char *sym_name) {
	if (getsym(sym_name) == 0) {
		register_error();
		printf("%d: `%s` is an unknown token.\n", line_number, sym_name);
	}
}

void sym_set_init(char *sym_name) {
	symrec *sym = getsym(sym_name);

	if (sym == 0) {
		printf("%d: `%s` is an unknown token.\n", line_number, sym_name);
	} else {
		sym->is_initialized = 1;
	}
}

void sym_init_check(char *sym_name) {
	symrec *sym = getsym(sym_name);

	if (sym == 0) {
		printf("%d: `%s` is an unknown token.\n", line_number, sym_name);
	} else if (sym->is_initialized == 0) {
		printf("%d: `%s` has not been initialized.\n", line_number, sym_name);
	}
}
