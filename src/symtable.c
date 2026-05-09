//bonus - • Implement a symbol table to validate that all branch targets refer to declared labels
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtable.h"

#define INITIAL_CAPACITY 16

SymTable *symtable_create(void){
    SymTable *table = calloc(1, sizeof(SymTable));
    if(!table){
        fprintf(stderr, "out of memoy\n");
        exit(EXIT_FAILURE);
    }
    table->entries = calloc(INITIAL_CAPACITY, sizeof(Symbol));
    table->count = 0;
    table->capacity = INITIAL_CAPACITY;
    return table;
}

void symtable_free(SymTable *table){
    if(!table) return;
    for(int i = 0; i < table->count; i++)
        free(table->entries[i].name);
    free(table->entries);
    free(table);
}

void symtable_add(SymTable *table, const char *name, int line){
    if(table->count >= table->capacity){
        table->capacity *= 2;
        table->entries = realloc(table->entries, table->capacity * sizeof(Symbol));
        if(!table->entries){
            fprintf(stderr, "out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
    table->entries[table->count].name = strdup(name);
    table->entries[table->count].line = line;
    table->count++;
}

int symtable_lookup(SymTable *table, const char *name){
    for(int i = 0; i <table->count; i++){
        if(strcmp(table->entries[i].name, name) == 0)
            return 1;
    }
    return 0;
}

void symtable_print(SymTable *table){
    printf("symbol table (%d entries):\n", table->count);
    for(int i = 0; i < table->count; i++){
        printf("  [%d] %s\n", table->entries[i].line, table->entries[i].name);
    }
}

static void collect_labels(ASTNode *node, SymTable *table, int *idx){
    if(!node) return;
    if(node->type == NODE_PROGRAM){
        collect_labels(node->data.program.left, table, idx);
        collect_labels(node->data.program.right, table, idx);
        return;
    }
    if(node->type == NODE_LABELED_STMT){
        const char *name = node->data.labeled_stmt.label->data.label_def.name;
        symtable_add(table,name,*idx);
    }
    (*idx)++;
}

static int validate_branches(ASTNode *node, SymTable *table){
    if(!node) return 0;
    if(node->type == NODE_PROGRAM){
        int left = validate_branches(node->data.program.left, table);
        int right = validate_branches(node->data.program.right, table);
        return left + right;
    }
    ASTNode *instr = node;
    if(node->type == NODE_LABELED_STMT)
        instr = node->data.labeled_stmt.instr;
    if(!instr) return 0;
    if(instr->type == NODE_BLT || instr->type == NODE_BGT || instr->type == NODE_BEQ){
        const char *target = instr->data.branch.label;
        if(!symtable_lookup(table, target)){
            fprintf(stderr, "semantic error - branch target %s is undefined\n", target);
            return 1;
        }
    }
    return 0;
}

int check_labels(ASTNode *root){
    SymTable *table = symtable_create();
    int idx = 0;
    collect_labels(root, table, &idx);
    symtable_print(table);
    int errors = validate_branches(root,table);
    symtable_free(table);
    return (errors > 0) ? -1 : 0;
}
