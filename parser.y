%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylineno;
extern int yylex();
void yyerror(const char *msg);
%}

%union{
    int ival;
    char *sval;
    ASTNode *node;
}

%token MOV LOAD STORE ADDI ADD AND OR XOR BLT BGT BEQ MEM
%token COMMA COLON LBRACKET RBRACKET PLUS MINUS NEWLINE
%token <ival> REGISTER
%token <ival> NUMBER
%token <sval> LABEL

%type <node> program
%type <node> statement_list
%type <node> statement
%type <node> instruction
%type <node> mov_instr
%type <node> load_instr
%type <node> store_instr
%type <node> addi_instr
%type <node> add_instr
%type <node> and_instr
%type <node> or_instr
%type <node> xor_instr
%type <node> blt_instr
%type <node> bgt_instr
%type <node> beq_instr
%type <node> mem_addr
%type <ival> immediate

%%
    
program
    : statement_list
    { $$ = $1; set_program_root($1); }
    ;

statement_list
    : statement
        { $$ = make_program_node($1, NULL); }
    | statement_list statement
        { $$ = make_program_node($1, $2); }
    ;

statement
    : LABEL COLON instruction newlines
    {
        ASTNode *lbl = make_label_def_node($1);
        $$ = make_labeled_stmt_node(lbl, $3);
        free($1);
    }
    | instruction newlines
        { $$ = $1; }
    ;
    
newlines
    : NEWLINE
    | newlines NEWLINE
    ;
    
instruction
    : mov_instr   { $$ = $1; }
    | load_instr  { $$ = $1; }
    | store_instr { $$ = $1; }
    | addi_instr  { $$ = $1; }
    | add_instr   { $$ = $1; }
    | and_instr   { $$ = $1; }
    | or_instr    { $$ = $1; }
    | xor_instr   { $$ = $1; }
    | blt_instr   { $$ = $1; }
    | bgt_instr   { $$ = $1; }
    | beq_instr   { $$ = $1; }
    ;

mov_instr
    : MOV REGISTER COMMA immediate
        { $$ = make_mov_node($2, $4); }
    | MOV LABEL COMMA immediate
        {
            fprintf(stderr, "Syntax error at line %d: mov requires a register '%s'\n", yylineno, $2);
            free($2);
            YYERROR;
        }
    ;
    
load_instr
    : LOAD REGISTER COMMA mem_addr
        { $$ = make_load_node($2, $4); }
    ;

store_instr
    : STORE REGISTER COMMA mem_addr
        { $$ = make_store_node($2, $4);}
    ;
    
addi_instr
    : ADDI REGISTER COMMA REGISTER COMMA immediate
        { $$ = make_addi_node($2, $4, $6); }
    ;
    
add_instr
    : ADD REGISTER COMMA REGISTER COMMA REGISTER
        { $$ = make_add_node($2, $4, $6); }
    ;
        
and_instr
    : AND REGISTER COMMA REGISTER COMMA REGISTER
        {$$ = make_and_node($2,$4,$6);}
    ;

or_instr
    : OR REGISTER COMMA REGISTER COMMA REGISTER
        { $$ = make_or_node($2,$4,$6);  }
    ;
    
xor_instr
    : XOR REGISTER COMMA REGISTER COMMA REGISTER
        { $$ = make_xor_node($2, $4, $6);}
    ;

blt_instr
    : BLT REGISTER COMMA REGISTER COMMA LABEL
        { $$ = make_branch_node(NODE_BLT, $2, $4, $6); free($6);}
    ;

beq_instr
    : BEQ REGISTER COMMA REGISTER COMMA LABEL
            { $$ = make_branch_node(NODE_BEQ, $2, $4, $6); free($6); }
    ;

bgt_instr
    : BGT REGISTER COMMA REGISTER COMMA LABEL
        { $$ = make_branch_node(NODE_BGT, $2, $4, $6); free($6);}
    ;
    
mem_addr
    : MEM LBRACKET REGISTER RBRACKET
        { $$ = make_mem_node($3, 0, 0); }
    | MEM LBRACKET REGISTER PLUS immediate RBRACKET
        { $$ = make_mem_node($3, $5, 1); }   /* 1 = positive offset */
    | MEM LBRACKET REGISTER MINUS immediate RBRACKET
        { $$ = make_mem_node($3, $5, -1); }  /* -1 = negative offset */
    ;

immediate
    : NUMBER    { $$ = $1 ;}
    | MINUS NUMBER { $$ = -$2; }
    ;
%%

void yyerror(const char *msg ){
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, msg);
}
