#ifndef _TAC_H
#define _TAC_H

#include "hash.h"
#include "ast.h"

typedef enum {
	TAC_SYMBOL,
	TAC_MOVE,

	TAC_GREAT,
	TAC_LESS,
	TAC_LE,
	TAC_GE,
	TAC_EQ,
	TAC_NE,
	TAC_AND,
	TAC_OR,
	TAC_ADD,
	TAC_SUB,
	TAC_MUL,
	TAC_DIV,

	TAC_ADDRESS,
	TAC_POINTER,
	TAC_NOT,

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
