#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/canvas/canvas_helpers.h"
#include "ui/ui.h"
#include "ui/canvas_editor/canvas_editor.h"
#include "ui/canvas_editor/tools/tool_selection.h"

static void generate_brush_hint(PA_CANVAS_EDITOR * cep)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int w, h;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(cep->scratch_bitmap);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
	al_use_shader(cep->conditional_copy_shader);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	w = al_get_bitmap_width(cep->brush);
	h = al_get_bitmap_height(cep->brush);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 1.0), 0, 0, w, h, 0.0, 0.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 1.0), 0, 0, w, h, 1.0, 0.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 1.0), 0, 0, w, h, 2.0, 0.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 1.0), 0, 0, w, h, 0.0, 1.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 1.0), 0, 0, w, h, 2.0, 1.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 1.0), 0, 0, w, h, 0.0, 2.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 1.0), 0, 0, w, h, 1.0, 2.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 1.0), 0, 0, w, h, 2.0, 2.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	al_draw_tinted_scaled_bitmap(cep->brush, al_map_rgba_f(0.0, 0.0, 0.0, 0.0), 0, 0, w, h, 1.0, 1.0, w * cep->view_zoom, h * cep->view_zoom, 0);
	al_restore_state(&old_state);
}

static void draw_grid(int ox, int oy, int width, int height, int space, ALLEGRO_COLOR color)
{
	int i;

	for(i = 0; i < height; i += space)
	{
		al_draw_line(ox + 0.5, oy + i + 0.5, ox + width + 0.5, oy + i + 0.5, color, 0.0);
	}
	for(i = 0; i < width; i += space)
	{
		al_draw_line(ox + i + 0.5, oy + 0.5, ox + i + 0.5, oy + height + 0.5, color, 0.0);
	}
}

void pa_canvas_editor_MSG_DRAW(T3GUI_ELEMENT * d, int c)
{
	PA_CANVAS_EDITOR * canvas_editor = (PA_CANVAS_EDITOR *)d->dp;
	T3GUI_THEME * theme;
	ALLEGRO_COLOR color = t3f_color_black;
	ALLEGRO_COLOR title_color = t3f_color_black;
	int tx, ty;
	ALLEGRO_BITMAP * peg_bitmap;
	char buf[256];
	int current_z;
	int vz = canvas_editor->view_break_out_frame;
	ALLEGRO_COLOR current_color;

	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int i;

	t3f_select_view(canvas_editor->view);
	al_set_clipping_rectangle(d->x, d->y, d->w, d->h);
	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_BLENDER);
	al_identity_transform(&identity);
	al_use_transform(&identity);

	if(canvas_editor->selection.layer < 0 && canvas_editor->selection.bitmap_stack)
	{
		for(i = 0; i < canvas_editor->canvas->layer_max; i++)
		{
			pa_tool_selection_render_layer(canvas_editor, i);
			al_use_shader(canvas_editor->premultiplied_alpha_shader);
			al_draw_bitmap(canvas_editor->tool_bitmap, d->x, d->y, 0);
		}
	}
	else
	{
		if(canvas_editor->view_break_out)
		{
			current_z = (canvas_editor->view_zoom * vz) * (canvas_editor->current_layer + 1);
		}
		else
		{
			current_z = 0;
		}

		/* render background layers */
		if(!canvas_editor->view_isolate)
		{
			for(i = 0; i < canvas_editor->current_layer; i++)
			{
				current_color = t3f_color_white;
				if(canvas_editor->view_break_out)
				{
					current_z -= vz * canvas_editor->view_zoom;
					if(i != canvas_editor->current_layer)
					{
						current_color = al_map_rgba_f(1.0, 1.0, 1.0, 0.5);
					}
				}
				pa_render_canvas_layer(canvas_editor->canvas, i, canvas_editor->view_x, canvas_editor->view_y, current_z, current_color, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
			}
		}

		current_color = t3f_color_white;
		if(canvas_editor->view_break_out)
		{
			current_z -= vz * canvas_editor->view_zoom;
		}
		if(canvas_editor->selection.bitmap_stack)
		{
			pa_tool_selection_render_layer(canvas_editor, canvas_editor->current_layer);
			al_use_shader(canvas_editor->premultiplied_alpha_shader);
			al_draw_bitmap(canvas_editor->tool_bitmap, d->x, d->y, 0);
		}
		else if(canvas_editor->tool_state == PA_TOOL_STATE_DRAWING)
		{
			al_draw_scaled_bitmap(canvas_editor->tool_bitmap, 0, 0,  al_get_bitmap_width(canvas_editor->tool_bitmap), al_get_bitmap_height(canvas_editor->tool_bitmap), d->x - (canvas_editor->view_x - canvas_editor->scratch_offset_x) * canvas_editor->view_zoom, d->y - (canvas_editor->view_y - canvas_editor->scratch_offset_y) * canvas_editor->view_zoom, al_get_bitmap_width(canvas_editor->tool_bitmap) * canvas_editor->view_zoom, al_get_bitmap_height(canvas_editor->tool_bitmap) * canvas_editor->view_zoom, 0);
		}
		else
		{
			pa_render_canvas_layer(canvas_editor->canvas, canvas_editor->current_layer, canvas_editor->view_x, canvas_editor->view_y, current_z, current_color, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
		}

		/* draw foreground layers */
		if(!canvas_editor->view_isolate)
		{
			for(i = canvas_editor->current_layer + 1; i < canvas_editor->canvas->layer_max; i++)
			{
				current_color = t3f_color_white;
				if(canvas_editor->view_break_out)
				{
					current_z -= vz * canvas_editor->view_zoom;
					if(i != canvas_editor->current_layer)
					{
						current_color = al_map_rgba_f(1.0, 1.0, 1.0, 0.5);
					}
				}
				pa_render_canvas_layer(canvas_editor->canvas, i, canvas_editor->view_x, canvas_editor->view_y, current_z, current_color, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
			}
		}
		for(i = 0; i < PA_MAX_GRIDS; i++)
		{
			if(canvas_editor->grid[i].space * canvas_editor->view_zoom > 1)
			{
				draw_grid(d->x, d->y, d->w, d->h, canvas_editor->grid[i].space * canvas_editor->view_zoom, canvas_editor->grid[i].color);
			}
		}
		if(canvas_editor->current_tool != PA_TOOL_DROPPER && canvas_editor->current_tool != PA_TOOL_FLOOD_FILL && canvas_editor->current_tool != PA_TOOL_SELECTION && canvas_editor->current_tool != PA_TOOL_FRAME)
		{
			generate_brush_hint(canvas_editor);
			al_draw_bitmap(canvas_editor->scratch_bitmap, ((int)(t3gui_get_mouse_x() - d->x) / canvas_editor->view_zoom) * canvas_editor->view_zoom + d->x - (al_get_bitmap_width(canvas_editor->brush) / 2) * canvas_editor->view_zoom - 1.0, ((int)(t3gui_get_mouse_y() - d->y) / canvas_editor->view_zoom) * canvas_editor->view_zoom + d->y - (al_get_bitmap_height(canvas_editor->brush) / 2) * canvas_editor->view_zoom - 1.0, 0);
		}
	}

	/* render frames */
	theme = t3gui_get_default_theme();
	if(canvas_editor->current_tool == PA_TOOL_FRAME)
	{
		peg_bitmap = canvas_editor->peg_bitmap;
	}
	else
	{
		peg_bitmap = NULL;
	}
	for(i = 0; i < canvas_editor->canvas->frame_max; i++)
	{
		if(i == canvas_editor->current_frame && canvas_editor->current_tool != PA_TOOL_FRAME)
		{
			color = t3f_color_black;
		}
		else
		{
			if(i == canvas_editor->hover_frame || i == canvas_editor->current_frame)
			{
				color = t3f_color_black;
			}
			else
			{
				color = al_map_rgba_f(0.0, 0.0, 0.0, 0.25);
			}
			if(i == canvas_editor->current_frame)
			{
				title_color = t3f_color_black;
			}
			else
			{
				title_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.25);
			}
		}
		if(theme)
		{
			sprintf(buf, "%s (%dx%d)", canvas_editor->canvas->frame[i]->name, canvas_editor->canvas->frame[i]->box.width, canvas_editor->canvas->frame[i]->box.height);
			al_draw_text(theme->state[0].font[0], title_color, d->x + (canvas_editor->canvas->frame[i]->box.start_x - canvas_editor->view_x) * canvas_editor->view_zoom, d->y + (canvas_editor->canvas->frame[i]->box.start_y - canvas_editor->view_y) * canvas_editor->view_zoom - al_get_font_line_height(theme->state[0].font[0]) - PA_UI_ELEMENT_SPACE, 0, buf);
		}
		pa_box_render(&canvas_editor->canvas->frame[i]->box, 0, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, color, peg_bitmap);
	}

	/* render selection box */
	if(canvas_editor->selection.box.width > 0 && canvas_editor->selection.box.height > 0)
	{
		pa_box_render(&canvas_editor->selection.box, 0, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, t3f_color_black, canvas_editor->peg_bitmap);
		if(canvas_editor->selection.bitmap_stack)
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
		tx = ((int)(t3gui_get_mouse_x() - d->x) / canvas_editor->view_zoom) * canvas_editor->view_zoom + canvas_editor->view_zoom / 2 + d->x;
		ty = ((int)(t3gui_get_mouse_y() - d->y) / canvas_editor->view_zoom) * canvas_editor->view_zoom + canvas_editor->view_zoom / 2 + d->y;
		al_draw_line(d->x, ty + 0.5, d->x + d->w, ty + 0.5, t3f_color_black, 0.0);
		al_draw_line(tx + 0.5, d->y, tx + 0/5, d->y + d->h, t3f_color_black, 0.0);
	}
	al_restore_state(&old_state);
}
