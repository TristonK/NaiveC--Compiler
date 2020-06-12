#include "common.h"

#define OPTIMIZE

void IrAnalysis(ast* root){
    createHash();
    createEnv();
    temp_gen = 1;
    label_gen = 1;
    ir_root = malloc(sizeof(InterCodes));
    ir_tail = ir_root;
    irExtDefList(root->child); //extDefList
    optimize();
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
        //irOpVar(root->child->context);
        return sym;
    }else{
        newarray->kind = ARRAY;
        newarray->u.array.elem = type;
        newarray->u.array.size = c1s2(root)->val.intVal;
        irGetArray(root->child,newarray,inStrcut);
    }
}

int getArraySize(Symbol sym){
    int ret=1;
    Type tp = sym->t.type;
    while(tp!=NULL && tp->kind==ARRAY){
        ret *= tp->u.array.size;
        tp = tp->u.array.elem;
    }
    ret *= irTypeSize(tp);
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
        irCodeOp1(CODE_PARAM,irOpVar(sym->name));
        return sym->t.type;
    }else{
        Symbol sym = createSymbol(type,c1s(root)->child->context);
        hashInsert(sym);
        irCodeOp1(CODE_PARAM,irOpVar(c1s(root)->child->context));
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
        sym->op->kind = OP_ADDRESS;
        //sym->op = irOpAddr(sym->name);
    }else{
        sym = irCreateSymbol(type,root->child->child->child->context);
        if(type->kind==STRUCTURE){
            irCodeOp2(CODE_DEC,irOpVar(sym->name),irOpConstant(irTypeSize(type)));
            sym->op = irOpAddr(sym->name);
        }
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

int isConstant(ast* root){
    if(!strcmp(root->name,"INT")){
        return 1;
    }
    return 0;
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
            if(!strcmp(root->child->child->name,"ID") && c1s(root->child)==NULL){
                if(hashFind(root->child->child)->t.type->kind==ARRAY){
                    Operand t1 = irOpTemp();
                    Operand t2 = irOpTemp();
                    Type a = irExp(root->child,t1);
                    Type b = irExp(c1s2(root),t2);
                    int a_size = irTypeSize(a);
                    int b_size = irTypeSize(b);
                    Operand t3 = irOpTemp();
                    for(int i=0;i<a_size&&i<b_size;i+=4){
                        irCodeOp2(CODE_DEREF,t3,t2);
                        irCodeOp2(CODE_DEREF_ASSIGN,t1,t3);
                        irCodeOp3(CODE_ADD,t1,t1,irOpConstant(4));
                        irCodeOp3(CODE_ADD,t2,t2,irOpConstant(4));
                    }
                    return hashFind(root->child->child)->t.type;
                }
                if(isConstant(c1s2(root)->child)){
                    irCodeOp2(CODE_ASSIGN ,irOpVar(root->child->child->context),irOpConstant(c1s2(root)->child->val.intVal));
                    if(place!=NULL){
                        irCodeOp2(CODE_ASSIGN,place,irOpVar(root->child->child->context));
                    }
                }else{
                    Operand t1 = irOpTemp();
                    Type a = irExp(c1s2(root),t1);
                    irCodeOp2(CODE_ASSIGN ,irOpVar(root->child->child->context),t1);
                    if(place!=NULL){
                        irCodeOp2(CODE_ASSIGN,place,irOpVar(root->child->child->context));
                    }
                }
            } else if(!strcmp(c1s(root->child)->name,"LB")){
                Operand t1 = irOpTemp();
                Operand t2 = irOpTemp();
                Type arr = irExp(root->child->child,t1);
                Type b = irExp(c1s2(root->child),t2);
                Operand ad1 = irOpTemp();
                Operand ad2 = irOpTemp();
                int array_size = irTypeSize(arr->u.array.elem);
                irCodeOp3(CODE_MUL,ad2,t2,irOpConstant(array_size));
                irCodeOp3(CODE_ADD,ad1,ad2,t1);
                Operand t3 = irOpTemp();
                Type to_assign = irExp(c1s2(root),t3);
                if(to_assign->kind==BASIC){
                    irCodeOp2(CODE_DEREF_ASSIGN,ad1,t3);
                    if(place!=NULL){
                        irCodeOp2(CODE_ASSIGN,place,t3);
                    }
                }else{
                    if(to_assign->kind==ARRAY){
                        int a_size = irTypeSize(arr);
                        int b_size = irTypeSize(to_assign);
                        Operand tc = irOpTemp();
                        for(int i=0;i<a_size&&i<b_size;i+=4){
                            irCodeOp2(CODE_DEREF,tc,t3);
                            irCodeOp2(CODE_DEREF_ASSIGN,ad1,tc);
                            irCodeOp3(CODE_ADD,ad1,ad1,irOpConstant(4));
                            irCodeOp3(CODE_ADD,t3,t3,irOpConstant(4));
                        }
                    }else{
                        Operand t4 = irOpTemp();
                        irCodeOp2(CODE_DEREF,t4,t3);
                        irCodeOp2(CODE_DEREF_ASSIGN,ad1,t4);
                        if(place!=NULL){
                            irCodeOp2(CODE_ASSIGN,place,ad1);
                        }
                    }
                }
                return arr->u.array.elem;
            } else if(!strcmp(c1s(root->child)->name,"DOT")){
                Operand t1 = irOpTemp();
                Type str = irExp(root->child->child,t1);
                char* name = c1s2(root->child)->context;
                FieldList findName = str->u.structure;
                Type ret = NULL;
                int offset = 0;
                while(findName!=NULL){
                    if(!strcmp(findName->name,name)){
                        ret = findName->type;
                        break;
                    }
                    offset += irTypeSize(findName->type);
                    findName = findName->tail;
                }
                Operand ad =  irOpTemp();
                irCodeOp3(CODE_ADD,ad,t1,irOpConstant(offset));
                Operand t2 = irOpTemp();
                Type to_assign = irExp(c1s2(root),t2);
                if(to_assign->kind==BASIC){
                    irCodeOp2(CODE_DEREF_ASSIGN,ad,t2);
                    if(place!=NULL){
                        irCodeOp2(CODE_ASSIGN,place,t2);
                    }
                }else{
                    Operand t3 = irOpTemp();
                    irCodeOp2(CODE_DEREF,t3,t2);
                    irCodeOp2(CODE_DEREF_ASSIGN,ad,t3);
                    if(place!=NULL){
                        irCodeOp2(CODE_ASSIGN,place,ad);
                    }
                }
                return to_assign;
            }
        } else if(!strcmp(c1s(root)->name,"AND") || !strcmp(c1s(root)->name,"OR")||!strcmp(c1s(root)->name,"RELOP")){
            
                Operand t1 = irOpTemp();
                Operand t2 = irOpTemp();
                if(place!=NULL){
                    irCodeOp2(CODE_ASSIGN,place,irOpConstant(0));
                }
                irCond(root,t1,t2);
                irCodeOp1(CODE_LABEL,t1);
                if(place!=NULL){
                    irCodeOp2(CODE_ASSIGN,place,irOpConstant(1));
                }
                irCodeOp1(CODE_LABEL,t2);
            
        } else if(!strcmp(c1s(root)->name,"PLUS")){
            if(isConstant(root->child->child)&&isConstant(c1s2(root)->child)){
                if(place!=NULL){
                    irCodeOp2(CODE_ASSIGN,place,irOpConstant(root->child->child->val.intVal + c1s2(root)->child->val.intVal));
                }
            }else{
                Operand t1 = irOpTemp();
                Operand t2 = irOpTemp();
                irExp(root->child,t1);
                irExp(c1s2(root),t2);
                if(place!=NULL){
                    irCodeOp3(CODE_ADD,place,t1,t2);
                }
            }
        } else if( !strcmp(c1s(root)->name,"MINUS")){
            if(isConstant(root->child->child)&&isConstant(c1s2(root)->child)){
                if(place!=NULL){
                    irCodeOp2(CODE_ASSIGN,place,irOpConstant(root->child->child->val.intVal - c1s2(root)->child->val.intVal));
                }
            }else{
            Operand t1 = irOpTemp();
            Operand t2 = irOpTemp();
            irExp(root->child,t1);
            irExp(c1s2(root),t2);
            if(place!=NULL){
                irCodeOp3(CODE_SUB,place,t1,t2);
            }}
        }else if( !strcmp(c1s(root)->name,"STAR")){
            if(isConstant(root->child->child)&&isConstant(c1s2(root)->child)){
                if(place!=NULL){
                    irCodeOp2(CODE_ASSIGN,place,irOpConstant(root->child->child->val.intVal * c1s2(root)->child->val.intVal));
                }
            }else{
            Operand t1 = irOpTemp();
            Operand t2 = irOpTemp();
            irExp(root->child,t1);
            irExp(c1s2(root),t2);
            if(place!=NULL){
                irCodeOp3(CODE_MUL,place,t1,t2);
            }}
        } else if( !strcmp(c1s(root)->name,"DIV")){
            if(isConstant(root->child->child)&&isConstant(c1s2(root)->child)&&c1s2(root)->child->val.intVal!=0){
                if(place!=NULL){
                    irCodeOp2(CODE_ASSIGN,place,irOpConstant(root->child->child->val.intVal / c1s2(root)->child->val.intVal));
                }
            }else{
                Operand t1 = irOpTemp();
                Operand t2 = irOpTemp();
                irExp(root->child,t1);
                irExp(c1s2(root),t2);
                if(place!=NULL){
                    irCodeOp3(CODE_DIV,place,t1,t2);
                }
            }
        } else if(!strcmp(c1s(root)->name,"LB")){
            Operand t1 = irOpTemp();
            Operand t2 = irOpTemp();
            Type arr = irExp(root->child,t1);
            Type arr2 = irExp(c1s2(root),t2);
            if(place!=NULL){
                Operand ad1 = irOpTemp();
                Operand ad2 = irOpTemp();
                int array_size = irTypeSize(arr->u.array.elem);
                irCodeOp3(CODE_MUL,ad2,t2,irOpConstant(array_size));
                irCodeOp3(CODE_ADD,ad1,ad2,t1);
                if(arr->u.array.elem->kind!=BASIC || (root->sib!=NULL && !strcmp(root->sib->name,"DOT"))){
                    irCodeOp2(CODE_ASSIGN,place,ad1);
                }else{
                    irCodeOp2(CODE_DEREF,place,ad1);
                }   
            }
            return arr->u.array.elem;
        } else if(!strcmp(c1s(root)->name,"DOT")){
            Operand t1 = irOpTemp();
            Type str = irExp(root->child,t1);
            char* name = c1s2(root)->context;
            FieldList findName = str->u.structure;
            Type ret = NULL;
            int offset = 0;
            while(findName!=NULL){
                if(!strcmp(findName->name,name)){
                    ret = findName->type;
                    break;
                }
                offset += irTypeSize(findName->type);
                findName = findName->tail;
            }
            if(place!=NULL){
                Operand ad =  irOpTemp();
                irCodeOp3(CODE_ADD,ad,t1,irOpConstant(offset));
                if(findName->type->kind==BASIC){
                    irCodeOp2(CODE_DEREF,place,ad);
                }else{
                    irCodeOp2(CODE_ASSIGN,place,ad);    
                }
            }
            return ret;
        }
    } else if(!strcmp(root->child->name,"LP")){
        return irExp(c1s(root),place);
    } else if(!strcmp(root->child->name,"MINUS")){
            Operand t1 = irOpTemp();
            irExp(c1s(root),t1);
            if(place!=NULL){
                irCodeOp3(CODE_SUB,place,irOpConstant(0),t1);
            }
    }else if(!strcmp(root->child->name,"NOT")){
            Operand t1 = irOpTemp();
            Operand t2 = irOpTemp();
            if(place!=NULL)
                irCodeOp2(CODE_ASSIGN,place,irOpConstant(0));
            irCond(root,t1,t2);
            irCodeOp1(CODE_LABEL,t1);
            if(place!=NULL)
                irCodeOp2(CODE_ASSIGN,place,irOpConstant(1));
            irCodeOp1(CODE_LABEL,t2);
    } else if(!strcmp(root->child->name,"ID")){
        Symbol sym = hashFind(root->child);
        if(c1s(root)!=NULL){//function
            if(place==NULL){
                place = irOpTemp();
            }
            if(c1s3(root)==NULL){
                // no args
                if(!strcmp(root->child->context,"read")){
                    irCodeOp1(CODE_READ,place); 
                } else{
                    irCodeOp2(CODE_CALL,place,irOpFunc(root->child->context));
                    return sym->t.func->returnType;
                }
            }else{
                ArgList arg_list = irTransArgs(c1s2(root),NULL);
                if(!strcmp(root->child->context,"write")){
                    irCodeOp1(CODE_WRITE,arg_list->arg);
                    if(place!=NULL)
                        irCodeOp2(CODE_ASSIGN,place,irOpConstant(0));
                }else{
                    while(arg_list!=NULL){
                        irCodeOp1(CODE_ARG,arg_list->arg);
                        arg_list = arg_list->next;
                    }
                    if(place!=NULL){
                        irCodeOp2(CODE_CALL,place,irOpFunc(sym->name));
                    } else{
                        irCodeOp2(CODE_CALL,irOpTemp(),irOpFunc(sym->name));
                        
                    }
                    return sym->t.func->returnType;
                }
            }
        }else{
            if(place!=NULL)
                irCodeOp2(CODE_ASSIGN,place,irOpVar(sym->name));
            return sym->t.type;
        }
    } else if(!strcmp(root->child->name,"INT")){
        if(place!=NULL)
            irCodeOp2(CODE_ASSIGN,place,irOpConstant(root->child->val.intVal));
    } else if(!strcmp(root->child->name,"FLOAT")){
        Type ret = malloc(sizeof(struct Type_));
        ret->kind = BASIC;
        ret->u.basic = 2;
        return ret;
    } else {
        printf("%s\n",root->child->name);
        assert(0);
    }
    Type ret = malloc(sizeof(struct Type_));
    ret->kind = BASIC;
    ret->u.basic = 1;
    return ret;
}


int irTypeSize(Type a){
    if(a==NULL){printf("shit\n");return 1;}
    if(a->kind==BASIC){
        return 4;
    } else if(a->kind==ARRAY){
        return a->u.array.size * irTypeSize(a->u.array.elem);
    } else if(a->kind==STRUCTURE){
        int ret = 0;
        FieldList findName = a->u.structure;
        while(findName!=NULL){
            ret += irTypeSize(findName->type);
            findName = findName->tail;
        }
        return ret;
    }
    return 0;
}

void irCond(ast* root, Operand label_true, Operand label_false){
    //printf("%s\n",root->child->name);
    if(!strcmp(root->child->name,"Exp")){
        if(!strcmp(c1s(root)->name,"RELOP")){
            //printf("hi\n");
            Operand t1 = irOpTemp();
            Operand t2 = irOpTemp();
            irExp(root->child,t1);
            irExp(c1s2(root),t2);
            irCodeOp4(CODE_IF,t1,irOpRelop(c1s(root)->context),t2,label_true);
            irCodeOp1(CODE_GOTO,label_false);
            return;
        } else if(!strcmp(c1s(root)->name,"AND")){
            Operand label1 = irOpLabel();
            irCond(root->child,label1,label_false);
            irCodeOp1(CODE_LABEL,label1);
            irCond(c1s2(root),label_true,label_false);
            return;
        } else if(!strcmp(c1s(root)->name,"OR")){
            Operand label1 = irOpLabel();
            irCond(root->child,label_true,label1);
            irCodeOp1(CODE_LABEL,label1);
            irCond(c1s2(root),label_true,label_false);
            return;
        } 
    } else if(!strcmp(root->child->name,"NOT")){
        irCond(c1s(root),label_false,label_true);
        return;
    } 
    Operand t1 = irOpTemp();
    irExp(root,t1);
    irCodeOp4(CODE_IF,t1,irOpRelop("!="),irOpConstant(0), label_true);
    irCodeOp1(CODE_GOTO,label_false);
}


/**
 * root: Args
 * child: Exp COMMA Args
 *      / Exp
*/
ArgList irTransArgs(ast* root,ArgList arg_list){
    Operand t1 = irOpTemp();
    Type arg = irExp(root->child,t1);
    ArgList n_t1 = malloc(sizeof(struct ArgList_));
    n_t1->arg = t1;
    n_t1->next = arg_list;
    if(c1s(root)!=NULL){
        return irTransArgs(c1s2(root),n_t1);
    }
    return n_t1;
}

/**
 * root:StmtList
 * child: Stmt StmtList
 *      | NULL
*/
void irStmtList(ast* root, Symbol func){
    if(root->child !=NULL){
        irStmt(root->child,func);
        irStmtList(c1s(root),func);
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
void irStmt(ast* root,Symbol func){
    if(!strcmp(root->child->name,"Exp")){
        irExp(root->child,NULL);
    } else if(!strcmp(root->child->name,"CompSt")){
        irCompst(root->child,func);
    } else if(!strcmp(root->child->name,"RETURN")){
        Operand t1 = irOpTemp();
        irExp(c1s(root),t1);
        irCodeOp1(CODE_RETURN,t1);
    } else if(!strcmp(root->child->name,"IF")){
        if(c1s5(root)!=NULL){
            Operand label1 = irOpLabel();
            Operand label2 = irOpLabel();
            Operand label3 = irOpLabel();
            irCond(c1s2(root),label1,label2);
            irCodeOp1(CODE_LABEL,label1);
            irStmt(c1s4(root),func);
            irCodeOp1(CODE_GOTO,label3);
            irCodeOp1(CODE_LABEL,label2);
            irStmt(c1s5(root)->sib,func);
            irCodeOp1(CODE_LABEL,label3);
        }else{
            Operand label1 = irOpLabel();
            Operand label2 = irOpLabel();
            irCond(c1s2(root),label1,label2);
            irCodeOp1(CODE_LABEL,label1);
            irStmt(c1s4(root),func);
            irCodeOp1(CODE_LABEL,label2);
        }
    } else if(!strcmp(root->child->name,"WHILE")){
        Operand label1 = irOpLabel();
        Operand label2 = irOpLabel();
        Operand label3 = irOpLabel();
        irCodeOp1(CODE_LABEL,label1);
        irCond(c1s2(root),label2,label3);
        irCodeOp1(CODE_LABEL,label2);
        irStmt(c1s4(root),func);
        irCodeOp1(CODE_GOTO,label1);
        irCodeOp1(CODE_LABEL,label3);
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
    ret->op = NULL;
    return ret;
}

Symbol irCreateFuncSymbol(Func func, char* name){
    Symbol ret = malloc(sizeof(struct Symbol_));
    ret->name = name;
    ret->t.func = func;
    ret->isfunc = 1;
    ret->isdef = 0;
    ret->op = NULL;
    return ret;
}


Operand irOpFunc(char* name){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_FUNC;
    new_op->u.name = malloc(strlen(name)+3);
    strcpy(new_op->u.name, name);
    if(strcmp(name,"main")){
        strcat(new_op->u.name, "_f");
    }
    new_op->in_func = NULL;
    new_op->offset.func_stack_size = 0;
    new_op->in_reg = -1;
    return new_op;
}

Operand irOpVar(char* name){
    Symbol sym = hashFindName(name);
    if(sym!=NULL && sym->op!=NULL){
        return sym->op;
    }
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_VARIABLE;
    new_op->u.name = malloc(strlen(name)+3);
    strcpy(new_op->u.name, name);
    strcat(new_op->u.name, "_v");
    /*if(!strcmp(new_op->u.name,"x_v")){
        if(sym==NULL){printf("fu\n");}
        printf("shit\n");
    }*/
    new_op->in_func = NULL;
    new_op->offset.var_offset = 0;
    new_op->in_reg = -1;
    if(sym!=NULL){
        sym->op = new_op;
    }
    return new_op;
}

Operand irOpConstant(int num){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_CONSTANT;
    new_op->u.value = num;
    new_op->in_func = NULL;
    new_op->offset.var_offset = 0;
    new_op->in_reg = -1;
    return new_op;
}

Operand irOpTemp(){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_VARIABLE;
    char* temp = malloc(sizeof(char)*16);
    //printf("%d\n",temp_gen);
    sprintf(temp,"ts_%d",temp_gen);
    temp_gen ++;
    new_op->u.name = temp;
    new_op->in_func = NULL;
    new_op->offset.var_offset = 0;
    new_op->in_reg = -1;
    return new_op;
}

Operand irOpOp(char* op){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_MATH;
    new_op->u.name = malloc(4);
    if(!strcmp(op,"ADD")){
        strcpy(new_op->u.name,"+");
    } else if(!strcmp(op,"SUB")){
        strcpy(new_op->u.name,"-");
    } else if(!strcmp(op,"MUL")){
        strcpy(new_op->u.name,"*");
    } else  {
        strcpy(new_op->u.name,"/");
    }
    return new_op;
}

Operand irOpRelop(char* relop){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_RELOP;
    new_op->u.name = malloc(strlen(relop)+1);
    strcpy(new_op->u.name,relop);
    return new_op;
}

Operand irOpAddr(char* name){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_ADDRESS;
    new_op->u.name = malloc(strlen(name)+3);
    strcpy(new_op->u.name, name);
    strcat(new_op->u.name, "_v");
    new_op->in_func = NULL;
    new_op->offset.var_offset = 0;
    return new_op;
}

Operand irOpLabel(){
    Operand new_op = malloc(sizeof(struct Operand_));
    new_op->kind = OP_LABEL;
    char* temp = malloc(sizeof(char)*16);
    label_gen ++;
    sprintf(temp,"label%d",label_gen);
    new_op->u.name = temp;
    new_op->in_reg = -1;
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
    //printIRCodes(ir_out);
    //printf("create code1\n");
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
    //printIRCodes(ir_out);
    //printf("create code2\n");
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
    //printIRCodes(ir_out);
    //printf("create code3\n");
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
    //printIRCodes(ir_out);
    //printf("create code4\n");
}

