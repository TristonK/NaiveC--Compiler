%{
    #include "common.h"
    ast* root;
    #define YYERROR_VERBOSE
    int emptyFile=1;
    int syn_error = 0;
%}

/*definitions*/
%union {
    ast* node;
}

%token <node> INT FLOAT ID
%token <node> SEMI COMMA
%token <node> ASSIGNOP RELOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR
%token <node> DOT
%token <node> NOT
%token <node> TYPE
%token <node> LP RP LB RB LC RC
%token <node> STRUCT RETURN IF ELSE WHILE

%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt
%type <node> DefList Def DecList Dec
%type <node> Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UMINUS
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

Program: ExtDefList {$$=Ast("Program",@$.first_line,nonTerm_);root=$$;addChild($$,$1);}
    ;
ExtDefList: ExtDef ExtDefList {$$=Ast("ExtDefList",@$.first_line,nonTerm_);addChild($$,$1);addChild($$,$2);emptyFile=0;}
    | {$$=Ast("ExtDefList",@$.first_line,nonTerm_);}
    ;
ExtDef: Specifier ExtDecList SEMI {$$=Ast("ExtDef",@$.first_line,nonTerm_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Specifier SEMI {$$=Ast("ExtDef",@$.first_line,nonTerm_);addChild($$,$1);addChild($$,$2);}
    | Specifier FunDec CompSt {$$=Ast("ExtDef",@$.first_line,nonTerm_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Specifier FunDec SEMI {$$=Ast("ExtDef",@$.first_line,nonTerm_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | error SEMI {}
    ;
ExtDecList: VarDec {$$=Ast("ExtDecList",@$.first_line,nonTerm_);addChild($$,$1);}
    | VarDec COMMA ExtDecList {$$=Ast("ExtDecList",@$.first_line,nonTerm_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    ;
Specifier: TYPE {$$=Ast("Specifier",@$.first_line,nonTerm_);addChild($$,$1);}
    | StructSpecifier {$$=Ast("Specifier",@$.first_line,nonTerm_);addChild($$,$1);}
    ;
StructSpecifier: STRUCT OptTag LC DefList RC {$$=Ast("StructSpecifier",@$.first_line,nonTerm_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);addChild($$,$5);}
    | STRUCT Tag {$$=Ast("StructSpecifier",@$.first_line,nonTerm_);addChild($$,$1);addChild($$,$2);}
    | STRUCT OptTag LC error RC {}
    ;
OptTag: ID {$$=Ast("OptTag",@$.first_line,syn_);addChild($$,$1);}
    | {$$=Ast("OptTag",@$.first_line,nonTerm_);}
    ;
Tag: ID{$$=Ast("Tag",@$.first_line,syn_);addChild($$,$1);}
    ;
VarDec: ID {$$=Ast("VarDec",@$.first_line,syn_);addChild($$,$1);}
    | VarDec LB INT RB {$$=Ast("VarDec",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
    | VarDec LB error RB {}
    | error RB {}
    ;
FunDec: ID LP VarList RP {$$=Ast("FunDec",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
    | ID LP RP {$$=Ast("FunDec",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | ID LP error RP {}
    ;
VarList: ParamDec COMMA VarList {$$=Ast("VarList",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | ParamDec {$$=Ast("VarList",@$.first_line,syn_);addChild($$,$1);}
    ;
ParamDec: Specifier VarDec {$$=Ast("ParamDec",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);}
    | error RP {}
    | error SEMI {}
    ;
CompSt: LC DefList StmtList RC {$$=Ast("CompSt",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
    ;
StmtList: Stmt StmtList {$$=Ast("StmtList",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);}
    | {$$=Ast("StmtList",@$.first_line,nonTerm_);}
    ;
Stmt: Exp SEMI {$$=Ast("Stmt",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);}
    | CompSt {$$=Ast("Stmt",@$.first_line,syn_);addChild($$,$1);}
    | RETURN Exp SEMI {$$=Ast("Stmt",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=Ast("Stmt",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);addChild($$,$5);}
    | IF LP Exp RP Stmt ELSE Stmt {$$=Ast("Stmt",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);addChild($$,$5);addChild($$,$6);addChild($$,$7);}
    | WHILE LP Exp RP Stmt {$$=Ast("Stmt",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);addChild($$,$5);}
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE {}
    | WHILE LP error RP Stmt {}
    | error SEMI {}
    | error {}
    ;
DefList: Def DefList {$$=Ast("DefList",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);}
    | {$$=Ast("DefList",@$.first_line,nonTerm_);}
    ;
Def: Specifier DecList SEMI {$$=Ast("Def",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    ;
DecList: Dec {$$=Ast("DecList",@$.first_line,syn_);addChild($$,$1);}
    | Dec COMMA DecList {$$=Ast("DecList",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    ;
Dec: VarDec {$$=Ast("Dec",@$.first_line,syn_);addChild($$,$1);}
    | VarDec ASSIGNOP Exp {$$=Ast("Dec",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    ;
Exp: Exp ASSIGNOP Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp AND Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp OR Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp RELOP Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp PLUS Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp MINUS Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp STAR Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp DIV Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | LP Exp RP {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | MINUS Exp %prec UMINUS {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);}
    | NOT Exp {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);}
    | ID LP Args RP {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
    | ID LP RP {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp LB Exp RB {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);addChild($$,$4);}
    | Exp DOT ID {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | ID {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);}
    | INT {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);}
    | FLOAT {$$=Ast("Exp",@$.first_line,syn_);addChild($$,$1);}
    | error RP {}
    ;
Args: Exp COMMA Args {$$=Ast("Args",@$.first_line,syn_);addChild($$,$1);addChild($$,$2);addChild($$,$3);}
    | Exp {$$=Ast("Args",@$.first_line,syn_);addChild($$,$1);}
    ;

%%
#include "lex.yy.c"

int yyerror(const char* msg){
    syn_error = 1;
    printf("Error type B at Line %d: %s.\n", yylineno, msg);
    return 0;
}
