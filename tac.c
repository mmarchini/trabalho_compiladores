#include <stdlib.h>
#include "tac.h"

TAC *TACCreate(TACType type, HashTable *result, HashTable *op1, HashTable *op2){
		TAC *tac = calloc(1, sizeof(TAC));
		tac->type=type;
		tac->result=result;
		tac->op1=op1;
		tac->op2=op2;
		return tac;
}

