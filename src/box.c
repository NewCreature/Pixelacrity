#include "t3f/t3f.h"
#include "box.h"

static void quixel_initialize_box_handle(QUIXEL_BOX_HANDLE * hp, int type, int offset_x, int offset_y, int * link_x, int * link_y)
{
	hp->type = type;
	hp->offset_x = offset_x;
	hp->offset_y = offset_y;
	hp->link_x = link_x;
	hp->link_y = link_y;
}

void quixel_initialize_box(QUIXEL_BOX * bp, int view_zoom, int x, int y, int width, int height, ALLEGRO_BITMAP * handle_bitmap)
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

	quixel_initialize_box_handle(&bp->handle[0], QUIXEL_BOX_HANDLE_TYPE_TOP_LEFT, -1, -1, &bp->start_x, &bp->start_y);
	quixel_initialize_box_handle(&bp->handle[1], QUIXEL_BOX_HANDLE_TYPE_TOP_RIGHT, view_zoom, -1, &bp->end_x, &bp->start_y);
	quixel_initialize_box_handle(&bp->handle[2], QUIXEL_BOX_HANDLE_TYPE_BOTTOM_LEFT, -1, view_zoom, &bp->start_x, &bp->end_y);
	quixel_initialize_box_handle(&bp->handle[3], QUIXEL_BOX_HANDLE_TYPE_BOTTOM_RIGHT, view_zoom, view_zoom, &bp->end_x, &bp->end_y);
	if(bp->width > 1)
	{
		quixel_initialize_box_handle(&bp->handle[4], QUIXEL_BOX_HANDLE_TYPE_TOP, 0, -1, &bp->middle_x, &bp->start_y);
		quixel_initialize_box_handle(&bp->handle[5], QUIXEL_BOX_HANDLE_TYPE_BOTTOM, 0, view_zoom, &bp->middle_x, &bp->end_y);
	}
	if(bp->height > 1)
	{
		quixel_initialize_box_handle(&bp->handle[6], QUIXEL_BOX_HANDLE_TYPE_LEFT, -1, 0, &bp->start_x, &bp->middle_y);
		quixel_initialize_box_handle(&bp->handle[7], QUIXEL_BOX_HANDLE_TYPE_RIGHT, view_zoom, 0, &bp->end_x, &bp->middle_y);
	}
}

void quixel_box_logic(QUIXEL_BOX * bp, int view_x, int view_y, int view_zoom, int offset_x, int offset_y)
{
	int i;
	int peg_size = al_get_bitmap_width(bp->bitmap);
	int peg_offset = peg_size / 2;

	for(i = 0; i < 10; i++)
	{
		if(bp->handle[i].type != QUIXEL_BOX_HANDLE_TYPE_NONE)
		{
			if(t3f_mouse_x - offset_x >= *bp->handle[i].link_x - peg_offset && t3f_mouse_x - offset_x <= *bp->handle[i].link_x + peg_offset && t3f_mouse_y - offset_y >= *bp->handle[i].link_y - peg_offset && t3f_mouse_y - offset_y <= *bp->handle[i].link_y + peg_offset)
			{

				printf("peg %d\n", i);
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
					t3f_draw_bitmap(bp->bitmap, t3f_color_white, offset_x + start_x + bp->handle[i].offset_x - peg_offset, offset_y + start_y + bp->handle[i].offset_y - peg_offset, 0, 0);
				}
			}
		}
	}
}
