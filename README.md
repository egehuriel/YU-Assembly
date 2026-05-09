# YU Assembly Compiler

## Project Overview
A lexer, parser and loop unrolling optimizer for YU Assembly, a custom pseudo-assembly language designed for CSE 351 Term Project. This project implements a complete compiler front-end for the language, consisting of a lexical analyzer built with Flex, a syntax analyzer built with Bison, and a loop unrolling optimizer.
The lexer tokenizes raw assembly source code, recognizing instructions, registers (R0–R7), memory addressing forms, labels, and immediates while rejecting invalid input such as out-of-range registers or unknown characters. The parser enforces the grammar rules of YU Assembly, builds an Abstract Syntax Tree from the token stream, and catches syntax errors such as missing commas, wrong operand types, or labels used where registers are expected.
Once the AST is built, the loop unroller walks it to detect loops via backward branch detection — a loop is identified when a branch instruction targets a label defined earlier in the program. Detected loops are then unrolled by a factor of 2: the loop body is duplicated, memory offsets in the duplicate are incremented by 1, and increment instructions are doubled, reducing the total number of branch and counter-update operations at runtime.

## Project Structure
    YU-Assembly/ 
    ├── bin
    │   ├── yuasm
    │   └── yuasm.dSYM
    ├── bnf
    │   └── grammar.bnf
    ├── build
    │   ├── lex.yy.c
    │   ├── parser.tab.c
    │   └── parser.tab.h
    ├── LICENSE
    ├── Makefile
    ├── README.md
    ├── src
    │   ├── ast.c
    │   ├── ast.h
    │   ├── lexer.l
    │   ├── main.c
    │   ├── parser.y
    │   ├── unroll.c
    │   └── unroll.h
    └── tests
        ├── invalid1.asm
        ├── invalid2.asm
        ├── invalid3.asm
        ├── valid1.asm
        ├── valid2.asm
        └── valid3.asm

## Run Project
Build the Project:
    make all
    
Run the Project:
    make run FILE=valid1.asm
    make run FILE=valid2.asm
    make run FILE=valid3.asm
    make run FILE=invalid1.asm
    make run FILE=invalid2.asm
    make run FILE=invalid3.asm
    
CLEAN:
    make clean
    make clear
    
## Notes
The README will updated as implementation progresses.
