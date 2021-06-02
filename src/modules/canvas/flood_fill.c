#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_helpers.h"
#include "modules/primitives.h"
#include "modules/color.h"
#include "modules/queue.h"

static ALLEGRO_COLOR get_pixel(QUIXEL_CANVAS * cp, int layer, int x, int y, ALLEGRO_BITMAP ** current_bp, int * ox, int * oy)
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

static void put_pixel(QUIXEL_CANVAS * cp, int layer, int ox, int oy, ALLEGRO_COLOR color, ALLEGRO_BITMAP * current_bp)
{
	al_set_target_bitmap(current_bp);
	al_put_pixel(ox, oy, color);
}

bool quixel_flood_fill_canvas(QUIXEL_CANVAS * cp, int layer, int start_x, int start_y, ALLEGRO_COLOR color)
{
	QUIXEL_QUEUE * qp;
	ALLEGRO_COLOR old_color;
	ALLEGRO_COLOR current_color;
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp;
	ALLEGRO_BITMAP * current_bp;
	int x, y, ox, oy;
	int bx, by, bwidth, bheight, right, bottom;
	int i, j;
	bool ret = true;

	if(layer >= cp->layer_max)
	{
		return false;
	}
	bp = cp->layer[layer]->bitmap[start_y / cp->bitmap_size][start_x / cp->bitmap_size];
	if(!bp)
	{
		return false;
	}
	quixel_get_canvas_dimensions(cp, &bx, &by, &bwidth, &bheight, 0);
	right = bx + bwidth - 1;
	bottom = by + bheight - 1;
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	qp = quixel_create_queue();
	if(qp)
	{
		old_color = get_pixel(cp, layer, start_x, start_y, &current_bp, &ox, &oy);
		if(quixel_color_equal(old_color, color))
		{
			ret = true;
			goto cleanup;
		}
		put_pixel(cp, layer, ox, oy, color, current_bp);
		quixel_queue_push(qp, start_x, start_y);
		while(quixel_queue_pop(qp, &x, &y))
		{
			current_color = get_pixel(cp, layer, x - 1, y, &current_bp, &ox, &oy);
			if(quixel_color_equal(current_color, old_color))
			{
				if(x - 1 < bx)
				{
					ret = false;
					goto cleanup;
				}
				put_pixel(cp, layer, ox, oy, color, current_bp);
				quixel_queue_push(qp, x - 1, y);
			}
			current_color = get_pixel(cp, layer, x + 1, y, &current_bp, &ox, &oy);
			if(quixel_color_equal(current_color, old_color))
			{
				if(x + 1 > right)
				{
					ret = false;
					goto cleanup;
				}
				put_pixel(cp, layer, ox, oy, color, current_bp);
				quixel_queue_push(qp, x + 1, y);
			}
			current_color = get_pixel(cp, layer, x, y - 1, &current_bp, &ox, &oy);
			if(quixel_color_equal(current_color, old_color))
			{
				if(y - 1 < by)
				{
					ret = false;
					goto cleanup;
				}
				put_pixel(cp, layer, ox, oy, color, current_bp);
				quixel_queue_push(qp, x, y - 1);
			}
			current_color = get_pixel(cp, layer, x, y + 1, &current_bp, &ox, &oy);
			if(quixel_color_equal(current_color, old_color))
			{
				if(y + 1 > bottom)
				{
					ret = false;
					goto cleanup;
				}
				put_pixel(cp, layer, ox, oy, color, current_bp);
				quixel_queue_push(qp, x, y + 1);
			}
		}
		quixel_destroy_queue(qp);
	}
	cleanup:
	{
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
		al_restore_state(&old_state);
	}
	return ret;
}
