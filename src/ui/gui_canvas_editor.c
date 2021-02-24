#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_editor.h"
#include "gui_canvas_editor.h"
#include "tool_pixel.h"

static ALLEGRO_COLOR shade_color(ALLEGRO_COLOR color, float l)
{
	float h, s, old_l;
	float r, g, b;

	al_unmap_rgb_f(color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &old_l);

	return al_color_hsl(h, s, l);
}

static ALLEGRO_COLOR alpha_color(ALLEGRO_COLOR color, float alpha)
{
	float r, g, b;

	al_unmap_rgb_f(color, &r, &g, &b);

	return al_map_rgba_f(r, g, b, alpha);
}

void quixel_canvas_editor_update_pick_colors(QUIXEL_CANVAS_EDITOR * cep)
{
	float new_l, step;
	int i;

	step = 1.0 / (float)(QUIXEL_COLOR_PICKER_SHADES - 1);
	for(i = 0; i < QUIXEL_COLOR_PICKER_SHADES; i++)
	{
		new_l = step * (float)i;
		cep->pick_color[i] = shade_color(cep->base_color, new_l);
	}
}

static bool color_equal(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
	unsigned char r[2], g[2], b[2], a[2];

	al_unmap_rgba(c1, &r[0], &g[0], &b[0], &a[0]);
	al_unmap_rgba(c2, &r[1], &g[1], &b[1], &a[1]);
	if(r[0] != r[1] || g[0] != g[1] || b[0] != b[1] || a[0] != a[1])
	{
		return false;
	}
	return true;
}

int quixel_gui_canvas_editor_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	QUIXEL_CANVAS_EDITOR * canvas_editor = (QUIXEL_CANVAS_EDITOR *)d->dp;

	switch(msg)
	{
		case MSG_MOUSEDOWN:
		{
			d->flags |= D_TRACKMOUSE;
			switch(canvas_editor->current_tool)
			{
				case QUIXEL_CANVAS_EDITOR_TOOL_PIXEL:
				{
					quixel_tool_pixel_logic(canvas_editor);
					break;
				}
			}
			break;
		}
		case MSG_MOUSEUP:
		{
			d->flags &= ~D_TRACKMOUSE;
			break;
		}
		case MSG_MOUSEMOVE:
		{
			switch(canvas_editor->current_tool)
			{
				case QUIXEL_CANVAS_EDITOR_TOOL_PIXEL:
				{
					quixel_tool_pixel_logic(canvas_editor);
					break;
				}
			}
			break;
		}
		case MSG_IDLE:
		{
			if(!color_equal(canvas_editor->base_color, canvas_editor->last_base_color))
			{
				canvas_editor->left_color = canvas_editor->base_color;
				canvas_editor->last_base_color = canvas_editor->base_color;
				quixel_canvas_editor_update_pick_colors(canvas_editor);
			}
			canvas_editor->shade_color = shade_color(canvas_editor->base_color, (float)canvas_editor->shade_slider_element->d2 / 1000.0);
			canvas_editor->alpha_color = alpha_color(canvas_editor->base_color, (float)canvas_editor->alpha_slider_element->d2 / 1000.0);;
			if(t3f_key[ALLEGRO_KEY_LEFT])
			{
				canvas_editor->view_x--;
			}
			if(t3f_key[ALLEGRO_KEY_RIGHT])
			{
				canvas_editor->view_x++;
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				canvas_editor->view_y--;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				canvas_editor->view_y++;
			}
			if(t3f_key[ALLEGRO_KEY_MINUS])
			{
				if(canvas_editor->view_zoom > 1)
				{
					canvas_editor->view_zoom--;
					canvas_editor->view_x -= (canvas_editor->view_x + t3f_mouse_x / canvas_editor->view_zoom) - canvas_editor->hover_x;
					canvas_editor->view_y -= (canvas_editor->view_y + t3f_mouse_y / canvas_editor->view_zoom) - canvas_editor->hover_y;
				}
				t3f_key[ALLEGRO_KEY_MINUS] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_EQUALS])
			{
				canvas_editor->view_zoom++;
				canvas_editor->view_x -= (canvas_editor->view_x + t3f_mouse_x / canvas_editor->view_zoom) - canvas_editor->hover_x;
				canvas_editor->view_y -= (canvas_editor->view_y + t3f_mouse_y / canvas_editor->view_zoom) - canvas_editor->hover_y;
				t3f_key[ALLEGRO_KEY_EQUALS] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_H])
			{
				canvas_editor->canvas->layer[canvas_editor->current_layer]->flags ^= QUIXEL_CANVAS_FLAG_HIDDEN;
				t3f_key[ALLEGRO_KEY_H] = 0;
			}
			canvas_editor->hover_x = canvas_editor->view_x + (t3f_mouse_x - d->x) / canvas_editor->view_zoom;
			canvas_editor->hover_y = canvas_editor->view_y + (t3f_mouse_y - d->y) / canvas_editor->view_zoom;

			if(t3f_key[ALLEGRO_KEY_PGUP])
			{
				canvas_editor->current_layer++;
				t3f_key[ALLEGRO_KEY_PGUP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_PGDN])
			{
				if(canvas_editor->current_layer > 0)
				{
					canvas_editor->current_layer--;
				}
				t3f_key[ALLEGRO_KEY_PGDN] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DELETE])
			{
				canvas_editor->signal = QUIXEL_CANVAS_EDITOR_SIGNAL_DELETE_LAYER;
				t3f_key[ALLEGRO_KEY_DELETE] = 0;
			}
			break;
		}
		case MSG_DRAW:
		{
			ALLEGRO_STATE old_state;
			ALLEGRO_TRANSFORM identity;
			int i, j;
			int tw, th;

			tw = d->w / canvas_editor->bg_scale + 1;
			th = d->h / canvas_editor->bg_scale + 1;
			al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM);
			al_identity_transform(&identity);
			al_use_transform(&identity);
			for(i = 0; i < th; i++)
			{
				for(j = 0; j < tw; j++)
				{
					t3f_draw_scaled_bitmap(canvas_editor->bg_tile, t3f_color_white, d->x + j * canvas_editor->bg_scale, d->y + i * canvas_editor->bg_scale, 0, canvas_editor->bg_scale, canvas_editor->bg_scale, 0);
				}
			}
			quixel_render_canvas(canvas_editor->canvas, canvas_editor->view_x, canvas_editor->view_y, canvas_editor->view_zoom, d->x, d->y, d->w, d->h);
			al_restore_state(&old_state);
			break;
		}
	}
	return D_O_K;
}
