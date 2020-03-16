%{

%}

/*definitions*/


%%

/*rules*/

%%
#include "lex.yy.c"

yyerror(char* msg){
    fprintf(stderr,"error: %s\n", msg);
}
