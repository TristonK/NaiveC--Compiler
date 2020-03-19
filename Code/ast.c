# include "common.h"

#define Ast(name,lineno,type) nAst(name,lineno,type,NULL)

ast* nAst(char* name,int lineno,int type,char* cont){
    ast* node = (ast*) malloc(sizeof(ast));
    node->name = (char *)malloc(strlen(name)+1);
    strcpy(node->name,name);
    if(cont!=NULL){
        node->context = (char*)malloc(strlen(cont)+1);
        strcpy(node->context,cont);
    }
    node->lineno = lineno;
    node->outType = type;
    return node;
}



void addChild(ast* p, ast* c){
    if(p->child==NULL){p->child=c;c->parent=p;return;}
    ast* tmp = p->child;
    while(tmp->sib!=NULL){tmp=tmp->sib;}
    tmp->sib = c;
    c->parent = p;
}

void printTree(ast* root, int index){

}