#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e 

echo "--- BUILDING COMPILER ---"
# Generate parser and lexer C code
bison -d parser.y
flex lexer.l     

# Compile the compiler itself (using C++11 standard to be safe)
g++ -std=c++11 -Wno-deprecated parser.tab.c lex.yy.c -o mycompiler

echo "--- TRANSLATING PSEUDO++ ---"
# Run your compiler on your test file
./mycompiler test.txt

echo "--- COMPILING FINAL PROGRAM ---"
# Compile the generated C++ code into the final executable
g++ -std=c++11 output.cpp -o program

echo ""
echo "=== PROGRAM OUTPUT ==="
# Run the final executable
./program
echo "======================"