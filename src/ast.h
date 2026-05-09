#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROGRAM,
    NODE_LABELED_STMT,
    NODE_LABEL_DEF,
    NODE_MOV,
    NODE_LOAD,
    NODE_STORE,
    NODE_ADDI,
    NODE_ADD,
    NODE_AND,
    NODE_OR,
    NODE_XOR,
    NODE_BLT,
    NODE_BGT,
    NODE_BEQ,
    NODE_MEM,
} NodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;
    union{
        struct{
            ASTNode *left;
            ASTNode *right;
        } program;
        struct{
            ASTNode *label;
            ASTNode *instr;
        } labeled_stmt;
        struct {
            char *name;
        } label_def;
        struct {
            int rd;
            int imm;
        } mov;
        struct{
            int reg;
            ASTNode *mem;
        } mem_instr;
        struct{
            int rd;
            int rs;
            int imm;
        } addi;
        struct {
            int rd;
            int rs1;
            int rs2;
        } alu;
        struct {
            int rs1;
            int rs2;
            char *label;
        } branch;
        struct{
            int reg;
            int offset;
            int sign;
        } mem;
    } data;
};

void set_program_root(ASTNode *root);
ASTNode *get_program_root(void);

ASTNode *make_program_node  (ASTNode *left, ASTNode *right);
ASTNode *make_labeled_stmt_node (ASTNode *label, ASTNode *instr);
ASTNode *make_label_def_node (const char *name);
ASTNode *make_mov_node (int rd, int imm);
ASTNode *make_load_node (int reg, ASTNode *mem);
ASTNode *make_store_node (int reg, ASTNode *mem);
ASTNode *make_addi_node (int rd, int rs, int imm);
ASTNode *make_add_node (int rd, int rs1, int rs2);
ASTNode *make_and_node (int rd, int rs1, int rs2);
ASTNode *make_or_node (int rd, int rs1, int rs2);
ASTNode *make_xor_node (int rd, int rs1, int rs2);
ASTNode *make_branch_node (NodeType type, int rs1, int rs2, const char *label);
ASTNode *make_mem_node (int reg, int offset, int sign);

void print_ast (ASTNode *node, int depth);
void free_ast (ASTNode *node);

#endif
