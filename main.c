#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "util.h"

extern HashTable *hashTable;
extern int running;

int yylex();
 
char tkstr[35][100] = {};

int main(){

strcpy(tkstr[256-256], "KW_WORD");
strcpy(tkstr[258-256], "KW_BOOL");
strcpy(tkstr[259-256], "KW_BYTE");
strcpy(tkstr[261-256], "KW_IF");
strcpy(tkstr[262-256], "KW_THEN");
strcpy(tkstr[263-256], "KW_ELSE");
strcpy(tkstr[264-256], "KW_LOOP");
strcpy(tkstr[266-256], "KW_INPUT");
strcpy(tkstr[267-256], "KW_RETURN");
strcpy(tkstr[268-256], "KW_OUTPUT");

strcpy(tkstr[270-256], "OPERATOR_LE");
strcpy(tkstr[271-256], "OPERATOR_GE");
strcpy(tkstr[272-256], "OPERATOR_EQ");
strcpy(tkstr[273-256], "OPERATOR_NE");
strcpy(tkstr[274-256], "OPERATOR_AND");
strcpy(tkstr[275-256], "OPERATOR_OR");

strcpy(tkstr[280-256], "TK_IDENTIFIER");
strcpy(tkstr[281-256], "LIT_INTEGER");
strcpy(tkstr[283-256], "LIT_FALSE");
strcpy(tkstr[284-256], "LIT_TRUE");
strcpy(tkstr[285-256], "LIT_CHAR");
strcpy(tkstr[286-256], "LIT_STRING");

strcpy(tkstr[290-256], "TOKEN_ERROR");


    int lexReturn;
    initMe();

    while(running){
        lexReturn=yylex();
        if(running){
            if(lexReturn<256)
                printf("%c\n", lexReturn);
            else
                printf("%s\n", tkstr[lexReturn-256]);
        }
    }
    printf("\nlinhas: %d\n", getLineNumber());
    hashPrint(hashTable);
    return 0;
}

