# include "common.h"
# include <string.h>

ast* nAst(char* name,int lineno,int type,char* cont){
    ast* node = (ast*) malloc(sizeof(ast));
    node->name = (char *)malloc(strlen(name)+1);
    strcpy(node->name,name);
    if(cont!=NULL){
        node->context = (char*)malloc(strlen(cont)+1);
        strcpy(node->context,cont);
    } else {
        node->context = NULL;
    }
    node->lineno = lineno;
    node->outType = type;
    node->parent = node->child = node->sib = NULL;
    return node;
}

void addChild(ast* p, ast* c){
    if(p->child==NULL){p->child=c;c->parent=p;return;}
    ast* tmp = p->child;
    while(tmp->sib!=NULL){tmp=tmp->sib;}
    tmp->sib = c;
    c->parent = p;
}

void printTree(ast* root, int index){
    if(root==NULL){return;}
    //printf("%d",index);
    if(root->outType==nonTerm_ && root->child==NULL){
        return;
    }
    for(int i=0;i<index;i++){
        printf("  ");
    }
    switch (root->outType)
    {
    case syn_:
        printf("%s (%d)\n",root->name,root->lineno);
        break;
    case nonTerm_:
        printf("%s (%d)\n",root->name,root->lineno);
        break;
    case lex_:
        printf("%s\n",root->name);
        break;
    case id_:
        printf("%s: %s\n",root->name,root->context);
        break;
    case type_:
        printf("%s: %s\n",root->name,root->context);
        break;
    case int_:
        printf("%s: %u\n",root->name,root->val.intVal);
        break;
    case float_:
        printf("%s: %f\n",root->name,root->val.floatVal);
        break;
    default:
        break;
    }
    ast* ch = root->child;
    while(ch!=NULL){
        printTree(ch,index+1);
        ch = ch->sib;
    }
}