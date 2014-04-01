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

%%

program:
       global_var  {}
       global_func {}
       |
       ;



type:
     KW_WORD     { $$ = "word"; }
     | KW_BOOL   { $$ = "bool"; }
     | KW_BYTE   { $$ = "byte"; }
     |
     ;

identifier:
          TK_IDENTIFIER {printf("Identifier: %s\n", $1);$$ = $1;  }
          |
          ;




boolean:
       LIT_FALSE   {}
       | LIT_TRUE    {}
       |
       ;


literal:
       LIT_INTEGER     {printf("Literal: %s\n", $1); $$ = $1;}
       | boolean       {printf("Literal: %s\n", $1); $$ = $1;}
       | LIT_CHAR      {printf("Literal: %s\n", $1); $$ = $1;}
       | LIT_STRING    {printf("Literal: %s\n", $1); $$ = $1;}
       |
       ;





global_var:
         type identifier ':' literal ';' { }
         |
         ;





def_func:
        header_func var_func command_block {}
        |
        ;

header_func:
           type identifier '(' header_parameters ')'  {}
           |
           ;

header_parameters:
                 type identifier {}
                 | header_parameters ',' header_parameter {}
                 |
                 ;

var_func:
        global_var {}
        var_func global_var {}
        |
        ;

command_block:
             '{' command_sequence '}' {}
             |
             ;

command_sequence:
                

commands:
        command_block {}
        | attribution   {}
        | input         {}
        | output        {}
        | return        {}
        | if_block      {}
        | loop_block    {}
        |
        ;

attribution:
           identifier '=' expr {}
           |
           ;

input:
     KW_INPUT identifier {}
     |
     ;

output:
     KW_OUTPUT identifier {}
     |
     ;

return:
     KW_RETURN identifier {}
     |
     ;

if_block:
        KW_IF '(' expr ')' KW_THEN commands {}
        | if_block KW_ELSE commands {}
        |
        ;

loop_block:
          KW_LOOP '(' expr ')' commands {}
          |
          ;


%%

