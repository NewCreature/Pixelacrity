#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/ui.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/tools/tool_selection.h"

void pa_canvas_editor_MSG_DRAW(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	T3GUI_THEME * theme;
	ALLEGRO_COLOR color = t3f_color_black;
	int tx, ty;

	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int i;

	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_identity_transform(&identity);
	al_use_transform(&identity);

	if(canvas_editor->selection.layer < 0 && canvas_editor->selection.bitmap)
	{
		for(i = 0; i < canvas_editor->canvas->layer_max; i++)
		{
			pa_tool_selection_render_layer(canvas_editor, i);
			al_use_shader(canvas_editor->premultiplied_alpha_shader);
			al_draw_bitmap(canvas_editor->scratch_bitmap, d->x, d->y, 0);
		}
	}
	else
	{
		/* render background layers */
		if(!canvas_editor->view_isolate)
		{
			for(i = 0; i < canvas_editor->current_layer; i++)
			{
				pa_render_canvas_layer(canvas_editor->canvas, i, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
			}
		}

		if(canvas_editor->selection.bitmap)
		{
			pa_tool_selection_render_layer(canvas_editor, canvas_editor->current_layer);
			al_draw_bitmap(canvas_editor->scratch_bitmap, d->x, d->y, 0);
		}
		else if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING)
		{
			al_draw_scaled_bitmap(canvas_editor->scratch_bitmap, 0, 0,  al_get_bitmap_width(canvas_editor->scratch_bitmap), al_get_bitmap_height(canvas_editor->scratch_bitmap), d->x - (canvas_editor->view_x - canvas_editor->scratch_offset_x) * canvas_editor->view_zoom, d->y - (canvas_editor->view_y - canvas_editor->scratch_offset_y) * canvas_editor->view_zoom, al_get_bitmap_width(canvas_editor->scratch_bitmap) * canvas_editor->view_zoom, al_get_bitmap_height(canvas_editor->scratch_bitmap) * canvas_editor->view_zoom, 0);
		}
		else
		{
			pa_render_canvas_layer(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
		}

		/* draw foreground layers */
		if(!canvas_editor->view_isolate)
		{
			for(i = canvas_editor->current_layer + 1; i < canvas_editor->canvas->layer_max; i++)
			{
				pa_render_canvas_layer(canvas_editor->canvas, i, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
			}
		}
	}

	/* render frames */
	theme = t3gui_get_default_theme();
	for(i = 0; i < canvas_editor->canvas->frame_max; i++)
	{
		if(i == canvas_editor->current_frame)
		{
			color = t3f_color_black;
		}
		else
		{
			color = al_map_rgba_f(0.0, 0.0, 0.0, 0.25);
		}
		if(theme)
		{
			al_draw_text(theme->state[0].font[0], color, d->x + (canvas_editor->canvas->frame[i]->box.start_x - canvas_editor->view_x) * canvas_editor->view_zoom, d->y + (canvas_editor->canvas->frame[i]->box.start_y - canvas_editor->view_y) * canvas_editor->view_zoom - al_get_font_line_height(theme->state[0].font[0]) - PA_UI_ELEMENT_SPACE, 0, canvas_editor->canvas->frame[i]->name);
		}
		al_draw_rectangle(d->x + (canvas_editor->canvas->frame[i]->box.start_x - canvas_editor->view_x) * canvas_editor->view_zoom - 1.0 + 0.5, d->y + (canvas_editor->canvas->frame[i]->box.start_y - canvas_editor->view_y) * canvas_editor->view_zoom - 1.0 + 0.5, d->x + (canvas_editor->canvas->frame[i]->box.start_x + canvas_editor->canvas->frame[i]->box.width - canvas_editor->view_x) * canvas_editor->view_zoom + 0.5, d->y + (canvas_editor->canvas->frame[i]->box.start_y + canvas_editor->canvas->frame[i]->box.height - canvas_editor->view_y) * canvas_editor->view_zoom + 0.5, color, 1.0);
	}

	/* render selection box */
	if(canvas_editor->selection.box.width > 0 && canvas_editor->selection.box.height > 0)
	{
		pa_box_render(&canvas_editor->selection.box, 0, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y);
		if(canvas_editor->selection.bitmap)
		{
			if(canvas_editor->selection.layer < 0)
			{
				al_draw_bitmap(canvas_editor->multilayer_bitmap, d->x + canvas_editor->selection.box.start_x * canvas_editor->view_zoom - canvas_editor->view_x * canvas_editor->view_zoom - 24, d->y + canvas_editor->selection.box.start_y * canvas_editor->view_zoom - canvas_editor->view_y * canvas_editor->view_zoom - 24, 0);
			}
			else
			{
				al_draw_bitmap(canvas_editor->singlelayer_bitmap, d->x + canvas_editor->selection.box.start_x * canvas_editor->view_zoom - canvas_editor->view_x * canvas_editor->view_zoom - 24, d->y + canvas_editor->selection.box.start_y * canvas_editor->view_zoom - canvas_editor->view_y * canvas_editor->view_zoom - 24, 0);
			}
		}
	}

	if(t3f_key[ALLEGRO_KEY_G])
	{
		tx = ((int)(t3f_mouse_x - d->x) / canvas_editor->view_zoom) * canvas_editor->view_zoom + canvas_editor->view_zoom / 2 + d->x;
		ty = ((int)(t3f_mouse_y - d->y) / canvas_editor->view_zoom) * canvas_editor->view_zoom + canvas_editor->view_zoom / 2 + d->y;
		al_draw_line(d->x, ty + 0.5, d->x + d->w, ty + 0.5, t3f_color_black, 0.0);
		al_draw_line(tx + 0.5, d->y, tx + 0/5, d->y + d->h, t3f_color_black, 0.0);
	}
	al_restore_state(&old_state);
}
