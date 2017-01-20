int ERRORS = 0;
int WARNINGS = 0;
int PR_LINE = 1;
char buf[128];

void register_new_line() {
	PR_LINE++;
}

void register_error() {
	ERRORS++;
	printf("\033[31;1m%s\033[0m occured\n", "Error");
}

void print_err(int line, char *message) {
	char *fmt;

	if (line < 0) {
		printf("\r\t\033[31;1mBłąd:\033[0m %s\n", message);
	} else {
		printf("%d\r\t\033[31;1mBłąd:\033[0m %s\n", line, message);
	}

	ERRORS++;
}

void print_warn(int line, char *message) {
	printf("%d\r\t\033[33;1mOstrzeżenie:\033[0m %s\n", line, message);
	WARNINGS++;
}

void __warn_div_0(int line) {
	sprintf(buf, "Dzielenie przez 0");
	print_warn(line, buf);
}

void __warn_always_true(int line) {
	sprintf(buf, "Warunek zawsze prawdziwy");
	print_warn(line, buf);
}

void __warn_always_false(int line) {
	sprintf(buf, "Warunek zawsze fałszywy");
	print_warn(line, buf);
}

void __err_bad_syntax() {
	sprintf(buf, "Niepoprawna składnia");
	print_err(-1, buf);
	exit(0);
}

void __err_bad_array_init(int line, char *var_name) {
	sprintf(buf, "Niepoprawna deklaracja tablicy `%s`", var_name);
	print_err(line, buf);
}

void __err_dup_decl(int line, char *var_name) {
	sprintf(buf, "Próba ponownej deklaracji zmiennej `%s`", var_name);
	print_err(line, buf);
}

void __err_wrong_use(int line, char *var_name) {
	sprintf(buf, "Niewłaściwe użycie zmiennej `%s`", var_name);
	print_err(line, buf);
}

void __err_undecl_var(int line, char *var_name) {
	sprintf(buf, "Użycie niezadeklarowanej zmiennej `%s`", var_name);
	print_err(line, buf);
}

void __err_uninit_var(int line, char *var_name) {
	sprintf(buf, "Użycie niezainicjalizowanej zmiennej `%s`", var_name);
	print_err(line, buf);
}

void __err_not_mut(int line, char *var_name) {
	sprintf(buf, "Iterator pętli for `%s` nie może być nadpisywany", var_name);
	print_err(line, buf);
}
