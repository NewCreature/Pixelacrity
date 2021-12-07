#include "queue.h"

bool queue_insert(PA_QUEUE * qp, int x, int y)
{
	PA_QUEUE_NODE * node = malloc(sizeof(PA_QUEUE_NODE));
	if(node)
	{
		memset(node, 0, sizeof(PA_QUEUE_NODE));
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

void queue_delete(PA_QUEUE * qp)
{
	PA_QUEUE_NODE * temp = NULL;
	if(qp->current)
	{
		temp = qp->current->previous;
		free(qp->current);
		qp->current = temp;
	}
}

PA_QUEUE * pa_create_queue(const char * name)
{
	PA_QUEUE * qp;

	qp = malloc(sizeof(PA_QUEUE));
	if(qp)
	{
		memset(qp, 0, sizeof(PA_QUEUE));
		if(name)
		{
			strcpy(qp->name, name);
		}
	}
	return qp;
}

void pa_destroy_queue(PA_QUEUE * qp)
{
	while(qp->current)
	{
		queue_delete(qp);
	}
	free(qp);
}

bool pa_queue_push(PA_QUEUE * qp, int x, int y)
{
	return queue_insert(qp, x, y);
}

bool pa_queue_pop(PA_QUEUE * qp, int * x, int * y)
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

int pa_get_queue_size(PA_QUEUE * qp)
{
	PA_QUEUE_NODE * current_node;
	int size = 0;

	current_node = qp->current;
	while(current_node)
	{
		size++;
		current_node = current_node->previous;
	}
	return size;
}
