#include "common.h"

void semaAnalysis(ast* root){
    createHash();
    createEnv();
    getExtDefList(root->child); //extDefList
    checkFunc();
}


/*
* root: extDefList
* child: ExtDef ExtdefList
*/
void getExtDef(ast* root){
    if(root!=NULL && root->child!=NULL){
        checkExtDef(root->child);
        getExtDefList(root->sib);
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
    Type specifier = getSpecifier(extDef->child);
    if(specifier==NULL){return;}
    if(!strcmp(checkFlag,"ExtDecList")){
        getExtDecList(c1s(extDef),specifier);
    } else if(!strcmp(checkFlag,"SEMI")){
        if(!strcmp(extDef->child->child->name,"StructSpecfier")){
            if(!strcmp(extDef->child->child->child->sib->name,"Tag")){
                /* cancled error form LiCong's word in 2020-03-29-22:04*
                printSemaError(17, extDef->child->child->child->lineno, extDef->child->child->child->name);
                */
            }else{
                getStructure(extDef->child->child);
            }
        }
    } else if(!strcmp(checkFlag,"FunDec")){
        if(c1s2(extDef)!=NULL){
            getFunc(c1s(extDef),specifier);
        } else{ //just declaration
            declareFunc(c1s(extDef),specifier);
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
            printSemaError(17,c1s(root)->child->lineno,c1s(root)->child->name);
            retrun NULL;
        }else{
            ret = id ->t.type;
        }
    } else{
        ret = (Type)malloc(sizeof(struct Type_));
        ret->kind = STRCTDEF;
        pushStack();
        ret->u.structure = getStructList(c1s3(root),NULL);
        if(ret->u.structure==NULL){return NULL;}
        popStack();
        if(c1s(root)->child!=NULL){
            if(checkDup(c1s(root)->name,getEnvdepth())==-1){
                printSemaError(16,c1s(root)->child->lineno,c1s(root)->child->name);
                return NULL;
            };
            Symbol sym = createSymbol(ret,c1s(root)->child->name);
            hashInsert(sym);
            envInsert(sym);
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
        FieldList newfield = malloc(sizeof(struct FieldList_));
        FieldList next = getDef(root->child,field);    
        if(next==NULL){return NULL;}
        next->tail = getStructList(c1s(root),newfield);
        return newfield;
    }
    return NULL;
}

/*
* root: Def
* child: Specifier DecList SEMI
*/

FieldList getDef(ast* root,FieldList field){
    Type specifier = getSpecifier(root->child);
    if(specifier==NULL){return;}
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
    if(c1s(root->child)!=NULL){
        printSemaError(15,c1s(root->child)->lineno,"");
        return NULL;
    } else{
        if(c1s(root->child->child)!=NULL){//array
            Symbol sym = getArray(root->child->child,field->type,1);
            field->type = sym->t.type;
            field->name = sym->name;
        }else{
            strcpy(field->name,root->child->child->child->context);
            Symbol sym = createSymbol(field->type,field->name);
            if(checkDup(sym->name,sym->depth)==-1){
                printSemaError(15,root->child->child->child->lineno,field->name);
                return NULL;
            }
            hashInsert(sym);
            envInsert(sym);
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
Symbol declareFunc(ast* root,Type type){
    Symbol checkf = findFunc(root->child->context);
    Func func = malloc(sizeof(struct Func_));
    func->returnType = type;
    func->isDefined = 0;
    func->lineno = root->child->lineno;    
    if(checkf!=NULL){
        if(!checkf->isfunc || !sameFunc(func,checkf->t.func)){
            printSemaError(19,func->lineno,root->child->context);
            return NULL;
        }
        free(func);
        return checkf;
    }
    Symbol sym = createFuncSymbol(func,root->child->context);
    hashInsert(sym);
    envInsert(sym);
    return sym;
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


void getFunc(ast* root, Type type){
    Symbol sym = declareFunc(root,type);
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