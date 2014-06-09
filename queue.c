#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "queue.h"

#define TAC_INVALID_ERROR 18

TACQueue *TACQueueInsert(TACQueue *queue, TAC *tac){
	TACQueue *newQueue = calloc(1, sizeof(TACQueue));
	newQueue->tac=tac;
	newQueue->next = NULL;

	if(queue!=NULL)
		queue->next = newQueue;

	return queue != NULL ? queue : newQueue;
}

TACQueue *TACQueueJoin(TACQueue *queue1, TACQueue *queue2){
	queue1->next = queue2;

	return queue1;
}

TACQueue *TACQueuePrint(TACQueue *queue){


}

/* Util */

HashTable *HashInsertTmp(HashTable *hash, char *tmp_name){
	//TODO implement
}


/* Evaluation Functions */

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

TACQueue *TACVarDeclaration(ASTNode *ast, HashTable *hash){
	TACQueue *queue=NULL;
	ASTNode *auxAST=ast;
	HashTable *tmp;
	char auxStr[1024];
	int index, length;

	switch(ast->type){
		case AST_var:
		case AST_pt_var:
			queue = TACAssign(queue, ast->children[0]->hashValue, ast->hashValue);
			break;
		case AST_array_var:
			length = atoi(ast->children[1]->hashValue->value);
			index=0;
			for(auxAST=ast->children[2];auxAST!=NULL;auxAST=auxAST->children[1]){
				sprintf(auxStr, "$_%s[%d]", ast->hashValue->value, index);
				queue = TACAssign(
					queue,
					hashInsert(hash, auxStr, ast->hashValue->code),
					ast->children[1]->hashValue
				);
			}
			for(;index<length;index++){
				sprintf(auxStr, "$_%s[%d]", ast->hashValue->value, index);
				queue = TACAssign(
					queue,
					hashInsert(hash, auxStr, ast->hashValue->code),
					hashInsert(hash, "0", ast->children[0]->hashValue->code)
				);
			}
			break;
		default:
			exit(TAC_INVALID_ERROR);
			break;
	}

	return queue;
}

TACQueue *TACFuncDeclaration(ASTNode *ast, HashTable *hash){
	TACQueue *queue=NULL;
	ASTNode *current=ast;

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
			exit(TAC_INVALID_ERROR);
			break;
	}

	return queue;
}
