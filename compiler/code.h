
struct OutputCode {
	char *type;
	char *name;
	int value;
	struct RMLine *cmd_tree[10000];
	struct RMLine *add_cmd_tree[10000];
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

int RM_LINE = 0;

//struct OutputCode *load_num_to_register(long long int, int);
struct OutputCode *insert(struct OutputCode *, struct RMLine *);
struct RMLine *cmd(char *, int);
struct RMLine *jcmd(char *, int, int);
struct RMLine *jscmd(char *, int);
void print_line(FILE *, char *);

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


void print_line(FILE *fp, char *l) {
	fprintf(fp, "%s\n", l);
	RM_LINE += 1;
}

void print_cmd_tree(char *output_file, struct OutputCode *f, int reg) {
	char buf[128];
	FILE *fp = fopen(output_file, "w");

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

		print_line(fp, buf);
	}

	print_line(fp, "HALT");
	fclose(fp);
}
