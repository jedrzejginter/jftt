struct Var {
	char *name;				// nazwa zmiennej
	char *type;				// typ { arr, num }
	int size;				// rozmiar w pamięci
	long long int memory_index;		// indeks w pamięci
	long long int value;				// wartość
	int is_mutable;		// czy można przypisać do zmiennej?
	int is_initialized;	// czy zmienna zainicjalizowana? 0 false, 1 true
	int is_definite;		// czy znamy wartość? 0 false, 1 true
};

struct Memory {
	struct Var *vars[4096];
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


void __declare_var(char *name, long long int array_size, int is_mutable, char *type) {
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
	struct Var *v = __get_var(name);

	if (v != NULL) {
		v->is_initialized = 1;
	}
}

void __set_var_undef(char *name) {
	struct Var *v = __get_var(name);

	if (v != NULL) {
		v->is_definite = 0;
	}
}
