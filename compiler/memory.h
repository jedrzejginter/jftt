struct Var {
	char *name;
	char *type;
	int size;
	int memory_index;
	int is_mutable;		// czy można przypisać do zmiennej?
	int is_initialized;	// czy zmienna zainicjalizowana? 0 false, 1 true
	int is_definite;		// czy znamy wartość? 0 false, 1 true
};

struct Memory {
	struct Var *vars[1024];
	int size;
};

struct Memory *mem;

struct Var *__get_var(char *name) {
	if (mem == NULL) {
		return NULL;
	}

	for (int i = 0; i < mem->size; i++) {
		if (strcmp(name, mem->vars[i]->name) == 0) {
			return mem->vars[i];
		}
	}

	return NULL;
}

void __insert_var_to_mem(struct Var *v) {
	if (mem == NULL) {
		mem = (struct Memory *)malloc(sizeof(struct Memory));
		mem->size = 0;
		v->memory_index = 0;

	} else {
		v->memory_index = mem->vars[mem->size-1]->memory_index + mem->vars[mem->size-1]->size;
	}

	mem->vars[mem->size++] = v;
}


void __declare_var(char *name, int array_size, int is_mutable, char *type) {
	if (__get_var(name) != NULL) {
		__err_dup_decl(PR_LINE, name);

	} else if (strcmp(type, "arr") == 0 && array_size <= 0) {
		__err_bad_array_init(PR_LINE, name);

	} else {

		struct Var *v = malloc(sizeof(struct Var));

		v->name = name;
		v->type = (array_size == 0)? "num" : "arr";
		v->size = (array_size == 0)? 1: array_size;
		v->memory_index = 0;
		v->is_mutable = is_mutable;
		v->is_initialized = 0;

		__insert_var_to_mem(v);
	}
}

void __unset_var(char *name) {
	mem->size--;
}

void __set_var_init(char *name) {
	__get_var(name)->is_initialized = 1;
}

void __print_memory() {
	struct Var *v;
	printf("--- Memory ---\n");
	if (mem == NULL) {
		printf("(empty)\n");
		return;
	}

	for (int i = 0; i < mem->size; i++) {
		v = mem->vars[i];

		printf("%d ... %s \n", v->memory_index, v->name);
	}
}
