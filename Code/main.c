#include "common.h"

extern FILE* yyin;
extern ast* root;
extern int lex_error;

int main(int argc, char** argv){
    if(argc>1){
        if(!(yyin = fopen(argv[1],"r"))){
            perror(argv[1]);
            return 1;
        }
        yyrestart(yyin);
    } else{return 1;}
    root = malloc(sizeof(ast));
    yyparse();
    if(!lex_error){
        printTree(root,0);
    }
    return 0;
}
