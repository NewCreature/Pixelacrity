#ifndef QUIXEL_QUEUE_H
#define QUIXEL_QUEUE_H

#include "t3f/t3f.h"

typedef struct
{

	int x;
	int y;

} QUIXEL_QUEUE_ITEM;

typedef struct
{

	void * previous;
	int x;
	int y;

} QUIXEL_QUEUE_NODE;

typedef struct
{

	QUIXEL_QUEUE_NODE * current;

} QUIXEL_QUEUE;

QUIXEL_QUEUE * quixel_create_queue(void);
void quixel_destroy_queue(QUIXEL_QUEUE * qp);
bool quixel_queue_push(QUIXEL_QUEUE * qp, int x, int y);
bool quixel_queue_pop(QUIXEL_QUEUE * qp, int * x, int * y);

#endif
