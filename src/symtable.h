
#ifndef SYMTABLE_H
#define SYMTABLE_H
#include "ast.h"

typedef struct{
    char *name;
    int line;
} Symbol;

typedef struct {
    Symbol *entries;
    int count;
    int capacity;
} SymTable;

SymTable *symtable_create(void);
void symtable_free(SymTable *table);
void symtable_add(SymTable *table, const char *name, int line);
int symtable_lookup(SymTable *table, const char *name);
void symtable_print(SymTable *table);
int check_labels(ASTNode *root);

#endif
