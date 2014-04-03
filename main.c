#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "util.h"

extern HashTable *hashTable;
extern int running;
extern FILE *yyin;

int yyparse();
 
char tkstr[35][100] = {};

int main(int argn, char *args[]){
    int lexReturn;

    if(argn > 1)
        yyin = fopen(args[1], "r");

    initMe();

    yyparse();

    return 0;
}

