#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_helpers.h"
#include "modules/pixel_shader.h"
#include "modules/primitives.h"

/* assume all canvas bitmaps are already locked */
static int get_canvas_alpha(PA_CANVAS * cp, int layer, int x, int y)
{
	ALLEGRO_COLOR color;
	int offset_x, offset_y;
	int tile_x, tile_y;
	unsigned char r, a;

	tile_x = x / cp->bitmap_size;
	tile_y = y / cp->bitmap_size;
	if(cp->layer[layer]->bitmap[tile_y][tile_x])
	{
		offset_x = cp->bitmap_size * tile_x;
		offset_y = cp->bitmap_size * tile_y;
		color = al_get_pixel(cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size], x - offset_x, y - offset_y);
		al_unmap_rgba(color, &r, &r, &r, &a);
		if(a > 0)
		{
			return a;
		}
	}
	return 0;
}

void pa_calculate_canvas_layer_dimensions(PA_CANVAS * cp, int layer, int * offset_x, int * offset_y, int * width, int * height)
{
	int j, k, l, m, x, y;
	int left_x = cp->layer_width * cp->bitmap_size;
	int right_x = 0;
	int top_y = cp->layer_height * cp->bitmap_size;
	int bottom_y = 0;
	bool need_check;

	for(j = 0; j < cp->layer_height; j++)
	{
		for(k = 0; k < cp->layer_width; k++)
		{
			need_check = false;
			if(cp->layer[layer]->bitmap[j][k])
			{
				al_lock_bitmap(cp->layer[layer]->bitmap[j][k], ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
				need_check = true;
			}
			if(need_check)
			{
				for(l = 0; l < cp->bitmap_size; l++)
				{
					for(m = 0; m < cp->bitmap_size; m++)
					{
						x = k * cp->bitmap_size + m;
						y = j * cp->bitmap_size + l;
						if(get_canvas_alpha(cp, layer, x, y))
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
			if(cp->layer[layer]->bitmap[j][k])
			{
				al_unlock_bitmap(cp->layer[layer]->bitmap[j][k]);
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

void pa_get_canvas_dimensions(PA_CANVAS * cp, int * offset_x, int * offset_y, int * width, int * height, int flags_filter, bool calculate)
{
	int i;
	int left_x = cp->layer_width * cp->bitmap_size;
	int right_x = 0;
	int top_y = cp->layer_height * cp->bitmap_size;
	int bottom_y = 0;
	int flags;

	if(cp->layer_width <= 0 && cp->layer_height <= 0)
	{
		if(width)
		{
			*width = 0;
		}
		if(height)
		{
			*height = 0;
		}
		return;
	}
	for(i = 0; i < cp->layer_max; i++)
	{
		if(calculate)
		{
			pa_calculate_canvas_layer_dimensions(cp, i, &cp->layer[i]->offset_x, &cp->layer[i]->offset_y, &cp->layer[i]->width, &cp->layer[i]->height);
		}
		if(cp->layer[i]->width > 0 && cp->layer[i]->height > 0)
		{
			flags = cp->layer[i]->flags & ~flags_filter;
			if(!(flags & PA_CANVAS_FLAG_HIDDEN))
			{
				if(cp->layer[i]->offset_x < left_x)
				{
					left_x = cp->layer[i]->offset_x;
				}
				if(cp->layer[i]->offset_x + cp->layer[i]->width >= right_x)
				{
					right_x = cp->layer[i]->offset_x + cp->layer[i]->width - 1;
				}
				if(cp->layer[i]->offset_y < top_y)
				{
					top_y = cp->layer[i]->offset_y;
				}
				if(cp->layer[i]->offset_y + cp->layer[i]->height >= bottom_y)
				{
					bottom_y = cp->layer[i]->offset_y + cp->layer[i]->height - 1;
				}
			}
		}
	}
	if(right_x >= left_x && bottom_y >= top_y)
	{
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
	else
	{
		if(width)
		{
			*width = 0;
		}
		if(height)
		{
			*height = 0;
		}
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

void pa_render_canvas_to_bitmap(PA_CANVAS * cp, int start_layer, int end_layer, int x, int y, int w, int h, int flags_filter, ALLEGRO_BITMAP * bp, ALLEGRO_SHADER * shader)
{
	ALLEGRO_STATE old_state;

	al_store_state(&old_state, ALLEGRO_STATE_TRANSFORM | ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
	al_set_target_bitmap(bp);
	pa_set_target_pixel_shader(shader);
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
	if(start_layer < cp->layer_max && end_layer <= cp->layer_max)
	{
		draw_canvas_layers(cp, start_layer, end_layer, flags_filter, bp, x, y, w, h);
	}
	al_restore_state(&old_state);
}

ALLEGRO_BITMAP * pa_get_bitmap_from_canvas(PA_CANVAS * cp, int start_layer, int end_layer, int flags_filter, bool calculate)
{
	ALLEGRO_BITMAP * bp;
	ALLEGRO_STATE old_state;
	int w, h;

	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
	pa_get_canvas_dimensions(cp, &cp->export_offset_x, &cp->export_offset_y, &w, &h, flags_filter, calculate);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	bp = al_create_bitmap(w, h);
	al_restore_state(&old_state);
	if(!bp)
	{
		goto fail;
	}
	pa_render_canvas_to_bitmap(cp, start_layer, end_layer, cp->export_offset_x, cp->export_offset_y, w, h, flags_filter, bp, NULL);
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
	pa_draw_primitive_to_canvas(cp, layer, x, y, x + al_get_bitmap_width(bp), y + al_get_bitmap_height(bp), NULL, t3f_color_white, bp, PA_RENDER_COPY, NULL, pa_draw_quad);
}

void pa_update_canvas_dimensions(PA_CANVAS * cp, int layer, int left, int top, int right, int bottom)
{
	int real_left, real_top, real_right, real_bottom;

	if(cp->layer[layer]->width < 1)
	{
		cp->layer[layer]->offset_x = left;
	}
	if(left < cp->layer[layer]->offset_x)
	{
		real_left = left;
	}
	else
	{
		real_left = cp->layer[layer]->offset_x;
	}
	if(right > cp->layer[layer]->offset_x + cp->layer[layer]->width)
	{
		real_right = right;
	}
	else
	{
		real_right = cp->layer[layer]->offset_x + cp->layer[layer]->width;
	}
	if(cp->layer[layer]->height < 1)
	{
		cp->layer[layer]->offset_y = top;
	}
	if(top < cp->layer[layer]->offset_y)
	{
		real_top = top;
	}
	else
	{
		real_top = cp->layer[layer]->offset_y;
	}
	if(bottom > cp->layer[layer]->offset_y + cp->layer[layer]->height)
	{
		real_bottom = bottom;
	}
	else
	{
		real_bottom = cp->layer[layer]->offset_y + cp->layer[layer]->height;
	}
	cp->layer[layer]->offset_x = real_left;
	cp->layer[layer]->offset_y = real_top;
	cp->layer[layer]->width = real_right - real_left;
	cp->layer[layer]->height = real_bottom - real_top;
}

void pa_draw_primitive_to_canvas(PA_CANVAS * cp, int layer, int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, ALLEGRO_BITMAP * texture, int mode, ALLEGRO_SHADER * shader, void (*primitive_proc)(int x1, int y1, int x2, int y2, ALLEGRO_BITMAP * bp, ALLEGRO_COLOR color, ALLEGRO_BITMAP * texture))
{
	ALLEGRO_STATE old_state;
	ALLEGRO_TRANSFORM identity;
	int start_x, start_y, end_x, end_y;
	int start_bitmap_x, start_bitmap_y;
	int end_bitmap_x, end_bitmap_y;
	int i, j, offset_x, offset_y;
	int w = 0, h = 0;
	int left, top, right, bottom;

	/* break early if we don't have any canvas tiles to render to */
	if(cp->layer_width <= 0 || cp->layer_height <= 0)
	{
		return;
	}
	if(bp)
	{
		w = al_get_bitmap_width(bp);
		h	= al_get_bitmap_height(bp);
	}

	/* copy and sort coordinates, we'll use the original for rendering to preserve
	   the user order of operations */
	start_x = x1;
	start_y = y1;
	end_x = x2;
	end_y = y2;
	pa_sort_coordinates(&start_x, &end_x);
	pa_sort_coordinates(&start_y, &end_y);
	left = start_x - w / 2;
	top = start_y - h / 2;
	right = end_x + w / 2 + w % 2;
	bottom = end_y + h / 2 + h % 2;

	pa_update_canvas_dimensions(cp, layer, left, top, right, bottom);

	/* loop through all affected bitmaps and render the primitive to them all */
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER | ALLEGRO_STATE_TRANSFORM);
	al_identity_transform(&identity);
	start_bitmap_x = left / cp->bitmap_size;
	start_bitmap_y = top / cp->bitmap_size;
	end_bitmap_x = right / cp->bitmap_size;
	end_bitmap_y = bottom / cp->bitmap_size;
	for(i = start_bitmap_y; i <= end_bitmap_y; i++)
	{
		for(j = start_bitmap_x; j <= end_bitmap_x; j++)
		{
			pa_expand_canvas(cp, layer, j * cp->bitmap_size, i * cp->bitmap_size);
			if(cp->layer[layer]->bitmap[i][j])
			{
				al_set_target_bitmap(cp->layer[layer]->bitmap[i][j]);
				if(shader)
				{
					al_use_shader(shader);
				}
				else
				{
					pa_set_target_pixel_shader(NULL);
				}
				al_use_transform(&identity);
				if(mode == PA_RENDER_COPY)
				{
					al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
				}
				offset_x = j * cp->bitmap_size;
				offset_y = i * cp->bitmap_size;
				primitive_proc(x1 - offset_x, y1 - offset_y, x2 - offset_x, y2 - offset_y, bp, color, texture);
			}
		}
	}

	/* reset the shader for all affected bitmaps */
	for(i = start_bitmap_y; i <= end_bitmap_y; i++)
	{
		for(j = start_bitmap_x; j <= end_bitmap_x; j++)
		{
			pa_set_target_pixel_shader(NULL);
		}
	}
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
	cx = right / cp->bitmap_size + *shift_x;
	if(cx >= cp->layer_width)
	{
		new_width = cx + 1;
	}
	new_height = cp->layer_height;
	cy = bottom / cp->bitmap_size + *shift_y;
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

void pa_swap_canvas_layer(PA_CANVAS * cp, int layer1, int layer2)
{
	PA_CANVAS_LAYER * temp_layer;

	temp_layer = cp->layer[layer2];
	cp->layer[layer2] = cp->layer[layer1];
	cp->layer[layer1] = temp_layer;
}
