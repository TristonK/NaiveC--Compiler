# include<stdio.h>
#include<stdlib.h>

enum astOutType {syn_,nonTerm_,lex_,id_,type_,int_,float_};

typedef struct AST {   
    char* name;
    char* context;
    int outType;
    int lineno;
    union{
        float floatVal; 
        unsigned int intVal; 
    }val; 
    ast* parent;
    ast* child;
    ast* sib;
}ast;

ast* Ast(char* name,int lineno,int type);
void addChild(ast* p, ast* c);