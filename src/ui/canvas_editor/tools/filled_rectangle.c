#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/primitives.h"
#include "modules/snap.h"
#include "ui/canvas_editor/canvas_editor.h"

void pa_tool_filled_rectangle_logic(PA_CANVAS_EDITOR * cep)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;

	cep->start_x = cep->click_x - cep->view_x;
	cep->start_y = cep->click_y - cep->view_y;
	cep->end_x = cep->hover_x - cep->view_x;
	cep->end_y = cep->hover_y - cep->view_y;

	/* apply constraint if user is pressing Control */
	if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL])
	{
		pa_snap_coordinates(cep->start_x, cep->start_y, &cep->end_x, &cep->end_y, 1, 0.0);
		pa_set_tool_boundaries(cep, cep->start_x + cep->view_x, cep->start_y + cep->view_y, cep->end_x + cep->view_x, cep->end_y + cep->view_y);
	}

	cep->scratch_offset_x = cep->view_x;
	cep->scratch_offset_y = cep->view_y;
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(cep->tool_bitmap);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	pa_render_canvas_layer(cep->canvas, cep->current_layer, cep->view_x, cep->view_y, 0, t3f_color_white, 1, 0, 0, cep->editor_element->w, cep->editor_element->h);
	pa_draw_filled_rectangle(cep->start_x, cep->start_y, cep->end_x, cep->end_y, NULL, cep->click_color, NULL);
	pa_draw_rectangle(cep->start_x, cep->start_y, cep->end_x, cep->end_y, NULL, cep->click_color, NULL);
	if(cep->tool_outline)
	{
		pa_draw_rectangle(cep->start_x, cep->start_y, cep->end_x, cep->end_y, cep->brush, cep->click_outline_color, NULL);
	}
	al_restore_state(&old_state);
}

void pa_tool_filled_rectangle_render(PA_CANVAS_EDITOR * cep)
{
}
