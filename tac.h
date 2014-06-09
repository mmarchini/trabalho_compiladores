#ifndef _TAC_H
#define _TAC_H

#include "hash.h"
#include "ast.h"

typedef enum {
	TAC_SYMBOL,
	TAC_MOVE,

	TAC_ADD,
	TAC_MUL,

	TAC_LABEL,
	TAC_BEGINFUN,
	TAC_ENDFUN,

	TAC_IFZ,
	TAC_JUMP,

	TAC_CALL,
	TAC_ARG,

	TAC_RET,
	TAC_PRINT,
	TAC_READ
} TACType;

typedef struct _TAC {
	TACType type;
	HashTable *result;
	HashTable *op1;
	HashTable *op2;
}TAC;

TAC *TACCreate(TACType type, HashTable *result, HashTable *op1, HashTable *op2);

TAC *ASTtoTAC(ASTNode *ast, HashTable *hash);

#endif
