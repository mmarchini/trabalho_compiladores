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

DataType check_var_type(ASTNode *ast){

    switch(ast->type){
        case AST_vet_ident:
        case AST_attr_array:
            if(ast->hashValue->nature != DN_ARRAY)
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Trying to use something that is not an array as array.");
            break;
        case AST_identifier:
        case AST_attr_ident:
            if(ast->hashValue->nature != DN_SCALAR && ast->hashValue->nature != DN_POINTER)
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Trying to use something that is not an scalar as scalar.");
            break;
        case AST_call_ident:
            if(ast->hashValue->nature != DN_FUNCTION)
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Trying to use something that is not an function as function.");
            break;
        case AST_pointer:
            if(ast->hashValue->nature != DN_POINTER)
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Trying to use something that is not an pointer as pointer.");
            break;
        default:
            break;
    }

    if(ast->hashValue==NULL){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Can't find declared variable in the declaration's table.");
        return DT_INVALID; 
    }
    if(ast->hashValue->type==DT_NULL){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Undeclared variable.");
    }


    return ast->hashValue->type;
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
    // Identifiers Type 
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

    // Identifiers Nature 
    switch(ast->type){
        case AST_var:
        case AST_head_param:
            ast->hashValue->nature = DN_SCALAR;
            break;
        case AST_array_var:
            ast->hashValue->nature = DN_ARRAY;
            break;
        case AST_pt_var:
            ast->hashValue->nature = DN_POINTER;
            break;
        case AST_head_func:
            ast->hashValue->nature = DN_FUNCTION;
            break;
        default:
            ast->hashValue->nature = DN_INVALID;
            error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid variable's nature.");
            break;
    }

    return ast->hashValue->type;
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

DataType check_expression(ASTNode *ast); 

void check_function_arguments(ASTNode *ast){
    ASTNode *head_args=NULL, *params=NULL;
    if(ast->children[0] != NULL)
        params = ast->children[0]->children[0];
    head_args = (ASTNode *)ast->hashValue->args;

    while(head_args != NULL && params!=NULL){
        if( !compare_types( check_var_type(head_args) , check_expression(params->children[0])) )
            error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid argument's type.");
        params = params->children[1];
        head_args = head_args->children[1];
    }
    if(params == NULL && head_args != NULL)
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Function need's more arguments.");
    if(params != NULL && head_args == NULL)
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Function need's less arguments.");

}

DataType check_function_call(ASTNode *ast) {

    check_function_arguments(ast);

    return check_var_type(ast);
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
            if( compare_types(check_expression(ast->children[0]), DT_ADDRESS) || compare_types(check_expression(ast->children[1]), DT_ADDRESS) )
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Can't compare adresses.");
        case AST_lit_true:
        case AST_lit_false:
            expr_type = DT_BOOL;
            break;
        case AST_add:
            if( check_expression(ast->children[0]) == DT_ADDRESS ) {
                if ( !compare_types(check_expression(ast->children[1]), DT_WORD)){
                    error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid operators for pointer arithmetic expression.");
                    expr_type = DT_INVALID;
                } else {
                    expr_type = DT_ADDRESS;
                } 
            } else if(check_expression(ast->children[1]) == DT_ADDRESS){
                if ( !compare_types(check_expression(ast->children[0]), DT_WORD)){ 
                    error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid operators for pointer arithmetic expression.");
                    expr_type = DT_INVALID;
                } else {
                    expr_type = DT_ADDRESS;
                } 
            } else if( !(compare_types(check_expression(ast->children[0]), DT_WORD) && compare_types(check_expression(ast->children[1]), DT_WORD)) ){
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid operators for arithmetic expression.");
                    expr_type = DT_INVALID;
            } else{
                expr_type = DT_WORD;
            }
            break;
        case AST_sub:
        case AST_mult:
        case AST_div:
            if( !(compare_types(check_expression(ast->children[0]), DT_WORD) && compare_types(check_expression(ast->children[1]), DT_WORD)) )
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid operators for arithmetic expression.");
        case AST_lit_char:
        case AST_lit_int:
            expr_type = DT_WORD;
            break;
        case AST_address:
            expr_type = DT_ADDRESS;
            break;
        case AST_pointer:
            expr_type = check_var_type(ast);
            break;
        case AST_identifier:
            if(ast->hashValue->nature == DN_POINTER){
                expr_type = DT_ADDRESS;
            }
            else{
                expr_type = check_var_type(ast);
            }
            break;
        case AST_vet_ident:
            expr_type = check_expression(ast->children[0]);
            if(!compare_types(expr_type, DT_WORD))
                error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Invalid expression for array index.");
            expr_type = check_var_type(ast);
            break;
        case AST_par_block:
            expr_type = check_expression(ast->children[1]->children[0]);
            break;
        case AST_call_ident:
            expr_type = check_function_call(ast);
            break;
        default:
            expr_type = DT_NULL;
            break;
    }

    return expr_type;
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

DataType check_head_func(ASTNode *ast){
    DataType return_type;
    ASTNode *aux = NULL;

    return_type = check_var_redeclaration(ast);

    // Check parameters
    
    if(ast->children[1] != NULL){
        ast->hashValue->args = (void *)ast->children[1];
        for(aux=ast->children[1];aux!=NULL;aux=aux->children[1]){
            check_var_redeclaration(aux);
        }
    }
    return return_type; 
}

void check_return(ASTNode *ast, DataType return_type){
    while(ast!=NULL){
        switch(ast->type){
            case AST_command:
                ast = ast->children[0];
                break;
            case AST_com_block:
                ast = ast->children[0];
                break;
            case AST_com_seq:
                check_return(ast->children[0], return_type);
                ast = ast->children[1];
                break;
            case AST_if_block:
                if(ast->children[2])
                    check_return(ast->children[2], return_type);
            case AST_loop_block:
                check_return(ast->children[1], return_type);
                ast = NULL;
                break;
            case AST_return:
                if(!compare_types(check_expression(ast->children[0]), return_type))
                    error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Return type didn't match function type.");
                ast=NULL;
                break;
            default:
                ast=NULL;
                break;

        }
    }

}

void check_def_func(ASTNode *ast){
    DataType return_type;
    ASTNode *aux;

    return_type = check_head_func(ast->children[0]);

    check_return(ast->children[1], return_type);
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

    if(ast->hashValue->nature == DN_POINTER){
        var_type = DT_ADDRESS;
    } else{
        var_type = check_var_type(ast);
    }

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

void check_input(ASTNode *ast){
    if(compare_types(check_var_type(ast), DT_BOOL)){
        error_queue=SemanticErrorInsert(error_queue, getLineNumber(), "Can't use bool variables in input.");
    }
}

int check_output(ASTNode *ast){
    DataType var_type, lit_type;
    ASTNode *aux;

    for(aux=ast->children[0];aux!=NULL;aux=aux->children[1]){
            if(aux->children[0] == NULL){
            }
            else
                check_expression(aux->children[0]);
    }

    return 0;
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
        case AST_def_func:
            check_def_func(ast);
            break;
        case AST_if_block:
            check_if_block(ast);
            break;
        case AST_loop_block:
            check_loop_block(ast);
            break;
        case AST_input:
            check_input(ast);
            break;
        case AST_output:
            check_output(ast);
            break;
        case AST_attr_ident:
            check_attr_ident(ast);
            break;
        case AST_attr_array:
            check_attr_array(ast);
            break;
        default:
            break;
    }

    return 0;
}

