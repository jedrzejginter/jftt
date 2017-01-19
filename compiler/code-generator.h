struct NumAsBinary {
	int bin[1024];
	int size;
};

struct OutputCode *fill_reg_with_num(int num, int reg);
struct OutputCode *id_addr_to_reg(struct Id *, int);
struct OutputCode *new_output_code();
struct OutputCode *code_gen(struct Commands *);

int while_loop_id_cnt = 0;

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
			/*
			ok
			*/
			oc = merge(oc, fill_reg_with_num(v1->num + v2->num, 1));

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			*/
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("LOAD", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("ADD", 1));
		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			*/
			oc = merge(oc, fill_reg_with_num(v1->num, 1));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("ADD", 1));
		} else {
			/*
			ok
			*/
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

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "num") == 0) {
			oc = fill_reg_with_num(v2->num, 4);
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = merge(oc, fill_reg_with_num(0, 1));
			oc = insert(oc, jcmd("JZERO", 4, 4));
			oc = insert(oc, cmd("ADD", 1));
			oc = insert(oc, cmd("DEC", 4));
			oc = insert(oc, jscmd("JUMP", -3));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			oc = fill_reg_with_num(v1->num, 4);
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = merge(oc, fill_reg_with_num(0, 1));
			oc = insert(oc, jcmd("JZERO", 4, 4));
			oc = insert(oc, cmd("ADD", 1));
			oc = insert(oc, cmd("DEC", 4));
			oc = insert(oc, jscmd("JUMP", -3));

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			oc = id_addr_to_reg(v2->id, 0);
			oc = insert(oc, cmd("LOAD", 4));
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = merge(oc, fill_reg_with_num(0, 1));
			oc = insert(oc, jcmd("JZERO", 4, 4));
			oc = insert(oc, cmd("ADD", 1));
			oc = insert(oc, cmd("DEC", 4));
			oc = insert(oc, jscmd("JUMP", -3));

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

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "num") == 0) {
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, cmd("LOAD", 1));
			oc = insert(oc, cmd("SHR", 1));

		}

	} else if (strcmp(e->op, "%") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			val = v1->num % v2->num;

			oc = merge(oc, fill_reg_with_num(val, 1));

		}
	}

	return oc;
}

struct OutputCode *process_condition(struct Condition *c) {
	struct OutputCode *oc = new_output_code();
	struct Value *v1, *v2;
	v1 = c->val1;
	v2 = c->val2;

	if (strcmp(c->rel, "=") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			10 = 16
			*/
			int val;

			if (v1->num == v2->num) {
				val = 1;
			} else {
				val = 0;
			}

			oc = fill_reg_with_num(val, 1);
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a = b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, cmd("LOAD", 1));
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));

			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("DEC", 1));
			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("INC", 1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 = b
			*/
			oc = fill_reg_with_num(v1->num, 1);
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));

			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("DEC", 1));
			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("INC", 1));

		} else {
			/*
			ok
			a = 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("SUB", 1));

			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("DEC", 1));
			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("INC", 1));
		}
	} else if (strcmp(c->rel, "<>") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			int val;

			if (v1->num != v2->num) {
				val = 1;
			} else {
				val = 0;
			}

			oc = fill_reg_with_num(val, 1);
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a <> b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, cmd("LOAD", 1));
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));

			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("DEC", 1));
			oc = insert(oc, jcmd("JZERO", 1, 4));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, cmd("INC", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("ZERO", 1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 <> a
			*/
			oc = fill_reg_with_num(v1->num, 1);
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));

			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("DEC", 1));
			oc = insert(oc, jcmd("JZERO", 1, 4));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, cmd("INC", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("ZERO", 1));

		} else {
			/*
			ok
			a <> 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("SUB", 1));

			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("DEC", 1));
			oc = insert(oc, jcmd("JZERO", 1, 4));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, cmd("INC", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("ZERO", 1));
		}
	} else if (strcmp(c->rel, "<") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			int val = (v1->num < v2->num)? 1 : 0;

			oc = fill_reg_with_num(val, 1);

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a < b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, cmd("LOAD", 1));
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));
			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("INC", 1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 < a
			*/
			oc = fill_reg_with_num(v1->num, 1);
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));
			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("INC", 1));

		} else {
			/*
			ok
			a < 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = insert(oc, cmd("DEC", 1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("SUB", 1));

			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, cmd("INC", 1));
		}
	} else if (strcmp(c->rel, ">") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			int val = (v1->num > v2->num)? 1 : 0;

			oc = fill_reg_with_num(val, 1);

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a > b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, cmd("LOAD", 1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));
			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, cmd("INC", 1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 > b
			*/
			oc = fill_reg_with_num(v1->num, 1);

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));
			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, cmd("INC", 1));

		} else {
			/*
			ok
			a > 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = insert(oc, cmd("INC", 1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("SUB", 1));
			oc = insert(oc, jcmd("JZERO", 1, 3));
			oc = insert(oc, cmd("ZERO", 1));
			oc = insert(oc, jscmd("JUMP", 2));
			oc = insert(oc, cmd("INC", 1));
		}

	} else if (strcmp(c->rel, "<=") == 0) {
		if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			oc = id_addr_to_reg(v2->id, 0);
			oc = insert(oc, cmd("LOAD", 1));
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("INC", 1));
			oc = insert(oc, cmd("SUB", 1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			int val = (v1->num <= v2->num)? 1 : 0;

			oc = fill_reg_with_num(val, 1);
		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			oc = fill_reg_with_num(v1->num, 1);
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("SUB", 1));

			oc->swap_blocks = 1;

		} else {
			oc = id_addr_to_reg(v1->id, 0);
			oc = merge(oc, fill_reg_with_num(v2->num, 1));
			oc = insert(oc, cmd("INC", 1));
			oc = insert(oc, cmd("SUB", 1));
		}
	} else {
		if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, cmd("LOAD", 1));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("INC", 1));
			oc = insert(oc, cmd("SUB", 1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			int val = (v1->num >= v2->num)? 1 : 0;

			oc = fill_reg_with_num(val, 1);
		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			oc = fill_reg_with_num(v1->num, 1);
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("INC", 1));
			oc = insert(oc, cmd("SUB", 1));

		} else {
			oc = id_addr_to_reg(v1->id, 0);
			oc = merge(oc, fill_reg_with_num(v2->num, 1));
			oc = insert(oc, cmd("SUB", 1));

			oc->swap_blocks = 1;
		}
	}

	return oc;
}

struct OutputCode *command_gen(struct Command *c) {
	struct OutputCode *oc = new_output_code();
	struct Var *var;

	if (strcmp(c->type, "read") == 0) {
		/* OK */
		var = __get_var(c->id->name);
		if (var == NULL) {
			printf("Niezadeklarowana zmienna: %s\n", c->id->name);
			exit(1);
		}

		oc = merge(oc, id_addr_to_reg(c->id, 0));
		oc = insert(oc, cmd("GET", 1));
		oc = insert(oc, cmd("STORE", 1));

	} else if (strcmp(c->type, "write") == 0) {
		/* OK */
		if (strcmp(c->val1->type, "id") == 0) {
			var = __get_var(c->val1->id->name);
			if (var == NULL) {
				printf("Niezadeklarowana zmienna: %s\n", c->val1->id->name);
				exit(1);
			}
		}

		oc = merge(oc, value_to_reg(c->val1, 1));
		oc = insert(oc, cmd("PUT", 1));

	} else if (strcmp(c->type, "skip") == 0) {
		/* OK */

	} else if (strcmp(c->type, "assign") == 0) {
		/* OK */
		var = __get_var(c->id->name);
		if (var->is_mutable == 0) {
			printf("Iterator pętli `%s` nie może być modyfikowany!\n", c->id->name);
			exit(1);
		}

		oc = merge(oc, process_expression(c->expr));
		oc = merge(oc, id_addr_to_reg(c->id, 0));
		oc = insert(oc, cmd("STORE", 1));

	} else if (strcmp(c->type, "if") == 0) {
		/* OK, TODO: przetestować */
		struct Commands *cmd1, *cmd2;
		struct OutputCode *oc_cmd1, *oc_cmd2, *oc_cond;
		oc_cond = process_condition(c->cond);

		oc = merge(oc, oc_cond);

		cmd1 = c->cmd1;
		cmd2 = c->cmd2;

		if (oc_cond->swap_blocks == 1) {
			cmd1 = c->cmd2;
			cmd2 = c->cmd1;
		}

		oc_cmd1 = code_gen(cmd1);
		oc_cmd2 = code_gen(cmd2);

		int raw_ln = oc_cmd1->cmd_tree_size + 2;
		int ln = raw_ln + oc->add_cmd_tree_size + 1;

		oc = insert(oc, jcmd("JZERO", 1, ln));

		if (oc->add_cmd_tree_size > 0) {
			oc = merge_add_tree(oc, oc);
			oc = insert(oc, jscmd("JUMP", raw_ln));
		}

		oc = merge(oc, oc_cmd1);

		oc = insert(oc, jscmd("JUMP", oc_cmd2->cmd_tree_size + 1));
		oc = merge(oc, oc_cmd2);

	} else if (strcmp(c->type, "while") == 0) {
		struct OutputCode *oc2, *oc_cond, *oc_cmd;
		char loop_id[16];

		sprintf(loop_id, "_while_%d", while_loop_id_cnt);
		while_loop_id_cnt++;

		struct Id *lid = __Id(loop_id, 0, NULL);

		__declare_var(loop_id, 0, 0);

		oc_cond = process_condition(c->cond);
		oc_cmd = code_gen(c->cmd1);

		int end_jump_ln = oc_cond->cmd_tree_size;

		oc = reg_overwrite(oc_cond, 1, 3);

		oc2 = id_addr_to_reg(lid, 0);
		oc2 = insert(oc2, cmd("STORE", 3));
		oc2 = merge(oc2, oc_cmd);
		oc2 = merge(oc2, id_addr_to_reg(lid, 0));
		oc2 = insert(oc2, cmd("LOAD", 3));
		oc2 = insert(oc2, cmd("DEC", 3));
		oc2 = insert(oc2, cmd("STORE", 3));

		int jumps = 1;
		if (oc_cond->swap_blocks == 1) {
			oc = insert(oc, jcmd("JZERO", 3, 2));
			oc = insert(oc, jscmd("JUMP", oc2->cmd_tree_size + 2));
			jumps = 2;

		} else {
			oc = insert(oc, jcmd("JZERO", 3, oc2->cmd_tree_size + 2));
		}

		oc = merge(oc, oc2);
		oc = insert(oc, jscmd("JUMP", -end_jump_ln - oc2->cmd_tree_size + 1 - jumps));

		__unset_var(loop_id);

	} else if (strcmp(c->type, "for") == 0) {
		struct Value *v1, *v2;
		struct OutputCode *oc2;
		char reg_cpy[16];

		__declare_var(c->pid, 0, 0);

		v1 = c->val1;
		v2 = c->val2;
		sprintf(reg_cpy, "_for_%s", c->pid);

		__declare_var(reg_cpy, 0, 0);

		struct Id *id = __Id(c->pid, 0, NULL);

		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			FROM 3 TO 8
			*/
			int val = v2->num - v1->num + 1;
			val = val < 0? 0 : val;

			oc = fill_reg_with_num(val, 2);
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = merge(oc, fill_reg_with_num(v1->num, 1));
			oc = insert(oc, cmd("STORE", 1));
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			FROM 3 TO 11
			*/
			oc = id_addr_to_reg(v2->id, 0);
			oc = insert(oc, cmd("LOAD", 2));
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("LOAD", 1));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, cmd("STORE", 1));
			oc = insert(oc, cmd("INC", 2));
			oc = insert(oc, cmd("SUB", 2));

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			FROM a TO 17
			*/
			oc = fill_reg_with_num(v2->num, 2);
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, cmd("LOAD", 1));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, cmd("STORE", 1));
			oc = insert(oc, cmd("INC", 2));
			oc = insert(oc, cmd("SUB", 2));

		} else {
			/*
			ok
			FROM 7 TO a
			*/
			oc = id_addr_to_reg(v2->id, 0);
			oc = insert(oc, cmd("LOAD", 2));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = merge(oc, fill_reg_with_num(v1->num, 1));
			oc = insert(oc, cmd("STORE", 1));
			oc = insert(oc, cmd("INC", 2));
			oc = insert(oc, cmd("SUB", 2));

		}

		struct Id *reg_cpy_id = __Id(reg_cpy, 0, NULL);

		oc2 = id_addr_to_reg(reg_cpy_id, 0);
		oc2 = insert(oc2, cmd("STORE", 2));
		oc2 = merge(oc2, code_gen(c->cmd1));
		oc2 = merge(oc2, id_addr_to_reg(reg_cpy_id, 0));
		oc2 = insert(oc2, cmd("LOAD", 2));
		oc2 = merge(oc2, id_addr_to_reg(id, 0));
		oc2 = insert(oc2, cmd("LOAD", 1));
		oc2 = insert(oc2, cmd("INC", 1));
		oc2 = insert(oc2, cmd("STORE", 1));

		oc = insert(oc, jcmd("JZERO", 2, oc2->cmd_tree_size + 3));
		oc = merge(oc, oc2);
		oc = insert(oc, cmd("DEC", 2));
		oc = insert(oc, jscmd("JUMP", - (oc2->cmd_tree_size + 2)));

		__unset_var(reg_cpy);
		__unset_var(c->pid);

	} else if (strcmp(c->type, "for-downto") == 0) {
		struct Value *v1, *v2;
		struct OutputCode *oc2;
		char reg_cpy[16];

		__declare_var(c->pid, 0, 0);

		v1 = c->val1;
		v2 = c->val2;
		sprintf(reg_cpy, "_for_%s", c->pid);

		__declare_var(reg_cpy, 0, 0);

		struct Id *id = __Id(c->pid, 0, NULL);

		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			FROM 10 DOWNTO 2
			*/
			int val = v1->num - v2->num + 1;
			val = val < 0? 0 : val;

			oc = fill_reg_with_num(val, 2);
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = merge(oc, fill_reg_with_num(v1->num, 1));
			oc = insert(oc, cmd("STORE", 1));
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			FROM a DOWNTO 1
			*/
			__declare_var("_tmp0", 0, 0);
			struct Id *tmp_id = __Id("_tmp0", 0, NULL);
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, cmd("LOAD", 2));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("LOAD", 1));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, cmd("STORE", 2));
			oc = merge(oc, id_addr_to_reg(tmp_id, 0));
			oc = insert(oc, cmd("STORE", 1));
			oc = insert(oc, cmd("INC", 2));
			oc = insert(oc, cmd("SUB", 2));
			__unset_var("_tmp0");

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			FROM a DOWNTO 1
			*/
			__declare_var("_tmp0", 0, 0);
			struct Id *tmp_id = __Id("_tmp0", 0, NULL);
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, cmd("LOAD", 2));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, cmd("STORE", 2));
			oc = merge(oc, fill_reg_with_num(v2->num, 1));
			oc = merge(oc, id_addr_to_reg(tmp_id, 0));
			oc = insert(oc, cmd("STORE", 1));
			oc = insert(oc, cmd("INC", 2));
			oc = insert(oc, cmd("SUB", 2));
			__unset_var("_tmp0");

		} else {
			/*
			ok
			FROM 10 DOWNTO a
			*/
			oc = fill_reg_with_num(v1->num, 2);
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, cmd("STORE", 2));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, cmd("INC", 2));
			oc = insert(oc, cmd("SUB", 2));

		}

		struct Id *reg_cpy_id = __Id(reg_cpy, 0, NULL);

		oc2 = id_addr_to_reg(reg_cpy_id, 0);
		oc2 = insert(oc2, cmd("STORE", 2));
		oc2 = merge(oc2, code_gen(c->cmd1));
		oc2 = merge(oc2, id_addr_to_reg(reg_cpy_id, 0));
		oc2 = insert(oc2, cmd("LOAD", 2));
		oc2 = merge(oc2, id_addr_to_reg(id, 0));
		oc2 = insert(oc2, cmd("LOAD", 1));
		oc2 = insert(oc2, cmd("DEC", 1));
		oc2 = insert(oc2, cmd("STORE", 1));

		oc = insert(oc, jcmd("JZERO", 2, oc2->cmd_tree_size + 3));
		oc = merge(oc, oc2);
		oc = insert(oc, cmd("DEC", 2));
		oc = insert(oc, jscmd("JUMP", - (oc2->cmd_tree_size + 2)));

		__unset_var(reg_cpy);
		__unset_var(c->pid);
	}

	return oc;
}

struct OutputCode *code_gen(struct Commands *cmds) {
	struct OutputCode *oc = new_output_code();

	if (cmds != NULL) {
		for (int i = 0; i < cmds->cmds_size; i++) {
			oc = merge(oc, command_gen(cmds->cmds[i]));
		}
	}

	return oc;
}

// struct NumAsBinary num_to_bin(int num) {
// 	struct NumAsBinary bn = malloc(sizeof(struct NumAsBinary));
//
// 	int mod;
//
// 	while (num > 0) {
// 		mod = num % 2;
// 		num = (num - mod) / 2;
//
// 		bn->bin[bn->size++] = mod;
// 	}
//
// 	return bn;
// }

struct OutputCode *fill_reg_with_num(int num, int reg) {
	struct OutputCode *f = malloc(sizeof(struct OutputCode));

	f->cmd_tree_size = 0;
	f->add_cmd_tree_size = 0;
	f->type = "num";
	f->value = num;

	int mod;
	int ix = 0;
	int bin[1024];
	//int org_num = num;

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
