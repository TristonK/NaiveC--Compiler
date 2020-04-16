#include "common.h"

Symbol hashTable[hashSize];

/* 
Algorithm By P.J.Weinberger
Get hash. Hash table size: 0x3fff
*/
unsigned int getHash(char* name){
    unsigned int val = 0,i;
    for(;*name;++name){
        val = (val<<2) + *name;
        if(i=val & ~hashSize){
            val = (val^(i>>12)) & hashSize;
        }
    }
    return val;
}

void createHash(){
    for(int i=0;i<hashSize;i++){
        hashTable[i] = NULL;
    }
}

int checkDup(char* name, int depth){
    unsigned int hashNum = getHash(name);
    Symbol checkDup = hashTable[hashNum];
    while(checkDup!=NULL){
        if(checkDup->depth < depth){break;}
        if(checkDup->depth == depth && !strcmp(checkDup->name,name)){
            return -1;
        }
        checkDup = checkDup->hashNext;
    }
    return 0;
}

Symbol findFunc(char* name){
    unsigned int hashNum = getHash(name);
    Symbol checkDup = hashTable[hashNum];
    while(checkDup!=NULL){
        if(!strcmp(checkDup->name,name)){
            break;
        }
        checkDup = checkDup->hashNext;
    }
    return checkDup;
}

void hashInsert(Symbol node){
    unsigned int hashNum = getHash(node->name);
    if(hashTable[hashNum]==NULL){hashTable[hashNum] = node;}
    else{
        node->hashNext = hashTable[hashNum];
        hashTable[hashNum] = node;
    }
}

int hashDelete(Symbol node){
    unsigned int hashNum = getHash(node->name);
    assert(hashTable[hashNum]!=NULL);
    if(hashTable[hashNum]==node){    
        hashTable[hashNum] = node->hashNext;
    } else{
        Symbol last = hashTable[hashNum];
        while(last->hashNext!=node){
            last=last->hashNext;
        }
        last->hashNext = node->hashNext;
    }
    #ifdef FREEM
    freeSymbol(node);
    #endif
}

/*
root: ID
*/
Symbol hashFind(ast* root){
    unsigned int nameHash = getHash(root->context);
    Symbol ret = hashTable[nameHash];
    while(ret!=NULL && strcmp(ret->name,root->context)){
        ret = ret->hashNext;
    }
    if(ret==NULL){
        ret = NULL;
    }
    return ret;
}

Symbol FindStruct(ast* root){
    unsigned int nameHash = getHash(root->context);
    Symbol ret = hashTable[nameHash];
    while(ret!=NULL){
        if(ret->t.type->kind==STRCTDEF && !strcmp(ret->name,root->context)){
            return ret;
        }
        ret = ret->hashNext;
    }
    if(ret==NULL){
        ret = NULL;
    }
    return ret;
}

