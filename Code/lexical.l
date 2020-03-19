%{
    #include "common.h"
    #include<syntax.tab.h>

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
{SEMI} {yyval.node = Ast("SEMI",lineno,lex_);return SEMI;}
{COMMA} {yyval.node = Ast("COMMA",lineno,lex_);return COMMA;}
{ASSIGNOP} {yyval.node = Ast("ASSIGNOP",lineno,lex_);return ASSIGNOP;}
{RELOP} {yyval.node = nAst("RELOP",lineno,lex_,yytext); return RELOP;}
{PLUS} {yyval.node = Ast("PLUS",lineno,lex_);return PLUS;}
{MINUS} {yyval.node = Ast("MINUS",lineno,lex_);return MINUS;}
{STAR} {yyval.node = Ast("STAR",lineno,lex_);return STAR;}
{DIV} {yyval.node = Ast("DIV",lineno,lex_);return DIV;}
{AND} {yyval.node = Ast("AND",lineno,lex_);return AND;}
{OR} {yyval.node = Ast("OR",lineno,lex_);return OR;}
{DOT} {yyval.node = Ast("DOT",lineno,lex_);return DOT;}
{NOT} {yyval.node = Ast("NOT",lineno,lex_);return NOT;}
{TYPE} {yyval.node = nAst("TYPE",lineno,type_,yytext);return TYPE;}
{LP} {yyval.node = Ast("LP",lineno,lex_);return LP;}
{RP} {yyval.node = Ast("RP",lineno,lex_);return RP;}
{LB} {yyval.node = Ast("LB",lineno,lex_);return LB;}
{RB} {yyval.node = Ast("RB",lineno,lex_);return RB;}
{LC} {yyval.node = Ast("LC",lineno,lex_);return LC;}
{RC} {yyval.node = Ast("RC",lineno,lex_);return RC;}
{STRUCT} {yyval.node = Ast("STRUCT",lineno,lex_);return STRUCT;}
{RETURN} {yyval.node = Ast("RETURN",lineno,lex_);return RETURN;} 
{IF} {yyval.node = Ast("IF",lineno,lex_);return IF;}
{ELSE} {yyval.node = Ast("ELSE",lineno,lex_);return ELSE;}
{WHILE} {yyval.node = Ast("WHILE",lineno,lex_);return WHILE;}
{INT} {yyval.node = Ast("INT",lineno,int_);yyval.node->val.intVal=atoi(yytext);return INT;}
{OCTINT} {yyval.node = Ast("INT",lineno,int_);yyval.node->val.intVal=(int)strtol(yytext,NULL,8);return INT;}
{OCTINT_BAD} {fprintf(stderr,"Error type A at Line %d: Illegal octal number \'%s\'.\n",yylineno,yytext);lex_error=1;}
{HEXINT} {yyval.node = Ast("INT",lineno,int_);yyval.node->val.intVal=(int)strtol(yytext,NULL,16);return INT;}
{HEXINT_BAD} {fprintf(stderr,"Error type A at Line %d: Illegal hexadecimal number \'%s\'.\n",yylineno,yytext);lex_error=1;}
{FLOAT} {yyval.node = Ast("FLOAT",lineno,float_);yyval.node->val.intVal=atof(yytext);return FLOAT;}
{FLOATEXP} {yyval.node = Ast("FLOAT",lineno,float_);yyval.node->val.intVal=atoi(yytext);return FLOAT;}
{FLOATEXP_BAD} {fprintf(stderr,"Error type A at Line %d: Illegal floating point number \"%s\".\n",yylineno,yytext);lex_error=1;}
{ID} {yyval.node = nAst("ID",lineno,id_,yytext);return ID;}
.   { fprintf(stderr,"Error type A at Line %d: Mysterious characters \"%s\".\n", yylineno, yytext);lex_error = 1;}
%%