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

%token TK_IDENTIFIER
%token LIT_INTEGER
%token LIT_FALSE
%token LIT_TRUE	
%token LIT_CHAR
%token LIT_STRING

%token TOKEN_ERROR

%{
#include <stdio.h>
#include "hash.h"
%}


%union {
    HashTable *symbol;
}

%nonassoc '='

%right '$'
%right '&'
%right '!'

%left '+' '-'
%left '*' '/'
%left '<' '>'
%left OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_NE OPERATOR_AND OPERATOR_OR

%%


program     : global_var ';' program     {}
            | def_func   ';' program     {}
            |
            ;    

    /* Variáveis */

global_var : type TK_IDENTIFIER ':' literal         { }
           | type TK_IDENTIFIER '[' LIT_INTEGER ']' { }
           | type TK_IDENTIFIER '[' LIT_INTEGER ']' ':' array_initializer  { }
           | type '$' TK_IDENTIFIER ':' literal     { }
           ;

array_initializer: literal array_initializer {}
                 |
                 ;


    /* Funçõs */

def_func: header_func command {}
        ;



header_func: type TK_IDENTIFIER '(' header_parameters ')'  {  }
           | type TK_IDENTIFIER '('                   ')'  {  }
           ;

header_parameters: type TK_IDENTIFIER {}
                 | type TK_IDENTIFIER ',' header_parameters {}
                 ;

    /* Comandos */

command_block : '{' command_sequence '}' {}
              | '{'  '}' {}
              ;
    
command_sequence: command {}
                | command command_sequence {}
                ;
                

command : command_block   {}
        | attribution     {}
        | input           {}
        | output          {}
        | return          {}
        | if_block        {}
        | loop_block      {}
        ;

attribution: TK_IDENTIFIER '=' expr {}
           | TK_IDENTIFIER '[' expr  ']' '=' expr {}
           ;

input: KW_INPUT TK_IDENTIFIER {}
     ;

output: KW_OUTPUT output_params {  }
      ;

output_params : LIT_STRING {}
              | LIT_STRING ',' output_params {}
              | expr {}
              | expr ',' output_params     {}
              ;

return : KW_RETURN expr {}
       ;

call :  TK_IDENTIFIER '(' call_params ')' {}
     ;

call_params : params_list  {}
            |
            ;

params_list : expr {}
            | expr ',' params_list {}
            ;

expr : expr '>'          expr    {} 
     | expr '<'          expr    {}
     | expr OPERATOR_LE  expr    {}
     | expr OPERATOR_GE  expr    {}
     | expr OPERATOR_EQ  expr    {}
     | expr OPERATOR_NE  expr    {}
     | expr OPERATOR_AND expr    {} 
     | expr OPERATOR_OR  expr    {}  
     | expr '+' expr             {}  
     | expr '-' expr             {}
     | expr '*' expr             {}
     | expr '/' expr             {}
     | expr '&' expr             {} 
     | expr '$' expr             {} 
     | expr '!' expr             {}
     | TK_IDENTIFIER                {}
     | call                      {}
     | TK_IDENTIFIER '[' expr ']'   {}
     | LIT_CHAR                  {}
     ;

if_block : KW_IF '(' expr ')' KW_THEN command {}
         | KW_IF '(' expr ')' KW_ELSE command KW_THEN command {}
         ;

loop_block : KW_LOOP  command '(' expr ')' {}
           ;

    /* Aliases */

type : KW_WORD              {}
     | KW_BOOL              {}
     | KW_BYTE              {}
     ;


boolean : LIT_FALSE         { }
        | LIT_TRUE          { }
        ;


literal : LIT_INTEGER       { }
        | boolean           { }
        | LIT_CHAR          { }
        | LIT_STRING        { }
        ;

%%

