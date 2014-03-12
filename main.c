#include <stdio.h>
#include "hash.h"
#include "util.h"

extern HashTable *hashTable;
extern int running; //TODO Perguntar para o professor

int yylex();

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

