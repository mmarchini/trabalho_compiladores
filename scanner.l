%{
    #include "y.tab.h"
    #include "hash.h"
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

    /* Operators */
[,;:()-/<>=!&\{\}\[\]\+\*\$]      {yylval=yytext; return yytext[0];}

\<=      return OPERATOR_LE;
>=       return OPERATOR_GE;
==       return OPERATOR_EQ;
!=       return OPERATOR_NE;
&&       return OPERATOR_AND;
\|\|     return OPERATOR_OR;

    /* Keywords */
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


    /* Literals */

    /* Booleans */
false                           {yylval=yytext;hashInsert(hashTable, yytext, LIT_FALSE); return LIT_FALSE;}
true                            {yylval=yytext;hashInsert(hashTable, yytext, LIT_TRUE); return LIT_TRUE;} 

    /* Integers */
[0-9]*                          {yylval=yytext;hashInsert(hashTable, yytext, LIT_INTEGER); return LIT_INTEGER;} 

    /* Characters */
'.'                             {yylval=yytext;char auxChar[2]; auxChar[0]=yytext[1]; auxChar[1]='\0'; hashInsert(hashTable, auxChar, LIT_INTEGER); return LIT_CHAR;}

    /* Identifiers */
[0-9]*[_a-zA-Z]+[_a-zA-Z0-9_]*  {yylval=yytext; hashInsert(hashTable, yytext, TK_IDENTIFIER); return TK_IDENTIFIER;}

    /* Begin String Rule */
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
    /* End String Rule */

    /* Begin Comments */
\/\/.*                  {}
"/*"       { BEGIN COMMENT_BLOCK; }
<COMMENT_BLOCK>"*/"     { BEGIN 0;}
<COMMENT_BLOCK>\n       {nline++;}
<COMMENT_BLOCK>.        {}
    /* End Comments */

<<EOF>>      {running=0; return;}

.      { return TOKEN_ERROR; } 

%%


