#include "t3f/t3f.h"
#include "canvas.h"

static QUIXEL_CANVAS_LAYER * quixel_create_canvas_layer(void)
{
	QUIXEL_CANVAS_LAYER * lp;

	lp = malloc(sizeof(QUIXEL_CANVAS_LAYER));
	if(lp)
	{
		memset(lp, 0, sizeof(QUIXEL_CANVAS_LAYER));
	}
	return lp;
}

static QUIXEL_CANVAS_FRAME * quixel_create_canvas_frame(const char * name, int x, int y, int width, int height)
{
	QUIXEL_CANVAS_FRAME * fp = NULL;

	fp = malloc(sizeof(QUIXEL_CANVAS_FRAME));
	if(!fp)
	{
		goto fail;
	}
	memset(fp, 0, sizeof(QUIXEL_CANVAS_FRAME));
	fp->name = strdup(name);
	if(!fp->name)
	{
		goto fail;
	}
	fp->box.start_x = x;
	fp->box.start_y = y;
	fp->box.width = width;
	fp->box.height = height;

	return fp;

	fail:
	{
		if(fp)
		{
			if(fp->name)
			{
				free(fp->name);
			}
			free(fp);
		}
	}
	return NULL;
}

QUIXEL_CANVAS * quixel_create_canvas(int bitmap_max)
{
	QUIXEL_CANVAS * cp;

	cp = malloc(sizeof(QUIXEL_CANVAS));
	if(cp)
	{
		memset(cp, 0, sizeof(QUIXEL_CANVAS));
		cp->bitmap_size = al_get_display_option(t3f_display, ALLEGRO_MAX_BITMAP_SIZE) / 2;
		if(cp->bitmap_size > bitmap_max)
		{
			cp->bitmap_size = bitmap_max;
		}
		cp->config = al_create_config();
		if(!cp->config)
		{
			goto fail;
		}
	}
	return cp;

	fail:
	{
		quixel_destroy_canvas(cp);
		return NULL;
	}
}

void quixel_destroy_canvas(QUIXEL_CANVAS * cp)
{
	int i, j, k, l;

	if(cp)
	{
		if(cp->config)
		{
			al_destroy_config(cp->config);
		}
		if(cp->layer)
		{
			for(i = 0; i < cp->layer_max; i++)
			{
				if(cp->layer[i])
				{
					for(j = 0; j < cp->layer_height; j++)
					{
						for(k = 0; k < cp->layer_width; k++)
						{
							if(cp->layer[i]->bitmap[j][k])
							{
								al_destroy_bitmap(cp->layer[i]->bitmap[j][k]);
							}
						}
					}
					if(cp->layer[i]->bitmap)
					{
						for(l = 0; l < cp->layer_height; l++)
						{
							free(cp->layer[i]->bitmap[l]);
						}
					}
					free(cp->layer[i]);
				}
			}
			free(cp->layer);
		}
		free(cp);
	}
}

static void destroy_bitmap_array(ALLEGRO_BITMAP *** bp, int width, int height)
{
	int i;

	if(bp)
	{
		for(i = 0; i < height; i++)
		{
			if(bp[i])
			{
				free(bp[i]);
			}
		}
		free(bp);
	}
}

static ALLEGRO_BITMAP *** create_bitmap_array(int width, int height)
{
	ALLEGRO_BITMAP *** bp;
	int i;

	bp = malloc(sizeof(ALLEGRO_BITMAP *) * height);
	if(!bp)
	{
		goto fail;
	}
	memset(bp, 0, sizeof(ALLEGRO_BITMAP *) * height);
	for(i = 0; i < height; i++)
	{
		bp[i] = malloc(sizeof(ALLEGRO_BITMAP *) * width);
		if(!bp[i])
		{
			goto fail;
		}
		memset(bp[i], 0, sizeof(ALLEGRO_BITMAP *) * width);
	}
	return bp;

	fail:
	{
		destroy_bitmap_array(bp, width, height);
		return NULL;
	}
}

bool quixel_add_canvas_layer(QUIXEL_CANVAS * cp)
{
	QUIXEL_CANVAS_LAYER ** old_layer;
	int layer_size;
	int i;

	old_layer = cp->layer;
	layer_size = sizeof(QUIXEL_CANVAS_LAYER *) * cp->layer_max + 1;
	cp->layer = malloc(layer_size);
	if(cp->layer)
	{
		memset(cp->layer, 0, layer_size);
		for(i = 0; i < cp->layer_max; i++)
		{
			cp->layer[i] = old_layer[i];
		}
		cp->layer[cp->layer_max] = quixel_create_canvas_layer();
		if(cp->layer[cp->layer_max])
		{
			if(cp->layer_width > 0 && cp->layer_height > 0)
			{
				cp->layer[cp->layer_max]->bitmap = create_bitmap_array(cp->layer_width, cp->layer_height);
			}
			cp->layer_max++;
			free(old_layer);
			return true;
		}
	}
	cp->layer = old_layer;
	return false;
}

bool quixel_remove_canvas_layer(QUIXEL_CANVAS * cp, int layer)
{
	QUIXEL_CANVAS_LAYER ** old_layer;
	int layer_size;
	int i;

	old_layer = cp->layer;
	layer_size = sizeof(QUIXEL_CANVAS_LAYER *) * cp->layer_max - 1;
	cp->layer = malloc(layer_size);
	if(cp->layer)
	{
		memset(cp->layer, 0, layer_size);
		for(i = 0; i < layer; i++)
		{
			cp->layer[i] = old_layer[i];
		}
		for(i = layer; i < cp->layer_max - 1; i++)
		{
			cp->layer[i] = old_layer[i + 1];
		}
		cp->layer_max--;
		if(cp->layer_max > 0)
		{
			destroy_bitmap_array(old_layer[cp->layer_max]->bitmap, cp->layer_width, cp->layer_height);
			free(old_layer[cp->layer_max]);
		}
		free(old_layer);
		return true;
	}
	cp->layer = old_layer;
	return false;
}

bool quixel_add_canvas_frame(QUIXEL_CANVAS * cp, const char * name, int x, int y, int width, int height)
{
	QUIXEL_CANVAS_FRAME ** old_frame;
	int frame_size;
	int i;

	old_frame = cp->frame;
	frame_size = sizeof(QUIXEL_CANVAS_FRAME *) * cp->frame_max + 1;
	cp->frame = malloc(frame_size);
	if(cp->frame)
	{
		memset(cp->frame, 0, frame_size);
		for(i = 0; i < cp->frame_max; i++)
		{
			cp->frame[i] = old_frame[i];
		}
		cp->frame[cp->frame_max] = quixel_create_canvas_frame(name, x, y, width, height);
		if(cp->frame[cp->frame_max])
		{
			cp->frame_max++;
			free(old_frame);
			return true;
		}
	}
	cp->frame = old_frame;
	return false;
}

bool quixel_remove_canvas_frame(QUIXEL_CANVAS * cp, int frame)
{
	QUIXEL_CANVAS_FRAME ** old_frame;
	int frame_size;
	int i;

	old_frame = cp->frame;
	frame_size = sizeof(QUIXEL_CANVAS_FRAME *) * cp->frame_max - 1;
	cp->frame = malloc(frame_size);
	if(cp->frame)
	{
		memset(cp->frame, 0, frame_size);
		for(i = 0; i < frame; i++)
		{
			cp->frame[i] = old_frame[i];
		}
		for(i = frame; i < cp->frame_max - 1; i++)
		{
			cp->frame[i] = old_frame[i + 1];
		}
		cp->frame_max--;
		free(old_frame);
		return true;
	}
	cp->frame = old_frame;
	return false;
}

void quixel_shift_canvas_bitmap_array(QUIXEL_CANVAS * cp, int shift_x, int shift_y)
{
	ALLEGRO_BITMAP *** bitmap;
	int new_width, new_height;
	int i, j, k;
	int tx, ty;

	new_width = cp->layer_width + shift_x;
	new_height = cp->layer_height + shift_y;

	for(i = 0; i < cp->layer_max; i++)
	{
		/* create a new array */
		bitmap = create_bitmap_array(new_width, new_height);
		if(bitmap)
		{
			/* copy old data to new array */
			for(j = 0; j < cp->layer_height; j++)
			{
				for(k = 0; k < cp->layer_width; k++)
				{
					tx = k + shift_x;
					ty = j + shift_y;
					if(tx >= 0 && tx < cp->layer_width && ty >= 0 && ty < cp->layer_height)
					{
						bitmap[j][k] = cp->layer[i]->bitmap[ty][tx];
					}
				}
			}

			/* destroy bitmaps that are no longer part of the new array */
			for(j = new_height; j < cp->layer_height; j++)
			{
				for(k = new_width; k < cp->layer_width; k++)
				{
					if(cp->layer[i]->bitmap[j][k])
					{
						al_destroy_bitmap(cp->layer[i]->bitmap[j][k]);
					}
				}
			}

			/* destroy the old array */
			destroy_bitmap_array(cp->layer[i]->bitmap, cp->layer_width, cp->layer_height);

			/* point to new array */
			cp->layer[i]->bitmap = bitmap;
		}
	}
	printf("expand %d %d\n", new_width, new_height);
	cp->layer_width = new_width;
	cp->layer_height = new_height;
}

bool quixel_resize_canvas_bitmap_array(QUIXEL_CANVAS * cp, int width, int height)
{
	ALLEGRO_BITMAP *** bitmap;
	int i, j, k;

	if(cp->layer_width != width || cp->layer_height != height)
	{
		for(i = 0; i < cp->layer_max; i++)
		{
			/* create a new array */
			bitmap = create_bitmap_array(width, height);
			if(bitmap)
			{
				/* copy old data to new array */
				for(j = 0; j < cp->layer_height; j++)
				{
					for(k = 0; k < cp->layer_width; k++)
					{
						if(k >= 0 && k < cp->layer_width && j >= 0 && j < cp->layer_height)
						{
							bitmap[j][k] = cp->layer[i]->bitmap[j][k];
						}
					}
				}

				/* destroy bitmaps that are no longer part of the new array */
				for(j = height; j < cp->layer_height; j++)
				{
					for(k = width; k < cp->layer_width; k++)
					{
						if(cp->layer[i]->bitmap[j][k])
						{
							al_destroy_bitmap(cp->layer[i]->bitmap[j][k]);
						}
					}
				}

				/* destroy the old array */
				destroy_bitmap_array(cp->layer[i]->bitmap, cp->layer_width, cp->layer_height);

				/* point to new array */
				cp->layer[i]->bitmap = bitmap;
			}
		}
		cp->layer_width = width;
		cp->layer_height = height;
	}
	return true;
}

bool quixel_expand_canvas(QUIXEL_CANVAS * cp, int layer, int x, int y)
{
	ALLEGRO_STATE old_state;

	if(layer >= cp->layer_max)
	{
		return false;
	}

	/*if(!quixel_resize_canvas_bitmap_array(cp, x / cp->bitmap_size + 1, y / cp->bitmap_size + 1))
	{
		printf("Failed to resize bitmap array!\n");
		return false;
	} */

	if(!cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size])
	{
		al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS | ALLEGRO_STATE_TARGET_BITMAP);
		al_set_new_bitmap_flags(0);
		cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size] = al_create_bitmap(cp->bitmap_size, cp->bitmap_size);
		if(cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size])
		{
			al_set_target_bitmap(cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size]);
			al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
		}
		else
		{
			printf("Failed to expand canvas!\n");
			return false;
		}
		al_restore_state(&old_state);
	}
	return true;
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

void quixel_render_canvas_layer(QUIXEL_CANVAS * cp, int i, int x, int y, int scale, float ox, float oy, int width, int height)
{
	int j, k;

	for(j = 0; j < cp->layer_height; j++)
	{
		for(k = 0; k < cp->layer_width; k++)
		{
			if(i < cp->layer_max)
			{
				if(!(cp->layer[i]->flags & QUIXEL_CANVAS_FLAG_HIDDEN) && cp->layer[i]->bitmap[j][k] && bitmap_visible(cp, k, j, x, y, width, height, scale))
				{
					t3f_draw_scaled_bitmap(cp->layer[i]->bitmap[j][k], t3f_color_white, ox + (k * cp->bitmap_size - x) * scale, oy + (j * cp->bitmap_size - y) * scale, 0, cp->bitmap_size * scale, cp->bitmap_size * scale, 0);
				}
			}
		}
	}
}

void quixel_render_canvas(QUIXEL_CANVAS * cp, int x, int y, int scale, float ox, float oy, int width, int height)
{
//	int cx, cy, cw, ch;
	int i;

//	al_get_clipping_rectangle(&cx, &cy, &cw, &ch);
//	al_set_clipping_rectangle(x, y, width, height);
	for(i = 0; i < cp->layer_max; i++)
	{
		quixel_render_canvas_layer(cp, i, x, y, scale, ox, oy, width, height);
	}
//	al_set_clipping_rectangle(cx, cy, cw, ch);
}
