#include "t3f/t3f.h"
#include "canvas.h"
#include "canvas_helpers.h"
#include "primitives.h"
#include "color.h"

typedef struct
{

	int x;
	int y;

} QUIXEL_QUEUE_ITEM;

typedef struct
{

	QUIXEL_QUEUE_ITEM * item;
	int item_size;
	int item_count;

} QUIXEL_QUEUE;

QUIXEL_QUEUE * quixel_create_queue(int size)
{
	QUIXEL_QUEUE * qp;

	qp = malloc(sizeof(QUIXEL_QUEUE));
	if(!qp)
	{
		goto fail;
	}
	qp->item = malloc(sizeof(QUIXEL_QUEUE_ITEM) * size);
	if(!qp->item)
	{
		goto fail;
	}
	memset(qp->item, 0, sizeof(QUIXEL_QUEUE_ITEM) * size);
	qp->item_size = size;
	qp->item_count = 0;

	return qp;

	fail:
	{
		if(qp->item)
		{
			if(qp)
			{
				if(qp->item)
				{
					free(qp->item);
				}
				free(qp);
			}
		}
		return NULL;
	}
}

void quixel_destroy_queue(QUIXEL_QUEUE * qp)
{
	free(qp->item);
	free(qp);
}

void quixel_queue_push(QUIXEL_QUEUE * qp, int x, int y)
{
	if(qp->item_count < qp->item_size)
	{
		qp->item[qp->item_count].x = x;
		qp->item[qp->item_count].y = y;
		qp->item_count++;
	}
}

bool quixel_queue_pop(QUIXEL_QUEUE * qp, int * x, int * y)
{
	if(qp->item_count > 0)
	{
		*x = qp->item[qp->item_count - 1].x;
		*y = qp->item[qp->item_count - 1].y;
		qp->item_count--;
		return true;
	}
	return false;
}

static void put_pixel(QUIXEL_CANVAS * cp, int layer, int x, int y, ALLEGRO_COLOR color)
{
	al_put_pixel(x % cp->bitmap_size, y % cp->bitmap_size, color);
}

void quixel_flood_fill_canvas(QUIXEL_CANVAS * cp, int layer, int start_x, int start_y, ALLEGRO_COLOR color)
{
	QUIXEL_QUEUE * qp;
	ALLEGRO_COLOR old_color;
	ALLEGRO_COLOR current_color;
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * bp;
	ALLEGRO_BITMAP * current_bp;
	int x, y;

	if(layer >= cp->layer_max)
	{
		return;
	}
	bp = cp->layer[layer]->bitmap[start_y / cp->bitmap_size][start_x / cp->bitmap_size];
	if(!bp)
	{
		return;
	}
	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(bp);
	al_lock_bitmap(bp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
	qp = quixel_create_queue(65536);
	if(qp)
	{
		old_color = quixel_get_canvas_pixel(cp, layer, start_x, start_y);
		put_pixel(cp, layer, start_x, start_y, color);
		quixel_queue_push(qp, start_x, start_y);
		while(quixel_queue_pop(qp, &x, &y))
		{
			current_bp = cp->layer[layer]->bitmap[y / cp->bitmap_size][x / cp->bitmap_size];
			if(current_bp == bp)
			{
				current_color = quixel_get_canvas_pixel(cp, layer, x - 1, y);
				if(quixel_color_equal(current_color, old_color))
				{
					put_pixel(cp, layer, x - 1, y, color);
					quixel_queue_push(qp, x - 1, y);
				}
				current_color = quixel_get_canvas_pixel(cp, layer, x + 1, y);
				if(quixel_color_equal(current_color, old_color))
				{
					put_pixel(cp, layer, x + 1, y, color);
					quixel_queue_push(qp, x + 1, y);
				}
				current_color = quixel_get_canvas_pixel(cp, layer, x, y - 1);
				if(quixel_color_equal(current_color, old_color))
				{
					put_pixel(cp, layer, x, y - 1, color);
					quixel_queue_push(qp, x, y - 1);
				}
				current_color = quixel_get_canvas_pixel(cp, layer, x, y + 1);
				if(quixel_color_equal(current_color, old_color))
				{
					put_pixel(cp, layer, x, y + 1, color);
					quixel_queue_push(qp, x, y + 1);
				}
			}
		}
		quixel_destroy_queue(qp);
	}
	al_unlock_bitmap(bp);
	al_restore_state(&old_state);
}
