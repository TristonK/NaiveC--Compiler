#include "common.h"

void semaAnalysis(ast* root){
    createHash();
    createEnv();
    getExtDef(root->child); //extDefList
    checkFunc();
}


/*
* root: extDefList
* child: ExtDef ExtdefList
*/
void getExtDef(ast* root){
    if(root!=NULL && root->child!=NULL){
        checkExtDef(root->child);
        getExtDef(root->child->sib);
    }
}

/*
* root: extDef
* child: Spcifier ExtDecList SEMI
*      | Specifier SEMI
*      | Specifier FunDec Compst
*/
void checkExtDef(ast* extDef){
    char* checkFlag = c1s(extDef)->name;
    if(!strcmp(checkFlag,"ExtDecList")){
        Type specifier = getSpecifier(extDef->child);
        if(specifier->kind==STRCTDEF){specifier->kind=STRUCTURE;}
        #ifdef SEMABUG
        printf("enter extdeclist\n");
        #endif
        if(specifier==NULL){ 
            return;
        }
        getExtDecList(c1s(extDef),specifier);
    } else if(!strcmp(checkFlag,"SEMI")){
        if(!strcmp(extDef->child->child->name,"StructSpecifier")){
            if(!strcmp(extDef->child->child->child->sib->name,"Tag")){
                /* cancled error form LiCong's word in 2020-03-29-22:04*
                printSemaError(17, extDef->child->child->child->lineno, extDef->child->child->child->name);
                */
            }else{
                getStructure(extDef->child->child);
            }
        }
    } else if(!strcmp(checkFlag,"FunDec")){
        Type specifier = getSpecifier(extDef->child);
        if(specifier==NULL){return;}
        if(!strcmp(c1s2(extDef)->name,"CompSt")){
            #ifdef SEMABUG
            printf("go here1 %s\n",c1s2(extDef)->name);
            #endif
            getFunc(c1s(extDef),specifier);
            #ifdef SEMABUG
            printAllSym();
            #endif
        } else{ //just declaration
            #ifdef SEMABUG
            printf("go here2\n");
             #endif
            pushEnv();
            declareFunc(c1s(extDef),specifier,1);
            #ifdef SEMABUG
            printAllSym();
            #endif
        }
    } else{
        fprintf(stderr,"Wrong Syn-Tree\n");
    }
}


/*
root: Specifier 
child: TYPE | StructSpecifier
*/
Type getSpecifier(ast* root){
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
        return getStructure(root->child);
    }
}

/*
root: StructSpecifier
child: STRUCT OptTag LC DefList RC
      | STRUCT Tag
*/
Type getStructure(ast* root){
    Type ret;
    if(!strcmp(c1s(root)->name,"Tag")){
        Symbol id = FindStruct(c1s(root)->child); //Param:ID
        if(id==NULL){
            #ifdef SEMABUG
            printAllSym();
            #endif
            printSemaError(17,c1s(root)->child->lineno,c1s(root)->child->context);
            return NULL;
        }else{
            ret = id ->t.type;
        }
    } else{
        #ifdef SEMABUG
            printAllSym();
        #endif
        ret = (Type)malloc(sizeof(struct Type_));
        ret->kind = STRCTDEF;
        pushEnv();
        ret->u.structure = getStructList(c1s3(root),NULL);
        if(ret->u.structure==NULL){
            #ifdef SEMABUG
            printf("NO ELE IN STRUCT\n");
            #endif
            return NULL;
        }
        popEnv();
        #ifdef SEMABUG
            printf("wtf\n");
        #endif
        if(c1s(root)->child!=NULL){
            if(checkDup(c1s(root)->child->context,getEnvdepth())==-1){
                printSemaError(16,c1s(root)->child->lineno,c1s(root)->child->context);
                return NULL;
            };
            Symbol sym = createSymbol(ret,c1s(root)->child->context);
            sym->t.type->kind = STRCTDEF;
            hashInsert(sym);
            envInsert(sym);
            #ifdef SEMABUG
            printf("shit\n");
            printAllSym();
            #endif
        }
    }
    return ret;
}

/*
* root: DefList
* child: Def DefList 
       | NULL
*/
FieldList getStructList(ast* root,FieldList field){
    if(root->child!=NULL){
        if(field==NULL){
            field = malloc(sizeof(struct FieldList_));
        }
        FieldList next = getDef(root->child,field);//返回一系列中最后一个    
        if(next==NULL){
            #ifdef SEMABUG
            printf("shit getdef null in struct\n");
            #endif
            return NULL;
        }
        FieldList newfield = malloc(sizeof(struct FieldList_));
        next->tail = getStructList(c1s(root),newfield);
        return field;
    }
    return NULL;
}

/*
* root: Def
* child: Specifier DecList SEMI
*/

FieldList getDef(ast* root,FieldList field){
    Type specifier = getSpecifier(root->child);
    if(specifier==NULL){return NULL;}
    if(field==NULL){
        field = malloc(sizeof(struct FieldList_));
    }
    field->type = specifier;
    return getDec(c1s(root),field);
}


/*
* root: DecList
* child: Dec
       | Dec COMMA DecList
*/

FieldList getDec(ast* root, FieldList field){
    Type type = field->type;
    #ifdef SEMABUG
    printf("hi shit %s\n",root->name);
    #endif
    if(c1s(root->child)!=NULL){
        printSemaError(15,c1s(root->child)->lineno,"");
        return NULL;
    } else{
        if(c1s(root->child->child)!=NULL){//array
            Symbol sym = getArray(root->child->child,field->type,1);
            field->type = sym->t.type;
            field->name = sym->name;
        }else{
            field->name = root->child->child->child->context;
            Symbol sym = createSymbol(field->type,field->name);
            if(checkDup(sym->name,sym->depth)==-1){
                printSemaError(15,root->child->child->child->lineno,field->name);
                return NULL;
            }
            hashInsert(sym);
            envInsert(sym);
            #ifdef SEMABUG
            printAllSym();
            #endif
        }
    }
    if(c1s(root)!=NULL){
        FieldList newfield = malloc(sizeof(struct FieldList_));
        newfield->type = type;
        field->tail = newfield;
        return getDec(c1s2(root),newfield);
    }
    return field;
}


/*
* root: ExtDecList
* child: VarDec
*      | VarDec COMMA ExtDecList
*/
void getExtDecList(ast* root, Type type){
    getVarDec(root->child,type);
    if(c1s(root)!=NULL){
        getExtDecList(c1s2(root),type);
    }
}

/*
* root: VarDec
* child: ID
*      | VarDec LB INT RB
*/
void getVarDec(ast* root, Type type){
    if(!strcmp(root->child->name,"ID")){
        Symbol sym = createSymbol(type,root->child->context);
        if(checkDup(sym->name,sym->depth)==-1){
            printSemaError(3,root->child->lineno,root->child->context);
            return;
        }
        hashInsert(sym);
        envInsert(sym);
    }else{
        getArray(root,type,0);
    }
}

/*
* root: VarDec
* child: ID
*      | VarDec LB INT RB
*/
Symbol getArray(ast* root, Type type,int inStrcut){
    Type newarray = malloc(sizeof(struct Type_));
    newarray->kind = ARRAY;
    newarray->u.array.elem = type;
    if(!strcmp(root->child->name,"ID")){
        Symbol sym = createSymbol(newarray,root->child->context);
        if(checkDup(sym->name,sym->depth)==-1){
            if(inStrcut){
                printSemaError(15,root->child->lineno,root->child->context);
            }else{
                printSemaError(3,root->child->lineno,root->child->context);
            }
            return NULL;
        }
        hashInsert(sym);
        envInsert(sym);
        return sym;
    }else{
        newarray->u.array.size = c1s2(root)->val.intVal;
        getArray(root->child,newarray,inStrcut);
    }
}

/*
* root: FunDec
* child: ID LP VarList RP
*      | ID LP RP
*/
Symbol declareFunc(ast* root,Type type,int inDec){
    #ifdef SEMABUG
        printf("Try to check function %s\n",root->child->context);
    #endif
    Symbol checkf = findFunc(root->child->context);
    Func func = malloc(sizeof(struct Func_));
    func->returnType = type;
    func->isDefined = 0;
    func->lineno = root->child->lineno; 
    if(!strcmp(c1s2(root)->name,"VarList")){
        func->agru = getArg(c1s2(root),NULL);
    }else{
        func->agru = NULL;
    }
    if(inDec){
        popEnv();
    }
    if(checkf!=NULL){
        if(checkf->isfunc && checkf->t.func->isDefined == 1 && !inDec){
            printSemaError(4,func->lineno,root->child->context);
            return NULL;
        }
        if(!checkf->isfunc || !sameFunc(func,checkf->t.func)){
            printSemaError(19,func->lineno,root->child->context);
            return NULL;
        }
        //free(func);
        return checkf;
    }
    Symbol sym = createFuncSymbol(func,root->child->context);
    hashInsert(sym);
    envInsert(sym);
    return sym;
}

/**
 * root: VarList
 * child: ParamDec COMMA VarList
 *     / ParamDec
*/

Agru getArg(ast* root, Agru arg){
    Agru arg2 = malloc(sizeof(struct Agru_));
    if(arg==NULL){arg=arg2;}else{arg->next = arg2;}
    arg2->type = getParamDec(root->child);
    if(c1s(root)!=NULL){
        getArg(c1s2(root),arg2);
    }
    return arg;
}

/**
 * root: ParamDec
 * child: Specifier VarDec
*/
Type getParamDec(ast* root){
    Type type = getSpecifier(root->child);
    if(c1s(c1s(root))!=NULL){
        Symbol sym = getArray(c1s(root),type,0);
        return sym->t.type;
    }else{
        if(checkDup(c1s(root)->child->context,getEnvdepth())==-1){
                printSemaError(3,c1s(root)->child->lineno,c1s(root)->child->context);
                return NULL;
        };
        Symbol sym = createSymbol(type,c1s(root)->child->context);
        hashInsert(sym);
        envInsert(sym);
        return type;
    }
}

int sameFunc(Func a, Func b){
    if(!sameType(a->returnType,b->returnType)){return 0;}
    Agru ag = a->agru;
    Agru bg = b->agru;
    while(1){
        if(ag==NULL && bg==NULL){
            return 1;
        } else if(ag==NULL||bg==NULL){
            return 0;
        } else{
            if(!sameType(ag->type,bg->type)){return 0;}
            ag = ag->next;bg = bg->next;
        }
    }
    return 1;
}

int sameType(Type a, Type b){
    if(a==NULL && b== NULL){return 1;}
    if(a==NULL || b== NULL){return 0;}
    if(a->kind!=b->kind){return 0;}
    if(a->kind == BASIC){
        if(a->u.basic == b->u.basic){return 1;}else{return 0;}
    }else if(a->kind == ARRAY){
        return sameType(a->u.array.elem,b->u.array.elem);
    } else if(a->kind == STRUCTURE){
        FieldList ta = a->u.structure;
        FieldList tb = b->u.structure;
        while(1){
            if(ta==NULL && tb==NULL){return 1;}
            if(ta==NULL || tb==NULL){return 0;}
            if(!sameType(ta->type,tb->type)){return 0;}
            ta = ta->tail;
            tb = tb->tail;
        }
    } else{
        return 0;
    }
}

/*
* root: FunDec
* child: ID LP VarList RP
*       / ID LP RP
*/
void getFunc(ast* root, Type type){
    pushEnv();
    Symbol sym = declareFunc(root,type,0);
    if(sym==NULL) {popEnv();return;}
    sym->t.func->lineno = root->child->lineno;
    getCompst(root->sib,sym);
    sym->t.func->isDefined = 1;
    popEnv();
}


/*
    * root:Compst
    * child: LC DefList StmtList RC
    */
void getCompst(ast* root,Symbol func){
    pushEnv();
    checkDefList(c1s(root));
    checkStmtList(c1s2(root),func);
    popEnv();
}


/*
* root: DefList
* child: Def DefList 
       | NULL
*/
void checkDefList(ast* root){
    if(root->child!=NULL){
        checkDef(root->child);    
        checkDefList(c1s(root));
    }
}

/*
* root: Def
* child: Specifier DecList SEMI
*/

void checkDef(ast* root){
    Type specifier = getSpecifier(root->child);
    if(specifier==NULL){
        #ifdef SEMABUG
        printf("null specifier \n");
        #endif
        return;
    }
    if(specifier->kind==STRCTDEF){specifier->kind=STRUCTURE;}
    checkDec(c1s(root),specifier);
}


/*
* root: DecList
* child: Dec
       | Dec COMMA DecList
*/

void checkDec(ast* root, Type type){
    if(c1s(root->child)!=NULL){    
        /**
        * root: Dec
        * child: VarDec
        *      | VarDec ASSIGNOP Exp
        */
        Type spec = checkExp(c1s2(root->child));
        if(spec!=NULL && !sameType(spec,type)){
            printSemaError(5,c1s(root->child)->lineno,"");
        }
    }
    if(c1s(root->child->child)!=NULL){//array
        Symbol sym = getArray(root->child->child,type,0);
    }else{
        Symbol sym = createSymbol(type,root->child->child->child->context);
        if(checkDup(sym->name,sym->depth)==-1){
            printSemaError(3,root->child->child->child->lineno,sym->name);
            return;
        }
        hashInsert(sym);
        envInsert(sym);
    }
    if(c1s(root)!=NULL){
        checkDec(c1s2(root),type);
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
Type checkExp(ast* root){
    if(!strcmp(root->child->name,"Exp")){
        if(!strcmp(c1s(root)->name,"ASSIGNOP")){
            Type a = checkExp(root->child);
            Type b = checkExp(c1s2(root));
            if(a==NULL||b==NULL) return NULL;
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
            if(arr == NULL || index==NULL){return NULL;}
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
            if(!sym->isfunc || !sym->t.func->isDefined){
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
            if(sym == NULL ||sym->isfunc){
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
    } else if(!strcmp(root->child->name,"Compst")){
        getCompst(root->child,func);
    } else if(!strcmp(root->child->name,"RETURN")){
        Type type = checkExp(c1s(root));
        checkRet(func->t.func->returnType,type,root->child->lineno);
    } else if(!strcmp(root->child->name,"IF")){
        checkExp(c1s2(root));
        checkStmt(c1s4(root),func);
        if(c1s5(root)!=NULL){
            checkStmt(c1s5(root)->sib,func);
        }
    } else if(!strcmp(root->child->name,"While")){
        checkExp(c1s2(root));
        checkStmt(c1s4(root),func);
    } else {
        assert(0);
    }
}

void checkRet(Type a, Type b, int lineno){
    if(!sameType(a,b)){
        printSemaError(8,lineno,"");
    }
}

void checkFunc(){
    // check undefined function: Error type 18
    Stack env = getEnv();
    assert(env->depth==0);
    Symbol sym = env->next;
    while(sym!=NULL){
        if(sym->isfunc==1 && !sym->t.func->isDefined){
            printSemaError(18,sym->t.func->lineno,sym->name);
        }
        sym = sym->stackNext;
    }
}

/*type*/
Symbol createSymbol(Type type, char* name){
    Symbol ret = malloc(sizeof(struct Symbol_));
    ret->name = name;
    ret->t.type = type;
    ret->isfunc = 0;
    ret->depth = getEnvdepth();
    return ret;
}

Symbol createFuncSymbol(Func func, char* name){
    Symbol ret = malloc(sizeof(struct Symbol_));
    ret->name = name;
    ret->t.func = func;
    ret->isfunc = 1;
    return ret;
}

