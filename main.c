#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "hash.h"
#include "util.h"

extern HashTable *hashTable;
extern ASTNode *ast;
extern int running;
extern FILE *yyin;

FILE *outputFile;

int yyparse();
 
char tkstr[35][100] = {};

int main(int argn, char *args[]){
    int lexReturn;

    if(argn > 1)
        yyin = fopen(args[1], "r");
    if(argn > 2){
        outputFile = fopen(args[2], "w+");
    }
    else
        outputFile = NULL;


    initMe();

    yyparse();

    if(outputFile) 
        fprintf(outputFile, "%s", astPrintFile(ast)); 
    else 
        astPrint(ast, 0); 

    return check_semantic(ast, hashTable);
}

