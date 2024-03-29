#include <math.h>

struct OutputCode *fill_reg_with_num(int num, int reg);
struct OutputCode *id_addr_to_reg(struct Id *, int);
struct OutputCode *new_output_code();
struct OutputCode *code_gen(struct Commands *);

int while_loop_id_cnt = 0;

struct OutputCode *id_addr_to_reg(struct Id *id, int reg) {
	struct OutputCode *oc = new_output_code();
	struct Var *v = __get_var(id->name);

	if (strcmp(id->type, "arr") == 0) {
		if (strcmp(v->type, "num") == 0) {
			__err_wrong_use(id->ln, v->name);
			exit(1);
		}

		// tab[]
		if (id->index_id != NULL) {
			// tab[var]
			struct Var *vv = __get_var(id->index_id);

			if (vv == NULL) {
				__err_undecl_var(id->ln, id->index_id);
				return oc;
			} else if (vv->is_initialized == 0) {
				__err_uninit_var(id->ln, vv->name);
				return oc;

			} else {

				oc = fill_reg_with_num(v->memory_index, 4);
				oc = merge(oc, fill_reg_with_num(vv->memory_index, 0));
				oc = insert(oc, __ADD(4));
				oc = insert(oc, __COPY(4));
				return oc;
			}

		} else {
			if (id->index_num >= v->size) {
				__err_bad_array_ix(id->ln, id->index_num, v->name);
				exit(1);
			}

			// tab[192]
			return fill_reg_with_num(v->memory_index + id->index_num, reg);

		}
	} else {
		// xyz
		if (strcmp(v->type, "arr") == 0) {
			__err_wrong_use(id->ln, v->name);
			exit(1);
		}

		return fill_reg_with_num(v->memory_index, reg);
	}
}

struct OutputCode *process_expression(struct Expression *e) {
	struct OutputCode *oc = new_output_code();
	struct Value *v1, *v2;
	int val;
	v1 = e->val1;
	v2 = e->val2;

	if (e->op == NULL) {

		if (strcmp(v1->type, "num") == 0) {
			/*
			ok
			*/
			oc = fill_reg_with_num(v1->num, 1);
		} else {
			/*
			ok
			*/
			struct Var *var = __get_var(v1->id->name);

			if (var == NULL) {
				__err_undecl_var(v1->id->ln, v1->id->name);
				exit(1);

			} else if (var->is_initialized == 0) {
				__err_uninit_var(v1->id->ln, var->name);

			} else {

				oc = id_addr_to_reg(v1->id, 0);
				oc = insert(oc, __LOAD(1));
			}
		}

	} else {
		struct Var *var;

		if (strcmp(v1->type, "id") == 0) {
			var = __get_var(v1->id->name);

			if (var == NULL) {
				__err_undecl_var(v1->id->ln, v1->id->name);
				exit(1);

			} else if (var->is_initialized == 0) {
				__err_uninit_var(v1->id->ln, var->name);
				return oc;

			}
		}

		if (strcmp(v2->type, "id") == 0) {
			var = __get_var(v2->id->name);

			if (var == NULL) {
				__err_undecl_var(v2->id->ln, v2->id->name);
				exit(1);

			} else if (var->is_initialized == 0) {
				__err_uninit_var(v2->id->ln, var->name);
				return oc;
			}
		}

		if (strcmp(e->op, "+") == 0) {
			if (strcmp(v1->type, "id") == 0) {
				oc = id_addr_to_reg(v1->id, 0);
				oc = insert(oc, __LOAD(1));

			} else {
				oc = fill_reg_with_num(v1->num, 1);

			}

			if (strcmp(v2->type, "id") == 0) {
				oc = merge(oc, id_addr_to_reg(v2->id, 0));

			} else {
				oc = merge(oc, fill_reg_with_num(v2->num, 4));
				oc = insert(oc, __ZERO(0));
				oc = insert(oc, __STORE(4));

			}

			oc = insert(oc, __ADD(1));


		} else if (strcmp(e->op, "-") == 0) {
			if (strcmp(v1->type, "id") == 0) {
				oc = id_addr_to_reg(v1->id, 0);
				oc = insert(oc, __LOAD(1));

			} else {
				oc = fill_reg_with_num(v1->num, 1);

			}

			if (strcmp(v2->type, "id") == 0) {
				oc = merge(oc, id_addr_to_reg(v2->id, 0));

			} else {
				oc = merge(oc, fill_reg_with_num(v2->num, 4));
				oc = insert(oc, __ZERO(0));
				oc = insert(oc, __STORE(4));

			}

			oc = insert(oc, __SUB(1));

		} else if (strcmp(e->op, "*") == 0) {
			/*
			OK
			Mnożenie szybkie, przesunięciami bitowymi.
			Rozmiar danych nie ma dużego wpływu na wyniki.
			*/

			if (strcmp(v1->type, "id") == 0) {
				oc = id_addr_to_reg(v1->id, 0);
				oc = insert(oc, __LOAD(1));
			} else {
				oc = fill_reg_with_num(v1->num, 1);
			}

			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __STORE(1));

			if (strcmp(v2->type, "id") == 0) {
				oc = merge(oc, id_addr_to_reg(v2->id, 0));
				oc = insert(oc, __LOAD(1));
			} else {
				oc = merge(oc, fill_reg_with_num(v2->num, 1));
			}

			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __INC(0));
			oc = insert(oc, __STORE(1));

			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __SUB(1));

			oc = insert(oc, __JZERO(1, 5));
			oc = insert(oc, __LOAD(4));
			oc = insert(oc, __INC(0));
			oc = insert(oc, __LOAD(1));
			oc = insert(oc, __JUMP(5));

			oc = insert(oc, __LOAD(1));
			oc = insert(oc, __INC(0));
			oc = insert(oc, __LOAD(4));
			oc = insert(oc, __STORE(1));

			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __STORE(0));

			oc = insert(oc, __JZERO(4, 14));
				oc = insert(oc, __JODD(4, 2));
					oc = insert(oc, __JUMP(9));

					oc = insert(oc, __ZERO(0));
					oc = insert(oc, __INC(0));
					oc = insert(oc, __STORE(1));

					oc = insert(oc, __ZERO(0));
					oc = insert(oc, __ADD(1));
					oc = insert(oc, __STORE(1));

					oc = insert(oc, __INC(0));
					oc = insert(oc, __LOAD(1));

				oc = insert(oc, __SHL(1));
				oc = insert(oc, __SHR(4));
			oc = insert(oc, __JUMP(-13));

			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __LOAD(1));


		} else if (strcmp(e->op, "/") == 0) {
			struct OutputCode *oc2, *oc3;

			if (strcmp(v2->type, "id") == 0) {
				oc = id_addr_to_reg(v2->id, 0);
				oc = insert(oc, __LOAD(4));
			} else {
				oc = fill_reg_with_num(v2->num, 4);
			}

			oc3 = fill_reg_with_num(1, 0);
			oc3 = insert(oc3, __STORE(4));
			oc3 = insert(oc3, __INC(0));
			oc3 = insert(oc3, __INC(0));
			oc3 = insert(oc3, __STORE(4));

			// p2 = 0
			oc3 = insert(oc3, __ZERO(4));
			oc3 = insert(oc3, __DEC(0));
			oc3 = insert(oc3, __STORE(4));

			if (strcmp(v1->type, "id") == 0) {
				oc3 = merge(oc3, id_addr_to_reg(v1->id, 0));
				oc3 = insert(oc3, __LOAD(4));
			} else {
				oc3 = merge(oc3, fill_reg_with_num(v1->num, 4));
			}

			oc3 = insert(oc3, __ZERO(0));
			oc3 = insert(oc3, __STORE(4));

			oc2 = fill_reg_with_num(1, 1);
			oc2 = insert(oc2, __ZERO(0)); //A
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __LOAD(4));
			oc2 = insert(oc2, __SHL(4));
			oc2 = insert(oc2, __STORE(4));
			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __SUB(4));

			oc2 = insert(oc2, __JZERO(4, 2));
			oc2 = insert(oc2, __JUMP(3));
			oc2 = insert(oc2, __SHL(1));
			oc2 = insert(oc2, __JUMP(-10));

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __LOAD(4));
			oc2 = insert(oc2, __SHR(4));
			oc2 = insert(oc2, __STORE(4));

			oc2 = insert(oc2, __INC(0)); //r0=2
			oc2 = insert(oc2, __ADD(1)); //r1=r1+p2
			oc2 = insert(oc2, __STORE(1)); //p2=r1

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __LOAD(4));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __SUB(4));
			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __STORE(4));

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __INC(0)); //r0=3
			oc2 = insert(oc2, __LOAD(1));
			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __STORE(1));

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __LOAD(1));
			oc2 = insert(oc2, __INC(1));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __SUB(1));

			oc2 = insert(oc2, __JZERO(1, 7));

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __INC(0)); //r0=3
			oc2 = insert(oc2, __LOAD(1));

			int tree_size = oc2->cmd_tree_size;

			oc3 = insert(oc3, __ZERO(0));
			oc3 = insert(oc3, __INC(0));
			oc3 = insert(oc3, __INC(4));
			oc3 = insert(oc3, __SUB(4));

			oc3 = insert(oc3, __JZERO(4, tree_size + 2 + 1 + 2));
			oc3 = insert(oc3, __ZERO(0));
			oc3 = insert(oc3, __LOAD(4));

			oc3 = insert(oc3, __JZERO(4, tree_size + 2));
			oc3 = merge(oc3, oc2);
			oc3 = insert(oc3, __JUMP(- tree_size - 1));

			int oc3_tree_size = oc3->cmd_tree_size;
			oc = insert(oc, __JZERO(4, 2));
			oc = insert(oc, __JUMP(4 + 3));
			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __INC(0));
			oc = insert(oc, __SHL(0));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __STORE(1));
			oc = insert(oc, __JUMP(oc3_tree_size + 1));
			oc = merge(oc, oc3);

			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __INC(0));
			oc = insert(oc, __SHL(0));
			oc = insert(oc, __LOAD(1));

		} else if (strcmp(e->op, "%") == 0) {

			struct OutputCode *oc2, *oc3;

			if (strcmp(v2->type, "id") == 0) {
				oc = id_addr_to_reg(v2->id, 0);
				oc = insert(oc, __LOAD(4));
			} else {
				oc = fill_reg_with_num(v2->num, 4);
			}

			oc3 = fill_reg_with_num(1, 0);
			oc3 = insert(oc3, __STORE(4));
			oc3 = insert(oc3, __INC(0));
			oc3 = insert(oc3, __INC(0));
			oc3 = insert(oc3, __STORE(4));

			// p2 = 0
			oc3 = insert(oc3, __ZERO(4));
			oc3 = insert(oc3, __DEC(0));
			oc3 = insert(oc3, __STORE(4));

			if (strcmp(v1->type, "id") == 0) {
				oc3 = merge(oc3, id_addr_to_reg(v1->id, 0));
				oc3 = insert(oc3, __LOAD(4));
			} else {
				oc3 = merge(oc3, fill_reg_with_num(v1->num, 4));
			}

			oc3 = insert(oc3, __ZERO(0));
			oc3 = insert(oc3, __STORE(4));

			oc2 = fill_reg_with_num(1, 1);
			oc2 = insert(oc2, __ZERO(0)); //A
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __LOAD(4));
			oc2 = insert(oc2, __SHL(4));
			oc2 = insert(oc2, __STORE(4));
			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __SUB(4));

			oc2 = insert(oc2, __JZERO(4, 2));
			oc2 = insert(oc2, __JUMP(3));
			oc2 = insert(oc2, __SHL(1));
			oc2 = insert(oc2, __JUMP(-10));

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __LOAD(4));
			oc2 = insert(oc2, __SHR(4));
			oc2 = insert(oc2, __STORE(4));

			oc2 = insert(oc2, __INC(0)); //r0=2
			oc2 = insert(oc2, __ADD(1)); //r1=r1+p2
			oc2 = insert(oc2, __STORE(1)); //p2=r1

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __LOAD(4));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __SUB(4));
			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __STORE(4));

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __INC(0)); //r0=3
			oc2 = insert(oc2, __LOAD(1));
			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __STORE(1));

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __LOAD(1));
			oc2 = insert(oc2, __INC(1));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __SUB(1));

			oc2 = insert(oc2, __JZERO(1, 7));

			oc2 = insert(oc2, __ZERO(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __INC(0));
			oc2 = insert(oc2, __INC(0)); //r0=3
			oc2 = insert(oc2, __LOAD(1));

			int tree_size = oc2->cmd_tree_size;

			oc3 = insert(oc3, __ZERO(0));
			oc3 = insert(oc3, __INC(0));
			oc3 = insert(oc3, __INC(4));
			oc3 = insert(oc3, __SUB(4));

			oc3 = insert(oc3, __JZERO(4, tree_size + 2 + 1 + 2));
			oc3 = insert(oc3, __ZERO(0));
			oc3 = insert(oc3, __LOAD(4));

			oc3 = insert(oc3, __JZERO(4, tree_size + 2));
			oc3 = merge(oc3, oc2);
			oc3 = insert(oc3, __JUMP(- tree_size - 1));

			int oc3_tree_size = oc3->cmd_tree_size;
			oc = insert(oc, __JZERO(4, 2));
			oc = insert(oc, __JUMP(4));
			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __STORE(0));
			oc = insert(oc, __JUMP(oc3_tree_size + 1));
			oc = merge(oc, oc3);

			oc = insert(oc, __ZERO(0));
			oc = insert(oc, __LOAD(1));

		}
	}

	return oc;
}

struct OutputCode *process_condition(struct Condition *c) {
	struct OutputCode *oc = new_output_code();
	struct Value *v1, *v2;
	v1 = c->val1;
	v2 = c->val2;

	struct Var *var;

	if (strcmp(v1->type, "id") == 0) {
		var = __get_var(v1->id->name);

		if (var == NULL) {
			__err_undecl_var(v1->id->ln, v1->id->name);
			exit(1);

		} else if (var->is_initialized == 0) {
			__err_uninit_var(v1->id->ln, var->name);
			return oc;

		}
	}

	if (strcmp(v2->type, "id") == 0) {
		var = __get_var(v2->id->name);

		if (var == NULL) {
			__err_undecl_var(v2->id->ln, v2->id->name);
			exit(1);

		} else if (var->is_initialized == 0) {
			__err_uninit_var(v2->id->ln, var->name);
			return oc;
		}
	}

	if (strcmp(c->rel, "=") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			10 = 16
			*/
			int val;

			if (v1->num == v2->num) {
				val = 1;
				__warn_always_true(v1->ln);
			} else {
				val = 0;
				__warn_always_false(v1->ln);
			}

			oc = fill_reg_with_num(val, 1);
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a = b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, __LOAD(1));
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));

			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __DEC(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 = b
			*/
			oc = fill_reg_with_num(v1->num, 1);
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));

			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __DEC(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));

		} else {
			/*
			ok
			a = 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __SUB(1));

			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __DEC(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));
		}
	} else if (strcmp(c->rel, "<>") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			int val;

			if (v1->num != v2->num) {
				val = 1;
				__warn_always_true(v1->ln);
			} else {
				val = 0;
				__warn_always_false(v1->ln);
			}

			oc = fill_reg_with_num(val, 1);
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a <> b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, __LOAD(1));
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));

			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __DEC(1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 <> a
			*/
			oc = fill_reg_with_num(v1->num, 1);
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));

			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __DEC(1));
			oc = insert(oc, __JZERO(1, 4));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __INC(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __ZERO(1));

		} else {
			/*
			ok
			a <> 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __SUB(1));

			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __DEC(1));
		}
	} else if (strcmp(c->rel, "<") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			10 < 11
			*/
			int val;
			if (v1->num < v2->num) {
				val = 1;
				__warn_always_true(v1->ln);
			} else {
				val = 0;
				__warn_always_false(v1->ln);
			}

			oc = fill_reg_with_num(val, 1);

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a < b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, __LOAD(1));
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 < a
			*/
			oc = fill_reg_with_num(v1->num, 1);
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));

		} else {
			/*
			ok
			a < 10
			*/
			oc = fill_reg_with_num(v2->num, 1);

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __SUB(1));

		}
	} else if (strcmp(c->rel, ">") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			10 > 9
			*/
			int val;

			if (v1->num > v2->num) {
				val = 1;
				__warn_always_true(v1->ln);
			} else {
				val = 0;
				__warn_always_false(v1->ln);
			}

			oc = fill_reg_with_num(val, 1);

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a > b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, __LOAD(1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 > a
			*/
			oc = fill_reg_with_num(v1->num, 1);

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));

		} else {
			/*
			ok
			a > 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __SUB(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));
		}

	} else if (strcmp(c->rel, "<=") == 0) {
		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			10 <= 10
			*/

			int val;

			if (v1->num <= v2->num) {
				val = 1;
				__warn_always_true(v1->ln);
			} else {
				val = 0;
				__warn_always_false(v1->ln);
			}

			oc = fill_reg_with_num(val, 1);
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a <= b
			*/
			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, __LOAD(1));

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 <= a
			*/
			oc = fill_reg_with_num(v1->num, 1);

			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));

		} else {
			/*
			ok
			a <= 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = insert(oc, __INC(1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __SUB(1));
		}
	} else if (strcmp(c->rel, ">=") == 0) {

		if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			10 >= 8
			*/
			int val;

			if (v1->num >= v2->num) {
				val = 1;
				__warn_always_true(v1->ln);
			} else {
				val = 0;
				__warn_always_false(v1->ln);
			}

			oc = fill_reg_with_num(val, 1);
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			a >= b
			*/
			oc = id_addr_to_reg(v2->id, 0);
			oc = insert(oc, __LOAD(1));

			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __SUB(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));

		} else if (strcmp(v1->type, "num") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			10 >= a
			*/
			oc = fill_reg_with_num(v1->num, 1);
			oc = insert(oc, __INC(1));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __SUB(1));

		} else {
			/*
			ok
			a >= 10
			*/
			oc = fill_reg_with_num(v2->num, 1);
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __SUB(1));
			oc = insert(oc, __JZERO(1, 3));
			oc = insert(oc, __ZERO(1));
			oc = insert(oc, __JUMP(2));
			oc = insert(oc, __INC(1));
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
			__err_undecl_var(c->id->ln, c->id->name);
			exit(1);
		} else if (var->is_mutable == 0) {
			__err_not_mut(c->id->ln, c->id->name);

		} else {

			__set_var_init(c->id->name);
			__set_var_undef(c->id->name);
			oc = merge(oc, id_addr_to_reg(c->id, 0));
			oc = insert(oc, __GET(1));
			oc = insert(oc, __STORE(1));
		}

	} else if (strcmp(c->type, "write") == 0) {
		/* OK */
		if (strcmp(c->val1->type, "id") == 0) {
			var = __get_var(c->val1->id->name);

			if (var == NULL) {
				__err_undecl_var(c->val1->id->ln, c->val1->id->name);
				exit(1);
			} else if (var->is_initialized == 0) {
				__err_uninit_var(c->val1->id->ln, var->name);
			}

			oc = id_addr_to_reg(c->val1->id, 0);
			oc = insert(oc, __LOAD(1));
			//TODO reg def
		} else {

			oc = fill_reg_with_num(c->val1->num, 1);
		}

		oc = insert(oc, __PUT(1));

	} else if (strcmp(c->type, "skip") == 0) {
		/* OK */

	} else if (strcmp(c->type, "assign") == 0) {
		/* OK */
		var = __get_var(c->id->name);
		if (var == NULL) {
			__err_undecl_var(c->id->ln, c->id->name);
			exit(1);

		} else if (var->is_mutable == 0) {
			__err_not_mut(c->id->ln, c->id->name);

		} else {

			__set_var_init(c->id->name);
			oc = process_expression(c->expr);
			oc = merge(oc, id_addr_to_reg(c->id, 0));
			oc = insert(oc, __STORE(1));
		}

	} else if (strcmp(c->type, "if") == 0) {
		/*
		ok
		*/
		struct Commands *cmd1, *cmd2;
		struct OutputCode *oc_cmd1, *oc_cmd2, *oc_cond;
		oc_cond = process_condition(c->cond);

		oc = merge(oc, oc_cond);

		cmd1 = c->cmd1;
		cmd2 = c->cmd2;

		oc_cmd1 = code_gen(cmd1);
		oc_cmd2 = code_gen(cmd2);

		int raw_ln = oc_cmd1->cmd_tree_size + 1;
		int ln = raw_ln + oc->add_cmd_tree_size + 1;

		oc = insert(oc, __JZERO(1, ln));

		if (oc->add_cmd_tree_size > 0) {
			oc = merge_add_tree(oc, oc);
			oc = insert(oc, __JUMP(raw_ln));
		}

		oc = merge(oc, oc_cmd1);

		oc = insert(oc, __JUMP(oc_cmd2->cmd_tree_size + 1));
		oc = merge(oc, oc_cmd2);

	} else if (strcmp(c->type, "while") == 0) {
		struct OutputCode *oc2, *oc_cond, *oc_cmd;
		char loop_id[16];

		sprintf(loop_id, "_while_%d", while_loop_id_cnt);
		while_loop_id_cnt++;

		struct Id *lid = __Id(loop_id, 0, NULL);

		__declare_var(loop_id, 0, 0, "num");
		__set_var_init(loop_id);

		oc_cond = process_condition(c->cond);
		oc_cmd = code_gen(c->cmd1);

		int cond_size = oc_cond->cmd_tree_size;

		oc = reg_overwrite(oc_cond, 1, 3);

		oc2 = id_addr_to_reg(lid, 0);
		oc2 = insert(oc2, __STORE(3));
		oc2 = merge(oc2, oc_cmd);
		oc2 = merge(oc2, id_addr_to_reg(lid, 0));
		oc2 = insert(oc2, __LOAD(3));
		oc2 = insert(oc2, __DEC(3));
		oc2 = insert(oc2, __STORE(3));

		int cmd_size = oc2->cmd_tree_size;

		int jumps = 1;

		oc = insert(oc, __JZERO(3, oc2->cmd_tree_size + 2));

		oc = merge(oc, oc2);
		oc = insert(oc, __JUMP(- 1 - cmd_size - cond_size));

		__unset_var(loop_id);

	} else if (strcmp(c->type, "for") == 0) {
		struct Var *var;
		struct Value *v1, *v2;
		struct OutputCode *oc2;
		char reg_cpy[16];

		v1 = c->val1;
		v2 = c->val2;

		if (strcmp(v1->type, "id") == 0) {
			var = __get_var(v1->id->name);

			if (var == NULL) {
				__err_undecl_var(v1->id->ln, v1->id->name);
			} else if (var->is_initialized == 0) {
				__err_uninit_var(v1->id->ln, var->name);
			}
		}

		if (strcmp(v2->type, "id") == 0) {
			var = __get_var(v2->id->name);

			if (var == NULL) {
				__err_undecl_var(v2->id->ln, v2->id->name);
			} else if (var->is_initialized == 0) {
				__err_uninit_var(v2->id->ln, var->name);
			}
		}

		__declare_var(c->pid, 0, 0, "num");
		__set_var_init(c->pid);

		sprintf(reg_cpy, "_for_%s", c->pid);

		__declare_var(reg_cpy, 0, 0, "num");
		__set_var_init(reg_cpy);

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
			oc = insert(oc, __STORE(1));
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			FROM a TO b
			*/
			oc = id_addr_to_reg(v2->id, 0);
			oc = insert(oc, __LOAD(2));
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __LOAD(1));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, __STORE(1));
			oc = insert(oc, __INC(2));
			oc = insert(oc, __SUB(2));

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			FROM a TO 17
			*/
			oc = fill_reg_with_num(v2->num, 2);
			oc = merge(oc, id_addr_to_reg(v1->id, 0));
			oc = insert(oc, __LOAD(1));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, __STORE(1));
			oc = insert(oc, __INC(2));
			oc = insert(oc, __SUB(2));

		} else {
			/*
			ok
			FROM 7 TO a
			*/
			oc = id_addr_to_reg(v2->id, 0);
			oc = insert(oc, __LOAD(2));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = merge(oc, fill_reg_with_num(v1->num, 1));
			oc = insert(oc, __STORE(1));
			oc = insert(oc, __INC(2));
			oc = insert(oc, __SUB(2));

		}

		struct Id *reg_cpy_id = __Id(reg_cpy, 0, NULL);

		oc2 = id_addr_to_reg(reg_cpy_id, 0);
		oc2 = insert(oc2, __STORE(2));
		oc2 = merge(oc2, code_gen(c->cmd1));
		oc2 = merge(oc2, id_addr_to_reg(reg_cpy_id, 0));
		oc2 = insert(oc2, __LOAD(2));
		oc2 = merge(oc2, id_addr_to_reg(id, 0));
		oc2 = insert(oc2, __LOAD(1));
		oc2 = insert(oc2, __INC(1));
		oc2 = insert(oc2, __STORE(1));

		oc = insert(oc, __JZERO(2, oc2->cmd_tree_size + 3));
		oc = merge(oc, oc2);
		oc = insert(oc, __DEC(2));
		oc = insert(oc, __JUMP(- (oc2->cmd_tree_size + 2)));

		__unset_var(reg_cpy);
		__unset_var(c->pid);

	} else if (strcmp(c->type, "for-downto") == 0) {
		struct Var *var;
		struct Value *v1, *v2;
		struct OutputCode *oc2;
		char reg_cpy[16];

		v1 = c->val1;
		v2 = c->val2;

		if (strcmp(v1->type, "id") == 0) {
			var = __get_var(v1->id->name);
			if (var == NULL) {
				__err_undecl_var(v1->id->ln, v1->id->name);
			} else if (var->is_initialized == 0) {
				__err_uninit_var(v1->id->ln, var->name);
			}
		}

		if (strcmp(v2->type, "id") == 0) {
			var = __get_var(v2->id->name);
			if (var == NULL) {
				__err_undecl_var(v2->id->ln, v1->id->name);
			} else if (var->is_initialized == 0) {
				__err_uninit_var(v2->id->ln, var->name);
			}
		}

		__declare_var(c->pid, 0, 0, "num");
		__set_var_init(c->pid);

		sprintf(reg_cpy, "_for_%s", c->pid);

		__declare_var(reg_cpy, 0, 0, "num");
		__set_var_init(reg_cpy);

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
			oc = insert(oc, __STORE(1));
		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "id") == 0) {
			/*
			ok
			FROM a DOWNTO 1
			*/
			__declare_var("_tmp0", 0, 0, "num");
			__set_var_init("_tmp0");

			struct Id *tmp_id = __Id("_tmp0", 0, NULL);

			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, __LOAD(2));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __LOAD(1));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, __STORE(2));
			oc = merge(oc, id_addr_to_reg(tmp_id, 0));
			oc = insert(oc, __STORE(1));
			oc = insert(oc, __INC(2));
			oc = insert(oc, __SUB(2));
			__unset_var("_tmp0");

		} else if (strcmp(v1->type, "id") == 0 && strcmp(v2->type, "num") == 0) {
			/*
			ok
			FROM a DOWNTO 1
			*/
			__declare_var("_tmp0", 0, 0, "num");
			__set_var_init("_tmp0");

			struct Id *tmp_id = __Id("_tmp0", 0, NULL);

			oc = id_addr_to_reg(v1->id, 0);
			oc = insert(oc, __LOAD(2));
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, __STORE(2));
			oc = merge(oc, fill_reg_with_num(v2->num, 1));
			oc = merge(oc, id_addr_to_reg(tmp_id, 0));
			oc = insert(oc, __STORE(1));
			oc = insert(oc, __INC(2));
			oc = insert(oc, __SUB(2));
			__unset_var("_tmp0");

		} else {
			/*
			ok
			FROM 10 DOWNTO a
			*/
			oc = fill_reg_with_num(v1->num, 2);
			oc = merge(oc, id_addr_to_reg(id, 0));
			oc = insert(oc, __STORE(2));
			oc = merge(oc, id_addr_to_reg(v2->id, 0));
			oc = insert(oc, __INC(2));
			oc = insert(oc, __SUB(2));

		}

		struct Id *reg_cpy_id = __Id(reg_cpy, 0, NULL);

		oc2 = id_addr_to_reg(reg_cpy_id, 0);
		oc2 = insert(oc2, __STORE(2));
		oc2 = merge(oc2, code_gen(c->cmd1));
		oc2 = merge(oc2, id_addr_to_reg(reg_cpy_id, 0));
		oc2 = insert(oc2, __LOAD(2));
		oc2 = merge(oc2, id_addr_to_reg(id, 0));
		oc2 = insert(oc2, __LOAD(1));
		oc2 = insert(oc2, __DEC(1));
		oc2 = insert(oc2, __STORE(1));

		oc = insert(oc, __JZERO(2, oc2->cmd_tree_size + 3));
		oc = merge(oc, oc2);
		oc = insert(oc, __DEC(2));
		oc = insert(oc, __JUMP(- (oc2->cmd_tree_size + 2)));

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

struct OutputCode *fill_reg_with_num(int num, int reg) {
	struct OutputCode *f = malloc(sizeof(struct OutputCode));

	f->cmd_tree_size = 0;
	f->add_cmd_tree_size = 0;
	f->type = "num";
	f->value = num;

	int mod;
	int ix = 0;
	int bin[1024];

	f = insert(f, __ZERO(reg));

	if (num > 0) {

		while (num > 0) {
			mod = num % 2;
			num = (num - mod) / 2;

			bin[ix] = mod;
			ix++;
		}

		for (int i = ix - 1; i >= 0; i--) {
			if (i != ix - 1) {
				f = insert(f, __SHL(reg));
			}

			if (bin[i] == 1) {
				f = insert(f, __INC(reg));
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
