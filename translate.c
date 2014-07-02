#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "util.h"

void outputTac(char *str, FILE *outputFile){
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

char *translateLabel(TACQueue *queue){
	char *str = calloc('\0', strlen(queue->tac->result->value)+2);
	strcpy(str, queue->tac->result->value);
	str[strlen(queue->tac->result->value)] = ':';

	return str;
}

char *translateEndFun(TACQueue *queue){

	char *str = calloc('\0', strlen(queue->tac->result->value)+6);
	strcpy(str, queue->tac->result->value);
	str[strlen(queue->tac->result->value)] = ' ';
	str[strlen(queue->tac->result->value)+1] = 'e';
	str[strlen(queue->tac->result->value)+2] = 'n';
	str[strlen(queue->tac->result->value)+3] = 'd';
	str[strlen(queue->tac->result->value)+4] = 'p';

	return str;
}

char *translateBeginFun(TACQueue *queue){
	char *str = calloc('\0', strlen(queue->tac->result->value)+12);
	strcpy(str, queue->tac->result->value);
	str[strlen(queue->tac->result->value)] = ' ';
		str[strlen(queue->tac->result->value)+1] = 'p';
		str[strlen(queue->tac->result->value)+2] = 'r';
		str[strlen(queue->tac->result->value)+3] = 'o';
		str[strlen(queue->tac->result->value)+4] = 'c';
		str[strlen(queue->tac->result->value)+5] = ' ';
		str[strlen(queue->tac->result->value)+6] = 'n';
		str[strlen(queue->tac->result->value)+7] = 'e';
		str[strlen(queue->tac->result->value)+8] = 'a';
		str[strlen(queue->tac->result->value)+9] = 'r';


	return str;
}

void translateTAC(TACQueue *queue, FILE *outputFile){
	switch(queue->tac->type){
		case TAC_SYMBOL: break; //TODO
		case TAC_MOVE: break; //TODO

		case TAC_GREAT: break; //TODO
		case TAC_LESS: break; //TODO
		case TAC_LE: break; //TODO
		case TAC_GE: break; //TODO
		case TAC_EQ: break; //TODO
		case TAC_NE: break; //TODO
		case TAC_AND: break; //TODO
		case TAC_OR: break; //TODO
		case TAC_ADD: break; //TODO
		case TAC_SUB: break; //TODO
		case TAC_MUL: break; //TODO
		case TAC_DIV: break; //TODO

		case TAC_ADDRESS: break; //TODO
		case TAC_POINTER: break; //TODO
		case TAC_NOT: break; //TODO

		case TAC_LABEL: outputTac(translateLabel(queue), outputFile); break;
		case TAC_BEGINFUN: outputTac(translateBeginFun(queue), outputFile); break;
		case TAC_ENDFUN: outputTac(translateEndFun(queue), outputFile); break;

		case TAC_IFZ: break; //TODO
		case TAC_JUMP: break; //TODO

		case TAC_CALL: break; //TODO
		case TAC_ARG: break; //TODO

		case TAC_RET: break; //TODO
		case TAC_PRINT: break; //TODO
		case TAC_READ: break; //TODO

		default: break;
	}
}

char *getTypeFromHash(HashTable *node){
	switch(node->type){
	case DT_BYTE:
	case DT_BOOL:
		return "DB";
	case DT_WORD:
		return "DW";
	default:
		//hashPrint(node);
		exit(75);
	}

}

void translateData(TACQueue *queue, FILE *outputFile) {
	char *aux = calloc('\0', 1024);
	char *value = queue->tac->result->value;
	int len = strlen(value);

	if(queue->tac->type != TAC_MOVE)
		exit(100);

	if(value[len-1] == ']'){
		// Vetores
		if(value[len-2] == '0' && value[len-3] == '['){
			sprintf(aux, "%s      %s   %s", strtok(value+2, "["), getTypeFromHash(queue->tac->result), queue->tac->op1->value);
		} else {
			strcpy(aux, "        ");
			strcpy(aux+8, getTypeFromHash(queue->tac->result));
			strcpy(aux+10, "    ");
			strcpy(aux+14, queue->tac->op1->value);
		}
	} else {
		// Não vetores
		sprintf(aux, "%s      %s   %s", value, getTypeFromHash(queue->tac->result), queue->tac->op1->value);
	}

	outputTac(aux, outputFile);
	//TODO
//	free(aux);
}

void createDataCode(TACQueue *queue, FILE *outputFile) {
	TACQueue *auxQueue;
	bool go=true;

	outputTac("assume cs:$$_CODIGO,ds:$$_DADOS,es:$$_DADOS,ss:$$_PILHA", outputFile);
	outputTac("", outputFile);
	outputTac("true      EQU    01H", outputFile);
	outputTac("false     EQU    00H", outputFile);
	outputTac("", outputFile);

	outputTac("$$_DADOS   segment", outputFile);
	for(auxQueue=queue;auxQueue!=NULL;auxQueue=auxQueue->next)
		switch(auxQueue->tac->type){
			case TAC_ENDFUN:
				go=true;
				break;
			case TAC_BEGINFUN:
				go=false;
				break;
			default:
				if(go)
					translateData(auxQueue, outputFile);
		}
	outputTac("$$_DADOS   ends", outputFile);

	outputTac("", outputFile);
	outputTac("$$_PILHA   segment", outputFile);
	outputTac("    dw     128 dup(?)", outputFile);
	outputTac("$$_PILHA   ends", outputFile);
	outputTac("", outputFile);


}

void createProgramCode(TACQueue *queue, FILE *outputFile) {
	TACQueue *auxQueue;
	bool go=false;

	outputTac("$$_CODIGO   segment", outputFile);
	outputTac("$$_BEGIN:", outputFile);

	outputTac("call main", outputFile);
	outputTac("mov ah, 4ch", outputFile);
	outputTac("int 21h", outputFile);

	for(auxQueue=queue;auxQueue!=NULL;auxQueue=auxQueue->next)
		switch(auxQueue->tac->type){
			case TAC_ENDFUN:
				translateTAC(auxQueue, outputFile);
				go=false;
				break;
			case TAC_BEGINFUN:
				go=true;
				/* No break */
			default:
				if(go)
					translateTAC(auxQueue, outputFile);
		}


	outputTac("$$_CODIGO   ends", outputFile);
	outputTac("end $$_BEGIN", outputFile);
}

void translate(TACQueue *queue, FILE *outputFile){
	createDataCode(queue, outputFile);
	createProgramCode(queue, outputFile);
}
