%{
    #include "syntax.tab.h"
    //#include "common.h"
    FILE* yyin;
    int yycolumn = 1;
    #define YY_USER_ACTION\
        yylloc.first_line = yylloc.last_line = yylineno;\
        yylloc.first_column = yycolumn;\
        yylloc.last_column = yycolumn + yyleng -1;\
        yycolumn += yyleng;
    int lex_error = 0;
%}

%option yylineno

/*regular definitions*/
delim [ \t]
WS {delim}+
NEWLINE [\n\r]+
COMMENTLINE \/\/[^\n]*
COMMENT \/\*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP  (>)|(<)|(>=)|(<=)|(==)|(!=)
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT \!
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
INT (0)|([1-9][0-9]*)
OCTINT (0[0-7]+)
OCTINT_BAD (0[0-9a-zA-Z]+])
HEXINT 0[xX][0-9a-fA-F]+
HEXINT_BAD 0[xX][0-9a-zA-Z]+
FLOAT [0-9]+\.[0-9]+
FLOAT_BAD ([0-9]+\.)|(\.[0-9]+)
FLOATEXP (([0-9]*\.[0-9]+)|([0-9]+\.))[eE][+-]?[0-9]+
FLOATEXP_BAD ((([0-9]*\.[0-9]+)|([0-9]+\.))[eE][+-]?)|((([0-9]*\.[0-9]+)|([0-9]+\.))[eE][+-]?[0-9]*\.[0-9]*)
ID [_a-zA-Z][_a-zA-Z0-9]*

%%
{WS} { }
{NEWLINE} {yycolumn = 1;}
{COMMENTLINE} { } 
{COMMENT} {char c = input();int flag = 0;while(flag!=2){if(flag==1 && c=='/'){flag=2;break;}else{flag=0;}if(c=='*'){flag=1;}c=input();}} 
{SEMI} {yylval.node = Ast("SEMI",yylineno,lex_);return SEMI;}
{COMMA} {yylval.node = Ast("COMMA",yylineno,lex_);return COMMA;}
{ASSIGNOP} {yylval.node = Ast("ASSIGNOP",yylineno,lex_);return ASSIGNOP;}
{RELOP} {yylval.node = nAst("RELOP",yylineno,lex_,yytext); return RELOP;}
{PLUS} {yylval.node = Ast("PLUS",yylineno,lex_);return PLUS;}
{MINUS} {yylval.node = Ast("MINUS",yylineno,lex_);return MINUS;}
{STAR} {yylval.node = Ast("STAR",yylineno,lex_);return STAR;}
{DIV} {yylval.node = Ast("DIV",yylineno,lex_);return DIV;}
{AND} {yylval.node = Ast("AND",yylineno,lex_);return AND;}
{OR} {yylval.node = Ast("OR",yylineno,lex_);return OR;}
{DOT} {yylval.node = Ast("DOT",yylineno,lex_);return DOT;}
{NOT} {yylval.node = Ast("NOT",yylineno,lex_);return NOT;}
{TYPE} {yylval.node = nAst("TYPE",yylineno,type_,yytext);return TYPE;}
{LP} {yylval.node = Ast("LP",yylineno,lex_);return LP;}
{RP} {yylval.node = Ast("RP",yylineno,lex_);return RP;}
{LB} {yylval.node = Ast("LB",yylineno,lex_);return LB;}
{RB} {yylval.node = Ast("RB",yylineno,lex_);return RB;}
{LC} {yylval.node = Ast("LC",yylineno,lex_);return LC;}
{RC} {yylval.node = Ast("RC",yylineno,lex_);return RC;}
{STRUCT} {yylval.node = Ast("STRUCT",yylineno,lex_);return STRUCT;}
{RETURN} {yylval.node = Ast("RETURN",yylineno,lex_);return RETURN;} 
{IF} {yylval.node = Ast("IF",yylineno,lex_);return IF;}
{ELSE} {yylval.node = Ast("ELSE",yylineno,lex_);return ELSE;}
{WHILE} {yylval.node = Ast("WHILE",yylineno,lex_);return WHILE;}
{INT} {yylval.node = Ast("INT",yylineno,int_);yylval.node->val.intVal=atoi(yytext);return INT;}
{OCTINT} {yylval.node = Ast("INT",yylineno,int_);yylval.node->val.intVal=(int)strtol(yytext,NULL,8);return INT;}
{OCTINT_BAD} {fprintf(stderr,"Error type A at Line %d: Illegal octal number \'%s\'.\n",yylineno,yytext);lex_error=1;}
{HEXINT} {yylval.node = Ast("INT",yylineno,int_);yylval.node->val.intVal=(int)strtol(yytext,NULL,16);return INT;}
{HEXINT_BAD} {fprintf(stderr,"Error type A at Line %d: Illegal hexadecimal number \'%s\'.\n",yylineno,yytext);lex_error=1;}
{FLOAT} {yylval.node = Ast("FLOAT",yylineno,float_);yylval.node->val.intVal=atof(yytext);return FLOAT;}
{FLOATEXP} {yylval.node = Ast("FLOAT",yylineno,float_);yylval.node->val.intVal=atoi(yytext);return FLOAT;}
{FLOATEXP_BAD} {fprintf(stderr,"Error type A at Line %d: Illegal floating point number \"%s\".\n",yylineno,yytext);lex_error=1;}
{ID} {yylval.node = nAst("ID",yylineno,id_,yytext);return ID;}
.   { fprintf(stderr,"Error type A at Line %d: Mysterious characters \"%s\".\n", yylineno, yytext);lex_error = 1;}
%%