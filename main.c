#include <stdio.h>
#include "hash.h"
#include "util.h"

HashTable *hashTable;
int running; //TODO Perguntar para o professor

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

