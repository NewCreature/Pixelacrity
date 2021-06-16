#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "modules/combine_bitmap.h"
#include "ui/canvas_editor/canvas_editor.h"

/*void quixel_tool_selection_logic(QUIXEL_CANVAS_EDITOR * cep)
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
} */

void quixel_tool_selection_logic(QUIXEL_CANVAS_EDITOR * cep)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;
	int current_x, current_y;
	int old_x, old_y;
	float start_x, start_y, end_x, end_y;
	int count = 0;

	cep->scratch_offset_x = cep->view_x;
	cep->scratch_offset_y = cep->view_y;
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(cep->scratch_bitmap);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	quixel_render_canvas_layer(cep->canvas, cep->current_layer, cep->view_x, cep->view_y, 1, 0, 0, cep->editor_element->w, cep->editor_element->h);

	quixel_render_canvas_to_bitmap(cep->canvas, 0, cep->current_layer + 1, cep->selection.box.start_x, cep->selection.box.start_y, cep->selection.box.width, cep->selection.box.height, 0, cep->selection.combined_bitmap);
	pa_combine_bitmap(cep->selection.bitmap, cep->selection.combined_bitmap);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_draw_scaled_bitmap(cep->selection.combined_bitmap, 0, 0,  cep->selection.box.width, cep->selection.box.height, cep->selection.box.start_x - cep->view_x, cep->selection.box.start_y - cep->view_y, cep->selection.box.width, cep->selection.box.height, 0);

	al_restore_state(&old_state);
}
