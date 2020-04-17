#include "common.h"

Stack env;

void createEnv(){
    env = malloc(sizeof(struct Stack_));
    env->depth = 0;
}

void pushEnv(){
    Stack newEnv = malloc(sizeof(struct Stack_));
    //env->nextEnv = newEnv;
    newEnv->depth = env->depth+1;
    newEnv->lastEnv = env;
    env = newEnv;
}

void popEnv(){
    #ifdef SEMABUG
    printf("now pop\n");
    #endif
    Stack lastEnv = env->lastEnv;
    clearEnv();
    #ifdef FREEM
    //to be add
    freeEnv(env)
    #endif
    env = lastEnv;
}

void clearEnv(){
    Symbol toFree = env->next; 
    while(toFree!=NULL){
        #ifdef FREEM
        freeSymbol(toFree);
        #endif
        if(!toFree->isfunc){
            hashDelete(toFree);
        }
        toFree = toFree->stackNext;
    }
}

void envInsert(Symbol toAdd){
    Symbol fir = env->next;
    if(fir==NULL){env->next = toAdd;return;}
    while(fir->stackNext!=NULL){fir=fir->stackNext;}
    fir->stackNext = toAdd;
}

int getEnvdepth(){
    return env->depth;
}

Stack getEnv(){
    return env;
}