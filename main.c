#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "hash.h"
#include "util.h"
#include "queue.h"

extern ASTNode *ast;
extern int running;
extern FILE *yyin;
extern SemanticError *error_queue;
extern HashTable *hashTable;

FILE *outputFile;

int yyparse();
 
char tkstr[35][100] = {};

int main(int argn, char *args[]){
    int lexReturn;
    TACQueue *queue;

    if(argn > 1)
        yyin = fopen(args[1], "r");
    if(argn > 2){
        outputFile = fopen(args[2], "w+");
    }
    else
        outputFile = NULL;


    initMe();

    yyparse();
    
    check_semantic(ast);

    if(error_queue != NULL){
        SemanticErrorPrint(error_queue);
        return SEMANTIC_ERROR_EXIT;
    }

    queue = TACProgram(ast, hashTable);
    TACQueuePrint(queue);

    return 0;
}

