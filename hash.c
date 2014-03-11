#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASH_SIZE 11

void hashPrint(HashTable *hash){
    printf("Hash Table:\n---------\n");
    for(int i=0;i<HASH_SIZE;i++){
        HashTable *curHash=&hash[i];
        if(curHash->value){
            printf("New Hash Index:\n");
            for(;curHash->next;curHash=curHash->next){
                    printf("%s\n", curHash->value);
            }
            if(curHash->value)
                printf("%s\n", curHash->value);
        }
    }
}

HashTable *hashInit(){
    HashTable *hash = calloc(11, sizeof(HashTable));
    if(!hash){
        exit(1);
    }
    return hash;
}

int hashCalculate(char *value){
    int hashIndex = 1;
    for(int i=0; i<strlen(value)-1; i++) hashIndex*=value[i];
    return hashIndex % HASH_SIZE;
}

void hashInsert(HashTable *hash, char *value, int code){
    int hashIndex = hashCalculate(value); 
    HashTable *curHash = &hash[hashIndex];

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
}

HashTable *hashSearch(HashTable *hash, char *value){
    HashTable *curHash=&hash[hashCalculate(value)];
    for(
        ;
        curHash->next && !strcmp(value, curHash->value);
        curHash=curHash->next
    ); 
    if(strcmp(curHash->value, value))
        return 0;
    return curHash;
}

