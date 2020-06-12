#include "common.h"

char* printMipsOperand(MipsOperand op){
    if(op==NULL){printf("shit\n");}
    if(op->kind == MOP_LABEL ||op->kind==MOP_FUNC){return op->u.name;}
    char* buf = malloc(sizeof(char)*12);
    switch (op->kind)
    {
    case MOP_REG:{
        if (op->u.value == 2){ 
            sprintf(buf, "$v0");
        } else if(op->u.value>=4 && op->u.value<=7){
            sprintf(buf,"$a%d",op->u.value-4);
        }else if(op->u.value>=8&&op->u.value<=15){
            sprintf(buf,"$t%d",op->u.value-8);
        }else if(op->u.value==29){
            sprintf(buf,"$sp");
        }else if(op->u.value==30){
            sprintf(buf,"$fp");
        }else if(op->u.value==31){
            sprintf(buf,"$ra");
        } else{
            sprintf(buf,"$%d",op->u.value);
        }
        break;
    }
    case MOP_IMM:
        sprintf(buf,"%d",op->u.value);
        break;
    case MOP_OFFSET:
        sprintf(buf,"%d($%d)",op->u.offset.offset,op->u.offset.reg);
        break;
    default:
        printf("shit error for %d\n",op->kind);
        assert(0);
        return NULL;
    }
    return buf;
}

char* printMipsCode(MipsCode code){
    char* buf = malloc(sizeof(char)*64);
    //printf("%d\n",code->kind);
    switch (code->kind)
    {
    case MIPS_LABEL:
        sprintf(buf,"%s:\n",printMipsOperand(code->u.single_op.op1));
        break; 
    case MIPS_LI:
        sprintf(buf,"  li %s, %s\n",printMipsOperand(code->u.assign.op1),printMipsOperand(code->u.assign.op2));
        break; 
    case MIPS_MOVE:
        sprintf(buf,"  move %s, %s\n",printMipsOperand(code->u.assign.op1),printMipsOperand(code->u.assign.op2));
        break;
    case MIPS_ADDI:
        sprintf(buf,"  addi %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_ADD:
        sprintf(buf,"  add %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_SUB:
        sprintf(buf,"  sub %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_MUL:
        sprintf(buf,"  mul %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_DIV:
        sprintf(buf,"  div %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_MFLO:
        sprintf(buf,"  mflo %s:\n",printMipsOperand(code->u.single_op.op1));
        break;
    case MIPS_SW:
        //printf("%s\n",printMipsOperand(code->u.assign.op1));
        sprintf(buf,"  sw %s, %s\n",printMipsOperand(code->u.assign.op1),printMipsOperand(code->u.assign.op2));
        break; 
    case MIPS_LW:
        sprintf(buf,"  lw %s, %s\n",printMipsOperand(code->u.assign.op1),printMipsOperand(code->u.assign.op2));
        break;
    case MIPS_J:
        sprintf(buf,"  j %s\n",printMipsOperand(code->u.single_op.op1));
        break;
    case MIPS_JAL:
        sprintf(buf,"  jal %s\n",printMipsOperand(code->u.single_op.op1));
        break; 
    case MIPS_JR:
        sprintf(buf,"  jr %s\n",printMipsOperand(code->u.single_op.op1));
        break;
    case MIPS_BEQ:
        sprintf(buf,"  beq %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_BNE:
        sprintf(buf,"  bne %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_BGT:
        sprintf(buf,"  bgt %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_BLT:
        sprintf(buf,"  blt %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_BGE:
        sprintf(buf,"  bge %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break; 
    case MIPS_BLE:
        sprintf(buf,"  ble %s, %s, %s\n",printMipsOperand(code->u.binop.op1),printMipsOperand(code->u.binop.op2),printMipsOperand(code->u.binop.op3));
        break;
    case MIPS_ANNO:
        sprintf(buf,"# %s",code->u.annotation);
        break;
    case MIPS_FUNC:
        sprintf(buf,"\n%s:\n",printMipsOperand(code->u.single_op.op1));
        break;
    case MIPS_LA:
        sprintf(buf,"  la %s, %s\n",printMipsOperand(code->u.assign.op1),printMipsOperand(code->u.assign.op2));
        break;
    default:
        printf("shit error %d\n",code->kind);
        assert(0);
        break;
    }
    return buf;
}

void printMipsCodes(FILE* out){
    fprintf(out, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
    fprintf(out, "read:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n\n");
    fprintf(out,"write:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
    MipsCode head = mips_head->next;
    while(head!=NULL){
        char* ret = printMipsCode(head);
        //printf("%s",ret);
        fprintf(out,"%s",ret);
        head = head->next;
    }
}
