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

%union {
    char *symbol;
    int integer;
}

%{
#include <stdio.h>
%}

%start program

%%


program     : global_var ';' program     {printf("Var: '%s'\n", $$);}
            | def_func   ';' program     {}
            ;    

    /* Variáveis */

global_var : type identifier ':' literal         { printf("a: '%s' '%s' '%s' '%s' \n", $$, $1, $2, $4);}
           | type identifier '[' LIT_INTEGER ']' { printf("b: '%s'\n", $$);}
           | type identifier '[' LIT_INTEGER ']' ':' array_initializer  { printf("c:'%s'\n", $$);}
           | type '$' identifier ':' literal     { printf("d: '%s'\n", $$);}
           ;

array_initializer: literal array_initializer {}
                 |
                 ;


    /* Funçõs */

def_func: header_func command {printf("DEF FUNC\n");}
        ;



header_func: type identifier '(' header_parameters ')'  { printf("header:'%s' '%s'  '%s' '%s' \n", $$, $1, $2, $3); }
           ;

header_parameters: type identifier {}
                 | type identifier ',' header_parameters {}
                 |
                 ;

    /* Comandos */

command_block : '{' command_sequence '}' {printf("command_block\n");}
              | '{'  '}'                 {printf("empty block\n");}
              ;
    
command_sequence: command {printf("command_sequence1\n");}
                | command command_sequence {printf("command_sequence2\n");}
                ;
                

command : command_block   {}
        | attribution     {}
        | input           {}
        | output          {printf("command\n");}
        | return          {}
        | if_block        {}
        | loop_block      {}
        ;

attribution: identifier '=' expr {}
           | identifier '[' expr  ']' '=' expr {}
           ;

input: KW_INPUT identifier {}
     ;

output: KW_OUTPUT output_params { printf("OUTPUT\n"); }
      ;

output_params : LIT_STRING {}
              | LIT_STRING ',' output_params {printf("string");}
              | expr {}
              | expr ',' output_params     {printf("expr");}
              |
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


expr : expr operator expr             {}
     | '(' expr ')'                   {}
     | identifier '(' call_params ')' {}
     | identifier                     {}
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

type : KW_WORD              {$$.symbol = "word";}
     | KW_BOOL              {$$.symbol = "bool";}
     | KW_BYTE              {$$.symbol = "byte";}
     ;


identifier :  TK_IDENTIFIER { printf("Ident: '%s' '%s'\n", $$, $1);}
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

