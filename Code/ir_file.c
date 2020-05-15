#include "common.h"


char* printOperand(Operand op){
    if(op==NULL){return NULL;}
    if(op->kind==OP_CONSTANT){
        char* ret = malloc(sizeof(char)*16);
        sprintf(ret,"#%d",op->u.value);
        return ret;
    }else if(op->kind==OP_ADDRESS){
        char* ret = malloc(strlen(op->u.name)+2);
        sprintf(ret,"&%s",op->u.name);
        return ret;
    }
    return op->u.name;
}

char* printIRCode(InterCode *code){
    if(code->kind==CODE_IF){
        char* buf = malloc(sizeof(char)*140);
        sprintf(buf,"IF %s %s %s GOTO %s\n",printOperand(code->u.if_op.left),printOperand(code->u.if_op.relop),printOperand(code->u.if_op.right),printOperand(code->u.if_op.label));
        return buf;
    }
    char* buf = malloc(sizeof(char)*80);
    switch (code->kind)
    {
    case CODE_ASSIGN:
        sprintf(buf,"%s := %s\n",printOperand(code->u.assign.left),printOperand(code->u.assign.right));
        break;
    case CODE_LABEL:
        sprintf(buf,"LABEL %s :\n",printOperand(code->u.single_op.result));
        break;
    case CODE_REF:
        sprintf(buf,"%s := &%s\n",printOperand(code->u.assign.left),printOperand(code->u.assign.right));
        break;
    case CODE_DEREF:
        sprintf(buf,"%s := *%s\n",printOperand(code->u.assign.left),printOperand(code->u.assign.right));
        break;
    case CODE_DEREF_ASSIGN:
        sprintf(buf,"*%s := %s\n",printOperand(code->u.assign.left),printOperand(code->u.assign.right));
        break;
    case CODE_ADD:
        sprintf(buf,"%s := %s + %s\n",printOperand(code->u.binop.result),printOperand(code->u.binop.op1),printOperand(code->u.binop.op2));
        break;
    case CODE_SUB:
        sprintf(buf,"%s := %s - %s\n",printOperand(code->u.binop.result),printOperand(code->u.binop.op1),printOperand(code->u.binop.op2));
        break;
    case CODE_MUL:
        sprintf(buf,"%s := %s * %s\n",printOperand(code->u.binop.result),printOperand(code->u.binop.op1),printOperand(code->u.binop.op2));
        break;
    case CODE_DIV:
        sprintf(buf,"%s := %s / %s\n",printOperand(code->u.binop.result),printOperand(code->u.binop.op1),printOperand(code->u.binop.op2));
        break;
    case CODE_FUNC:
        sprintf(buf,"FUNCTION %s :\n",printOperand(code->u.single_op.result));
        break;
    case CODE_PARAM:
        sprintf(buf,"PARAM %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_ARG:
        sprintf(buf,"ARG %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_RETURN:
        sprintf(buf,"RETURN %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_CALL:
        sprintf(buf,"%s := CALL %s\n",printOperand(code->u.assign.left),printOperand(code->u.assign.right));
        break;
    case CODE_READ:
        sprintf(buf,"READ %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_WRITE:
        sprintf(buf,"WRITE %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_GOTO:
        sprintf(buf,"GOTO %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_DEC:
        sprintf(buf,"DEC %s %d\n",printOperand(code->u.assign.left),code->u.assign.right->u.value);
        break;
    default:
        break;
    }
    return buf;
}

void printIRCodes(FILE* out){
    InterCodes head = ir_root->next;
    while(head!=NULL){
        char* ret = printIRCode(&(head->code));
        fprintf(ir_out,"%s",ret);
        head = head->next;
    }
}