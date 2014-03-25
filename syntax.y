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
       program TK_IDENTIFIER { printf("Variavel: %s, \n", $2); }
       |
       ;

%%

