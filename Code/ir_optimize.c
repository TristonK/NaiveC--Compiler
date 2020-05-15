#include "common.h"

void deleteConstantCal();
void deleteConstant();
void changeCodeCons(InterCodes starting, char* victim,int num);
int changeOpCons(Operand op,char* victim,int num, int flag);
void changeIf();
int findLableUse(char* str, InterCodes starting);
int noUseCons(char* str, InterCodes starting);
void optimize(){
    for(int i=0;i<2;i++){
        //deleteConstantCal();
        //deleteConstant();
        //changeIf();
    }
}

void deleteConstantCal(){
    InterCodes head = ir_root->next;
    while(head->next!=NULL){
        InterCode code = head->code;
        if(code.kind==CODE_ADD){
            if(code.u.binop.op1->kind==OP_CONSTANT && code.u.binop.op2->kind==OP_CONSTANT){
                int n1 = code.u.binop.op1->u.value;
                int n2 = code.u.binop.op2->u.value;
                int n = n1+n2;
                Operand new_cons = code.u.binop.op1;
                new_cons->u.value = n; 
                Operand new_result = code.u.binop.result;
                head->code.kind = CODE_ASSIGN;
                head->code.u.assign.left = new_result;
                head->code.u.assign.right = new_cons;
            }else if(code.u.binop.op1->kind==OP_CONSTANT){
                int n1 = code.u.binop.op1->u.value;
                if(n1==0){
                    Operand new_cons = code.u.binop.op2;
                    Operand new_result = code.u.binop.result;
                    head->code.kind = CODE_ASSIGN;
                    head->code.u.assign.left = new_result;
                    head->code.u.assign.right = new_cons;
                }
            }
        }else if(code.kind==CODE_SUB){
            if(code.u.binop.op1->kind==OP_CONSTANT && code.u.binop.op2->kind==OP_CONSTANT){
                int n1 = code.u.binop.op1->u.value;
                int n2 = code.u.binop.op2->u.value;
                int n = n1-n2;
                Operand new_cons = code.u.binop.op1;
                new_cons->u.value = n; 
                Operand new_result = code.u.binop.result;
                head->code.kind = CODE_ASSIGN;
                head->code.u.assign.left = new_result;
                head->code.u.assign.right = new_cons;
            }
        }
        else if(code.kind==CODE_MUL){
            if(code.u.binop.op1->kind==OP_CONSTANT && code.u.binop.op2->kind==OP_CONSTANT){
                int n1 = code.u.binop.op1->u.value;
                int n2 = code.u.binop.op2->u.value;
                int n = n1*n2;
                Operand new_cons = code.u.binop.op1;
                new_cons->u.value = n; 
                Operand new_result = code.u.binop.result;
                head->code.kind = CODE_ASSIGN;
                head->code.u.assign.left = new_result;
                head->code.u.assign.right = new_cons;
            }else if(code.u.binop.op1->kind==OP_CONSTANT){
                int n1 = code.u.binop.op1->u.value;
                if(n1==0){
                    Operand new_cons = code.u.binop.op1;
                    new_cons->u.value = 0; 
                    Operand new_result = code.u.binop.result;
                    head->code.kind = CODE_ASSIGN;
                    head->code.u.assign.left = new_result;
                    head->code.u.assign.right = new_cons;
                }else if(n1==1){
                    Operand new_cons = code.u.binop.op2;
                    Operand new_result = code.u.binop.result;
                    head->code.kind = CODE_ASSIGN;
                    head->code.u.assign.left = new_result;
                    head->code.u.assign.right = new_cons;
                }
            }
        }else if(code.kind==CODE_DIV){
            if(code.u.binop.op1->kind==OP_CONSTANT && code.u.binop.op2->kind==OP_CONSTANT){
                int n1 = code.u.binop.op1->u.value;
                int n2 = code.u.binop.op2->u.value;
                if(n2!=0){
                    int n = n1/n2;
                    Operand new_cons = code.u.binop.op1;
                    new_cons->u.value = n; 
                    Operand new_result = code.u.binop.result;
                    head->code.kind = CODE_ASSIGN;
                    head->code.u.assign.left = new_result;
                    head->code.u.assign.right = new_cons;
                }
            }else if(code.u.binop.op1->kind==OP_CONSTANT){
                int n1 = code.u.binop.op1->u.value;
                if(n1==0){
                    Operand new_cons = code.u.binop.op1;
                    new_cons->u.value = 0; 
                    Operand new_result = code.u.binop.result;
                    head->code.kind = CODE_ASSIGN;
                    head->code.u.assign.left = new_result;
                    head->code.u.assign.right = new_cons;
                }
            }
        }
        head = head->next;
    }
}

void deleteConstant(){
    InterCodes head = ir_root->next;
    while (head!=NULL)
    {
        InterCode code = head->code;
        if(code.kind==CODE_ASSIGN){
            if(code.u.assign.right->kind == OP_CONSTANT && !noUseCons(code.u.assign.left->u.name,head->next)){
                InterCodes prev = head->prev;
                InterCodes next = head->next;
                changeCodeCons(head->next,code.u.assign.left->u.name,code.u.assign.right->u.value);
                prev->next = next;
                next->prev = prev;
            }
        }
        head = head->next;
    }
    
}

int noUseCons(char* str, InterCodes starting){
    if(starting==NULL){return 1;}
    starting = starting->next;
    while(starting!=NULL){
        if(starting->code.kind==CODE_ASSIGN||starting->code.kind==CODE_REF||starting->code.kind==CODE_DEREF||starting->code.kind==CODE_DEREF_ASSIGN||starting->code.kind==CODE_CALL||starting->code.kind==CODE_DEC){
            if(!strcmp(starting->code.u.assign.left->u.name,str)){return 1;}
        }else if(starting->code.kind==CODE_ADD||starting->code.kind==CODE_SUB||starting->code.kind==CODE_MUL||starting->code.kind==CODE_DIV){
            if(!strcmp(starting->code.u.binop.result->u.name,str)){return 1;}
        }else if(starting->code.kind==CODE_IF){
            if(!strcmp(starting->code.u.if_op.left->u.name,str)){return 1;}
            if(!strcmp(starting->code.u.if_op.right->u.name,str)){return 1;}
        }
        starting = starting->next;
    }
    return 0;
}

void changeCodeCons(InterCodes starting, char* victim,int num){
    if(starting == NULL){return;}
    int flags = 0;
    switch (starting->code.kind)
    {
    case CODE_WRITE:
    case CODE_PARAM:
    case CODE_ARG:
    case CODE_RETURN:
        changeOpCons(starting->code.u.single_op.result,victim,num,0);
        break;
    case CODE_ASSIGN:
    case CODE_REF:
    case CODE_DEREF:
    case CODE_DEREF_ASSIGN:
    case CODE_CALL:
    case CODE_DEC:
        flags = changeOpCons(starting->code.u.assign.left,victim,num,1);
        changeOpCons(starting->code.u.assign.right,victim,num,0);
        if (flags==-1){printf("shit1\n");return;}
        break;
    case CODE_ADD:
    case CODE_SUB:
    case CODE_MUL:
    case CODE_DIV:
        flags = changeOpCons(starting->code.u.binop.result,victim,num,1);
        changeOpCons(starting->code.u.binop.op1,victim,num,0);
        changeOpCons(starting->code.u.binop.op2,victim,num,0);
        if (flags==-1){printf("shit2\n");return;}
        break;
    case CODE_IF:
        changeOpCons(starting->code.u.if_op.left,victim,num,0);
        changeOpCons(starting->code.u.if_op.right,victim,num,0);
    default:
        break;
    }
    changeCodeCons(starting->next,victim,num);
}

int changeOpCons(Operand op,char* victim,int num, int flag){
    if(op==NULL){return -1;}
    if(flag==1){ //check re-use
        if(op->kind!=OP_CONSTANT && !strcmp(op->u.name,victim)){
            return -1;
        }
    }else{
        if(op->kind!=OP_CONSTANT && !strcmp(op->u.name,victim)){
            op->kind = OP_CONSTANT;
            op->u.value = num;
        }
    }
    return 0;
}

void changeIf(){
    InterCodes head = ir_root->next;
    while (head!=NULL)
    {   
        if(head->code.kind==CODE_IF && head->next!=NULL && head->next->code.kind == CODE_GOTO){
            if(head->next->next!=NULL &&head->next->next->code.kind ==CODE_LABEL && !strcmp(head->code.u.if_op.label->u.name,head->next->next->code.u.single_op.result->u.name)){
            if(!strcmp(head->code.u.if_op.relop->u.name,"<")){strcpy(head->code.u.if_op.relop->u.name,">=");}
            else if(!strcmp(head->code.u.if_op.relop->u.name,">")){strcpy(head->code.u.if_op.relop->u.name,"<=");}
            else if(!strcmp(head->code.u.if_op.relop->u.name,"<=")){strcpy(head->code.u.if_op.relop->u.name,">");}
            else if(!strcmp(head->code.u.if_op.relop->u.name,">=")){strcpy(head->code.u.if_op.relop->u.name,"<");}
            else if(!strcmp(head->code.u.if_op.relop->u.name,"==")){strcpy(head->code.u.if_op.relop->u.name,"!=");}
            else if(!strcmp(head->code.u.if_op.relop->u.name,"!=")){strcpy(head->code.u.if_op.relop->u.name,"==");}
            head->code.u.if_op.label = head->next->code.u.single_op.result;
            if(findLableUse(head->code.u.if_op.label->u.name, head->next->next)){
                head->next->next->prev = head;
                head->next = head->next->next;
            }else{
                head->next->next->next->prev = head;
                head->next= head->next->next->next;
            }}
        }
        head = head->next;
    }
    
}

int findLableUse(char* str, InterCodes starting){
    if(starting==NULL){return 1;}
    starting = starting->next;
    while(starting!=NULL){
        if(starting->code.kind==CODE_LABEL){
            if(!strcmp(starting->code.u.single_op.result->u.name,str)){return 1;}
        }else if(starting->code.kind==CODE_IF){
            if(!strcmp(starting->code.u.if_op.label->u.name,str)){return 1;}
        }else if(starting->code.kind==CODE_GOTO){
            if(!strcmp(starting->code.u.single_op.result->u.name,str)){return 1;}
        }
        starting = starting->next;
    }
    return 0;
}