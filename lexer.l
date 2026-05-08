%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"
%}

%option noyywrap
%option yylineno
%option nounput
%option noinput

DIGIT   [0-9]
LETTER  [a-zA-Z]
NUMBER  {DIGIT}+
LABEL   {LETTER}({LETTER}|{DIGIT}|"_")*

%%
    
[ \t\r]+ { }
\n { return NEWLINE; }
    "#".* { }
    
    "mov"   { return MOV; }
    "load"  { return LOAD; }
    "store" { return STORE; }
    "addi"  { return ADDI; }
    "add"   { return ADD; }
    "and"   { return AND; }
    "or"    { return OR; }
    "xor"   { return XOR; }
    "blt"   { return BLT; }
    "bgt"   { return BGT; }
    "beq"   { return BEQ; }
    "mem"   { return MEM; }
    
    "R"[0-7]    {
        yylval.ival = yytext[1] - '0';
        return REGISTER;
    }
    "R"[0-9]+   {
        fprintf(stderr,"Lexer error at line %d invalid register '%s'. ", yylineno, yytext);
        exit(EXIT_FAILURE);
    }
    
    {NUMBER}    {
        yylval.ival = atoi(yytext);
        return NUMBER;
    }
    
    ","     { return COMMA; }
    ":"     { return COLON; }
    "["     { return LBRACKET; }
    "]"     { return RBRACKET; }
    "+"     { return PLUS; }
    "-"     { return MINUS; }
    
    {LABEL}     {
        yylval.sval = strdup(yytext);
        return LABEL;
    }
    
    .       {
        fprintf(stderr,
                "Lexer error at line %d: unknown character %s", yylineno, yytext);
    }
    
%%
