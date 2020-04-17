#include "common.h"

Symbol CreateSymbol(char* name){
    Symbol ret = malloc(sizeof(struct Symbol_));
    ret->name = name;
    return ret;
}

void freeSymble(Symbol node){
    free((void*)node);
}