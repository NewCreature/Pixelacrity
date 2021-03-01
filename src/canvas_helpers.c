#include "t3f/t3f.h"
#include "canvas.h"

/* assume all canvas bitmaps are already locked */
static int get_canvas_alpha(QUIXEL_CANVAS * cp, int x, int y, int flags_filter)
{
	ALLEGRO_COLOR color;
	int i;
	int offset_x, offset_y;
	int tile_x, tile_y;
	unsigned char r, a;
	int flags;

	for(i = 0; i < cp->layer_max; i++)
	{
		flags = cp->layer[i]->flags & ~flags_filter;
		if(!(flags & QUIXEL_CANVAS_FLAG_HIDDEN))
		{
			tile_x = x / cp->bitmap_size;
			tile_y = y / cp->bitmap_size;
			if(cp->layer[i]->bitmap[tile_y][tile_x])
			{
				offset_x = cp->bitmap_size * tile_x;
				offset_y = cp->bitmap_size * tile_y;
				color = al_get_pixel(cp->layer[i]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size], x - offset_x, y - offset_y);
				al_unmap_rgba(color, &r, &r, &r, &a);
				if(a > 0)
				{
					return a;
				}
			}
		}
	}
	return 0;
}

void quixel_get_canvas_dimensions(QUIXEL_CANVAS * cp, int * offset_x, int * offset_y, int * width, int * height, int flags_filter)
{
	int i, j, k, l, m, x, y;
	int left_x = cp->bitmap_size * QUIXEL_CANVAS_MAX_WIDTH;
	int right_x = 0;
	int top_y = cp->bitmap_size * QUIXEL_CANVAS_MAX_HEIGHT;
	int bottom_y = 0;
	bool need_check;
	int flags;

	for(j = 0; j < QUIXEL_CANVAS_MAX_HEIGHT; j++)
	{
		for(k = 0; k < QUIXEL_CANVAS_MAX_WIDTH; k++)
		{
			need_check = false;
			for(i = 0; i < cp->layer_max; i++)
			{
				flags = cp->layer[i]->flags & ~flags_filter;
				if(!(flags & QUIXEL_CANVAS_FLAG_HIDDEN))
				{
					if(cp->layer[i]->bitmap[j][k])
					{
						al_lock_bitmap(cp->layer[i]->bitmap[j][k], ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
						need_check = true;
					}
				}
			}
			if(need_check)
			{
				for(l = 0; l < cp->bitmap_size; l++)
				{
					for(m = 0; m < cp->bitmap_size; m++)
					{
						x = k * cp->bitmap_size + m;
						y = j * cp->bitmap_size + l;
						if(get_canvas_alpha(cp, x, y, flags_filter))
						{
							if(x < left_x)
							{
								left_x = x;
							}
							if(x > right_x)
							{
								right_x = x;
							}
							if(y < top_y)
							{
								top_y = y;
							}
							if(y > bottom_y)
							{
								bottom_y = y;
							}
						}
					}
				}
			}
			for(i = 0; i < cp->layer_max; i++)
			{
				flags = cp->layer[i]->flags & ~flags_filter;
				if(!(flags & QUIXEL_CANVAS_FLAG_HIDDEN))
				{
					if(cp->layer[i]->bitmap[j][k])
					{
						al_unlock_bitmap(cp->layer[i]->bitmap[j][k]);
					}
				}
			}
		}
	}
	if(offset_x)
	{
		*offset_x = left_x;
	}
	if(offset_y)
	{
		*offset_y = top_y;
	}
	if(width)
	{
		*width = (right_x - left_x) + 1;
	}
	if(height)
	{
		*height = (bottom_y - top_y) + 1;
	}
}

static void draw_canvas_layer(QUIXEL_CANVAS * cp, int layer, int flags, ALLEGRO_BITMAP * bp, int offset_x, int offset_y, int width, int height)
{
	int j, k;
	int x, y;

	for(j = 0; j < QUIXEL_CANVAS_MAX_HEIGHT; j++)
	{
		for(k = 0; k < QUIXEL_CANVAS_MAX_WIDTH; k++)
		{
			if(cp->layer[layer]->bitmap[j][k])
			{
				x = k * cp->bitmap_size - offset_x;
				y = j * cp->bitmap_size - offset_y;
				al_draw_bitmap(cp->layer[layer]->bitmap[j][k], x, y, 0);
			}
		}
	}
}

static void draw_canvas_layers(QUIXEL_CANVAS * cp, int start_layer, int end_layer, int flags_filter, ALLEGRO_BITMAP * bp, int offset_x, int offset_y, int width, int height)
{
	ALLEGRO_TRANSFORM identity;
	int flags;
	int i;

	al_identity_transform(&identity);
	al_set_target_bitmap(bp);
	al_use_transform(&identity);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	for(i = start_layer; i < end_layer; i++)
	{
		flags = cp->layer[i]->flags & ~flags_filter;
		if(!(flags & QUIXEL_CANVAS_FLAG_HIDDEN))
		{
			draw_canvas_layer(cp, i, flags, bp, offset_x, offset_y, width, height);
		}
	}
}

ALLEGRO_BITMAP * quixel_render_canvas_to_bitmap(QUIXEL_CANVAS * cp, int start_layer, int end_layer, int flags_filter)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;
	int w, h;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP);
	quixel_get_canvas_dimensions(cp, &cp->export_offset_x, &cp->export_offset_y, &w, &h, flags_filter);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(w, h);
	if(!bp)
	{
		goto fail;
	}
	draw_canvas_layers(cp, start_layer, end_layer, flags_filter, bp, cp->export_offset_x, cp->export_offset_y, w, h);
	al_restore_state(&old_state);
	return bp;

	fail:
	{
		al_restore_state(&old_state);
		return NULL;
	}
}

void quixel_import_bitmap_to_canvas(QUIXEL_CANVAS * cp, ALLEGRO_BITMAP * bp, int layer, int x, int y)
{
	ALLEGRO_TRANSFORM identity;
	ALLEGRO_STATE old_state;
	int tile_x, tile_y;
	int offset_x, offset_y;
	int x_remaining, y_remaining;
	int x_use, y_use;

	al_identity_transform(&identity);
	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP);
	y_remaining = al_get_bitmap_height(bp);
	y_use = cp->bitmap_size - y % cp->bitmap_size;
	tile_y = y / cp->bitmap_size;
	offset_y = y % cp->bitmap_size;
	while(y_remaining > 0)
	{
		tile_x = x / cp->bitmap_size;
		offset_x = x % cp->bitmap_size;
		x_remaining = al_get_bitmap_width(bp);
		x_use = cp->bitmap_size - x % cp->bitmap_size;
		while(x_remaining > 0)
		{
			quixel_expand_canvas(cp, layer, tile_x * cp->bitmap_size, tile_y * cp->bitmap_size);
			if(cp->layer[layer]->bitmap[tile_y][tile_x])
			{
				al_set_target_bitmap(cp->layer[layer]->bitmap[tile_y][tile_x]);
				al_use_transform(&identity);
				al_draw_bitmap(bp, offset_x, offset_y, 0);
			}
			offset_x -= cp->bitmap_size;
			x_remaining -= x_use;
			x_use = cp->bitmap_size;
			tile_x++;
		}
		offset_y -= cp->bitmap_size;
		y_remaining -= y_use;
		y_use = cp->bitmap_size;
		tile_y++;
	}
	al_restore_state(&old_state);
}
