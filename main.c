#include <stdio.h>
#include "hash.h"

HashTable *hashTable;
int running;
int nline;
int commentControl;

int yywrap(){
    return 1;
}

void initMe(){
    running = 1;
    nline=0;
    commentControl=0;
    hashTable = hashInit();
}

int getLineNumber(){ return nline; }


int main(){
    int lexReturn;
    initMe();
    while(running){
        lexReturn=yylex();
        if(running)
            printf("%d\n", lexReturn);
    }
    printf("\nlinhas: %d\n", getLineNumber());
    hashPrint(hashTable);
    return 0;
}

