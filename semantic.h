#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include "ast.h"
#include "hash.h"

int check_semantic(ASTNode *ast, HashTable *hash);

#endif
