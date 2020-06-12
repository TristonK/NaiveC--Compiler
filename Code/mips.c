#include "common.h"
//#define mips_debug

#ifdef mips_debug

void createMipsANNO(char* anno){
    MipsCode annotation = malloc(sizeof(struct MipsCode_));
    annotation->kind = MIPS_ANNO;
    annotation->u.annotation = anno;
    addMipsCodes(annotation);
}

#endif

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
    mips_fp = malloc(sizeof(struct Mips_Operand_));
    mips_fp->kind = MOP_REG;
    mips_fp->u.value = 30;
    mips_0 = malloc(sizeof(struct Mips_Operand_));
    mips_0->kind = MOP_REG;
    mips_0->u.value = 0;
    for(int i=3;i<=7;i++){
        mips_arg[i-3] = malloc(sizeof(struct Mips_Operand_));
        mips_arg[i-3]->kind = MOP_REG;
        mips_arg[i-3]->u.value = i;
    }
}

void MipsGen(){
    MipsInit();
    mips_head = malloc(sizeof(struct MipsCode_));
    mips_tail = mips_head;
    InterCodes head = ir_root->next;
    while(head!=NULL){
        #ifdef mips_debug
        createMipsANNO(printIRCode(&(head->code)));
        #endif
        //printf("%s",printIRCode(&(head->code)));
        head = genMips(head);
        clearReg();
        //printf("hi\n");
    }
}

InterCodes genMips(InterCodes codes){
    InterCode* code = &codes->code;
    char* buf = malloc(sizeof(char)*80);
    switch (code->kind)
    {
    case CODE_IF:{
        // beq reg(x),reg(y),z
        MipsOperand op1 = getVarToReg(code->u.if_op.left);
        MipsOperand op2 = getVarToReg(code->u.if_op.right);
        MipsOperand op3 = createMOpLabel(code->u.if_op.label);
        createMipsRelop(op1,op2,op3,code->u.if_op.relop);
        break;
    }
    case CODE_ASSIGN:{
        //li reg(x) k
        //move reg(x),reg(y)
        if(code->u.assign.right->kind==OP_ADDRESS){
            MipsOperand op1 = getOffsetForFP(code->u.assign.left);
            MipsOperand op2 = getOffsetForFP(code->u.assign.right);
            MipsOperand tmp = createTmpReg();
            createMipsLA(tmp,op2);
            createMipsSw(tmp,op1);
            break;
        }else{
            MipsOperand op1 = getOffsetForFP(code->u.assign.left);
            MipsOperand op2 = getVarToReg(code->u.assign.right);
            MipsOperand tmp = createTmpReg();
            createMipsMove(tmp,op2);
            createMipsSw(tmp,op1);
        }
        break;
    }
    case CODE_LABEL:{
        //x:
        MipsOperand op = createMOpLabel(code->u.single_op.result);
        createMipsLabel(op);
        break;
    }
    case CODE_REF:{
        //printf("it's not support suppose\n");
        MipsOperand op1 = getOffsetForFP(code->u.assign.left);
        MipsOperand op2 = getOffsetForFP(code->u.assign.right);
        MipsOperand tmp = createTmpReg();
        createMipsLA(tmp,op2);
        createMipsSw(tmp,op1);
        break;
    }
    case CODE_DEREF:{
        //lw reg(x),0(reg(y))
        MipsOperand op1 = getOffsetForFP(code->u.assign.left);
        MipsOperand op2_reg = getVarToReg(code->u.assign.right);
        MipsOperand op2 = createMOpOffset(op2_reg->u.value,0);
        MipsOperand tmp = createTmpReg();
        createMipsLw(tmp,op2);
        createMipsSw(tmp,op1); 
        break;
    }
    case CODE_DEREF_ASSIGN:{
        //sw reg(y) 0(reg(x))
        MipsOperand op1 = getVarToReg(code->u.assign.right);
        MipsOperand op2_reg = getVarToReg(code->u.assign.left);
        MipsOperand op2 = createMOpOffset(op2_reg->u.value,0);
        createMipsSw(op1,op2); 
        break;
    }
    case CODE_ADD:{
        // addi
        if(code->u.binop.op1->kind==OP_CONSTANT && code->u.binop.op2->kind!=OP_CONSTANT){
            MipsOperand result = getOffsetForFP(code->u.binop.result);
            MipsOperand op1 = getVarToReg(code->u.binop.op2);
            MipsOperand op2 = createMOpImm(code->u.binop.op1->u.value);
            MipsOperand tmp_ans = createTmpReg();
            createMipsAddi(tmp_ans,op1,op2);
            createMipsSw(tmp_ans,result);
        } else if(code->u.binop.op2->kind==OP_CONSTANT && code->u.binop.op1->kind!=OP_CONSTANT){
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
    }
    case CODE_SUB:{
        if(code->u.binop.op2->kind==OP_CONSTANT && code->u.binop.op1->kind!=OP_CONSTANT){
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
    }
    case CODE_MUL:{
        MipsOperand result = getOffsetForFP(code->u.binop.result);
        MipsOperand op1 = getVarToReg(code->u.binop.op1);
        MipsOperand op2 = getVarToReg(code->u.binop.op2);
        MipsOperand tmp_ans = createTmpReg();
        createMipsMul(tmp_ans,op1,op2);
        createMipsSw(tmp_ans,result);
        break;
    }
    case CODE_DIV:{
        MipsOperand result = getOffsetForFP(code->u.binop.result);
        MipsOperand op1 = getVarToReg(code->u.binop.op1);
        MipsOperand op2 = getVarToReg(code->u.binop.op2);
        MipsOperand tmp_ans = createTmpReg();
        createMipsDiv(tmp_ans,op1,op2);
        createMipsSw(tmp_ans,result);
        break;
    }
    case CODE_FUNC:{
        cur_func = code->u.single_op.result;
        MipsOperand op = createMOpFunc(code->u.single_op.result);
        MipsOperand minus4 = createMOpImm(-4);
        createMipsFunc(op);
        createMipsAddi(mips_sp,mips_sp,minus4);
        MipsOperand sp0 =  createMOpOffset(29,0);
        createMipsSw(mips_fp,sp0);
        createMipsMove(mips_fp,mips_sp);
        MipsOperand sizeee = createMOpImm(-10*1024);
        createMipsAddi(mips_sp,mips_sp,sizeee);
        break;
    }
    case CODE_PARAM:{
        int cnt = 0;
        int arg_cnt = 0;
        InterCodes getCnt = codes;
        while(getCnt !=NULL && getCnt->code.kind==CODE_PARAM){
            arg_cnt++;
            getCnt = getCnt->next;
        }
        int offset = -(arg_cnt/*-5*/)*4-4;
        while(codes!=NULL && codes->code.kind == CODE_PARAM){
            code = &(codes->code);
            code->u.single_op.result->in_func = cur_func;
            /*if(cnt<5){
                code->u.single_op.result->in_reg = cnt;
                mips_arg[cnt]->reg_op = code->u.single_op.result;
                cnt++;
            }else{*/
                code->u.single_op.result->offset.var_offset = offset;
                offset+=4;
            //}
            codes = codes->next;
        }
        return codes;
    }
    case CODE_ARG:{
        int offset = 0;
        int arg_cnt = 0;
        InterCodes getCnt = codes;
        while(getCnt !=NULL && getCnt->code.kind==CODE_ARG){
            arg_cnt++;
            getCnt = getCnt->next;
        }
        while(codes!=NULL && codes->code.kind == CODE_ARG){
            code = &(codes->code);
            MipsOperand arg = getVarToReg(code->u.single_op.result);
            /*if(arg_cnt<=5 && arg_cnt>=0){
                if(mips_arg[arg_cnt-1]->reg_op!=NULL){
                    mips_arg[arg_cnt-1]->reg_op->in_reg = -1;
                    MipsOperand old = getOffsetForFP(mips_arg[arg_cnt-1]->reg_op);
                    createMipsSw(mips_arg[arg_cnt-1],old);
                }
                createMipsMove(mips_arg[arg_cnt-1],arg);
                //mips_arg[arg_cnt-1] = code->u.single_op->result;
                arg_cnt--;
            }
            else{*/
                MipsOperand offsets = createMOpOffset(29,offset);
                createMipsSw(arg,offsets);
                offset+=4;
            //}
            codes = codes->next;
        }
        return codes;
    }
    case CODE_RETURN:
        // move $v0 reg(x)
        // jr $ra
    {
        MipsOperand ret = getVarToReg(code->u.single_op.result);
        createMipsMove(mips_v0,ret);
        createMipsMove(mips_sp,mips_fp);
        MipsOperand plus4 = createMOpImm(4);
        MipsOperand sp0 = createMOpOffset(29,0);
        createMipsLw(mips_fp,sp0);
        createMipsAddi(mips_sp,mips_sp,plus4);
        createMipsJr(mips_ra);
        /*for(int i=0;i<5;i++){
            if(mips_arg[i]->reg_op!=NULL){
                mips_arg[i]->reg_op->in_reg = -1;
                mips_arg[i]->reg_op = NULL;
            }
        }*/
        break;
    }
    case CODE_CALL:
        // jal f
        // move reg(x),$v0
        //sprintf(buf,"%s := CALL %s\n",printOperand(code->u.assign.left),printOperand(code->u.assign.right));
        {
            MipsOperand to_call = createMOpFunc(code->u.assign.right);
            MipsOperand minus4 = createMOpImm(-4);
            MipsOperand plus4 = createMOpImm(4);
            createMipsAddi(mips_sp,mips_sp,minus4);
            MipsOperand sp0 = createMOpOffset(29,0);
            createMipsSw(mips_ra,sp0);
            createMipsJal(to_call);
            MipsOperand sp00 = createMOpOffset(29,0);
            createMipsLw(mips_ra,sp00);
            createMipsAddi(mips_sp,mips_sp,plus4);
            MipsOperand retmp = createTmpReg();
            createMipsMove(retmp,mips_v0);
            MipsOperand ret = getOffsetForFP(code->u.assign.left);
            createMipsSw(retmp,ret);
            break;
        }
    case CODE_READ:{
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
    }
    case CODE_WRITE:{
        MipsOperand t2 = getVarToReg(code->u.single_op.result);
        createMipsMove(mips_arg[1],t2);
        MipsOperand minus4 = createMOpImm(-4);
        MipsOperand plus4 = createMOpImm(4);
        createMipsAddi(mips_sp,mips_sp,minus4);
        MipsOperand sp0 =  createMOpOffset(29,0);
        createMipsSw(mips_ra,sp0);
        createMipsJal(mips_write);
        createMipsLw(mips_ra,sp0);
        createMipsAddi(mips_sp,mips_sp,plus4);
        createMipsMove(mips_v0,mips_0);
        break;
    }
    case CODE_GOTO:{
        MipsOperand op = createMOpLabel(code->u.single_op.result);
        createMipsJ(op);
        break;
    }
    case CODE_DEC:{
        insertFuncStack(code->u.assign.left,code->u.assign.right->u.value);
        //printf("%s\n",code->u.assign.left->u.name);
        break;
    }
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
    /*if(op->in_reg!=-1){
        return mips_arg[op->in_reg];
    }*/
    return createMOpOffset(30,-op->offset.var_offset);
}

MipsOperand insertFuncStack(Operand op,int size){
    //printf("insert %s\n",op->u.name);
    int offset = 0;
    switch (op->kind)
    {
        case OP_VARIABLE:
            op->in_func = cur_func;
            offset = op->in_func->offset.func_stack_size + size;
            op->in_func->offset.func_stack_size = offset;
            op->offset.var_offset = offset;
            break;
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
    /*if(op->in_reg!=-1){
        return mips_arg[op->in_reg];
    }*/
    MipsOperand ret = createTmpReg();
    switch (op->kind)
    {
    case OP_CONSTANT:{
        MipsOperand imm = createMOpImm(op->u.value);
        createMipsLi(ret,imm);
        break;
    }
    case OP_VARIABLE:{
        //printf("find %s \n",op->u.name);
        MipsOperand pos = createMOpOffset(30,-op->offset.var_offset);
        createMipsLw(ret,pos);
        break;
    }
    case OP_ADDRESS:{
        //printf("find %s \n",op->u.name);
        MipsOperand pos = createMOpOffset(30,-op->offset.var_offset);
        createMipsLA(ret,pos);
        break;
    }
    default:
        assert(0);
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
    regMap[reg] =1;
    return createMOpReg(reg);
}

void clearReg(){
    for(int i=8;i<16;i++){regMap[i]=0;}
    for(int i=24;i<26;i++){regMap[i]=0;}
}