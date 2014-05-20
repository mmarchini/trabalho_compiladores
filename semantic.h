#ifndef _SEMANTIC_H
#define _SEMANTIC_H

#include "ast.h"
#include "hash.h"

int is_declared(HashTable hash, char *var);

int check_scalar();

int check_vector();

int check_vector_index();

int check_function();

int check_parameters();

int check_return();

int check_pointer();

int check_expression();

int check_attribution();

int check_syntax(ASTNode *ast, HashTable hash);

#endif
