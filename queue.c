#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "queue.h"
#include "y.tab.h"

#define TAC_INVALID_ERROR 18

TACQueue *TACQueueInsert(TACQueue *queue, TAC *tac){
	TACQueue *newQueue = calloc(1, sizeof(TACQueue));
	TACQueue *auxQueue = NULL;
	newQueue->tac=tac;
	newQueue->next = NULL;

	if(queue==NULL) return newQueue;

	for(auxQueue=queue;auxQueue->next!=NULL;auxQueue=auxQueue->next);
	auxQueue->next = newQueue;

	return queue;
}

TACQueue *TACQueueJoin(TACQueue *queue1, TACQueue *queue2){
	TACQueue *auxQueue = NULL;

	if(queue1==NULL)
		return queue2;

	for(auxQueue=queue1;auxQueue->next!=NULL;auxQueue=auxQueue->next);
	auxQueue->next = queue2;

	return queue1;
}

char *codeString(int code){
	switch(code){
	case TAC_SYMBOL: return "SYMBOL";
	case TAC_MOVE: return "MOVE";

	case TAC_ADD: return "ADD";
	case TAC_MUL: return "MUL";

	case TAC_LABEL: return "LABEL";
	case TAC_BEGINFUN: return "BEGINFUN";
	case TAC_ENDFUN: return "ENDFUN";

	case TAC_IFZ: return "IFZ";
	case TAC_JUMP: return "JUMP";

	case TAC_CALL: return "CALL";
	case TAC_ARG: return "ARG";

	case TAC_RET: return "RET";
	case TAC_PRINT: return "PRINT";
	case TAC_READ: return "READ";
	}
}

TACQueue *TACQueuePrint(TACQueue *queue){
	TAC *tac;
	char code[256];
	for(;queue!=NULL;queue=queue->next){
		tac = queue->tac;
		printf(
			"%s %s %s %s\n",
			codeString(tac->type),
			tac->result != NULL ? tac->result->value : "",
			tac->op1    != NULL ? tac->op1->value    : "",
			tac->op2    != NULL ? tac->op2->value    : ""
		);
	}


}

/* Util */

HashTable *HashInsertTmp(HashTable *hash, char *tmp_name){
	//TODO implement
}


/* Evaluation Functions */

HashTable *DefaultValue(ASTNode *ast, HashTable *hash){
	return hashInsert(hash, ast->children[0]->type == KW_BOOL ? "true" : "0",
			ast->children[0]->type == KW_BOOL ? LIT_TRUE : LIT_INTEGER);
}

TACQueue *TACAssign(TACQueue *queue, HashTable *var, HashTable *value){
	return TACQueueInsert(
		queue,
		TACCreate(
			TAC_MOVE,
			var,
			value,
			NULL
		)
	);
}

TACQueue *TACArrayAssign(TACQueue *queue, HashTable *hash, HashTable *var,
		int index, HashTable *value) {
	char auxStr[1024];
	sprintf(auxStr, "$_%s[%d]", var->value, index);
	return TACAssign(
		queue,
		hashInsert(hash, auxStr, var->code),
		value
	);
	return TACQueueInsert(
		queue,
		TACCreate(
			TAC_MOVE,
			var,
			value,
			NULL
		)
	);
}

TACQueue *TACVarDeclaration(ASTNode *ast, HashTable *hash){
	TACQueue *queue=NULL;
	ASTNode *auxAST=ast;
	HashTable *tmp;
	char auxStr[1024];
	int index, length;

	switch(ast->type){
		case AST_var:
		case AST_pt_var:
			queue = TACAssign(queue, ast->hashValue, ast->children[0]->hashValue);
			break;
		case AST_array_var:
			length = atoi(ast->children[1]->hashValue->value);
			index=0;
			for(auxAST=ast->children[2];auxAST!=NULL;auxAST=auxAST->children[1]){
				queue = TACArrayAssign(
					queue,hash,
					ast->hashValue,index,
					ast->children[1]->hashValue
				);
			}
			for(;index<length;index++){
				queue = TACArrayAssign(
					queue,
					hash,
					ast->hashValue,
					index,
					DefaultValue(ast, hash)
				);
			}
			break;
		default:
			exit(TAC_INVALID_ERROR);
			break;
	}

	return queue;
}

TACQueue *TACCommand(ASTNode *ast, HashTable *hash){
	return NULL;
}

TACQueue *TACArgsDeclaration(ASTNode *ast, HashTable *hash){
	TACQueue *queue=NULL;
	for(ast=ast->children[1];ast!=NULL;ast=ast->children[1]){
		queue = TACAssign(
			queue,
			ast->hashValue,
			DefaultValue(ast, hash)
		);
	}
	return queue;
}

TACQueue *TACFuncDeclaration(ASTNode *ast, HashTable *hash){
	TACQueue *queue = TACQueueInsert(
		queue,
		TACCreate(
			TAC_BEGINFUN,
			NULL,
			NULL,
			NULL
		)
	);

	queue = TACQueueJoin(
		queue,
		TACQueueJoin(
			TACArgsDeclaration(ast->children[0], hash),
			TACCommand(ast->children[1], hash)
		)
	);

	queue = TACQueueInsert(
		queue,
		TACCreate(
			TAC_ENDFUN,
			NULL,
			NULL,
			NULL
		)
	);

	return queue;
}


TACQueue *TACProgram(ASTNode *ast, HashTable *hash){
	TACQueue *queue=NULL;
	ASTNode *current=ast;

	if(ast==NULL)
		return NULL;

	switch(ast->children[0]->type){
		case AST_def_func:
			queue = TACQueueJoin(
				TACFuncDeclaration(ast->children[0], hash),
				TACProgram(ast->children[1], hash)
			);
			break;
		case AST_var:
		case AST_array_var:
		case AST_pt_var:
			queue = TACQueueJoin(
				TACVarDeclaration(ast->children[0], hash),
				TACProgram(ast->children[1], hash)
			);
			break;
		default:
			//exit(TAC_INVALID_ERROR);
			break;
	}

	return queue;
}
