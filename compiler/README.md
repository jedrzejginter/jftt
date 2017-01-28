## Kompilacja

Pierwsza kompilacja
```bash
make compile
```

"Posprzątanie" wygenerowanych plików
```bash
make clean
```

## Użycie
Programowi jako pierwszy parametr podajemy nazwę pliku, do którego zostanie zapisany output.
Następnie przekierowujemy treść pliku zawierającego kod języka imperatywnego do kompilatora.
```bash
./compiler out.mr < program.imp
```

## Opis plików
`code-generator.h` - funkcje, które tłumaczą ciąg tokenów na instrukcje maszyny rejestrowej
`code.h` - funkcje, która pozwalają "sklejać" tokeny ze sobą w celu utworzenia struktury przechowującej cały kod programu
`errors.h` - funkcje wypisujące informacje o błędach
`memory.h` - funkcje odpowiedzialne za rejestrowanie deklaracji zmiennych i kontrolujące ich późniejsze użycie
`rm-cmds.h` - funkcje analogiczne do rozkazów maszyny rejestrowej potrzebne do wypisywania odpowiednich instrukcji
`type-generators.h` - funkcje generujące tokeny dla rozpoznanej gramatyki
`types.h` - zdefiniowane typy tokenów

`compiler.lex` - analiza leksykalna dla kompilatora
`compiler.y` - definicja gramatyki dla kompilatora
