#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"

static char list_item_name_buffer[64] = {0};

const char * quixel_layer_list_proc(int index, int * num_elem, void * dp3)
{
	QUIXEL_CANVAS * canvas = (QUIXEL_CANVAS *)dp3;

	if(index < 0)
	{
		if(canvas)
		{
			*num_elem = canvas->layer_max;
		}
		else
		{
			*num_elem = 0;
		}
		return NULL;
	}
	else
	{
		if(canvas)
		{
			sprintf(list_item_name_buffer, "Layer %d", index + 1);
		}
		else
		{
			strcpy(list_item_name_buffer, "");
		}
		return list_item_name_buffer;
	}
}
