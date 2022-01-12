#include "t3f/t3f.h"
#include "modules/canvas/canvas.h"
#include "modules/canvas/canvas_helpers.h"
#include "modules/primitives.h"
#include "ui/canvas_editor/canvas_editor.h"

/*void pa_tool_selection_logic(PA_CANVAS_EDITOR * cep)
{
	int start_x, start_y, end_x, end_y;

	start_x = cep->click_x;
	start_y = cep->click_y;
	end_x = cep->hover_x;
	end_y = cep->hover_y;

	pa_sort_coordinates(&start_x, &end_x);
	pa_sort_coordinates(&start_y, &end_y);
	cep->selection.layer = cep->current_layer;
	pa_initialize_box(&cep->selection.box, start_x, start_y, end_x - start_x + 1, end_y - start_y + 1, cep->peg_bitmap);
} */

void pa_tool_selection_render_layer_preview(PA_CANVAS_EDITOR * cep, int layer, ALLEGRO_BITMAP * scratch, int * offset_x, int * offset_y)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;
	float cx, cy;
	float rx, ry;
	int px, py;

	al_store_state(&old_state, ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	if(cep->selection.bitmap_stack->bitmap[layer])
	{
		al_set_target_bitmap(scratch);
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		al_identity_transform(&identity);
		al_use_transform(&identity);
		cx = al_get_bitmap_width(cep->selection.bitmap_stack->bitmap[layer]) / 2.0;
		cy = al_get_bitmap_height(cep->selection.bitmap_stack->bitmap[layer]) / 2.0;
		rx = (float)cep->selection.box.width / (float)al_get_bitmap_width(cep->selection.bitmap_stack->bitmap[layer]);
		ry = (float)cep->selection.box.height / (float)al_get_bitmap_height(cep->selection.bitmap_stack->bitmap[layer]);
		px = cx * rx * 2.0;
		py = cy * ry * 2.0;
		if(px > py)
		{
			if(offset_y)
			{
				*offset_y = px - py;
			}
			py = px;
		}
		else
		{
			if(offset_x)
			{
				*offset_x = py - px;
			}
			px = py;
		}
		al_draw_scaled_rotated_bitmap(cep->selection.bitmap_stack->bitmap[layer], cx, cy, px, py, rx, ry, cep->selection.box.angle, 0);
	}
	al_restore_state(&old_state);
}

void pa_tool_selection_render_layer(PA_CANVAS_EDITOR * cep, int layer)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;
	float cx, cy;
	float rx, ry;
	int px, py;
	int offset_x = 0, offset_y = 0;
	int preview_size;
	int i;

	al_store_state(&old_state, ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_target_bitmap(cep->tool_bitmap);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	al_identity_transform(&identity);
	al_use_transform(&identity);
	cep->scratch_offset_x = cep->view_x;
	cep->scratch_offset_y = cep->view_y;
	pa_render_canvas_layer(cep->canvas, layer, cep->view_x, cep->view_y, 0, t3f_color_white, cep->view_zoom, 0, 0, cep->editor_element->w, cep->editor_element->h);

	al_use_shader(cep->conditional_copy_shader);
	i = cep->selection.layer < 0 ? layer : cep->selection.layer;
	if(cep->selection.bitmap_stack->bitmap[i])
	{
		cx = al_get_bitmap_width(cep->selection.bitmap_stack->bitmap[layer]) / 2.0;
		cy = al_get_bitmap_height(cep->selection.bitmap_stack->bitmap[layer]) / 2.0;
		rx = (float)cep->selection.box.width / (float)al_get_bitmap_width(cep->selection.bitmap_stack->bitmap[layer]);
		ry = (float)cep->selection.box.height / (float)al_get_bitmap_height(cep->selection.bitmap_stack->bitmap[layer]);
		px = cx * rx;
		py = cy * ry + 0.5;
		preview_size = cep->selection.box.width * 2;
		if(cep->selection.box.height * 2 > preview_size)
		{
			preview_size = cep->selection.box.height * 2;
		}
		pa_tool_selection_render_layer_preview(cep, i, cep->scratch_bitmap, &offset_x, &offset_y);
		al_set_target_bitmap(cep->tool_bitmap);
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		al_identity_transform(&identity);
		al_use_transform(&identity);
		al_draw_scaled_bitmap(cep->scratch_bitmap, 0, 0, preview_size, preview_size, (cep->selection.box.start_x - cep->view_x - px - offset_x) * cep->view_zoom, (cep->selection.box.start_y - cep->view_y - py - offset_y) * cep->view_zoom, preview_size * cep->view_zoom, preview_size * cep->view_zoom, 0);
	}
	al_restore_state(&old_state);
	al_use_shader(cep->standard_shader);
}
