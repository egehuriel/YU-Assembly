#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "unroll.h"
#include "symtable.h"

extern FILE *yyin;
extern int yyparse(void);

int main(int argc, char *argv[]){
    //check - 1
    if(argc != 2){
        fprintf(stderr, "USAGE: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //2
    FILE *input = fopen(argv[1], "r");
    if(!input){
        fprintf(stderr, "Error - cannot open file %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    //3
    yyin = input;
    //4
    printf("PARSING -- %s ... \n", argv[1]);
    if(yyparse() != 0){
        fprintf(stderr, "error: parsing failed \n");
        fclose(input);
        return EXIT_FAILURE;
    }
    //5
    printf("parsing successfull \n\n");
    ASTNode *root = get_program_root();
    if(!root){
        fprintf(stderr, "error - empty program\n");
        fclose(input);
        return EXIT_FAILURE;
    }
    //symbol table chec
    printf("-----Symbol Table Check-----\n");
    if(check_labels(root) != 0){
        fprintf(stderr, "Semantic Error: undefined label found\n");
        fclose(input);
        return EXIT_FAILURE;
    }
    printf("\nAll branch targets resolved!\n");
    //6
    printf("=== AST (before unrolling) ===\n");
    print_ast(root, 0);
    printf("\n");
    //7
    printf("=== Original Program ===\n");
    print_program(root);
    printf("\n");
    //8
    printf("Applying loop unrolling...\n\n");
    ASTNode *unrolled = unroll_loops(root);
    //9
    printf("=== Unrolled Program === \n");
    print_program(unrolled);
    printf("\n");
    //10
    //free_ast(unrolled);
    fclose(input);
    
    return EXIT_SUCCESS;
}
