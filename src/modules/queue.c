#include "queue.h"

bool queue_insert(QUIXEL_QUEUE * qp, int x, int y)
{
	QUIXEL_QUEUE_NODE * node = malloc(sizeof(QUIXEL_QUEUE_NODE));
	if(node)
	{
		memset(node, 0, sizeof(QUIXEL_QUEUE_NODE));
		node->x = x;
		node->y = y;
		node->previous = qp->current;
		qp->current = node;
		return true;
	}
	else
	{
		printf("Error allocating node!\n");
		return false;
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

bool quixel_queue_push(QUIXEL_QUEUE * qp, int x, int y)
{
	return queue_insert(qp, x, y);
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

int quixel_get_queue_size(QUIXEL_QUEUE * qp)
{
	QUIXEL_QUEUE_NODE * current_node;
	int size = 0;

	current_node = qp->current;
	while(current_node)
	{
		size++;
		current_node = current_node->previous;
	}
	return size;
}
