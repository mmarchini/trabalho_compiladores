#include "util.h"

int running;
int nline;
HashTable *hashTable;
int commentControl;



int yywrap(){
    return 1;
}

void initMe(){
    running = 1;
    nline=0;
    commentControl=0;
    hashTable = hashInit();
}

int getLineNumber(){ return nline; }

