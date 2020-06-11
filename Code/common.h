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

typedef struct Operand_* Operand;

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
    Operand op;
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
Symbol hashFindName(char* name);

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
void irExtDefList(ast* root);
void irExtDef(ast* extDef);
Type irSpecifier(ast* root);
Type irStructure(ast* root);
FieldList irStructList(ast* root,FieldList field);
FieldList irDefInStruct(ast* root,FieldList field);
FieldList irDecListInStruct(ast* root, FieldList field);
void irVarDec(ast* root, Type type);
Symbol irGetArray(ast* root, Type type,int inStrcut);
int getArraySize(Symbol sym);
Symbol irFunDec(ast* root,Type type);
Agru irGetArg(ast* root, Agru arg);
Type irParamDec(ast* root);
void irCompst(ast* root,Symbol func);
void irDefList(ast* root);
void irDef(ast* root);
void irDec(ast* root, Type type);
Type irExp(ast* root,Operand place);
int irTypeSize(Type a);
void irCond(ast* root, Operand label_true, Operand label_false);
ArgList irTransArgs(ast* root,ArgList arg_list);
void irStmtList(ast* root, Symbol func);
void irStmt(ast* root,Symbol func);
Symbol irCreateSymbol(Type type, char* name);
Symbol irCreateFuncSymbol(Func func, char* name);
Operand irOpFunc(char* name);
Operand irOpVar(char* name);
Operand irOpConstant(int num);
Operand irOpTemp();
Operand irOpOp(char* op);
Operand irOpRelop(char* relop);
Operand irOpLabel();
Operand irOpAddr(char* name);
void irCodeOp1(int kind,Operand op1);
void irCodeOp2(int kind,Operand op1,Operand op2);
void irCodeOp3(int kind,Operand result,Operand op1,Operand op2);
void irCodeOp4(int kind,Operand left,Operand relop,Operand right,Operand label);

FILE *ir_out;
InterCodes ir_root;
InterCodes ir_tail;

//ir_file.c

void printIRCodes(FILE* out);

void optimize();

//mips_out.c

FILE *mips_out;
void printMipsCodes(FILE* out);

//mips.h
typedef struct Mips_Operand_* MipsOperand;
typedef struct MipsCode_* MipsCode;

struct Mips_Operand_
{
    enum {
        MOP_LABEL,MOP_REG,MOP_IMM,MOP_FUNC,MOP_OFFSET
    } kind;
    union{
        int value;
        char* name;
        struct {char* reg; int offset;} offset;
    }u;
};

struct MipsCode_{
    enum{
        MIPS_LABEL,MIPS_LI,MIPS_MOVE,
        MIPS_ADDI,MIPS_ADD,MIPS_SUB,MIPS_MUL,MIPS_DIV,MIPS_MFLO,
        MIPS_SW,MIPS_LW,
        MIPS_J,
        MIPS_JAL,MIPS_JR,
        MIPS_BEQ,MIPS_BNE,MIPS_BGT,MIPS_BLT,MIPS_BGE,MIPS_BLE,
        MIPS_ANNO
        } kind;
    union{
        struct{MipsOperand op1,op2;} assign;//from left to right
        struct{MipsOperand op1,op2,op3;}binop;
        struct{Operand op1;} single_op;
        char* annotation;
    }u;
    MipsCode next;
};


//mips.c