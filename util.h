#ifndef _UTIL
#define _UTIL
#include "hash.h"

// Global variables
extern int running;
extern int nline;
extern HashTable *hashTable;

extern int commentControl;


// Methods

void initMe();
int getLineNumber();

#endif
