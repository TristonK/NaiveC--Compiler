%{
    #include<stdio.h>
    #include<syntax.tab.h>

    int yycolumn = 1;
    #define YY_USER_ACTION\
        yylloc.first_line = yylloc.last_line = yylineno;\
        yylloc.first_column = yycolumn;\
        yylloc.last_column = yycolumn + yyleng -1;\
        yycolumn += yyleng;
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
HEXINT ((0x)|(0X))[0-9a-fA-F]+
FLOAT [0-9]+\.[0-9]+
FLOATEXP [0-9]*\.[0-9]+[eE][+-]?[0-9]+
ID [_a-zA-Z][_a-zA-Z0-9]*

%%
{WS} {printf("WS");}
{NEWLINE} {yycolumn = 1;}
{COMMENTLINE} {printf("it a comment in %d",yylineno);} 
{COMMENT} {printf("COMMENT");char c = input();int flag = 0;while(flag!=2){if(flag==1 && c=='/'){flag=2;break;}else{flag=0;}if(c=='*'){flag=1;}c=input();}} 
{SEMI} {printf(" ; ");}
{COMMA} {printf(" , ");}
{ASSIGNOP} {printf(" = ");}
{RELOP} {printf(" RELOP ");}
{PLUS} {printf(" + ");}
{MINUS} {printf(" - ");}
{STAR} {printf(" * ");}
{DIV} {printf(" / ");}
{AND} {printf(" && ");}
{OR} {printf(" | ");}
{DOT} {printf(" . ");}
{NOT} {printf(" ! ");}
{TYPE} {printf(" TYPE ");}
{LP} {printf(" LP ");}
{RP} {printf(" RP ");}
{LB} {printf(" LB ");}
{RB} {printf(" RB ");}
{LC} {printf(" LC ");}
{RC} {printf(" RC ");}
{STRUCT} {printf(" STRUCT ");}
{RETURN} {printf(" RETURN ");} 
{IF} {printf(" IF ");}
{ELSE} {printf(" ELSE ");}
{WHILE} {printf(" WHILE ");}
{INT} {printf(" INT ");}
{OCTINT} {printf(" OC ");}
{HEXINT} {printf(" HEX ");}
{FLOAT} {printf(" FL ");}
{FLOATEXP} {printf(" FLE ");}
{ID} {printf(" ID ");}
.   { printf("Error type A at Line %d: Mysterious characters \"%s\".\n", yylineno, yytext);}
%%