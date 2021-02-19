#include "t3f/t3f.h"
#include "canvas_editor.h"
#include "tool_pixel.h"

static ALLEGRO_BITMAP * make_checkerboard_bitmap(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_BITMAP * bp;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_TRANSFORM);
	al_set_new_bitmap_flags(0);
	bp = al_create_bitmap(2, 2);
	if(bp)
	{
		al_set_target_bitmap(bp);
		al_identity_transform(&identity);
		al_use_transform(&identity);
		al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		al_put_pixel(0, 0, c1);
		al_put_pixel(0, 1, c2);
		al_put_pixel(1, 0, c2);
		al_put_pixel(1, 1, c1);
		al_unlock_bitmap(bp);
	}
	al_restore_state(&old_state);
	return bp;
}

QUIXEL_CANVAS_EDITOR * quixel_create_canvas_editor(void)
{
	QUIXEL_CANVAS_EDITOR * cep;

	cep = malloc(sizeof(QUIXEL_CANVAS_EDITOR));
	if(!cep)
	{
		goto fail;
	}
	memset(cep, 0, sizeof(QUIXEL_CANVAS_EDITOR));
	cep->bg_tile = make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
	if(!cep->bg_tile)
	{
		goto fail;
	}
	cep->base_color = al_map_rgba_f(1.0, 0.0, 0.0, 1.0);
	return cep;

	fail:
	{
		if(cep)
		{
			if(cep->bg_tile)
			{
				al_destroy_bitmap(cep->bg_tile);
			}
			free(cep);
		}
		return NULL;
	}
}

void quixel_canvas_editor_update_pick_colors(QUIXEL_CANVAS_EDITOR * cep)
{
	float h, s, l, new_l, step;
	float r, g, b;
	int i;

	step = 1.0 / (float)(QUIXEL_COLOR_PICKER_SHADES - 1);
	al_unmap_rgb_f(cep->base_color, &r, &g, &b);
	al_color_rgb_to_hsl(r, g, b, &h, &s, &l);
	for(i = 0; i < QUIXEL_COLOR_PICKER_SHADES; i++)
	{
		new_l = step * (float)i;
		cep->pick_color[i] = al_color_hsl(h, s, new_l);
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

void quixel_canvas_editor_logic(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_CANVAS * cp)
{
	if(!color_equal(cep->base_color, cep->last_base_color))
	{
		cep->left_color = cep->base_color;
		cep->last_base_color = cep->base_color;
		quixel_canvas_editor_update_pick_colors(cep);
	}
	if(t3f_key[ALLEGRO_KEY_LEFT])
	{
		cep->view_x--;
	}
	if(t3f_key[ALLEGRO_KEY_RIGHT])
	{
		cep->view_x++;
	}
	if(t3f_key[ALLEGRO_KEY_UP])
	{
		cep->view_y--;
	}
	if(t3f_key[ALLEGRO_KEY_DOWN])
	{
		cep->view_y++;
	}
	if(t3f_key[ALLEGRO_KEY_MINUS])
	{
		if(cep->view_zoom > 1)
		{
			cep->view_zoom--;
			cep->view_x -= (cep->view_x + t3f_mouse_x / cep->view_zoom) - cep->hover_x;
			cep->view_y -= (cep->view_y + t3f_mouse_y / cep->view_zoom) - cep->hover_y;
		}
		t3f_key[ALLEGRO_KEY_MINUS] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_EQUALS])
	{
		cep->view_zoom++;
		cep->view_x -= (cep->view_x + t3f_mouse_x / cep->view_zoom) - cep->hover_x;
		cep->view_y -= (cep->view_y + t3f_mouse_y / cep->view_zoom) - cep->hover_y;
		t3f_key[ALLEGRO_KEY_EQUALS] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_H])
	{
		cp->layer[cep->current_layer]->flags ^= QUIXEL_CANVAS_FLAG_HIDDEN;
		t3f_key[ALLEGRO_KEY_H] = 0;
	}
	cep->hover_x = cep->view_x + t3f_mouse_x / cep->view_zoom;
	cep->hover_y = cep->view_y + t3f_mouse_y / cep->view_zoom;

	if(t3f_key[ALLEGRO_KEY_PGUP])
	{
		cep->current_layer++;
		t3f_key[ALLEGRO_KEY_PGUP] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_PGDN])
	{
		if(cep->current_layer > 0)
		{
			cep->current_layer--;
		}
		t3f_key[ALLEGRO_KEY_PGDN] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DELETE])
	{
		cep->signal = QUIXEL_CANVAS_EDITOR_SIGNAL_DELETE_LAYER;
		t3f_key[ALLEGRO_KEY_DELETE] = 0;
	}

	if(t3f_mouse_button[0])
	{
		switch(cep->current_tool)
		{
			case QUIXEL_CANVAS_EDITOR_TOOL_PIXEL:
			{
				quixel_tool_pixel_logic(cep, cp);
				break;
			}
		}
	}
}

void quixel_canvas_editor_render(QUIXEL_CANVAS_EDITOR * cep, QUIXEL_CANVAS * cp)
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int i, j;
	int tw, th;

	tw = al_get_display_width(t3f_display) / cep->bg_scale + 1;
	th = al_get_display_height(t3f_display) / cep->bg_scale + 1;
	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM);
	al_identity_transform(&identity);
	al_use_transform(&identity);
	for(i = 0; i < th; i++)
	{
		for(j = 0; j < tw; j++)
		{
			t3f_draw_scaled_bitmap(cep->bg_tile, t3f_color_white, j * cep->bg_scale, i * cep->bg_scale, 0, cep->bg_scale, cep->bg_scale, 0);
		}
	}
	quixel_render_canvas(cp, cep->view_x, cep->view_y, al_get_display_width(t3f_display), al_get_display_height(t3f_display), cep->view_zoom);
	al_restore_state(&old_state);
}
