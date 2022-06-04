#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/color.h"
#include "modules/bitmap.h"
#include "instance.h"

int pa_gui_brush_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	ALLEGRO_COLOR color;
	PA_CANVAS_EDITOR * cep = (PA_CANVAS_EDITOR *)d->dp;
	ALLEGRO_BITMAP * bp = cep->brush;
	float scale = 1.0;
	float usable_width = d->w - d->theme->state[0].left_margin - d->theme->state[0].right_margin;
	float usable_height = d->h - d->theme->state[0].top_margin - d->theme->state[0].bottom_margin;
	color = d->dp2 ? *(ALLEGRO_COLOR *)d->dp2 : t3f_color_black;
	float cx, cy;
	int grid_scale;
	int grid_offset;
	int brush_size;
	int i, j;

	switch(msg)
	{
		case MSG_MOUSEDOWN:
		{
			//d->id1 = 1;
			break;
		}
		case MSG_DRAW:
		{
			brush_size = al_get_bitmap_width(bp);
			if(al_get_bitmap_height(bp) > brush_size)
			{
				brush_size = al_get_bitmap_height(bp);
			}
			grid_scale = usable_width / (brush_size + 2);
			al_draw_filled_rectangle(d->x, d->y, d->x + d->w - 1, d->y + d->h - 1, t3f_color_white);
			cx = d->x + d->w / 2 - (al_get_bitmap_width(bp) * grid_scale) / 2;
			cy = d->y + d->h / 2 - (al_get_bitmap_height(bp) * grid_scale) / 2;
			if(grid_scale > cep->grid_thickness)
			{
				for(i = cx; i >= d->x; i -= grid_scale)
				{
					al_draw_line(i, d->y, i, d->y + d->h, cep->grid_color, cep->grid_thickness);
				}
				for(i = cx + grid_scale; i <= d->x + d->w; i += grid_scale)
				{
					al_draw_line(i, d->y, i, d->y + d->h, cep->grid_color, cep->grid_thickness);
				}
				for(i = cy; i >= d->y; i -= grid_scale)
				{
					al_draw_line(d->x, i, d->x + d->w, i, cep->grid_color, cep->grid_thickness);
				}
				for(i = cy + grid_scale; i <= d->y + d->h; i += grid_scale)
				{
					al_draw_line(d->x, i, d->x + d->w, i, cep->grid_color, cep->grid_thickness);
				}
			}
			if(grid_scale)
			{
				al_draw_tinted_scaled_bitmap(bp, color, 0, 0, al_get_bitmap_width(bp), al_get_bitmap_height(bp), cx, cy, al_get_bitmap_width(bp) * grid_scale, al_get_bitmap_height(bp) * grid_scale, 0);
			}
			else
			{
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
				al_draw_tinted_scaled_bitmap(bp, color, 0, 0, al_get_bitmap_width(bp), al_get_bitmap_height(bp), cx, cy, al_get_bitmap_width(bp) * scale, al_get_bitmap_height(bp) * scale, 0);
			}
			break;
		}
	}
	return D_O_K;
}
