#include "semantic.h"
#include "util.h"
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

DataType set_literal_type(ASTNode *ast){
    switch(ast->type){
        case AST_lit_true:
        case AST_lit_false:
            ast->hashValue->type = DT_BOOL;
            break;
        case AST_lit_char:
            ast->hashValue->type = DT_BYTE;
            break;
        case AST_lit_int:
            ast->hashValue->type = DT_WORD;
            break;
        case AST_lit_string:
            ast->hashValue->type = DT_STRING;
            break;
        default:
            ast->hashValue->type = DT_INVALID;
            error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid literal's type.");
            break;
    }

    return ast->hashValue->type;
}

DataType set_var_type(ASTNode *ast){
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
            ast->hashValue->type = DT_INVALID;
            error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid variable's type.");
            break;
    }

    return ast->hashValue->type;
}

DataType check_var_redeclaration(ASTNode *ast){

    if(ast->hashValue==NULL){
        ast->hashValue->type = DT_INVALID;
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Can't find declared variable in the declaration's table.");
        return ast->hashValue->type;
    }
    if(ast->children[0] == NULL){
        ast->hashValue->type = DT_INVALID;
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Can't determine declared variable's type.");
        return ast->hashValue->type;
    }
    if(ast->hashValue->type!= DT_NULL){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Redefinition of declared variable.");
        return ast->hashValue->type;
    }

    return set_var_type(ast);
}

bool compare_types(DataType type1, DataType type2){
    switch (type1) {
        case DT_WORD:
        case DT_BYTE:
            switch (type2){ 
                case DT_WORD:
                case DT_BYTE:
                    break;
                default:
                    return false;
            }
            break;
        default:
            if(type1 != type2){
                return false;
            }
            break;
    }
    return true;
}

int check_var_declaration(ASTNode *ast){
    DataType var_type, lit_type;

    var_type = check_var_redeclaration(ast);
    lit_type = set_literal_type(ast->children[1]);

    if(!compare_types(var_type, lit_type)){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Literal's type didn't match variable's type.");
    }

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

