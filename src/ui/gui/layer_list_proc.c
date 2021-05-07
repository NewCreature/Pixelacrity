#include "t3f/t3f.h"
#include "ui/canvas_editor/canvas_editor.h"

static char list_item_name_buffer[64] = {0};

const char * quixel_layer_list_proc(int index, int * num_elem, void * dp3)
{
	QUIXEL_CANVAS_EDITOR * canvas_editor = (QUIXEL_CANVAS_EDITOR *)dp3;

	if(index < 0)
	{
		if(canvas_editor->canvas)
		{
			*num_elem = canvas_editor->canvas->layer_max;
		}
		else
		{
			*num_elem = 0;
		}
		return NULL;
	}
	else
	{
		if(canvas_editor->canvas)
		{
			if(index == canvas_editor->current_layer)
			{
				sprintf(list_item_name_buffer, "[Layer %d]", index + 1);
			}
			else
			{
				sprintf(list_item_name_buffer, "Layer %d", index + 1);
			}
		}
		else
		{
			strcpy(list_item_name_buffer, "");
		}
		return list_item_name_buffer;
	}
}
