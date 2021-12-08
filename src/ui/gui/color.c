#include "t3f/t3f.h"
#include "t3gui/t3gui.h"
#include "modules/color.h"
#include "modules/bitmap.h"
#include "instance.h"

static ALLEGRO_BITMAP * color_texture = NULL;
static ALLEGRO_BITMAP * color_background = NULL;
static T3F_ATLAS * color_atlas;

int pa_gui_color_proc(int msg, T3GUI_ELEMENT * d, int c)
{
	int * left_clicked = d->dp6;
	int * right_clicked = d->dp7;

	switch(msg)
	{
		case MSG_START:
		{
			if(!color_texture)
			{
				color_texture = pa_make_checkerboard_bitmap(t3f_color_white, t3f_color_white);
			}
			if(!color_background)
			{
				color_background = pa_make_checkerboard_bitmap(t3f_color_white, al_map_rgba_f(0.9, 0.9, 0.9, 1.0));
			}
			if(!color_atlas)
			{
				color_atlas = t3f_create_atlas(32, 32);
				if(color_atlas)
				{
					t3f_add_bitmap_to_atlas(color_atlas, &color_texture, T3F_ATLAS_TILE);
					t3f_add_bitmap_to_atlas(color_atlas, &color_background, T3F_ATLAS_TILE);
				}
			}
			break;
		}
		case MSG_MOUSEDOWN:
		{
			if(t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL] || t3f_key[ALLEGRO_KEY_COMMAND])
			{
				if(c == 1 && d->dp4)
				{
					*(ALLEGRO_COLOR *)d->dp = *(ALLEGRO_COLOR *)d->dp4;
				}
				else if(d->dp5)
				{
					*(ALLEGRO_COLOR *)d->dp = *(ALLEGRO_COLOR *)d->dp5;
				}
			}
			else
			{
				if(c == 1 && d->dp2)
				{
					*(ALLEGRO_COLOR *)d->dp2 = *(ALLEGRO_COLOR *)d->dp;
				}
				else if(d->dp3)
				{
					*(ALLEGRO_COLOR *)d->dp3 = *(ALLEGRO_COLOR *)d->dp;
				}
			}
			if(c == 1 && left_clicked)
			{
				*left_clicked = 1;
			}
			else if(c == 2 && right_clicked)
			{
				*right_clicked = 1;
			}
			break;
		}
		case MSG_DRAW:
		{
			al_draw_tinted_scaled_bitmap(color_background, t3f_color_white, 0, 0, al_get_bitmap_width(color_background), al_get_bitmap_height(color_background), d->x, d->y, d->w, d->h, 0);
			al_draw_tinted_scaled_bitmap(color_texture, *(ALLEGRO_COLOR *)d->dp, 0, 0, al_get_bitmap_width(color_background), al_get_bitmap_height(color_background), d->x, d->y, d->w, d->h, 0);
			break;
		}
	}
	return D_O_K;
}
