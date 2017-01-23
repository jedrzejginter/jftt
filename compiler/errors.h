int ERRORS = 0;
int WARNINGS = 0;
int PR_LINE = 1;
char buf[128];

void register_new_line() {
	PR_LINE++;
}

void print_compil_succ() {
	printf("\n\t\033[0;32mKompilacja zakończona pomyślnie\033[0m\n");
	printf("\tBłędów\r\t\t\t%d\n", ERRORS);
	printf("\tOstrzeżeń\r\t\t\t%d\n\n", WARNINGS);
}

void print_err(int line, char *message) {
	char *fmt;

	if (line < 0) {
		printf("\r\t\033[0;31mBłąd\r\t\t\t\033[0m%s\n", message);
	} else {
		printf("%d\r\t\033[0;31mBłąd\r\t\t\t\033[0m%s\n", line, message);
	}

	ERRORS++;
}

void print_warn(int line, char *message) {
	printf("%d\r\t\033[0;33mOstrzeżenie\r\t\t\t\033[0m%s\n", line, message);
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
	sprintf(buf, "Nierozpoznany napis");
	print_err(PR_LINE, buf);
	exit(1);
}

void __err_unknown_str(int line, char *str) {
	sprintf(buf, "Niepoprawny napis: `%s`", str);
	print_err(line, buf);
}

void __err_bad_array_init(int line, char *var_name) {
	sprintf(buf, "Niepoprawna deklaracja tablicy `%s`", var_name);
	print_err(line, buf);
}

void __err_bad_array_ix(int line, int index, char *var_name) {
	sprintf(buf, "Indeks `%d` poza zakresem tablicy `%s`", index, var_name);
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
