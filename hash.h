#ifndef _HASH
#define _HASH

typedef struct _HashTable{
    char *value;
    int code;
    struct _HashTable *next;
} HashTable;

HashTable *hashInit();

HashTable *hashInsert(HashTable *hash, char *value, int code);

HashTable *hashSearch(HashTable *hash, char *value, int code);

int hashCalculate(char *value);

void hashPrint(HashTable *hash);

#endif
