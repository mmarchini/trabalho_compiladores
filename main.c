#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "util.h"

extern HashTable *hashTable;
extern int running;

int yyparse();
 
char tkstr[35][100] = {};

int main(){
    int lexReturn;
    initMe();

    yyparse();

    return 0;
}

