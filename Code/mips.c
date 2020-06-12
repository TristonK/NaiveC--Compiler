#include "common.h"

void MipsInit(){
    mips_read = malloc(sizeof(struct Mips_Operand_));
    mips_read->kind = MOP_FUNC;
    mips_read->u.name = malloc(sizeof(char)*8);
    strcpy(mips_read->u.name,"read");
    mips_write = malloc(sizeof(struct Mips_Operand_));
    mips_write->kind = MOP_FUNC;
    mips_write->u.name = malloc(sizeof(char)*8);
    strcpy(mips_write->u.name,"write");
    mips_sp = malloc(sizeof(struct Mips_Operand_));
    mips_sp->kind = MOP_REG;
    mips_sp->u.value = 29;
    mips_ra = malloc(sizeof(struct Mips_Operand_));
    mips_ra->kind = MOP_REG;
    mips_ra->u.value = 31;
    mips_v0 = malloc(sizeof(struct Mips_Operand_));
    mips_v0->kind = MOP_REG;
    mips_v0->u.value = 2;
}

void MipsGen(){
    mips_head = malloc(sizeof(struct MipsCode_));
    mips_tail = mips_head;
    InterCodes head = ir_root->next;
    while(head!=NULL){
        head = genMips(head);
        clearReg();
    }
}

InterCodes genMips(InterCodes codes){
    InterCode* code = &codes->code;
    char* buf = malloc(sizeof(char)*80);
    switch (code->kind)
    {
    case CODE_IF:
        // beq reg(x),reg(y),z
        MipsOperand op1 = getOffsetForFP(code->u.if_op.left);
        MipsOperand op2 = getOffsetForFP(code->u.if_op.right);
        MipsOperand op3 = createMOpLabel(code->u.if_op.label);
        createMipsRelop(op1,op2,op3,code->u.if_op.relop);
        break;
    case CODE_ASSIGN:
        //li reg(x) k
        //move reg(x),reg(y)
        MipsOperand op1 = getOffsetForFP(code->u.assign.left);
        MipsOperand op2 = getVarToReg(code->u.assign.right);
        MipsOperand tmp = createTmpReg();
        createMipsMove(tmp,op2);
        createMipsSw(tmp,op1);
        break;
    case CODE_LABEL:
        //x:
        MipsOperand op = createMOpLabel(code->u.single_op.result);
        createMipsLabel(op);
        break;
    case CODE_REF:
        //printf("it's not support suppose\n");
        MipsOperand op1 = getOffsetForFP(code->u.assign.left);
        MipsOperand op2 = getOffsetForFP(code->u.assign.right);
        MipsOperand tmp = createTmpReg();
        createMipsLA(tmp,op2);
        createMipsSw(tmp,op1);
        break;
    case CODE_DEREF:
        //lw reg(x),0(reg(y))
        MipsOperand op1 = getOffsetForFP(code->u.assign.left);
        MipsOperand op2_reg = getVarToReg(code->u.assign.right);
        MipsOperand op2 = createMOpOffset(op2_reg->u.value,0);
        MipsOperand tmp = createTmpReg();
        createMipsLw(tmp,op2);
        createMipsSw(tmp,op1); 
        break;
    case CODE_DEREF_ASSIGN:
        //sw reg(y) 0(reg(x))
        MipsOperand op1 = getVarToReg(code->u.assign.right);
        MipsOperand op2_reg = getVarToReg(code->u.assign.left);
        MipsOperand op2 = createMOpOffset(op2_reg->u.value,0);
        createMipsSw(op1,op2); 
        break;
    case CODE_ADD:
        // addi
        if(code->u.binop.op1->kind==OP_CONSTANT && code->u.binop.op2!=OP_CONSTANT){
            MipsOperand result = getOffsetForFP(code->u.binop.result);
            MipsOperand op1 = getVarToReg(code->u.binop.op2);
            MipsOperand op2 = createMOpImm(code->u.binop.op1->u.value);
            MipsOperand tmp_ans = createTmpReg();
            createMipsAddi(tmp_ans,op1,op2);
            createMipsSw(tmp_ans,result);
        } else if(code->u.binop.op2->kind==OP_CONSTANT && code->u.binop.op1!=OP_CONSTANT){
            MipsOperand result = getOffsetForFP(code->u.binop.result);
            MipsOperand op1 = getVarToReg(code->u.binop.op1);
            MipsOperand op2 = createMOpImm(code->u.binop.op2->u.value);
            MipsOperand tmp_ans = createTmpReg();
            createMipsAddi(tmp_ans,op1,op2);
            createMipsSw(tmp_ans,result);
        } else{
            MipsOperand result = getOffsetForFP(code->u.binop.result);
            MipsOperand op1 = getVarToReg(code->u.binop.op1);
            MipsOperand op2 = getVarToReg(code->u.binop.op2);
            MipsOperand tmp_ans = createTmpReg();
            createMipsAdd(tmp_ans,op1,op2);
            createMipsSw(tmp_ans,result);
        }
        break;
    case CODE_SUB:
        if(code->u.binop.op2->kind==OP_CONSTANT && code->u.binop.op1!=OP_CONSTANT){
            MipsOperand result = getOffsetForFP(code->u.binop.result);
            MipsOperand op1 = getVarToReg(code->u.binop.op1);
            MipsOperand op2 = createMOpImm(-code->u.binop.op2->u.value);
            MipsOperand tmp_ans = createTmpReg();
            createMipsAddi(tmp_ans,op1,op2);
            createMipsSw(tmp_ans,result);
        }else{
            MipsOperand result = getOffsetForFP(code->u.binop.result);
            MipsOperand op1 = getVarToReg(code->u.binop.op1);
            MipsOperand op2 = getVarToReg(code->u.binop.op2);
            MipsOperand tmp_ans = createTmpReg();
            createMipsSub(tmp_ans,op1,op2);
            createMipsSw(tmp_ans,result);
        }
        break;
    case CODE_MUL:
        MipsOperand result = getOffsetForFP(code->u.binop.result);
        MipsOperand op1 = getVarToReg(code->u.binop.op1);
        MipsOperand op2 = getVarToReg(code->u.binop.op2);
        MipsOperand tmp_ans = createTmpReg();
        createMipsMul(tmp_ans,op1,op2);
        createMipsSw(tmp_ans,result);
        break;
    case CODE_DIV:
        MipsOperand result = getOffsetForFP(code->u.binop.result);
        MipsOperand op1 = getVarToReg(code->u.binop.op1);
        MipsOperand op2 = getVarToReg(code->u.binop.op2);
        MipsOperand tmp_ans = createTmpReg();
        createMipsDiv(tmp_ans,op1,op2);
        createMipsSw(tmp_ans,result);
        break;
    case CODE_FUNC:
        cur_func = code->u.single_op.result;
        MipsOperand op = createMOpFunc(code->u.single_op.result);
        createMipsFunc(op);
        break;
    case CODE_PARAM:
        sprintf(buf,"PARAM %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_ARG:
        sprintf(buf,"ARG %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_RETURN:
        // move $v0 reg(x)
        // jr $ra
        //sprintf(buf,"RETURN %s\n",printOperand(code->u.single_op.result));
        break;
    case CODE_CALL:
        // jal f
        // move reg(x),$v0
        //sprintf(buf,"%s := CALL %s\n",printOperand(code->u.assign.left),printOperand(code->u.assign.right));
        break;
    case CODE_READ:
        MipsOperand minus4 = createMOpImm(-4);
        MipsOperand plus4 = createMOpImm(4);
        createMipsAddi(mips_sp,mips_sp,minus4);
        MipsOperand sp0 =  createMOpOffset(29,0);
        createMipsSw(mips_ra,sp0);
        createMipsJal(mips_read);
        createMipsLw(mips_ra,sp0);
        createMipsAddi(mips_sp,mips_sp,plus4);
        MipsOperand t1 = createTmpReg();
        createMipsMove(t1,mips_v0);
        MipsOperand to_read = getOffsetForFP(code->u.single_op.result);
        createMipsSw(t1,to_read);
        break;
    case CODE_WRITE:
        MipsOperand t2 = getVarToReg(code->u.single_op.result);
        createMipsMove(mips_a0,t2);
        MipsOperand minus4 = createMOpImm(-4);
        MipsOperand plus4 = createMOpImm(4);
        createMipsAddi(mips_sp,mips_sp,minus4);
        MipsOperand sp0 =  createMOpOffset(29,0);
        createMipsSw(mips_ra,sp0);
        createMipsJal(mips_read);
        createMipsLw(mips_ra,sp0);
        createMipsAddi(mips_sp,mips_sp,plus4);
        MipsOperand imm0 = createMOpImm(0);
        createMipsMove(mips_v0,imm0);
        break;
    case CODE_GOTO:
        MipsOperand op = createMOpLabel(code->u.single_op.result);
        createMipsJ(op);
        break;
    case CODE_DEC:
        insertFuncStack(code->u.assign.left,code->u.assign.right->u.value);
        break;
    default:
        return NULL;
        break;
    }
    return codes->next;
}

void createMipsLabel(MipsOperand op){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_LABEL;
    newcode->u.single_op.op1 = op;
    addMipsCodes(newcode);
}

void createMipsFunc(MipsOperand op){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_FUNC;
    newcode->u.single_op.op1 = op;
    addMipsCodes(newcode);
}

void createMipsLi(MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_LI;
    newcode->u.assign.op1 = op1;
    newcode->u.assign.op2 = op2;
    addMipsCodes(newcode);
}

void createMipsMove(MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_MOVE;
    newcode->u.assign.op1 = op1;
    newcode->u.assign.op2 = op2;
    addMipsCodes(newcode);
}

void createMipsAddi(MipsOperand result,MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_ADDI;
    newcode->u.binop.op1 = result;
    newcode->u.binop.op2 = op1;
    newcode->u.binop.op3 = op2;
    addMipsCodes(newcode);
}

void createMipsAdd(MipsOperand result,MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_ADD;
    newcode->u.binop.op1 = result;
    newcode->u.binop.op2 = op1;
    newcode->u.binop.op3 = op2;
    addMipsCodes(newcode);
}

void createMipsSub(MipsOperand result,MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_SUB;
    newcode->u.binop.op1 = result;
    newcode->u.binop.op2 = op1;
    newcode->u.binop.op3 = op2;
    addMipsCodes(newcode);
}

void createMipsMul(MipsOperand result,MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_MUL;
    newcode->u.binop.op1 = result;
    newcode->u.binop.op2 = op1;
    newcode->u.binop.op3 = op2;
    addMipsCodes(newcode);
}

void createMipsDiv(MipsOperand result,MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_DIV;
    newcode->u.binop.op1 = result;
    newcode->u.binop.op2 = op1;
    newcode->u.binop.op3 = op2;
    addMipsCodes(newcode);
}

void createMipsMflo(MipsOperand op1){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_MFLO;
    newcode->u.single_op.op1 = op1;
    addMipsCodes(newcode);
}

void createMipsLw(MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_LW;
    newcode->u.assign.op1 = op1;
    newcode->u.assign.op2 = op2;
    addMipsCodes(newcode);
}

void createMipsSw(MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_SW;
    newcode->u.assign.op1 = op1;
    newcode->u.assign.op2 = op2;
    addMipsCodes(newcode);
}
void createMipsJ(MipsOperand op){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_J;
    newcode->u.single_op.op1 = op;
    addMipsCodes(newcode);
}

void createMipsJal(MipsOperand op){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_JAL;
    newcode->u.single_op.op1 = op;
    addMipsCodes(newcode);
}

void createMipsJr(MipsOperand op){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_JR;
    newcode->u.single_op.op1 = op;
    addMipsCodes(newcode);
}

void createMipsRelop(MipsOperand op1,MipsOperand op2,MipsOperand op3,Operand relop){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    if(!strcmp(relop->u.name,"==")){
        newcode->kind = MIPS_BEQ;
    }else if(!strcmp(relop->u.name,"!=")){
        newcode->kind = MIPS_BNE;
    }else if(!strcmp(relop->u.name,">")){
        newcode->kind = MIPS_BGT;
    }else if(!strcmp(relop->u.name,"<")){
        newcode->kind = MIPS_BLT;
    }else if(!strcmp(relop->u.name,">=")){
        newcode->kind = MIPS_BGE;
    }else if(!strcmp(relop->u.name,"<=")){
        newcode->kind = MIPS_BLE;
    }
    newcode->u.binop.op1 = op1;
    newcode->u.binop.op2 = op2;
    newcode->u.binop.op3 = op3;
    addMipsCodes(newcode);
    // beq,bne,bgt,blt,bge,ble
}

void createMipsFunc(MipsOperand op){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_FUNC;
    newcode->u.single_op.op1 = op;
    addMipsCodes(newcode);
}

void createMipsLA(MipsOperand op1,MipsOperand op2){
    MipsCode newcode = malloc(sizeof(struct MipsCode_));
    newcode->kind = MIPS_LA;
    newcode->u.assign.op1 = op1;
    newcode->u.assign.op2 = op2;
    addMipsCodes(newcode);
}

MipsOperand createMOpLabel(Operand op){
    MipsOperand mop = malloc(sizeof(struct Mips_Operand_));
    mop->kind = MOP_LABEL;
    mop->u.name = op->u.name;
    return mop;
}

MipsOperand createMOpFunc(Operand op){
    MipsOperand mop = malloc(sizeof(struct Mips_Operand_));
    mop->kind = MOP_FUNC;
    mop->u.name = op->u.name;
    return mop;
}

MipsOperand createMOpReg(int reg){
    MipsOperand ret = malloc(sizeof(struct Mips_Operand_));
    ret->kind = MOP_REG;
    ret->u.value = reg;
    return ret;
}

MipsOperand createMOpOffset(int reg,int offset){
    MipsOperand ret = malloc(sizeof(struct Mips_Operand_));
    ret->kind = MOP_OFFSET;
    ret->u.offset.offset = offset;
    ret->u.offset.reg = reg; //$fp
    return ret;
}

MipsOperand createMOpImm(int imm){
    MipsOperand ret = malloc(sizeof(struct Mips_Operand_));
    ret->kind = MOP_IMM;
    ret->u.value = imm;
    return ret;
}

void addMipsCodes(MipsCode newcode){
    mips_tail->next = newcode;
    mips_tail = newcode;
}

MipsOperand getOffsetForFP(Operand op){
    if (op->in_func==NULL){
        return insertFuncStack(op,4);
    }
    return createMOpOffset(30,-op->offset.var_offset);
}

MipsOperand insertFuncStack(Operand op,int size){
    int offset = 0;
    switch (op->kind)
    {
        case OP_VARIABLE:
        case OP_ADDRESS:
            op->in_func = cur_func;
            offset = op->in_func->offset.func_stack_size + size;
            op->in_func->offset.func_stack_size = offset;
            op->offset.var_offset = offset;
            break;
        default:
            printf("what a shit insertFuncStack op: %d\n",op->kind);
            assert(0);
            break;
    }
    return createMOpOffset(30,-offset);
}


MipsOperand getVarToReg(Operand op){
    MipsOperand ret = createTmpReg();
    switch (op->kind)
    {
    case OP_CONSTANT:
        MipsOperand imm = createMOpImm(op->u.value);
        createMipsLi(ret,imm);
        break;
    case OP_VARIABLE:
        MipsOperand pos = createMOpOffset(30,-op->offset.var_offset);
        createMipsLw(ret,pos);
        break;
    case OP_ADDRESS:
        MipsOperand pos = createMOpOffset(30,-op->offset.var_offset);
        createMipsLA(ret,pos);
        break;
    default:
        break;
    }
    return ret;

}

MipsOperand createTmpReg(){
    int reg = -1;
    for(int i=8;i<16;i++){if(!regMap[i]){reg=i;break;}}
    if(reg==-1){
        for(int i=24;i<26;i++){if(!regMap[i]){reg=i;break;}}
    }
    if(reg==-1){
        for(int i=16;i<24;i++){if(!regMap[i]){reg=i;break;}}
    }
    if(reg==-1){printf("shit reg no tmp\n");assert(0);}
    return createMOpReg(reg);
}

void clearReg(){
    for(int i=8;i<16;i++){regMap[i]=0;}
    for(int i=24;i<26;i++){regMap[i]=0;}
}