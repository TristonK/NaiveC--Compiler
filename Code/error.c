#include "common.h"

char* semaError[19] = {
    "Undefined variable \"%s\".\n", // 1
    "Undefined function \"%s\".\n", // 2
    "Redefined variable \"%s\".\n", // 3
    "Redefined function \"%s\".\n", // 4
    "Type mismatched for assignment%s.\n", // 5
    "The left-hand side of an assignment must be a variable%s.\n", // 6
    "Type mismatched for operands%s.\n", // 7
    "Type mismatched for return%s.\n", // 8
    "Function \"%s\" is not applicable for arguments.\n", // 9
    "\"%s\" is not an array.\n", // 10
    "\"%s\" is not a function.\n", // 11
    "\"%s\" is not an integer.\n", // 12
    "Illegal use of \".\"%s.\n", // 13
    "Non-existent field \"%s\".\n", // 14
    "Redefined field \"%s\" or try to initialize.\n", // 15
    "Duplicated name \"%s\".\n", // 16
    "Undefined structure \"%s\".\n", // 17
    "Undefined function \"%s\".\n", // 18
    "Inconsistent declaration of function \"%s\".\n" //19
};

void printSemaError(int type, int lineno, char* info){
    fprintf(stderr, "Error type %d at Line %d: ",type,lineno);
    fprintf(stderr, semaError[type-1],info);   
}