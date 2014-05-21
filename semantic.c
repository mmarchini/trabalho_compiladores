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

int check_array_declaration(ASTNode *ast){
    DataType var_type, lit_type;
    bool is_equal = true;
    ASTNode *aux;

    var_type = check_var_redeclaration(ast);

    if(ast->children[1] == NULL)
        exit(10);
    if(ast->children[1]->hashValue == NULL)
        exit(11);
    ast->children[1]->hashValue->type = DT_WORD;

    // Null array, can return now
    if(ast->children[2] == NULL)
        return 0;

    for(aux=ast->children[2];aux!=NULL;aux=aux->children[1]){
        lit_type = set_literal_type(aux->children[0]);
        is_equal = is_equal && compare_types(var_type, lit_type);
    }

    if(!is_equal){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Literal's type didn't match variable's type.");
    }

    return 0;
}

void check_def_func(ASTNode *ast){
    DataType var_type;
    ASTNode *aux = NULL;

    check_var_redeclaration(ast);

    if(ast->children[1] == NULL)
        return;

    for(aux=ast->children[1];aux!=NULL;aux=aux->children[1]){
        check_var_redeclaration(aux);
    }
}

DataType check_var_type(ASTNode *ast){

    if(ast->hashValue==NULL){
        ast->hashValue->type = DT_INVALID;
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Can't find declared variable in the declaration's table.");
    }
    if(ast->hashValue->type==DT_NULL){
        ast->hashValue->type = DT_INVALID;
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Undeclared variable.");
    }


    return ast->hashValue->type;
}


DataType check_expression(ASTNode *ast) {
    DataType expr_type;

    switch(ast->type){
        case AST_and:
        case AST_or:
            if( !compare_types(check_expression(ast->children[1]), DT_BOOL)) 
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid right operator for bool expression.");
        case AST_not:
            if( !(compare_types(check_expression(ast->children[0]), DT_BOOL)) )
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid left operator for bool expression.");
            expr_type = DT_BOOL;
            break;

        case AST_great:
        case AST_ge:
        case AST_less:
        case AST_le:
            if( !(compare_types(check_expression(ast->children[0]), DT_WORD) && compare_types(check_expression(ast->children[1]), DT_WORD)) )
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid operators for comparation expression.");
        case AST_eq:
        case AST_ne:
            if( !compare_types(check_expression(ast->children[0]), check_expression(ast->children[1])) )
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid operators for comparation expression.");
        case AST_lit_true:
        case AST_lit_false:
            expr_type = DT_BOOL;
            break;
        case AST_add:
        case AST_sub:
        case AST_mult:
        case AST_div:
            if( !(compare_types(check_expression(ast->children[0]), DT_WORD) && compare_types(check_expression(ast->children[1]), DT_WORD)) )
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid operators for arithmetic expression.");
        case AST_lit_char:
        case AST_lit_int:
            expr_type = DT_WORD;
            break;
        case AST_identifier:
            expr_type = check_var_type(ast);
            break;
        case AST_vet_ident:
            expr_type = check_var_type(ast);
            if(!compare_types(check_expression(ast->children[0]), DT_WORD))
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid expression for array index.");
            break;
        case AST_par_block:
            expr_type = check_expression(ast->children[1]->children[0]);
            break;
        default:
            expr_type = DT_NULL;
            break;
    }

    return expr_type;
}

void check_if_block(ASTNode *ast){
    DataType expr_type;

    expr_type = check_expression(ast->children[0]);
    
    if(!compare_types(expr_type, DT_BOOL)){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid expression for if.");
    }
    
}

void check_loop_block(ASTNode *ast){
    DataType expr_type;

    expr_type = check_expression(ast->children[0]);
    
    if(!compare_types(expr_type, DT_BOOL)){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid expression for loop.");
    }
    
}

void check_attr_ident(ASTNode *ast){
    DataType expr_type, var_type;

    var_type = check_var_type(ast);

    expr_type = check_expression(ast->children[0]);
    
    if(!compare_types(expr_type, var_type)){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Left and right sides of attribution have different types.");
    }
}

void check_attr_array(ASTNode *ast){
    DataType expr_type, var_type;

    var_type = check_var_type(ast);

    expr_type = check_expression(ast->children[1]);
    
    if(!compare_types(expr_type, var_type)){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Left and right sides of attribution have different types.");
    }
    if(!compare_types(check_expression(ast->children[0]), DT_WORD)){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid expression for array index.");
    }

}

int check_semantic(ASTNode *ast){
    if(ast==NULL)
        return 0;
    switch(ast->type){
        case AST_var:
        case AST_pt_var:
            check_var_declaration(ast);
            break;
        case AST_array_var:
            check_array_declaration(ast);
            break;
        case AST_head_func:
            check_def_func(ast);
            break;
        case AST_if_block:
            check_if_block(ast);
            break;
        case AST_loop_block:
            //TODO: Tests
            check_loop_block(ast);
            break;
        case AST_input:
            //TODO
            break;
        case AST_output:
            //TODO
            break;
        case AST_attr_ident:
            //TODO: Tests
            check_attr_ident(ast);
            break;
        case AST_attr_array:
            check_attr_array(ast);
            //TODO
            break;
        default:
            break;
    }

    return 0;
}

