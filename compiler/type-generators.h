struct Declaration *__Declaration(char *name, int size) {
	struct Declaration *d = malloc(sizeof(struct Declaration));

	d->name = name;
	d->size = size;
	d->next = NULL;

	return d;
}

struct Declarations *__Declarations(struct Declarations *current, struct Declaration *d) {
	printf("-1- \n");
	if (current == NULL) {
		current = malloc(sizeof(struct Declarations));
	}
	printf("-2- \n");

	if (current->root == NULL) {
		printf("-2a-pre- \n");
		current->root = d;
		current->last = d;
		printf("-2a- \n");

	} else if (d != NULL) {
		printf("-2b-pre- \n");
		current->last->next = d;
		current->last = d;
		printf("-2b- \n");
	}

	printf("-3- \n");

	return current;
}

struct Id *__Id(char *name, int index_num, char *index_id) {
	struct Id *e = malloc(sizeof(struct Id));

	e->name = name;
	e->index_num = index_num;
	e->index_id = index_id;

	if (index_num > 0 || index_id != NULL) {
		e->type = "arr";
	} else {
		e->type = "num";
	}

	return e;
}

struct Value *__Value(char *type, int num, struct Id *id) {
	struct Value *e = malloc(sizeof(struct Value));

	e->type = type;
	e->id = id;
	e->num = num;

	return e;
}

struct Condition *__Condition(struct Value *v1, struct Value *v2, char *rel) {
	struct Condition *c = malloc(sizeof(struct Condition));

	c->val1 = v1;
	c->val2 = v2;
	c->rel = rel;

	return c;
}

struct Expression *__Expression(struct Value *v1, struct Value *v2, char *op) {
	struct Expression *e = malloc(sizeof(struct Expression));

	e->val1 = v1;
	e->val2 = v2;
	e->op = op;

	return e;
}

struct Command *__Command(char *type) {
	struct Command *c = malloc(sizeof(struct Command));
	c->type = type;

	return c;
}

struct Commands *__Commands(struct Commands *current, struct Command *cmd) {
	if (current == NULL) {
		current = malloc(sizeof(struct Commands));
	}

	if (current->root == NULL) {
		current->root = cmd;
		current->last = cmd;
		current->size++;

	} else if (cmd != NULL) {
		current->last->next = cmd;
		current->last = cmd;
		current->size++;

	}

	return current;
}

struct Commands *__insert_command(struct Commands *cmds, struct Command *cmd) {
	if (cmds == NULL) {
		cmds = malloc(sizeof(struct Commands));
	}

	cmds->cmds[cmds->cmds_size++] = cmd;
	return cmds;
}
