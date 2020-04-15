#include "common.h"

void semaAnalysis(ast* root){
    createHash();
    getExtDefList(root->child); //extDefList
    checkFunc();
}

void getExtDef(ast* root){
    if(root!=NULL && root->child!=NULL){
        // ExtDefList -> ExtDef ExtDefList
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
    if(!strcmp(checkFlag,"ExtDecList")){

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
        if(!strcmp(c1s(extDef)->sib,"CompSt")){

        } else{ //just declaration
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
        Symbol id = hashFind(c1s(root)->child); //Param:ID
        if(id==NULL){
            printSemaError(17,c1s(root)->child->lineno,c1s(root)->child->name);
            retrun NULL;
        }else{
            ret = id ->t.type;
        }
    } else{
        ret = (Type)malloc(sizeof(struct Type_));
        ret->kind = STRUCTURE;
        FieldList struct_ = malloc(sizeof(struct FieldList_));
        if(c1s(root)->child!=NULL){
            struct_->name = c1s(root)->child->context;
        }
        else{ 
            struct_-> name = "";
        }
        ret->u.structure = struct_;
        
    }
    return ret;
}

void declareFunc(){}

void getFunc(){}

void checkFunc(){
    // check undefined function: Error type 18
}