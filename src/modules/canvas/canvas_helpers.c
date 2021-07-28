#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_helpers.h"
#include "modules/pixel_shader.h"
#include "modules/primitives.h"

/* assume all canvas bitmaps are already locked */
static int get_canvas_alpha(PA_CANVAS * cp, int x, int y, int flags_filter)
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
		if(!(flags & PA_CANVAS_FLAG_HIDDEN))
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

void pa_get_canvas_dimensions(PA_CANVAS * cp, int * offset_x, int * offset_y, int * width, int * height, int flags_filter)
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
				if(!(flags & PA_CANVAS_FLAG_HIDDEN))
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
				if(!(flags & PA_CANVAS_FLAG_HIDDEN))
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

static void draw_canvas_layer(PA_CANVAS * cp, int layer, int flags, ALLEGRO_BITMAP * bp, int offset_x, int offset_y, int width, int height)
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

static void draw_canvas_layers(PA_CANVAS * cp, int start_layer, int end_layer, int flags_filter, ALLEGRO_BITMAP * bp, int offset_x, int offset_y, int width, int height)
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
			if(!(flags & PA_CANVAS_FLAG_HIDDEN))
			{
				draw_canvas_layer(cp, i, flags, bp, offset_x, offset_y, width, height);
			}
		}
	}
}

void pa_render_canvas_to_bitmap(PA_CANVAS * cp, int start_layer, int end_layer, int x, int y, int w, int h, int flags_filter, ALLEGRO_BITMAP * bp)
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

ALLEGRO_BITMAP * pa_get_bitmap_from_canvas(PA_CANVAS * cp, int start_layer, int end_layer, int flags_filter)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;
	int w, h;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	pa_get_canvas_dimensions(cp, &cp->export_offset_x, &cp->export_offset_y, &w, &h, flags_filter);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(w, h);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	pa_render_canvas_to_bitmap(cp, start_layer, end_layer, cp->export_offset_x, cp->export_offset_y, w, h, flags_filter, bp);
	return bp;

	fail:
	{
		return NULL;
	}
}

void pa_import_bitmap_to_canvas(PA_CANVAS * cp, ALLEGRO_BITMAP * bp, int layer, int x, int y)
{
	int shift_x, shift_y;

	pa_handle_canvas_expansion(cp, x, y, x + al_get_bitmap_width(bp), y + al_get_bitmap_height(bp), &shift_x, &shift_y);
	pa_get_canvas_shift(cp, x, y, &shift_x, &shift_y);
	x += shift_x * cp->bitmap_size;
	y += shift_y * cp->bitmap_size;
	pa_draw_primitive_to_canvas(cp, layer, x, y, x + al_get_bitmap_width(bp), y + al_get_bitmap_height(bp), bp, t3f_color_white, PA_RENDER_COPY, NULL, pa_draw_quad);
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
		memset(use_bitmap[i], 0, sizeof(bool) * layer_width);
	}

	return use_bitmap;

	fail:
	{
		free_use_array(use_bitmap, layer_width, layer_height);
		return NULL;
	}
}

void pa_draw_primitive_to_canvas(PA_CANVAS * cp, int layer, int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, int mode, ALLEGRO_SHADER * shader, void (*primitive_proc)(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color))
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	bool ** use_bitmap;
	int start_bitmap_x, start_bitmap_y;
	int end_bitmap_x, end_bitmap_y;
	int x_dir, y_dir;
	int i, j, offset_x, offset_y;

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
	if(cp->layer_width <= 0 || cp->layer_height <= 0)
	{
		return;
	}
	use_bitmap = make_use_array(cp->layer_width, cp->layer_height);
	if(!use_bitmap)
	{
		printf("can't allocate memory!\n");
		return;
	}
	use_bitmap[start_bitmap_y][start_bitmap_x] = true;
	use_bitmap[end_bitmap_y][end_bitmap_x] = true;
	while(loop_break_test(start_bitmap_y, end_bitmap_y, y_dir))
	{
		start_bitmap_x = x1 / cp->bitmap_size;
		while(loop_break_test(start_bitmap_x, end_bitmap_x, x_dir))
		{
			use_bitmap[start_bitmap_y][start_bitmap_x] = true;
			start_bitmap_x += x_dir;
		}
		start_bitmap_y += y_dir;
	}
	for(i = 0; i < cp->layer_height; i++)
	{
		for(j = 0; j < cp->layer_width; j++)
		{
			if(use_bitmap[i][j])
			{
				pa_expand_canvas(cp, layer, j * cp->bitmap_size, i * cp->bitmap_size);
				al_set_target_bitmap(cp->layer[layer]->bitmap[i][j]);
				if(shader)
				{
					al_use_shader(shader);
				}
				al_use_transform(&identity);
				if(mode == PA_RENDER_COPY)
				{
					al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
				}
				offset_x = j * cp->bitmap_size;
				offset_y = i * cp->bitmap_size;
				primitive_proc(x1 - offset_x, y1 - offset_y, x2 - offset_x, y2 - offset_y, bp, color);
			}
		}
	}
	for(i = 0; i < cp->layer_height; i++)
	{
		for(j = 0; j < cp->layer_width; j++)
		{
			if(use_bitmap[i][j])
			{
				pa_set_target_pixel_shader(NULL);
			}
		}
	}
	free_use_array(use_bitmap, cp->layer_width, cp->layer_height);
	al_restore_state(&old_state);
}

ALLEGRO_COLOR pa_get_canvas_pixel(PA_CANVAS * cp, int layer, int x, int y)
{
	int tx, ty;

	tx = x / cp->bitmap_size;
	ty = y / cp->bitmap_size;

	if(layer < cp->layer_max && tx >= 0 && tx < cp->layer_width && ty >= 0 && ty < cp->layer_height && cp->layer[layer]->bitmap && cp->layer[layer]->bitmap[ty][tx])
	{
		return al_get_pixel(cp->layer[layer]->bitmap[ty][tx], x % cp->bitmap_size, y % cp->bitmap_size);
	}
	return al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
}

void pa_get_canvas_shift(PA_CANVAS * cp, int x, int y, int * shift_x, int * shift_y)
{
	*shift_x = 0;
	*shift_y = 0;

	/* calculate shift amount */
	if(x < 0)
	{
		*shift_x = -x / cp->bitmap_size + 1;
	}
	if(y < 0)
	{
		*shift_y = -y / cp->bitmap_size + 1;
	}
}

bool pa_handle_canvas_expansion(PA_CANVAS * cp, int left, int top, int right, int bottom, int * shift_x, int * shift_y)
{
	int cx, cy;
	int new_width, new_height;

	/* create initial array if needed */
	if(cp->layer_width < 1 || cp->layer_height < 1)
	{
		pa_resize_canvas_bitmap_array(cp, 1, 1);
	}

	pa_get_canvas_shift(cp, left, top, shift_x, shift_y);

	/* actually shift the bitmap array and update variables if we need to */
	if(*shift_x || *shift_y)
	{
		pa_shift_canvas_bitmap_array(cp, *shift_x, *shift_y);
	}

	/* expand down and to the right if needed */
	new_width = cp->layer_width;
	cx = right / cp->bitmap_size;
	if(cx >= cp->layer_width)
	{
		new_width = cx + 1;
	}
	new_height = cp->layer_height;
	cy = bottom / cp->bitmap_size;
	if(cy >= cp->layer_height)
	{
		new_height = cy + 1;
	}
	if(new_width != cp->layer_width || new_height != cp->layer_height)
	{
		pa_resize_canvas_bitmap_array(cp, new_width, new_height);
	}
	return true;
}
