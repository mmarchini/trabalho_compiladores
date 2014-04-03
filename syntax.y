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

%%


program     : global_var ';' program     {}
            | def_func   ';' program     {}
            |
            ;    

    /* Variáveis */

global_var : type identifier ':' literal         { }
           | type identifier '[' LIT_INTEGER ']' { }
           | type identifier '[' LIT_INTEGER ']' ':' array_initializer  { }
           | type '$' identifier ':' literal     { }
           ;

array_initializer: literal array_initializer {}
                 |
                 ;


    /* Funçõs */

def_func: header_func command {}
        ;



header_func: type identifier '(' header_parameters ')'  {  }
           | type identifier '('                   ')'  {  }
           ;

header_parameters: type identifier {}
                 | type identifier ',' header_parameters {}
                 ;

    /* Comandos */

command_block : '{' command_sequence '}' {}
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
        |
        ;

attribution: identifier '=' expr {}
           | identifier '[' expr  ']' '=' expr {}
           ;

input: KW_INPUT identifier {}
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

operator : boolean_operator      {}
         | arithmetic_operator {}
         ;

boolean_operator : '>'           {} 
                 | '<'           {} 
                 | '!'           {} 
                 | OPERATOR_LE   {} 
                 | OPERATOR_GE   {} 
                 | OPERATOR_EQ   {} 
                 | OPERATOR_NE   {} 
                 | OPERATOR_AND  {} 
                 | OPERATOR_OR   {} 
                 ;

arithmetic_operator  : '+'    {}
                     | '-'  {}
                     | '*'  {}
                     | '/'  {}
                     ;

call_params : expr {}
            | expr ',' call_params {}
            |
            ;

unary_operator : '&' {}
               | '$' {}
               ;


expr : expr operator expr             {}
     | unary_operator expr            {}
     | '(' expr ')'                   {}
     | identifier                     {}
     | identifier '(' call_params ')' {}
     | identifier '[' expr ']'        {}
     | LIT_INTEGER                    {}
     | boolean                        {}
     | LIT_CHAR                       {}
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


identifier :  TK_IDENTIFIER { }
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

