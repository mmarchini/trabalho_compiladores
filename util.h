#ifndef _UTIL
#define _UTIL
#include "hash.h"

// Global variables
extern int running;
extern int nline;
extern HashTable *hashTable;

extern int commentControl;

typedef enum {true = 1, false = 0} bool;

// Methods

void initMe();
int getLineNumber();

int yyerror();

#endif
