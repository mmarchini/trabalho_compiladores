#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"

ASTNode *astCreate(int type, HashTable *hash, 
                   ASTNode *ast0, ASTNode *ast1, 
                   ASTNode *ast2, ASTNode *ast3){
    ASTNode *ast = NULL;

    ast = calloc(1, sizeof(ASTNode));
    ast->type = type;
    ast->hashValue = hash;
    ast->children[0] = ast0;
    ast->children[1] = ast1;
    ast->children[2] = ast2;
    ast->children[3] = ast3;

    check_semantic(ast);

    return ast;
}

char *astCodeToString(int type){
    switch(type){
        // Operadores binarios
        case AST_great :
            return ">";
        case AST_less  :
            return "<";
        case AST_ge    :
            return ">=";
        case AST_le    :
            return "<=";
        case AST_eq    :
            return "==";
        case AST_ne    :
            return "!=";
        case AST_and   :
            return "&&";
        case AST_or    :
            return "||";
        case AST_add   :
            return "+";
        case AST_sub   :
            return "-";
        case AST_mult  :
            return "*";
        case AST_div   :
            return "/";

        // Operadores Unarios
        case AST_address : 
            return "&";
        case AST_pointer : 
            return "$";
        case AST_not     : 
            return "!";

        // Parenteses 
        case AST_par_block : 
            return "\\";
        case AST_left_par  : 
            return "(";
        case AST_right_par : 
            return ")";

        //
        case AST_identifier : 
            return "ident";
        case AST_vet_ident   : 
            return "vet_ident";
        case AST_call_ident  :
            return "call_ident";
        case AST_call_params :
            return "call_params";
        case AST_expr       :
            return "expr";

        //Literals
        case AST_lit_char  :
            return "char";
        case AST_lit_int   :
            return "int";
        case AST_lit_true  :
            return "true";
        case AST_lit_false :
            return "false";
        case AST_lit_string :
            return "string";
            //Attribution
        case AST_attr_ident :
            return "=";
        case AST_attr_array :
            return "[=]";

        //
        case AST_input  :
            return "input";
        case AST_output :
            return "output";
        case AST_return :
            return "return";
        case AST_command   :
            return "command";
        case AST_com_block :
            return "com_block";
        case AST_com_seq   :
            return "com_seq";

        //
        case AST_if_block   :
            return "if_block";
        case AST_loop_block :
            return "loop_block";

        //
        case AST_type_word  :
            return "word";
        case AST_type_bool  :
            return "bool";
        case AST_type_byte  :
            return "byte";
        case AST_var        :
            return "var";
        case AST_pt_var     :
            return "pt_var";
        case AST_array_var  :
            return "array_var";
        case AST_array_init :
            return "array_init";
        case AST_def_func   :
            return "def_func";
        case AST_head_func  :
            return "head_func";
        case AST_head_param :
            return "head_param";

        //
        case  AST_program    :
            return "program";
        case  AST_root       :
            return "root";



        default:
            return "";
    }
}

void astPrint(ASTNode *root, int level){
    if(root){
        int i=0;
        char *hashValue = "";
        if(i<level){
            printf(" ");
            i++;
        }
        for(;i<level;i++){
            printf("|");
        }
        if(level){
            printf("└->");
        }

        if(root->hashValue)
            hashValue = root->hashValue->value;

        printf("TYPE %s VALUE %s\n", astCodeToString(root->type), hashValue);
        for(i=0;i<4;i++)
            if(root->children[i]){
                astPrint(root->children[i], level+1);
            }
    }
}

char *astPrintFile(ASTNode *root){
    char *auxStr = calloc(1000, sizeof(char));
    if(root){
        switch(root->type){
            // Operadores binarios
            case AST_great :
                sprintf(auxStr, "%s > %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_less  :
                sprintf(auxStr, "%s < %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_ge    :
                sprintf(auxStr, "%s >= %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_le    :
                sprintf(auxStr, "%s <= %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_eq    :
                sprintf(auxStr, "%s == %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_ne    :
                sprintf(auxStr, "%s != %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_and   :
                sprintf(auxStr, "%s && %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_or    :
                sprintf(auxStr, "%s || %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_add   :
                sprintf(auxStr, "%s + %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_sub   :
                sprintf(auxStr, "%s - %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_mult  :
                sprintf(auxStr, "%s * %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_div   :
                sprintf(auxStr, "%s / %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;

            // Operadores Unarios
            case AST_address : 
                sprintf(auxStr, "& %s", astPrintFile(root->children[0]));
                return auxStr;
            case AST_pointer : 
                sprintf(auxStr, "$ %s", astPrintFile(root->children[0]));
                return auxStr;
            case AST_not     : 
                sprintf(auxStr, "! %s", astPrintFile(root->children[0]));
                return auxStr;

            // Parenteses 
            case AST_par_block : 
                sprintf(auxStr, "( %s )", astPrintFile(root->children[1]));
                return auxStr;
            case AST_left_par  : 
                break;
            case AST_right_par : 
                break;

            //
            case AST_identifier : 
                return root->hashValue->value;
            case AST_vet_ident   : 
                sprintf(auxStr, "%s [%s]", root->hashValue->value, astPrintFile(root->children[0]));
                return auxStr;
            case AST_call_ident  :
                if(root->children[0]){
                    sprintf(auxStr, "%s ( %s )", root->hashValue->value, astPrintFile(root->children[0]));
                }
                else{
                    sprintf(auxStr, "%s ()", root->hashValue->value);
                }
                return auxStr ;
            case AST_call_params :
                sprintf(auxStr, "%s", astPrintFile(root->children[0]));
                return auxStr;
            case AST_expr       :
                if(root->children[1]){
                    sprintf(auxStr, "%s,  %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                }
                else{
                    sprintf(auxStr, "%s", astPrintFile(root->children[0]));
                }

                return auxStr;

            //Literals
            case AST_lit_char  :
                sprintf(auxStr, "'%s'", root->hashValue->value);
                return auxStr;
            case AST_lit_int   :
                sprintf(auxStr, "%s", root->hashValue->value);
                return auxStr;
            case AST_lit_true  :
                return "true";
            case AST_lit_false :
                return "false";
            case AST_lit_string :
                sprintf(auxStr, "\"%s\"", root->hashValue->value);
                return auxStr;
            //Attribution
            case AST_attr_ident :
                sprintf(auxStr, "%s = %s", root->hashValue->value, astPrintFile(root->children[0]));
                return auxStr;
            case AST_attr_array :
                sprintf(auxStr, "%s[%s] = %s", root->hashValue->value, astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;

            //
            case AST_input  :
                sprintf(auxStr, "input %s", root->hashValue->value);
                return auxStr;
            case AST_output :
                sprintf(auxStr, "output %s", astPrintFile(root->children[0]));
                return auxStr;
            case AST_output_param :
                if(root->children[0]){
                    if(root->children[1])
                        sprintf(auxStr, "%s,  %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                    else
                        sprintf(auxStr, "%s", astPrintFile(root->children[0]));
                }
                else {
                    if(root->children[1]){
                        sprintf(auxStr, "\"%s\",  %s", root->hashValue->value, astPrintFile(root->children[1]));
                    }
                    else{
                        sprintf(auxStr, "\"%s\"", root->hashValue->value);
                    }

                }

                return auxStr;

            case AST_return :
                sprintf(auxStr, "return %s", astPrintFile(root->children[0]));
                return auxStr;
            case AST_command:
                sprintf(auxStr, "%s\n", astPrintFile(root->children[0]));
                return auxStr;
            case AST_com_block :
                if(root->children[0]){
                    sprintf(auxStr, "{ %s }", astPrintFile(root->children[0]));
                }
                else{
                    return "{  }";
                }
                return auxStr;
            case AST_com_seq   :
                if(root->children[1]){
                    sprintf(auxStr, "%s %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                }
                else{
                    sprintf(auxStr, "%s", astPrintFile(root->children[0]));
                }

                return auxStr;
            //
            case AST_if_block   :
                if(root->children[2])
                    sprintf(auxStr, "if ( %s ) else %s then %s", astPrintFile(root->children[0]), astPrintFile(root->children[2]), astPrintFile(root->children[1]));
                else
                    sprintf(auxStr, "if ( %s ) then %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));

                return auxStr;
            case AST_loop_block :
                sprintf(auxStr, "loop %s ( %s )", astPrintFile(root->children[1]), astPrintFile(root->children[0]));
                return auxStr;

            //
            case AST_type_word  :
                return "word";
            case AST_type_bool  :
                return "bool";
            case AST_type_byte  :
                return "byte";

            case AST_var    :
                sprintf(auxStr, "%s %s : %s", astPrintFile(root->children[0]), root->hashValue->value, astPrintFile(root->children[1]));
                return auxStr;
            case AST_pt_var     :
                sprintf(auxStr, "%s $ %s : %s", astPrintFile(root->children[0]), root->hashValue->value, astPrintFile(root->children[1]));
                return auxStr;
            case AST_array_var  :
                if(root->children[2]){
                    sprintf(auxStr, "%s %s [ %s ] : %s", astPrintFile(root->children[0]), root->hashValue->value, astPrintFile(root->children[1]), astPrintFile(root->children[2]));
                }
                else{
                    sprintf(auxStr, "%s %s [ %s ]", astPrintFile(root->children[0]), root->hashValue->value, astPrintFile(root->children[1]));
                }
                return auxStr;
            case AST_array_init :
                if(root->children[1]){
                    sprintf(auxStr, "%s %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                }
                else{
                    sprintf(auxStr, "%s", astPrintFile(root->children[0]));
                }

                return auxStr;

            case AST_def_func   :
                sprintf(auxStr, "%s %s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case AST_head_func  :
                if(root->children[1]){
                    sprintf(auxStr, "%s %s ( %s )", astPrintFile(root->children[0]), root->hashValue->value, astPrintFile(root->children[1]));
                }
                else{
                    sprintf(auxStr, "%s %s (  )", astPrintFile(root->children[0]), root->hashValue->value);
                }
                return auxStr;
            case AST_head_param :
                if(root->children[1]){
                    sprintf(auxStr, "%s %s,  %s", astPrintFile(root->children[0]), root->hashValue->value, astPrintFile(root->children[1]));
                }
                else{
                    sprintf(auxStr, "%s %s", astPrintFile(root->children[0]), root->hashValue->value);
                }
                return auxStr;

            //
            case  AST_program    :
                sprintf(auxStr, "%s;\n%s", astPrintFile(root->children[0]), astPrintFile(root->children[1]));
                return auxStr;
            case  AST_root       :
                return "";

            default:
                return "";
        }
    }
    else
        return "";
    
}

