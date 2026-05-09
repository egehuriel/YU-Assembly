#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "unroll.h"
#define MAX_STMTS 1024
#define MAX_LABELS 128

typedef struct {
    char *name;
    int index;
} LabelEntry;

static ASTNode *copy_node(const ASTNode *src){
    if(!src) return NULL;
    ASTNode *dst = calloc(1, sizeof(ASTNode));
    if(!dst) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    *dst = *src;
    switch(src->type){
        case NODE_PROGRAM:
            dst->data.program.left = copy_node(src->data.program.left);
            dst->data.program.right = copy_node(src->data.program.right);
            break;
        case NODE_LABELED_STMT:
            dst->data.labeled_stmt.label = copy_node(src->data.labeled_stmt.label);
            dst->data.labeled_stmt.instr = copy_node(src->data.labeled_stmt.instr);
            break;
        case NODE_LABEL_DEF:
            dst->data.label_def.name = strdup(src->data.label_def.name);
            break;
        case NODE_LOAD:
        case NODE_STORE:
            dst->data.mem_instr.mem = copy_node(src->data.mem_instr.mem);
            break;
        case NODE_BLT:
        case NODE_BGT:
        case NODE_BEQ:
            dst->data.branch.label = strdup(src->data.branch.label);
            break;
        default:
            break;
    }
    return dst;
}

static int flatten(ASTNode *node, ASTNode **stmts, int idx){
    if (!node) return idx;
    if(node->type == NODE_PROGRAM){
        idx = flatten(node->data.program.left, stmts, idx);
        idx = flatten(node->data.program.right, stmts, idx);
    } else{
        if(idx < MAX_STMTS){
            stmts[idx++] = node;
        }
    }
    return idx;
}

static ASTNode *get_instr(ASTNode *stmt){
    if(!stmt) return NULL;
    if(stmt->type == NODE_LABELED_STMT)
        return stmt->data.labeled_stmt.instr;
    return stmt;
}

static const char *get_label_name(ASTNode *stmt){
    if(!stmt) return NULL;
    if(stmt->type == NODE_LABELED_STMT)
        return stmt->data.labeled_stmt.label->data.label_def.name;
    return NULL;
}

static int is_branch(ASTNode *node){
    if(!node) return 0;
    return node->type == NODE_BLT || node->type == NODE_BGT || node->type == NODE_BEQ;
}

static const char *branch_target(ASTNode *node){
    if(!is_branch(node)) return NULL;
    return node->data.branch.label;
}

static ASTNode *adjust_mem(ASTNode *mem, int delta){
    ASTNode *m = copy_node(mem);
    if(m->data.mem.sign == 0){
        m->data.mem.sign = 1;
        m->data.mem.offset = delta;
    } else if(m->data.mem.sign == 1){
        m->data.mem.offset += delta;
    }else{
        m->data.mem.offset -= delta;
        if(m->data.mem.offset < 0){
            m->data.mem.sign = 1;
            m->data.mem.offset = -m->data.mem.offset;
        } else if (m->data.mem.offset == 0){
            m->data.mem.sign = 0;
        }
    }
    return m;
}

static ASTNode *copy_stmt_with_offset(ASTNode *stmt, int delta){
    ASTNode *instr = get_instr(stmt);
    if(!instr) return copy_node(stmt);
    ASTNode *new_instr = NULL;
    switch(instr->type){
        case NODE_LOAD: {
            ASTNode *new_mem = adjust_mem(instr->data.mem_instr.mem, delta);
            new_instr = make_load_node(instr->data.mem_instr.reg, new_mem);
            break;
        }
        case NODE_STORE: {
            ASTNode *new_mem = adjust_mem(instr->data.mem_instr.mem, delta);
            new_instr = make_store_node(instr->data.mem_instr.reg, new_mem);
            break;
        }
        default:
            new_instr = copy_node(instr);
            break;
    }
    return new_instr;
}

static ASTNode *double_addi(ASTNode *stmt){
    ASTNode *instr = get_instr(stmt);
    if(!instr || instr->type != NODE_ADDI) return copy_node(stmt);
    return make_addi_node(instr->data.addi.rd, instr->data.addi.rs, instr->data.addi.imm * 2);
}

static ASTNode *build_ast(ASTNode **stmts, int count){
    if(count == 0) return NULL;
    ASTNode *root = stmts[0];
    for (int i =1; i < count; i++){
        root = make_program_node(root, stmts[i]);
    }
    return root;
}

ASTNode *unroll_loops(ASTNode *root){
    ASTNode *stmts[MAX_STMTS];
    int count = flatten(root, stmts, 0);
    LabelEntry labels[MAX_LABELS];
    int nlabels = 0;
    
    for(int i = 0; i < count; i++){
        const char *lname = get_label_name(stmts[i]);
        if(lname && nlabels < MAX_LABELS){
            labels[nlabels].name = strdup(lname);
            labels[nlabels].index = i;
            nlabels++;
        }
    }
    for(int i = 0; i < count; i++){
        ASTNode *instr = get_instr(stmts[i]);
        if(!is_branch(instr)) continue;
        const char *target = branch_target(instr);
        int loop_start = -1;
        for(int j = 0; j < nlabels; j++){
            if(strcmp(labels[j].name, target) == 0){
                loop_start = labels[j].index;
                break;
            }
        }
        if(loop_start < 0 || loop_start >= i) continue;
        ASTNode *result[MAX_STMTS];
        int rcount = 0;
        for(int k = 0; k < loop_start; k++){
            result[rcount++] = stmts[k];
        }
        result[rcount++] = stmts[loop_start];
        for(int k = loop_start + 1; k < i; k++){
            result[rcount++] = stmts[k];
        }
        for(int k = loop_start +1 ; k < i; k++){
            ASTNode *instr_k = get_instr(stmts[k]);
            if(instr_k && instr_k->type == NODE_ADDI){
                continue;
            }
            result[rcount++] = copy_stmt_with_offset(stmts[k],1);
        }
        for(int k = loop_start + 1; k < i; k++){
            ASTNode *instr_k = get_instr(stmts[k]);
            if(instr_k && instr_k->type == NODE_ADDI)
                result[rcount++] = double_addi(stmts[k]);
        }
        result[rcount++] = stmts[i];
        for(int k = i + 1; k < count; k++){
            result[rcount++] = stmts[k];
        }
        for(int k = 0; k < count; k++){
            stmts[k] = result[k];
        }
        count = rcount;
        for(int j = 0; j < nlabels; j++){
            free(labels[j].name);
        }
        return build_ast(stmts, count);
    }
    for(int j = 0; j < nlabels; j++){
        free(labels[j].name);
    }
    return root;
}

static void print_mem(ASTNode *mem){
    if(!mem) return;
    if (mem->data.mem.sign == 0)
        printf("mem[R%d]", mem->data.mem.reg);
    else if( mem->data.mem.sign == 1)
        printf("mem[R%d+%d]", mem->data.mem.reg, mem->data.mem.offset);
    else
        printf("mem[R%d-%d]", mem->data.mem.reg, mem->data.mem.offset);
}

static void print_instr(ASTNode *node){
    if(!node) return;
    switch (node->type){
        case NODE_MOV:
            printf("mov R%d, %d\n", node->data.mov.rd, node->data.mov.imm);
            break;
        case NODE_LOAD:
            printf("load R%d, ", node->data.mem_instr.reg);
            print_mem(node->data.mem_instr.mem);
            printf("\n");
            break;
        case NODE_STORE:
            printf("store R%d, ", node->data.mem_instr.reg);
            print_mem(node->data.mem_instr.mem);
            printf("\n");
            break;
        case NODE_ADDI:
            printf("addi R%d, R%d, %d\n", node->data.addi.rd, node->data.addi.rs, node->data.addi.imm);
            break;
        case NODE_ADD:
            printf("add R%d, R%d, R%d\n", node->data.alu.rd, node->data.alu.rs1, node->data.alu.rs2);
            break;
        case NODE_AND:
            printf("and R%d, R%d, R%d\n", node->data.alu.rd, node->data.alu.rs1, node->data.alu.rs2);
            break;
        case NODE_OR:
            printf("or R%d, R%d, R%d\n", node->data.alu.rd, node->data.alu.rs1, node->data.alu.rs2);
            break;
        case NODE_XOR:
            printf("xor R%d, R%d, R%d\n", node->data.alu.rd, node->data.alu.rs1, node->data.alu.rs2);
            break;
        case NODE_BLT:
            printf("blt R%d, R%d, %s\n", node->data.branch.rs1, node->data.branch.rs2, node->data.branch.label);
            break;
        case NODE_BGT:
            printf("bgt R%d, R%d, %s\n", node->data.branch.rs1, node->data.branch.rs2, node->data.branch.label);
            break;
        case NODE_BEQ:
            printf("beq R%d, R%d, %s\n", node->data.branch.rs1, node->data.branch.rs2, node->data.branch.label);
            break;
        default:
            break;
    }
}

void print_program(ASTNode *root){
    if(!root) return;
    if(root->type == NODE_PROGRAM){
        print_program(root->data.program.left);
        print_program(root->data.program.right);
        return;
    }
    if(root->type == NODE_LABELED_STMT){
        printf("%s:\n", root->data.labeled_stmt.label->data.label_def.name);
        print_instr(get_instr(root));
        return;
    }
    print_instr(root);
}