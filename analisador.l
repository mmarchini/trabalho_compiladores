%{
    #include "hash.h"
    #include "tokens.h"

    HashTable *hashTable;
%}
%%
[,;:()-/<>=!&\{\}\[\]\+\*\$]      return yytext[0];

\<=      return OPERATOR_LE;
>=      return OPERATOR_GE;
==      return OPERATOR_EQ;
!=      return OPERATOR_NE;
&&      return OPERATOR_AND;
\|\|      return OPERATOR_OR;


word   return KW_WORD; 
bool   return KW_BOOL; 
byte   return KW_BYTE; 
if     return KW_IF; 
then   return KW_THEN; 
else   return KW_ELSE; 
loop   return KW_LOOP; 
input  return KW_INPUT; 
output return KW_OUTPUT; 
return return KW_RETURN; 


False                  return LIT_FALSE; 
false                  return LIT_FALSE; 
FALSE                  return LIT_FALSE; 

True                   return LIT_TRUE; 
true                   return LIT_TRUE; 
TRUE                   return LIT_TRUE; 

[a-zA-Z][a-zA-Z0-9_]*  return TK_IDENTIFIER;
[0-9][0-9a-fA-F]*      return LIT_INTEGER;
'.'                    return LIT_CHAR;

\".*\"                   return LIT_STRING; 

" "*     {}
\t*    {}

.*      return TOKEN_ERROR;
%%


