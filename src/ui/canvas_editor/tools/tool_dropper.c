#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"

void quixel_tool_dropper_logic(QUIXEL_CANVAS_EDITOR * cep, int button)
{
	if(button == 1)
	{
		cep->left_base_color = quixel_get_canvas_pixel(cep->canvas, cep->current_layer, cep->hover_x, cep->hover_y);
	}
	else
	{
		cep->right_base_color = quixel_get_canvas_pixel(cep->canvas, cep->current_layer, cep->hover_x, cep->hover_y);
	}
}
