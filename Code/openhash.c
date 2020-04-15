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

void hashInsert(Symbol node){
    unsigned int hashNum = getHash(node->name);
    if(hashTable[hashNum]==NULL){hashTable[hashNum] = node;}
    else{node->hashNext = hashTable[hashNum];hashTable[hashNum] = node;}
}

int hashDelete(Symbol node){
    unsigned int hashNum = getHash(node->name);
    assert(hashTable[hashNum]!=NULL);
    hashTable[hashNum] = node->hashNext;
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

