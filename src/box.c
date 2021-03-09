#include "t3f/t3f.h"
#include "box.h"

static void quixel_initialize_box_handle(QUIXEL_BOX_HANDLE * hp, int view_x, int view_y, int view_zoom, int type, int offset_x, int offset_y, int * link_x, int * link_y)
{
	hp->type = type;
	hp->offset_x = offset_x;
	hp->offset_y = offset_y;
	hp->link_x = link_x;
	hp->link_y = link_y;
	hp->screen_x = (*link_x - view_x) * view_zoom + offset_x;
	hp->screen_y = (*link_y - view_y) * view_zoom + offset_y;
}

void quixel_initialize_box(QUIXEL_BOX * bp, int x, int y, int width, int height, ALLEGRO_BITMAP * handle_bitmap)
{
	memset(bp, 0, sizeof(QUIXEL_BOX));
	bp->start_x = x;
	bp->start_y = y;
	bp->width = width;
	bp->height = height;
	bp->end_x = x + width - 1;
	bp->end_y = y + height - 1;
	bp->middle_x = bp->start_x + bp->width / 2;
	bp->middle_y = bp->start_y + bp->height / 2;
	bp->bitmap = handle_bitmap;

}

/* update boxes each frame before running the logic to modify boxes */
void quixel_update_box(QUIXEL_BOX * bp, int view_x, int view_y, int view_zoom)
{
	quixel_initialize_box_handle(&bp->handle[0], view_x, view_y, view_zoom, QUIXEL_BOX_HANDLE_TYPE_TOP_LEFT, -1, -1, &bp->start_x, &bp->start_y);
	quixel_initialize_box_handle(&bp->handle[1], view_x, view_y, view_zoom, QUIXEL_BOX_HANDLE_TYPE_TOP_RIGHT, view_zoom, -1, &bp->end_x, &bp->start_y);
	quixel_initialize_box_handle(&bp->handle[2], view_x, view_y, view_zoom, QUIXEL_BOX_HANDLE_TYPE_BOTTOM_LEFT, -1, view_zoom, &bp->start_x, &bp->end_y);
	quixel_initialize_box_handle(&bp->handle[3], view_x, view_y, view_zoom, QUIXEL_BOX_HANDLE_TYPE_BOTTOM_RIGHT, view_zoom, view_zoom, &bp->end_x, &bp->end_y);
	if(bp->width > 1)
	{
		quixel_initialize_box_handle(&bp->handle[4], view_x, view_y, view_zoom, QUIXEL_BOX_HANDLE_TYPE_TOP, 0, -1, &bp->middle_x, &bp->start_y);
		quixel_initialize_box_handle(&bp->handle[5], view_x, view_y, view_zoom, QUIXEL_BOX_HANDLE_TYPE_BOTTOM, 0, view_zoom, &bp->middle_x, &bp->end_y);
	}
	if(bp->height > 1)
	{
		quixel_initialize_box_handle(&bp->handle[6], view_x, view_y, view_zoom, QUIXEL_BOX_HANDLE_TYPE_LEFT, -1, 0, &bp->start_x, &bp->middle_y);
		quixel_initialize_box_handle(&bp->handle[7], view_x, view_y, view_zoom, QUIXEL_BOX_HANDLE_TYPE_RIGHT, view_zoom, 0, &bp->end_x, &bp->middle_y);
	}
}

/* handle user interaction with boxes */
void quixel_box_logic(QUIXEL_BOX * bp, int view_x, int view_y, int view_zoom, int offset_x, int offset_y)
{
	int i;
	int peg_size = al_get_bitmap_width(bp->bitmap);
	int peg_offset = peg_size / 2;

	if(bp->hover_handle >= 0 && bp->handle[bp->hover_handle].state != QUIXEL_BOX_HANDLE_STATE_MOVING)
	{
		bp->hover_handle = -1;
	}
	for(i = 0; i < 10; i++)
	{
		if(bp->handle[i].type != QUIXEL_BOX_HANDLE_TYPE_NONE)
		{
			if(bp->handle[i].state == QUIXEL_BOX_HANDLE_STATE_MOVING)
			{
			}
			else
			{
				if(t3f_mouse_x >= bp->handle[i].screen_x - peg_offset + offset_x && t3f_mouse_x <= bp->handle[i].screen_x + peg_offset + offset_x && t3f_mouse_y >= bp->handle[i].screen_y - peg_offset + offset_y && t3f_mouse_y <= bp->handle[i].screen_y + peg_offset + offset_y)
				{
					bp->handle[i].state = QUIXEL_BOX_HANDLE_STATE_HOVER;
					bp->hover_handle = i;
				}
				else if(t3f_mouse_x < bp->handle[i].screen_x - peg_offset + offset_x || t3f_mouse_x > bp->handle[i].screen_x + peg_offset + offset_x || t3f_mouse_y < bp->handle[i].screen_y - peg_offset + offset_y || t3f_mouse_y > bp->handle[i].screen_y + peg_offset + offset_y)
				{
					bp->handle[i].state = QUIXEL_BOX_HANDLE_STATE_IDLE;
				}
			}
		}
	}
}

void quixel_box_render(QUIXEL_BOX * bp, int style, int view_x, int view_y, int view_zoom, int offset_x, int offset_y)
{
	int i;
	int peg_size = 8;
	int peg_offset = peg_size / 2;
	int start_x, start_y, end_x, end_y;

	if(bp->bitmap)
	{
		peg_size = al_get_bitmap_width(bp->bitmap);
	}
	peg_offset = peg_size / 2;
	if(bp->width > 0 && bp->height > 0)
	{
		start_x = (bp->start_x - view_x) * view_zoom;
		start_y = (bp->start_y - view_y) * view_zoom;
		end_x = (bp->end_x - view_x + 1) * view_zoom;
		end_y = (bp->end_y - view_y + 1) * view_zoom;
		al_draw_rectangle(offset_x + start_x - 1.0 + 0.5, offset_y + start_y - 1.0 + 0.5, offset_x + end_x + 0.5, offset_y + end_y + 0.5, t3f_color_black, 1.0);
		for(i = 0; i < 10; i++)
		{
			if(bp->handle[i].type != QUIXEL_BOX_HANDLE_TYPE_NONE)
			{
				if(bp->bitmap)
				{
					start_x = (*bp->handle[i].link_x - view_x) * view_zoom;
					start_y = (*bp->handle[i].link_y - view_y) * view_zoom;
					t3f_draw_bitmap(bp->bitmap, t3f_color_white, bp->handle[i].screen_x + offset_x - peg_offset, bp->handle[i].screen_y + offset_y - peg_offset, 0, 0);
				}
			}
		}
	}
}
