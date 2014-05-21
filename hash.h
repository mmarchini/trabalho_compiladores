#ifndef _HASH
#define _HASH

typedef enum {
    DT_INVALID= -1, 
    DT_NULL = 0, 
    DT_BYTE = 1, 
    DT_WORD = 2, 
    DT_CHAR = 3, 
    DT_STRING = 4, 
    DT_BOOL = 5
}DataType;

typedef struct _HashTable{
    char *value;
    int code;
    DataType type;
    struct _HashTable *next;
} HashTable;

HashTable *hashInit();

HashTable *hashInsert(HashTable *hash, char *value, int code);

HashTable *hashSearch(HashTable *hash, char *value, int code);

int hashCalculate(char *value);

void hashPrint(HashTable *hash);

#endif
