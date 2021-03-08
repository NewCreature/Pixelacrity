#include "t3f/t3f.h"
#include "canvas.h"
#include "primitives.h"
#include "canvas_editor.h"

void quixel_tool_selection_logic(QUIXEL_CANVAS_EDITOR * cep)
{
	int start_x, start_y, end_x, end_y;

	start_x = cep->click_x;
	start_y = cep->click_y;
	end_x = cep->hover_x;
	end_y = cep->hover_y;

	quixel_sort_coordinates(&start_x, &end_x);
	quixel_sort_coordinates(&start_y, &end_y);
	cep->selection.layer = cep->current_layer;
	quixel_initialize_box(&cep->selection.box, start_x, start_y, end_x - start_x + 1, end_y - start_y + 1, cep->peg_bitmap);
}

void quixel_tool_selection_render(QUIXEL_CANVAS_EDITOR * cep)
{
}
