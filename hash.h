#ifndef _HASH
#define _HASH

typedef enum {
    DT_INVALID= -1, 
    DT_NULL = 0, 
    DT_BYTE = 1, 
    DT_WORD = 2, 
    DT_STRING = 3, 
    DT_BOOL = 4
}DataType;

typedef enum {
    DN_INVALID= -1, 
    DN_NULL = 0, 
    DN_SCALAR = 1, 
    DN_ARRAY = 2, 
    DN_POINTER = 3, 
    DN_FUNCTION = 4, 
}DataNature;

typedef struct _HashTable{
    char *value;
    int code;
    DataType type;
    DataNature nature;
    struct _HashTable *next;
    void *args;
} HashTable;

HashTable *hashInit();

HashTable *hashInsert(HashTable *hash, char *value, int code);

HashTable *hashSearch(HashTable *hash, char *value, int code);

int hashCalculate(char *value);

void hashPrint(HashTable *hash);

#endif
