#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_helpers.h"
#include "modules/pixel_shader.h"

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
	int left_x = 1000000;
	int right_x = 0;
	int top_y = 1000000;
	int bottom_y = 0;
	bool need_check;
	int flags;

	for(j = 0; j < cp->layer_height; j++)
	{
		for(k = 0; k < cp->layer_width; k++)
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

	for(j = 0; j < cp->layer_height; j++)
	{
		for(k = 0; k < cp->layer_width; k++)
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
		if(i == start_layer)
		{
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		}
		else
		{
			al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
		}
		if(i < cp->layer_max)
		{
			flags = cp->layer[i]->flags & ~flags_filter;
			if(!(flags & QUIXEL_CANVAS_FLAG_HIDDEN))
			{
				draw_canvas_layer(cp, i, flags, bp, offset_x, offset_y, width, height);
			}
		}
	}
}

void quixel_render_canvas_to_bitmap(QUIXEL_CANVAS * cp, int start_layer, int end_layer, int x, int y, int w, int h, int flags_filter, ALLEGRO_BITMAP * bp)
{
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(bp);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	if(start_layer < cp->layer_max && end_layer <= cp->layer_max)
	{
		draw_canvas_layers(cp, start_layer, end_layer, flags_filter, bp, x, y, w, h);
	}
	al_restore_state(&old_state);
}

ALLEGRO_BITMAP * quixel_get_bitmap_from_canvas(QUIXEL_CANVAS * cp, int start_layer, int end_layer, int flags_filter)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;
	int w, h;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	quixel_get_canvas_dimensions(cp, &cp->export_offset_x, &cp->export_offset_y, &w, &h, flags_filter);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(w, h);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	quixel_render_canvas_to_bitmap(cp, start_layer, end_layer, cp->export_offset_x, cp->export_offset_y, w, h, flags_filter, bp);
	return bp;

	fail:
	{
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
	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
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
				al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
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

static bool loop_break_test(int i1, int i2, int dir)
{
	if(dir < 0)
	{
		if(i2 <= i1)
		{
			return true;
		}
	}
	else
	{
		if(i1 <= i2)
		{
			return true;
		}
	}
	return false;
}

static void free_use_array(bool ** use_bitmap, int layer_width, int layer_height)
{
	int i;

	if(use_bitmap)
	{
		for(i = 0; i < layer_height; i++)
		{
			if(use_bitmap[i])
			{
				free(use_bitmap[i]);
			}
		}
		free(use_bitmap);
	}
}

static bool ** make_use_array(int layer_width, int layer_height)
{
	bool ** use_bitmap;
	int i;

	if(layer_width < 1 || layer_height < 1)
	{
		return NULL;
	}
	use_bitmap = malloc(sizeof(bool *) * layer_height);
	if(!use_bitmap)
	{
		goto fail;
	}
	memset(use_bitmap, 0, sizeof(bool *) * layer_height);
	for(i = 0; i < layer_height; i++)
	{
		use_bitmap[i] = malloc(sizeof(bool) * layer_width);
		if(!use_bitmap[i])
		{
			goto fail;
		}
	}

	return use_bitmap;

	fail:
	{
		free_use_array(use_bitmap, layer_width, layer_height);
		return NULL;
	}
}

void quixel_draw_primitive_to_canvas(QUIXEL_CANVAS * cp, int layer, int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, int mode, void (*primitive_proc)(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color))
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	bool ** use_bitmap;
	int start_bitmap_x, start_bitmap_y;
	int end_bitmap_x, end_bitmap_y;
	int x_dir, y_dir;
	int i, j, offset_x, offset_y;
	int tw, th;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM);
	al_identity_transform(&identity);
	start_bitmap_x = x1 / cp->bitmap_size;
	start_bitmap_y = y1 / cp->bitmap_size;
	end_bitmap_x = x2 / cp->bitmap_size;
	end_bitmap_y = y2 / cp->bitmap_size;
	if(start_bitmap_x < end_bitmap_x)
	{
		x_dir = 1;
	}
	else
	{
		x_dir = -1;
	}
	if(start_bitmap_y < end_bitmap_y)
	{
		y_dir = 1;
	}
	else
	{
		y_dir = -1;
	}
	tw = abs(start_bitmap_x - end_bitmap_x) + 1;
	th = abs(start_bitmap_y - end_bitmap_y) + 1;
	use_bitmap = make_use_array(tw, th);
	if(!use_bitmap)
	{
		printf("can't allocate memory!\n");
		return;
	}
	use_bitmap[start_bitmap_y][start_bitmap_x] = true;
	use_bitmap[end_bitmap_y][end_bitmap_x] = true;
	while(!loop_break_test(start_bitmap_y, end_bitmap_y, y_dir))
	{
		start_bitmap_x = x1 / cp->bitmap_size;
		while(!loop_break_test(start_bitmap_x, end_bitmap_x, x_dir))
		{
			use_bitmap[start_bitmap_y][start_bitmap_x] = true;
		}
	}
	for(i = 0; i < th; i++)
	{
		for(j = 0; j < tw; j++)
		{
			if(use_bitmap[i][j])
			{
				quixel_expand_canvas(cp, layer, j * cp->bitmap_size, i * cp->bitmap_size);
				al_set_target_bitmap(cp->layer[layer]->bitmap[i][j]);
				al_use_transform(&identity);
				if(mode == QUIXEL_RENDER_COPY)
				{
					al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
				}
				offset_x = j * cp->bitmap_size;
				offset_y = i * cp->bitmap_size;
				primitive_proc(x1 - offset_x, y1 - offset_y, x2 - offset_x, y2 - offset_y, bp, color);
			}
		}
	}
	free_use_array(use_bitmap, tw, th);
	al_restore_state(&old_state);
}

ALLEGRO_COLOR quixel_get_canvas_pixel(QUIXEL_CANVAS * cp, int layer, int x, int y)
{
	int tx, ty;

	tx = x / cp->bitmap_size;
	ty = y / cp->bitmap_size;

	if(layer < cp->layer_max && cp->layer[layer]->bitmap && cp->layer[layer]->bitmap[ty][tx])
	{
		return al_get_pixel(cp->layer[layer]->bitmap[ty][tx], x % cp->bitmap_size, y % cp->bitmap_size);
	}
	return al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
}
