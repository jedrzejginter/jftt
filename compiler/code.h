// struct Sym {
// 	struct Sym *prev;
// 	char *name;
// 	char *type_name;
// 	int memory_index;
// 	int size;
// };

struct OutputCode {
	char *type;
	char *name;
	int value;
	struct RMLine *cmd_tree[1024];
	struct RMLine *add_cmd_tree[1024];
	int cmd_tree_size;
	int add_cmd_tree_size;
	int swap_blocks;
};

struct RMLine {
	char *cmd;
	int reg;
	int line;
	int is_jump_cmd;
	struct RMLine *prev;
};

typedef struct RMLine Command;
// typedef struct Sym Sym;
typedef struct Registers Registers;

int RM_LINE = 0;

// Sym *symlist = NULL;

struct OutputCode *load_num_to_register(int, int);
struct RMLine *cmd(char *, int);
struct RMLine *jcmd(char *, int, int);
struct RMLine *jscmd(char *, int);
struct OutputCode *insert(struct OutputCode *, struct RMLine *);
void print_line(char *);

// void add_sym(Sym *s) {
// 	if (symlist == NULL) {
// 		s->memory_index = 0;
// 		s->prev = NULL;
//
// 	} else {
// 		s->memory_index = symlist->memory_index + symlist->size;
// 		s->prev = symlist;
//
// 	}
//
// 	symlist = s;
// }

// Sym *get_sym(char *name) {
// 	Sym *s = symlist;
//
// 	while (s != NULL) {
// 		if (strcmp(s->name, name) == 0) {
// 			break;
// 		}
//
// 		s = s->prev;
// 	}
//
// 	return s;
// }
//
// void uninstall_sym(char *name) {
// 	Sym *s = get_sym(name);
//
// 	if (s == NULL) {
// 		register_error();
// 		printf("Nieznana zmianna: `%s`\n", name);
// 		return;
// 	}
//
// 	symlist = s->prev;
// 	free(s);
// }
//
// void install_sym(char *name, int size, char *type_name) {
// 	if (get_sym(name) != NULL) {
// 		register_error();
// 		printf("Symbol `%s` już został zadeklarowany\n", name);
// 		return;
// 	}
//
// 	Sym *s = (Sym *)malloc(sizeof(Sym));
//
// 	s->name = name;
// 	s->size = size;
// 	s->type_name = type_name;
//
// 	add_sym(s);
// }

// struct OutputCode *load_sym_to_register(char *name, int reg) {
// 	Sym *s = get_sym(name);
//
// 	if (s == NULL) {
// 		register_error();
// 		printf("Nieznany symbol: `%s`\n", name);
// 		return NULL;
// 	} else {
// 		struct OutputCode *f = load_num_to_register(s->memory_index, reg);
// 		f->name = name;
// 		f->type = "id";
// 		return f;
// 	}
// }

// struct OutputCode *load_tab_sym_to_register(char *name, int index, int reg) {
// 	Sym *s = get_sym(name);
//
// 	if (s == NULL) {
// 		register_error();
// 		printf("Nieznany symbol: `%s`\n", name);
// 		return NULL;
// 	} else {
// 		if (strcmp(s->type_name, "array") != 0) {
// 			register_error();
// 			printf("Symbol `%s` nie jest typu tablicowego\n", name);
// 			return NULL;
//
// 		} else if (index < 0) {
// 			register_error();
// 			printf("Nieprawidłowy indeks tablicy `%s`: %d\n", name, index);
// 			return NULL;
//
// 		} else {
// 			if (s->memory_index + index > s->memory_index + s->size - 1) {
// 				register_error();
// 				printf("Indeks poza zakresem tablicy `%s`: %d\n", name, index);
// 				return NULL;
// 			} else {
// 				struct OutputCode *f = load_num_to_register(s->memory_index + index, reg);
// 				f->type = "id";
// 				f->name = name;
//
// 				return f;
// 			}
// 		}
// 	}
// }

struct OutputCode *merge(struct OutputCode *a, struct OutputCode *b) {
	for (int i = 0; i < b->cmd_tree_size; i++) {
		a = insert(a, b->cmd_tree[i]);
	}

	return a;
}

struct OutputCode *merge_add_tree(struct OutputCode *a, struct OutputCode *b) {
	for (int i = 0; i < b->add_cmd_tree_size; i++) {
		a = insert(a, b->add_cmd_tree[i]);
	}

	return a;
}

struct OutputCode *insert(struct OutputCode *f, struct RMLine *c) {
	f->cmd_tree[f->cmd_tree_size++] = c;
	return f;
}

struct OutputCode *insert_add(struct OutputCode *f, struct RMLine *c) {
	f->add_cmd_tree[f->add_cmd_tree_size++] = c;
	return f;
}

struct OutputCode *reg_overwrite(struct OutputCode *f, int regToOverwrite, int regToSet) {
	for (int i = 0; i < f->cmd_tree_size; i++) {
		if (f->cmd_tree[i]->reg == regToOverwrite) {
			f->cmd_tree[i]->reg = regToSet;
		}
	}

	return f;
}

// struct OutputCode *load_tab_id_index_to_register(char *tab_name, char* ix_name, int reg) {
// 	Sym *tab_s = get_sym(tab_name);
// 	Sym *ix_s = get_sym(ix_name);
//
// 	if (tab_s == NULL) {
// 		register_error();
// 		printf("Nieznany symbol: `%s`\n", tab_name);
// 		return NULL;
// 	} else if (ix_s == NULL) {
// 		register_error();
// 		printf("Nieznany symbol: `%s`\n", ix_name);
// 		return NULL;
// 	} else {
// 		struct OutputCode *f = load_sym_to_register(tab_name, 1);
// 		struct OutputCode *g = load_sym_to_register(ix_name, 0);
//
// 		f = merge(f, g);
// 		f->type = "id";
// 		f->name = tab_name;
//
// 		f = insert(f, cmd("ADD", 1));
// 		f = insert(f, cmd("COPY", 1));
//
// 		return g;
// 	}
// }

struct RMLine *cmd(char *command, int reg) {
	struct RMLine *c = malloc(sizeof(struct RMLine));

	c->is_jump_cmd = 0;
	c->cmd = command;
	c->prev = NULL;
	c->reg = reg;
	c->line = -1;

	return c;
}

struct RMLine *jcmd(char *command, int reg, int line) {
	struct RMLine *c = malloc(sizeof(struct RMLine));

	c->is_jump_cmd = 1;
	c->cmd = command;
	c->prev = NULL;
	c->reg = reg;
	c->line = line;

	return c;
}

struct RMLine *jscmd(char *command, int line) {
	struct RMLine *c = malloc(sizeof(struct RMLine));

	c->is_jump_cmd = 1;
	c->cmd = command;
	c->prev = NULL;
	c->reg = -2;
	c->line = line;

	return c;
}

struct OutputCode *new_foobar() {
	struct OutputCode *f = malloc(sizeof(struct OutputCode));

	f->cmd_tree_size = 0;
	f->add_cmd_tree_size = 0;
	f->type = NULL;
	f->value = 0;
	f->swap_blocks = 0;

	return f;
}

struct OutputCode *load_num_to_register(int num, int reg) {
	struct OutputCode *f = malloc(sizeof(struct OutputCode));
	int mod;
	int ix = 0;
	int bin[1024];
	int org_num = num;

	f->cmd_tree_size = 0;
	f->add_cmd_tree_size = 0;
	f->type = "num";
	f->value = num;
	f->swap_blocks = 0;

	f = insert(f, cmd("ZERO", reg));

	if (num > 0) {

		while (num > 0) {
			mod = num % 2;
			num = (num - mod) / 2;

			bin[ix] = mod;
			ix++;
		}

		for (int i = ix - 1; i >= 0; i--) {
			if (i != ix - 1) {
				f = insert(f, cmd("SHL", reg));
			}

			if (bin[i] == 1) {
				f = insert(f, cmd("INC", reg));
			}
		}
	}

	return f;
}

void print_line(char *l) {
	printf("%s\n", l);
	RM_LINE += 1;
}

void print_cmd_tree(struct OutputCode *f, int reg) {
	char buf[128];

	for (int i = 0; i < f->cmd_tree_size; i++) {
		int r = reg;

		if (f->cmd_tree[i]->reg >= 0) {
			r = f->cmd_tree[i]->reg;
		}

		if (f->cmd_tree[i]->is_jump_cmd == 1) {
			if (f->cmd_tree[i]->reg == -2) {
				sprintf(buf, "%s %d", f->cmd_tree[i]->cmd, f->cmd_tree[i]->line + RM_LINE);
			} else {
				sprintf(buf, "%s %d %d", f->cmd_tree[i]->cmd, r, f->cmd_tree[i]->line + RM_LINE);
			}
		} else {
			sprintf(buf, "%s %d", f->cmd_tree[i]->cmd, r);
		}

		print_line(buf);
	}

	print_line("HALT");
}
