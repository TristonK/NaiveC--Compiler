#include "common.h"

Symbol createSymbol(char* name){
    Symbol ret = malloc(sizeof(Symbol));
    ret->name = name;
    return ret;
}

void freeSymble(Symbol node){
    free((void*)node);
}