#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#define SEMANTIC_ERROR_EXIT 4

#include "ast.h"
#include "hash.h"

typedef struct _SemanticError {
    int line;
    char error[100];
    struct _SemanticError *next;
} SemanticError;

void SemanticErrorPrint(SemanticError *queue);

SemanticError *SemanticErrorInsert(SemanticError *queue, int line, char error[100]);

int check_semantic(ASTNode *ast);

#endif
