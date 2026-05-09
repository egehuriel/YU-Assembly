#ifndef UNROLL_H
#define UNROLL_H
#include "ast.h"

ASTNode *unroll_loops(ASTNode *root);
void print_program(ASTNode *root);
#endif
