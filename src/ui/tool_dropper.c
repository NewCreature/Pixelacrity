#include "canvas_editor.h"

void quixel_tool_dropper_logic(QUIXEL_CANVAS_EDITOR * cep)
{
	int tx, ty;

	tx = cep->hover_x / cep->canvas->bitmap_size;
	ty = cep->hover_y / cep->canvas->bitmap_size;

	if(cep->canvas->layer[cep->current_layer]->bitmap[ty][tx])
	{
		cep->base_color = al_get_pixel(cep->canvas->layer[cep->current_layer]->bitmap[ty][tx], cep->hover_x % cep->canvas->bitmap_size, cep->hover_y % cep->canvas->bitmap_size);
	}
	else
	{
		cep->base_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
	}
}
