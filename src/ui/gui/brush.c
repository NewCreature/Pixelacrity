#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/color.h"
#include "modules/bitmap.h"
#include "instance.h"

int pa_gui_brush_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	ALLEGRO_COLOR color;
	ALLEGRO_BITMAP * bp = *(ALLEGRO_BITMAP **)d->dp;
	float scale = 1.0;
	float usable_width = d->w - d->theme->state[0].left_margin - d->theme->state[0].right_margin;
	float usable_height = d->h - d->theme->state[0].top_margin - d->theme->state[0].bottom_margin;
	color = *(ALLEGRO_COLOR *)d->dp2;
	float cx, cy;

	switch(msg)
	{
		case MSG_MOUSEDOWN:
		{
			d->id1 = 1;
			break;
		}
		case MSG_DRAW:
		{
			al_draw_filled_rectangle(d->x, d->y, d->x + d->w - 1, d->y + d->h - 1, t3f_color_white);
			if(al_get_bitmap_width(bp) > usable_width)
			{
				scale = usable_width / (float)al_get_bitmap_width(bp);
			}
			if(al_get_bitmap_height(bp) * scale > usable_height)
			{
				scale = usable_height / (float)al_get_bitmap_height(bp);
			}
			cx = d->x + d->w / 2 - (al_get_bitmap_width(bp) * scale) / 2;
			cy = d->y + d->h / 2 - (al_get_bitmap_height(bp) * scale) / 2;
			al_draw_tinted_scaled_bitmap(bp, t3f_color_black, 0, 0, al_get_bitmap_width(bp), al_get_bitmap_height(bp), cx, cy, al_get_bitmap_width(bp) * scale, al_get_bitmap_height(bp) * scale, 0);
			break;
		}
	}
	return D_O_K;
}
