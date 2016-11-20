#!/bin/bash

flex -o z2.c z2.lex && gcc z2.c -o z2.out -ll && ./z2.out < data/z2.txt > r2.txt && cat r2.txt
