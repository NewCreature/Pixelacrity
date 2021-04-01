#include "t3f/t3f.h"
#include "canvas.h"
#include "primitives.h"
#include "canvas_editor.h"

void quixel_tool_line_logic(QUIXEL_CANVAS_EDITOR * cep)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;
	float start_x, start_y, end_x, end_y;

	start_x = cep->click_x - cep->view_x;
	start_y = cep->click_y - cep->view_y;
	end_x = cep->hover_x - cep->view_x;
	end_y = cep->hover_y - cep->view_y;

	cep->scratch_offset_x = cep->view_x;
	cep->scratch_offset_y = cep->view_y;
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(cep->scratch_bitmap);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	quixel_render_canvas_layer(cep->canvas, cep->current_layer, cep->view_x, cep->view_y, 1, 0, 0, cep->editor_element->w, cep->editor_element->h);
	quixel_draw_line(start_x, start_y, end_x, end_y, NULL, cep->click_color);
	al_restore_state(&old_state);
}

void quixel_tool_line_render(QUIXEL_CANVAS_EDITOR * cep)
{
}
