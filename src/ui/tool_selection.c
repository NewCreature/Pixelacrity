#include "t3f/t3f.h"
#include "canvas.h"
#include "primitives.h"
#include "canvas_editor.h"

void quixel_tool_selection_logic(QUIXEL_CANVAS_EDITOR * cep)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;
	float start_x, start_y, end_x, end_y;

	start_x = cep->click_x;
	start_y = cep->click_y;
	end_x = cep->hover_x;
	end_y = cep->hover_y;

	cep->selection.layer = cep->current_layer;
	cep->selection.x = start_x;
	cep->selection.y = start_y;
	cep->selection.width = end_x - start_x;
	cep->selection.height = end_y - start_y;
}

void quixel_tool_selection_render(QUIXEL_CANVAS_EDITOR * cep)
{
}
