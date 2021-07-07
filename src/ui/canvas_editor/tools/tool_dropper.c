#include "ui/canvas_editor/canvas_editor.h"
#include "modules/canvas/canvas_helpers.h"

void pa_tool_dropper_logic(PA_CANVAS_EDITOR * cep, int button)
{
	if(button == 1)
	{
		cep->left_base_color = pa_get_canvas_pixel(cep->canvas, cep->current_layer, cep->hover_x, cep->hover_y);
		cep->left_color = cep->left_base_color;
	}
	else
	{
		cep->right_base_color = pa_get_canvas_pixel(cep->canvas, cep->current_layer, cep->hover_x, cep->hover_y);
		cep->right_color = cep->right_base_color;
	}
}
