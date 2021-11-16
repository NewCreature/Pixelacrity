#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_helpers.h"
#include "modules/primitives.h"
#include "modules/color.h"
#include "modules/queue.h"
#include "flood_fill.h"

static ALLEGRO_COLOR get_pixel(PA_CANVAS * cp, int layer, int x, int y, ALLEGRO_BITMAP ** current_bp, int * ox, int * oy)
{
	*current_bp = cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size];
	*ox = x % cp->bitmap_size;
	*oy = y % cp->bitmap_size;

	if(!al_is_bitmap_locked(*current_bp))
	{
		al_lock_bitmap(*current_bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
	}
	return al_get_pixel(*current_bp, *ox, *oy);
}

static ALLEGRO_COLOR get_pixel_2(PA_CANVAS * cp, int layer, int x, int y)
{
	ALLEGRO_BITMAP * current_bp = cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size];
	int ox = x % cp->bitmap_size;
	int oy = y % cp->bitmap_size;

	if(current_bp)
	{
		if(!al_is_bitmap_locked(current_bp))
		{
			al_lock_bitmap(current_bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
		}
		return al_get_pixel(current_bp, ox, oy);
	}
	return al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
}

static void put_pixel(PA_CANVAS * cp, int layer, int ox, int oy, ALLEGRO_COLOR color, ALLEGRO_BITMAP * current_bp)
{
	al_set_target_bitmap(current_bp);
	al_put_pixel(ox, oy, color);
}

static void put_pixel_2(PA_CANVAS * cp, int layer, int x, int y, ALLEGRO_COLOR color)
{
	pa_expand_canvas(cp, layer, x, y);
	al_set_target_bitmap(cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size]);
	al_put_pixel(x % cp->bitmap_size, y % cp->bitmap_size, color);
}

static void unlock_canvas_layer(PA_CANVAS * cp, int layer)
{
	ALLEGRO_BITMAP * current_bp;
	int i, j;

	for(i = 0; i < cp->layer_height; i++)
	{
		for(j = 0; j < cp->layer_width; j++)
		{
			current_bp = cp->layer[layer]->bitmap[i][j];
			if(current_bp && al_is_bitmap_locked(current_bp))
			{
				al_unlock_bitmap(current_bp);
			}
		}
	}
}

static ALLEGRO_BITMAP * get_canvas_tile(PA_CANVAS * cp, int layer, int x, int y)
{
	int tx, ty;

	tx = x / cp->bitmap_size;
	ty = y / cp->bitmap_size;

	if(x >= 0 && y >= 0 && tx < cp->layer_width && ty < cp->layer_height)
	{
		return cp->layer[layer]->bitmap[ty][tx];
	}
	return NULL;
}

bool pa_flood_fill_canvas(PA_CANVAS * cp, int layer, int start_x, int start_y, ALLEGRO_COLOR color, PA_QUEUE * out_qp)
{
	PA_QUEUE * qp;
	ALLEGRO_COLOR old_color;
	ALLEGRO_COLOR current_color;
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp;
	ALLEGRO_BITMAP * current_bp;
	int x, y, ox, oy;
	bool ret = true;

	t3f_debug_message("Enter pa_flood_fill_canvas()\n");
	if(layer >= cp->layer_max)
	{
		t3f_debug_message("Fail pa_flood_fill_canvas() 1\n");
		return false;
	}
	bp = cp->layer[layer]->bitmap[start_y / cp->bitmap_size][start_x / cp->bitmap_size];
	if(!bp)
	{
		t3f_debug_message("Fail pa_flood_fill_canvas() 2\n");
		return false;
	}
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	qp = pa_create_queue();
	if(qp)
	{
		old_color = get_pixel(cp, layer, start_x, start_y, &current_bp, &ox, &oy);
		if(pa_color_equal(old_color, color))
		{
			ret = true;
			goto cleanup;
		}
		put_pixel(cp, layer, ox, oy, color, current_bp);
		pa_queue_push(qp, start_x, start_y);
		pa_queue_push(out_qp, start_x, start_y);
		while(pa_queue_pop(qp, &x, &y))
		{
			if(!get_canvas_tile(cp, layer, x - 1, y))
			{
				ret = false;
				goto cleanup;
			}
			current_color = get_pixel(cp, layer, x - 1, y, &current_bp, &ox, &oy);
			if(pa_color_equal(current_color, old_color))
			{
				put_pixel(cp, layer, ox, oy, color, current_bp);
				pa_queue_push(qp, x - 1, y);
				pa_queue_push(out_qp, x - 1, y);
			}
			if(!get_canvas_tile(cp, layer, x + 1, y))
			{
				ret = false;
				goto cleanup;
			}
			current_color = get_pixel(cp, layer, x + 1, y, &current_bp, &ox, &oy);
			if(pa_color_equal(current_color, old_color))
			{
				put_pixel(cp, layer, ox, oy, color, current_bp);
				pa_queue_push(qp, x + 1, y);
				pa_queue_push(out_qp, x + 1, y);
			}
			if(!get_canvas_tile(cp, layer, x, y - 1))
			{
				ret = false;
				goto cleanup;
			}
			current_color = get_pixel(cp, layer, x, y - 1, &current_bp, &ox, &oy);
			if(pa_color_equal(current_color, old_color))
			{
				put_pixel(cp, layer, ox, oy, color, current_bp);
				pa_queue_push(qp, x, y - 1);
				pa_queue_push(out_qp, x, y - 1);
			}
			if(!get_canvas_tile(cp, layer, x, y + 1))
			{
				ret = false;
				goto cleanup;
			}
			current_color = get_pixel(cp, layer, x, y + 1, &current_bp, &ox, &oy);
			if(pa_color_equal(current_color, old_color))
			{
				put_pixel(cp, layer, ox, oy, color, current_bp);
				pa_queue_push(qp, x, y + 1);
				pa_queue_push(out_qp, x, y + 1);
			}
		}
		pa_destroy_queue(qp);
	}
	cleanup:
	{
		t3f_debug_message("Cleanup pa_flood_fill_canvas()\n");
		unlock_canvas_layer(cp, layer);
		al_restore_state(&old_state);
	}
	if(!ret)
	{
		t3f_debug_message("Revert pa_flood_fill_canvas()\n");
		pa_flood_fill_canvas_from_queue(cp, layer, old_color, out_qp);
	}
	t3f_debug_message("Exit pa_flood_fill_canvas()\n");
	return ret;
}

bool pa_flood_fill_canvas_area(PA_CANVAS * cp, int layer, int left, int top, int right, int bottom, int start_x, int start_y, ALLEGRO_COLOR color, PA_QUEUE * out_qp)
{
	PA_QUEUE * qp;
	ALLEGRO_COLOR old_color;
	ALLEGRO_COLOR current_color;
	ALLEGRO_STATE old_state;
	int x, y;
	bool ret = true;

	t3f_debug_message("Enter pa_flood_fill_canvas_area()\n");
	if(layer >= cp->layer_max)
	{
		t3f_debug_message("Fail pa_flood_fill_canvas_area() 1\n");
		return false;
	}
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	qp = pa_create_queue();
	if(qp)
	{
		old_color = get_pixel_2(cp, layer, start_x, start_y);
		if(pa_color_equal(old_color, color))
		{
			ret = true;
			goto cleanup;
		}
		put_pixel_2(cp, layer, start_x, start_y, color);
		pa_queue_push(qp, start_x, start_y);
		pa_queue_push(out_qp, start_x, start_y);
		while(pa_queue_pop(qp, &x, &y))
		{
			if(x >= left && x <= right && y >= top && y <= bottom)
			{
				if(x - 1 >= left)
				{
					current_color = get_pixel_2(cp, layer, x - 1, y);
					if(pa_color_equal(current_color, old_color))
					{
						put_pixel_2(cp, layer, x - 1, y, color);
						pa_queue_push(qp, x - 1, y);
						pa_queue_push(out_qp, x - 1, y);
					}
				}
				if(x + 1 <= right)
				{
					current_color = get_pixel_2(cp, layer, x + 1, y);
					if(pa_color_equal(current_color, old_color))
					{
						put_pixel_2(cp, layer, x + 1, y, color);
						pa_queue_push(qp, x + 1, y);
						pa_queue_push(out_qp, x + 1, y);
					}
				}
				if(y - 1 >= top)
				{
					current_color = get_pixel_2(cp, layer, x, y - 1);
					if(pa_color_equal(current_color, old_color))
					{
						put_pixel_2(cp, layer, x, y - 1, color);
						pa_queue_push(qp, x, y - 1);
						pa_queue_push(out_qp, x, y - 1);
					}
				}
				if(y + 1 <= bottom)
				{
					current_color = get_pixel_2(cp, layer, x, y + 1);
					if(pa_color_equal(current_color, old_color))
					{
						put_pixel_2(cp, layer, x, y + 1, color);
						pa_queue_push(qp, x, y + 1);
						pa_queue_push(out_qp, x, y + 1);
					}
				}
			}
		}
		pa_destroy_queue(qp);
	}
	cleanup:
	{
		t3f_debug_message("Cleanup pa_flood_fill_canvas_area()\n");
		unlock_canvas_layer(cp, layer);
		al_restore_state(&old_state);
	}
	if(!ret)
	{
		t3f_debug_message("Revert pa_flood_fill_canvas_area()\n");
		pa_flood_fill_canvas_from_queue(cp, layer, old_color, out_qp);
	}
	t3f_debug_message("Exit pa_flood_fill_canvas_area()\n");
	return ret;
}

void pa_flood_fill_canvas_from_queue(PA_CANVAS * cp, int layer, ALLEGRO_COLOR color, PA_QUEUE * qp)
{
	ALLEGRO_STATE old_state;
	PA_QUEUE_NODE * current_node;
	ALLEGRO_COLOR current_color;
	ALLEGRO_BITMAP * current_bp;
	int ox, oy;

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	current_node = qp->current;
	while(current_node)
	{
		current_color = get_pixel(cp, layer, current_node->x, current_node->y, &current_bp, &ox, &oy);
		put_pixel(cp, layer, ox, oy, color, current_bp);
		current_node = current_node->previous;
	}
	al_restore_state(&old_state);
	unlock_canvas_layer(cp, layer);
}
