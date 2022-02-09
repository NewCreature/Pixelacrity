#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "box.h"
#include "snap.h"

static void pa_initialize_box_handle(PA_BOX_HANDLE * hp, int view_x, int view_y, int view_zoom, int type, int offset_x, int offset_y, int * link_x, int * link_y)
{
	hp->type = type;
	hp->offset_x = offset_x;
	hp->offset_y = offset_y;
	hp->link_x = link_x;
	hp->link_y = link_y;
	hp->screen_x = (*link_x - view_x) * view_zoom + offset_x;
	hp->screen_y = (*link_y - view_y) * view_zoom + offset_y;
}

void pa_setup_box(PA_BOX * bp, int x, int y, int width, int height, float angle)
{
	float angle_length;

	bp->start_x = x;
	bp->start_y = y;
	bp->width = width;
	bp->height = height;
	bp->end_x = x + width - 1;
	bp->end_y = y + height - 1;
	if(fabs(angle) >= 10000)
	{
		angle = 0;
		angle_length = hypot(width, height) / 2.0 + 32;
		bp->angle_x = x + width / 2 + cos(angle) * angle_length;
		bp->angle_y = y + height / 2 + sin(angle) * angle_length;
	}
	bp->angle = angle;
	bp->middle_x = bp->start_x + bp->width / 2;
	bp->middle_y = bp->start_y + bp->height / 2;
}

void pa_initialize_box(PA_BOX * bp, int x, int y, int width, int height)
{
	memset(bp, 0, sizeof(PA_BOX));
	pa_setup_box(bp, x, y, width, height, 10000);
}

static void update_box(PA_BOX * bp)
{
	if(bp->hover_handle >= 0)
	{
		bp->width = abs(bp->end_x - bp->start_x) + 1;
		switch(bp->handle[bp->hover_handle].type)
		{
			case PA_BOX_HANDLE_TYPE_TOP_LEFT:
			{
				if(*bp->handle[bp->hover_handle].link_x > *bp->handle[1].link_x)
				{
					bp->hover_handle = 1;
					bp->start_x = *bp->handle[bp->hover_handle].link_x;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_LEFT:
			{
				if(*bp->handle[bp->hover_handle].link_x > *bp->handle[7].link_x)
				{
					bp->hover_handle = 7;
					bp->start_x = *bp->handle[bp->hover_handle].link_x;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_BOTTOM_LEFT:
			{
				if(*bp->handle[bp->hover_handle].link_x > *bp->handle[3].link_x)
				{
					bp->hover_handle = 3;
					bp->start_x = *bp->handle[bp->hover_handle].link_x;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_TOP_RIGHT:
			{
				if(*bp->handle[bp->hover_handle].link_x < *bp->handle[0].link_x)
				{
					bp->start_x = *bp->handle[bp->hover_handle].link_x;
					bp->hover_handle = 0;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_RIGHT:
			{
				if(*bp->handle[bp->hover_handle].link_x < *bp->handle[6].link_x)
				{
					bp->start_x = *bp->handle[bp->hover_handle].link_x;
					bp->hover_handle = 6;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_BOTTOM_RIGHT:
			{
				if(*bp->handle[bp->hover_handle].link_x < *bp->handle[2].link_x)
				{
					bp->start_x = *bp->handle[bp->hover_handle].link_x;
					bp->hover_handle = 2;
				}
				break;
			}
		}
		bp->height = abs(bp->end_y - bp->start_y) + 1;
		switch(bp->handle[bp->hover_handle].type)
		{
			case PA_BOX_HANDLE_TYPE_TOP_LEFT:
			{
				if(*bp->handle[bp->hover_handle].link_y > *bp->handle[2].link_y)
				{
					bp->hover_handle = 2;
					bp->start_y = *bp->handle[bp->hover_handle].link_y;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_TOP:
			{
				if(*bp->handle[bp->hover_handle].link_y > *bp->handle[5].link_y)
				{
					bp->hover_handle = 5;
					bp->start_y = *bp->handle[bp->hover_handle].link_y;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_TOP_RIGHT:
			{
				if(*bp->handle[bp->hover_handle].link_y > *bp->handle[3].link_y)
				{
					bp->hover_handle = 3;
					bp->start_y = *bp->handle[bp->hover_handle].link_y;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_BOTTOM_LEFT:
			{
				if(*bp->handle[bp->hover_handle].link_y < *bp->handle[0].link_y)
				{
					bp->start_y = *bp->handle[bp->hover_handle].link_y;
					bp->hover_handle = 0;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_BOTTOM:
			{
				if(*bp->handle[bp->hover_handle].link_y < *bp->handle[4].link_y)
				{
					bp->start_y = *bp->handle[bp->hover_handle].link_y;
					bp->hover_handle = 4;
				}
				break;
			}
			case PA_BOX_HANDLE_TYPE_BOTTOM_RIGHT:
			{
				if(*bp->handle[bp->hover_handle].link_y < *bp->handle[1].link_y)
				{
					bp->start_y = *bp->handle[bp->hover_handle].link_y;
					bp->hover_handle = 1;
				}
				break;
			}
		}
		if(bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_ANGLE)
		{
			bp->angle = atan2(bp->angle_y - bp->middle_y, bp->angle_x - bp->middle_x);
		}
	}
	pa_setup_box(bp, bp->start_x, bp->start_y, bp->width, bp->height, bp->angle);
}

/* update boxes each frame before running the logic to modify boxes */
void pa_update_box_handles(PA_BOX * bp, int view_x, int view_y, int view_zoom, bool floating)
{
	int offset;

	pa_initialize_box_handle(&bp->handle[0], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_TOP_LEFT, -1, -1, &bp->start_x, &bp->start_y);
	pa_initialize_box_handle(&bp->handle[1], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_TOP_RIGHT, view_zoom, -1, &bp->end_x, &bp->start_y);
	pa_initialize_box_handle(&bp->handle[2], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_BOTTOM_LEFT, -1, view_zoom, &bp->start_x, &bp->end_y);
	pa_initialize_box_handle(&bp->handle[3], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_BOTTOM_RIGHT, view_zoom, view_zoom, &bp->end_x, &bp->end_y);
	if(bp->width > 1)
	{
		offset = bp->width % 2 == 0 ? 0 : view_zoom / 2;
		pa_initialize_box_handle(&bp->handle[4], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_TOP, offset, -1, &bp->middle_x, &bp->start_y);
		pa_initialize_box_handle(&bp->handle[5], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_BOTTOM, offset, view_zoom, &bp->middle_x, &bp->end_y);
	}
	else
	{
		offset = bp->width % 2 == 0 ? 0 : view_zoom / 2;
		pa_initialize_box_handle(&bp->handle[4], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_NONE, offset, -1, &bp->middle_x, &bp->start_y);
		pa_initialize_box_handle(&bp->handle[5], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_NONE, offset, view_zoom, &bp->middle_x, &bp->end_y);
	}
	if(bp->height > 1)
	{
		offset = bp->height % 2 == 0 ? 0 : view_zoom / 2;
		pa_initialize_box_handle(&bp->handle[6], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_LEFT, -1, offset, &bp->start_x, &bp->middle_y);
		pa_initialize_box_handle(&bp->handle[7], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_RIGHT, view_zoom, offset, &bp->end_x, &bp->middle_y);
	}
	else
	{
		offset = bp->height % 2 == 0 ? 0 : view_zoom / 2;
		pa_initialize_box_handle(&bp->handle[6], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_NONE, -1, offset, &bp->start_x, &bp->middle_y);
		pa_initialize_box_handle(&bp->handle[7], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_NONE, view_zoom, offset, &bp->end_x, &bp->middle_y);
	}
	pa_initialize_box_handle(&bp->handle[8], view_x, view_y, view_zoom, PA_BOX_HANDLE_TYPE_NONE, view_zoom, 0, &bp->angle_x, &bp->angle_y);
}

void pa_get_box_hover_handle(PA_BOX * bp, int offset_x, int offset_y, int peg_offset)
{
	int i;

	bp->hover_handle = -1;
	for(i = 0; i < 10; i++)
	{
		if(bp->handle[i].type != PA_BOX_HANDLE_TYPE_NONE)
		{
			if(t3gui_get_mouse_x() >= bp->handle[i].screen_x - peg_offset + offset_x && t3gui_get_mouse_x() <= bp->handle[i].screen_x + peg_offset + offset_x && t3gui_get_mouse_y() >= bp->handle[i].screen_y - peg_offset + offset_y && t3gui_get_mouse_y() <= bp->handle[i].screen_y + peg_offset + offset_y)
			{
				bp->hover_handle = i;
			}
		}
	}
}

/* handle user interaction with boxes */
void pa_box_logic(PA_BOX * bp, int view_x, int view_y, int view_zoom, int offset_x, int offset_y, bool snap, ALLEGRO_BITMAP * handle_bitmap, bool floating)
{
	int peg_size = handle_bitmap ? al_get_bitmap_width(handle_bitmap) : 0;
	int peg_offset = peg_size / 2;
	int i;
	float start_x, start_y, end_x, end_y;

	bp->hover_x = (t3gui_get_mouse_x() - offset_x) / view_zoom + view_x;
	bp->hover_y = (t3gui_get_mouse_y() - offset_y) / view_zoom + view_y;
	switch(bp->state)
	{
		case PA_BOX_STATE_IDLE:
		{
			pa_get_box_hover_handle(bp, offset_x, offset_y, peg_offset);
			if(bp->hover_handle < 0)
			{
				if(t3gui_get_mouse_x() >= bp->handle[0].screen_x + offset_x && t3gui_get_mouse_x() <= bp->handle[1].screen_x + offset_x && t3gui_get_mouse_y() >= bp->handle[0].screen_y + offset_y && t3gui_get_mouse_y() <= bp->handle[2].screen_y + offset_y)
				{
					bp->state = PA_BOX_STATE_HOVER;
				}
			}
			else
			{
				bp->state = PA_BOX_STATE_HOVER_HANDLE;
			}
			break;
		}
		case PA_BOX_STATE_HOVER:
		{
			bp->hover_handle = -1;
			for(i = 0; i < 10; i++)
			{
				if(bp->handle[i].type != PA_BOX_HANDLE_TYPE_NONE)
				{
					if(t3gui_get_mouse_x() >= bp->handle[i].screen_x - peg_offset + offset_x && t3gui_get_mouse_x() <= bp->handle[i].screen_x + peg_offset + offset_x && t3gui_get_mouse_y() >= bp->handle[i].screen_y - peg_offset + offset_y && t3gui_get_mouse_y() <= bp->handle[i].screen_y + peg_offset + offset_y)
					{
						bp->hover_handle = i;
					}
				}
			}
			if(bp->hover_handle >= 0)
			{
				bp->state = PA_BOX_STATE_HOVER_HANDLE;
			}
			else if(t3gui_get_mouse_x() < bp->handle[0].screen_x + offset_x || t3gui_get_mouse_x() > bp->handle[1].screen_x + offset_x || t3gui_get_mouse_y() < bp->handle[0].screen_y + offset_y || t3gui_get_mouse_y() > bp->handle[2].screen_y + offset_y)
			{
				bp->state = PA_BOX_STATE_IDLE;
			}
			break;
		}
		case PA_BOX_STATE_HOVER_HANDLE:
		{
			if(t3gui_get_mouse_x() < bp->handle[bp->hover_handle].screen_x - peg_offset + offset_x || t3gui_get_mouse_x() > bp->handle[bp->hover_handle].screen_x + peg_offset + offset_x || t3gui_get_mouse_y() < bp->handle[bp->hover_handle].screen_y - peg_offset + offset_y || t3gui_get_mouse_y() > bp->handle[bp->hover_handle].screen_y + peg_offset + offset_y)
			{
				bp->hover_handle = -1;
				bp->state = PA_BOX_STATE_IDLE;
			}
			break;
		}
		case PA_BOX_STATE_MOVING:
		{
			start_x = bp->click_x;
			start_y = bp->click_y;
			end_x = bp->hover_x;
			end_y = bp->hover_y;
			if(snap)
			{
				pa_snap_coordinates(start_x, start_y, &end_x, &end_y, 0, ALLEGRO_PI / 2.0);
			}
			pa_setup_box(bp, end_x - (bp->click_x - bp->click_start_x), end_y - (bp->click_y - bp->click_start_y), bp->width, bp->height, bp->angle);
			pa_update_box_handles(bp, view_x, view_y, view_zoom, floating);
			bp->click_tick++;
			break;
		}
		case PA_BOX_STATE_DRAWING:
		case PA_BOX_STATE_RESIZING:
		{
			if(bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_TOP_LEFT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_TOP_RIGHT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_BOTTOM_LEFT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_BOTTOM_RIGHT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_LEFT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_RIGHT)
			{
				bp->handle[bp->hover_handle].screen_x = t3gui_get_mouse_x() - offset_x;
				*bp->handle[bp->hover_handle].link_x = (bp->handle[bp->hover_handle].screen_x) / view_zoom + view_x;
				bp->handle[bp->hover_handle].screen_x = (*bp->handle[bp->hover_handle].link_x - view_x) * view_zoom + bp->handle[bp->hover_handle].offset_x;
			}
			if(bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_TOP_LEFT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_TOP_RIGHT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_BOTTOM_LEFT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_BOTTOM_RIGHT || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_TOP || bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_BOTTOM)
			{
				bp->handle[bp->hover_handle].screen_y = t3gui_get_mouse_y() - offset_y;
				*bp->handle[bp->hover_handle].link_y = (bp->handle[bp->hover_handle].screen_y) / view_zoom + view_y;
				bp->handle[bp->hover_handle].screen_y = (*bp->handle[bp->hover_handle].link_y - view_y) * view_zoom + bp->handle[bp->hover_handle].offset_y;
			}
			update_box(bp);
			pa_update_box_handles(bp, view_x, view_y, view_zoom, floating);
			break;
		}
		case PA_BOX_STATE_ROTATING:
		{
			if(bp->handle[bp->hover_handle].type == PA_BOX_HANDLE_TYPE_ANGLE)
			{
				bp->handle[bp->hover_handle].screen_x = t3gui_get_mouse_x() - offset_x;
				*bp->handle[bp->hover_handle].link_x = (bp->handle[bp->hover_handle].screen_x) / view_zoom + view_x;
				bp->handle[bp->hover_handle].screen_y = t3gui_get_mouse_y() - offset_y;
				*bp->handle[bp->hover_handle].link_y = (bp->handle[bp->hover_handle].screen_y) / view_zoom + view_y;
			}
			update_box(bp);
			pa_update_box_handles(bp, view_x, view_y, view_zoom, floating);
			break;
		}
	}
}

void pa_box_render(PA_BOX * bp, int style, int view_x, int view_y, int view_zoom, int offset_x, int offset_y, ALLEGRO_COLOR color, ALLEGRO_BITMAP * handle_bitmap)
{
	int i;
	int peg_size = 8;
	int peg_offset = peg_size / 2;
	int start_x, start_y, end_x, end_y;

	if(handle_bitmap)
	{
		peg_size = al_get_bitmap_width(handle_bitmap);
	}
	peg_offset = peg_size / 2;
	if(bp->width > 0 && bp->height > 0)
	{
		start_x = (bp->start_x - view_x) * view_zoom;
		start_y = (bp->start_y - view_y) * view_zoom;
		end_x = (bp->end_x - view_x + 1) * view_zoom;
		end_y = (bp->end_y - view_y + 1) * view_zoom;
		al_draw_rectangle(offset_x + start_x - 1.0 + 0.5, offset_y + start_y - 1.0 + 0.5, offset_x + end_x + 0.5, offset_y + end_y + 0.5, color, style);
		for(i = 0; i < 10; i++)
		{
//			printf("box %d %d\n", i, bp->handle[i].type);
			if(bp->handle[i].type != PA_BOX_HANDLE_TYPE_NONE)
			{
				if(handle_bitmap)
				{
					start_x = (*bp->handle[i].link_x - view_x) * view_zoom;
					start_y = (*bp->handle[i].link_y - view_y) * view_zoom;
					t3f_draw_bitmap(handle_bitmap, (i == bp->hover_handle && bp->state != PA_BOX_STATE_DRAWING) ? al_map_rgba_f(0.0, 1.0, 0.0, 1.0) : t3f_color_white, bp->handle[i].screen_x + offset_x - peg_offset, bp->handle[i].screen_y + offset_y - peg_offset, 0, 0);
				}
			}
		}
	}
}
