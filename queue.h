#ifndef _QUEUE_H
#define _QUEUE_H

#include "tac.h"

typedef struct _TACQueue {
	TAC *tac;
	struct _TACQueue *next;
}TACQueue;

TACQueue *TACQueueInsert(TACQueue *queue, TAC *tac);

TACQueue *TACQueueJoin(TACQueue *queue1, TACQueue *queue2);

TACQueue *TACQueueInvert(TACQueue *queue);

TACQueue *TACQueuePrint(TACQueue *queue);

TACQueue *TACProgram(ASTNode *ast, HashTable *hash);

#endif
