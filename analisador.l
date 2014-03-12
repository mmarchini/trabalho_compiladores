%{
    #include "hash.h"
    #include "tokens.h"
    #include "util.h"

    int running;
    int nline;
    char buf[1024];
    char *s;
    HashTable *hashTable;

%}

%x STRING
%x COMMENT_BLOCK 

%%

[ \t]   ;
\n      nline++;


[,;:()-/<>=!&\{\}\[\]\+\*\$]      return yytext[0];

\<=      return OPERATOR_LE;
>=       return OPERATOR_GE;
==       return OPERATOR_EQ;
!=       return OPERATOR_NE;
&&       return OPERATOR_AND;
\|\|     return OPERATOR_OR;


word     return KW_WORD; 
bool     return KW_BOOL; 
byte     return KW_BYTE; 
if       return KW_IF; 
then     return KW_THEN; 
else     return KW_ELSE; 
loop     return KW_LOOP; 
input    return KW_INPUT; 
output   return KW_OUTPUT; 
return   return KW_RETURN; 


false                  return LIT_FALSE; 

true                   return LIT_TRUE; 

[a-zA-Z][a-zA-Z0-9_]*  {hashInsert(hashTable, yytext, TK_IDENTIFIER); return TK_IDENTIFIER;}
[0-9][0-9a-fA-F]*      return LIT_INTEGER;


'.'                    return LIT_CHAR;

\" { BEGIN STRING; s = buf; } 
<STRING>\\n { *s++ = '\n'; } 
<STRING>\\t { *s++ = '\t'; } 
<STRING>\\\" { *s++ = '\"'; } 
<STRING>\" { 

    *s++ = '\0';
    *s = 0; 
    hashInsert(hashTable, buf, LIT_STRING);


    BEGIN 0; 
    return LIT_STRING; 

} 
<STRING>\n { nline++; *s++ = '\n';} 
<STRING>. { *s++ = yytext[0]; }

<<EOF>>      {running=0; return;}

\/\/.*    {}
"/**/"                    { }
"/*"[^ \t\n"*/"]*                    { BEGIN COMMENT_BLOCK; }
<COMMENT_BLOCK>"*/"     { BEGIN 0;}
<COMMENT_BLOCK>\n       {nline++;}
<COMMENT_BLOCK>.       {}

    /*[^ \t\n]+      { return TOKEN_ERROR; } */

%%


