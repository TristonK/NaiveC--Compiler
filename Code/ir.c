#include "common.h"

void IrAnalysis(ast* root){
    createHash();
    createEnv();
    ir_root = malloc(sizeof(InterCodes));
    ir_tail = ir_root;
    irExtDefList(root->child); //extDefList
}


/*
* root: extDefList
* child: ExtDef ExtdefList
*/
void irExtDefList(ast* root){
    if(root!=NULL && root->child!=NULL){
        irExtDef(root->child);
        irExtDefList(root->child->sib);
    }
}

/*
* root: extDef
* child: Spcifier ExtDecList SEMI
*      | Specifier SEMI
*      | Specifier FunDec Compst
*/
void irExtDef(ast* extDef){
    char* checkFlag = c1s(extDef)->name;
    Type specifier = irSpecifier(extDef->child);
    // as suppose 4, only need considering function and struct
    if(!strcmp(checkFlag,"FunDec")){
        if(specifier==NULL){return;}
        if(!strcmp(c1s2(extDef)->name,"CompSt")){
            Symbol sym = irFunDec(c1s(extDef),specifier);
            irCompst(c1s2(extDef),sym);
        }
    }
}


/*
root: Specifier 
child: TYPE | StructSpecifier
*/
Type irSpecifier(ast* root){
    if(!strcmp(root->child->name,"TYPE")){
        Type ret = malloc(sizeof(struct Type_));
        if(!strcmp(root->child->context,"int")){
            ret->kind = BASIC;
            ret->u.basic = 1;
        }else{
            ret->kind = BASIC;
            ret->u.basic = 2;
        }
        return ret;
    } else{
        return irStructure(root->child);
    }
}

/*
root: StructSpecifier
child: STRUCT OptTag LC DefList RC
      | STRUCT Tag
*/
Type irStructure(ast* root){
    Type ret;
    if(!strcmp(c1s(root)->name,"Tag")){
        Symbol id = FindStruct(c1s(root)->child); //Param:ID
        ret = id ->t.type;
    } else{
        ret = (Type)malloc(sizeof(struct Type_));
        ret->kind = STRUCTURE;
        ret->u.structure = irStructList(c1s3(root),NULL);
        if(c1s(root)->child!=NULL){
            Symbol sym = irCreateSymbol(ret,c1s(root)->child->context);
            sym->t.type->kind = STRUCTURE;
            sym->isdef = 1;
            hashInsert(sym);
        }
    }
    return ret;
}

/*
* root: DefList
* child: Def DefList 
       | NULL
*/
FieldList irStructList(ast* root,FieldList field){
    if(root->child!=NULL){
        if(field==NULL){
            field = malloc(sizeof(struct FieldList_));
        }
        FieldList next = irDefInStruct(root->child,field);//返回一系列中最后一个    
        if(next==NULL){
            return NULL;
        }
        FieldList newfield = malloc(sizeof(struct FieldList_));
        next->tail = irStructList(c1s(root),newfield);
        return field;
    }
    return NULL;
}

/*
* root: Def
* child: Specifier DecList SEMI
*/

FieldList irDefInStruct(ast* root,FieldList field){
    Type specifier = irSpecifier(root->child);
    if(specifier==NULL){return NULL;}
    if(field==NULL){
        field = malloc(sizeof(struct FieldList_));
    }
    field->type = specifier;
    return irDecListInStruct(c1s(root),field);
}


/*
* root: DecList
* child: Dec
       | Dec COMMA DecList
*/
FieldList irDecListInStruct(ast* root, FieldList field){
    Type type = field->type;
    if(c1s(root->child->child)!=NULL){//array
        Symbol sym = irGetArray(root->child->child,field->type,1);
        field->type = sym->t.type;
        field->name = sym->name;
    }else{
        field->name = root->child->child->child->context;
        Symbol sym = irCreateSymbol(field->type,field->name);
        //hashInsert(sym);
    }
    if(c1s(root)!=NULL){
        FieldList newfield = malloc(sizeof(struct FieldList_));
        newfield->type = type;
        field->tail = newfield;
        return irDecListInStruct(c1s2(root),newfield);
    }
    return field;
}


/*
* root: VarDec
* child: ID
*      | VarDec LB INT RB
*/
void irVarDec(ast* root, Type type){
    if(!strcmp(root->child->name,"ID")){
        Symbol sym = irCreateSymbol(type,root->child->context);
        hashInsert(sym);
    }else{
        irGetArray(root,type,0);
    }
}

/*
* root: VarDec
* child: ID
*      | VarDec LB INT RB
*/
Symbol irGetArray(ast* root, Type type,int inStrcut){
    Type newarray = malloc(sizeof(struct Type_));
    if(!strcmp(root->child->name,"ID")){
        newarray = type;
        Symbol sym = createSymbol(newarray,root->child->context);
        hashInsert(sym);
        return sym;
    }else{
        newarray->kind = ARRAY;
        newarray->u.array.elem = type;
        newarray->u.array.size = c1s2(root)->val.intVal;
        irGetArray(root->child,newarray,inStrcut);
    }
}

int getArraySize(Symbol sym){
    int ret=4;
    Type tp = sym->t.type;
    while(tp!=NULL && tp->kind==ARRAY){
        ret *= tp->u.array.size;
        tp = tp->u.array.elem;
    }
    return ret;
}

/*
* root: FunDec
* child: ID LP VarList RP
*      | ID LP RP
*/
Symbol irFunDec(ast* root,Type type){
    Func func = malloc(sizeof(struct Func_));
    func->returnType = type;
    func->lineno = root->child->lineno; 
    irCodeOp1(CODE_FUNC,irOpFunc(root->child->context));
    if(!strcmp(c1s2(root)->name,"VarList")){
        func->agru = irGetArg(c1s2(root),NULL);
    }else{
        func->agru = NULL;
    }
    Symbol sym = irCreateFuncSymbol(func,root->child->context);
    hashInsert(sym);
    return sym;
}

/**
 * root: VarList
 * child: ParamDec COMMA VarList
 *     / ParamDec
*/

Agru irGetArg(ast* root, Agru arg){
    Agru arg2 = malloc(sizeof(struct Agru_));
    if(arg==NULL){arg=arg2;}else{arg->next = arg2;}
    arg2->type = irParamDec(root->child);
    if(c1s(root)!=NULL){
        irGetArg(c1s2(root),arg2);
    }
    return arg;
}

/**
 * root: ParamDec
 * child: Specifier VarDec
*/
Type irParamDec(ast* root){
    Type type = getSpecifier(root->child);
    if(c1s(c1s(root))!=NULL){
        Symbol sym = irGetArray(c1s(root),type,0);
        irCodeOp1(CODE_PARAM,sym->name);
        return sym->t.type;
    }else{
        Symbol sym = createSymbol(type,c1s(root)->child->context);
        irCodeOp1(CODE_PARAM,irOpVar(c1s(root)->child->context));
        hashInsert(sym);
        return type;
    }
}


/*
    * root:Compst
    * child: LC DefList StmtList RC
    */
void irCompst(ast* root,Symbol func){
    irDefList(c1s(root));
    irStmtList(c1s2(root),func);
}


/*
* root: DefList
* child: Def DefList 
       | NULL
*/
void irDefList(ast* root){
    if(root->child!=NULL){
        irDef(root->child);    
        irDefList(c1s(root));
    }
}

/*
* root: Def
* child: Specifier DecList SEMI
*/

void irDef(ast* root){
    Type specifier = irSpecifier(root->child);
    if(specifier==NULL){
        return;
    }
    irDec(c1s(root),specifier);
}


/*
* root: DecList
* child: Dec
       | Dec COMMA DecList
*/

void irDec(ast* root, Type type){
    Symbol sym;
    if(c1s(root->child->child)!=NULL){//array
        sym = irGetArray(root->child->child,type,0);
        irCodeOp2(CODE_DEC,irOpVar(sym->name),irOpConstant(getArraySize(sym)));
    }else{
        sym = irCreateSymbol(type,root->child->child->child->context);
        hashInsert(sym);
    }
    if(c1s(root->child)!=NULL){    
        /**
        * root: Dec
        * child: VarDec
        *      | VarDec ASSIGNOP Exp
        */
        Operand place = irOpVar(sym->name);
        irExp(c1s2(root->child),place);
    }
    if(c1s(root)!=NULL){
        irDec(c1s2(root),type);
    }
}


/**
 * root: Exp
 * child: Exp ASSIGNOP EXP
 *      EXP AND EXP
 *      EXP OR EXP
 *      EXP RELOP EXP
 *      EXP PLUS EXP
 *      EXP MINUS EXP
 *      EXP STAR EXP
 *      EXP DIV EXP
 *      LP EXP RP
 *      MINUS EXP
 *      NOT EXP
 *      ID LP Args RP
 *      ID LP RP
 *      EXP LB EXP RB
 *      EXP DOT ID
 *      ID
 *      INT 
 *      FLOAT
*/
Type irExp(ast* root,Operand place){
    if(!strcmp(root->child->name,"Exp")){
        if(!strcmp(c1s(root)->name,"ASSIGNOP")){
            Type a = irExp(root->child);
            Type b = irExp(c1s2(root));
            if(a==NULL||b==NULL) {
                return NULL;
            }
            if(!sameType(a,b)){
                printSemaError(5,c1s(root)->lineno,"");
                return NULL;
            } else{
                if(!isVarible(root->child)){
                    printSemaError(6,c1s(root)->lineno,"");
                    return NULL;
                }
                return a;
            }
        } else if(!strcmp(c1s(root)->name,"AND") || !strcmp(c1s(root)->name,"OR")){
            Type a = checkExp(root->child);
            Type b = checkExp(c1s2(root));
            if(a==NULL||b==NULL) return NULL;
            if(sameType(a,b) && a->kind == BASIC && a->u.basic == 1){
                return a;
            }
            printSemaError(7,c1s(root)->lineno,"");
        } else if(!strcmp(c1s(root)->name,"RELOP")){
            Type a = checkExp(root->child);
            Type b = checkExp(c1s2(root));
            if(a==NULL||b==NULL) return NULL;
            if(!sameType(a,b)){
                printSemaError(7,c1s(root)->lineno,"");
                return NULL;
            }
            Type ret = malloc(sizeof(struct Type_));
            ret->kind = BASIC;
            ret->u.basic = 1;
            return ret;
        } else if(!strcmp(c1s(root)->name,"PLUS") ||!strcmp(c1s(root)->name,"MINUS")|| !strcmp(c1s(root)->name,"STAR") || !strcmp(c1s(root)->name,"DIV")){
            Type a = checkExp(root->child);
            Type b = checkExp(c1s2(root));
            if(a==NULL||b==NULL) return NULL;
            if(!sameType(a,b)){
                printSemaError(7,c1s(root)->lineno,"");
                return NULL;
            }
            return a;
        } else if(!strcmp(c1s(root)->name,"LB")){
            Type arr = checkExp(root->child);
            Type index = checkExp(c1s2(root));
            if(arr == NULL || index==NULL){
                #ifdef SEMABUG
                printf("arr is null or index is null\n");
                #endif
                return NULL;
            }
            if(arr->kind != ARRAY){
                printSemaError(10,root->child->lineno,"");   
                return NULL;
            }
            if(index==NULL){return NULL;}
            if(index->kind ==BASIC && index->u.basic == 1){
                return arr->u.array.elem;
            } else{
                printSemaError(12,c1s2(root)->lineno,"");
                return NULL;
            }
        } else if(!strcmp(c1s(root)->name,"DOT")){
            Type str = checkExp(root->child);
            if(str==NULL)return NULL;
            if(str->kind!=STRUCTURE){
                printSemaError(13,c1s(root)->lineno,"");
                return NULL;
            }
            char* name = c1s2(root)->context;
            FieldList findName = str->u.structure;
            while(findName!=NULL){
                if(!strcmp(findName->name,name)){
                    return findName->type;
                }
                findName = findName->tail;
            }
            printSemaError(14,c1s2(root)->lineno,name);
            return NULL;
        }
    } else if(!strcmp(root->child->name,"LP")){
        return checkExp(c1s(root));
    } else if(!strcmp(root->child->name,"MINUS")){
        return checkExp(c1s(root));
    } else if(!strcmp(root->child->name,"NOT")){
        Type type = checkExp(c1s(root));
        if(type==NULL) return NULL;
        if(type->kind == BASIC && type->u.basic==1){return type;}
        printSemaError(7,c1s(root)->lineno,"");
        return NULL;
    } else if(!strcmp(root->child->name,"ID")){
        Symbol sym = hashFind(root->child);
        if(c1s(root)!=NULL){//func
            if(sym==NULL){
                printSemaError(2,root->child->lineno,root->child->context);
                return NULL;
            }
            if(!sym->isfunc){
                printSemaError(11,root->child->lineno,root->child->context);
                return NULL;
            }
            if(c1s3(root)==NULL){
                if(sym->t.func->agru ==NULL){
                    return sym->t.func->returnType;
                }
                printSemaError(9,root->child->lineno,root->child->context);
                return NULL;
            }else{
                Agru useType = getArgs(c1s2(root),NULL); 
                Agru funType = sym->t.func->agru;
                while(useType!=NULL || funType!=NULL){
                    if(useType==NULL || funType==NULL){
                        printSemaError(9,root->child->lineno,root->child->context);
                        return NULL;
                    }
                    if(!sameType(useType->type,funType->type)){
                        printSemaError(9,root->child->lineno,root->child->context);
                        return NULL;
                    }
                    useType = useType->next;
                    funType = funType->next;
                }
                return sym->t.func->returnType;
            }
        }else{
            if(sym == NULL ||sym->isfunc || sym->isdef){
                printSemaError(1,root->child->lineno,root->child->context);
                return NULL;
            }
            return sym->t.type;
        }
    } else if(!strcmp(root->child->name,"INT")){
        Type ret = malloc(sizeof(struct Type_));
        ret->kind = BASIC;
        ret->u.basic = 1;
        return ret;
    } else if(!strcmp(root->child->name,"FLOAT")){
        Type ret = malloc(sizeof(struct Type_));
        ret->kind = BASIC;
        ret->u.basic = 2;
        return ret;
    } else {
        assert(0);
    }
}

/**
 * root: Exp in (EXP) ASSIGNOP EXP
*/
int isVarible(ast* root){
    if(!strcmp(root->child->name,"ID") && c1s(root)==NULL){return 1;}
    if(!strcmp(root->child->name,"Exp")){
        if(!strcmp(c1s(root)->name,"LB") || !strcmp(c1s(root)->name,"DOT")){
            return 1;
        }
    }
    return 0;
}

/**
 * root: Args
 * child: Exp COMMA Args
 *      / Exp
*/
Agru getArgs(ast* root,Agru arg){
    Agru nextArg = malloc(sizeof(struct Agru_));
    nextArg->type = checkExp(root->child);
    if(arg==NULL){
        arg = nextArg;
    }else{
        arg->next = nextArg;
    }
    if(c1s(root)!=NULL){
        getArgs(c1s2(root),nextArg);
    }
    return arg;
}

/**
 * root:StmtList
 * child: Stmt StmtList
 *      | NULL
*/
void checkStmtList(ast* root, Symbol func){
    if(root->child !=NULL){
        checkStmt(root->child,func);
        checkStmtList(c1s(root),func);
    }
}

/**
 * root: Stmt
 * child: Exp SEMI
 *     / Compst
 *     / RETURN EXP SEMI
 *    / IF LP Exp RP Stmt
 *     / IF LP Exp RP Stmt ELSE Stmt
 *    / WHILE LP EXP RP Stmt 
*/
void checkStmt(ast* root,Symbol func){
    if(!strcmp(root->child->name,"Exp")){
        checkExp(root->child);
    } else if(!strcmp(root->child->name,"CompSt")){
        getCompst(root->child,func,0);
    } else if(!strcmp(root->child->name,"RETURN")){
        Type type = checkExp(c1s(root));
        checkRet(func->t.func->returnType,type,root->child->lineno);
    } else if(!strcmp(root->child->name,"IF")){
        Type cond = checkExp(c1s2(root));
        if(cond!=NULL){
            if(cond->kind!=BASIC ||  (cond->kind ==BASIC && cond->u.basic!=1)){
                printSemaError(7,c1s2(root)->lineno,"");
            }
        }
        checkStmt(c1s4(root),func);
        if(c1s5(root)!=NULL){
            checkStmt(c1s5(root)->sib,func);
        }
    } else if(!strcmp(root->child->name,"WHILE")){
        Type cond = checkExp(c1s2(root));
        if(cond!=NULL){
            if(cond->kind!=BASIC || (cond->kind ==BASIC && cond->u.basic!=1)){
                printSemaError(7,c1s2(root)->lineno,"");
            }
        }
        checkStmt(c1s4(root),func);
        #ifdef SEMABUG
        printf("quit while\n");
        #endif
    } else {
        printf("%s\n",root->child->name);
        assert(0);
    }
}

void checkRet(Type a, Type b, int lineno){
    if(!sameType(a,b)){
        printSemaError(8,lineno,"");
    }
}


/*type*/
Symbol irCreateSymbol(Type type, char* name){
    Symbol ret = malloc(sizeof(struct Symbol_));
    ret->name = name;
    ret->t.type = type;
    ret->isfunc = 0;
    ret->depth = getEnvdepth();
    ret->isdef = 0;
    return ret;
}

Symbol irCreateFuncSymbol(Func func, char* name){
    Symbol ret = malloc(sizeof(struct Symbol_));
    ret->name = name;
    ret->t.func = func;
    ret->isfunc = 1;
    ret->isdef = 0;
    return ret;
}


Operand irOpFunc(char* name){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_FUNC;
    new_op->u.name = malloc(strlen(name)+3);
    strcpy(new_op->u.name, name);
    strcat(new_op->u.name, "_f");
    return new_op;
}

Operand irOpVar(char* name){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_VARIABLE;
    new_op->u.name = malloc(strlen(name)+3);
    strcpy(new_op->u.name, name);
    strcat(new_op->u.name, "_v");
    return new_op;
}

Operand irOpConstant(int num){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_CONSTANT;
    new_op->u.value = num;
    return new_op;
}

Operand irOpTemp(){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_VARIABLE;
    char tmp[16];
    char tp[16];
    char t[2] = "t"; 
    char zm[11]="0123456789";
    int tmp_num = temp_gen;
    int i=-1;
    while(tmp_num!=0){
        i++;
        tmp[i] = zm[tmp_num%10];
        tmp_num /= 10;
    }
    for(int j=0;i>=0;i--,j++){
        tp[j]=tmp[i-1];
    }
    char* temp = malloc(strlen(tp)+2);
    strcpy(temp,t);
    strcat(temp,tp);
    temp_gen ++;
    new_op->u.name = temp;
    return new_op;
}

// LABEL, FUNC, GOTO,READ,WRITE,PARAM,ARG,RETURN
void irCodeOp1(int kind,Operand op1){
    InterCodes newcode = malloc(sizeof(struct InterCodes_));
    newcode->prev = ir_tail;
    newcode->next = NULL;
    ir_tail->next = newcode; 
    ir_tail = newcode;
    newcode->code.kind = kind;
    newcode->code.u.single_op.result = op1;
}

// ASSIGN,REF,DEREF,DEREF_ASSIGN,CALL,DEC
void irCodeOp2(int kind,Operand op1,Operand op2){
    InterCodes newcode = malloc(sizeof(struct InterCodes_));
    newcode->prev = ir_tail;
    newcode->next = NULL;
    ir_tail->next = newcode; 
    ir_tail = newcode;
    newcode->code.kind = kind;
    newcode->code.u.assign.left = op1;
    newcode->code.u.assign.right = op2;
}


// ADD,SUB,MUL,DIV,
void irCodeOp3(int kind,Operand result,Operand op1,Operand op2){
    InterCodes newcode = malloc(sizeof(struct InterCodes_));
    newcode->prev = ir_tail;
    newcode->next = NULL;
    ir_tail->next = newcode; 
    ir_tail = newcode;
    newcode->code.kind = kind;
    newcode->code.u.binop.result = result;
    newcode->code.u.binop.op1 = op1;
    newcode->code.u.binop.op2 = op2;
}

// IF
void irCodeOp4(int kind,Operand left,Operand relop,Operand right,Operand label){
    InterCodes newcode = malloc(sizeof(struct InterCodes_));
    newcode->prev = ir_tail;
    newcode->next = NULL;
    ir_tail->next = newcode; 
    ir_tail = newcode;
    newcode->code.kind = kind;
    newcode->code.u.if_op.left = left;
    newcode->code.u.if_op.relop = relop;
    newcode->code.u.if_op.right = right;
    newcode->code.u.if_op.label = label;
}

