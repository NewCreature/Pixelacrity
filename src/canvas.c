#include "t3f/t3f.h"
#include "canvas.h"

QUIXEL_CANVAS * quixel_create_canvas(void)
{
	QUIXEL_CANVAS * cp;

	cp = malloc(sizeof(QUIXEL_CANVAS));
	if(cp)
	{
		memset(cp, 0, sizeof(QUIXEL_CANVAS));
		cp->bitmap_size = al_get_display_option(t3f_display, ALLEGRO_MAX_BITMAP_SIZE) / 2;
	}
	return cp;
}

void quixel_destroy_canvas(QUIXEL_CANVAS * cp)
{
	int i, j;

	for(i = 0; i < QUIXEL_CANVAS_MAX_HEIGHT; i++)
	{
		for(j = 0; j < QUIXEL_CANVAS_MAX_WIDTH; j++)
		{
			if(cp->bitmap[i][j])
			{
				al_destroy_bitmap(cp->bitmap[i][j]);
			}
		}
	}
}

void quixel_expand_canvas(QUIXEL_CANVAS * cp, int x, int y)
{
	ALLEGRO_STATE old_state;

	if(!cp->bitmap[y / cp->bitmap_size][x / cp->bitmap_size])
	{
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP);
		al_set_new_bitmap_flags(0);
		cp->bitmap[y / cp->bitmap_size][x / cp->bitmap_size] = al_create_bitmap(cp->bitmap_size, cp->bitmap_size);
		if(cp->bitmap[y / cp->bitmap_size][x / cp->bitmap_size])
		{
			al_set_target_bitmap(cp->bitmap[y / cp->bitmap_size][x / cp->bitmap_size]);
			al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		}
		al_restore_state(&old_state);
	}
}

static bool bitmap_visible(QUIXEL_CANVAS * cp, int j, int i, int x, int y, int width, int height, int scale)
{
	int b_top, b_bottom, b_left, b_right;
	int c_top, c_bottom, c_left, c_right;

	b_top = i * cp->bitmap_size;
	b_bottom = i * cp->bitmap_size + cp->bitmap_size - 1;
	b_left = j * cp->bitmap_size;
	b_right = j * cp->bitmap_size + cp->bitmap_size - 1;
	c_top = y;
	c_bottom = y + height / scale;
	c_left = x;
	c_right = x + width / scale;
	if(b_top <= c_bottom && c_top <= b_bottom && b_left <= c_right && c_left <= b_right)
	{
		return true;
	}
	return false;
}

void quixel_render_canvas(QUIXEL_CANVAS * cp, int x, int y, int width, int height, int scale)
{
//	int cx, cy, cw, ch;
	int i, j;

//	al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
//	al_set_clipping_rectangle(x, y, width, height);
	for(i = 0; i < QUIXEL_CANVAS_MAX_HEIGHT; i++)
	{
		for(j = 0; j < QUIXEL_CANVAS_MAX_WIDTH; j++)
		{
			if(cp->bitmap[i][j] && bitmap_visible(cp, j, i, x, y, width, height, scale))
			{
				t3f_draw_scaled_bitmap(cp->bitmap[i][j], t3f_color_white, (j * cp->bitmap_size - x) * scale, (i * cp->bitmap_size - y) * scale, 0, cp->bitmap_size * scale, cp->bitmap_size * scale, 0);
			}
		}
	}
//	al_set_clipping_rectangle(cx, cy, cw, ch);
}
