#include "canvas_editor.h"
#include "canvas_helpers.h"

void quixel_tool_dropper_logic(QUIXEL_CANVAS_EDITOR * cep)
{
	cep->base_color = quixel_get_canvas_pixel(cep->canvas, cep->current_layer, cep->hover_x, cep->hover_y);
}
