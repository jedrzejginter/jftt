#!/bin/bash

flex -o z3.c z3.lex && gcc z3.c -o z3.out -ll && ./z3.out < data/z3.txt > r3.txt && cat r3.txt
