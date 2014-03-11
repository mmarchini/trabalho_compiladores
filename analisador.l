%{
    #include "hash.h"
    #include "tokens.h"
    #include "util.h"

    int running;
    int nline;
    HashTable *hashTable;

    int commentControl;

%}
%%

[,;:()-/<>=!&\{\}\[\]\+\*\$]      return yytext[0];

\<=      if(!commentControl) return OPERATOR_LE;
>=       if(!commentControl) return OPERATOR_GE;
==       if(!commentControl) return OPERATOR_EQ;
!=       if(!commentControl) return OPERATOR_NE;
&&       if(!commentControl) return OPERATOR_AND;
\|\|     if(!commentControl) return OPERATOR_OR;


word     if(!commentControl) return KW_WORD; 
bool     if(!commentControl) return KW_BOOL; 
byte     if(!commentControl) return KW_BYTE; 
if       if(!commentControl) return KW_IF; 
then     if(!commentControl) return KW_THEN; 
else     if(!commentControl) return KW_ELSE; 
loop     if(!commentControl) return KW_LOOP; 
input    if(!commentControl) return KW_INPUT; 
output   if(!commentControl) return KW_OUTPUT; 
return   if(!commentControl) return KW_RETURN; 


False                  if(!commentControl) return LIT_FALSE; 
false                  if(!commentControl) return LIT_FALSE; 
FALSE                  if(!commentControl) return LIT_FALSE; 

True                   if(!commentControl) return LIT_TRUE; 
true                   if(!commentControl) return LIT_TRUE; 
TRUE                   if(!commentControl) return LIT_TRUE; 

[a-zA-Z][a-zA-Z0-9_]*  if(!commentControl) {hashInsert(hashTable, yytext, TK_IDENTIFIER); return TK_IDENTIFIER;}
[0-9][0-9a-fA-F]*      if(!commentControl) return LIT_INTEGER;
'.'                    if(!commentControl) return LIT_CHAR;

\".*\"                 if(!commentControl) return LIT_STRING; 

" "*     {}
\t*    {}

\n      nline++;

<<EOF>>      {running=0; return;}

\/\/.*    {}
"/*"      { commentControl = 1;}
\*\/      { commentControl = 0;}


%%


