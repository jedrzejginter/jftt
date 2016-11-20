#!/bin/bash

flex -o z4.c z4.lex && gcc z4.c -o z4.out -ll && ./z4.out < data/z4.txt > r4.txt && cat r4.txt
