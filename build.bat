@echo off
clang example.c cerde.c -o out\example.exe -Wno-macro-redefined -O0 -g3 -gfull -Wall -fsanitize=address
@echo on