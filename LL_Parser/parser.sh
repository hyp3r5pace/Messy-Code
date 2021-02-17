#!/bin/bash

# compilation of code files and producing executables
# lex files compilation
flex input.l
gcc lex.yy.c -ll

# parser files compilation
g++ -std=c++11 parser.cpp -o parser

echo "Enter the inputs (press ctrl + D for executing the parser)"
# running the lex analyser
./a.out

# running the parser
./parser