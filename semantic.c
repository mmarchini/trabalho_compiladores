#include "semantic.h"
#include "util.h"
#include "ast.h"
#include <stdlib.h>

// I hate globals, but this will give me some extra time
SemanticError *error_queue;

//////////////////
// Semantic Errors
    void SemanticErrorPrint(SemanticError *queue){
        SemanticError *aux;
        for(aux=queue;aux!=NULL;aux=aux->next){
            printf("Line %d: %s\n", aux->line, aux->error);
        }
    }
//
    SemanticError *SemanticErrorInsert(SemanticError *queue, int line, char error[100]){
        SemanticError *aux;

        SemanticError *new = calloc(1, sizeof(SemanticError));
        new->line = line;
        strcpy(new->error, error);
        new->next = NULL;

        if(queue == NULL)
            return new;

        for(aux=queue;aux->next!=NULL;aux=aux->next);
        aux->next = new;
        return queue;
    }
// Semantic Errors
//////////////////

int is_declared(HashTable *hash, char *var){
    return 0;
}

int check_scalar(ASTNode *ast){
    return 0;
}

int check_vector(ASTNode *ast){
    return 0;
}

int check_vector_index(ASTNode *ast){
    return 0;
}

int check_function(ASTNode *ast){
    return 0;
}

int check_parameters(ASTNode *ast){
    return 0;
}

int check_return(ASTNode *ast){
    return 0;
}

int check_pointer(ASTNode *ast){
    return 0;
}

int check_expression(ASTNode *ast){
    return 0;
}

int check_attribution(ASTNode *ast){
    return 0;
}

int check_array_declaration(ASTNode *ast){
    return 0;
}

void check_var_redeclaration(ASTNode *ast){

    if(ast->hashValue==NULL){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Can't find declared variable in the declaration's table.");
        return ;
    }
    if(ast->children[0] == NULL){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Can't determine declared variable's type.");
        return ;
    }
    if(ast->hashValue->type!= DT_NULL){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Redefinition of declared variable.");
        return ;
    }


    switch(ast->children[0]->type){
        case AST_type_byte:
            ast->hashValue->type = DT_BYTE;
            break;
        case AST_type_word:
            ast->hashValue->type = DT_WORD;
            break;
        case AST_type_bool:
            ast->hashValue->type = DT_BOOL;
            break;
        default:
            error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid variable's type.");
            break;
    }
}

int check_var_declaration(ASTNode *ast){
    check_var_redeclaration(ast);
    return 0;
}

int check_def_func(ASTNode *ast){
    return 0;
}

int check_semantic(ASTNode *ast){
    if(ast==NULL)
        return 0;
    switch(ast->type){
        case AST_var:
            check_var_declaration(ast);
            break;
        default:
            break;
    }

    return 0;
}

