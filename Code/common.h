# include<stdio.h>
#include<stdlib.h>

int yylex();
void yyrestart(FILE*);
int yyparse();
int yyerror(char* msg);

enum astOutType {syn_,nonTerm_,lex_,id_,type_,int_,float_};

typedef struct ast {   
    char* name;
    char* context;
    int outType;
    int lineno;
    union{
        float floatVal; 
        unsigned int intVal; 
    }val; 
    struct ast* parent;
    struct ast* child;
    struct ast* sib;
}ast;

#define Ast(name,lineno,type) nAst(name,lineno,type,NULL)
ast* nAst(char* name,int lineno,int type,char* context);
void addChild(ast* p, ast* c);
void printTree(ast* root, int index);