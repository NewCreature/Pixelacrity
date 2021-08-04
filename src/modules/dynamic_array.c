#include "t3f/t3f.h"

void ** pa_malloc(int element_size, int elements)
{
	void ** p;
	int size;

	size = element_size * elements;
	p = malloc(size);
	if(p)
	{
		memset(p, 0, size);
	}
	return p;
}

void pa_free(void ** p, int elements)
{
	int i;

	if(p)
	{
		free(p);
	}
}
