#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
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

	case TAC_GREAT: return "GREAT";
	case TAC_LESS: return "LESS";
	case TAC_LE: return "LE";
	case TAC_GE: return "GE";
	case TAC_EQ: return "EQ";
	case TAC_NE: return "NE";
	case TAC_AND: return "AND";
	case TAC_OR: return "OR";
	case TAC_ADD: return "ADD";
	case TAC_SUB: return "SUB";
	case TAC_MUL: return "MUL";
	case TAC_DIV: return "DIV";

	case TAC_ADDRESS: return "ADDRESS";
	case TAC_POINTER: return "POINTER";
	case TAC_NOT: return "NOT";

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

	default: return "**NOSTRING**";
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

	return queue;
}

/* Util */

HashTable *HashInsertTmp(HashTable *hash, char *tmp_name){
	bool not_found=true;
	char aux_name[strlen(tmp_name)+16];
	int index=0;

	while(not_found){
		sprintf(aux_name, "$_%s_%d", tmp_name, index);
		if( hashSearch(hash, aux_name, TK_IDENTIFIER) == NULL)
			not_found=false;
		else
			index++;
	}
	return hashInsert(hash, aux_name, TK_IDENTIFIER);
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

TACQueue *TACArrayAssignInt(TACQueue *queue, HashTable *hash, HashTable *var,
		int index, HashTable *value)
{
	char auxStr[1024];
	sprintf(auxStr, "$_%s[%d]", var->value, index);
	return TACAssign(
		queue,
		hashInsert(hash, auxStr, var->code),
		value
	);
}

TACQueue *TACArrayAssignExpr(TACQueue *queue, HashTable *hash, HashTable *var,
		HashTable *index, HashTable *value)
{

	return TACAssign(
		queue,
		HashInsertTmp(hash, "array_attribution"),
		value
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
			queue = TACAssign(
				queue,
				ast->hashValue,
				ast->children[1]->hashValue
			);
			break;
		case AST_array_var:
			length = atoi(ast->children[1]->hashValue->value);
			index=0;
			for(auxAST=ast->children[2];auxAST!=NULL;auxAST=auxAST->children[1]){
				queue = TACArrayAssignInt(
					queue, hash,
					ast->hashValue, index,
					auxAST->children[0]->hashValue
				);
				index++;
			}
			for(;index<length;index++){
				queue = TACArrayAssignInt(
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

HashTable *TACExpression(ASTNode *ast, HashTable *hash, TACQueue **queue);
TACQueue *TACCommand(ASTNode *ast, HashTable *hash);

TACQueue *TACOperate(HashTable *hash, TACType operator, HashTable *result,
		ASTNode *op_left, ASTNode *op_right)
{
	TACQueue *queue=NULL;
	HashTable *hash_left;
	HashTable *hash_right;

	hash_left  = TACExpression(op_left, hash, &queue);
	hash_right = TACExpression(op_right, hash, &queue);

	queue = TACQueueInsert(
		queue,
		TACCreate(operator, result, hash_left, hash_right)
	);

	return queue;
}

HashTable *TACCall(ASTNode *ast, HashTable *hash, TACQueue **queue){
	HashTable *return_value = HashInsertTmp(hash, "return_value");
	ASTNode *args;
	char params[16];
	int arg_count=0;

	if(ast->children[0]!=NULL)
		for(args=ast->children[0]->children[0];args!=NULL;args=args->children[1]){
			arg_count++;
			*queue = TACQueueInsert(
				*queue,
				TACCreate(
					TAC_ARG,
					TACExpression(args->children[0], hash, queue),
					NULL,
					NULL
				)
			);
		}

	sprintf(params, "%d", arg_count);

	TACQueueInsert(
		*queue,
		TACCreate(
			TAC_CALL,
			return_value,
			ast->hashValue,
			hashInsert(hash, params, LIT_INTEGER)
		)
	);

	return return_value;
}

HashTable *TACExpression(ASTNode *ast, HashTable *hash, TACQueue **queue){
	HashTable *val=NULL;
	TACType operator = TAC_SYMBOL;

	if(ast==NULL)
		return NULL;

	switch(ast->type){
		case AST_great:
			operator = operator == TAC_SYMBOL ? TAC_GREAT : operator;
			/* no break */
		case AST_less:
			operator = operator == TAC_SYMBOL ? TAC_LESS  : operator;
			/* no break */
		case AST_le:
			operator = operator == TAC_SYMBOL ? TAC_LE    : operator;
			/* no break */
		case AST_ge:
			operator = operator == TAC_SYMBOL ? TAC_GE    : operator;
			/* no break */
		case AST_eq:
			operator = operator == TAC_SYMBOL ? TAC_EQ    : operator;
			/* no break */
		case AST_ne:
			operator = operator == TAC_SYMBOL ? TAC_NE    : operator;
			/* no break */
		case AST_and:
			operator = operator == TAC_SYMBOL ? TAC_AND   : operator;
			/* no break */
		case AST_or:
			operator = operator == TAC_SYMBOL ? TAC_OR    : operator;
			/* no break */
		case AST_add:
			operator = operator == TAC_SYMBOL ? TAC_ADD   : operator;
			/* no break */
		case AST_sub:
			operator = operator == TAC_SYMBOL ? TAC_SUB   : operator;
			/* no break */
		case AST_mult:
			operator = operator == TAC_SYMBOL ? TAC_MUL   : operator;
			/* no break */
		case AST_div:
			operator = operator == TAC_SYMBOL ? TAC_DIV   : operator;
			/* no break */
		case AST_address:
			operator = operator == TAC_SYMBOL ? TAC_ADDRESS : operator;
			/* no break */
		case AST_pointer:
			operator = operator == TAC_SYMBOL ? TAC_POINTER   : operator;
			/* no break */
		case AST_not:
			operator = operator == TAC_SYMBOL ? TAC_NOT : operator;
			val=HashInsertTmp(hash, "expression");
			*queue = TACQueueJoin(
				*queue,
				TACOperate(hash, operator, val, ast->children[0],ast->children[1])
			);
			break;
		case AST_identifier:
		case AST_lit_char:
		case AST_lit_int:
		case AST_lit_true:
		case AST_lit_false:
			val = ast->hashValue;
			break;
		case AST_call_ident:
			val = TACCall(ast, hash, queue);
			break;
		case AST_vet_ident:
			// TODO
			break;
		case AST_par_block:
			val = TACExpression(ast->children[1]->children[0], hash, queue);
			break;
		default:
			printf("Expression evaluation error. ast->type: %d\n", ast->type);
			exit(11);
			break;
	}

	return val;
}

TACQueue *TACLabel(HashTable *label){
	return TACQueueInsert(
		NULL,
		TACCreate(
			TAC_LABEL,
			label,
			NULL,
			NULL
		)
	);
}

TACQueue *TACJump(HashTable *label){
	return TACQueueInsert(
		NULL,
		TACCreate(
			TAC_JUMP,
			label,
			NULL,
			NULL
		)
	);
}

TACQueue *TACIf(ASTNode *ast, HashTable *hash){
	TACQueue *queue=NULL;
	HashTable *else_=HashInsertTmp(hash, "else");
	HashTable *finally_=HashInsertTmp(hash, "finally");

	// Test condition
	queue = TACQueueInsert(
		queue,
		TACCreate(
			TAC_IFZ,
			else_,
			TACExpression(ast->children[0], hash, &queue),
			NULL
		)
	);

	// Condition passed code

	queue = TACQueueJoin(
		queue,
		TACCommand(ast->children[1],hash)
	);

	queue = TACQueueJoin(queue, TACJump(finally_));

	// Else code
	queue = TACQueueJoin(queue, TACLabel(else_));
	queue = TACQueueJoin(
		queue,
		TACCommand(ast->children[2],hash)
	);

	queue = TACQueueJoin(queue, TACLabel(finally_));

	return queue;
}

TACQueue *TACLoop(ASTNode *ast, HashTable *hash){
	TACQueue *queue=NULL;
	HashTable *loop_start=HashInsertTmp(hash, "loop_start");
	HashTable *loop_end=HashInsertTmp(hash, "loop_end");

	queue = TACQueueJoin(queue, TACLabel(loop_start));

	// Test condition
	queue = TACQueueInsert(
		queue,
		TACCreate(
			TAC_IFZ,
			loop_end,
			TACExpression(ast->children[0], hash, &queue),
			NULL
		)
	);

	queue = TACQueueJoin(queue, TACJump(loop_start));
	queue = TACQueueJoin(queue, TACLabel(loop_end));

	return queue;
}


TACQueue *TACCommand(ASTNode *ast, HashTable *hash){
	TACQueue *queue=NULL;

	if(ast==NULL)
		return queue;

	ast = ast->children[0];

	switch(ast->type){
		case AST_com_block:
			for(ast=ast->children[0];ast!=NULL;ast=ast->children[1]){
				queue=TACQueueJoin(
					queue,
					TACCommand(ast->children[0], hash)
				);
			}
			break;
		case AST_attr_ident:
			queue = TACAssign(
				queue,
				ast->hashValue,
				TACExpression(ast->children[0], hash, &queue)
			);
			break;
		case AST_attr_array:
			//TODO
			/*queue = TACArrayAssignExpr(
				queue,hash,
				ast->hashValue,TACExpression(ast->children[0], hash, &queue),
				TACExpression(ast->children[1], hash, &queue)
			);*/
			break;
		case AST_input:
			queue = TACQueueInsert(
				queue,
				TACCreate(
					TAC_READ,
					ast->hashValue,
					NULL,
					NULL
				)
			);
			break;
		case AST_output:
			for(ast=ast->children[0];ast!=NULL;ast=ast->children[1]){
				queue = TACQueueInsert(
					queue,
					TACCreate(
						TAC_PRINT,
						ast->hashValue != NULL ? ast->hashValue : TACExpression(ast->children[0],hash,&queue),
						NULL,
						NULL
					)
				);
			}
			break;
		case AST_if_block:
			queue = TACIf(ast, hash);
			break;
		case AST_return:
			queue = TACQueueInsert(
				queue,
				TACCreate(
					TAC_RET,
					TACExpression(ast->children[0],hash,&queue),
					NULL,
					NULL
				)
			);
			break;
		case AST_loop_block:
			queue = TACLoop(ast, hash);
			break;
		default:
			exit(12);
			break;
	}

	return queue;
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
		NULL,
		TACCreate(
			TAC_BEGINFUN,
			NULL,
			NULL,
			NULL
		)
	);

	queue = TACQueueJoin(
		TACArgsDeclaration(ast->children[0], hash),
		TACQueueJoin(
			queue,
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
			exit(TAC_INVALID_ERROR);
			break;
	}

	return queue;
}

