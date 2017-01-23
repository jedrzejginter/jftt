struct RMLine *__ZERO(int reg) {
	return cmd("ZERO", reg);
}

struct RMLine *__DEC(int reg) {
	return cmd("DEC", reg);
}

struct RMLine *__INC(int reg) {
	return cmd("INC", reg);
}

struct RMLine *__SHL(int reg) {
	return cmd("SHL", reg);
}

struct RMLine *__SHR(int reg) {
	return cmd("SHR", reg);
}

struct RMLine *__COPY(int reg) {
	return cmd("COPY", reg);
}

struct RMLine *__SUB(int reg) {
	return cmd("SUB", reg);
}

struct RMLine *__ADD(int reg) {
	return cmd("ADD", reg);
}

struct RMLine *__STORE(int reg) {
	return cmd("STORE", reg);
}

struct RMLine *__LOAD(int reg) {
	return cmd("LOAD", reg);
}

struct RMLine *__PUT(int reg) {
	return cmd("PUT", reg);
}

struct RMLine *__GET(int reg) {
	return cmd("GET", reg);
}

struct RMLine *__JZERO(int reg, int add_ln) {
	return jcmd("JZERO", reg, add_ln);
}

struct RMLine *__JODD(int reg, int add_ln) {
	return jcmd("JODD", reg, add_ln);
}

struct RMLine *__JUMP(int reg) {
	return jscmd("JUMP", reg);
}

struct OutputCode *load_num_to_register(int num, int reg) {
	struct OutputCode *oc = malloc(sizeof(struct OutputCode));
	int mod;
	int ix = 0;
	int bin[1024];
	int org_num = num;

	oc->cmd_tree_size = 0;
	oc->add_cmd_tree_size = 0;
	oc->type = "num";
	oc->value = num;
	oc->swap_blocks = 0;

	oc = insert(oc, __ZERO(reg));

	if (num > 0) {

		while (num > 0) {
			mod = num % 2;
			num = (num - mod) / 2;

			bin[ix] = mod;
			ix++;
		}

		for (int i = ix - 1; i >= 0; i--) {
			if (i != ix - 1) {
				oc = insert(oc, __SHL(reg));
			}

			if (bin[i] == 1) {
				oc = insert(oc, __INC(reg));
			}
		}
	}

	return oc;
}
