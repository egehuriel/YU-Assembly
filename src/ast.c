#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static ASTNode *program_root = NULL;

void set_program_root(ASTNode *root){
    program_root = root;
}
ASTNode *get_program_root(void){
    return program_root;
}

static ASTNode *alloc_node(NodeType type){
    ASTNode *node = calloc(1, sizeof(ASTNode));
    if(!node){
        fprintf(stderr, "out of memory\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    return node;
}

ASTNode *make_program_node(ASTNode *left, ASTNode *right){
    ASTNode *node = alloc_node(NODE_PROGRAM);
    node->data.program.left = left;
    node->data.program.right = right;
    return node;
}
ASTNode *make_labeled_stmt_node(ASTNode *label, ASTNode *instr){
    ASTNode *node = alloc_node(NODE_LABELED_STMT);
    node->data.labeled_stmt.label = label;
    node->data.labeled_stmt.instr = instr;
    return node;
}

ASTNode *make_label_def_node(const char *name){
    ASTNode *node = alloc_node(NODE_LABEL_DEF);
    node->data.label_def.name = strdup(name);
    if(!node->data.label_def.name){
        fprintf(stderr, "out of memory");
        exit(EXIT_FAILURE);
    }
    return node;
}

ASTNode *make_mov_node(int rd, int imm){
    ASTNode *node = alloc_node(NODE_MOV);
    node->data.mov.rd = rd;
    node->data.mov.imm = imm;
    return node;
}

ASTNode *make_load_node(int reg, ASTNode *mem){
    ASTNode *node = alloc_node(NODE_LOAD);
    node->data.mem_instr.reg = reg;
    node->data.mem_instr.mem = mem;
    return node;
}

ASTNode *make_store_node(int reg, ASTNode *mem){
    ASTNode *node = alloc_node(NODE_STORE);
    node->data.mem_instr.reg = reg;
    node->data.mem_instr.mem = mem;
    return node;
}

ASTNode *make_addi_node(int rd, int rs, int imm){
    ASTNode *node = alloc_node(NODE_ADDI);
    node->data.addi.rd = rd;
    node->data.addi.rs = rs;
    node->data.addi.imm = imm;
    return node;
}

static ASTNode *make_alu_node(NodeType type, int rd, int rs1, int rs2){
    ASTNode *node = alloc_node(type);
    node->data.alu.rd = rd;
    node->data.alu.rs1 = rs1;
    node->data.alu.rs2 = rs2;
    return node;
}

ASTNode *make_add_node(int rd, int rs1, int rs2) {
    return make_alu_node(NODE_ADD, rd, rs1, rs2);
}

ASTNode *make_and_node(int rd, int rs1, int rs2) {
    return make_alu_node(NODE_AND, rd, rs1, rs2);
}

ASTNode *make_or_node(int rd, int rs1, int rs2) {
    return make_alu_node(NODE_OR, rd, rs1, rs2);
}

ASTNode *make_xor_node(int rd, int rs1, int rs2) {
    return make_alu_node(NODE_XOR, rd, rs1, rs2);
}

ASTNode *make_branch_node(NodeType type, int rs1, int rs2, const char *label){
    ASTNode *node = alloc_node(type);
    node->data.branch.rs1 = rs1;
    node->data.branch.rs2 = rs2;
    node->data.branch.label = strdup(label);
    if(!node->data.branch.label){
        fprintf(stderr, "out of memory\n");
        exit(EXIT_FAILURE);
    }
    return node;
}

ASTNode *make_mem_node(int reg, int offset, int sign){
    ASTNode *node = alloc_node(NODE_MEM);
    node->data.mem.reg = reg;
    node->data.mem.offset = offset;
    node->data.mem.sign = sign;
    return node;
}

static void indent(int depth){
    for(int i = 0; i < depth; i++ ){
        printf(" ");
    }
}

void print_ast(ASTNode *node, int depth){
    if(!node) return;
    indent(depth);
    switch(node->type){
        case NODE_PROGRAM:
            printf("PROGRAM\n");
            print_ast(node->data.program.left, depth + 1);
            print_ast(node->data.program.right, depth + 1);
            break;
            
        case NODE_LABELED_STMT:
            printf("LABELED_STMT\n");
            print_ast(node->data.labeled_stmt.label, depth + 1);
            print_ast(node->data.labeled_stmt.instr, depth + 1);
            break;
            
        case NODE_LABEL_DEF:
            printf("LABEL_DEF %s \n", node->data.label_def.name);
            break;
        
        case NODE_MOV:
            printf("MOV R%d, %d\n", node->data.mov.rd, node->data.mov.imm);
            break;
            
        case NODE_LOAD:
            printf("LOAD R%d, ", node->data.mem_instr.reg);
            print_ast(node->data.mem_instr.mem, 0);
            break;
        
        case NODE_STORE:
            printf("STORE R%d ", node->data.mem_instr.reg);
            print_ast(node->data.mem_instr.mem, 0);
            break;
            
        case NODE_ADDI:
            printf("ADDI R%d, R%d, %d\n", node->data.addi.rd, node->data.addi.rs, node->data.addi.imm);
            break;
            
        case NODE_ADD:
            printf("ADD R%d, R%d, R%d\n ", node->data.alu.rd, node->data.alu.rs1, node->data.alu.rs2);
            break;
            
        case NODE_AND:
            printf("AND R%d, R%d, R%d\n ", node->data.alu.rd, node->data.alu.rs1, node->data.alu.rs2);
            break;
        
        case NODE_OR:
            printf("OR R%d, R%d, R%d\n", node->data.alu.rd, node->data.alu.rs1, node->data.alu.rs2);
            break;
            
        case NODE_XOR:
            printf("XOR R%d, R%d, R%d\n", node->data.alu.rd, node->data.alu.rs1, node->data.alu.rs2);
            break;
            
        case NODE_BLT:
        case NODE_BGT:
        case NODE_BEQ: {
            const char *name = node->type == NODE_BLT ? "BLT" : node->type == NODE_BGT ? "BGT" : "BEQ";
            printf("[%s] R%d, R%d, %s\n", name, node->data.branch.rs1, node->data.branch.rs2, node->data.branch.label);
            break;
        }
        case NODE_MEM:
            if(node->data.mem.sign == 0){
                printf("MEM --> mem[R%d]\n", node->data.mem.reg);
            }else if(node->data.mem.sign == 1){
                printf("MEM --> mem[R%d+%d]\n", node->data.mem.reg, node->data.mem.offset);
            }else{
                printf("MEM --> mem[R%d-%d]\n", node->data.mem.reg, node->data.mem.offset);
            }
            break;
        }
}

void free_ast(ASTNode *node){
    if(!node) return;
    
    switch(node->type){
        case NODE_PROGRAM:
            free_ast(node->data.program.left);
            free_ast(node->data.program.right);
            break;
        
        case NODE_LABELED_STMT:
            free_ast(node->data.labeled_stmt.label);
            free_ast(node->data.labeled_stmt.instr);
            break;
            
        case NODE_LABEL_DEF:
            free(node->data.label_def.name);
            break;
        
        case NODE_LOAD:
        case NODE_STORE:
            free_ast(node->data.mem_instr.mem);
            break;
        
        case NODE_BLT:
        case NODE_BGT:
        case NODE_BEQ:
            free(node->data.branch.label);
            break;
        
        default:
            break;
    }
    free(node);
}
