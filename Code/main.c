#include "common.h"

extern FILE* yyin;
extern ast* root;
extern int lex_error;
extern int syn_error;
extern int emptyFile;
//extern int emptyeof;
extern int yylineno;

int main(int argc, char** argv){
    if(argc>1){
        if(!(yyin = fopen(argv[1],"r"))){
            perror(argv[1]);
            return 1;
        }
        yyrestart(yyin);
    } else{return 1;}
    root = malloc(sizeof(ast));
    //yydebug = 1;
    yyparse();
    if(!lex_error && !syn_error){
        if(emptyFile){
            printf("Program (%d)\n", yylineno);
        }else{
            printTree(root,0);
            //printf("end\n");
        }
    }
    return 0;
}