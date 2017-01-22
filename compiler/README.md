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
