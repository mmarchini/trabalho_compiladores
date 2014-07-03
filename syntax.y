%token KW_WORD
%token KW_BOOL
%token KW_BYTE
%token KW_IF
%token KW_THEN
%token KW_ELSE
%token KW_LOOP
%token KW_INPUT
%token KW_RETURN
%token KW_OUTPUT

%token OPERATOR_LE
%token OPERATOR_GE
%token OPERATOR_EQ
%token OPERATOR_NE
%token OPERATOR_AND
%token OPERATOR_OR

%token<symbol> TK_IDENTIFIER
%token<symbol> LIT_INTEGER
%token<symbol> LIT_FALSE
%token<symbol> LIT_TRUE	
%token<symbol> LIT_CHAR
%token<symbol> LIT_STRING

%token TOKEN_ERROR

%{
#include <stdio.h>
#include "hash.h"
#include "ast.h"

#define SET_TYPE_NATURE(hash, _type, _nature) hash->type=_type; hash->nature=_nature; 

extern FILE *outputFile;
ASTNode *ast;
%}


%union {
    HashTable *symbol;
    ASTNode   *ast;
}

%type<ast> expr;
%type<ast> call;
%type<ast> call_params;
%type<ast> params_list;
%type<ast> boolean;
%type<ast> attribution;
%type<ast> input;
%type<ast> output;
%type<ast> output_params;
%type<ast> return;
%type<ast> command;
%type<ast> command_block;
%type<ast> command_sequence;
%type<ast> if_block;
%type<ast> loop_block;
%type<ast> type;
%type<ast> integer;
%type<ast> literal;
%type<ast> global_var;
%type<ast> def_func;
%type<ast> header_func;
%type<ast> header_parameters;
%type<ast> array_initializer;
%type<ast> program;
%type<ast> root;

%nonassoc '='

%right '$'
%right '&'
%right '!'

%left '+' '-'
%left '*' '/'
%left '<' '>'
%left OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_NE OPERATOR_AND OPERATOR_OR

%start root

%%

root        : program { ast = $1; }
            ;

program     : global_var ';' program     { $$ = astCreate(AST_program, NULL, $1, $3, NULL, NULL); }
            | def_func   ';' program     { $$ = astCreate(AST_program, NULL, $1, $3, NULL, NULL); }
            |                            { $$ = NULL; }
            ;    

    /* Variáveis */

global_var : type TK_IDENTIFIER ':' literal         { $$ = astCreate(AST_var, $2, $1, $4, NULL, NULL); }
           | type TK_IDENTIFIER '[' integer ']' { $$ = astCreate(AST_array_var, $2, $1, $4, NULL, NULL); }
           | type '$' TK_IDENTIFIER ':' literal     { $$ = astCreate(AST_pt_var, $3, $1, $5, NULL, NULL); }
           | type TK_IDENTIFIER '[' integer ']' ':' array_initializer  { $$ = astCreate(AST_array_var, $2, $1, $4, $7, NULL); }
           ;

array_initializer: literal                   { $$ = astCreate(AST_array_init, NULL, $1, NULL, NULL, NULL); }
                 | literal array_initializer { $$ = astCreate(AST_array_init, NULL, $1, $2, NULL, NULL); }
                 ;


    /* Funçõs */

def_func: header_func command { $$ = astCreate(AST_def_func, NULL, $1, $2, NULL, NULL); }
        ;



header_func: type TK_IDENTIFIER '(' header_parameters ')'  { $$ = astCreate(AST_head_func, $2, $1, $4, NULL, NULL); }
           | type TK_IDENTIFIER '('                   ')'  { $$ = astCreate(AST_head_func, $2, $1, NULL, NULL, NULL);}
           ;

header_parameters: type TK_IDENTIFIER                       { $$ = astCreate(AST_head_param, $2, $1, NULL, NULL, NULL); }
                 | type TK_IDENTIFIER ',' header_parameters { $$ = astCreate(AST_head_param, $2, $1, $4, NULL, NULL); }
                 ;

    /* Comandos */

command_block : '{' command_sequence '}' {$$ = astCreate(AST_com_block, NULL, $2, NULL, NULL, NULL);}
              | '{'  '}'                 {$$ = astCreate(AST_com_block, NULL, NULL, NULL, NULL, NULL);}
              ;
    
command_sequence: command                  {$$ = astCreate(AST_com_seq, NULL, $1, NULL, NULL, NULL);}
                | command command_sequence {$$ = astCreate(AST_com_seq, NULL, $1, $2, NULL, NULL);}
                ;
                

command : command_block   {$$ = astCreate(AST_command, NULL, $1, NULL, NULL, NULL);}
        | attribution     {$$ = astCreate(AST_command, NULL, $1, NULL, NULL, NULL);}
        | input           {$$ = astCreate(AST_command, NULL, $1, NULL, NULL, NULL);}
        | output          {$$ = astCreate(AST_command, NULL, $1, NULL, NULL, NULL);}
        | return          {$$ = astCreate(AST_command, NULL, $1, NULL, NULL, NULL);}
        | if_block        {$$ = astCreate(AST_command, NULL, $1, NULL, NULL, NULL);}
        | loop_block      {$$ = astCreate(AST_command, NULL, $1, NULL, NULL, NULL);}
        ;

attribution: TK_IDENTIFIER '=' expr  {$$ = astCreate(AST_attr_ident, $1, $3, NULL, NULL, NULL);} // TODO Acho que falta ponteiros aqui
           | TK_IDENTIFIER '[' expr  ']' '=' expr {$$ = astCreate(AST_attr_array, $1, $3, $6, NULL, NULL);}
           ;

input: KW_INPUT TK_IDENTIFIER   {$$ = astCreate(AST_input, $2, NULL, NULL, NULL, NULL);}
     ;

output: KW_OUTPUT output_params { $$ = astCreate(AST_output, NULL, $2, NULL, NULL, NULL); }
      ;

output_params : LIT_STRING                   {$$ = astCreate(AST_output_param,   $1, NULL, NULL, NULL, NULL);}
              | LIT_STRING ',' output_params {$$ = astCreate(AST_output_param,   $1, NULL,   $3, NULL, NULL);}
              | expr                         {$$ = astCreate(AST_output_param, NULL,   $1, NULL, NULL, NULL);}
              | expr ',' output_params       {$$ = astCreate(AST_output_param, NULL,   $1,   $3, NULL, NULL);}
              ;

return : KW_RETURN expr {$$ = astCreate(AST_return, NULL, $2, NULL, NULL, NULL);}
       ;

call :  TK_IDENTIFIER '(' call_params ')' {$$ = astCreate(AST_call_ident,  $1,   $3,   NULL, NULL, NULL);}
     ;

call_params : params_list  {$$ = astCreate(AST_call_params,  NULL,   $1,   NULL, NULL, NULL);}
            |              {$$ = NULL;}
            ;

params_list : expr                 {$$ = astCreate(AST_expr,  NULL,   $1,   NULL, NULL, NULL);}
            | expr ',' params_list {$$ = astCreate(AST_expr,  NULL,   $1,   $3, NULL, NULL);}
            ;

expr : expr '>'          expr     {$$ = astCreate(AST_great,      NULL,   $1,   $3, NULL, NULL);} // 
     | expr '<'          expr     {$$ = astCreate(AST_less,       NULL,   $1,   $3, NULL, NULL);} //
     | expr OPERATOR_LE  expr     {$$ = astCreate(AST_le,         NULL,   $1,   $3, NULL, NULL);} //
     | expr OPERATOR_GE  expr     {$$ = astCreate(AST_ge,         NULL,   $1,   $3, NULL, NULL);} //
     | expr OPERATOR_EQ  expr     {$$ = astCreate(AST_eq,         NULL,   $1,   $3, NULL, NULL);} //
     | expr OPERATOR_NE  expr     {$$ = astCreate(AST_ne,         NULL,   $1,   $3, NULL, NULL);} //
     | expr OPERATOR_AND expr     {$$ = astCreate(AST_and,        NULL,   $1,   $3, NULL, NULL);} //
     | expr OPERATOR_OR  expr     {$$ = astCreate(AST_or,         NULL,   $1,   $3, NULL, NULL);} //
     | expr '+' expr              {$$ = astCreate(AST_add,        NULL,   $1,   $3, NULL, NULL);} //
     | expr '-' expr              {$$ = astCreate(AST_sub,        NULL,   $1,   $3, NULL, NULL);} //
     | expr '*' expr              {$$ = astCreate(AST_mult,       NULL,   $1,   $3, NULL, NULL);} //
     | expr '/' expr              {$$ = astCreate(AST_div,        NULL,   $1,   $3, NULL, NULL);} //
     | '&' TK_IDENTIFIER          {$$ = astCreate(AST_address,    $2,   NULL, NULL, NULL, NULL);} 
     | '$' TK_IDENTIFIER          {$$ = astCreate(AST_pointer,    $2,   NULL, NULL, NULL, NULL);} 
     | '!' expr                   {$$ = astCreate(AST_not,        NULL,   $2, NULL, NULL, NULL);} //
     | TK_IDENTIFIER              {$$ = astCreate(AST_identifier,   $1, NULL, NULL, NULL, NULL);} // 
     | LIT_CHAR                   {$$ = astCreate(AST_lit_char,     HashSetTypeNature($1, DT_BYTE, DN_SCALAR), NULL, NULL, NULL, NULL);} // 
     | LIT_INTEGER                {$$ = astCreate(AST_lit_int,      HashSetTypeNature($1, DT_WORD, DN_SCALAR), NULL, NULL, NULL, NULL);} //
     | boolean                    {$1->hashValue=HashSetTypeNature($1->hashValue, DT_BYTE, DN_SCALAR); $$ = $1;} //
     | call                       {$$ = $1;} //
     | TK_IDENTIFIER '[' expr ']' {$$ = astCreate(AST_vet_ident,  $1, $3, NULL, NULL, NULL);} //
     | '(' expr ')'               {$$ = astCreate(AST_par_block, NULL, 
                                    astCreate(AST_left_par,  NULL, NULL, NULL, NULL, NULL),
                                    astCreate(AST_expr,      NULL,   $2, NULL, NULL, NULL),
                                    astCreate(AST_right_par, NULL, NULL, NULL, NULL, NULL),
                                    NULL
                                    );
                                  } //
     ;

if_block : KW_IF '(' expr ')' KW_THEN command                 {$$ = astCreate(AST_if_block, NULL, $3, $6, NULL, NULL);}
         | KW_IF '(' expr ')' KW_ELSE command KW_THEN command {$$ = astCreate(AST_if_block, NULL, $3, $8, $6, NULL);}
         ;

loop_block : KW_LOOP  command '(' expr ')' { $$ = astCreate(AST_loop_block, NULL, $4, $2, NULL, NULL); }
           ;

    /* Aliases */

type : KW_WORD              { $$ = astCreate(AST_type_word, NULL, NULL, NULL, NULL, NULL); }
     | KW_BOOL              { $$ = astCreate(AST_type_bool, NULL, NULL, NULL, NULL, NULL); }
     | KW_BYTE              { $$ = astCreate(AST_type_byte, NULL, NULL, NULL, NULL, NULL); }
     ;


boolean : LIT_TRUE  {$$ = astCreate(AST_lit_true,  $1, NULL, NULL, NULL, NULL);}
        | LIT_FALSE {$$ = astCreate(AST_lit_false, $1, NULL, NULL, NULL, NULL);}
        ;

integer : LIT_INTEGER       { $$ = astCreate(AST_lit_int,  $1, NULL, NULL, NULL, NULL); }
        ;

literal : integer           { $$ = $1; }
        | boolean           { $$ = $1; }
        | LIT_CHAR          { $$ = astCreate(AST_lit_char,  $1, NULL, NULL, NULL, NULL);}
        | LIT_STRING        { $$ = astCreate(AST_lit_string,  $1, NULL, NULL, NULL, NULL);}
        ;

%%

