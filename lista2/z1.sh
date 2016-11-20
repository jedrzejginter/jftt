#!/bin/bash

flex -o z1.c z1.lex && gcc z1.c -o z1.out -ll && ./z1.out < data/z1.txt > r1.txt && cat r1.txt
