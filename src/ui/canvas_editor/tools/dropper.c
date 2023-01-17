#include "ui/canvas_editor/canvas_editor.h"
#include "ui/element/canvas_editor/element.h"
#include "modules/canvas/canvas_helpers.h"

void pa_tool_dropper_logic(PA_CANVAS_EDITOR * cep, int button)
{
	if(button == 1)
	{
		cep->left_color.base_color = cep->hover_color;
		pa_set_color(&cep->left_color, cep->left_color.base_color);
		pa_canvas_editor_update_pick_colors(cep);
	}
	else
	{
		cep->right_color.base_color = cep->hover_color;
		pa_set_color(&cep->right_color, cep->right_color.base_color);
	}
}
