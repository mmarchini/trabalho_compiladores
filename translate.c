#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "queue.h"
#include "util.h"
#include "hash.h"
#include "y.tab.h"
#include "symbols.h"

#define LAST_CHAR(str) str[strlen(str)-1]
#define REGISTER(hash) hash->type == DT_WORD ? "ax" : "al"
#define DX(hash) hash->type == DT_WORD ? "DX" : "DL"
#define GET_ARRAY_INDEX(dest, origin) strtok(origin, "["); dest = strtok(NULL, "["); dest[strlen(dest)-1] = '\0';
#define GET_ARRAY_NAME(str) strcpy(str, str+2); strtok(str, "[");

extern HashTable *hashTable;
extern FILE *outputFile;

void outputTac(char *str){
	int i;
	if(outputFile != NULL){
		fprintf(outputFile, str);
		fprintf(outputFile, "\n");
	}
	else {
		printf(str);
		printf("\n");
	}
}

void _LABEL(char *label){
	char str[strlen(label)+2];

	sprintf(str, "%s:", label);

	outputTac(str);
}

void translateLabel(TACQueue *queue){
	_LABEL(queue->tac->result->value);
}

void translateEndFun(TACQueue *queue){
	char str[strlen(queue->tac->result->value)+12];
	sprintf(str, "%s endp", queue->tac->result->value);

	outputTac("mov ax, 0");
	outputTac("ret");

	outputTac(str);

	sprintf(str, "$_%s_end", queue->tac->result->value);
	_LABEL(str);

}

void translateBeginFun(TACQueue *queue){
	char *str = calloc('\0', strlen(queue->tac->result->value)+12);

	sprintf(str, "jmp $_%s_end", queue->tac->result->value);
	outputTac(str);

	sprintf(str, "%s proc near", queue->tac->result->value);

	outputTac(str);
}

void _ADD(HashTable *result, HashTable *op1, HashTable *op2);

char *__ADDRESS(HashTable *hash) {
	char aux[1024];
	char name[1024];
	char *index;

	if(hash->code == SYMBOL_LIT_CHAR){
		sprintf(aux, "'%s'", hash->value);
		index = calloc(strlen(aux)+1, sizeof(char));
		strcpy(index, aux);
		return index;
	}


	if(LAST_CHAR(hash->value) != ']')
		return hash->value;

	strcpy(name, hash->value);
	GET_ARRAY_NAME(name);

	sprintf(aux, "LEA bx, %s", name);
	outputTac(aux);

	strcpy(name, hash->value);
	GET_ARRAY_INDEX(index, name);
	sprintf(aux, "ADD bx, %s", index);
	outputTac(aux);

	outputTac("");

	return "[bx]";
}

void _MOV(HashTable *result, HashTable *op){
	char aux[1024];


	outputTac("MOV ax, 0");
	sprintf(aux, "MOV %s,  %s", REGISTER(op), __ADDRESS(op));
	outputTac(aux);

	sprintf(aux, "MOV %s,  %s", __ADDRESS(result), REGISTER(result));
	outputTac(aux);

	outputTac("");
}

void _2OP(char *operator, HashTable *result, HashTable *op1, HashTable *op2){
	char aux[1024];

	outputTac("MOV ax, 0");

	sprintf(aux, "MOV %s,  %s", REGISTER(op1), __ADDRESS(op1));
	outputTac(aux);

	sprintf(aux, "%s %s,  %s", operator, REGISTER(op1), __ADDRESS(op2));
	outputTac(aux);

	sprintf(aux, "MOV %s,  %s", __ADDRESS(result), REGISTER(result));
	outputTac(aux);

}

void _ADD(HashTable *result, HashTable *op1, HashTable *op2) {
	_2OP("ADD", result, op1, op2);
	outputTac("");
}

void _SUB(HashTable *result, HashTable *op1, HashTable *op2) {
	_2OP("SUB", result, op1, op2);
	outputTac("");
}

void _AND(HashTable *result, HashTable *op1, HashTable *op2) {
	_2OP("AND", result, op1, op2);
	outputTac("");
}

void _OR(HashTable *result, HashTable *op1, HashTable *op2) {
	_2OP("OR", result, op1, op2);
	outputTac("");
}


void _1OP(char *operator, HashTable *result, HashTable *op1, HashTable *op2){
	char aux[1024];

	outputTac("MOV ax, 0");
	sprintf(aux, "MOV %s,  %s", REGISTER(op1), __ADDRESS(op1));
	outputTac(aux);

	sprintf(aux, "%s %s", operator, __ADDRESS(op2));
	outputTac(aux);

	sprintf(aux, "MOV %s,  %s", __ADDRESS(result), REGISTER(op1));
	outputTac(aux);


}

void _MUL(HashTable *result, HashTable *op1, HashTable *op2) {
	_1OP("IMUL", result, op1, op2);
	outputTac("");
}

void _DIV(HashTable *result, HashTable *op1, HashTable *op2) {
	_1OP("IDIV", result, op1, op2);
	outputTac("");
}

void translateMOVE(TAC *tac){
	_MOV(tac->result, tac->op1);
}

void _CMP(TACType operator, HashTable *result, HashTable *op1, HashTable *op2){
	char aux[1024];

	outputTac("MOV ax, 0");
	sprintf(aux, "MOV %s,  %s", REGISTER(op1), __ADDRESS(op1));
	outputTac(aux);

	sprintf(aux, "CMP %s, %s", REGISTER(op2), __ADDRESS(op2));
	outputTac(aux);

	outputTac("LAHF");
	outputTac("MOV AL, AH");
	outputTac("MOV BL, AH");

	outputTac("SHR AL, 6"); // ZF = 7
	outputTac("AND AL, 1"); // ZF = 7

	outputTac("SHR AH, 7"); // SF = 8
	outputTac("AND AH, 1"); // SF = 8

	outputTac("SHR BL, 11"); // OF = 12
	outputTac("AND BL, 1");  // OF = 12


	switch(operator) {
		case TAC_LE:
		case TAC_GREAT:
			outputTac("XOR bl, ah");
			outputTac("OR bl, al");
			outputTac("MOV ax, 0");
			outputTac("MOV al, bl");
			if(operator == TAC_GREAT)
				outputTac("XOR ax, 1");
			break;
		case TAC_LESS:
		case TAC_GE:
			outputTac("XOR bl, ah");
			outputTac("MOV ax, 0");
			outputTac("MOV al, bl");
			if(operator == TAC_GE)
				outputTac("XOR ax, 1");
			break;
		case TAC_NE:
		case TAC_EQ:
			outputTac("MOV bl, al");
			outputTac("MOV ax, 0");
			outputTac("MOV al, bl");
			if(operator == TAC_NE)
				outputTac("XOR ax, 1");
			break;
		default:
			exit(67);
	}

	sprintf(aux, "MOV %s, %s", __ADDRESS(result), REGISTER(result));
	outputTac(aux);
	outputTac("");
}

void _NOT(HashTable *result, HashTable *op){
	char aux[1024];

	sprintf(aux, "MOV %s, %s", REGISTER(op), __ADDRESS(op));
	outputTac(aux);

	sprintf(aux, "NOT %s", REGISTER(op));
	outputTac(aux);

	sprintf(aux, "MOV %s, %s", __ADDRESS(result), REGISTER(result));
	outputTac(aux);

	outputTac("");
}

void _IFZ(HashTable *result, HashTable *expr){
	char aux[1024];
	sprintf(aux, "CMP %s, 0", __ADDRESS(expr));
	outputTac(aux);

	sprintf(aux, "JE %s", result->value);
	outputTac(aux);

	outputTac("");
}

void _IFN(HashTable *result, HashTable *expr){
	char aux[1024];
	sprintf(aux, "CMP %s, 0", __ADDRESS(expr));
	outputTac(aux);

	sprintf(aux, "JNE %s", result->value);
	outputTac(aux);

	outputTac("");
}

void _JUMP(HashTable *result){
	char aux[1024];

	sprintf(aux, "JMP %s", result->value);
	outputTac(aux);

	outputTac("");
}

void _ARG(HashTable *arg){
	char aux[1024];
	outputTac("MOV AH, 0");
	sprintf(aux, "MOV %s, %s", REGISTER(arg), __ADDRESS(arg));
	outputTac(aux);
	outputTac("PUSH AX");

	outputTac("");
	return;
}

typedef struct _AUX {
	HashTable *arg;
	struct _AUX *parent;
}AUX;

void _CALL(HashTable *result, HashTable *op1){
	char aux[1024];
	ASTNode *args=NULL;
	AUX *stack=NULL;
	AUX *stack_aux=NULL;
	for(args = (ASTNode *)op1->args; args!=NULL; args = args->children[1]){
		stack_aux = calloc(0, sizeof(AUX));
		stack_aux->arg = args->hashValue;
		stack_aux->parent = stack;
		stack=stack_aux;
	}

	while(stack!=NULL){
		outputTac("POP ax");
		sprintf(aux, "MOV %s, %s", stack->arg->value, REGISTER(stack->arg));
		outputTac(aux);
		stack_aux = stack;
		stack = stack->parent;
		free(stack_aux);
	}



	sprintf(aux, "CALL %s", op1->value);
	outputTac(aux);

	sprintf(aux, "MOV %s, %s", __ADDRESS(result), REGISTER(result));
	outputTac(aux);

	outputTac("");
}

void _RETURN(HashTable *result, HashTable *op1){
	char aux[1024];
	outputTac("MOV AX, 0");

	sprintf(aux, "MOV %s, %s", REGISTER(result), __ADDRESS(result));
	outputTac(aux);

	outputTac("RET");

	outputTac("");
}

void _PRINT(HashTable *result){
	char aux[1024];
	outputTac("MOV AX, 0");

	sprintf(aux, "MOV AH, %d", result->type == DT_STRING ? 9 : 2);
	outputTac(aux);

	sprintf(
		aux,
		"%s %s, %s",
		result->type == DT_STRING ? "LEA" : "MOV",
		result->type == DT_STRING ? "DX" : DX(result),
		__ADDRESS(result)
	);
	outputTac(aux);

	outputTac("INT 21H");
}

void _INPUT(HashTable *result){
	char aux[1024];
	outputTac("MOV AX, 0");
	outputTac("MOV AH, 1");

	outputTac("INT 21H");
	outputTac("MOV AH, 0");

	sprintf(aux, "MOV %s, %s", __ADDRESS(result), REGISTER(result));
	outputTac(aux);
}


void translateTAC(TACQueue *queue){
	switch(queue->tac->type){
		case TAC_MOVE: translateMOVE(queue->tac); break;

		case TAC_GREAT:
		case TAC_LESS:
		case TAC_LE:
		case TAC_GE:
		case TAC_EQ:
		case TAC_NE:
			_CMP(queue->tac->type, queue->tac->result, queue->tac->op1, queue->tac->op2);
			break;

		case TAC_AND: _AND(queue->tac->result, queue->tac->op1, queue->tac->op2); break;
		case TAC_OR: _OR(queue->tac->result, queue->tac->op1, queue->tac->op2); break;

		case TAC_ADD: _ADD(queue->tac->result, queue->tac->op1, queue->tac->op2); break;
		case TAC_SUB: _SUB(queue->tac->result, queue->tac->op1, queue->tac->op2); break;
		case TAC_MUL: _MUL(queue->tac->result, queue->tac->op1, queue->tac->op2); break;
		case TAC_DIV: _DIV(queue->tac->result, queue->tac->op1, queue->tac->op2); break;

		//case TAC_ADDRESS: break; //TODOa Consertar no resto do compilador
		//case TAC_POINTER: break; //TODOa Consertar no resto do compilador
		case TAC_NOT: _NOT(queue->tac->result, queue->tac->op1); break;

		case TAC_LABEL: translateLabel(queue); break;
		case TAC_BEGINFUN: translateBeginFun(queue); break;
		case TAC_ENDFUN: translateEndFun(queue); break;

		case TAC_IFZ: _IFZ(queue->tac->result, queue->tac->op1); break;
		case TAC_IFN: _IFN(queue->tac->result, queue->tac->op1); break;
		case TAC_JUMP: _JUMP(queue->tac->result); break;

		case TAC_CALL: _CALL(queue->tac->result, queue->tac->op1); break;
		case TAC_ARG: _ARG(queue->tac->result); break;
		case TAC_RET: _RETURN(queue->tac->result, queue->tac->op1); break;

		case TAC_PRINT: _PRINT(queue->tac->result); break;
		case TAC_READ: _INPUT(queue->tac->result); break;

		default: break;
	}
}

char *getTypeFromHash(HashTable *node){
	switch(node->type){
	case DT_BYTE:
	case DT_BOOL:
	case DT_STRING:
		return "DB";
	case DT_WORD:
		return "DW";
	default:
		PRINT_HASH(node);
		exit(36);
	}

}

void translateData(HashTable *hash) {
	char aux[1024];
	char *index_aux;
	char *intConvAux;
	char *value = hash->value;
	long int index, array_len, i;
	HashTable *curHash;
	int len = strlen(value);

	if(hash->code != SYMBOL_IDENTIFIER || value[len-1] == ']' || hash->nature == DN_FUNCTION || hash->type == DT_LABEL)
		return;
	if(hash->type == DT_STRING){
		sprintf(aux, "%s      %s   '%s$'", value, getTypeFromHash(hash), (char *)hash->args);
		outputTac(aux);
		return;
	}

	// Não vetores
	sprintf(aux, "%s      %s   ?", value, getTypeFromHash(hash));
	outputTac(aux);
	// Ajeita vetor
	if(hash->nature == DN_ARRAY && hash->value[0] != '$'){
		array_len = 1;
		value = calloc(1024, sizeof(char));
		for(i=0;i<HASH_SIZE;i++) {
			curHash=&hashTable[i];
			if(curHash->value){
				for(;curHash->next;curHash=curHash->next){
					if(curHash->value[strlen(curHash->value)-1] == ']'){
						strcpy(aux, curHash->value);
						GET_ARRAY_INDEX(index_aux, aux);

						strcpy(value, curHash->value);
						GET_ARRAY_NAME(value);
						//FIXME Bug conhecido: indices acessados no meio das funções somam aqui
						if(!strcmp(value, hash->value)){
							index=strtol(index_aux,&intConvAux,10);
							if(index != 0)
								array_len++;
						}
					}
				}
				if(curHash->value) {
					if(curHash->value[strlen(curHash->value)-1] == ']'){
						strcpy(aux, curHash->value);
						GET_ARRAY_INDEX(index_aux, aux);

						strcpy(value, curHash->value);
						GET_ARRAY_NAME(value);
						//FIXME Bug conhecido: indices acessados no meio das funções somam aqui
						if(!strcmp(value, hash->value)){
							index=strtol(index_aux,&intConvAux,10);
							if(index != 0)
								array_len++;
						}
					}
				}
			}
		}

		for(i=1; i<array_len;i++){
			bzero(aux, 1024);
			strcpy(aux, "        ");
			strcpy(aux+8, getTypeFromHash(hash));
			strcpy(aux+10, "    ?");
			outputTac(aux);
		}
	}


}

void createDataCode(TACQueue *queue) {
	int i;
	HashTable *curHash;
	bool go=true;

	outputTac("assume cs:$$_CODIGO,ds:$$_DADOS,es:$$_DADOS,ss:$$_PILHA");
	outputTac("");
	outputTac("true      EQU    01H");
	outputTac("false     EQU    00H");
	outputTac("");

	outputTac("$$_DADOS   segment");
	for(i=0;i<HASH_SIZE;i++) {
		curHash=&hashTable[i];
		if(curHash->value){
			for(;curHash->next;curHash=curHash->next)
				translateData(curHash);
			if(curHash->value)
				translateData(curHash);
		}
	}

	outputTac("$$_DADOS   ends");

	outputTac("");
	outputTac("$$_PILHA   segment");
	outputTac("    dw     128 dup(?)");
	outputTac("$$_PILHA   ends");
	outputTac("");


}

void createProgramCode(TACQueue *queue) {
	TACQueue *auxQueue;
	bool go=false;

	outputTac("$$_CODIGO   segment");
	outputTac("$$_BEGIN:");
	outputTac("MOV AX, $$_DADOS");
	outputTac("MOV DS, AX");
	outputTac("MOV ES, AX");

	for(auxQueue=queue;auxQueue!=NULL;auxQueue=auxQueue->next)
		translateTAC(auxQueue);

	outputTac("call main");
	outputTac("mov al, 0");
	outputTac("mov ah, 4ch");
	outputTac("int 21h");

	outputTac("$$_CODIGO   ends");
	outputTac("end $$_BEGIN");
}

void translate(TACQueue *queue){
	createDataCode(queue);
	createProgramCode(queue);
}
