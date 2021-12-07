#ifndef PA_QUEUE_H
#define PA_QUEUE_H

#include "t3f/t3f.h"

typedef struct
{

	int x;
	int y;

} PA_QUEUE_ITEM;

typedef struct
{

	void * previous;
	int x;
	int y;

} PA_QUEUE_NODE;

typedef struct
{

	char name[256];
	PA_QUEUE_NODE * current;

} PA_QUEUE;

PA_QUEUE * pa_create_queue(const char * name);
void pa_destroy_queue(PA_QUEUE * qp);
bool pa_queue_push(PA_QUEUE * qp, int x, int y);
bool pa_queue_pop(PA_QUEUE * qp, int * x, int * y);
int pa_get_queue_size(PA_QUEUE * qp);

#endif
