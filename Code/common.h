# include<stdio.h>
#include<stdlib.h>

// lex

int yylex();
void yyrestart(FILE* yyin);
int yyparse();
int yyerror(const char* msg);

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

/*--------------^-^----sema--------------------------------*/

//symtable

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Agru_* Agru;
typedef struct Func_* Func;
typedef struct Symbol_* Symbol;
typedef struct Stack_* Stack;
typedef struct retType_* retType;

#define hashSize 0x3fff

struct Type_
{
    enum {BASIC=1, ARRAY=3,STRUCTURE=4,STRCTDEF=5} kind;
    union 
    {
        int basic;// int:1 float:2
        struct {
            Type elem;
            int size;
        }array;
        FieldList structure;
    }u;
};

struct FieldList_
{
    char* name;
    Type type;
    FieldList tail;
};

struct Agru_
{
    Type type;
    Agru next;
};

struct Func_
{
    Type returnType;
    int isDefined; /*1-defined 0-notDefined*/
    int lineno;
    Agru agru;
};

struct Symbol_
{
    char* name;
    union 
    {
        Type type;
        Func func;
    }t;
    Symbol hashNext;
    Symbol stackNext;
    int depth;
    int isfunc;
};

struct Stack_
{
    int depth;
    Stack lastEnv;
    Symbol next;
};

struct retType_{
    Type type;
    retType next;
};

// error.c
void printSemaError(int type, int lineno, char* info);

// hash.c
unsigned int getHash(char* name);
void createHash();
void hashInsert(Symbol node);
int hashDelete(Symbol node);
Symbol hashFind(ast* root);
int checkDup(char* name, int depth);
Symbol FindStruct(ast* root);
Symbol FindFunc(char* name);

//envStack.c

void createEnv();
void pushStack();
void popStack();
void envInsert(Symbol toAdd);
int getEnvdepth();
Stack getEnv();

//sematic.c
#define c1s(x) x->child->sib
#define c2s(x) x->child->child->sib
#define c1s2(x) x->child->sib->sib
#define c1s3(x) x->child->sib->sib->sib
#define c1s4(x) x->child->sib->sib->sib->sib
#define c1s5(x) x->child->sib->sib->sib->sib->sib
#define c2s2(x) x->child->child->sib->sib

void semaAnalysis(ast* root);
void getExtDef(ast* root);
void checkExtDef(ast* extDef);
Type getStructure(ast* root);
