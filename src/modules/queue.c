#include "queue.h"

void queue_insert(QUIXEL_QUEUE * qp, int x, int y)
{
	QUIXEL_QUEUE_NODE * node = malloc(sizeof(QUIXEL_QUEUE_NODE));
	if(node)
	{
		memset(node, 0, sizeof(QUIXEL_QUEUE_NODE));
		node->x = x;
		node->y = y;
		node->previous = qp->current;
		qp->current = node;
	}
	else
	{
		printf("Error allocating node!\n");
	}
}

void queue_delete(QUIXEL_QUEUE * qp)
{
	QUIXEL_QUEUE_NODE * temp = NULL;
	if(qp->current)
	{
		temp = qp->current->previous;
		free(qp->current);
		qp->current = temp;
	}
}

QUIXEL_QUEUE * quixel_create_queue(void)
{
	QUIXEL_QUEUE * qp;

	qp = malloc(sizeof(QUIXEL_QUEUE));
	if(qp)
	{
		memset(qp, 0, sizeof(QUIXEL_QUEUE));
	}
	return qp;
}

void quixel_destroy_queue(QUIXEL_QUEUE * qp)
{
	while(qp->current)
	{
		queue_delete(qp);
	}
	free(qp);
}

void quixel_queue_push(QUIXEL_QUEUE * qp, int x, int y)
{
	queue_insert(qp, x, y);
}

bool quixel_queue_pop(QUIXEL_QUEUE * qp, int * x, int * y)
{
	if(qp->current)
	{
		*x = qp->current->x;
		*y = qp->current->y;
		queue_delete(qp);
		return true;
	}
	return false;
}
