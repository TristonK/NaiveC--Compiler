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

void checkExtDef(ast* extDef){
    char* checkFlag = c1s(extDef)->name;
    if(!strcmp(checkFlag,"ExtDecList")){

    } else if(!strcmp(checkFlag,"SEMI")){
        if(!strcmp(extDef->child->child->name,"StructSpecfier")){
            if(!strcmp(extDef->child->child->child->sib->name,"Tag")){
                /* cancled error form LiCong's word in 2020-03-29-22:04*
                printSemaError(17, extDef->child->child->child->lineno, extDef->child->child->child->name);
                */
            }else{
                getStructure(extDef->child);
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

void getStructure(ast* specifier){}

void declareFunc(){}

void getFunc(){}

void checkFunc(){
    // check undefined function: Error type 18
}