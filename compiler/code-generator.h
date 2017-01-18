struct OutputCode *fill_reg_with_num(int num, int reg);
struct OutputCode *id_addr_to_reg(struct Id *, int);
struct OutputCode *new_output_code();
struct OutputCode *code_gen(struct Commands *);

struct OutputCode *value_to_reg(struct Value *v, int reg) {
	struct OutputCode *oc;

	if (strcmp(v->type, "num") == 0) {
		oc = fill_reg_with_num(v->num, 1);

	} else {
		oc = id_addr_to_reg(v->id, 0);
		oc = insert(oc, cmd("LOAD", 1));

	}

	return oc;
}

struct OutputCode *id_addr_to_reg(struct Id *id, int reg) {
	struct Var *v = __get_var(id->name);

	if (strcmp(id->type, "arr") == 0) {
		// tab[]
		if (id->index_id != NULL) {
			// tab[var]
			struct Var *vv = __get_var(id->index_id);
			struct OutputCode *oc = fill_reg_with_num(v->memory_index, 1);
			oc = merge(oc, fill_reg_with_num(vv->memory_index, 0));
			oc = insert(oc, cmd("ADD", 1));
			oc = insert(oc, cmd("COPY", 1));
			return oc;
		} else {
			// tab[192]
			return fill_reg_with_num(v->memory_index + id->index_num, 0);

		}
	} else {
		// xyz
		return fill_reg_with_num(v->memory_index, 0);

	}
}

struct OutputCode *process_expression(struct Expression *e) {
	struct OutputCode *oc = new_output_code();
	struct Value *v1, *v2;
	int val;
	v1 = e->val1;
	v2 = e->val2;

	if (strcmp(e->op, "+") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			oc = merge(oc, fill_reg_with_num(v1->num + v2->num, 1));

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("LOAD", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("ADD", 1));
		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			oc = merge(oc, fill_reg_with_num(v1->num, 1));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("ADD", 1));
		} else {
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = merge(oc, fill_reg_with_num(v2->num, 1));
			oc = insert(oc, cmd("ADD", 1));
		}

	} else if (strcmp(e->op, "-") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			val = v1->num - v2->num;
			val = val < 0? 0 : val;

			oc = merge(oc, fill_reg_with_num(val, 1));

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("LOAD", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));
		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			oc = merge(oc, fill_reg_with_num(v1->num, 1));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));
		} else {
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("LOAD", 1));

			oc = merge(oc, fill_reg_with_num(v2->num, 0));
			oc = insert(oc, jcmd("JZERO", 0, 4));
			oc = insert(oc, cmd("DEC", 0));
			oc = insert(oc, cmd("DEC", 1));
			oc = insert(oc, jscmd("JUMP", -3));
		}
	} else if (strcmp(e->op, "*") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			val = v1->num * v2->num;

			oc = merge(oc, fill_reg_with_num(val, 1));

		}

	} else if (strcmp(e->op, "/") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			if (v2->num == 0) {
				val = 0;
			} else {
				val = (v1->num - v1->num % v2->num) / v2->num;
			}

			val = val < 0? 0 : val;

			oc = merge(oc, fill_reg_with_num(val, 1));

		}

	} else if (strcmp(e->op, "%") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			val = v1->num % v2->num;

			oc = merge(oc, fill_reg_with_num(val, 1));

		}
	}

	return oc;
}

struct OutputCode *command_gen(struct Command *c) {
	struct OutputCode *oc = new_output_code();

	if (strcmp(c->type, "read") == 0) {
		/* OK */
		oc = merge(oc, id_addr_to_reg(c->id, 0));
		oc = insert(oc, cmd("GET", 1));
		oc = insert(oc, cmd("STORE", 1));

	} else if (strcmp(c->type, "write") == 0) {
		/* OK */
		oc = merge(oc, value_to_reg(c->val1, 1));
		oc = insert(oc, cmd("PUT", 1));

	} else if (strcmp(c->type, "skip") == 0) {
		/* OK */
	} else if (strcmp(c->type, "assign") == 0) {
		/* TODO: tutaj wyliczenie wartości expression do R1 */
		oc = merge(oc, process_expression(c->expr));
		oc = merge(oc, id_addr_to_reg(c->id, 0));
		oc = insert(oc, cmd("STORE", 1));

	} else if (strcmp(c->type, "if") == 0) {
		printf("IF\n");
		oc = merge(oc, code_gen(c->cmd1));
		oc = merge(oc, code_gen(c->cmd2));
	} else if (strcmp(c->type, "while") == 0) {
		printf("WHILE LOOP\n");
		oc = merge(oc, code_gen(c->cmd1));
	} else if (strcmp(c->type, "for") == 0) {
		printf("FOR LOOP with ITERATOR: %s\n", c->pid);
		oc = merge(oc, code_gen(c->cmd1));
	} else if (strcmp(c->type, "for-downto") == 0) {
		printf("FOR DOWNTO LOOP with ITERATOR: %s\n", c->pid);
		oc = merge(oc, code_gen(c->cmd1));
	} else {
		printf("Unknown command\n");
	}

	return oc;
}

struct OutputCode *code_gen(struct Commands *cmds) {
	struct Command *c;
	struct OutputCode *oc = new_output_code();

	if (cmds == NULL) {
		return oc;
	}

	for (int i = 0; i < cmds->cmds_size; i++) {
		c = cmds->cmds[i];
		//
		// if (strcmp(c->type, "read") == 0) {
		// 	/* OK */
		// 	oc = merge(oc, id_addr_to_reg(c->id, 0));
		// 	oc = insert(oc, cmd("GET", 1));
		// 	oc = insert(oc, cmd("STORE", 1));
		//
		// } else if (strcmp(c->type, "write") == 0) {
		// 	/* OK */
		// 	oc = merge(oc, value_to_reg(c->val1, 1));
		// 	oc = insert(oc, cmd("PUT", 1));
		//
		// } else if (strcmp(c->type, "skip") == 0) {
		// 	/* OK */
		// } else if (strcmp(c->type, "assign") == 0) {
		// 	/* TODO: tutaj wyliczenie wartości expression do R1 */
		// 	oc = merge(oc, id_addr_to_reg(c->id, 0));
		// 	oc = insert(oc, cmd("STORE", 1));
		//
		// } else if (strcmp(c->type, "if") == 0) {
		// 	printf("IF\n");
		// 	oc = merge(oc, code_gen(c->cmd1));
		// 	oc = merge(oc, code_gen(c->cmd2));
		// } else if (strcmp(c->type, "while") == 0) {
		// 	printf("WHILE LOOP\n");
		// 	oc = merge(oc, code_gen(c->cmd1));
		// } else if (strcmp(c->type, "for") == 0) {
		// 	printf("FOR LOOP with ITERATOR: %s\n", c->pid);
		// 	oc = merge(oc, code_gen(c->cmd1));
		// } else if (strcmp(c->type, "for-downto") == 0) {
		// 	printf("FOR DOWNTO LOOP with ITERATOR: %s\n", c->pid);
		// 	oc = merge(oc, code_gen(c->cmd1));
		// } else {
		// 	printf("Unknown command\n");
		// }
		oc = merge(oc, command_gen(c));
	}

	return oc;
}

struct OutputCode *fill_reg_with_num(int num, int reg) {
	struct OutputCode *f = malloc(sizeof(struct OutputCode));

	f->cmd_tree_size = 0;
	f->add_cmd_tree_size = 0;
	f->type = "num";
	f->value = num;

	int mod;
	int ix = 0;
	int bin[1024];
	int org_num = num;

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

struct OutputCode *new_output_code() {
	struct OutputCode *f = malloc(sizeof(struct OutputCode));

	f->cmd_tree_size = 0;
	f->add_cmd_tree_size = 0;
	f->type = NULL;
	f->value = 0;

	return f;
}
