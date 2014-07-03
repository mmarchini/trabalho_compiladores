#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HashTable *HashSetTypeNature(HashTable *hash, DataType type, DataNature nature){
	hash->type = type;
	hash->nature = nature;

	return hash;
}

void hashPrint(HashTable *hash){
    int i;
    printf("Hash Table:\n---------\n");
    for(i=0;i<HASH_SIZE;i++){
        HashTable *curHash=&hash[i];
        if(curHash->value){
            for(;curHash->next;curHash=curHash->next){
            	PRINT_IHASH(curHash, i);
            }
            if(curHash->value)
            	PRINT_IHASH(curHash, i);
        }
    }
}

HashTable *hashInit(){
    HashTable *hash = calloc(HASH_SIZE, sizeof(HashTable));
    if(!hash){
        exit(1);
    }
    return hash;
}

int hashCalculate(char *value){
    int i;
    int hashIndex = 1;
    for(i=0; i<strlen(value); i++) hashIndex = (hashIndex*value[i])%HASH_SIZE + 1;
    return hashIndex - 1;
}

HashTable *hashInsert(HashTable *hash, char *value, int code){
    HashTable *curHash = NULL;
    int hashIndex = hashCalculate(value); 
    curHash = hashSearch(hash, value, code);
    if(curHash)
        return curHash;
    curHash = &hash[hashIndex];

    // Busca o último elemento daquele slot;
    for(
        ;
        curHash->next;
        curHash=curHash->next
    );

    // Se o slot já está ocupado, criamos um novo elemento na lista
    if(curHash->value){
        curHash->next = calloc(1, sizeof(HashTable));
        curHash = curHash->next;
    }

    curHash->value = calloc(1, strlen(value)+1);
    strcpy(curHash->value, value);
    curHash->code  = code;
    curHash->type  = DT_NULL;
    curHash->nature = DN_NULL;
    curHash->args = NULL; // SUPERSAYAJIN HACK


    return curHash;
}

HashTable *hashSearch(HashTable *hash, char *value, int code){
    HashTable *curHash=&hash[hashCalculate(value)];
    if(!curHash->value)
        return 0;
    for(
        ;
        curHash->next && (strcmp(value, curHash->value) || code != curHash->code);
        curHash=curHash->next
    ); 
    if(strcmp(curHash->value, value) || code != curHash->code)
        return 0;
    return curHash;
}

