#include "semantic.h"
#include <stdlib.h>

int is_declared(HashTable *hash, char *var){
    return 0;
}

int check_scalar(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_vector(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_vector_index(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_function(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_parameters(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_return(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_pointer(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_expression(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_attribution(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_array_declaration(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_var_declaration(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_def_func(ASTNode *ast, HashTable *hash){
    return 0;
}

int check_semantic(ASTNode *ast, HashTable *hash){
    ASTNode *aux;
    int status = 0;
    aux=ast;
    while(aux!=NULL){
        if(aux->type != AST_program)
            return 8;

        switch(aux->children[0]->type){
            case AST_array_var:
                status = check_array_declaration(aux->children[0], hash) || status ? 4 : 0;
                break;
            case AST_pt_var:
            case AST_var:
                status = check_var_declaration(aux->children[0], hash) || status ? 4 : 0;
                break;
            case AST_def_func:
                status = check_def_func(aux->children[0], hash) || status ? 4 : 0;
                break;
            default:
                return 8;
        }
        aux = aux->children[1];
    }

    return status;
}

