#ifndef _AST
#define _AST

#include "hash.h"

#define CHILDREN 4

// Operadores binarios
#define AST_great 1
#define AST_less  2
#define AST_ge    3
#define AST_le    4
#define AST_eq    5
#define AST_ne    6
#define AST_and   7
#define AST_or    8
#define AST_add   9
#define AST_sub  10
#define AST_mult 11
#define AST_div  12

// Operadores Unarios
#define AST_address 30 
#define AST_pointer 31 
#define AST_not     32 

// Parenteses 
#define AST_par_block 40 
#define AST_left_par  41 
#define AST_right_par 42 

//
#define AST_identifier  50 
#define AST_vet_ident   51 
#define AST_call_ident  53
#define AST_call_params 54
#define AST_expr        59

//Literals
#define AST_lit_char   60
#define AST_lit_int    61
#define AST_lit_true   62
#define AST_lit_false  63
#define AST_lit_string 64

//Attribution
#define AST_attr_ident 70
#define AST_attr_array 71

//
#define AST_input        80
#define AST_output       81
#define AST_output_param 82
#define AST_return       83
#define AST_command      84
#define AST_com_block    85
#define AST_com_seq      86

//
#define AST_if_block    90
#define AST_loop_block  91

//
#define AST_type_word       100
#define AST_type_bool       101
#define AST_type_byte       102
#define AST_var             103
#define AST_pt_var          104
#define AST_array_var       105
#define AST_array_init      106
#define AST_def_func        107
#define AST_head_func        108
#define AST_head_param        109

//
#define AST_program 200
#define AST_root    300



typedef struct _astNode{
    int type;
    HashTable *hashValue;
    int currentLine;
    struct _astNode *children[CHILDREN];

} ASTNode;

ASTNode *astCreate(int type, HashTable *hash, 
                   ASTNode *ast0, ASTNode *ast1, 
                   ASTNode *ast2, ASTNode *ast3);

void astPrint(ASTNode *root, int level);

char *astPrintFile(ASTNode *root);

#endif
