#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>

//#define SEMABUG

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
    int isdef;
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
Symbol findFunc(char* name);
void printAllSym();
int missDepthDup(char* name, int depth);

//envStack.c

void createEnv();
void pushEnv();
void popEnv();
void clearEnv();
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
Type getSpecifier(ast* root);
Type getStructure(ast* root);
FieldList getStructList(ast* root,FieldList field);
FieldList getDef(ast* root,FieldList field);
FieldList getDec(ast* root, FieldList field);
void getExtDecList(ast* root, Type type);
void getVarDec(ast* root, Type type);
Symbol getArray(ast* root, Type type,int inStrcut);
Symbol declareFunc(ast* root,Type type,int inDec);
Agru getArg(ast* root, Agru arg);
Type getParamDec(ast* root);
int sameFunc(Func a, Func b);
int sameType(Type a, Type b);
void getFunc(ast* root, Type type);
void getCompst(ast* root,Symbol func,int bigfunc);
void checkDefList(ast* root);
void checkDef(ast* root);
void checkDec(ast* root, Type type);
Type checkExp(ast* root);
int isVarible(ast* root);
Agru getArgs(ast* root,Agru arg);
void checkStmtList(ast* root, Symbol func);
void checkStmt(ast* root,Symbol func);
void checkRet(Type a, Type b, int lineno);
void checkFunc();
Symbol createSymbol(Type type, char* name);
Symbol createFuncSymbol(Func func, char* name);


// IR.c

int temp_gen;
int label_gen;

typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct InterCode InterCode;
typedef struct ArgList_* ArgList;

struct Operand_
{
    enum {OP_VARIABLE,OP_CONSTANT,OP_ADDRESS,OP_FUNC,OP_LABEL,OP_RELOP,OP_MATH} kind;
    union{
        int var_no;
        int value;
        char* name;
    }u;
};

struct InterCode{
    enum{
        CODE_ASSIGN=0,
        CODE_LABEL,
        CODE_REF,CODE_DEREF,CODE_DEREF_ASSIGN,
        CODE_ADD,CODE_SUB,CODE_MUL,CODE_DIV,
        CODE_FUNC,CODE_PARAM,CODE_ARG,CODE_RETURN,CODE_CALL,
        CODE_READ,CODE_WRITE,
        CODE_IF,CODE_GOTO,
        CODE_DEC
        } kind;
    union{
        struct{Operand right,left;} assign;
        struct{Operand result,op1,op2;}binop;
        struct{Operand left,relop,right,label;} if_op;
        struct{Operand result;} single_op;
    }u;
};

struct InterCodes_{
    InterCode code;
    InterCodes prev,next;
};

struct ArgList_{
    Operand arg;
    ArgList next;
};

void IrAnalysis(ast* root);

InterCodes ir_root;
InterCodes ir_tail;

//ir_file.c

void printIRCode(FILE* out);