#include "ui/canvas_editor/canvas_editor.h"
#include "ui/element/canvas_editor/element.h"
#include "modules/canvas/canvas_helpers.h"

static ALLEGRO_COLOR get_apparent_color(PA_CANVAS_EDITOR * cep, int x, int y)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_COLOR c;

	bp = al_create_bitmap(1, 1);
	if(bp)
	{
		pa_render_canvas_to_bitmap(cep->canvas, 0, cep->canvas->layer_max, x, y, 1, 1, 0, bp, cep->premultiplied_alpha_shader);
		c = al_get_pixel(bp, 0, 0);
		al_destroy_bitmap(bp);
		return c;
	}
	return al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
}

void pa_tool_dropper_logic(PA_CANVAS_EDITOR * cep, int button)
{
	ALLEGRO_COLOR c;

	if(t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT])
	{
		c = get_apparent_color(cep, cep->hover_x, cep->hover_y);
	}
	else
	{
		c = pa_get_canvas_pixel(cep->canvas, cep->current_layer, cep->hover_x, cep->hover_y, NULL, NULL);
	}

	if(button == 1)
	{
		cep->left_color.base_color = c;
		pa_set_color(&cep->left_color, cep->left_color.base_color);
		pa_canvas_editor_update_pick_colors(cep);
	}
	else
	{
		cep->right_color.base_color = c;
		pa_set_color(&cep->right_color, cep->right_color.base_color);
	}
}
