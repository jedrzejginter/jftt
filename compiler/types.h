
struct Id {
	char *name;			// nazwa zmiennej np `dzielnik`
	char *index_id;	// name[indexId] np `tab[a]`
	int index_num;		// name[indexNum] np `tab[10]`
	char *type;			// typ: num, arr
	int ln;
};

struct Value {
	char *type;		// typ wartości: { num, id }
	struct Id *id;	// wskaźnik na wartość, jeśli type == id, else: NULL
	int num;			// wartość jako int, jeśli type == num
	int ln;			// numer linii w której występuje wartość
};

struct Condition {
	struct Value *val1;	// wartość z lewej strony
	struct Value *val2;	// wartość z prawej strony
	char *rel;				// relacja
};

struct Expression {
	struct Value *val1;	// wartość z lewej strony
	struct Value *val2;	// wartość z prawej strony; NULL, jeśli op = NULL (exp -> value)
	char *op;				// działanie, NULL jeśli exp -> value
};

struct Commands {
	int size;					// ilość komend
	struct Command *root;	// pierwsza komenda
	struct Command *last;	// ostatnia komenda
	struct Command *cmds[4096];	// lista komend zagnieżdżonych
	int cmds_size;				// liczba komend
};

struct Command {
	char *type;					// typ komendy: assign, if, while, for, for_downto, read, write, skip
	char *pid;					// nazwa zmiennej dla for'ów
	struct Id *id;				// id dla assign i read
	struct Expression *expr;// wyrażenie dla assign
	struct Condition *cond;	// warunek dla if i while
	struct Commands *cmd1;	// lista zagnieżdżonych komend dla if (gdy true), while, for'ów
	struct Commands *cmd2;	// lista zagnieżdżonych komend dla if (gdy false)
	struct Value *val1;		// wartość from dla for
	struct Value *val2;		// wartość to/downto dla for

	struct Command *next;	// wskaźnik na kolejną komendę (potrzebne dla listy komend)
	int ln;						// numer linii
};
